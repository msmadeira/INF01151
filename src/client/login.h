#ifndef LOGIN_H
#define LOGIN_H

#include "../shared/shared.h"
#include <string>

bool try_login(ClientMsgType msg_type, std::string *username, socket_t socket_descriptor, char buffer[]);

#endif
