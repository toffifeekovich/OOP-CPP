#include "CsvOutputer.h"
#include <fstream>
#include <iostream>
#include <iomanip>


csvOutputer::csvOutputer(const std::string &fileName) : fileName(fileName) {}

void csvOutputer::write(const int& wordCount, 
    const std::vector<std::pair<std::string, int>>& sortedList) const {
    std::ofstream out(fileName);
    if (!out.is_open()) {
        throw std::runtime_error("Error opening output file: " + fileName);
    } else {   
        for (const auto& [word, count] : sortedList) {
            double percentage = 100.0 * count / wordCount;
            out << word << ',' 
            << count << ',' 
            <<  std::fixed << std::setprecision(2) << percentage << "%\n";
        }
        out.close();
    }
}
