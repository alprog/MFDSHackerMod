#include "clients/client.h"

#include <iostream>
#include <mutex>
#include <optional>
#include <string>

int TransmitterClientMain(const std::string& host, const std::string& port)
{
    Client client(host, port);
    std::cout << "Transmitter, server " << client.Url() << "\n"
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

        std::optional<Client::Response> response = client.Get(path);
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

    return client.FollowStream("/transmitter/log", print, onKey);
}
