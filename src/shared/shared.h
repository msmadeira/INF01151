#include <string>

// Booleans
#define TRUE 1
#define FALSE 0

// Flags
#define NONE 0

#define ERROR_VALUE -1

#define DEFAULT_PROTOCOL 0

#define PORT 4000
#define BUFFER_SIZE 256

// Data Definitions
enum MsgType
{
  Login,
  Follow,
  Tweet,
  RequestTweets,
  ConfirmMessageReceived
};

union ClientMsgPayload
{
  char username[20];
  char message[140];
  int last_tweet;
  int id;
};

typedef struct ClientMsg
{
  MsgType type;
  int id;
  ClientMsgPayload payload;
} clientMsg;