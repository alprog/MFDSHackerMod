#include "net.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

Winsock::Winsock()
{
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
}

Winsock::~Winsock()
{
    WSACleanup();
}

bool SendText(uintptr_t socket, const std::string& text)
{
    return send(socket, text.data(), int(text.size()), 0) != SOCKET_ERROR;
}

void CloseSocket(uintptr_t socket)
{
    closesocket(socket);
}
