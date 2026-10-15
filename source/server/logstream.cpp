#include "logstream.h"

#include "utils/net.h"

LogStream::~LogStream()
{
    for (uintptr_t client : clients)
    {
        CloseSocket(client);
    }
}

bool LogStream::Subscribe(uintptr_t client, const std::string& firstLine)
{
    std::string headers =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "X-Content-Type-Options: nosniff\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n"
        "\r\n";
    std::lock_guard lock(mutex);
    if (!SendText(client, headers + firstLine + "\n"))
    {
        return false;
    }
    clients.push_back(client);
    return true;
}

void LogStream::Broadcast(const std::string& line)
{
    std::string text = line + "\n";
    std::lock_guard lock(mutex);
    std::erase_if(clients, [&text](uintptr_t client)
    {
        if (!SendText(client, text))
        {
            CloseSocket(client);
            return true;
        }
        return false;
    });
}
