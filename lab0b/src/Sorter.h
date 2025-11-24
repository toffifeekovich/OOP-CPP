#ifndef SORTER_H
#define SORTER_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>
using Item = std::pair<std::string, int>;

class Sorter {
    std::vector<Item> pairs;
    
    static bool descending(const Item& a, const Item& b);

    static bool ascending(const Item& a, const Item& b);

public:
    explicit Sorter(const std::map<std::string, int>&refDict);
    void sort(const std::string& comp);
    const std::vector<Item>& getPairs() const;
};

#endif // SORTER_H
