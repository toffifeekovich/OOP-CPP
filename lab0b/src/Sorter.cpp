#include "Sorter.h"

Sorter::Sorter(const std::list<std::string>& text) {
    wordCount = text.size();
    for (const std::string &str : text) {
        dict[str] += 1;
    }
    for (const auto& [key, value] : dict) {
        sortedDict.insert({value, key});
    }
}

const std::map<std::string, int>& Sorter::getDict() const {
    return dict;
}

const int& Sorter::getWordCount() const {
    return wordCount;
}

const std::multimap<int, std::string, std::greater<int>>& Sorter::getSortedDict() const {
    return sortedDict;
}
