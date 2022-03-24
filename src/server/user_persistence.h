#ifndef USER_PERSISTENCE_H
#define USER_PERSISTENCE_H

// Forward declarations due to circular dependencies.
class UserPersistence;

#include <unordered_map>
#include <vector>
#include <string>
#include "server_defines.h"
#include "notification.h"

class UserIdManager
{
private:
    user_id_t user_id = INVALID_USER_ID;

public:
    user_id_t last_user_id();
    user_id_t next_user_id();
};

struct UserPersistentData
{
public:
    user_id_t user_id;
    std::string username;
    std::vector<user_id_t> followed_by;
    std::vector<Notification> notification_list;
    std::vector<PendingNotification> pending_notifications;
};

class UserPersistence
{
private:
    UserIdManager user_id_manager;
    std::unordered_map<user_id_t, UserPersistentData> id_to_user;
    std::unordered_map<std::string, user_id_t> username_to_id;

    user_id_t next_user_id();

public:
    bool user_id_exists(user_id_t user_id);
    user_id_t add_user(std::string username);
    user_id_t add_or_update_user(std::string username);
    user_id_t get_user_id_from_username(std::string username);
    void add_follow(user_id_t followed_id, user_id_t follower_id);
    void add_notification(user_id_t user_id, Notification notification);
    std::vector<Notification> get_notifications(user_id_t user_id);
    std::vector<user_id_t> get_followers(user_id_t user_id);
    std::string get_username_from_user_id(user_id_t user_id);
    void add_pending_notification(user_id_t user_id, PendingNotification pending_notification);
    std::vector<PendingNotification> get_pending_notifications(user_id_t user_id);
};

#endif
