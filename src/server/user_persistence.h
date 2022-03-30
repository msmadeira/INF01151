#ifndef USER_PERSISTENCE_H
#define USER_PERSISTENCE_H

// Forward declarations due to circular dependencies.
class UserPersistence;
struct UserPersistentData;

#include <unordered_map>
#include <vector>
#include <string>
#include "server_defines.h"
#include "notification.h"
#include "disk_operations.h"

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
private:
    user_id_t user_id;
    std::string username;
    std::vector<user_id_t> followed_by;

public:
    UserPersistentData();
    UserPersistentData(user_id_t user_id, std::string username);
    UserPersistentData(user_id_t user_id, std::string username, std::vector<user_id_t> followed_by);
    user_id_t get_user_id();
    std::string get_username();
    std::vector<user_id_t> get_followed_by();
    void add_followed_by(user_id_t new_follower, DiskOperationsManagment *disk_managment);
};

struct UserData
{
private:
    UserPersistentData persistent_data;

public:
    std::vector<Notification> notification_list;
    std::vector<PendingNotification> pending_notifications;

    UserData();
    UserData(UserPersistentData persistent_data);
    UserPersistentData get_persistent_data();
    user_id_t get_user_id();
    std::string get_username();
    std::vector<user_id_t> get_followed_by();
    void add_followed_by(user_id_t new_follower, DiskOperationsManagment *disk_managment);
};

class UserPersistence
{
private:
    UserIdManager user_id_manager;
    std::unordered_map<user_id_t, UserData> id_to_user;
    std::unordered_map<std::string, user_id_t> username_to_id;
    DiskOperationsManagment *disk_managment;

    user_id_t next_user_id();

public:
    UserPersistence(std::vector<UserPersistentData> persistent_data_vector, DiskOperationsManagment *disk_managment);
    bool user_id_exists(user_id_t user_id);
    user_id_t add_or_update_user(std::string username);
    user_id_t get_user_id_from_username(std::string username);
    void add_follow(user_id_t followed_id, user_id_t follower_id);
    void add_notification(user_id_t user_id, Notification notification);
    std::vector<Notification> get_notifications(user_id_t user_id);
    std::vector<user_id_t> get_followers(user_id_t user_id);
    std::string get_username_from_user_id(user_id_t user_id);
    void add_pending_notification(user_id_t user_id, PendingNotification pending_notification);
    std::vector<PendingNotification> *get_pending_notifications(user_id_t user_id);
    std::vector<PendingNotification> drain_pending_notifications(user_id_t user_id);
};

UserPersistence *load_user_persistence(DiskOperationsManagment *disk_managment);

#endif
