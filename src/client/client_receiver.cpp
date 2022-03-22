#include "client_receiver.h"
#include "client_helpers.h"
#include "login.h"

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

    string *username = client_receiver->username;
    string *server_address = client_receiver->server_address;
    string *server_port = client_receiver->server_port;

    char buffer[BUFFER_SIZE];

    ConnectionDetails *connection_details = connect_to_address_port(*server_address, *server_port);
    if (connection_details == NULL)
    {
        fprintf(stderr, "fn_client_listener failed to connect to server.\n");
        exit(EXIT_FAILURE);
    }

    int socket_descriptor = connection_details->socket_descriptor;

    { // Login
        bool login_success = try_login(ClientMsgType::LoginReceiver, username, socket_descriptor, buffer);
        if (!login_success)
        {
            pthread_exit(NULL);
        }
    }

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
