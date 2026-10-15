#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// The single source of truth for receivers and transmitters: an HTTP server that
//   - samples the stone and streams the receiver log to every receiver client,
//   - keeps the transmitter state and streams its changes to every transmitter client.
// Streams are text/plain without Content-Length, so they never end: lines show up
// in curl -N, a console client or the web page as they are sent.
// Lives for the scope of the object: the constructor starts listening, the destructor stops.
class Server
{
public:
    Server(const std::string& host, const std::string& port);
    ~Server();

    // False if the server could not start (the reason is printed to std::cerr)
    bool Running() const;

    // API description for the console, one endpoint per line
    static std::string Api();

private:
    // Clients of one stream; the mutex also guards the state the stream reports
    struct Stream
    {
        std::mutex mutex;
        std::vector<uintptr_t> clients; // SOCKETs, kept as integers so this header doesn't need winsock2.h
    };

    void AcceptLoop();
    void Handle(uintptr_t client);
    void ReceiverLoop();

    // Request handlers: return the response body, set status on error
    std::string TransmitterSend(const std::string& query, std::string& status);
    std::string TransmitterStop();

    // Lines describing the current state, sent first to a new client. Call with the stream mutex locked.
    std::string ReceiverStateLine();
    std::string TransmitterStateLine();

    // Send a line to all clients of the stream, dropping the disconnected ones. Call with stream.mutex locked.
    static void Broadcast(Stream& stream, const std::string& line);

    uintptr_t listener;
    std::atomic<bool> stop = false;
    std::thread acceptThread;
    std::thread receiverThread;

    Stream receiver;
    Stream transmitter;

    // Last receiver sample streamed, guarded by receiver.mutex
    // (a new client gets the state the stream has already reported, so no edge comes twice)
    int64_t receiverSampleTime;
    double receiverFrequency;

    // Transmitter state, guarded by transmitter.mutex
    double transmitterFrequency = 0.0; // Hz, 0 - off
    int64_t transmitterChangeTime;     // nanoseconds since Unix epoch
};
