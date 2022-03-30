#include "server_sender.h"
#include "../libs/jsoncpp/json/json.h"
// C
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// C++
#include <iostream>

using namespace std;

inline void send_to_client(const char *buffer, struct sockaddr *client_address, socket_t socket_descriptor)
{
    int number_of_bytes = sendto(
        socket_descriptor,
        buffer,
        strlen(buffer),
        NONE,
        client_address,
        sizeof(struct sockaddr));
    if (number_of_bytes < 0)
        printf("ERROR on sendto");
}

void *fn_server_sender(void *arg)
{
    ServerSender *server_sender = static_cast<ServerSender *>(arg);

    char buffer[BUFFER_SIZE];

    socket_t socket_descriptor = server_sender->socket_descriptor;

    for (;;)
    {
        bool must_terminate = server_sender->must_terminate.read();
        vector<ServerMessage> message_queue = server_sender->send_queue.drain();
        for (ServerMessage message : message_queue)
        {
            ServerMessageData data = message.data;
            string json_encoded = data.serialize();
            send_to_client(json_encoded.c_str(), (struct sockaddr *)&(message.address), socket_descriptor);
        }
        if (must_terminate)
        {
            break;
        }
    }

    pthread_exit(NULL);
}
