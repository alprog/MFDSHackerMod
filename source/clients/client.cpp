#include "client.h"

#include "utils/net.h"
#include "utils/utils.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <conio.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>

constexpr int ESC = 27;

static SOCKET Connect(const std::string& host, const std::string& port)
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

static int OpenGet(SOCKET socket, const std::string& host, const std::string& path, std::string& body)
{
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
    if (send(socket, request.data(), int(request.size()), 0) == SOCKET_ERROR)
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

struct Response
{
    int status;
    std::string body;
};

static std::optional<Response> Get(const std::string& host, const std::string& port, const std::string& path)
{
    SOCKET socket = Connect(host, port);
    if (socket == INVALID_SOCKET)
    {
        return std::nullopt;
    }

    Response response;
    response.status = OpenGet(socket, host, path, response.body);

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

static int FollowStream(const std::string& host, const std::string& port, const std::string& path,
                        const std::function<void(const std::string&)>& onLine,
                        const std::function<void(int)>& onKey)
{
    SOCKET socket = Connect(host, port);
    std::string data;
    if (socket == INVALID_SOCKET || OpenGet(socket, host, path, data) != 200)
    {
        std::cerr << "Cannot connect to the server at " << MakeUrl(host, port)
                  << ", start it first: MFDSHackerMod.exe\n";
        if (socket != INVALID_SOCKET)
        {
            closesocket(socket);
        }
        return 1;
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

    bool escape = false;
    while (!ended && !escape)
    {
        while (_kbhit())
        {
            int key = _getch();
            if (key == 0 || key == 0xE0)
            {
                _getch();
            }
            else if (key == ESC)
            {
                escape = true;
                break;
            }
            else
            {
                onKey(key);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    shutdown(socket, SD_BOTH);
    reader.join();
    closesocket(socket);

    std::cout << "\r\x1b[K" << (escape ? "" : "Disconnected from the server\n") << std::flush;
    return escape ? 0 : 1;
}

int RunReceiver(const std::string& host, const std::string& port)
{
    Winsock winsock;
    std::cout << "Receiver, server " << MakeUrl(host, port) << ", ESC to exit" << std::endl;

    return FollowStream(host, port, "/receiver/log",
                        [](const std::string& line) { std::cout << line << std::endl; },
                        [](int) {});
}

int RunTransmitter(const std::string& host, const std::string& port)
{
    Winsock winsock;
    std::cout << "Transmitter, server " << MakeUrl(host, port) << "\n"
              << "Type <Hz> + Enter to transmit, off to stop, ESC to exit" << std::endl;

    std::mutex consoleMutex;
    std::string input;
    auto print = [&](const std::string& line)
    {
        std::lock_guard lock(consoleMutex);
        std::cout << "\r\x1b[K" << line << "\n> " << input << std::flush;
    };

    auto run = [&](const std::string& command)
    {
        std::string path;
        if (command == "off" || command == "0")
        {
            path = "/transmitter/stop";
        }
        else if (command.find_first_not_of("0123456789.eE+-") == std::string::npos)
        {
            path = "/transmitter/send?freq=" + command;
        }
        else
        {
            print("Unknown command: " + command + " (<Hz> or off)");
            return;
        }

        std::optional<Response> response = Get(host, port, path);
        if (!response)
        {
            print("Server is not reachable");
        }
        else if (response->status != 200)
        {
            std::string body = response->body;
            while (!body.empty() && body.back() == '\n')
            {
                body.pop_back();
            }
            print(body);
        }
    };

    auto onKey = [&](int key)
    {
        std::string command;
        {
            std::lock_guard lock(consoleMutex);
            if (key == '\r')
            {
                command = input;
                input.clear();
                std::cout << "\r\x1b[K> " << std::flush;
            }
            else if (key == '\b')
            {
                if (!input.empty())
                {
                    input.pop_back();
                    std::cout << "\b \b" << std::flush;
                }
                return;
            }
            else if (key >= ' ' && key <= '~')
            {
                input += char(key);
                std::cout << char(key) << std::flush;
                return;
            }
        }
        if (!command.empty())
        {
            run(command);
        }
    };

    return FollowStream(host, port, "/transmitter/log", print, onKey);
}
