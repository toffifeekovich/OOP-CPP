#pragma once

#include <vector>
#include <cstddef>
#include <iosfwd> // forward-declaration для std::ostream

class PriorityQueue {
public:
    using value_type = int;
    using size_type  = std::size_t;

    // --- Конструкторы / деструктор ---
    PriorityQueue();                               // max-heap по умолчанию
    explicit PriorityQueue(bool max_heap);         // true = max-heap, false = min-heap
    explicit PriorityQueue(const std::vector<value_type>& data,
                           bool max_heap = true);
    ~PriorityQueue();

    PriorityQueue(const PriorityQueue& other);             // Конструктор копирования
    PriorityQueue(PriorityQueue&& other) noexcept;         // Конструктор перемещения

    PriorityQueue& operator=(const PriorityQueue& other);        // Присваивание копированием
    PriorityQueue& operator=(PriorityQueue&& other) noexcept;    // Присваивание перемещением

    // --- Основные методы ---
    void push(value_type x);        // Добавить элемент
    void pop();                     // Удалить элемент с наивысшим приоритетом
    const value_type& top() const;  // Получить элемент с наивысшим приоритетом

    bool empty() const noexcept;     // Проверка на пустоту
    size_type size() const noexcept; // Количество элементов
    void clear() noexcept;           // Очистить очередь
    void reserve(size_type n);       // Зарезервировать память под кучу

    bool is_max_heap() const noexcept; // Проверить режим работы (min/max куча)

    // --- Операторы ---
    bool operator==(const PriorityQueue& rhs) const noexcept;
    bool operator!=(const PriorityQueue& rhs) const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const PriorityQueue& pq);

private:
    std::vector<value_type> data_;
    bool max_heap_{true}; // true = max-heap, false = min-heap

    // Вспомогательные функции кучи
    bool compare(value_type a, value_type b) const noexcept;
    void sift_up(size_type idx);
    void sift_down(size_type idx);

    // Индексы родителя и детей в бинарной куче
    static size_type parent(size_type idx) noexcept { return (idx - 1) / 2; }
    static size_type left(size_type idx)   noexcept { return 2 * idx + 1; }
    static size_type right(size_type idx)  noexcept { return 2 * idx + 2; }
};
