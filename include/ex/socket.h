#pragma once

#ifdef _WIN32
#include <exception>
#include <WinSock2.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif

#endif

namespace ex {
namespace socket {
#ifdef _WIN32
using sock_t = SOCKET;
inline WSADATA WSAData;
inline void startup() {
  auto res = WSAStartup(MAKEWORD(2, 2), &WSAData);
  if (res) {
    throw std::exception("WSAStartup failed.", res);
  }
}
const auto cleanup = WSACleanup;
#else
using sock_t = int;
inline void startup() {}
inline void cleanup() {}
#endif

} // namespace socket
} // namespace ex