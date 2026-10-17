#include "puzzle.h"

#include "../alien_utils.h"

Puzzle::Puzzle(std::initializer_list<Message> messages)
	: maxResponseLength{0}
{
	message = *messages.begin();
	validResponses.assign(messages.begin() + 1, messages.end());
	for (auto response : validResponses)
	{
		maxResponseLength = std::max(maxResponseLength, (int32_t)response.size());
	}
	cycleLength = message.size() + maxResponseLength;
}

bool Puzzle::isMessagePhase(int32_t intervalIndex) const
{
	return intervalIndex >= 0 && intervalIndex < message.size();
}

double Puzzle::getFreqAt(int32_t intervalIndex) const
{
	if (isMessagePhase(intervalIndex))
	{
		int32_t code = message[intervalIndex];
		return NAlienCode::toHumanFrequency(code);
	}
	else
	{
		return 0.0;
	}
}

bool Puzzle::checkResponse(Message& response) const
{
	for (auto& validResponse : validResponses)
	{
		if (response == validResponse)
		{
			return true;
		}
	}
	return false;
}