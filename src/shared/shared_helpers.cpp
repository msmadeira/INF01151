#include <string>
#include "shared_helpers.h"

bool is_valid_username(std::string username)
{
    int len = username.size();
    if (len < 4 || len > 20)
    {
        return false;
    }
    return is_valid_ascii(username);
}

bool is_valid_message(std::string message)
{
    int len = message.size();
    if (len < 1 || len > 128)
    {
        return false;
    }
    return is_valid_ascii(message);
}

bool is_valid_ascii(std::string text)
{
    for (const char c : text)
    {
        switch (c)
        {
        case 32 ... 126:
            // Valid range. No special chars.
            break;
        default:
            return false;
            break;
        }
    }
    return true;
}
