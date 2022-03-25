#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include <string>
#include <netdb.h>
#include "../libs/jsoncpp/json/json.h"
#include "../shared/shared.h"
#include "client_connection.h"

struct ClientReceiver
{
    ConnectionManager *connection_manager;
    AtomicVecQueue<Json::Value> receive_queue;
    AtomicVar<bool> must_terminate{false};
};

void *fn_client_listener(void *arg);

#endif
