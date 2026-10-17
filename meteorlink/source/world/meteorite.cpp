#include "meteorite.h"
#include "world/transmitter.h"
#include "utils/utils.h"

#include "puzzles/puzzles.h"
#include "alien_utils.h"
#include <chrono>

Meteorite::Meteorite(const Transmitter& transmitter)
	: transmitter(transmitter)
{
	pPuzzle = &getPuzzles()[0];
	thread = std::thread([this] { Loop(); });
}

Meteorite::~Meteorite()
{
	stop = true;
	thread.join();
}

int64_t Meteorite::getLocalIntervalIndex(int64_t timestamp) const
{
	int64_t globalIndex = NAlienTime::getGlobalIntervalIndex(timestamp);
	return (globalIndex - puzzleStartIntervalIndex) % pPuzzle->getCycleLength();
}

Meteorite::FrequencySample Meteorite::sampleFrequency() const
{
	int64_t timestamp = GetTimeNanoseconds();
	int64_t intervalIndex = getLocalIntervalIndex(timestamp);
	double frequency = pPuzzle->getFreqAt(intervalIndex);
	return { timestamp, frequency };
}

void Meteorite::Loop()
{
	while (!stop)
	{
		int64_t currentTime = GetTimeNanoseconds();
		int64_t listenTime = NAlienTime::getNextIntervalMiddleTime(currentTime);
		std::this_thread::sleep_until(std::chrono::sys_time<std::chrono::nanoseconds>(std::chrono::nanoseconds(listenTime)));
		
		int64_t timestamp = GetTimeNanoseconds();
		int64_t intervalIndex = getLocalIntervalIndex(timestamp);;
		if (!pPuzzle->isMessagePhase(intervalIndex))
		{
			checkCode();

			if (intervalIndex == pPuzzle->getCycleLength() - 1)
			{
				checkMessage();
			}
		}
	}
}

void Meteorite::checkCode()
{
	auto frequency = transmitter.Frequency();
	if (frequency == 0.0)
	{
		return;
	}

	int32_t code = NAlienCode::fromHumanFrequency(frequency);
	receivedMessage.push_back(code);
}

void Meteorite::checkMessage()
{
	if (pPuzzle->checkResponse(receivedMessage))
	{
		pPuzzle++;

		int64_t timestamp = GetTimeNanoseconds();
		int64_t globalIndex = NAlienTime::getGlobalIntervalIndex(timestamp);
		puzzleStartIntervalIndex = globalIndex + 1;
	}
	receivedMessage.clear();
}