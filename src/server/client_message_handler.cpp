#include "client_message_handler.h"
#include "server_message.h"
#include <string>

using namespace std;

ClientMessageHandler::ClientMessageHandler(
    UserPersistence *user_persistence,
    UserConnectionManager *connection_manager,
    NotificationManager *notification_manager)
    : user_persistence(user_persistence),
      connection_manager(connection_manager),
      notification_manager(notification_manager) {}

inline void ClientMessageHandler::handle_follow_command(vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address)
{
    user_id_t follower_user_id = connection_manager->get_user_id_from_address(client_address);
    if (follower_user_id == INVALID_USER_ID)
    {
        return; // Client not properly logged, no response.
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
    pending_actions->push_back(ServerAction{
        ServerActionType::ActionMessageUser,
        action_data});
}

inline void ClientMessageHandler::handle_send_command(vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address)
{
    user_id_t user_id = connection_manager->get_user_id_from_address(client_address);
    if (user_id == INVALID_USER_ID)
    {
#ifdef DEBUG
        cout << "ClientMessageHandler::handle_send_command processed for unlogged or disconnected client." << endl
             << endl;
#endif
        return; // Client not properly logged, no response.
    }

    string sent = messageValue["send"].asString();

    msg_id_t msg_id = connection_manager->get_next_msg_id(user_id);
    ServerMsgType response_type;

    if (!is_valid_message(sent))
    {
        response_type = ServerMsgType::SendCommandFail;
#ifdef DEBUG
        cout << "ClientMessageHandler::handle_send_command processed invalid message:" << sent << endl
             << "User id: " << user_id << endl
             << endl;
#endif
    }
    else
    {
        {
            vector<user_id_t> followers = user_persistence->get_followers(user_id);
            if (followers.size() > 0)
            {
                notification_manager->add_notification(
                    pending_actions,
                    user_id,
                    time(nullptr),
                    &followers,
                    &sent);
                cout << "ClientMessageHandler::handle_send_command processed valid message:" << sent << endl
                     << "User id: " << user_id << endl
                     << "Number of followers: " << followers.size() << endl
                     << endl;
            }
#ifdef DEBUG
            else
            {
                cout << "ClientMessageHandler::handle_send_command processed valid message, but for no followers:" << sent << endl
                     << "User id: " << user_id << endl
                     << endl;
            }
#endif
        }
        response_type = ServerMsgType::SendCommandSuccess;
    }

    ServerActionData action_data;
    action_data.message_user = MessageUserAction{user_id, ServerMessageData{msg_id, response_type}};

    pending_actions->push_back(ServerAction{
        ServerActionType::ActionMessageUser,
        action_data});
}

inline void ClientMessageHandler::handle_login(vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address)
{
    msg_id_t msg_id = 0;
    string username = messageValue["username"].asString();
    ServerMsgType response_type;
    if (!is_valid_username(username))
    {
        response_type = ServerMsgType::LoginFailInvalidUsername;
        msg_id = 0;
        ServerActionData action_data;
        action_data.message_address = MessageAddressAction{client_address, ServerMessageData{msg_id, response_type}};
        pending_actions->push_back(ServerAction{
            ServerActionType::ActionMessageAddress,
            action_data});
#ifdef DEBUG
        cout << "Login receiver failure, invalid username: " << username << endl;
#endif
        return;
    }
    user_id_t user_id = user_persistence->add_or_update_user(username);

    bool had_previous_session = connection_manager->user_id_exists(user_id);
    if (had_previous_session && !connection_manager->session_available(user_id))
    {
        response_type = ServerMsgType::LoginFailTooManySessions;
        ServerActionData action_data;
        action_data.message_address = MessageAddressAction{client_address, ServerMessageData{msg_id, response_type}};
        pending_actions->push_back(ServerAction{
            ServerActionType::ActionMessageAddress,
            action_data});
#ifdef DEBUG
        cout << "Login receiver failure, too many connected sessions: " << username << endl;
#endif
        return;
    }
    else
    {
        user_id_t address_linked_user_id = connection_manager->get_user_id_from_address(client_address);
        if (address_linked_user_id != INVALID_USER_ID && address_linked_user_id != user_id)
        {
            response_type = ServerMsgType::LoginFailAlreadyConnectedToDifferentUser;
            ServerActionData action_data;
            action_data.message_address = MessageAddressAction{client_address, ServerMessageData{msg_id, response_type}};
            pending_actions->push_back(ServerAction{
                ServerActionType::ActionMessageAddress,
                action_data});
#ifdef DEBUG
            cout << "Login receiver failure, address already associated to a different user: " << username << endl;
#endif
            return;
        }
    }

#ifdef DEBUG
    cout << "Login receiver success: " << username << endl;
    cout << endl;
#endif
    response_type = ServerMsgType::LoginSuccess;

    connection_manager->add_or_update_user_address(user_id, client_address);
    msg_id = connection_manager->get_next_msg_id(user_id);

    ServerActionData action_data;
    action_data.message_address = MessageAddressAction{client_address, ServerMessageData{msg_id, response_type}};
    pending_actions->push_back(ServerAction{
        ServerActionType::ActionMessageAddress,
        action_data});

    if (!had_previous_session)
    {
        for (const PendingNotification pending_notification : user_persistence->drain_pending_notifications(user_id))
        {
            string author = user_persistence->get_username_from_user_id(pending_notification.user_id);
            Notification *notification = notification_manager->get_notification(pending_notification.notification_id);
            if (notification == NULL)
            {
#ifdef DEBUG
                cout << "Pending notification improperly cleaned." << endl
                     << "Notification id: " << pending_notification.notification_id << endl
                     << "Author: " << author << " (id: " << pending_notification.user_id << " )" << endl
                     << "Receiver: " << username << " (id: " << user_id << " )" << endl;
                cout << endl;
#endif
                continue;
            }
            { // Send notification.
                ServerMsgPayload payload;
                strcpy(payload.message.username, author.c_str());
                strcpy(payload.message.body, notification->get_message()->c_str());

                ServerActionData action_data;
                action_data.message_user = MessageUserAction{
                    user_id,
                    ServerMessageData{
                        this->connection_manager->get_next_msg_id(user_id),
                        ServerMsgType::ServerSendCommand,
                        payload}};
                pending_actions->push_back(
                    ServerAction{
                        ServerActionType::ActionMessageUser,
                        action_data});
            }
            { // Decrement notification.
                notification_manager->decrement_pending_users_from(pending_notification.notification_id);
            }
        }
    }
}

inline void ClientMessageHandler::handle_logout(vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address)
{
    if (!connection_manager->address_exists(client_address))
    {

#ifdef DEBUG
        cout << "Logout failed, unknown address: " << client_address.sin_addr.s_addr << ":" << client_address.sin_port << endl;
        cout << endl;
#endif
        return;
    }
    connection_manager->remove_address(client_address);
#ifdef DEBUG
    cout << "Logout receiver success: " << client_address.sin_addr.s_addr << ":" << client_address.sin_port << endl;
    cout << endl;
#endif
}

void ClientMessageHandler::handle_incoming_datagram(vector<ServerAction> *pending_actions, Json::Value messageValue, struct sockaddr_in client_address)
{
#ifdef DEBUG
    cout << "Received JSON: " << messageValue << endl
         << endl;
#endif

    int client_msg_id = messageValue["msg_id"].asInt();
    ClientMsgType client_msg_type = static_cast<ClientMsgType>(messageValue["msg_type"].asInt());

    switch (client_msg_type)
    {
    case ClientMsgType::ClientLogin:
    {
        this->handle_login(pending_actions, messageValue, client_address);
        break;
    }
    case ClientMsgType::ClientLogout:
    {
        this->handle_logout(pending_actions, messageValue, client_address);
        break;
    }
    case ClientMsgType::ClientSend:
    {
        this->handle_send_command(pending_actions, messageValue, client_address);
        break;
    }
    case ClientMsgType::ClientFollow:
    {
        this->handle_follow_command(pending_actions, messageValue, client_address);
        break;
    }
    default:
    {
#ifdef DEBUG
        cout << "Invalid ClientMsgType value received in handle_incoming_datagram(): " << client_msg_type << endl
             << endl;
#endif
        break;
    }
    }
}
