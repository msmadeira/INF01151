#include "user_persistence.h"

#include "../shared/shared.h"

using namespace std;

UserIdManager::UserIdManager() {}

UserIdManager::UserIdManager(user_id_t starting_user_id)
    : user_id(starting_user_id) {}

user_id_t UserIdManager::last_user_id()
{
    return user_id;
}

user_id_t UserIdManager::next_user_id()
{
    return ++user_id;
}

UserPersistentData::UserPersistentData() {}

UserPersistentData::UserPersistentData(user_id_t user_id, std::string username)
    : user_id(user_id), username(username) {}

UserPersistentData::UserPersistentData(user_id_t user_id, std::string username, std::vector<user_id_t> followed_by)
    : user_id(user_id), username(username), followed_by(followed_by) {}

user_id_t UserPersistentData::get_user_id()
{
    return this->user_id;
}

std::string UserPersistentData::get_username()
{
    return this->username;
}
std::vector<user_id_t> UserPersistentData::get_followed_by()
{
    return this->followed_by;
}

void UserPersistentData::add_followed_by(user_id_t new_follower, DiskOperationsManagment *disk_managment)
{
    for (const user_id_t old_follower : this->followed_by)
    {
        if (old_follower == new_follower)
        {
#ifdef DEBUG
            cout << "Add follow processed for already-following user pair." << endl
                 << "Followed username: " << this->username << endl
                 << "Followed id: " << this->user_id << endl
                 << "Follower id: " << new_follower << endl
                 << endl;
#endif
            return; // Already present.
        }
    }
    this->followed_by.push_back(new_follower);
#ifdef DEBUG
    cout << "Add follow succeeded." << endl
         << "Followed username: " << this->username << endl
         << "Followed id: " << this->user_id << endl
         << "Follower id: " << new_follower << endl
         << endl;
#endif
    if (disk_managment == NULL)
    {
        return; // No persistence.
    }
    disk_managment->user_data_update_queue.push(
        UserPersistentData(
            this->user_id,
            this->username,
            this->followed_by));
}

UserData::UserData() {}

UserData::UserData(UserPersistentData persistent_data)
    : persistent_data(persistent_data) {}

UserPersistentData UserData::get_persistent_data()
{
    return this->persistent_data;
}

user_id_t UserData::get_user_id()
{
    return this->persistent_data.get_user_id();
}

std::string UserData::get_username()
{
    return this->persistent_data.get_username();
}

std::vector<user_id_t> UserData::get_followed_by()
{
    return this->persistent_data.get_followed_by();
}

void UserData::add_followed_by(user_id_t new_follower, DiskOperationsManagment *disk_managment)
{
    this->persistent_data.add_followed_by(new_follower, disk_managment);
}

user_id_t UserPersistence::next_user_id()
{
    return this->user_id_manager.next_user_id();
}

UserPersistence::UserPersistence(vector<UserPersistentData> persistent_data_vector, DiskOperationsManagment *disk_managment)
    : disk_managment(disk_managment)
{
    user_id_t biggest_user_id = INVALID_USER_ID;
    for (UserPersistentData persistent_data : persistent_data_vector)
    {
        user_id_t user_id = persistent_data.get_user_id();
        string username = persistent_data.get_username();
        this->username_to_id[username] = user_id;
        UserData user_data = UserData(persistent_data);
        this->id_to_user[user_id] = user_data;
        if (user_id > biggest_user_id)
        {
            biggest_user_id = user_id;
        }
    }
    this->user_id_manager = UserIdManager(biggest_user_id);
}

bool UserPersistence::user_id_exists(user_id_t user_id)
{
    return this->id_to_user.find(user_id) != this->id_to_user.end();
}

user_id_t UserPersistence::add_or_update_user(string username)
{
    user_id_t user_id = get_user_id_from_username(username);
    if (user_id != INVALID_USER_ID)
    {
        return user_id;
    }

    user_id = next_user_id();
    UserPersistentData persistent_data(user_id, username);
    UserData user_data(persistent_data);
    id_to_user[user_id] = user_data;
    username_to_id[username] = user_id;

    if (this->disk_managment != NULL)
    {
        this->disk_managment->user_data_update_queue.push(persistent_data);
    }

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
        return INVALID_USER_ID; // Not id found for this username.
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

    this->id_to_user[followed_id].add_followed_by(follower_id, this->disk_managment);
}

void UserPersistence::add_notification(user_id_t user_id, Notification notification)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::add_notification failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        return;
    }

    this->id_to_user[user_id].notification_list.push_back(notification);
}

vector<Notification> UserPersistence::get_notifications(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::get_sent failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        vector<Notification> empty_vector;
        return empty_vector;
    }

    return this->id_to_user[user_id].notification_list;
}

vector<user_id_t> UserPersistence::get_followers(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::get_number_of_followers failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        vector<user_id_t> empty_vector;
        return empty_vector;
    }
    return this->id_to_user[user_id].get_followed_by();
}

string UserPersistence::get_username_from_user_id(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::get_number_of_followers failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        string empty_string;
        return empty_string;
    }
    return this->id_to_user[user_id].get_username();
}

void UserPersistence::add_pending_notification(user_id_t user_id, PendingNotification pending_notification)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::add_pending_notification failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        return;
    }

    this->id_to_user[user_id].pending_notifications.push_back(pending_notification);
}

vector<PendingNotification> *UserPersistence::get_pending_notifications(user_id_t user_id)
{
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::get_pending_notifications failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif
        return NULL;
    }
    return &(this->id_to_user[user_id].pending_notifications);
}

vector<PendingNotification> UserPersistence::drain_pending_notifications(user_id_t user_id)
{
    vector<PendingNotification> drain_vector;
    if (!user_id_exists(user_id))
    {
#ifdef DEBUG
        cout << "UserPersistence::get_pending_notifications failed: user_id does not exist." << endl
             << "user_id: " << user_id << endl
             << endl;
#endif

        return drain_vector;
    }
    drain_vector = this->id_to_user[user_id].pending_notifications;
    this->id_to_user[user_id].pending_notifications.clear();
    return drain_vector;
}

UserPersistence *load_user_persistence(DiskOperationsManagment *disk_managment)
{

    vector<UserPersistentData> persistent_data_vector = disk_managment->load_and_update_persistent_data();

    return new UserPersistence(persistent_data_vector, disk_managment);
}
