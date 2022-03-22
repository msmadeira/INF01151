#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <string>
#include <netdb.h>
#include "client_helpers.h"
#include "../shared/shared.h"

struct ClientSender
{
    std::string *username;
    std::string *server_address;
    std::string *server_port;
    AtomicVecQueue<ClientMsg> send_queue;
};

void *fn_client_sender(void *arg);

#endif
