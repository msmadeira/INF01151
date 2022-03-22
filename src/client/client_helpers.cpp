#include "client_helpers.h"
#include "../shared/shared.h"
#include <string.h>
#include <vector>

// Networking
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// Multithreading
#include <semaphore.h>

void write_from_buffer(int socket_descriptor, const char *buffer)
{
    int number_of_bytes = write(socket_descriptor, buffer, strlen(buffer));
    if (number_of_bytes != strlen(buffer))
    {
        fprintf(stderr, "partial/failed write\n");
        exit(EXIT_FAILURE);
    }
}

void read_to_buffer(int socket_descriptor, void *buffer)
{
    int number_of_bytes = read(socket_descriptor, buffer, BUFFER_SIZE);
    if (number_of_bytes == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
}

ConnectionDetails *connect_to_address_port(std::string server_address, std::string server_port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = NONE;          /* For wildcard IP address */
    hints.ai_protocol = DEFAULT_PROTOCOL;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    struct addrinfo *address_candidates;
    int exit_code = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &address_candidates);
    if (exit_code != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(exit_code));
        // exit(EXIT_FAILURE);
        return NULL;
    }

    struct addrinfo *server_address_info;
    int socket_descriptor;
    for (server_address_info = address_candidates; server_address_info != NULL; server_address_info = server_address_info->ai_next)
    {
        socket_descriptor = socket(
            server_address_info->ai_family,
            server_address_info->ai_socktype,
            server_address_info->ai_protocol);
        if (socket_descriptor == -1)
            continue;

        if (connect(socket_descriptor, server_address_info->ai_addr, server_address_info->ai_addrlen) != -1)
            break; /* Success */

        close(socket_descriptor);
    }
    freeaddrinfo(address_candidates);

    if (server_address_info == NULL)
    { /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        return NULL;
    }

    return new ConnectionDetails{socket_descriptor, server_address_info};
}
