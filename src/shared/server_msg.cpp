#include "server_msg.h"
#include <string>
#include "../libs/jsoncpp/json/json.h"

std::string ServerMsg::serialize()
{
    Json::FastWriter fastWriter;
    Json::Value server_message;

    server_message["id"] = this->id;
    server_message["type"] = this->msg_type;

    switch (this->msg_type)
    {
    case ServerMsgType::ServerMessage:
    {
        server_message["username"] = this->payload.message.username;
        server_message["body"] = this->payload.message.body;
        break;
    }
    }

    std::string json = fastWriter.write(server_message);
    return json;
}
