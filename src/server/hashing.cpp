#include "hashing.h"
#include <netinet/in.h>

bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs)
{
    return lhs.sin_port == rhs.sin_port && lhs.sin_addr.s_addr == rhs.sin_addr.s_addr;
}
