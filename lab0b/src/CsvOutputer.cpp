#include "CsvOutputer.h"
#include <fstream>
#include <iostream>

csvOutputer::csvOutputer(const std::string &fileName) : fileName(fileName) {}

void csvOutputer::write(const int& wordCount,
                        const std::multimap<int, std::string, std::greater<int>>& sortedDict) const {
    std::ofstream out(fileName);
    if (!out.is_open()) {
        throw std::runtime_error("Error opening output file: " + fileName);
    } else {
        for (const auto& [key, value] : sortedDict) {
            out << value << ',' << key << ',' << (100.0 * key / wordCount) << "%\n";
        }
        out.close();
    }
}
