#include "transmitter.h"

#include "utils/utils.h"

void Transmitter::SetListener(Listener* newListener)
{
    std::lock_guard lock(mutex);
    listener = newListener;
}

void Transmitter::Send(double frequency)
{
    Set(frequency);
}

void Transmitter::Stop()
{
    Set(0.0);
}

double Transmitter::Frequency() const
{
    std::lock_guard lock(mutex);
    return frequency;
}

void Transmitter::Set(double newFrequency)
{
    std::lock_guard lock(mutex);
    if (frequency != newFrequency)
    {
        frequency = newFrequency;
        if (listener)
        {
            listener->OnEvent(EventType::FrequencyChanged, GetTimeNanoseconds(), frequency);
        }
    }
}
