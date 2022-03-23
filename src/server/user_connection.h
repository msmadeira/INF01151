#ifndef USER_CONNECTION_H
#define USER_CONNECTION_H

#include "../shared/shared.h"
#include <unordered_map>
#include "hashing.h"
#include "user_persistence.h"

struct UserConnectionData
{
public:
    user_id_t user_id;
    sockaddr_in address;
    int last_msg_id = 0;

    int next_msg_id();
};

class UserConnectionManager
{
private:
    std::unordered_map<int, UserConnectionData> user_id_to_connection_data;
    std::unordered_map<sockaddr_in, int> address_to_user_id;

public:
    bool user_id_exists(user_id_t user_id);
    bool address_exists(sockaddr_in address);
    void add_or_update_user_address(user_id_t user_id, sockaddr_in new_address);
    msg_id_t get_next_msg_id(user_id_t user_id);
    sockaddr_in get_address_from_user(user_id_t user_id);
    user_id_t get_user_id_from_address(sockaddr_in address);
};

#endif
