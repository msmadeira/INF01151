#include "user_connection.h"

#include "../shared/shared.h"
#include <iostream>

using namespace std;

int UserConnectionData::next_msg_id()
{
    return ++(this->last_msg_id);
}

bool UserConnectionManager::user_id_exists(int user_id)
{
    return this->user_id_to_connection_data.find(user_id) != this->user_id_to_connection_data.end();
}

bool UserConnectionManager::address_exists(sockaddr_in address)
{
    return this->address_to_user_id.find(address) != this->address_to_user_id.end();
}

void UserConnectionManager::add_or_update_user_address(int user_id, sockaddr_in new_address)
{
    if (!user_id_exists(user_id))
    { // Add whole user.
        this->user_id_to_connection_data[user_id] = UserConnectionData{
            user_id,
            new_address};
        this->address_to_user_id[new_address] = user_id;
        return; // Whole user added.
    }

    UserConnectionData *connection_data = &(this->user_id_to_connection_data[user_id]);
    sockaddr_in old_address = connection_data->address;
    if (old_address == new_address)
    {
        return; // Already up-to-date, no changes.
    }

    connection_data->address = new_address;
    this->address_to_user_id[new_address] = user_id;
    this->address_to_user_id.erase(old_address);
    // Address updated.
}

int UserConnectionManager::get_next_msg_id(int user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserConnectionManager::get_next_msg_id() called for non-existent user_id: " << user_id << endl
             << endl;
#endif
        return 0;
    }

    return this->user_id_to_connection_data[user_id].next_msg_id();
}

sockaddr_in UserConnectionManager::get_address_from_user(int user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserConnectionManager::get_address_from_user() called for non-existent user_id: " << user_id << endl
             << endl;
#endif
        return sockaddr_in{};
    }

    return this->user_id_to_connection_data[user_id].address;
}

int UserConnectionManager::get_user_id_from_address(sockaddr_in address)
{
    if (!address_exists(address))
    {
        return 0;
    }
    return this->address_to_user_id[address];
}
