#ifndef READER_H
#define READER_H

#include <string>
#include <list>
#include <map>

class Reader {
    std::map<std::string, int> dict;
    std::string fileName;
    int wordCount = 0;

public:
    explicit Reader(const std::string &fileName);
    std::map<std::string, int>& getDict();
    const int& getWordCount() const;
    void clearDict();

};

#endif // READER_H
