#pragma once

#include "world/listener.h"
#include "world/meteorite.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>

class Receiver
{
public:
    struct State
    {
        int64_t edgeTime;
        bool on;
    };

    explicit Receiver(const Meteorite& meteorite);
    ~Receiver();

    void SetListener(Listener* listener);
    void WithState(const std::function<void(const State&)>& action) const;
    double Frequency() const;

private:
    void Loop();

    const Meteorite& meteorite;

    mutable std::mutex mutex;
    Listener* listener = nullptr;
    int64_t sampleTime;
    double frequency;

    std::atomic<bool> stop = false;
    std::thread thread;
};
