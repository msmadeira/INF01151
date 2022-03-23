#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include "../shared/shared.h"
#include <string>

struct ConnectionDetails
{
    socket_t socket_descriptor;
};

ConnectionDetails *connect_to_address_port(std::string server_address, std::string server_port);

void write_from_buffer(socket_t socket_descriptor, const char *buffer);

void read_to_buffer(socket_t socket_descriptor, void *buffer);

#endif
