#ifndef USER_PERSISTENCE_H
#define USER_PERSISTENCE_H

#include <unordered_map>
#include <vector>
#include <string>

struct UserPersistentData
{
public:
    int user_id;
    std::string username;
    std::vector<int> followed_by;
};

class UserPersistence
{
private:
    int last_user_id = 0;
    std::unordered_map<int, UserPersistentData> id_to_user;
    std::unordered_map<std::string, int> username_to_id;

    int nextUserId();

public:
    bool user_id_exists(int user_id);
    int add_user(std::string username);
    int add_or_update_user(std::string username);
    int get_id_from_username(std::string username);
    void add_follow(int followed_id, int follower_id);
};

#endif
