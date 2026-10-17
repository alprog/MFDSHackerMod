#pragma once

#include "puzzle.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

class Reader
{
public:
    std::vector<Puzzle> readPuzzles(const std::string& srcFolderPath);

private:
    using asset_map = std::unordered_map<std::string, std::filesystem::path>;
    using guid_list = std::vector<std::string>;

    static asset_map collectAssets(const std::filesystem::path& root);
    static std::filesystem::path findAsset(const asset_map& assets, const std::string& fileName);
    static void readGuidList(const std::filesystem::path& path, const std::string& propertyName, guid_list& outGuids);
};
