#include "user_persistence.h"

#include "../shared/shared.h"
#include <iostream>

using namespace std;

user_id_t UserIdManager::last_user_id()
{
    return user_id;
}

user_id_t UserIdManager::next_user_id()
{
    return ++user_id;
}

user_id_t UserPersistence::next_user_id()
{
    return this->user_id_manager.next_user_id();
}

bool UserPersistence::user_id_exists(user_id_t user_id)
{
    return this->id_to_user.find(user_id) != this->id_to_user.end();
}

user_id_t UserPersistence::add_or_update_user(string username)
{
    user_id_t user_id = get_user_id_from_username(username);
    if (user_id != 0)
    {
        return user_id;
    }

    user_id = next_user_id();
    UserPersistentData user_data{
        user_id,
        username};
    id_to_user[user_id] = user_data;
    username_to_id[username] = user_id;

    return user_id;
}

user_id_t UserPersistence::get_user_id_from_username(string username)
{
    try
    {
        return username_to_id.at(username);
    }
    catch (...)
    {
        return 0; // Not id found for this username.
    }
}

void UserPersistence::add_follow(user_id_t followed_id, user_id_t follower_id)
{
    if (!user_id_exists(followed_id))
    {
#ifdef DEBUG
        cout << "Add follow failed: followed_id does not exist." << endl
             << "Followed: " << followed_id << endl
             << "Follower: " << follower_id << endl
             << endl;
#endif
        return;
    }
    if (!user_id_exists(follower_id))
    {
#ifdef DEBUG
        cout << "Add follow failed: follower_id does not exist." << endl
             << "Followed: " << followed_id << endl
             << "Follower: " << follower_id << endl
             << endl;
#endif
        return;
    }

    vector<int> *followed_by = &(this->id_to_user[followed_id].followed_by);
    for (const user_id_t user_id : *followed_by)
    {
        if (follower_id == user_id)
        {
#ifdef DEBUG
            cout << "Add follow processed for already-following user pair." << endl
                 << "Followed: " << followed_id << endl
                 << "Follower: " << follower_id << endl
                 << endl;
#endif
            return; // Already there.
        }
    }
    followed_by->push_back(follower_id);
#ifdef DEBUG
    cout << "Add follow succeeded." << endl
         << "Followed: " << followed_id << endl
         << "Follower: " << follower_id << endl
         << endl;
#endif
}

void UserPersistence::add_sent(user_id_t user_id, string sent)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::add_sent failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << "sent: " << sent << endl
             << endl;
#endif
        return;
    }

    this->id_to_user[user_id].sent_list.push_back(sent);
}

vector<string> UserPersistence::get_sent(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::get_sent failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        vector<string> empty_vector;
        return empty_vector;
    }

    return this->id_to_user[user_id].sent_list;
}
