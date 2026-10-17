#include "alien_utils.h"

constexpr int64_t HumanSecond = 1'000'000'000; // nanoseconds
constexpr int64_t AlienSecond = 806'641'031; // nanoseconds
constexpr int64_t AlienEpochStartShift = 847'13'462; // nanoseconds (random)

constexpr double HumanHydrogenLine = 1'420'405'751.786; // Hz
constexpr double AlienHydrogenline = HumanHydrogenLine * AlienSecond / HumanSecond; // Hz

int64_t	NAlienTime::getGlobalIntervalIndex(int64_t nanoseconds)
{
	return (nanoseconds - AlienEpochStartShift) / AlienSecond;
}

int64_t NAlienTime::getTimeSinceIntervalStart(int64_t nanoseconds)
{
	return (nanoseconds - AlienEpochStartShift) % AlienSecond;
}

int64_t NAlienTime::getIntervalStartTime(int64_t nanoseconds)
{
	return nanoseconds - getTimeSinceIntervalStart(nanoseconds);
}

double NFrequency::fromSignalOffset(int32_t signalOffset)
{
	constexpr double ratio = static_cast<double>(AlienSecond) / HumanSecond;
	return (AlienHydrogenline + signalOffset) / ratio;
}