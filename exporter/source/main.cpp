#include "reader.h"
#include "writer.h"

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: exporter <srcFolderPath>\n";
        return 1;
    }
    const char* srcFolderPath = argv[1];
    constexpr const char* dstFilePath = "../meteorlink/source/world/puzzles.h";

    std::vector<Puzzle> puzzles = Reader().readPuzzles(srcFolderPath);
    Writer(dstFilePath).writePuzzles(puzzles);
    std::cout << "puzzles: " << puzzles.size() << " -> " << std::filesystem::absolute(dstFilePath).string() << "\n";
    return 0;
}
