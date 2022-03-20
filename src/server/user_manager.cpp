#include "user_manager.h"

int User::nextMsgId()
{
    return ++last_msg_id;
}

int UserManager::nextUserId()
{
    return ++last_user_id;
}

bool UserManager::user_id_exists(int user_id)
{
    return this->uid_to_user.find(user_id) != this->uid_to_user.end();
}

bool UserManager::username_exists(std::string username)
{
    return this->username_to_user_id.find(username) != this->username_to_user_id.end();
}

bool UserManager::address_exists(sockaddr_in address)
{
    return this->address_to_user_id.find(address) != this->address_to_user_id.end();
}

int UserManager::add_or_update_user(std::string username, sockaddr_in address)
{
    int user_id;
    if (!username_exists(username))
    { // Add
        user_id = nextUserId();
        this->username_to_user_id[username] = user_id;
        this->address_to_user_id[address] = user_id;
        this->uid_to_user[user_id] = {user_id, username, address, 0};
    }
    else
    { // Update
        user_id = this->username_to_user_id[username];
        this->address_to_user_id[address] = user_id;
        User user = this->uid_to_user.at(user_id);
        user.address = address;
        user.last_msg_id = 0;
    }
    return user_id;
}

int UserManager::get_next_msg_id(int user_id)
{
    return this->uid_to_user.at(user_id).nextMsgId();
}

void UserManager::add_follow(std::string follow_target, sockaddr_in address_source)
{
    if (!username_exists(follow_target))
    {
        std::cout << "Add follow failed: Username does not exist." << std::endl
                  << "Target: " << follow_target << std::endl
                  << std::endl;
        return;
    }
    if (!address_exists(address_source))
    {
        std::cout << "Add follow failed: Address does not exist." << std::endl
                  << "Target: " << follow_target << std::endl
                  << "Address: " << address_source.sin_addr.s_addr << std::endl
                  << std::endl;
        return;
    }

    int follower_user_id = this->address_to_user_id[address_source];
    int followed_user_id = this->username_to_user_id[follow_target];

    User user = this->uid_to_user.at(follower_user_id);
    std::vector<int> followed_users = user.followed_users;

    for (const int user_id : followed_users)
    {
        if (user_id == followed_user_id)
        {
            return;
        }
    };

    followed_users.push_back(followed_user_id);

    user = this->uid_to_user.at(followed_user_id);

    user.followed_by.push_back(follower_user_id);

    std::cout << "Add follow succeeded." << std::endl
              << "Target: " << follow_target << std::endl
              << "Address: " << address_source.sin_addr.s_addr << std::endl
              << std::endl;
}

User *UserManager::get_user_by_user_id(int user_id)
{
    if (!user_id_exists(user_id))
    {
        return NULL;
    }
    return &(this->uid_to_user.at(user_id));
}

User *UserManager::get_user_by_address(sockaddr_in address_source)
{
    if (!address_exists(address_source))
    {
        return NULL;
    }
    int messenger_user_id = this->address_to_user_id[address_source];
    return &(this->uid_to_user.at(messenger_user_id));
}
