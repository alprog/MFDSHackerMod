#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

class yamlFile
{
public:
    yamlFile(const std::filesystem::path& path);

    std::string readValue(const std::string& baseProperty, const std::string& key);
    std::vector<std::string> readArray(const std::string& baseProperty, const std::string& key);

private:
    std::string findProperty(const std::string& propertyName);

    std::ifstream file;
};
