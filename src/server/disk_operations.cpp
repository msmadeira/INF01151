#include "disk_operations.h"

#include <iostream>

// Unix directory manipulation.
#include <sys/stat.h>

#include <fstream>
#include <cstring>

using namespace std;

UserDirectory::UserDirectory() {}

UserDirectory::UserDirectory(vector<string> usernames)
    : usernames(usernames) {}

bool UserDirectory::add_user(string new_username)
{
    for (const string old_username : this->usernames)
    {
        if (old_username == new_username)
        {
            return false;
        }
    }

    this->usernames.push_back(new_username);
    return true;
}

void guarantee_folder_exists(const char *path)
{
    struct stat buffer;
    if (stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode))
    {
        return; // Already exists.
    }
    if (mkdir(path, 0777) == -1)
    {
        cout << "Sanity check failed, directory existed, failed to create." << endl
             << "Path: " << path << endl
             << endl;
    }
}

void guarantee_user_directory_exists()
{
    guarantee_folder_exists("server_data");
    struct stat buffer;
    if (stat("./server_data/user_directory.dat", &buffer) == 0)
    {
        return; // Already exists.
    }
    // Create empty file.
    ofstream output_file_stream("./server_data/user_directory.dat");
    output_file_stream.close();
}

vector<UserPersistentData> DiskOperationsManagment::load_and_update_persistent_data()
{
    guarantee_user_directory_exists();

    vector<UserPersistentData> persistent_data_vector;

    ifstream input_file_stream;
    input_file_stream.open("./server_data/user_directory.dat", ifstream::in | ifstream::binary);
    if (!input_file_stream)
    {
        cout << "Cannot access user_directory.dat file inside server_data folder." << endl
             << "Make sure you have a folder named \"server_data\" on the same folder as the executable binary." << endl
             << endl;
        return persistent_data_vector;
    }
    if (!(input_file_stream.is_open()))
    {
        cout << "Unable to open user_directory.dat for load_and_update_persistent_data()" << endl
             << endl;
        return persistent_data_vector;
    }

    vector<string> usernames;

    {
        string username;
        while (getline(input_file_stream, username))
        {
            if (!is_valid_username(username))
            {
                continue;
            }
            usernames.push_back(username);
        }
    }

    input_file_stream.close();

    for (const string username : usernames)
    {
        string user_file = "./server_data/" + username + ".dat";
        input_file_stream.open(user_file, ifstream::in | ifstream::binary);
        if (!input_file_stream.is_open())
        {
            cout << "Error while opening " << username << ".dat file." << endl
                 << endl;
            continue;
        }

        unsigned int buffer_size = sizeof(int);

        input_file_stream.seekg(0, input_file_stream.end);
        int file_length = input_file_stream.tellg();
        input_file_stream.seekg(0, input_file_stream.beg);

        if (file_length < buffer_size * 2)
        {
            cout << "Error while reading " << username << ".dat file." << endl
                 << "File is improperly formatted, no room for headers." << endl
                 << "File size: " << file_length << endl
                 << endl;
            input_file_stream.close();
            continue;
        }

        char buffer[buffer_size];

        input_file_stream.read(buffer, buffer_size);
        user_id_t user_id;
        memcpy(&user_id, buffer, buffer_size);

        input_file_stream.read(buffer, buffer_size);
        unsigned int number_of_followers;
        memcpy(&number_of_followers, buffer, buffer_size);

        if (file_length != buffer_size * (2 + number_of_followers))
        {
            cout << "Error while reading " << username << ".dat file." << endl
                 << "File is improperly formatted, wrong file_length header." << endl
                 << "User id header: " << user_id << endl
                 << "Number of followers header: " << number_of_followers << endl
                 << "Buffer size: " << buffer_size << endl
                 << "Actual file size: " << file_length << endl
                 << endl;
            input_file_stream.close();
            continue;
        }

        vector<user_id_t> followed_by;
        for (unsigned int i = 0; i < number_of_followers; i++)
        {
            user_id_t follower;
            input_file_stream.read(buffer, buffer_size);
            memcpy(&follower, buffer, buffer_size);
            followed_by.push_back(follower);
        }

#ifdef DEBUG
        cout << "User loaded from file:" << endl
             << "user id:" << user_id << endl
             << "username:" << username << endl;
        for (user_id_t follower : followed_by)
        {
            cout << "Follower: " << follower << endl;
        }
        cout << endl;
#endif

        persistent_data_vector.push_back(
            UserPersistentData(
                user_id,
                username,
                followed_by));

        input_file_stream.close();
    }

    this->set_user_directory(usernames);

    return persistent_data_vector;
}

DiskOperationsManagment::DiskOperationsManagment() {}

void DiskOperationsManagment::set_user_directory(vector<string> usernames)
{
    this->user_directory = UserDirectory(usernames);
}

void DiskOperationsManagment::update_user_on_disk(UserPersistentData persistent_data)
{
    string username = persistent_data.get_username();
    bool user_added = this->user_directory.add_user(username);

    ofstream output_file_stream;
    {
        string username_file = "./server_data/" + username + ".dat";
        output_file_stream.open(username_file, ofstream::out | ofstream::trunc | ofstream::binary);
        if (!output_file_stream.is_open())
        {
            cout << "Failed to access " << username_file << "file to write." << endl
                 << endl;
            return;
        }
    }

    unsigned int user_id_header = persistent_data.get_user_id();
    vector<user_id_t> followers = persistent_data.get_followed_by();
    unsigned int number_of_followers = followers.size();

    unsigned int buffer_size = sizeof(user_id_header) + sizeof(number_of_followers) + (sizeof(user_id_t) * number_of_followers);
    char *buffer = new char[buffer_size]();

    unsigned int position = 0;
    unsigned int size_of_data = sizeof(user_id_header);
    char *bytes = static_cast<char *>(static_cast<void *>(&user_id_header));
    memcpy(&buffer[position], bytes, size_of_data);

    position += size_of_data;
    size_of_data = sizeof(number_of_followers);
    bytes = static_cast<char *>(static_cast<void *>(&number_of_followers));
    memcpy(&buffer[position], bytes, size_of_data);

    for (user_id_t user_id : followers)
    {
        position += size_of_data;
        size_of_data = sizeof(user_id);
        bytes = static_cast<char *>(static_cast<void *>(&user_id));
        memcpy(&buffer[position], bytes, size_of_data);
    }

#ifdef DEBUG
    cout << "User updated" << endl;
    cout << "User id: " << persistent_data.get_user_id() << endl;
    cout << "Number of followers: " << number_of_followers << endl;
    cout << "Buffer: ";
    for (int i = 0; i < buffer_size; i++)
    {
        cout << int(buffer[i]) << ' ';
    }
    cout << endl
         << endl;
#endif

    output_file_stream.write(buffer, buffer_size);
    delete buffer;
    output_file_stream.close();

    if (user_added)
    {
        ofstream output_file_stream("./server_data/user_directory.dat", ofstream::out | ofstream::binary | ofstream::app);
        if (!output_file_stream.is_open())
        {
            cout << "Failed to access user_directory.dat file to write on update_user_on_disk()." << endl
                 << endl;
            return;
        }
        output_file_stream << username << '\n';
        output_file_stream.close();
    }
}

void *fn_disk_io(void *arg)
{
    DiskOperationsManagment *disk_operations = static_cast<DiskOperationsManagment *>(arg);

    for (;;)
    {
        bool must_terminate = disk_operations->must_terminate.read();
        for (UserPersistentData persistent_data : disk_operations->user_data_update_queue.drain())
        {
            disk_operations->update_user_on_disk(persistent_data);
        }
        if (must_terminate)
        {
            break;
        }
    }

    pthread_exit(NULL);
}
