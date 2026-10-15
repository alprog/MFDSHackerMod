#pragma once

#include "server/logstream.h"
#include "utils/net.h"
#include "world/world.h"

#include <cstdint>
#include <string>
#include <thread>

class Server : private Listener
{
public:
    Server(World& world, const std::string& host, const std::string& port);
    ~Server();

    bool Running() const;

    static std::string Api();

private:
    void OnEvent(EventType type, int64_t timestamp, double param) override;

    void AcceptLoop();
    void Handle(uintptr_t client);
    bool SubscribeReceiver(uintptr_t client);
    bool SubscribeTransmitter(uintptr_t client);
    std::string TransmitterSend(const std::string& query, std::string& status);

    Winsock winsock;
    World& world;

    LogStream receiverLog;
    LogStream transmitterLog;

    uintptr_t listener;
    std::thread acceptThread;
};
