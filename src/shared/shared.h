#ifndef SHARED_H
#define SHARED_H

#include "shared_helpers.h"
#include "client_msg.h"
#include "shared_server_msg.h"
#include "atomic.h"
#include "msg_id.h"

// Compilation flags
#define DEBUG

// Booleans
#define TRUE 1
#define FALSE 0

// Bitflags
#define NONE 0

#define ERROR_VALUE -1

#define DEFAULT_PROTOCOL 0

#define PORT_STR "4000"
#define BUFFER_SIZE 512

// Sockets
typedef int socket_t;
#define INVALID_SOCKET -1

#endif
