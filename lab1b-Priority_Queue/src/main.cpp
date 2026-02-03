#include <iostream>
#include "PriorityQueue.hpp"

int main() {
    try {
        std::cout << "=== PriorityQueue Demo ===\n\n";

        PriorityQueue pq;
        pq.push(10);
        pq.push(3);
        pq.push(7);
        pq.push(20);
        pq.push(15);

        std::cout << "Max-heap contents: " << pq << "\n";
        std::cout << "Top (max): " << pq.top() << "\n";

        pq.pop();
        std::cout << "After pop(): " << pq << "\n";
        std::cout << "New top: " << pq.top() << "\n\n";

        PriorityQueue minq(false);
        minq.push(10);
        minq.push(3);
        minq.push(7);
        minq.push(20);
        minq.push(15);

        std::cout << "Min-heap contents: " << minq << "\n";
        std::cout << "Top (min): " << minq.top() << "\n";

        minq.pop();
        std::cout << "After pop(): " << minq << "\n\n";

        std::vector<int> data = {5, 1, 9, 3, 7};
        PriorityQueue fromVec(data, true); 

        std::cout << "Heap from vector: " << fromVec << "\n";
        std::cout << "Top: " << fromVec.top() << "\n\n";

        PriorityQueue pq2 = pq;  
        std::cout << "pq == pq2 ? " << (pq == pq2 ? "YES" : "NO") << "\n";

        pq2.push(100);
        std::cout << "After modifying pq2, pq == pq2 ? "
                  << (pq == pq2 ? "YES" : "NO") << "\n\n";

        pq2.clear();
        std::cout << "After clear(), pq2 empty? "
                  << (pq2.empty() ? "YES" : "NO") << "\n";

        std::cout << "\nTesting exception on empty queue...\n";
        pq2.pop(); 

    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << "\n";
    }

    return 0;
}
