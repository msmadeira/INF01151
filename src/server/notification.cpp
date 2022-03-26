#include "notification.h"
#include <string.h>

using namespace std;

// Default constructor for hash table.
Notification::Notification() : notification_id(INVALID_NOTIFICATION_ID) {}

Notification::Notification(notification_id_t notification_id,
                           user_id_t author,
                           std::time_t timestamp,
                           unsigned int message_length,
                           unsigned int pending_users,
                           string message)
    : notification_id(notification_id),
      author(author),
      timestamp(timestamp),
      message_length(message_length),
      pending_users(pending_users),
      message(message)
{
}

string *Notification::get_message()
{
    return &(this->message);
}

NotificationManager::NotificationManager(
    UserPersistence *user_persistence, UserConnectionManager *connection_manager)
    : user_persistence(user_persistence), connection_manager(connection_manager){};

notification_id_t NotificationManager::last_notification_id()
{
    return notification_id;
}

notification_id_t NotificationManager::next_notification_id()
{
    return ++notification_id;
}

bool NotificationManager::notification_exists(notification_id_t notification_id)
{
    return this->notification_id_to_notification.find(notification_id) != this->notification_id_to_notification.end();
}

void NotificationManager::add_notification(vector<ServerAction> *pending_actions,
                                           user_id_t author,
                                           time_t timestamp,
                                           vector<user_id_t> *followers,
                                           string *message)
{
    string username = this->user_persistence->get_username_from_user_id(author);

    vector<user_id_t> offline_users;

    for (const user_id_t follower : *followers)
    {
        if (!this->connection_manager->user_id_exists(follower))
        { // Follower offline.
            if (!this->user_persistence->user_id_exists(follower))
            { // Sanity check, this shouldn't happen.
#ifdef DEBUG
                cout << "NotificationManager::add_notification() failed due to invalid user_id in follower list." << endl
                     << "Followed username: " << username << endl
                     << "Followed user id:" << author << endl
                     << "Invalid follower user id: " << follower << endl
                     << endl;
#endif
                continue;
            }
            offline_users.push_back(follower);
        }
        else
        { // Follower online.
            ServerMsgPayload payload;
            strcpy(payload.message.username, username.c_str());
            strcpy(payload.message.body, message->c_str());

            ServerActionData action_data;
            action_data.message_user = MessageUserAction{
                follower,
                ServerMessageData{
                    this->connection_manager->get_next_msg_id(follower),
                    ServerMsgType::ServerSendCommand,
                    payload}};
            pending_actions->push_back(
                ServerAction{
                    ServerActionType::ActionMessageUser,
                    action_data});
        }
    }

    if (offline_users.size() > 0)
    { // Only create a notification if there's users pending to be sent.
        notification_id_t notification_id = this->next_notification_id();
        this->notification_id_to_notification[notification_id] = Notification{
            notification_id,
            author,
            timestamp,
            (unsigned int)message->size(),
            (unsigned int)offline_users.size(),
            *message};
        PendingNotification pending_notification{author, notification_id};
        for (const user_id_t follower : offline_users)
        {
            user_persistence->add_pending_notification(follower, pending_notification);
        }
    }
}

Notification *NotificationManager::get_notification(notification_id_t notification_id)
{
    if (!notification_exists(notification_id))
    {
        return NULL;
    }
    return &(this->notification_id_to_notification.at(notification_id));
}
