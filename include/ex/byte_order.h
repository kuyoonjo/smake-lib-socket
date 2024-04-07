#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define htonll(x) __builtin_bswap64(x)
#define ntohll(x) __builtin_bswap64(x)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define htonll(x) x
#define ntohll(x) x
#else
#error "Unsupported endian"
#endif
#endif

#define htobe16 htons
#define htobe32 htonl
#define htobe64 htonll
#define be16toh ntohs
#define be32toh ntohl
#define be64toh ntohll

#else /* linux, mac */

#if defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define htobe16 OSSwapHostToBigInt16
#define htobe32 OSSwapHostToBigInt32
#define htobe64 OSSwapHostToBigInt64
#define be16toh OSSwapBigToHostInt16
#define be32toh OSSwapBigToHostInt32
#define be64toh OSSwapBigToHostInt64

#elif defined(__linux__)
#include <endian.h>
#else
#include <sys/endian.h>
#endif

#endif

namespace ex {
inline uint16_t hton16(uint16_t v) {
    return htobe16(v);
}

inline uint32_t hton32(uint32_t v) {
    return htobe32(v);
}

inline uint64_t hton64(uint64_t v) {
    return htobe64(v);
}

inline uint16_t ntoh16(uint16_t v) {
    return be16toh(v);
}

inline uint32_t ntoh32(uint32_t v) {
    return be32toh(v);
}

inline uint64_t ntoh64(uint64_t v) {
    return be64toh(v);
}
} // namespace ex