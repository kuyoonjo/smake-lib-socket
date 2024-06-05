#pragma once

#if defined(__cpp_exceptions) && !defined(DISABLE_SOCKET_EXCEPTION)
#define USE_SOCKET_EXCEPTION
#include <exception>
#include <stdexcept>
#include <string>
#endif

#ifdef _WIN32
#include <WinSock2.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif

#define ERRNO WSAGetLastError()
#else
#define ERRNO errno
#endif

#ifdef USE_SOCKET_EXCEPTION
#define THROWS_SOCKET_EXCEPTION
#else
#define THROWS_SOCKET_EXCEPTION noexcept
#endif

namespace ex {
namespace socket {
#ifdef USE_SOCKET_EXCEPTION
class exception : public std::runtime_error {
public:
  explicit exception(const char *msg, int code)
      : std::runtime_error(msg), code(code) {}
  int code;
};
#endif

#ifdef _WIN32
using sock_t = SOCKET;
inline WSADATA WSAData;

// This function initiates use of the Winsock DLL by a process.
//   - This function does nothing on unix-like system.
//   - If no error occurs, this function returns zero. Otherwise, it returns a
//   value of SOCKET_ERROR if c++ exception disabled, or it throws an
//   ex::socket::exception if c++ exception enabled. The specific error code can
//   be retrieved by using macro `ERRNO`.
// https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup
inline int startup() {
  auto res = WSAStartup(MAKEWORD(2, 2), &WSAData);
#ifdef USE_SOCKET_EXCEPTION
  if (res)
    throw exception("WSAStartup failed.", res);
#endif
  return res;
}

// This function terminates use of the Winsock 2 DLL (Ws2_32.dll).
//   - This function does nothing on unix-like system.
//   - If no error occurs, this function returns zero. Otherwise, it returns a
//   value of SOCKET_ERROR if c++ exception disabled, or it throws an
//   ex::socket::exception if c++ exception enabled. The specific error code can
//   be retrieved by using macro `ERRNO`.
// https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
inline int cleanup() {
  auto res = WSACleanup();
#ifdef USE_SOCKET_EXCEPTION
  if (res)
    throw exception("WSACleanup failed.", res);
#endif
  return res;
}
#else
using sock_t = int;
// This function initiates use of the Winsock DLL by a process.
//   - This function does nothing on unix-like system.
//   - If no error occurs, this function returns zero. Otherwise, it returns a
//   value of SOCKET_ERROR if c++ exception disabled, or it throws an
//   ex::socket::exception if c++ exception enabled. The specific error code can
//   be retrieved by using macro `ERRNO`.
// https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup
constexpr int startup() THROWS_SOCKET_EXCEPTION { return 0; }

// This function terminates use of the Winsock 2 DLL (Ws2_32.dll).
//   - This function does nothing on unix-like system.
//   - If no error occurs, this function returns zero. Otherwise, it returns a
//   value of SOCKET_ERROR if c++ exception disabled, or it throws an
//   ex::socket::exception if c++ exception enabled. The specific error code can
//   be retrieved by using macro `ERRNO`.
// https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
constexpr int cleanup() THROWS_SOCKET_EXCEPTION { return 0; }
#endif

} // namespace socket
} // namespace ex