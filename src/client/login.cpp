
#include "login.h"
#include "../shared/shared.h"
#include <string>
#include <string.h>
#include "../libs/jsoncpp/json/json.h"
#include <iostream>

using namespace std;

bool try_login(ClientMsgType msg_type, string *username, ConnectionManager *connection_manager, char buffer[])
{
    msg_id_t msg_id = 0;
    ClientMsgPayload payload;
    strcpy(payload.username, (*username).c_str());

    ClientMessageData login_request{msg_id, msg_type, payload};
    string json_encoded = login_request.serialize();

    {
        cout << "Starting login..." << endl;
        connection_manager->write_from_buffer(json_encoded.c_str());

        connection_manager->read_to_buffer(buffer);
        Json::Reader reader;
        Json::Value messageValue;
        bool parseSuccess = reader.parse(buffer, messageValue, false);
        if (!parseSuccess)
        {
            cout << "ERROR parsing message while connecting: " << messageValue << endl;
            return false;
        }

        ServerMsgType server_msg_type = static_cast<ServerMsgType>(messageValue["msg_type"].asInt());
        switch (server_msg_type)
        {
        case ServerMsgType::LoginFailInvalidUsername:
        {
            cout << "Login failure: invalid username." << endl;
            return false;
            break;
        }
        case ServerMsgType::LoginFailTooManySessions:
        {
            cout << "Login failure: too many sessions currently connected, disconnect one and try again." << endl;
            return false;
            break;
        }
        case ServerMsgType::LoginFailAlreadyConnectedToDifferentUser:
        {
            cout << "Login failure: this address is already associated to an active session for a different username." << endl;
            return false;
            break;
        }
        case ServerMsgType::LoginSuccess:
        {
            cout << "Login successful." << endl;
            break;
        }
        default:
        {
            cout << "Invalid server message type received while connecting: " << server_msg_type << endl;
            return false;
            break;
        }
        }
    }

    return true;
}
