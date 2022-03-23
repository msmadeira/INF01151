#ifndef SERVER_MSG_H
#define SERVER_MSG_H

#include "../shared/shared.h"
#include <netdb.h>
#include <string>

struct ServerMessageMsg
{
    char username[20];
    char body[128];
};

union ServerMsgPayload
{
    char empty;
    char username[20];
    ServerMessageMsg message;
};

class ServerMessageData
{
public:
    msg_id_t msg_id;
    ServerMsgType msg_type;
    ServerMsgPayload payload;

    std::string serialize();
};

struct ServerMessage
{
public:
    sockaddr_in address;
    ServerMessageData data;
};

#endif
