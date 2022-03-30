#include "user_connection.h"

#include "../shared/shared.h"
#include <iostream>

using namespace std;

msg_id_t UserConnectionData::next_msg_id()
{
    return ++(this->last_msg_id);
}

bool UserConnectionManager::user_id_exists(user_id_t user_id)
{
    return this->user_id_to_connection_data.find(user_id) != this->user_id_to_connection_data.end();
}

bool UserConnectionManager::session_available(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
        return false;
    }
    UserConnectionData *connection_data = &(this->user_id_to_connection_data.at(user_id));
    sockaddr_in *session_one = connection_data->session_one;
    sockaddr_in *session_two = connection_data->session_two;
    return ((session_one == NULL) || (session_two == NULL));
}

bool UserConnectionManager::address_exists(sockaddr_in address)
{
    return this->address_to_user_id.find(address) != this->address_to_user_id.end();
}

void UserConnectionManager::add_or_update_user_address(user_id_t user_id, sockaddr_in new_address)
{
    if (!user_id_exists(user_id))
    { // Add whole user.
        this->user_id_to_connection_data[user_id] = UserConnectionData{
            user_id,
            new sockaddr_in(new_address),
            NULL};
        this->address_to_user_id[new_address] = user_id;
        return; // Whole user added.
    }

    UserConnectionData *connection_data = &(this->user_id_to_connection_data[user_id]);
    sockaddr_in *old_session_one = connection_data->session_one;
    sockaddr_in *old_session_two = connection_data->session_two;
    if (
        (old_session_one != NULL && *old_session_one == new_address) || (old_session_two != NULL && *old_session_two == new_address))
    {
        return; // Already up-to-date, no changes.
    }

    if (old_session_one != NULL && old_session_two != NULL)
    {
#ifdef DEBUG
        cout << "UserConnectionManager::add_or_update_user_address() called with no sessions available." << endl
             << "User ID: " << user_id << endl
             << "New address: " << new_address.sin_addr.s_addr << endl
             << endl;
#endif
        return; // Sanity check.
    }

    if (old_session_one == NULL)
    {
        connection_data->session_one = new sockaddr_in(new_address);
    }
    else
    {
        connection_data->session_two = new sockaddr_in(new_address);
    }

    this->address_to_user_id[new_address] = user_id;
}

void UserConnectionManager::remove_address(sockaddr_in address)
{
    if (!address_exists(address))
    {
#ifdef DEBUG
        cout << "UserConnectionManager::remove_address() called for non-existent address: " << address.sin_addr.s_addr << ":" << address.sin_port << endl
             << endl;
#endif
        return;
    }
    user_id_t user_id = this->address_to_user_id.at(address);
    UserConnectionData *connection_data = &(user_id_to_connection_data.at(user_id));
    if (connection_data->session_one != NULL && *(connection_data->session_one) == address)
    {
        delete (connection_data->session_one);
        connection_data->session_one = NULL;
    }
    else if (connection_data->session_two != NULL && *(connection_data->session_two) == address)
    {
        delete (connection_data->session_two);
        connection_data->session_two = NULL;
    }
    else
    {
#ifdef DEBUG
        cout << "UserConnectionManager::remove_address() called for improper address: " << address.sin_addr.s_addr << ":" << address.sin_port << endl
             << endl;
#endif
        return;
    }
    address_to_user_id.erase(address);
    if (connection_data->session_one == NULL && connection_data->session_two == NULL)
    {
        user_id_to_connection_data.erase(user_id);
    }
}

msg_id_t UserConnectionManager::get_next_msg_id(user_id_t user_id)
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

tuple<sockaddr_in *, sockaddr_in *> UserConnectionManager::get_address_from_user(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserConnectionManager::get_address_from_user() called for non-existent user_id: " << user_id << endl
             << endl;
#endif
        return make_tuple((sockaddr_in *)NULL, (sockaddr_in *)NULL);
    }

    return make_tuple(this->user_id_to_connection_data[user_id].session_one, this->user_id_to_connection_data[user_id].session_two);
}

user_id_t UserConnectionManager::get_user_id_from_address(sockaddr_in address)
{
    if (!address_exists(address))
    {
        return INVALID_USER_ID;
    }
    return this->address_to_user_id[address];
}
