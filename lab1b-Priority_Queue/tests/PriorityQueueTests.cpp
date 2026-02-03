#include <gtest/gtest.h>
#include "PriorityQueue.hpp"

static std::vector<int> drain_queue(PriorityQueue& pq) {
    std::vector<int> result;
    result.reserve(pq.size());
    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }
    return result;
}

TEST(PriorityQueueBasic, EmptyQueueProperties) {
    PriorityQueue pq;

    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);

    EXPECT_THROW(pq.top(), std::out_of_range);
    EXPECT_THROW(pq.pop(), std::out_of_range);
}

TEST(PriorityQueueMaxHeap, PushPopOrder) {
    PriorityQueue pq(true);

    pq.push(10);
    pq.push(3);
    pq.push(7);
    pq.push(20);
    pq.push(15);

    EXPECT_FALSE(pq.empty());
    EXPECT_EQ(pq.size(), 5u);
    EXPECT_EQ(pq.top(), 20);

    std::vector<int> drained = drain_queue(pq);
    std::vector<int> expected = {20, 15, 10, 7, 3};

    EXPECT_EQ(drained, expected);
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);
}

TEST(PriorityQueueMinHeap, PushPopOrder) {
    PriorityQueue pq(false);

    pq.push(10);
    pq.push(3);
    pq.push(7);
    pq.push(20);
    pq.push(15);

    EXPECT_EQ(pq.top(), 3);

    std::vector<int> drained = drain_queue(pq);
    std::vector<int> expected = {3, 7, 10, 15, 20};

    EXPECT_EQ(drained, expected);
    EXPECT_TRUE(pq.empty());
}

TEST(PriorityQueueConstructors, FromVectorMaxHeap) {
    std::vector<int> data = {5, 1, 9, 3, 7};
    PriorityQueue pq(data, true);

    EXPECT_EQ(pq.size(), data.size());
    EXPECT_EQ(pq.top(), 9);

    std::vector<int> drained = drain_queue(pq);
    std::vector<int> expected = {9, 7, 5, 3, 1};
    EXPECT_EQ(drained, expected);
}

TEST(PriorityQueueConstructors, FromVectorMinHeap) {
    std::vector<int> data = {5, 1, 9, 3, 7};
    PriorityQueue pq(data, false);

    EXPECT_EQ(pq.size(), data.size());
    EXPECT_EQ(pq.top(), 1);

    std::vector<int> drained = drain_queue(pq);
    std::vector<int> expected = {1, 3, 5, 7, 9};
    EXPECT_EQ(drained, expected);
}

TEST(PriorityQueueCopyMove, CopyConstructorAndAssignment) {
    PriorityQueue original(true);
    original.push(1);
    original.push(5);
    original.push(3);

    PriorityQueue copy(original); 
    EXPECT_EQ(copy.size(), original.size());
    EXPECT_EQ(copy.top(), original.top());
    EXPECT_EQ(copy, original);

    copy.push(10);
    EXPECT_NE(copy, original);

    PriorityQueue assigned(false);
    assigned = original;

    EXPECT_EQ(assigned.size(), original.size());
    EXPECT_EQ(assigned.top(), original.top());
    EXPECT_TRUE(assigned.is_max_heap());
    EXPECT_EQ(assigned, original);
}

TEST(PriorityQueueCopyMove, MoveConstructorAndAssignment) {
    PriorityQueue src(true);
    src.push(2);
    src.push(8);
    src.push(4);

    PriorityQueue moved(std::move(src));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_EQ(moved.top(), 8);

    EXPECT_NO_THROW(src.size());
    EXPECT_NO_THROW(src.empty());

    PriorityQueue another(false);
    another.push(100);
    another.push(200);

    another = std::move(moved);
    EXPECT_TRUE(another.is_max_heap());
    EXPECT_EQ(another.size(), 3u);
    EXPECT_EQ(another.top(), 8);
}

TEST(PriorityQueueModifiers, ClearAndEmptyAndSize) {
    PriorityQueue pq;
    pq.push(1);
    pq.push(2);
    pq.push(3);

    EXPECT_FALSE(pq.empty());
    EXPECT_EQ(pq.size(), 3u);

    pq.clear();
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);

    EXPECT_THROW(pq.top(), std::out_of_range);
}

TEST(PriorityQueueModifiers, ReserveDoesNotBreakHeap) {
    PriorityQueue pq(true);
    pq.reserve(100);

    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);

    pq.push(10);
    pq.push(5);
    pq.push(20);

    EXPECT_EQ(pq.top(), 20);
    std::vector<int> drained = drain_queue(pq);
    std::vector<int> expected = {20, 10, 5};
    EXPECT_EQ(drained, expected);
}

TEST(PriorityQueueProperties, IsMaxHeapFlag) {
    PriorityQueue max_default;
    PriorityQueue max_explicit(true);
    PriorityQueue min_explicit(false);

    EXPECT_TRUE(max_default.is_max_heap());
    EXPECT_TRUE(max_explicit.is_max_heap());
    EXPECT_FALSE(min_explicit.is_max_heap());
}

TEST(PriorityQueueComparison, EqualityAndInequality) {
    PriorityQueue a(true);
    PriorityQueue b(true);

    a.push(3);
    a.push(1);
    a.push(2);

    b.push(3);
    b.push(1);
    b.push(2);

    EXPECT_EQ(a, b);
    EXPECT_FALSE(a != b);

    b.push(100);
    EXPECT_NE(a, b);

    PriorityQueue c(false);
    c.push(3);
    c.push(1);
    c.push(2);

    EXPECT_NE(a, c);
}

TEST(PriorityQueueExceptions, TopAndPopThrowOnEmpty) {
    PriorityQueue pq;

    EXPECT_THROW(pq.top(), std::out_of_range);
    EXPECT_THROW(pq.pop(), std::out_of_range);

    pq.push(42);
    EXPECT_NO_THROW(pq.top());
    EXPECT_NO_THROW(pq.pop());
    EXPECT_THROW(pq.top(), std::out_of_range);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
