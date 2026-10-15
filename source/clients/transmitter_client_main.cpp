#include "clients/client.h"

#include <cstdlib>
#include <format>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>

int TransmitterClientMain(const std::string& host, const std::string& port)
{
    Client client(host, port);
    std::cout << "Transmitter, server " << client.Url() << "\n"
              << "Type <Hz> to transmit" << std::endl;

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
        if (command == "stop" || command == "0")
        {
            path = "/transmitter/stop";
        }
        else if (command.find_first_not_of("0123456789.eE+-") == std::string::npos)
        {
            path = "/transmitter/send?freq=" + command;
        }
        else
        {
            print("Unknown command: " + command + " (<Hz> or stop)");
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
            }
            else if (key >= ' ' && key <= '~')
            {
                input += char(key);
                std::cout << char(key) << std::flush;
            }
        }
        if (!command.empty())
        {
            run(command);
        }
        return true;
    };

    auto printState = [&]
    {
        std::optional<Client::Response> response = client.Get("/transmitter/freq");
        if (response && response->status == 200)
        {
            double frequency = std::atof(response->body.c_str());
            print(frequency > 0.0 ? std::format("TX ON  {:.1f} Hz", frequency) : "TX OFF");
        }
    };

    return client.FollowStream("/transmitter/log", print, onKey, printState);
}
