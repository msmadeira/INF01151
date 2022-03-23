#ifndef SERVER_MSG_H
#define SERVER_MSG_H

#include <string>

struct ServerMessageMsg
{
    char username[20];
    char body[128];
};

enum ServerMsgType
{
    LoginSuccess,
    LoginFail,
    ServerMessage,
};

union ServerMsgPayload
{
    char empty;
    ServerMessageMsg message;
};

class ServerMsg
{
public:
    int id;
    ServerMsgType msg_type;
    ServerMsgPayload payload;

    std::string serialize();
};

#endif
