#pragma once

#include <cstdint>

namespace NAlienTime
{
	int64_t	getGlobalIntervalIndex(int64_t nanoseconds);
	int64_t getTimeSinceIntervalStart(int64_t nanoseconds);
	int64_t getIntervalStartTime(int64_t nanoseconds);
}

namespace NFrequency
{
	double fromSignalOffset(int32_t signalOffset);
}