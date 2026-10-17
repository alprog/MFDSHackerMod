#include "puzzle.h"
#include "yamlfile.h"

Puzzle::Puzzle(const std::filesystem::path& path)
{
	name = path.stem().string();

	yamlFile yaml(path);
	rockOutput = yaml.readValue("rockOutput", "signals");
	winningResponse = yaml.readValue("winningResponse", "signals");
	altResponses = yaml.readArray("altResponses", "signals");
}
