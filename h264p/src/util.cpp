#include "util.h"

/// @brief Convert 64 bit integer from network to host order.
/// @param n [in] The 64 bit integer in network order.
/// @return The 64 bit integer in host order.
uint64_t ThetaStream::ntohll(uint64_t n)
{
    uint64_t x = n;
    x = (x >> 56) |
        ((x << 40) & 0x00FF000000000000) |
        ((x << 24) & 0x0000FF0000000000) |
        ((x << 8) & 0x000000FF00000000) |
        ((x >> 8) & 0x00000000FF000000) |
        ((x >> 24) & 0x0000000000FF0000) |
        ((x >> 40) & 0x000000000000FF00) |
        (x << 56);
    return x;
}
