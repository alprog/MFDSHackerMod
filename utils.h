#pragma once

#include <cstdint>
#include <string>

int64_t GetTimeMicroseconds();
int64_t GetTimeNanoseconds();

// Nanoseconds since Unix epoch -> local date/time, e.g. "2026-10-12 20:13:33.857863"
std::string FormatTime(int64_t ns);

// "http://127.0.0.1:8080" (IPv6 host goes in brackets)
std::string MakeUrl(const std::string& host, const std::string& port);
