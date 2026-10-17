#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#include "puzzles/puzzle.h"

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

		bool hasSignal() const { return frequence != 0.0; }
	};

	FrequencySample sampleFrequency() const;
	
private:
	int64_t getLocalIntervalIndex(int64_t timestamp) const;
	
	void Loop();
	void checkCode();
	void checkMessage();

	const Puzzle* pPuzzle = nullptr;
	int64_t puzzleStartIntervalIndex = 0;
	Message receivedMessage;

	const Transmitter& transmitter;
	std::atomic<bool> stop = false;
	std::thread thread;
};