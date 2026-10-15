#pragma once

#include "utils/net.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>

class Client
{
public:
    struct Response
    {
        int status;
        std::string body;
    };

    Client(const std::string& host, const std::string& port);

    std::string Url() const;
    std::optional<Response> Get(const std::string& path) const;
    int FollowStream(const std::string& path,
                     const std::function<void(const std::string&)>& onLine,
                     const std::function<bool(int)>& onKey,
                     const std::function<void()>& onConnected = {}) const;

private:
    uintptr_t Connect() const;
    int OpenGet(uintptr_t socket, const std::string& path, std::string& body) const;

    Winsock winsock;
    std::string host;
    std::string port;
};
