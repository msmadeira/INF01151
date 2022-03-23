#include "client_sender.h"
#include "../libs/jsoncpp/json/json.h"
#include "client_connection.h"

// C
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// C++
#include <iostream>

using namespace std;

void *fn_client_sender(void *arg)
{
    ClientSender *client_sender = static_cast<ClientSender *>(arg);

    char buffer[BUFFER_SIZE];

    socket_t socket_descriptor = client_sender->connection_details->socket_descriptor;

    for (;;)
    {
        vector<ClientMsg> message_queue = client_sender->send_queue.drain();
        for (ClientMsg message : message_queue)
        {
            string json_encoded = message.serialize();
            write_from_buffer(socket_descriptor, json_encoded.c_str());
        }
    }
}
