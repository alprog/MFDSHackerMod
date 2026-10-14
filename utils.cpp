#include "utils.h"

#include <chrono>

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