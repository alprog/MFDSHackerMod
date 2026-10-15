#include "receiver.h"

#include <chrono>

constexpr std::chrono::milliseconds SAMPLE_INTERVAL(100);

Receiver::Receiver(const Meteorite& meteorite)
    : meteorite(meteorite)
{
    Meteorite::FrequencySample sample = meteorite.sampleFrequency();
    sampleTime = sample.timestamp;
    frequency = sample.frequence;

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

void Receiver::WithState(const std::function<void(const State&)>& action) const
{
    std::lock_guard lock(mutex);
    action({ meteorite.getLastSignalEdgeTime(sampleTime), frequency != 0.0 });
}

double Receiver::Frequency() const
{
    return meteorite.sampleFrequency().frequence;
}

void Receiver::Loop()
{
    while (!stop)
    {
        Meteorite::FrequencySample sample = meteorite.sampleFrequency();
        bool hasSignal = sample.frequence != 0.0;
        int64_t edgeTime = meteorite.getLastSignalEdgeTime(sample.timestamp);
        {
            std::lock_guard lock(mutex);
            if (listener && edgeTime > sampleTime)
            {
                listener->OnEvent(EventType::SignalEdge, edgeTime, hasSignal ? 1.0 : 0.0);
            }
            if (listener && hasSignal)
            {
                listener->OnEvent(EventType::FrequencySampled, sample.timestamp, sample.frequence);
            }
            sampleTime = sample.timestamp;
            frequency = sample.frequence;
        }

        std::this_thread::sleep_for(SAMPLE_INTERVAL);
    }
}
