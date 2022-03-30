#include "server_message.h"
#include "../libs/jsoncpp/json/json.h"
#include<ctime>
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

std::string ServerMessageData::serialize()
{
    Json::FastWriter fastWriter;
    Json::Value server_message;

    server_message["msg_id"] = this->msg_id;
    server_message["msg_type"] = this->msg_type;

    switch (this->msg_type)
    {
    case ServerMsgType::LoginFailInvalidUsername:
    case ServerMsgType::LoginFailTooManySessions:
    case ServerMsgType::LoginFailAlreadyConnectedToDifferentUser:
    case ServerMsgType::LoginSuccess:
    case ServerMsgType::SendCommandFail:
    case ServerMsgType::SendCommandSuccess:
    {
        break; // No payload.
    }
    case ServerMsgType::ServerSendCommand:
    {
        server_message["username"] = this->payload.message.username;
        server_message["body"] = this->payload.message.body;
        time_t t = this->payload.message.timestap;
        std::string str(std::ctime(&t));
        server_message["time"] = str;
        break;
    }

    case ServerMsgType::FollowCommandFail:
    case ServerMsgType::FollowCommandSuccess:
    {
        server_message["username"] = this->payload.username;
        break;
    }
    default:
    {
#ifdef DEBUG
        cout << "ServerMessageData::serialize() used for unimplemented ServerMsgType: " << this->msg_type << endl
             << endl;
#endif
        break;
    }
    }

    std::string json = fastWriter.write(server_message);
    return json;
}
