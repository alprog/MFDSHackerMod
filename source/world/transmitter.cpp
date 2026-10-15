#include "transmitter.h"

#include "utils/utils.h"

Transmitter::Transmitter()
    : changeTime(GetTimeNanoseconds())
{
}

void Transmitter::SetListener(Listener* newListener)
{
    std::lock_guard lock(mutex);
    listener = newListener;
}

void Transmitter::WithState(const std::function<void(const State&)>& action) const
{
    std::lock_guard lock(mutex);
    action({ changeTime, frequency });
}

Transmitter::State Transmitter::Send(double frequency)
{
    return Set(frequency);
}

Transmitter::State Transmitter::Stop()
{
    return Set(0.0);
}

double Transmitter::Frequency() const
{
    std::lock_guard lock(mutex);
    return frequency;
}

Transmitter::State Transmitter::Set(double newFrequency)
{
    std::lock_guard lock(mutex);
    frequency = newFrequency;
    changeTime = GetTimeNanoseconds();
    if (listener)
    {
        listener->OnEvent(EventType::FrequencyChanged, changeTime, frequency);
    }
    return { changeTime, frequency };
}
