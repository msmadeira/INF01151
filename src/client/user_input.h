#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <signal.h>
#include <string>
#include <netdb.h>
#include "../shared/shared.h"

enum UserInputType
{
    NoInput,
    InputFollow,
    InputSend,
    InputQuit,
};

union UserInputData
{
    char username[20];
    char message[128];
};

struct UserInput
{
    UserInputType input_type;
    UserInputData input_data;
};

struct UserInputManager
{
    std::string username;
    AtomicVar<UserInput> user_command;
    AtomicVar<bool> must_terminate{false};
};

void *fn_user_input(void *arg);

#endif
