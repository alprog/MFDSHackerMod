
#include <iostream>
#include <string>
#include <vector>

using MainFunction = int(const std::string& host, const std::string& port);

MainFunction ServerMain;
MainFunction ReceiverClientMain;
MainFunction TransmitterClientMain;

int main(int argc, char* argv[])
{
    MainFunction* mainFunction = ServerMain;
    std::string host = "127.0.0.1";
    std::string port = "8080";

    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i)
    {
        const std::string& arg = args[i];
        if (arg == "--receiver")
        {
            mainFunction = ReceiverClientMain;
        }
        else if (arg == "--transmitter")
        {
            mainFunction = TransmitterClientMain;
        }
        else if ((arg == "--host" || arg == "--port") && i + 1 < args.size())
        {
            (arg == "--host" ? host : port) = args[++i];
        }
        else
        {
            std::cerr << "Invalid argument: " << arg << "\n";
            std::cout << "Usage: stone.exe [--receiver | --transmitter] [--host <address>] [--port <number>]\n"
                << "  (none)         run communication server with meteorite\n"
                << "  --receiver     console client displaying receiver stream\n"
                << "  --transmitter  console client controlling transmitter \n"
                << "  --host, --port server address, default 127.0.0.1 8080;\n"
                << "                 the server with --host 0.0.0.0 is reachable from other machines\n";
            return 1;
        }
    }

    return mainFunction(host, port);
}
