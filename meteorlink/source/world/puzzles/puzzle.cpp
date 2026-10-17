#include "puzzle.h"

#include "../alien_utils.h"

Puzzle::Puzzle(std::initializer_list<Message> messages)
	: maxResponseLength{0}
{
	message = *messages.begin();
	responses.assign(messages.begin() + 1, messages.end());
	for (auto response : responses)
	{
		maxResponseLength = std::max(maxResponseLength, response.size());
	}
	cycleLength = message.size() + maxResponseLength;
}

double Puzzle::getFreqAt(int32_t localIntervalIndex) const
{
	int64_t index = localIntervalIndex % cycleLength;
	if (index < message.size())
	{
		int32_t signalOffset = message[index];
		return NFrequency::fromSignalOffset(signalOffset);
	}
	else
	{
		return 0.0;
	}
}