// MFDSHackerMod.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stone.h"

#include "utils.h"

#include <format>
#include <iostream>
#include <string>
#include <thread>

#include <chrono>

// Microseconds since Unix epoch (system_clock epoch is 1970-01-01 UTC, guaranteed since C++20)


// 1776000000123456 -> "1 776 000 000 123 456"
std::string GroupDigits(int64_t value)
{
    std::string digits = std::to_string(value);
    std::string result;
    for (size_t i = 0; i < digits.size(); ++i)
    {
        if (i > 0 && (digits.size() - i) % 3 == 0)
            result += ' ';
        result += digits[i];
    }
    return result;
}



int main()
{
    // ANSI escape sequences used for drawing:
    //   \x1b[2J   - clear the whole screen
    //   \x1b[?25l - hide the cursor
    //   \x1b[H    - move the cursor to the top-left corner
    //   \x1b[K    - clear from the cursor to the end of the line
    // The screen is cleared only once; each frame overwrites the previous one in place (no flicker)
    std::cout << "\x1b[2J\x1b[?25l";

    while (true)
    {
        // Take the time once so all lines of the frame describe the same moment
        int64_t us = GetTimeMicroseconds();

        // Microseconds since epoch -> local date/time (system time zone)
        std::chrono::sys_time<std::chrono::microseconds> tp{std::chrono::microseconds(us)};
        std::chrono::zoned_time local{std::chrono::current_zone(), tp};

        constexpr const char* STRT = "\x1b[H";
        constexpr const char* CLR = "\x1b[K";

        std::cout << STRT;

        double frequence = NStone::getFrequency();
        if (frequence == 0.0)
        {
            std::cout << "--NO SIGNAL--";
        }
        else
        {
            // Fixed notation (no 1.14576e+09), one digit after the decimal point
            std::cout << std::format("{:.1f}", frequence) << " Hz";
        }
        std::cout << CLR << std::endl;


        //std::cout << std::format("{:%Y-%m-%d %H:%M:%S}", local) << CLR << std::endl;
        //std::cout << GroupDigits(us) << " us" << CLR;


        // Push the whole frame to the console at once
        std::cout << std::flush;

        // Refresh 4 times per second
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

