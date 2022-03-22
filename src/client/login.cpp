
#include "login.h"
#include "../shared/shared.h"
#include <string>
#include <string.h>
#include "client_helpers.h"
#include "../libs/jsoncpp/json/json.h"
#include <iostream>

using namespace std;

bool try_login(ClientMsgType msg_type, string *username, int socket_descriptor, char buffer[])
{
    int id = 0;
    ClientMsgPayload payload;
    strcpy(payload.username, (*username).c_str());

    ClientMsg login_request{id, msg_type, payload};
    string json_encoded = login_request.serialize();

    {
        write_from_buffer(socket_descriptor, json_encoded.c_str());

        read_to_buffer(socket_descriptor, buffer);
        Json::Reader reader;
        Json::Value messageValue;
        bool parseSuccess = reader.parse(buffer, messageValue, false);
        if (!parseSuccess)
        {
            cout << "ERROR parsing message while connecting: " << messageValue << endl;
            return false;
        }

        ServerMsgType server_msg_type = static_cast<ServerMsgType>(messageValue["type"].asInt());
        switch (server_msg_type)
        {
        case ServerMsgType::LoginFail:
        {
            cout << "Login failure for system " << msg_type << endl;
            return false;
            break;
        }
        case ServerMsgType::LoginSuccess:
        {
            cout << "Login successful for system " << msg_type << endl;
            break;
        }
        default:
        {
            cout << "Invalid server message type received while connecting: " << server_msg_type << endl;
            return false;
            break;
        }
        }
    } // TO DO: Make this block non-blocking and repeating.

    return true;
}
