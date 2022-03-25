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
    bool must_terminate = false;

    for (;;)
    {
        vector<ClientMessageData> message_queue = client_sender->send_queue.drain();
        for (ClientMessageData message : message_queue)
        {
            string json_encoded = message.serialize();
            client_sender->connection_manager->write_from_buffer(json_encoded.c_str());
            if (message.msg_type == ClientMsgType::ClientLogout)
            {
                must_terminate = true;
            }
        }
        if (must_terminate)
        {
            break;
        }
    }

    pthread_exit(NULL);
}
