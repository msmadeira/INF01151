#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <netinet/in.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "hashing.h"

class User
{
public:
    int user_id;
    std::string username;
    int last_msg_id = 0;
    sockaddr_in *sender_address;
    sockaddr_in *receiver_address;
    std::vector<int> followed_users;
    std::vector<int> followed_by;

    int nextMsgId();
};

class UserManager
{
private:
    int last_user_id = 0;
    std::unordered_map<std::string, int> username_to_user_id;
    std::unordered_map<sockaddr_in, int> address_to_user_id;
    std::unordered_map<int, User> uid_to_user;

    int nextUserId();

public:
    bool user_id_exists(int user_id);
    bool username_exists(std::string username);
    bool address_exists(sockaddr_in address);
    int add_or_update_user_receiver_address(std::string username, sockaddr_in receiver_address);
    int add_or_update_user_sender_address(std::string username, sockaddr_in sender_address);
    int get_next_msg_id(int user_id);
    void add_follow(std::string follow_target, sockaddr_in address_source);
    User *get_user_by_user_id(int user_id);
    User *get_user_by_address(sockaddr_in address_source);
};

#endif
