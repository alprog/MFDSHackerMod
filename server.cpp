#include "server.h"

#include "stone.h"
#include "utils.h"
#include "web.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <charconv>
#include <chrono>
#include <format>
#include <iostream>
#include <optional>
#include <sstream>

constexpr std::chrono::milliseconds RECEIVER_INTERVAL(100);

constexpr DWORD SOCKET_TIMEOUT_MS = 1000;

static std::string Line(int64_t ns, const std::string& text)
{
    return FormatTime(ns) + "  " + text;
}

static std::optional<double> ParseFrequency(const std::string& value)
{
    double frequency;
    auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), frequency);
    if (error != std::errc() || end != value.data() + value.size())
    {
        return std::nullopt;
    }
    return frequency;
}

static std::optional<std::string> QueryParam(const std::string& query, const std::string& name)
{
    std::istringstream stream(query);
    for (std::string pair; std::getline(stream, pair, '&');)
    {
        size_t equals = pair.find('=');
        if (equals != std::string::npos && pair.substr(0, equals) == name)
        {
            return pair.substr(equals + 1);
        }
    }
    return std::nullopt;
}

static bool Send(uintptr_t client, const std::string& text)
{
    return send(client, text.data(), int(text.size()), 0) != SOCKET_ERROR;
}

static void SendResponse(uintptr_t client, const std::string& status, const std::string& type, const std::string& body)
{
    Send(client, std::format(
        "HTTP/1.1 {}\r\nContent-Type: {}; charset=utf-8\r\nContent-Length: {}\r\nConnection: close\r\n\r\n{}",
        status, type, body.size(), body));
}

Server::Server(const std::string& host, const std::string& port)
    : listener(INVALID_SOCKET), transmitterChangeTime(GetTimeNanoseconds())
{
    NStone::FrequencySample sample = NStone::sampleFrequency();
    receiverSampleTime = sample.timestamp;
    receiverFrequency = sample.frequence;

    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        std::cerr << "Cannot start Winsock\n";
        return;
    }

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    addrinfo* info = nullptr;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &info) != 0)
    {
        std::cerr << "Cannot resolve " << host << "\n";
        return;
    }

    listener = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    BOOL exclusive = TRUE;
    bool listening = listener != INVALID_SOCKET
        && setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusive, sizeof(exclusive)) != SOCKET_ERROR
        && bind(listener, info->ai_addr, int(info->ai_addrlen)) != SOCKET_ERROR
        && listen(listener, SOMAXCONN) != SOCKET_ERROR;
    bool busy = WSAGetLastError() == WSAEADDRINUSE;
    freeaddrinfo(info);
    if (!listening)
    {
        std::cerr << "Cannot listen on " << MakeUrl(host, port)
                  << (busy ? ": the address is in use, is a server already running?" : "") << "\n";
        return;
    }

    receiverThread = std::thread([this] { ReceiverLoop(); });
    acceptThread = std::thread([this] { AcceptLoop(); });
}

Server::~Server()
{
    stop = true;

    if (listener != INVALID_SOCKET)
    {
        closesocket(listener);
    }
    if (acceptThread.joinable())
    {
        acceptThread.join();
    }
    if (receiverThread.joinable())
    {
        receiverThread.join();
    }

    for (Stream* stream : {&receiver, &transmitter})
    {
        for (uintptr_t client : stream->clients)
        {
            closesocket(client);
        }
    }
    WSACleanup();
}

bool Server::Running() const
{
    return acceptThread.joinable();
}

std::string Server::Api()
{
    return
        "  GET /receiver/log                receiver log, endless text/plain\n"
        "  GET /receiver/freq               current receiver frequency in Hz\n"
        "  GET /transmitter/log             transmitter log, endless text/plain\n"
        "  GET /transmitter/send?freq=<Hz>  transmitter on at this frequency\n"
        "  GET /transmitter/stop            transmitter off\n";
}

void Server::AcceptLoop()
{
    while (true)
    {
        SOCKET client = accept(listener, nullptr, nullptr);
        if (client == INVALID_SOCKET)
        {
            return;
        }
        Handle(client);
    }
}

void Server::Handle(uintptr_t client)
{
    DWORD timeout = SOCKET_TIMEOUT_MS;
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    char buffer[1024];
    int size = recv(client, buffer, sizeof(buffer), 0);
    std::istringstream request(size > 0 ? std::string(buffer, size) : "");
    std::string method, target;
    request >> method >> target;

    size_t question = target.find('?');
    std::string path = target.substr(0, question);
    std::string query = question == std::string::npos ? "" : target.substr(question + 1);

    if (method != "GET")
    {
        SendResponse(client, "405 Method Not Allowed", "text/plain", "Only GET is supported\n");
    }
    else if (path == "/receiver/log" || path == "/transmitter/log")
    {
        bool isReceiver = path == "/receiver/log";
        Stream& stream = isReceiver ? receiver : transmitter;
        std::lock_guard lock(stream.mutex);
        std::string headers =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n"
            "X-Content-Type-Options: nosniff\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: close\r\n"
            "\r\n";
        std::string state = (isReceiver ? ReceiverStateLine() : TransmitterStateLine()) + "\n";
        if (Send(client, headers + state))
        {
            stream.clients.push_back(client);
            return;
        }
    }
    else if (path == "/receiver/freq")
    {
        SendResponse(client, "200 OK", "text/plain", std::format("{:.1f}\n", NStone::sampleFrequency().frequence));
    }
    else if (path == "/transmitter/send")
    {
        std::string status = "200 OK";
        std::string body = TransmitterSend(query, status);
        SendResponse(client, status, "text/plain", body);
    }
    else if (path == "/transmitter/stop")
    {
        SendResponse(client, "200 OK", "text/plain", TransmitterStop());
    }
    else if (path == "/")
    {
        SendResponse(client, "200 OK", "text/html", WEB_PAGE);
    }
    else
    {
        SendResponse(client, "404 Not Found", "text/plain", "Not Found\n");
    }
    shutdown(client, SD_SEND);
    closesocket(client);
}

std::string Server::TransmitterSend(const std::string& query, std::string& status)
{
    std::optional<std::string> value = QueryParam(query, "freq");
    std::optional<double> frequency = value ? ParseFrequency(*value) : std::nullopt;
    if (!frequency || *frequency <= 0.0)
    {
        status = "400 Bad Request";
        return "Invalid frequency: " + value.value_or("") + "\n";
    }

    std::lock_guard lock(transmitter.mutex);
    transmitterFrequency = *frequency;
    transmitterChangeTime = GetTimeNanoseconds();
    std::string line = TransmitterStateLine();
    Broadcast(transmitter, line);
    return line + "\n";
}

std::string Server::TransmitterStop()
{
    std::lock_guard lock(transmitter.mutex);
    transmitterFrequency = 0.0;
    transmitterChangeTime = GetTimeNanoseconds();
    std::string line = TransmitterStateLine();
    Broadcast(transmitter, line);
    return line + "\n";
}

std::string Server::TransmitterStateLine()
{
    return Line(transmitterChangeTime,
                transmitterFrequency > 0.0 ? std::format("TX ON  {:.1f} Hz", transmitterFrequency) : "TX OFF");
}

std::string Server::ReceiverStateLine()
{
    int64_t edgeTime = NStone::getLastSignalEdgeTime(receiverSampleTime);
    return Line(edgeTime, receiverFrequency != 0.0 ? "SIGNAL ON" : "SIGNAL OFF");
}

void Server::ReceiverLoop()
{
    while (!stop)
    {
        NStone::FrequencySample sample = NStone::sampleFrequency();
        bool hasSignal = sample.frequence != 0.0;
        int64_t edgeTime = NStone::getLastSignalEdgeTime(sample.timestamp);
        {
            std::lock_guard lock(receiver.mutex);
            if (edgeTime > receiverSampleTime)
            {
                Broadcast(receiver, Line(edgeTime, hasSignal ? "SIGNAL ON" : "SIGNAL OFF"));
            }
            if (hasSignal)
            {
                Broadcast(receiver, Line(sample.timestamp, std::format("{:.1f} Hz", sample.frequence)));
            }
            receiverSampleTime = sample.timestamp;
            receiverFrequency = sample.frequence;
        }

        std::this_thread::sleep_for(RECEIVER_INTERVAL);
    }
}

void Server::Broadcast(Stream& stream, const std::string& line)
{
    std::string text = line + "\n";
    std::erase_if(stream.clients, [&text](uintptr_t client)
    {
        if (!Send(client, text))
        {
            closesocket(client);
            return true;
        }
        return false;
    });
}
