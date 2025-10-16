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

    Reader A(inputFile);
    Sorter B(A.getText());
    csvOutputer C(outputFile);
    C.write(B.getWordCount(), B.getSortedDict());

    return 0;
}
