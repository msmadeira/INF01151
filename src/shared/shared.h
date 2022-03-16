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

typedef struct MsgId
{
  int id;
} msgId;

typedef struct TweetId
{
  int id;
} tweetId;

union ClientMsgPayload
{
  char username[];
  char message[];
  TweetId last_tweet;
  MsgId id;
};

typedef struct ClientMsg
{
  MsgType type;
  MsgId id;
  ClientMsgPayload payload;
} clientMsg;