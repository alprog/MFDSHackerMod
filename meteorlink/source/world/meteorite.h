#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

class Transmitter;

class Meteorite
{
public:
	explicit Meteorite(const Transmitter& transmitter);
	~Meteorite();

	struct FrequencySample
	{
		int64_t timestamp;
		double frequence;
		int64_t intervalStart;
	};

	FrequencySample sampleFrequency() const;
	
	int64_t getLastSignalEdgeTime(int64_t nanoseconds) const;

private:
	void Loop();
	void checkSignal(double frequency);

	const Transmitter& transmitter;
	std::atomic<bool> stop = false;
	std::thread thread;
};