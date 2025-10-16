#include "CsvOutputer.h"
#include <fstream>
#include <iostream>

csvOutputer::csvOutputer(const std::string &fileName) {
    this->fileName = fileName;
}

void csvOutputer::write(const int& wordCount,
                        const std::multimap<int, std::string, std::greater<int>>& sortedDict) const {
    std::ofstream out(fileName);
    if (out.is_open()) {
        for (const auto& [key, value] : sortedDict) {
            out << value << ',' << key << ',' << (100.0 * key / wordCount) << "%\n";
        }
        out.close();
    } else {
        std::cerr << "Error output-file-opening\n";
        exit(0);
    }
}
