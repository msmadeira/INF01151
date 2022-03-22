#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include <string>
#include <netdb.h>
#include "../libs/jsoncpp/json/json.h"
#include "../shared/shared.h"

struct ClientReceiver
{
    std::string *username;
    std::string *server_address;
    std::string *server_port;
    AtomicVecQueue<Json::Value> receive_queue;
};

void *fn_client_listener(void *arg);

#endif
