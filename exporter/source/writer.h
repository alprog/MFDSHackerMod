#pragma once

#include "puzzle.h"

#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

class Writer
{
public:
    Writer(const std::filesystem::path& path);

    void writePuzzles(const std::vector<Puzzle>& puzzles);

private:
    void writeMessageLine(std::string_view hexString);
    void writeMessage(std::string_view signalString);
    void writeHexFreq(std::string_view hexFreq);

    std::ofstream writeStream;
};
