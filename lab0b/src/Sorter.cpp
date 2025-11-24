#include "Sorter.h"

Sorter::Sorter(const std::map<std::string, int>&refDict) {
    pairs.assign(refDict.begin(), refDict.end());
}


bool Sorter::descending(const Item& a, const Item& b) {
if (a.second != b.second)
    return a.second > b.second;
return a.first < b.first; 
}

bool Sorter::ascending(const Item& a,
const Item& b) {
if (a.second != b.second)
    return a.second < b.second;
return a.first < b.first;
}

void Sorter::sort(const std::string& comp) {
    if (comp == "<"){
        std::sort(pairs.begin(), pairs.end(), ascending);
    }
    else if (comp == ">"){
        std::sort(pairs.begin(), pairs.end(), descending);
    }
    else {
        throw std::runtime_error("Invalid sort comparator: " + comp);
    }
}

const std::vector<Item>& Sorter::getPairs() const{
    return pairs;
}