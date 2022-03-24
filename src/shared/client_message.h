#ifndef CLIENT_MSG_H
#define CLIENT_MSG_H

#include "msg_id.h"
#include <string>

enum ClientMsgType
{
    Login,
    ClientSend,
    Follow,
};

union ClientMsgPayload
{
    char username[20];
    char message[128];
};

class ClientMessageData
{
public:
    msg_id_t msg_id;
    ClientMsgType msg_type;
    ClientMsgPayload payload;

    ClientMessageData(msg_id_t msg_id, ClientMsgType msg_type, ClientMsgPayload payload)
        : msg_id(msg_id), msg_type(msg_type), payload(payload)
    {
    }

    std::string serialize();
};

#endif
