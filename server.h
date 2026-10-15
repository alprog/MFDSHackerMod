#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Server
{
public:
    Server(const std::string& host, const std::string& port);
    ~Server();

    bool Running() const;

    static std::string Api();

private:
    struct Stream
    {
        std::mutex mutex;
        std::vector<uintptr_t> clients;
    };

    void AcceptLoop();
    void Handle(uintptr_t client);
    void ReceiverLoop();

    std::string TransmitterSend(const std::string& query, std::string& status);
    std::string TransmitterStop();

    std::string ReceiverStateLine();
    std::string TransmitterStateLine();

    static void Broadcast(Stream& stream, const std::string& line);

    uintptr_t listener;
    std::atomic<bool> stop = false;
    std::thread acceptThread;
    std::thread receiverThread;

    Stream receiver;
    Stream transmitter;

    int64_t receiverSampleTime;
    double receiverFrequency;

    double transmitterFrequency = 0.0;
    int64_t transmitterChangeTime;
};
