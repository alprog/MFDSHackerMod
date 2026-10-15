#pragma once

#include "world/meteorite.h"
#include "world/receiver.h"
#include "world/transmitter.h"

class World
{
public:
    World();

    Transmitter transmitter;
    Meteorite meteorite;
    Receiver receiver;
};
