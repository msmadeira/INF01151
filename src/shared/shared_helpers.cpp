#include <string>
#include "shared_helpers.h"

bool is_valid_username(std::string username)
{
    int len = username.size();
    bool allow_space = false;
    if (len < 4 || len > 20)
    {
        return false;
    }
    return is_valid_ascii(username, allow_space);
}

bool is_valid_message(std::string message)
{
    int len = message.size();
    bool allow_space = true;

    if (len < 1 || len > 128)
    {
        return false;
    }
    return is_valid_ascii(message, allow_space);
}

bool is_valid_ascii(std::string text, bool allow_space)
{
    for (const char c : text)
    {
        switch (c)
        {
        case 32:
            if (!allow_space)
            {
                return false;
            }
            break;
        case 33 ... 126:
            // Valid range. No special chars .
            break;
        default:
            return false;
            break;
        }
    }
    return true;
}
