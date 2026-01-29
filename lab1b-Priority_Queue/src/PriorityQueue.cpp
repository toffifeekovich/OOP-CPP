#include "PriorityQueue.hpp"

#include <ostream>
#include <stdexcept>
#include <utility> // std::move, std::swap

// --- Конструкторы / деструктор ---

PriorityQueue::PriorityQueue()
    : data_()
    , max_heap_(true)
{
}

PriorityQueue::PriorityQueue(bool max_heap)
    : data_()
    , max_heap_(max_heap)
{
}

PriorityQueue::PriorityQueue(const std::vector<value_type>& data, bool max_heap)
    : data_(data)
    , max_heap_(max_heap)
{
    // Построение кучи из произвольного вектора (heapify)
    if (!data_.empty()) {
        // Идём с последнего внутреннего узла к корню
        for (size_type i = data_.size() / 2; i-- > 0; ) {
            sift_down(i);
        }
    }
}

PriorityQueue::~PriorityQueue() = default;

PriorityQueue::PriorityQueue(const PriorityQueue& other)
    : data_(other.data_)
    , max_heap_(other.max_heap_)
{
}

PriorityQueue::PriorityQueue(PriorityQueue&& other) noexcept
    : data_(std::move(other.data_))
    , max_heap_(other.max_heap_)
{
    // other остаётся в валидном, но не определённом с точки зрения содержимого состоянии
}

PriorityQueue& PriorityQueue::operator=(const PriorityQueue& other)
{
    if (this != &other) {
        data_     = other.data_;
        max_heap_ = other.max_heap_;
    }
    return *this;
}

PriorityQueue& PriorityQueue::operator=(PriorityQueue&& other) noexcept
{
    if (this != &other) {
        data_     = std::move(other.data_);
        max_heap_ = other.max_heap_;
    }
    return *this;
}

// --- Основные методы ---

void PriorityQueue::push(value_type x)
{
    data_.push_back(x);
    sift_up(data_.size() - 1);
}

void PriorityQueue::pop()
{
    if (data_.empty()) {
        throw std::out_of_range("PriorityQueue::pop on empty queue");
    }

    if (data_.size() == 1) {
        data_.pop_back();
        return;
    }

    // Помещаем последний элемент в корень и восстанавливаем кучу
    std::swap(data_.front(), data_.back());
    data_.pop_back();
    sift_down(0);
}

const PriorityQueue::value_type& PriorityQueue::top() const
{
    if (data_.empty()) {
        throw std::out_of_range("PriorityQueue::top on empty queue");
    }
    return data_.front();
}

bool PriorityQueue::empty() const noexcept
{
    return data_.empty();
}

PriorityQueue::size_type PriorityQueue::size() const noexcept
{
    return data_.size();
}

void PriorityQueue::clear() noexcept
{
    data_.clear();
}

void PriorityQueue::reserve(size_type n)
{
    data_.reserve(n);
}

bool PriorityQueue::is_max_heap() const noexcept
{
    return max_heap_;
}

// --- Операторы сравнения ---

bool PriorityQueue::operator==(const PriorityQueue& rhs) const noexcept
{
    return max_heap_ == rhs.max_heap_ && data_ == rhs.data_;
}

bool PriorityQueue::operator!=(const PriorityQueue& rhs) const noexcept
{
    return !(*this == rhs);
}

// --- Вспомогательные методы ---

bool PriorityQueue::compare(value_type a, value_type b) const noexcept
{
    // Для max-heap "лучше" тот, у кого значение больше,
    // для min-heap — наоборот
    return max_heap_ ? (a > b) : (a < b);
}

void PriorityQueue::sift_up(size_type idx)
{
    while (idx > 0) {
        size_type p = parent(idx);
        if (!compare(data_[idx], data_[p])) {
            // Инвариант кучи не нарушен
            break;
        }
        std::swap(data_[idx], data_[p]);
        idx = p;
    }
}

void PriorityQueue::sift_down(size_type idx)
{
    const size_type n = data_.size();

    while (true) {
        size_type l = left(idx);
        size_type r = right(idx);
        size_type best = idx;

        if (l < n && compare(data_[l], data_[best])) {
            best = l;
        }
        if (r < n && compare(data_[r], data_[best])) {
            best = r;
        }

        if (best == idx) {
            // Уже куча
            break;
        }

        std::swap(data_[idx], data_[best]);
        idx = best;
    }
}

// --- Оператор вывода ---

std::ostream& operator<<(std::ostream& os, const PriorityQueue& pq)
{
    os << "[mode=" << (pq.is_max_heap() ? "max" : "min")
       << ", size=" << pq.size() << "] ";

    for (PriorityQueue::value_type v : pq.data_) {
        os << v << ' ';
    }

    return os;
}
