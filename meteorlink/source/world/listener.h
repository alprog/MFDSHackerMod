#pragma once

#include <cstdint>

enum class EventType
{
    SignalEdge,
    FrequencySampled,
    FrequencyChanged
};

class Listener
{
public:
    virtual ~Listener() = default;
    virtual void OnEvent(EventType type, int64_t timestamp, double param) = 0;
};
