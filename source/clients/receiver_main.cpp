#include "clients/client.h"

#include <iostream>
#include <string>

int ReceiverClientMain(const std::string& host, const std::string& port)
{
    Client client(host, port);
    std::cout << "Receiver, server " << client.Url() << ", ESC to exit" << std::endl;

    return client.FollowStream("/receiver/log",
                               [](const std::string& line) { std::cout << line << std::endl; },
                               [](int) {});
}
