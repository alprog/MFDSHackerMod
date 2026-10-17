#include "utils.h"

#include <chrono>
#include <format>

int64_t GetTimeMicroseconds()
{
    using namespace std::chrono;
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

int64_t GetTimeNanoseconds()
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

std::string FormatTime(int64_t ns)
{
    int64_t us = ns / 1000;
    std::chrono::sys_time<std::chrono::microseconds> tp{std::chrono::microseconds(us)};
    std::chrono::zoned_time local{std::chrono::current_zone(), tp};
    return std::format("{:%Y-%m-%d %H:%M:%S}", local);
}

std::string LogLine(int64_t ns, const std::string& text)
{
    return FormatTime(ns) + "  " + text;
}

std::string MakeUrl(const std::string& host, const std::string& port)
{
    bool ipv6 = host.find(':') != std::string::npos;
    return std::format("http://{}{}{}:{}", ipv6 ? "[" : "", host, ipv6 ? "]" : "", port);
}
