#include "stone.h"
#include "utils.h"

constexpr double HumanHydrogenLine = 1'420'405'751.786; // Hz

constexpr int32_t AlienHydrogenline = 1'145'757'516; // Hz
constexpr int32_t AlienSecond = 806641; // microseconds
constexpr int32_t AlienEpochStartShift = 84713562; // microseconds (random)

constexpr int32_t CycleLength = 2;
constexpr int32_t MessageLength = 1;

int64_t	getGlobalIntervalIndex()
{
	return (GetTimeMicroseconds() - AlienEpochStartShift) / AlienSecond;
}

int32_t getCycleIntervalIndex()
{
	return getGlobalIntervalIndex() % CycleLength;
}

int64_t getTimeOfInteral(int64_t index)
{
	return int64_t(AlienSecond) * index + AlienEpochStartShift;
}

double ToHumanFrequency(double alienFrequence)
{
	constexpr double ratio = static_cast<double>(AlienSecond) / 1'000'000.0;
	return double(AlienHydrogenline) / ratio;
}

double NStone::getFrequency()
{
	if (getCycleIntervalIndex() < MessageLength)
	{
		return ToHumanFrequency(AlienHydrogenline);
	}
	else
	{
		return 0;
	}
}

int64_t NStone::getLastMessageStartTime()
{
	int64_t index = getGlobalIntervalIndex();
	int64_t cycleStart = index - index % CycleLength;
	return getTimeOfInteral(cycleStart);
}

int64_t NStone::getLastMessageEndTime()
{
	int64_t index = getGlobalIntervalIndex();
	int64_t cycleStart = index - index % CycleLength;
	int64_t endInterval = cycleStart + MessageLength;
	if (index - cycleStart < MessageLength)
	{
		endInterval -= CycleLength;
	}
	return getTimeOfInteral(endInterval);
}