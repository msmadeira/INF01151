#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <ctime>
#include <vector>
#include <unordered_map>

// Forward declarations due to circular dependencies.
struct Notification;
struct PendingNotification;

#include "server_defines.h"
#include "server_action.h"
#include "user_connection.h"
#include "user_persistence.h"

struct PendingNotification
{
    user_id_t user_id;
    notification_id_t notification_id;
};

struct Notification
{
private:
    notification_id_t notification_id;
    user_id_t author;
    std::time_t timestamp;
    unsigned int message_length;
    unsigned int pending_users;
    std::string message;

public:
    Notification();
    Notification(notification_id_t notification_id,
                 user_id_t author,
                 std::time_t timestamp,
                 unsigned int message_length,
                 unsigned int pending_users,
                 std::string message);
    std::string *get_message();
};

class NotificationManager
{
private:
    UserPersistence *user_persistence;
    UserConnectionManager *connection_manager;
    notification_id_t notification_id = INVALID_NOTIFICATION_ID;
    std::unordered_map<notification_id_t, Notification> notification_id_to_notification;

    notification_id_t last_notification_id();
    notification_id_t next_notification_id();

public:
    NotificationManager(UserPersistence *user_persistence, UserConnectionManager *connection_manager);
    bool notification_exists(notification_id_t notification_id);
    void add_notification(std::vector<ServerAction> *pending_actions,
                          user_id_t author,
                          std::time_t timestamp,
                          std::vector<user_id_t> *followers,
                          std::string *message);
    Notification *get_notification(notification_id_t notification_id);
};

#endif
