#ifndef SHARED_H
#define SHARED_H

#include "msg_id.h"
#include <string>

// Booleans
#define TRUE 1
#define FALSE 0

// Flags
#define NONE 0

#define ERROR_VALUE -1

#define DEFAULT_PROTOCOL 0

#define PORT_STR "4000"
#define BUFFER_SIZE 512

// Data Definitions

enum ClientMsgType
{
  Login,
  ClientMessage,
  Follow,
  RequestTweets,
  ConfirmMessageReceived
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

bool is_valid_username(std::string username);

bool is_valid_message(std::string message);

bool is_valid_ascii(std::string text);

#endif
