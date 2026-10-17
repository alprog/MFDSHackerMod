#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

using Message = std::vector<int32_t>;

class Puzzle
{
public:
	Puzzle(std::initializer_list<Message> messages);

	int32_t getMessageLength() const { return message.size(); }
	int32_t getGapLength() const { return maxResponseLength; }
	int32_t getCycleLength() const { return cycleLength; }
	
	bool isMessagePhase(int32_t intervalIndex) const;
	double getFreqAt(int32_t intervalIndex) const;
	
	bool checkResponse(Message& response) const;

private:
	Message message;
	std::vector<Message> validResponses;
	int32_t maxResponseLength;
	int32_t cycleLength;
};