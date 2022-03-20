#include <string>
#include "shared.h"

// libs
#include "../libs/jsoncpp/json/json.h"

std::string ClientMsg::serialize()
{
    Json::FastWriter fastWriter;
    Json::Value client_message;

    client_message["id"] = this->id;
    client_message["type"] = this->msg_type;

    switch (this->msg_type)
    {
    case ClientMsgType::Login:
    case ClientMsgType::Follow:
    {
        client_message["username"] = this->payload.username;
        break;
    }
    case ClientMsgType::ClientMessage:
    {
        client_message["message"] = this->payload.message;
        break;
    }
    }
    std::string json = fastWriter.write(client_message);
    return json;
}

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

bool is_valid_username(std::string username)
{
    int len = username.size();
    if (len < 1 || len > 20)
    {
        return false;
    }
    return is_valid_ascii(username);
}

bool is_valid_message(std::string message)
{
    int len = message.size();
    if (len < 1 || len > 128)
    {
        return false;
    }
    return is_valid_ascii(message);
}

bool is_valid_ascii(std::string text)
{
    for (const char c : text)
    {
        switch (c)
        {
        case 33 ... 126:
            // Valid range. No special chars and no spaces.
            break;
        default:
            return false;
            break;
        }
    }
    return true;
}
