#include "server_broadcast.h"

// Network
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <string.h>

using namespace std;

socket_t fn_server_broadcast()
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    struct addrinfo *address_candidates;
    int exit_code = getaddrinfo(NULL, PORT_STR, &hints, &address_candidates);
    if (exit_code != EXIT_SUCCESS)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(exit_code));
        freeaddrinfo(address_candidates);
        return INVALID_SOCKET;
    }

    struct addrinfo *server_address;
    socket_t socket_descriptor;
    for (server_address = address_candidates; server_address != NULL; server_address = server_address->ai_next)
    {
        socket_descriptor = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
        if (socket_descriptor == INVALID_SOCKET)
            continue;

        if (bind(socket_descriptor, server_address->ai_addr, server_address->ai_addrlen) == 0)
            break; /* Success */

        close(socket_descriptor);
    }

    freeaddrinfo(address_candidates); /* No longer needed */

    if (server_address == NULL)
    { /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        return INVALID_SOCKET;
    }

    return socket_descriptor;
}
