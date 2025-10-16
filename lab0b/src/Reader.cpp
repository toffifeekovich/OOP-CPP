#include "Reader.h"
#include <iostream>
#include <fstream>
#include <cctype>

bool Reader::symbolCheker(const char ch) {
    return !isalnum(ch);
}

Reader::Reader(const std::string &fileName) {
    this->fileName = fileName;
    std::ifstream file(this->fileName);

    if (file.is_open()) {
        std::string line, word;
        while (std::getline(file, line)) {
            for (char ch : line) {
                if (isalnum(ch)) {
                    word += ch;
                } else if (!word.empty()) {
                    text.push_back(word);
                    word.clear();
                }
            }
        }
        if (!word.empty()) {
            text.push_back(word);
            word.clear();
        }
        file.close();
    } else {
        std::cerr << "Error of file opening\n";
        exit(0);
    }
}

const std::list<std::string>& Reader::getText() {
    return text;
}

const std::string& Reader::getFileName() {
    return fileName;
}

void Reader::printText() const {
    for (const std::string &str : text)
        std::cout << str << std::endl;
}

void Reader::printFileName() const {
    std::cout << fileName << std::endl;
}
