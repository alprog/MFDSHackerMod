#pragma once

#include <cstdint>
#include <string>

struct Winsock
{
    Winsock();
    ~Winsock();
};

bool SendText(uintptr_t socket, const std::string& text);
void CloseSocket(uintptr_t socket);
