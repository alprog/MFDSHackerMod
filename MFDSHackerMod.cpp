#include "stone.h"

#include <conio.h>

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <thread>
#include "utils.h"

constexpr std::chrono::milliseconds LOG_INTERVAL(100);

std::string FormatTime(int64_t ns)
{
    int64_t us = ns / 1000;
    std::chrono::sys_time<std::chrono::microseconds> tp{std::chrono::microseconds(us)};
    std::chrono::zoned_time local{std::chrono::current_zone(), tp};
    return std::format("{:%Y-%m-%d %H:%M:%S}", local);
}

void PrintLine(int64_t ns, const std::string& text)
{
    std::cout << FormatTime(ns) << "  " << text << std::endl;
}

int main()
{
    int64_t lasttime = GetTimeNanoseconds();

    constexpr int ESC = 27;
    while (!(_kbhit() && _getch() == ESC))
    {
        NStone::FrequencySample sample = NStone::sampleFrequency();
        bool hasSignal = sample.frequence != 0.0;

        int64_t edgeTime = NStone::getLastSignalEdgeTime(sample.timestamp);
        if (edgeTime > lasttime)
        {
            PrintLine(edgeTime, hasSignal ? "SIGNAL ON" : "SIGNAL OFF");
        }

        if (hasSignal)
        {
            PrintLine(sample.timestamp, std::format("{:.1f} Hz", sample.frequence));
        }

        lasttime = sample.timestamp;

        std::this_thread::sleep_for(LOG_INTERVAL);
    }
    return 0;
}
