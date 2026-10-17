#include "writer.h"

#include <algorithm>
#include <cctype>
#include <string>

Writer::Writer(const std::filesystem::path& path) 
    : writeStream(path)
{
    if (!writeStream)
    {
        throw "can't open file";
    }
}

void Writer::writePuzzles(const std::vector<Puzzle>& puzzles)
{
    writeStream << "#pragma once\n";
    writeStream << "\n";
    writeStream << "#include <cstdint>\n";
    writeStream << "#include <vector>\n";
    writeStream << "\n";
    writeStream << "using Message = std::vector<int32_t>;\n";
    writeStream << "using Puzzle = std::vector<Message>;\n";
    writeStream << "\n";
    writeStream << "inline const std::vector<Puzzle> puzzles = {\n";
    for (size_t i = 0; i < puzzles.size(); i++)
    {
        const Puzzle& puzzle = puzzles[i];
        writeStream << "\t// " << i << " " << puzzle.name << ":\n";
        writeStream << "\t{\n";
        writeMessageLine(puzzle.rockOutput);
        writeMessageLine(puzzle.winningResponse);
        for (const std::string& altResponse : puzzle.altResponses)
        {
            writeMessageLine(altResponse);
        }
        writeStream << "\t},\n";
    }
    writeStream << "};\n";
}

void Writer::writeMessageLine(std::string_view hexString)
{
    writeStream << "\t\t{ ";
    writeMessage(hexString);
    writeStream << " },\n";
}

void Writer::writeMessage(std::string_view hexString)
{
    if (hexString.size() % 8 != 0)
    {
        throw "message length is not a multiple of 8";
    }

    std::string hex(hexString);
    std::transform(hex.begin(), hex.end(), hex.begin(), [](unsigned char c) { return char(std::toupper(c)); });

    for (size_t group = 0; group < hex.size(); group += 8)
    {
        if (group > 0)
        {
            writeStream << ", ";
        }
        writeHexFreq(std::string_view(hex).substr(group, 8));
    }
}

void Writer::writeHexFreq(std::string_view hexFreq)
{
    writeStream << "0x";
    for (size_t byte = 4; byte-- > 0;) // little indian
    {
        writeStream << hexFreq.substr(byte * 2, 2);
    }
}
