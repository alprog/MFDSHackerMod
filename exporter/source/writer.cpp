#include "writer.h"

#include <cstdint>
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

    for (size_t group = 0; group < hexString.size(); group += 8)
    {
        if (group > 0)
        {
            writeStream << ", ";
        }
        writeFreq(hexString.substr(group, 8));
    }
}

void Writer::writeFreq(std::string_view hexFreq)
{
    std::string bigEndian;
    for (size_t byte = 4; byte-- > 0;) // little indian
    {
        bigEndian += hexFreq.substr(byte * 2, 2);
    }
    writeStream << int32_t(std::stoul(bigEndian, nullptr, 16));
}
