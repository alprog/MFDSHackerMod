#pragma once

#include <string>

// Console clients of the server. Both keep no state of their own: they show what the server
// streams, so every receiver and transmitter sees the same thing. ESC to exit.
// Return the process exit code.

// --receiver: print the receiver log as the server streams it
int RunReceiver(const std::string& host, const std::string& port);

// --transmitter: show the transmitter state, type <Hz> to transmit, off to stop
int RunTransmitter(const std::string& host, const std::string& port);
