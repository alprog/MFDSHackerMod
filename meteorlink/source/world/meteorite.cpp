#include "meteorite.h"
#include "world/transmitter.h"
#include "utils/utils.h"

#include "puzzles/puzzles.h"
#include "alien_utils.h"
#include <chrono>

constexpr std::chrono::milliseconds ListenInterval(100);

Meteorite::FrequencySample Meteorite::sampleFrequency() const
{
	int64_t timestamp = GetTimeNanoseconds();
	int64_t globalIndex = NAlienTime::getGlobalIntervalIndex(timestamp);
	int64_t localIndex = globalIndex - puzzleStartIntervalIndex;

	double frequency = pPuzzle->getFreqAt(localIndex);
	return { timestamp, frequency };
}

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

void Meteorite::Loop()
{
	while (!stop)
	{
		checkSignal(transmitter.Frequency());
		std::this_thread::sleep_for(ListenInterval);
	}
}

void Meteorite::checkSignal(double frequency)
{
}
