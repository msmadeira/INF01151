#ifndef DISK_OPERATIONS_H
#define DISK_OPERATIONS_H

// Forward declarations due to circular dependencies.
class DiskOperationsManagment;

#include "user_persistence.h"
#include <vector>
#include <string>

struct UserDirectory
{
private:
    std::vector<std::string> usernames;

public:
    UserDirectory();
    UserDirectory(std::vector<std::string> usernames);
    bool add_user(std::string new_username);
};

class DiskOperationsManagment
{
private:
    UserDirectory user_directory;

public:
    AtomicVar<bool> must_terminate = false;
    AtomicVecQueue<UserPersistentData> user_data_update_queue;

private:
    void set_user_directory(std::vector<std::string> usernames);

public:
    DiskOperationsManagment();
    std::vector<UserPersistentData> load_and_update_persistent_data();
    void update_user_on_disk(UserPersistentData persistent_data);
};

void *fn_disk_io(void *arg);

#endif
