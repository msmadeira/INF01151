#include "client_receiver.h"

// C
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// C++
#include <iostream>

using namespace std;

void *fn_client_listener(void *arg)
{
    ClientReceiver *client_receiver = static_cast<ClientReceiver *>(arg);

    char buffer[BUFFER_SIZE];

    socket_t socket_descriptor = client_receiver->connection_details->socket_descriptor;

    Json::Reader reader;

    for (;;)
    {
        Json::Value messageValue;
        read_to_buffer(socket_descriptor, buffer);

        bool parseSuccess = reader.parse(buffer, messageValue, false);
        if (!parseSuccess)
        {
            cout << "ERROR parsing message on fn_client_listener: " << messageValue << endl;
            pthread_exit(NULL);
        }
        client_receiver->receive_queue.push(messageValue);
    }
}
