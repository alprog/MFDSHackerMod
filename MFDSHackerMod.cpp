// MFDSHackerMod.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stone.h"

#include "utils.h"

#include <atomic>
#include <charconv>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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



// Current receiver frequency as text, or "--NO SIGNAL--"
std::string FormatReceiver()
{
    double frequence = NStone::getFrequency();
    if (frequence == 0.0)
    {
        return "--NO SIGNAL--";
    }
    // Fixed notation (no 1.14576e+09), one digit after the decimal point
    return std::format("{:.1f} Hz", frequence);
}

// Transmitter frequency is stored in a file next to the exe, so it survives between runs
// (in command-line mode every command is a separate process). Set in main() from argv[0].
std::filesystem::path TransmitterFile;

// Transmitter frequency in Hz, or nullopt if it was never set
std::optional<double> LoadTransmitter()
{
    std::ifstream file(TransmitterFile);
    double frequence;
    if (file >> frequence)
    {
        return frequence;
    }
    return std::nullopt;
}

// Transmitter frequency as text, or "n/a" if not set
std::string FormatTransmitter()
{
    std::optional<double> frequence = LoadTransmitter();
    if (!frequence)
    {
        return "n/a";
    }
    return std::format("{:.1f} Hz", *frequence);
}

// -s <Hz>: store the transmitter frequency
int SetTransmitter(const std::string& value)
{
    double frequence;
    auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), frequence);
    if (error != std::errc() || end != value.data() + value.size())
    {
        std::cerr << "Invalid frequency: " << value << "\n";
        return 1;
    }

    std::ofstream file(TransmitterFile);
    file << std::format("{:.1f}", frequence);
    file.close(); // flush before FormatTransmitter() reads it back
    if (!file)
    {
        std::cerr << "Cannot write " << TransmitterFile.string() << "\n";
        return 1;
    }

    std::cout << "Transmitter set to " << FormatTransmitter() << std::endl;
    return 0;
}

// -g: print the current receiver frequency once and exit
void GetReceiver()
{
    std::cout << FormatReceiver() << std::endl;
}

// Microseconds since Unix epoch -> local date/time, e.g. "2026-10-12 20:13:33.857863"
std::string FormatTime(int64_t us)
{
    std::chrono::sys_time<std::chrono::microseconds> tp{std::chrono::microseconds(us)};
    std::chrono::zoned_time local{std::chrono::current_zone(), tp};
    return std::format("{:%Y-%m-%d %H:%M:%S}", local);
}

// --laststart: when the last message started
void LastStart()
{
    std::cout << FormatTime(NStone::getLastMessageStartTime()) << std::endl;
}

// --laststop: when the last message ended
void LastStop()
{
    std::cout << FormatTime(NStone::getLastMessageEndTime()) << std::endl;
}

// ANSI escape sequences used for drawing:
//   \x1b[2J   - clear the whole screen
//   \x1b[H    - move the cursor to the top-left corner
//   \x1b[K    - clear from the cursor to the end of the line
//   \x1b[J    - clear from the cursor to the end of the screen
//   \x1b[R;CH - move the cursor to row R, column C
//   \x1b[?25l - hide the cursor,  \x1b[?25h - show it
//   \x1b7     - save the cursor position,  \x1b8 - restore it
//   \x1b[Nr   - scroll region: only lines N..bottom scroll, lines above stay fixed;  \x1b[r - reset
// Frames overwrite the previous one in place instead of clearing the screen (no flicker)
constexpr const char* CLR = "\x1b[K";

// Number of screen lines taken by MonitorFrame()
constexpr int MONITOR_LINES = 3;

// Receiver/transmitter table (MONITOR_LINES lines), drawn from the current cursor position.
// Returned as one string so the whole frame goes to the console in a single write.
std::string MonitorFrame()
{
    // Receiver cell text
    std::string receiver = FormatReceiver();

    // Transmitter cell text (re-read every frame, so -s from another console shows up live)
    std::string transmitter = FormatTransmitter();

    // Two-column table, each cell centered within COL characters, plus a separator line
    constexpr int COL = 30;
    std::string frame;
    frame += std::format("{:^{}}{:^{}}", "Receiver:", COL, "Transmitter:", COL) + CLR + "\n";
    frame += std::format("{:^{}}{:^{}}", receiver, COL, transmitter, COL) + CLR + "\n";
    frame += std::string(COL * 2, '-') + CLR;
    return frame;
}

// -m: full-screen live monitor, Enter to exit
void Monitor()
{
    // Wait for Enter in a separate thread (std has no non-blocking key read)
    std::atomic<bool> stop = false;
    std::thread input([&stop]
    {
        std::string line;
        if (std::getline(std::cin, line))
        {
            stop = true;
        }
    });

    // Clear the screen once and hide the cursor
    std::cout << "\x1b[2J\x1b[?25l";

    while (!stop)
    {
        // Back to the top-left corner and redraw the frame over the previous one
        std::cout << "\x1b[H" + MonitorFrame() << std::flush;

        // Refresh 10 times per second
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    input.join();

    // Clear the screen and show the cursor again
    std::cout << "\x1b[2J\x1b[H\x1b[?25h" << std::flush;
}

void PrintUsage()
{
    std::cout << "Usage:\n"
              << "  -g          get current receiver frequency (Hz)\n"
              << "  -s <Hz>     set transmitter frequency\n"
              << "  -m          live receiver/transmitter monitor\n"
              << "  --laststart time the last message started\n"
              << "  --laststop  time the last message ended\n";
}

void PrintReplHelp()
{
    std::cout << "Commands:\n"
              << "  -h, --help   show this help\n"
              << "  -g           get current receiver frequency (Hz)\n"
              << "  -s <Hz>      set transmitter frequency\n"
              << "  --laststart  time the last message started\n"
              << "  --laststop   time the last message ended\n"
              << "  -q           quit\n";
}

// Run one command; args[0] is the command, the rest are its arguments. Returns exit code.
int RunCommand(const std::vector<std::string>& args)
{
    std::string command = args.empty() ? "" : args[0];

    if (command == "-g")
    {
        GetReceiver();
    }
    else if (command == "--laststart")
    {
        LastStart();
    }
    else if (command == "--laststop")
    {
        LastStop();
    }
    else if (command == "-m")
    {
        Monitor();
    }
    else if (command == "-s")
    {
        if (args.size() < 2)
        {
            std::cerr << "Missing frequency for " << command << "\n";
            PrintUsage();
            return 1;
        }
        return SetTransmitter(args[1]);
    }
    else
    {
        std::cerr << "Unknown command: " << command << "\n";
        PrintUsage();
        return 1;
    }
    return 0;
}

// Interactive mode: live monitor fixed at the top of the screen, command prompt below it.
// Reads commands line by line until -q or end of input.
void Repl()
{
    // Both threads write to the console; the lock keeps their escape sequences from interleaving
    std::mutex consoleMutex;

    // Clear the screen and make only the lines below the monitor scroll.
    // Setting the scroll region moves the cursor home, so then move it to the first line under the monitor.
    std::cout << "\x1b[2J" << std::format("\x1b[{}r\x1b[{};1H", MONITOR_LINES + 1, MONITOR_LINES + 1);
    PrintReplHelp();

    // Background thread redraws the monitor without disturbing the prompt and the text being typed
    std::atomic<bool> stop = false;
    std::thread monitor([&]
    {
        while (!stop)
        {
            {
                std::lock_guard lock(consoleMutex);
                // Save cursor position, draw at the top, restore position - all in one write.
                // The cursor is not hidden/shown here: toggling it every frame makes it flicker.
                // ("\x1b" "7" is split: "\x1b7" would be parsed as a single hex escape)
                std::cout << "\x1b" "7" "\x1b[H" + MonitorFrame() + "\x1b" "8" << std::flush;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    while (true)
    {
        {
            std::lock_guard lock(consoleMutex);
            std::cout << "> " << std::flush;
        }

        std::string line;
        if (!std::getline(std::cin, line))
        {
            break;
        }

        // Keep only the last command's output on screen: clear everything below the monitor
        // (\x1b[J - clear from the cursor to the end of the screen)
        {
            std::lock_guard lock(consoleMutex);
            std::cout << std::format("\x1b[{};1H\x1b[J", MONITOR_LINES + 1);
        }

        // Split the line into words
        std::vector<std::string> args;
        std::istringstream stream(line);
        for (std::string word; stream >> word;)
        {
            args.push_back(word);
        }
        if (args.empty())
        {
            continue;
        }

        std::lock_guard lock(consoleMutex);
        const std::string& command = args[0];
        if (command == "-q")
        {
            break;
        }
        else if (command == "-h" || command == "--help")
        {
            PrintReplHelp();
        }
        else if (command == "-g" || command == "-s" || command == "--laststart" || command == "--laststop")
        {
            RunCommand(args);
        }
        else
        {
            std::cerr << "Unknown command: " << command << " (-h for help)\n";
        }
    }

    stop = true;
    monitor.join();

    // Reset the scroll region and clear the screen
    std::cout << "\x1b[r\x1b[2J\x1b[H" << std::flush;
}

int main(int argc, char* argv[])
{
    TransmitterFile = std::filesystem::absolute(argv[0]).parent_path() / "transmitter.txt";

    if (argc < 2)
    {
        Repl();
        return 0;
    }
    return RunCommand(std::vector<std::string>(argv + 1, argv + argc));
}

