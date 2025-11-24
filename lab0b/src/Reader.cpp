#include "Reader.h"
#include <iostream>
#include <fstream>
#include <cctype>

Reader::Reader(const std::string &fileName) : fileName(fileName) {
    std::ifstream file(this->fileName);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + fileName);
    } else {
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
            if (!word.empty()) {
                text.push_back(word);
                word.clear();
            }
        }
    }
}

const std::list<std::string>& Reader::getText() {
    return text;
}
