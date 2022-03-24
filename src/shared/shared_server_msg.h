#ifndef SHARED_SERVER_MSG_H
#define SHARED_SERVER_MSG_H

enum ServerMsgType
{
    LoginSuccess,
    LoginFail,
    ServerSendCommand,
    FollowCommandFail,
    FollowCommandSuccess,
    SendCommandFail,
    SendCommandSuccess,
};

#endif
