
#include "server/server.h"
#include "utils/utils.h"
#include "world/world.h"

#include <conio.h>

#include <chrono>
#include <iostream>
#include <thread>

int ServerMain(const std::string& host, const std::string& port)
{
    World world;
    Server server(world, host, port);
    if (!server.Running())
    {
        return 1;
    }

    std::string url = MakeUrl(host, port);
    std::cout << "Server running on " << url << "\n\n"
              << "Web Page: " << url << "/\n\n"
              << "API:\n" << Server::Api() << "\n"
              << "Console Clients:\n"
              << "  meteorlink.exe --receiver\n"
              << "  meteorlink.exe --transmitter\n"
              << "  (add --host / --port if the server is not on 127.0.0.1:8080)\n\n"
              << "ESC to stop the server" << std::endl;

    constexpr int ESC = 27;
    while (!(_kbhit() && _getch() == ESC))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}
