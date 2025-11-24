#include "Reader.h"
#include "Sorter.h"
#include "CsvOutputer.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " input.txt output.csv\n";
        return 1;
    }

    std::string inputFile(argv[1]);
    std::string outputFile(argv[2]);

    try {
        Reader readObject(inputFile);
        Sorter sortObject(readObject.getText());
        csvOutputer outputObject(outputFile);

        outputObject.write(sortObject.getWordCount(), sortObject.getSortedDict());
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1; 
    }

    return 0;
}
