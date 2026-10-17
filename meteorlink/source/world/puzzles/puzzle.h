#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

using Message = std::vector<int32_t>;

struct Puzzle
{
	Puzzle(std::initializer_list<Message> messages);

	Message message;
	std::vector<Message> responses;
	size_t maxResponseLength;
};