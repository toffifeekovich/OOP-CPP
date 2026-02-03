#pragma once

#include <vector>
#include <cstddef>
#include <iosfwd> // forward-declaration для std::ostream

class PriorityQueue {
public:
    using value_type = int;
    using size_type  = std::size_t;

    PriorityQueue();
    explicit PriorityQueue(bool max_heap);
    explicit PriorityQueue(const std::vector<value_type>& data,
                           bool max_heap = true);
    ~PriorityQueue();

    PriorityQueue(const PriorityQueue& other);
    PriorityQueue(PriorityQueue&& other) noexcept;

    PriorityQueue& operator=(const PriorityQueue& other);
    PriorityQueue& operator=(PriorityQueue&& other) noexcept;

    void push(value_type x);
    void pop();
    const value_type& top() const;

    bool empty() const noexcept;
    size_type size() const noexcept;
    void clear() noexcept;
    void reserve(size_type n);

    bool is_max_heap() const noexcept;

    bool operator==(const PriorityQueue& rhs) const noexcept;
    bool operator!=(const PriorityQueue& rhs) const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const PriorityQueue& pq);

private:
    std::vector<value_type> data_;
    bool max_heap_{true};

    bool compare(value_type a, value_type b) const noexcept;
    void sift_up(size_type idx);
    void sift_down(size_type idx);

    static size_type parent(size_type idx) noexcept { return (idx - 1) / 2; }
    static size_type left(size_type idx)   noexcept { return 2 * idx + 1; }
    static size_type right(size_type idx)  noexcept { return 2 * idx + 2; }
};
