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

    Json::Reader reader;

    for (;;)
    {
        Json::Value messageValue;
        client_receiver->connection_manager->read_to_buffer(buffer);

        bool parseSuccess = reader.parse(buffer, messageValue, false);
        if (!parseSuccess)
        {
            cout << "ERROR parsing message on fn_client_listener: " << messageValue << endl;
            pthread_exit(NULL);
        }
        client_receiver->receive_queue.push(messageValue);

        if (client_receiver->must_terminate.read())
        {
            break;
        }
    }

    pthread_exit(NULL);
}
