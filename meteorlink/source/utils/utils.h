#pragma once

#include <cstdint>
#include <string>

int64_t GetTimeMicroseconds();
int64_t GetTimeNanoseconds();

std::string FormatTime(int64_t ns);
std::string LogLine(int64_t ns, const std::string& text);

std::string MakeUrl(const std::string& host, const std::string& port);
