#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

class Transmitter;
class Puzzle;

class Meteorite
{
public:
	explicit Meteorite(const Transmitter& transmitter);
	~Meteorite();

	struct FrequencySample
	{
		int64_t timestamp;
		double frequence;

		bool hasSignal() const { return frequence != 0.0; }
	};

	FrequencySample sampleFrequency() const;
	
private:
	void Loop();
	void checkSignal(double frequency);

	const Puzzle* pPuzzle = nullptr;
	int64_t puzzleStartIntervalIndex = 0;

	const Transmitter& transmitter;
	std::atomic<bool> stop = false;
	std::thread thread;
};