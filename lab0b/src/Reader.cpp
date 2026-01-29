#include "Reader.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <stdexcept>


Reader::Reader(const std::string &fileName) : fileName(fileName) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + fileName);
    } else {
        std::string line, word;
        while (std::getline(file, line)) {
            for (char ch : line) {
                if (isalnum(ch)) {
                    word += ch;
                } else if (!word.empty()) {
                    dict[word] += 1;
                    wordCount += 1;
                    word.clear();
                }
            }
            if (!word.empty()) {
                dict[word] += 1;
                wordCount += 1;
                word.clear();
            }
        }
    }
}

std::map<std::string, int>& Reader::getDict() {
    return dict;
}

const int& Reader::getWordCount() const {
    return wordCount;
}
void Reader::clearDict() {
    dict.clear();
}
