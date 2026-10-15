#pragma once

#include "world/listener.h"

#include <mutex>

class Transmitter
{
public:
    void SetListener(Listener* listener);

    void Send(double frequency);
    void Stop();
    double Frequency() const;

private:
    void Set(double frequency);

    mutable std::mutex mutex;
    Listener* listener = nullptr;
    double frequency = 0.0;
};
