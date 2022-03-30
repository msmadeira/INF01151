#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include "../shared/shared.h"
#include "../libs/jsoncpp/json/json.h"
#include "server_broadcast.h"

#include <string>
#include <netdb.h>

struct ValueAddressTuple
{
public:
    Json::Value value;
    sockaddr_in address;
};

struct ServerReceiver
{
    socket_t socket_descriptor;
    AtomicVecQueue<ValueAddressTuple> receive_queue;
    AtomicVar<bool> must_terminate = false;
};

void *fn_server_listener(void *arg);

#endif
