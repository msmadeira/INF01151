#ifndef LOGIN_H
#define LOGIN_H

#include "../shared/shared.h"
#include "client_connection.h"
#include <string>

bool try_login(ClientMsgType msg_type, std::string *username, ConnectionManager *connection_manager, char buffer[]);

#endif
