#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include <string>
#include <netdb.h>
#include "../libs/jsoncpp/json/json.h"
#include "../shared/shared.h"
#include "client_connection.h"

struct ClientReceiver
{
    ConnectionDetails *connection_details;
    AtomicVecQueue<Json::Value> receive_queue;
};

void *fn_client_listener(void *arg);

#endif
