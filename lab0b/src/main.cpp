#include "Reader.h"
#include "Sorter.h"
#include "CsvOutputer.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        std::cerr << "You have to use 3/4 args. Usage: " << argv[0] << " \n";
        return 1;
    }

    std::string inputFile(argv[1]);
    std::string outputFile(argv[2]);
    std::string sortComp = ">";
    if (argc == 4) {
        sortComp = argv[3];
    }

    try {
        Reader readObject(inputFile);
        if (readObject.getWordCount() == 0) {
            throw std::runtime_error("Input file is empty or contains no valid words.");
        }
        Sorter sortObject(readObject.getDict());
        sortObject.sort(sortComp);
        csvOutputer outputObject(outputFile);

        outputObject.write(readObject.getWordCount(), sortObject.getPairs());
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1; 
    }

    return 0;
}
