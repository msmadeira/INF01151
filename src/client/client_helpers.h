#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include <string>

void write_from_buffer(int socket_descriptor, const char *buffer);

void read_to_buffer(int socket_descriptor, void *buffer);

struct ConnectionDetails
{
    int socket_descriptor;
    struct addrinfo *server_address_info;
};

ConnectionDetails *connect_to_address_port(std::string server_address, std::string server_port);

#endif
