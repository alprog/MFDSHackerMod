#include "receiver.h"

#include <chrono>
#include "alien_utils.h"

constexpr std::chrono::milliseconds SAMPLE_INTERVAL(100);

Receiver::Receiver(const Meteorite& meteorite)
    : meteorite(meteorite)
{
    thread = std::thread([this] { Loop(); });
}

Receiver::~Receiver()
{
    stop = true;
    thread.join();
}

void Receiver::SetListener(Listener* newListener)
{
    std::lock_guard lock(mutex);
    listener = newListener;
}

double Receiver::Frequency() const
{
    return meteorite.sampleFrequency().frequence;
}

void Receiver::Loop()
{
    auto lastSample = meteorite.sampleFrequency();
    while (!stop)
    {
        auto sample = meteorite.sampleFrequency();
        bool hasSignal = sample.hasSignal();
        if (hasSignal != lastSample.hasSignal())
        {
            int64_t edgeTime = NAlienTime::getIntervalStartTime(sample.timestamp);
            listener->OnEvent(EventType::SignalEdge, edgeTime, hasSignal ? 1.0 : 0.0);
        }
        if (listener && sample.hasSignal())
        {
            listener->OnEvent(EventType::FrequencySampled, sample.timestamp, sample.frequence);
        }
        lastSample = sample;

        std::this_thread::sleep_for(SAMPLE_INTERVAL);
    }
}
