#include "stone.h"
#include "utils.h"

constexpr double HumanHydrogenLine = 1'420'405'751.786; // Hz

constexpr int32_t AlienHydrogenline = 1'145'757'516; // Hz
constexpr int32_t AlienSecond = 806641; // microseconds
constexpr int32_t AlienEpochStartShift = 8471093562; // microseconds (random)

int32_t	getIntervalIndex()
{
	return (GetTimeMicroseconds() - AlienEpochStartShift) / AlienSecond;
}

double ToHumanFrequency(double alienFrequence)
{
	constexpr double ratio = static_cast<double>(AlienSecond) / 1'000'000.0;
	return double(AlienHydrogenline) / ratio;
}

double NStone::getFrequency()
{
	if (getIntervalIndex() % 2)
	{
		return 0;
	}
	else
	{
		return ToHumanFrequency(AlienHydrogenline);
	}
}