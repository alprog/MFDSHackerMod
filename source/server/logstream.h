#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

class LogStream
{
public:
    ~LogStream();

    bool Subscribe(uintptr_t client);
    void Broadcast(const std::string& line);

private:
    std::mutex mutex;
    std::vector<uintptr_t> clients;
};
