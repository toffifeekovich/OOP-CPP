#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <utility>
#include <new>
#include <type_traits>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include <immintrin.h> // SSE2

// Плоский hash-map с открытой адресацией и SIMD-поиском по control-байтам.
template <
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>
>
class flat_hash_map
{
public:
    using key_type        = Key;
    using mapped_type     = T;
    using value_type      = std::pair<const Key, T>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using hasher          = Hash;
    using key_equal       = KeyEqual;

    class iterator;
    class const_iterator;

    //=== Конструкторы / деструктор / присваивание =========================

    flat_hash_map()
        : buckets_(nullptr)
        , ctrl_(nullptr)
        , size_(0)
        , capacity_(0)
        , max_load_factor_(0.75f)
        , hasher_()
        , key_equal_()
    {
        init_table(kInitialCapacity);
    }

    explicit flat_hash_map(size_type bucket_count,
                           const Hash& hash = Hash(),
                           const KeyEqual& equal = KeyEqual())
        : buckets_(nullptr)
        , ctrl_(nullptr)
        , size_(0)
        , capacity_(0)
        , max_load_factor_(0.75f)
        , hasher_(hash)
        , key_equal_(equal)
    {
        size_type cap = normalize_capacity(bucket_count);
        init_table(cap);
    }

    template <class InputIt>
    flat_hash_map(InputIt first, InputIt last,
                  size_type bucket_count = 0,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
        : buckets_(nullptr)
        , ctrl_(nullptr)
        , size_(0)
        , capacity_(0)
        , max_load_factor_(0.75f)
        , hasher_(hash)
        , key_equal_(equal)
    {
        size_type cap = normalize_capacity(bucket_count);
        init_table(cap);
        insert(first, last);
    }

    flat_hash_map(std::initializer_list<value_type> init,
                  size_type bucket_count = 0,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
        : buckets_(nullptr)
        , ctrl_(nullptr)
        , size_(0)
        , capacity_(0)
        , max_load_factor_(0.75f)
        , hasher_(hash)
        , key_equal_(equal)
    {
        size_type cap = normalize_capacity(std::max(bucket_count, init.size()));
        init_table(cap);
        insert(init.begin(), init.end());
    }

    flat_hash_map(const flat_hash_map& other)
        : buckets_(nullptr)
        , ctrl_(nullptr)
        , size_(0)
        , capacity_(0)
        , max_load_factor_(other.max_load_factor_)
        , hasher_(other.hasher_)
        , key_equal_(other.key_equal_)
    {
        init_table(other.capacity_);
        for (size_type i = 0; i < other.capacity_; ++i) {
            if (other.is_full(other.ctrl_[i])) {
                const value_type* val = other.buckets_[i].value_ptr();
                insert(*val);
            }
        }
    }

    flat_hash_map(flat_hash_map&& other) noexcept
        : buckets_(other.buckets_)
        , ctrl_(other.ctrl_)
        , size_(other.size_)
        , capacity_(other.capacity_)
        , max_load_factor_(other.max_load_factor_)
        , hasher_(std::move(other.hasher_))
        , key_equal_(std::move(other.key_equal_))
    {
        other.buckets_  = nullptr;
        other.ctrl_     = nullptr;
        other.size_     = 0;
        other.capacity_ = 0;
    }

    ~flat_hash_map() {
        destroy_all();
        deallocate();
    }

    flat_hash_map& operator=(const flat_hash_map& other) {
        if (this == &other) {
            return *this;
        }
        flat_hash_map tmp(other);
        swap(tmp);
        return *this;
    }

    flat_hash_map& operator=(flat_hash_map&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        destroy_all();
        deallocate();

        buckets_        = other.buckets_;
        ctrl_           = other.ctrl_;
        size_           = other.size_;
        capacity_       = other.capacity_;
        max_load_factor_= other.max_load_factor_;
        hasher_         = std::move(other.hasher_);
        key_equal_      = std::move(other.key_equal_);

        other.buckets_  = nullptr;
        other.ctrl_     = nullptr;
        other.size_     = 0;
        other.capacity_ = 0;

        return *this;
    }

    flat_hash_map& operator=(std::initializer_list<value_type> init) {
        clear();
        reserve(init.size());
        insert(init.begin(), init.end());
        return *this;
    }

    // Обмен содержимого
    void swap(flat_hash_map& other) noexcept {
        using std::swap;
        swap(buckets_, other.buckets_);
        swap(ctrl_, other.ctrl_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
        swap(max_load_factor_, other.max_load_factor_);
        swap(hasher_, other.hasher_);
        swap(key_equal_, other.key_equal_);
    }

    //=== Итераторы =========================================================

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = flat_hash_map::value_type;
        using difference_type   = flat_hash_map::difference_type;
        using pointer           = value_type*;
        using reference         = value_type&;

        iterator() noexcept : map_(nullptr), index_(0) {}

        reference operator*() const {
            return *map_->buckets_[index_].value_ptr();
        }

        pointer operator->() const {
            return map_->buckets_[index_].value_ptr();
        }

        iterator& operator++() {
            advance();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(*this);
            advance();
            return tmp;
        }

        friend bool operator==(const iterator& a, const iterator& b) noexcept {
            return a.map_ == b.map_ && a.index_ == b.index_;
        }

        friend bool operator!=(const iterator& a, const iterator& b) noexcept {
            return !(a == b);
        }

    private:
        friend class flat_hash_map;

        flat_hash_map* map_;
        size_type      index_;

        iterator(flat_hash_map* map, size_type idx) noexcept
            : map_(map), index_(idx) {}

        void advance() {
            if (!map_) return;
            ++index_;
            while (index_ < map_->capacity_ &&
                   !map_->is_full(map_->ctrl_[index_])) {
                ++index_;
            }
        }
    };

    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = flat_hash_map::value_type;
        using difference_type   = flat_hash_map::difference_type;
        using pointer           = const value_type*;
        using reference         = const value_type&;

        const_iterator() noexcept : map_(nullptr), index_(0) {}

        const_iterator(const iterator& it) noexcept
            : map_(it.map_), index_(it.index_) {}

        reference operator*() const {
            return *map_->buckets_[index_].value_ptr();
        }

        pointer operator->() const {
            return map_->buckets_[index_].value_ptr();
        }

        const_iterator& operator++() {
            advance();
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp(*this);
            advance();
            return tmp;
        }

        friend bool operator==(const const_iterator& a, const const_iterator& b) noexcept {
            return a.map_ == b.map_ && a.index_ == b.index_;
        }

        friend bool operator!=(const const_iterator& a, const const_iterator& b) noexcept {
            return !(a == b);
        }

    private:
        friend class flat_hash_map;

        const flat_hash_map* map_;
        size_type            index_;

        const_iterator(const flat_hash_map* map, size_type idx) noexcept
            : map_(map), index_(idx) {}

        void advance() {
            if (!map_) return;
            ++index_;
            while (index_ < map_->capacity_ &&
                   !map_->is_full(map_->ctrl_[index_])) {
                ++index_;
            }
        }
    };

    iterator begin() noexcept {
        return iterator(this, first_full_index());
    }

    const_iterator begin() const noexcept {
        return const_iterator(this, first_full_index());
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    iterator end() noexcept {
        return iterator(this, capacity_);
    }

    const_iterator end() const noexcept {
        return const_iterator(this, capacity_);
    }

    const_iterator cend() const noexcept {
        return end();
    }

    //=== Размер / состояние ================================================

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type capacity() const noexcept {
        return capacity_;
    }

    float load_factor() const noexcept {
        return capacity_ == 0 ? 0.0f : static_cast<float>(size_) / static_cast<float>(capacity_);
    }

    void max_load_factor(float ml) {
        if (ml <= 0.0f) {
            ml = 0.25f;
        }
        max_load_factor_ = ml;
        if (size_ > static_cast<size_type>(max_load_factor_ * capacity_)) {
            rehash(capacity_ * 2);
        }
    }

    float max_load_factor() const noexcept {
        return max_load_factor_;
    }

    //=== Очистка и управление ёмкостью ====================================

    void clear() noexcept {
        if (!buckets_) return;
        for (size_type i = 0; i < capacity_; ++i) {
            if (is_full(ctrl_[i])) {
                buckets_[i].destroy();
            }
            ctrl_[i] = kEmpty;
        }
        size_ = 0;
    }

    void reserve(size_type new_capacity) {
        // гарантируем возможность хранить new_capacity элементов при текущем max_load_factor
        size_type required_buckets =
            normalize_capacity(static_cast<size_type>(new_capacity / max_load_factor_) + 1);
        if (required_buckets > capacity_) {
            rehash(required_buckets);
        }
    }

    //=== Вставка ===========================================================

    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_impl(value);
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_impl(std::move(value));
    }

    template <class InputIt>
    void insert(InputIt first, InputIt last) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    void insert(std::initializer_list<value_type> init) {
        insert(init.begin(), init.end());
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        value_type v(std::forward<Args>(args)...);
        return insert(std::move(v));
    }

    //=== Удаление ==========================================================

    size_type erase(const key_type& key) {
        size_type idx = find_index(key);
        if (idx == npos()) {
            return 0;
        }
        erase_at_index(idx);
        return 1;
    }

    iterator erase(iterator pos) {
        if (pos == end()) {
            return pos;
        }
        size_type idx = pos.index_;
        erase_at_index(idx);
        // вернуть итератор на следующий элемент
        iterator it(this, idx);
        if (idx >= capacity_) {
            return end();
        }
        if (!is_full(ctrl_[idx])) {
            ++it;
        }
        return it;
    }

    //=== Доступ по ключу ===================================================

    mapped_type& operator[](const key_type& key) {
        return subscript_impl(key);
    }

    mapped_type& operator[](key_type&& key) {
        return subscript_impl(std::move(key));
    }

    mapped_type& at(const key_type& key) {
        size_type idx = find_index(key);
        if (idx == npos()) {
            throw std::out_of_range("flat_hash_map::at: key not found");
        }
        return buckets_[idx].value_ptr()->second;
    }

    const mapped_type& at(const key_type& key) const {
        size_type idx = find_index(key);
        if (idx == npos()) {
            throw std::out_of_range("flat_hash_map::at: key not found");
        }
        return buckets_[idx].value_ptr()->second;
    }

    //=== Поиск / наличие ===================================================

    iterator find(const key_type& key) {
        size_type idx = find_index(key);
        if (idx == npos()) {
            return end();
        }
        return iterator(this, idx);
    }

    const_iterator find(const key_type& key) const {
        size_type idx = find_index(key);
        if (idx == npos()) {
            return end();
        }
        return const_iterator(this, idx);
    }

    bool contains(const key_type& key) const {
        return find_index(key) != npos();
    }

    size_type count(const key_type& key) const {
        return contains(key) ? 1u : 0u;
    }

private:
    //=== Внутренние структуры и константы ==================================

    static constexpr int8_t kEmpty   = static_cast<int8_t>(-128); // 0x80
    static constexpr int8_t kDeleted = static_cast<int8_t>(-2);   // 0xFE
    static constexpr size_type kInitialCapacity = 16;
    static constexpr int kGroupWidth = 16;

    struct Bucket {
        alignas(value_type) unsigned char storage[sizeof(value_type)];

        value_type* value_ptr() noexcept {
            return std::launder(reinterpret_cast<value_type*>(storage));
        }

        const value_type* value_ptr() const noexcept {
            return std::launder(reinterpret_cast<const value_type*>(storage));
        }

        template <class V>
        void construct(V&& v) {
            ::new (static_cast<void*>(storage)) value_type(std::forward<V>(v));
        }

        template <class... Args>
        void construct_emplace(Args&&... args) {
            ::new (static_cast<void*>(storage)) value_type(std::forward<Args>(args)...);
        }

        void destroy() noexcept {
            value_ptr()->~value_type();
        }
    };

    Bucket*   buckets_;
    int8_t*   ctrl_;
    size_type size_;
    size_type capacity_;
    float     max_load_factor_;
    Hash      hasher_;
    KeyEqual  key_equal_;

    //=== Вспомогательные методы для control-байтов =========================

    static bool is_empty(int8_t c) noexcept {
        return c == kEmpty;
    }

    static bool is_deleted(int8_t c) noexcept {
        return c == kDeleted;
    }

    static bool is_full(int8_t c) noexcept {
        return c >= 0;
    }

    static size_type npos() noexcept {
        return static_cast<size_type>(-1);
    }

    static uint8_t hash_tag(std::size_t h) noexcept {
        // 7 младших битов хеша
        return static_cast<uint8_t>(h & 0x7F);
    }

    static size_type normalize_capacity(size_type requested) {
        size_type cap = kInitialCapacity;
        while (cap < requested) {
            cap <<= 1;
        }
        return cap;
    }

    void init_table(size_type bucket_count) {
        capacity_ = bucket_count ? bucket_count : kInitialCapacity;
        buckets_  = static_cast<Bucket*>(::operator new[](capacity_ * sizeof(Bucket)));
        ctrl_     = static_cast<int8_t*>(::operator new[](capacity_ * sizeof(int8_t)));
        std::fill(ctrl_, ctrl_ + capacity_, kEmpty);
        size_ = 0;
    }

    void destroy_all() noexcept {
        if (!buckets_) return;
        for (size_type i = 0; i < capacity_; ++i) {
            if (is_full(ctrl_[i])) {
                buckets_[i].destroy();
            }
        }
        size_ = 0;
    }

    void deallocate() noexcept {
        ::operator delete[](buckets_);
        ::operator delete[](ctrl_);
        buckets_  = nullptr;
        ctrl_     = nullptr;
        capacity_ = 0;
    }

    size_type first_full_index() const noexcept {
        for (size_type i = 0; i < capacity_; ++i) {
            if (is_full(ctrl_[i])) {
                return i;
            }
        }
        return capacity_;
    }

    //=== Работа с битовыми масками (для SIMD) ==============================

    static int lowest_bit_index(unsigned mask) noexcept {
        for (int i = 0; i < kGroupWidth; ++i) {
            if (mask & (1u << i)) {
                return i;
            }
        }
        return -1;
    }

    //=== Поиск индекса элемента по ключу (с SIMD) ==========================

    size_type find_index(const key_type& key) const {
        if (size_ == 0 || capacity_ == 0) {
            return npos();
        }

        std::size_t h = hasher_(key);
        uint8_t tag   = hash_tag(h);
        size_type idx = static_cast<size_type>(h & (capacity_ - 1));
        size_type probed = 0;

        const __m128i tag_vec    = _mm_set1_epi8(static_cast<char>(tag));
        const __m128i empty_vec  = _mm_set1_epi8(static_cast<char>(kEmpty));

        while (probed < capacity_) {
            if (idx + kGroupWidth <= capacity_) {
                // SIMD-блок [idx, idx+15]
                const __m128i ctrl_vec = _mm_loadu_si128(
                    reinterpret_cast<const __m128i*>(ctrl_ + idx));

                const __m128i eq_tag   = _mm_cmpeq_epi8(ctrl_vec, tag_vec);
                const __m128i eq_empty = _mm_cmpeq_epi8(ctrl_vec, empty_vec);

                unsigned mask_tag   = static_cast<unsigned>(_mm_movemask_epi8(eq_tag));
                unsigned mask_empty = static_cast<unsigned>(_mm_movemask_epi8(eq_empty));

                // Проверка кандидатов по тегу
                while (mask_tag) {
                    int bit = lowest_bit_index(mask_tag);
                    size_type candidate = idx + static_cast<size_type>(bit);
                    if (candidate >= capacity_) break; // защита, но по идее не должно
                    const value_type* val = buckets_[candidate].value_ptr();
                    if (key_equal_(val->first, key)) {
                        return candidate;
                    }
                    mask_tag &= ~(1u << bit);
                }

                // Пустая ячейка — элемент не найден
                if (mask_empty) {
                    return npos();
                }

                idx += kGroupWidth;
                probed += kGroupWidth;
                if (idx == capacity_) {
                    idx = 0;
                }
            } else {
                // Хвост — скалярно до конца массива
                for (; idx < capacity_ && probed < capacity_; ++idx, ++probed) {
                    int8_t c = ctrl_[idx];
                    if (is_empty(c)) {
                        return npos();
                    }
                    if (is_full(c) && static_cast<uint8_t>(c) == tag) {
                        const value_type* val = buckets_[idx].value_ptr();
                        if (key_equal_(val->first, key)) {
                            return idx;
                        }
                    }
                }
                if (idx == capacity_) {
                    idx = 0;
                }
            }
        }

        return npos();
    }

    struct InsertPos {
        size_type index;
        bool      found;
    };

    InsertPos find_insert_position(const key_type& key, std::size_t h) {
        uint8_t tag = hash_tag(h);
        size_type idx = static_cast<size_type>(h & (capacity_ - 1));
        size_type probed = 0;
        size_type first_deleted = npos();

        const __m128i tag_vec     = _mm_set1_epi8(static_cast<char>(tag));
        const __m128i empty_vec   = _mm_set1_epi8(static_cast<char>(kEmpty));
        const __m128i deleted_vec = _mm_set1_epi8(static_cast<char>(kDeleted));

        while (probed < capacity_) {
            if (idx + kGroupWidth <= capacity_) {
                const __m128i ctrl_vec = _mm_loadu_si128(
                    reinterpret_cast<const __m128i*>(ctrl_ + idx));

                const __m128i eq_tag     = _mm_cmpeq_epi8(ctrl_vec, tag_vec);
                const __m128i eq_empty   = _mm_cmpeq_epi8(ctrl_vec, empty_vec);
                const __m128i eq_deleted = _mm_cmpeq_epi8(ctrl_vec, deleted_vec);

                unsigned mask_tag     = static_cast<unsigned>(_mm_movemask_epi8(eq_tag));
                unsigned mask_empty   = static_cast<unsigned>(_mm_movemask_epi8(eq_empty));
                unsigned mask_deleted = static_cast<unsigned>(_mm_movemask_epi8(eq_deleted));

                // Сначала ищем уже существующий ключ
                while (mask_tag) {
                    int bit = lowest_bit_index(mask_tag);
                    size_type candidate = idx + static_cast<size_type>(bit);
                    if (candidate >= capacity_) break;
                    const value_type* val = buckets_[candidate].value_ptr();
                    if (key_equal_(val->first, key)) {
                        return {candidate, true};
                    }
                    mask_tag &= ~(1u << bit);
                }

                // Запоминаем первый DELETED
                if (mask_deleted && first_deleted == npos()) {
                    int bit = lowest_bit_index(mask_deleted);
                    first_deleted = idx + static_cast<size_type>(bit);
                }

                // Если есть пустая ячейка — сюда можно вставлять
                if (mask_empty) {
                    int bit = lowest_bit_index(mask_empty);
                    size_type empty_index = idx + static_cast<size_type>(bit);
                    size_type insert_index =
                        (first_deleted != npos()) ? first_deleted : empty_index;
                    return {insert_index, false};
                }

                idx += kGroupWidth;
                probed += kGroupWidth;
                if (idx == capacity_) {
                    idx = 0;
                }
            } else {
                // Хвост — скалярно
                for (; idx < capacity_ && probed < capacity_; ++idx, ++probed) {
                    int8_t c = ctrl_[idx];
                    if (is_empty(c)) {
                        size_type insert_index =
                            (first_deleted != npos()) ? first_deleted : idx;
                        return {insert_index, false};
                    }
                    if (is_deleted(c) && first_deleted == npos()) {
                        first_deleted = idx;
                    }
                    if (is_full(c) && static_cast<uint8_t>(c) == tag) {
                        const value_type* val = buckets_[idx].value_ptr();
                        if (key_equal_(val->first, key)) {
                            return {idx, true};
                        }
                    }
                }
                if (idx == capacity_) {
                    idx = 0;
                }
            }
        }

        // Таблица полностью заполнена (не должны сюда доходить при корректном rehash)
        return { first_deleted != npos() ? first_deleted : 0, false };
    }

    //=== Реализация insert через общий шаблон ==============================

    template <class V>
    std::pair<iterator, bool> insert_impl(V&& value) {
        maybe_rehash_before_insert();

        const key_type& key = value.first;
        std::size_t h = hasher_(key);

        InsertPos pos = find_insert_position(key, h);
        if (pos.found) {
            return { iterator(this, pos.index), false };
        }

        // Новая вставка
        buckets_[pos.index].construct(std::forward<V>(value));
        ctrl_[pos.index] = static_cast<int8_t>(hash_tag(h));
        ++size_;

        return { iterator(this, pos.index), true };
    }

    void maybe_rehash_before_insert() {
        if (capacity_ == 0) {
            init_table(kInitialCapacity);
            return;
        }
        float lf = load_factor();
        if (lf > max_load_factor_) {
            rehash(capacity_ * 2);
        }
    }

    //=== Реализация erase по индексу ======================================

    void erase_at_index(size_type idx) {
        if (!is_full(ctrl_[idx])) {
            return;
        }
        buckets_[idx].destroy();
        ctrl_[idx] = kDeleted;
        --size_;
    }

    //=== Реализация operator[] через общий шаблон ==========================

    template <class K>
    mapped_type& subscript_impl(K&& key) {
        size_type idx = find_index(key);
        if (idx != npos()) {
            return buckets_[idx].value_ptr()->second;
        }

        // Вставка значения по умолчанию
        maybe_rehash_before_insert();

        std::size_t h = hasher_(key);
        InsertPos pos = find_insert_position(key, h);
        if (!is_full(ctrl_[pos.index])) {
            buckets_[pos.index].construct(
                value_type(std::forward<K>(key), mapped_type{}));
            ctrl_[pos.index] = static_cast<int8_t>(hash_tag(h));
            ++size_;
        }
        return buckets_[pos.index].value_ptr()->second;
    }

    //=== Перераспределение (rehash) =======================================

    void rehash(size_type new_bucket_count) {
        size_type new_cap = normalize_capacity(new_bucket_count);
        flat_hash_map tmp(new_cap, hasher_, key_equal_);
        tmp.max_load_factor_ = max_load_factor_;

        for (size_type i = 0; i < capacity_; ++i) {
            if (is_full(ctrl_[i])) {
                value_type* val = buckets_[i].value_ptr();
                tmp.insert(std::move(*val));
            }
        }

        swap(tmp);
    }
};

#endif // FLAT_HASH_MAP_HPP
