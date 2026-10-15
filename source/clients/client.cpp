#include "client.h"

#include "utils/utils.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <conio.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>


Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port)
{
}

std::string Client::Url() const
{
    return MakeUrl(host, port);
}

uintptr_t Client::Connect() const
{
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo* info = nullptr;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &info) != 0)
    {
        return INVALID_SOCKET;
    }

    SOCKET result = INVALID_SOCKET;
    for (addrinfo* address = info; address && result == INVALID_SOCKET; address = address->ai_next)
    {
        result = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (result != INVALID_SOCKET && connect(result, address->ai_addr, int(address->ai_addrlen)) == SOCKET_ERROR)
        {
            closesocket(result);
            result = INVALID_SOCKET;
        }
    }
    freeaddrinfo(info);
    return result;
}

int Client::OpenGet(uintptr_t socket, const std::string& path, std::string& body) const
{
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
    if (!SendText(socket, request))
    {
        return -1;
    }

    std::string data;
    size_t end;
    while ((end = data.find("\r\n\r\n")) == std::string::npos)
    {
        char buffer[1024];
        int size = recv(socket, buffer, sizeof(buffer), 0);
        if (size <= 0)
        {
            return -1;
        }
        data.append(buffer, size);
    }
    body = data.substr(end + 4);

    size_t space = data.find(' ');
    return space == std::string::npos ? -1 : std::atoi(data.c_str() + space + 1);
}

std::optional<Client::Response> Client::Get(const std::string& path) const
{
    uintptr_t socket = Connect();
    if (socket == INVALID_SOCKET)
    {
        return std::nullopt;
    }

    Response response;
    response.status = OpenGet(socket, path, response.body);

    char buffer[1024];
    for (int size; (size = recv(socket, buffer, sizeof(buffer), 0)) > 0;)
    {
        response.body.append(buffer, size);
    }
    closesocket(socket);

    if (response.status < 0)
    {
        return std::nullopt;
    }
    return response;
}

int Client::FollowStream(const std::string& path,
                         const std::function<void(const std::string&)>& onLine,
                         const std::function<bool(int)>& onKey,
                         const std::function<void()>& onConnected) const
{
    uintptr_t socket = Connect();
    std::string data;
    if (socket == INVALID_SOCKET || OpenGet(socket, path, data) != 200)
    {
        std::cerr << "Cannot connect to the server at " << Url() << ", start it first: MFDSHackerMod.exe\n";
        if (socket != INVALID_SOCKET)
        {
            closesocket(socket);
        }
        return 1;
    }

    if (onConnected)
    {
        onConnected();
    }

    std::atomic<bool> ended = false;
    std::thread reader([&]
    {
        while (true)
        {
            size_t end;
            while ((end = data.find('\n')) != std::string::npos)
            {
                onLine(data.substr(0, end));
                data.erase(0, end + 1);
            }

            char buffer[1024];
            int size = recv(socket, buffer, sizeof(buffer), 0);
            if (size <= 0)
            {
                break;
            }
            data.append(buffer, size);
        }
        ended = true;
    });

    bool stopped = false;
    while (!ended && !stopped)
    {
        while (_kbhit())
        {
            int key = _getch();
            if (key == 0 || key == 0xE0)
            {
                _getch();
            }
            else if (!onKey(key))
            {
                stopped = true;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    shutdown(socket, SD_BOTH);
    reader.join();
    closesocket(socket);

    std::cout << "\r\x1b[K" << (stopped ? "" : "Disconnected from the server\n") << std::flush;
    return stopped ? 0 : 1;
}
