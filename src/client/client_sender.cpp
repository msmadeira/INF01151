#include "client_sender.h"
#include "../libs/jsoncpp/json/json.h"
#include "client_helpers.h"
#include "login.h"

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

    string *username = client_sender->username;
    string *server_address = client_sender->server_address;
    string *server_port = client_sender->server_port;

    char buffer[BUFFER_SIZE];

    ConnectionDetails *connection_details = connect_to_address_port(*server_address, *server_port);
    if (connection_details == NULL)
    {
        fprintf(stderr, "fn_client_sender failed to connect to server.\n");
        exit(EXIT_FAILURE);
    }

    int socket_descriptor = connection_details->socket_descriptor;

    { // Login
        bool login_success = try_login(ClientMsgType::LoginSender, username, socket_descriptor, buffer);
        if (!login_success)
        {
            pthread_exit(NULL);
        }
    }

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
