#ifndef SHARED_SERVER_MSG_H
#define SHARED_SERVER_MSG_H

enum ServerMsgType
{
    LoginSuccess,
    LoginFailInvalidUsername,
    LoginFailTooManySessions,
    LoginFailAlreadyConnectedToDifferentUser,
    ServerSendCommand,
    FollowCommandFail,
    FollowCommandSuccess,
    SendCommandFail,
    SendCommandSuccess,
};

#endif
