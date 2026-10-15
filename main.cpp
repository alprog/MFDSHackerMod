#include "client.h"
#include "server.h"
#include "utils.h"

#include <conio.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void PrintUsage()
{
    std::cout << "Usage: stone.exe [--receiver | --transmitter] [--host <address>] [--port <number>]\n"
              << "  (none)         run communication server with meteorite\n"
              << "  --receiver     console client displaying receiver stream\n"
              << "  --transmitter  console client controlling transmitter \n"
              << "  --host, --port server address, default 127.0.0.1 8080;\n"
              << "                 the server with --host 0.0.0.0 is reachable from other machines\n";
}

int RunServer(const std::string& host, const std::string& port)
{
    Server server(host, port);
    if (!server.Running())
    {
        return 1;
    }

    std::string url = MakeUrl(host, port);
    std::cout << "Server running on " << url << "\n\n"
              << "Web Page: " << url << "/\n\n"
              << "API:\n" << Server::Api() << "\n"
              << "Console Clients:\n"
              << "  stone.exe --receiver\n"
              << "  stone.exe --transmitter\n"
              << "  (add --host / --port if the server is not on 127.0.0.1:8080)\n\n"
              << "ESC to stop the server" << std::endl;

    constexpr int ESC = 27;
    while (!(_kbhit() && _getch() == ESC))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}

enum class Mode
{
    Server,
    Receiver,
    Transmitter
};

int main(int argc, char* argv[])
{
    Mode mode = Mode::Server;
    std::string host = "127.0.0.1";
    std::string port = "8080";

    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i)
    {
        const std::string& arg = args[i];
        if (arg == "--receiver")
        {
            mode = Mode::Receiver;
        }
        else if (arg == "--transmitter")
        {
            mode = Mode::Transmitter;
        }
        else if ((arg == "--host" || arg == "--port") && i + 1 < args.size())
        {
            (arg == "--host" ? host : port) = args[++i];
        }
        else
        {
            std::cerr << "Invalid argument: " << arg << "\n";
            PrintUsage();
            return 1;
        }
    }

    switch (mode)
    {
    case Mode::Receiver:
        return RunReceiver(host, port);
    case Mode::Transmitter:
        return RunTransmitter(host, port);
    default:
        return RunServer(host, port);
    }
}
