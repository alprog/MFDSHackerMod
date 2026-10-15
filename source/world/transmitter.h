#pragma once

#include "world/listener.h"

#include <cstdint>
#include <functional>
#include <mutex>

class Transmitter
{
public:
    struct State
    {
        int64_t changeTime;
        double frequency;
    };

    Transmitter();

    void SetListener(Listener* listener);
    void WithState(const std::function<void(const State&)>& action) const;

    State Send(double frequency);
    State Stop();
    double Frequency() const;

private:
    State Set(double frequency);

    mutable std::mutex mutex;
    Listener* listener = nullptr;
    double frequency = 0.0;
    int64_t changeTime;
};
