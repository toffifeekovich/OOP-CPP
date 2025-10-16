#ifndef READER_H
#define READER_H

#include <string>
#include <list>

class Reader {
    std::string fileName;
    std::list<std::string> text;

    bool symbolCheker(const char ch);

public:
    explicit Reader(const std::string &fileName);
    const std::list<std::string>& getText();
    const std::string& getFileName();
    void printText() const;
    void printFileName() const;
};

#endif // READER_H
