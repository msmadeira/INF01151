#ifndef HASHING_H
#define HASHING_H

#include <netinet/in.h>
#include <iostream>

// Necessary to use sockaddr_in in hash tables.

bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs);

template <>
struct std::hash<sockaddr_in>
{
    std::size_t operator()(sockaddr_in const &s) const noexcept
    {
        std::size_t h1 = std::hash<std::uint16_t>{}(s.sin_port);
        std::size_t h2 = std::hash<std::uint32_t>{}(s.sin_addr.s_addr);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};

#endif
