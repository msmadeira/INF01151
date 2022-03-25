#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include "../shared/shared.h"
#include <string>

class ConnectionManager
{
public:
    socket_t socket_descriptor;

    void write_from_buffer(const char *buffer);
    void read_to_buffer(void *buffer);
    void close_socket();
};

ConnectionManager *connect_to_address_port(std::string server_address, std::string server_port);

#endif
