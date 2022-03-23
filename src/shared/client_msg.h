#ifndef CLIENT_MSG_H
#define CLIENT_MSG_H

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

class ClientMsg
{
public:
    int id;
    ClientMsgType msg_type;
    ClientMsgPayload payload;

    ClientMsg(int id, ClientMsgType msg_type, ClientMsgPayload payload)
        : id(id), msg_type(msg_type), payload(payload)
    {
    }

    std::string serialize();
};

#endif
