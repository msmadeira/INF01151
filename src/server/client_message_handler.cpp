#include "client_message_handler.h"
#include "server_message.h"
#include <string>

using namespace std;

ClientMessageHandler::ClientMessageHandler(
    UserPersistence *user_persistence,
    UserConnectionManager *connection_manager)
    : user_persistence(user_persistence),
      connection_manager(connection_manager) {}

inline ServerAction *ClientMessageHandler::handle_follow_command(Json::Value messageValue, struct sockaddr_in client_address)
{
    user_id_t follower_user_id = connection_manager->get_user_id_from_address(client_address);
    if (follower_user_id == INVALID_USER_ID)
    {
        return NULL; // Client not properly logged, no response.
    }

    string username = messageValue["username"].asString();

    msg_id_t msg_id = connection_manager->get_next_msg_id(follower_user_id);
    ServerMsgType response_type;

    user_id_t followed_user_id = user_persistence->get_user_id_from_username(username);

    if (followed_user_id == INVALID_USER_ID)
    {
        response_type = ServerMsgType::FollowCommandFail;
    }
    else
    {
        user_persistence->add_follow(followed_user_id, follower_user_id);
        response_type = ServerMsgType::FollowCommandSuccess;
    }

    ServerMsgPayload payload;
    strcpy(payload.username, username.c_str());
    ServerActionData action_data;
    action_data.message_user = MessageUserAction{follower_user_id, ServerMessageData{msg_id, response_type, payload}};
    return new ServerAction{
        ServerActionType::ActionMessageUser,
        action_data};
}

/* TO DO
inline ServerAction *ClientMessageHandler::handle_send_command(Json::Value messageValue, struct sockaddr_in client_address)
{
}
 */

inline ServerAction *ClientMessageHandler::handle_login(Json::Value messageValue, struct sockaddr_in client_address)
{
    msg_id_t msg_id;
    string username = messageValue["username"].asString();
    ServerMsgType response_type;
    if (!is_valid_username(username))
    {
#ifdef DEBUG
        cout << "Login receiver failure: " << username << endl;
#endif
        response_type = ServerMsgType::LoginFail;
        msg_id = 0;
        ServerActionData action_data;
        action_data.message_address = MessageAddressAction{client_address, ServerMessageData{msg_id, response_type}};
        return new ServerAction{
            ServerActionType::ActionMessageAddress,
            action_data};
    }
#ifdef DEBUG
    cout << "Login receiver success: " << username << endl;
    cout << endl;
#endif
    response_type = ServerMsgType::LoginSuccess;
    user_id_t user_id = user_persistence->add_or_update_user(username);
    // DEBUG
    // cout << "add_or_update_user -> user_id: " << user_id << endl << endl;
    connection_manager->add_or_update_user_address(user_id, client_address);
    msg_id = connection_manager->get_next_msg_id(user_id);

    ServerActionData action_data;
    action_data.message_user = MessageUserAction{user_id, ServerMessageData{msg_id, response_type}};
    return new ServerAction{
        ServerActionType::ActionMessageUser,
        action_data};
}

ServerAction *ClientMessageHandler::handle_incoming_datagram(Json::Value messageValue, struct sockaddr_in client_address)
{
    int client_msg_id = messageValue["msg_id"].asInt();
    ClientMsgType client_msg_type = static_cast<ClientMsgType>(messageValue["msg_type"].asInt());

    switch (client_msg_type)
    {
    case ClientMsgType::Login:
    {
        return this->handle_login(messageValue, client_address);
    }
    case ClientMsgType::ClientSend:
    {
        return NULL; // this->handle_send_command(messageValue, client_address);
    }
    case ClientMsgType::Follow:
    {
        return this->handle_follow_command(messageValue, client_address);
    }
    default:
    {
#ifdef DEBUG
        cout << "Invalid ClientMsgType value received in handle_incoming_datagram(): " << client_msg_type << endl
             << endl;
#endif
        return NULL;
    }
    }
#ifdef DEBUG
    cout << "Received JSON: " << messageValue << endl
         << endl;
#endif

    return NULL; // DEBUG
}
