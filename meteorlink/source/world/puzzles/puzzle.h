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
	size_t getCycleLength() const { return cycleLength; }
	
	bool isMessagePhase(int32_t intervalIndex) const;
	double getFreqAt(int32_t intervalIndex) const;
	
	bool checkResponse(Message& response) const;

private:
	Message message;
	std::vector<Message> validResponses;
	size_t maxResponseLength;
	size_t cycleLength;
};