#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <netdb.h>
#include "client_connection.h"
#include "../shared/shared.h"

struct ClientSender
{
    ConnectionManager *connection_manager;
    AtomicVecQueue<ClientMessageData> send_queue;
};

void *fn_client_sender(void *arg);

#endif
