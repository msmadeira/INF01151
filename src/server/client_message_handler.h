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
    NotificationManager *notification_manager;

public:
    ClientMessageHandler(UserPersistence *user_persistence, UserConnectionManager *connection_manager, NotificationManager *notification_manager);
    inline void handle_follow_command(std::vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address);
    inline void handle_send_command(std::vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address);
    inline void handle_login(std::vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address);
    void handle_incoming_datagram(std::vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address);
};

#endif
