#pragma once

#include <cstdint>

namespace NStone
{
	struct FrequencySample
	{
		int64_t timestamp;
		double frequence;
	};

	FrequencySample sampleFrequency();
	
	int64_t getLastSignalEdgeTime(int64_t nanoseconds);
};