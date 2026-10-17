#include "alien_utils.h"

#include <cmath>

constexpr int64_t HumanSecond = 1'000'000'000; // nanoseconds
constexpr int64_t AlienSecond = 806'641'031; // nanoseconds
constexpr int64_t AlienEpochStartShift = 847'13'462; // nanoseconds (random)

constexpr double HumanHydrogenLine = 1'420'405'751.768; // Hz
constexpr double AlienHydrogenline = HumanHydrogenLine / HumanSecond * AlienSecond; // Hz

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

int64_t NAlienTime::getNextIntervalMiddleTime(int64_t nanoseconds)
{
	int64_t middle = getIntervalStartTime(nanoseconds) + AlienSecond / 2;
	return middle > nanoseconds ? middle : middle + AlienSecond;
}

double NAlienCode::toHumanFrequency(int32_t alienCode)
{
	constexpr double alienToHumanRatio = static_cast<double>(HumanSecond) / AlienSecond;
	return (AlienHydrogenline + alienCode) * alienToHumanRatio;
}

int32_t NAlienCode::fromHumanFrequency(double humanFrequency)
{
	constexpr double humanToAlienRatio = static_cast<double>(AlienSecond) / HumanSecond;
	double AlienFrequency = humanFrequency * humanToAlienRatio;
	double offset = AlienFrequency - AlienHydrogenline;

	return std::lround(offset);
}