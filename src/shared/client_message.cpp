#include "client_message.h"
#include "../libs/jsoncpp/json/json.h"

std::string ClientMessageData::serialize()
{
    Json::FastWriter fastWriter;
    Json::Value client_message;

    client_message["msg_id"] = this->msg_id;
    client_message["msg_type"] = this->msg_type;

    switch (this->msg_type)
    {
    case ClientMsgType::ClientLogout:
    {
        break; // Empty payload.
    }
    case ClientMsgType::ClientLogin:
    case ClientMsgType::ClientFollow:
    {
        client_message["username"] = this->payload.username;
        break;
    }
    case ClientMsgType::ClientSend:
    {
        client_message["send"] = this->payload.message;
        break;
    }
    }
    std::string json = fastWriter.write(client_message);
    return json;
}
