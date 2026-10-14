#include "stone.h"
#include "utils.h"

constexpr int64_t HumanSecond = 1'000'000'000; // nanoseconds
constexpr int64_t AlienSecond =   806'641'031; // nanoseconds
constexpr int64_t AlienEpochStartShift = 847'13'462; // nanoseconds (random)

constexpr double HumanHydrogenLine = 1'420'405'751.786; // Hz
constexpr double AlienHydrogenline = HumanHydrogenLine * AlienSecond / HumanSecond; // Hz

constexpr int64_t CycleLength = 2;
constexpr int64_t MessageLength = 1;

int64_t	getGlobalIntervalIndex(int64_t nanoseconds)
{
	return (nanoseconds - AlienEpochStartShift) / AlienSecond;
}

int64_t getIntervalStartTime(int64_t globalIndex)
{
	return AlienEpochStartShift + globalIndex * AlienSecond;
}

int64_t getCycleIntervalIndex(int64_t nanoseconds)
{
	return getGlobalIntervalIndex(nanoseconds) % CycleLength;
}

double toHumanFrequency(double alienFrequence)
{
	constexpr double ratio = static_cast<double>(AlienSecond) / HumanSecond;
	return alienFrequence / ratio;
}

double getFrequencyAt(int64_t nanoseconds)
{
	int64_t index = getCycleIntervalIndex(nanoseconds);
	if (index < MessageLength)
	{
		return toHumanFrequency(AlienHydrogenline + index);
	}
	else
	{
		return 0;
	}
}

NStone::FrequencySample NStone::sampleFrequency()
{
	int64_t nanoseconds = GetTimeNanoseconds();
	return { nanoseconds, getFrequencyAt(nanoseconds) };
}

bool isMessageInterval(int64_t globalIndex)
{
	return globalIndex % CycleLength < MessageLength;
}

int64_t NStone::getLastSignalEdgeTime(int64_t nanoseconds)
{
	int64_t globalIndex = getGlobalIntervalIndex(nanoseconds);
	int64_t cycleIndex = globalIndex % CycleLength;
	if (cycleIndex < MessageLength)
	{
		return getIntervalStartTime(globalIndex - cycleIndex);
	}
	else
	{
		return getIntervalStartTime(globalIndex - cycleIndex + MessageLength);
	}
}
