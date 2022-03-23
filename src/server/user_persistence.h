#ifndef USER_PERSISTENCE_H
#define USER_PERSISTENCE_H

#include <unordered_map>
#include <vector>
#include <string>

typedef int user_id_t;
#define INVALID_USER_ID 0

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
    std::vector<int> followed_by;
};

class UserPersistence
{
private:
    UserIdManager user_id_manager;
    std::unordered_map<int, UserPersistentData> id_to_user;
    std::unordered_map<std::string, int> username_to_id;

    user_id_t next_user_id();

public:
    bool user_id_exists(user_id_t user_id);
    user_id_t add_user(std::string username);
    user_id_t add_or_update_user(std::string username);
    user_id_t get_user_id_from_username(std::string username);
    void add_follow(user_id_t followed_id, user_id_t follower_id);
};

#endif
