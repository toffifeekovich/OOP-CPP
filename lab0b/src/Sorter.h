#ifndef SORTER_H
#define SORTER_H

#include <string>
#include <list>
#include <map>
#include <functional>

class Sorter {
    std::map<std::string, int> dict;
    int wordCount = 0;
    std::multimap<int, std::string, std::greater<int>> sortedDict;

public:
    explicit Sorter(const std::list<std::string>& text);

    const std::map<std::string, int>& getDict() const;
    const int& getWordCount() const;
    const std::multimap<int, std::string, std::greater<int>>& getSortedDict() const;
};

#endif // SORTER_H
