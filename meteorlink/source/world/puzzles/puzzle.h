#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

using Message = std::vector<int32_t>;

class Puzzle
{
public:
	Puzzle(std::initializer_list<Message> messages);

	size_t getMessageLength() const { return message.size(); }
	size_t getGapLength() const { return maxResponseLength; }
	
	double getFreqAt(int32_t localIntervalIndex) const;

private:
	Message message;
	std::vector<Message> responses;
	size_t maxResponseLength;
	size_t cycleLength;
};