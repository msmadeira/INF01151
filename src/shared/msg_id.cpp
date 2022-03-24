#include "msg_id.h"

msg_id_t MsgIdManager::last_msg_id()
{
    return msg_id;
}

msg_id_t MsgIdManager::next_msg_id()
{
    return ++msg_id;
}
