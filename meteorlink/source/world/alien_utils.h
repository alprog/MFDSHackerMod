#pragma once

#include <cstdint>

namespace NAlienTime
{
	int64_t	getGlobalIntervalIndex(int64_t nanoseconds);
	int64_t getTimeSinceIntervalStart(int64_t nanoseconds);
	int64_t getIntervalStartTime(int64_t nanoseconds);
	int64_t getNextIntervalMiddleTime(int64_t nanoseconds);
}

namespace NAlienCode
{
	double toHumanFrequency(int32_t alienCode);
	int32_t fromHumanFrequency(double humanFrequency);
}