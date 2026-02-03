#include "PriorityQueue.hpp"

#include <ostream>
#include <stdexcept>
#include <utility> // std::move, std::swap

PriorityQueue::PriorityQueue(): data_(), max_heap_(true) {}

PriorityQueue::PriorityQueue(bool max_heap): data_(), max_heap_(max_heap) {}

PriorityQueue::PriorityQueue(const std::vector<value_type>& data, bool max_heap): data_(data), max_heap_(max_heap){
    if (!data_.empty()) {
        size_type last_parent = data_.size() / 2 - 1; 
        for (size_type i = last_parent + 1; i > 0; i--) {
            sift_down(i - 1);
        }
    }
}

PriorityQueue::~PriorityQueue() = default;

PriorityQueue::PriorityQueue(const PriorityQueue& other): data_(other.data_), max_heap_(other.max_heap_) {}

PriorityQueue::PriorityQueue(PriorityQueue&& other) noexcept: data_(std::move(other.data_)), max_heap_(other.max_heap_){}

PriorityQueue& PriorityQueue::operator=(const PriorityQueue& other){
    if (this != &other) {
        data_     = other.data_;
        max_heap_ = other.max_heap_;
    }
    return *this;
}

PriorityQueue& PriorityQueue::operator=(PriorityQueue&& other) noexcept{
    if (this != &other) {
        data_     = std::move(other.data_);
        max_heap_ = other.max_heap_;
    }
    return *this;
}

void PriorityQueue::push(value_type x){
    data_.push_back(x);
    sift_up(data_.size() - 1);
}

void PriorityQueue::pop(){
    if (data_.empty()) {
        throw std::out_of_range("PriorityQueue::pop on empty queue");
    }

    if (data_.size() == 1) {
        data_.pop_back();
        return;
    }

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

bool PriorityQueue::operator==(const PriorityQueue& rhs) const noexcept
{
    return max_heap_ == rhs.max_heap_ && data_ == rhs.data_;
}

bool PriorityQueue::operator!=(const PriorityQueue& rhs) const noexcept
{
    return !(*this == rhs);
}

bool PriorityQueue::compare(value_type a, value_type b) const noexcept
{
    return max_heap_ ? (a > b) : (a < b);
}

void PriorityQueue::sift_up(size_type idx)
{
    while (idx > 0) {
        size_type p = parent(idx);
        if (!compare(data_[idx], data_[p])) {
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
            break;
        }

        std::swap(data_[idx], data_[best]);
        idx = best;
    }
}

std::ostream& operator<<(std::ostream& os, const PriorityQueue& pq)
{
    os << "[mode=" << (pq.is_max_heap() ? "max" : "min")
       << ", size=" << pq.size() << "] ";

    for (PriorityQueue::value_type v : pq.data_) {
        os << v << ' ';
    }

    return os;
}
