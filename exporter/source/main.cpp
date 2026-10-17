#include "reader.h"
#include "writer.h"

#include <iostream>

int main()
{
    constexpr const char* srcFolderPath = "C:/projects/deepspace/ExportedProject";
    constexpr const char* dstFilePath = "../meteorlink/source/world/puzzles.h";

    std::vector<Puzzle> puzzles = Reader().readPuzzles(srcFolderPath);
    Writer(dstFilePath).writePuzzles(puzzles);
    std::cout << "puzzles: " << puzzles.size() << " -> " << std::filesystem::absolute(dstFilePath).string() << "\n";
    return 0;
}
