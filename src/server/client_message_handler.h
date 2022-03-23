#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include "../shared/shared.h"
#include "../libs/jsoncpp/json/json.h"

#include "server_action.h"
#include "user_persistence.h"
#include "user_connection.h"

class ClientMessageHandler
{
private:
    MsgIdManager msg_id_manager;
    UserPersistence *user_persistence;
    UserConnectionManager *connection_manager;

public:
    ClientMessageHandler(UserPersistence *user_persistence, UserConnectionManager *connection_manager);
    inline ServerAction *handle_follow_command(Json::Value messageValue, struct sockaddr_in client_address);
    // inline ServerAction *handle_send_command(Json::Value messageValue, struct sockaddr_in client_address);
    inline ServerAction *handle_login(Json::Value messageValue, struct sockaddr_in client_address);
    ServerAction *handle_incoming_datagram(Json::Value messageValue, struct sockaddr_in client_address);
};

#endif
