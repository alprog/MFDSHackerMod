#include "reader.h"
#include "yamlfile.h"

#include <fstream>

std::vector<Puzzle> Reader::readPuzzles(const std::string& srcFolderPath)
{
    asset_map assets = collectAssets(srcFolderPath);
    auto scenePath = findAsset(assets, "ControlRoom.unity");

    guid_list puzzleListGuids;
    readGuidList(scenePath, "puzzleLists", puzzleListGuids);

    guid_list puzzleGuids;
    for (auto guid : puzzleListGuids)
    {
        auto it = assets.find(guid);
        if (it == std::end(assets))
        {
            throw "can't find asset";
        }
        readGuidList(it->second, "puzzles", puzzleGuids);
    }

    std::vector<Puzzle> puzzles;
    for (const std::string& guid : puzzleGuids)
    {
        auto it = assets.find(guid);
        if (it == std::end(assets))
        {
            throw "can't find asset";
        }
        puzzles.emplace_back(it->second);
    }
    return puzzles;
}

Reader::asset_map Reader::collectAssets(const std::filesystem::path& root)
{
    const std::string prefix = "guid: ";

    std::unordered_map<std::string, std::filesystem::path> guids;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".meta")
        {
            continue;
        }

        std::ifstream file(entry.path());
        for (std::string line; std::getline(file, line);)
        {
            if (line.starts_with(prefix))
            {
                std::string guid = line.substr(prefix.size());
                std::filesystem::path assetPath = entry.path();
                assetPath.replace_extension();
                guids[guid] = assetPath;
                break;
            }
        }
    }
    return guids;
}

std::filesystem::path Reader::findAsset(const asset_map& assets, const std::string& fileName)
{
    for (const auto& [guid, path] : assets)
    {
        if (path.filename() == fileName)
        {
            return path;
        }
    }
    throw "can't find asset";
}

void Reader::readGuidList(const std::filesystem::path& path, const std::string& propertyName, guid_list& outGuids)
{
    const guid_list guids = yamlFile{ path }.readArray(propertyName, "guid");
    for (auto& guid : guids)
    {
        outGuids.push_back(guid);
    }
}
