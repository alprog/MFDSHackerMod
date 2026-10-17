#pragma once

#include "world/listener.h"
#include "world/meteorite.h"

#include <atomic>
#include <mutex>
#include <thread>

class Receiver
{
public:
    explicit Receiver(const Meteorite& meteorite);
    ~Receiver();

    void SetListener(Listener* listener);
    double Frequency() const;

private:
    void Loop();

    const Meteorite& meteorite;

    std::mutex mutex;
    Listener* listener = nullptr;

    std::atomic<bool> stop = false;
    std::thread thread;
};
