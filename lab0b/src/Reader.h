#ifndef READER_H
#define READER_H

#include <string>
#include <list>

class Reader {
    std::string fileName;
    std::list<std::string> text;

public:
    explicit Reader(const std::string &fileName);
    const std::list<std::string>& getText();
};

#endif // READER_H
