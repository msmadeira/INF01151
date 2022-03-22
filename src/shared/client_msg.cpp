#include "client_msg.h"
#include <string>
#include "../libs/jsoncpp/json/json.h"

std::string ClientMsg::serialize()
{
    Json::FastWriter fastWriter;
    Json::Value client_message;

    client_message["id"] = this->id;
    client_message["type"] = this->msg_type;

    switch (this->msg_type)
    {
    case ClientMsgType::LoginSender:
    case ClientMsgType::LoginReceiver:
    case ClientMsgType::Follow:
    {
        client_message["username"] = this->payload.username;
        break;
    }
    case ClientMsgType::ClientSend:
    {
        client_message["message"] = this->payload.message;
        break;
    }
    }
    std::string json = fastWriter.write(client_message);
    return json;
}
