#ifndef SERVER_ACTION_H
#define SERVER_ACTION_H

// Forward declarations due to circular dependencies.
class ServerAction;

#include "../shared/shared.h"
#include "server_message.h"
#include "user_persistence.h"

enum ServerActionType
{
    ErrorDefaultAction,
    ActionMessageUser,
    ActionMessageAddress
};

struct MessageUserAction
{
public:
    user_id_t user_id;
    ServerMessageData message;
};

struct MessageAddressAction
{
public:
    sockaddr_in address;
    ServerMessageData message;
};

union ServerActionData
{
    MessageUserAction message_user;
    MessageAddressAction message_address;
};

class ServerAction
{
public:
    ServerActionType action_type;
    ServerActionData action_data;
};

#endif
