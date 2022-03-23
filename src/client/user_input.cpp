#include "user_input.h"
#include <iostream>
#include <string.h>

using namespace std;

void *fn_user_input(void *arg)
{
    UserInputManager *user_input_manager = static_cast<UserInputManager *>(arg);
    char buffer[BUFFER_SIZE];

    for (;;)
    {
        cout << endl
             << "Type one of the following commands:" << endl;
        cout << "FOLLOW @username" << endl;
        cout << "SEND message" << endl;
        cout << "QUIT" << endl
             << endl;

        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);

        if (strlen(buffer) < 4)
        {
            cout << "Malformed command type 1: " << buffer << endl;
            continue;
        }

        string buffer_string(buffer);
        buffer_string.pop_back(); // Remove new line.
        string command = buffer_string.substr(0, 4);

        if (!command.compare("QUIT"))
        {
            UserInput user_input = {UserInputType::InputQuit};
            user_input_manager->user_command.write(user_input);
            pthread_exit(NULL);
        }

        if (buffer_string.size() < 6)
        {
            cout << "Malformed command type 2: " << buffer_string << endl;
            continue;
        }

        if (!command.compare("SEND"))
        {
            if (buffer_string[4] != ' ')
            {
                cout << "Malformed command type 3: " << buffer_string << endl;
                continue;
            }
            string message = buffer_string.substr(5);
            if (!is_valid_message(message))
            {
                cout << "Invalid message type 4: " << message << endl;
                continue;
            }

            UserInput user_input = {UserInputType::InputSend};
            strcpy(user_input.input_data.message, message.c_str());

            user_input_manager->user_command.write(user_input);
            continue;
        }

        if (buffer_string.size() < 9)
        {
            cout << "Malformed command type 5: " << buffer_string << endl;
            continue;
        }

        command = buffer_string.substr(0, 8);
        if (!command.compare("FOLLOW @"))
        {
            string follow_username = buffer_string.substr(8);
            if (!is_valid_username(follow_username))
            {
                cout << "Invalid username: " << follow_username << endl;
                continue;
            }
            UserInput user_input = {UserInputType::InputFollow};
            strcpy(user_input.input_data.username, follow_username.c_str());
            user_input_manager->user_command.write(user_input);
            continue;
        }

        cout << "Malformed command type 6: " << buffer_string << endl;
        continue;
    }
}
