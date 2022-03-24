#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

#include <netdb.h>
#include "../shared/shared.h"
#include "user_connection.h"
#include "server_message.h"

struct ServerSender
{
    socket_t socket_descriptor;
    AtomicVecQueue<ServerMessage> send_queue;
};

void *fn_server_sender(void *arg);

#endif
