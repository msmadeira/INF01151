#include "server_receiver.h"

// C++
#include <iostream>

using namespace std;

void *fn_server_listener(void *arg)
{
    ServerReceiver *server_receiver = static_cast<ServerReceiver *>(arg);
    socket_t socket_descriptor = server_receiver->socket_descriptor;

    char buffer[BUFFER_SIZE];
    char host[NI_MAXHOST], service[NI_MAXSERV];
    struct sockaddr_in client_address;
    socklen_t client_address_struct_size = sizeof(struct sockaddr_storage);
    Json::Reader reader;

    for (;;)
    {
        if (server_receiver->must_terminate.read())
        {
            break;
        }
        /* receive from socket */
        memset(buffer, 0, sizeof(buffer));
        int number_of_bytes = recvfrom(
            socket_descriptor,
            buffer,
            BUFFER_SIZE,
            NONE,
            (struct sockaddr *)&client_address,
            &client_address_struct_size);
        if (number_of_bytes < 0)
        {
#ifdef DEBUG
            cout << "ERROR on recvfrom" << endl
                 << endl;
#endif
            continue;
        }
#ifdef DEBUG
        printf("Received a datagram: %s\n", buffer);
#endif

        int exit_code = getnameinfo(
            (struct sockaddr *)&client_address,
            client_address_struct_size,
            host,
            NI_MAXHOST,
            service,
            NI_MAXSERV,
            NI_NUMERICSERV);
        if (exit_code != EXIT_SUCCESS)
        {
#ifdef DEBUG
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(exit_code));
#endif
            continue;
        }
        else
        {
#ifdef DEBUG
            printf("Received %d bytes from %s:%s\n\n", number_of_bytes, host, service);
#endif
        }

        Json::Value messageValue;
        bool parseSuccess = reader.parse(buffer, messageValue, false);
        if (!parseSuccess)
        {
#ifdef DEBUG
            cout << "ERROR parsing message" << endl
                 << endl;
#endif
            continue;
        }

        server_receiver->receive_queue.push(ValueAddressTuple{messageValue, client_address});
    }

    pthread_exit(NULL);
}
