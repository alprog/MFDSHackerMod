#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct Puzzle
{
	Puzzle(const std::filesystem::path& path);

	std::string name;
	std::string rockOutput;
	std::string winningResponse;
	std::vector<std::string> altResponses;
};