#ifndef CSVOUTPUTER_H
#define CSVOUTPUTER_H

#include <string>
#include <map>
#include <functional>

class csvOutputer {
    std::string fileName;

public:
    explicit csvOutputer(const std::string &fileName);
    void write(const int& wordCount, const std::multimap<int, std::string, std::greater<int>>& sortedDict) const;
};

#endif // CSVOUTPUTER_H
