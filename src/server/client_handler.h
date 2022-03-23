#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "../shared/shared.h"
#include "user_persistence.h"

#include <string>

class ClientHandler
{
private:
    user_id_t user_id;
    std::string username;
};

#endif
