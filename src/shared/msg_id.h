#ifndef MSG_ID_H
#define MSG_ID_H

class MsgIdManager
{
private:
    int last_id{0};

public:
    MsgIdManager() {}

    int lastId()
    {
        return last_id;
    }

    int nextId()
    {
        return ++last_id;
    }
};

#endif
