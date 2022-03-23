#ifndef MSG_ID_H
#define MSG_ID_H

typedef int msg_id_t;

class MsgIdManager
{
private:
    msg_id_t msg_id = 0;

public:
    msg_id_t last_msg_id();
    msg_id_t next_msg_id();
};

#endif
