#ifndef CSVOUTPUTER_H
#define CSVOUTPUTER_H

#include <string>
#include <vector>

class csvOutputer {
    std::string fileName;

public:
    explicit csvOutputer(const std::string &fileName);
    void write(const int& wordCount, const std::vector<std::pair<std::string, int>>& sortedList) const;
};

#endif // CSVOUTPUTER_H
