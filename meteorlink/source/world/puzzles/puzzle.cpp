#include "puzzle.h"

Puzzle::Puzzle(std::initializer_list<Message> messages)
	: maxResponseLength{0}
{
	message = *messages.begin();
	responses.assign(messages.begin() + 1, messages.end());
	for (auto response : responses)
	{
		maxResponseLength = std::max(maxResponseLength, response.size());
	}
}