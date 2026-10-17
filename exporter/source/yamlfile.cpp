#include "yamlfile.h"

namespace
{
    bool startsWith(std::string_view text, std::string_view prefix)
    {
        size_t begin = text.find_first_not_of(' ');
        return begin != std::string_view::npos && text.substr(begin).starts_with(prefix);
    }

    std::string extractInlineValue(std::string_view line, std::string_view key)
    {
        const std::string prefix = std::string(key) + ": ";
        size_t begin = line.find(prefix);
        if (begin == std::string_view::npos)
        {
            throw "can't extract value";
        }
        begin += prefix.size();
        size_t end = line.find_first_of(", }", begin);
        return std::string(line.substr(begin, end - begin));
    }
}

yamlFile::yamlFile(const std::filesystem::path& path) : file(path)
{
    if (!file)
    {
        throw "can't open file";
    }
}

std::string yamlFile::readValue(const std::string& baseProperty, const std::string& key)
{
    findProperty(baseProperty);
    std::string line;
    if (!std::getline(file, line))
    {
        throw "can't read property";
    }
    return extractInlineValue(line, key);
}

std::vector<std::string> yamlFile::readArray(const std::string& baseProperty, const std::string& key)
{
    findProperty(baseProperty);

    std::vector<std::string> items;
    std::string line;
    while (std::getline(file, line) && startsWith(line, "-"))
    {
        items.push_back(extractInlineValue(line, key));
    }
    return items;
}

std::string yamlFile::findProperty(const std::string& propertyName)
{
    const std::string propertyLine = propertyName + ":";
    std::string line;
    while (std::getline(file, line))
    {
        if (startsWith(line, propertyLine))
        {
            return line;
        }
    }
    throw "can't find property";
}
