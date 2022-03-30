#include "user_input.h"
#include <bits/stdc++.h>
#include <iostream>

using namespace std;

inline void print_input_instructions()
{
    cout << endl
         << "Type one of the following commands:" << endl;
    cout << "FOLLOW @username" << endl;
    cout << "SEND message" << endl;
    cout << "QUIT" << endl
         << endl;
}

void *fn_user_input(void *arg)
{
    UserInputManager *user_input_manager = static_cast<UserInputManager *>(arg);
    string username = user_input_manager->username;
    string buffer;

    for (;;)
    {
        if (user_input_manager->must_terminate.read())
        {
            pthread_exit(NULL);
        }
        print_input_instructions();

        if (!getline(cin, buffer))
        { // EoF reached.
            break;
        }
        if (user_input_manager->must_terminate.read())
        {
            pthread_exit(NULL);
        }

        stringstream buffer_stream(buffer);
        string token;

        if (!getline(buffer_stream, token, ' '))
        {
            cout << "Malformed command: " << buffer << endl;
            continue;
        }

        if (token == "QUIT")
        {
            break; // Break and clean-up.
        }

        if (token == "SEND")
        {
            if (!getline(buffer_stream, token))
            {
                cout << "Malformed command: " << buffer << endl;
                continue;
            }
            if (!is_valid_message(token))
            {
                cout << "Invalid message, cannot send: " << token << endl;
                continue;
            }

            UserInput user_input = {UserInputType::InputSend};
            strcpy(user_input.input_data.message, token.c_str());

            user_input_manager->user_command.write(user_input);
            continue;
        }

        if (token == "FOLLOW")
        {
            if (!getline(buffer_stream, token) || token[0] != '@')
            {
                cout << "Malformed command: " << buffer << endl;
                continue;
            }
            token = token.substr(1); // Trim starting @.
            if (!is_valid_username(token))
            {
                cout << "Invalid username, cannot follow: " << token << endl;
                continue;
            }
            if (token == username)
            {
                cout << "Invalid username, cannot follow oneself!" << endl;
                continue;
            }
            UserInput user_input = {UserInputType::InputFollow};
            strcpy(user_input.input_data.username, token.c_str());
            user_input_manager->user_command.write(user_input);
            continue;
        }

        cout << "Malformed command: " << buffer << endl;
        continue;
    }
    { // Signal to clean-up.
        UserInput user_input = {UserInputType::InputQuit};
        user_input_manager->user_command.write(user_input);
    }
    pthread_exit(NULL);
}
