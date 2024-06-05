#pragma once
#include <ex/buffer.h>
#include "ipaddr.h"
#include "socket.h"
#include <cstddef>
#include <ex/shared_buffer.h>

#ifdef _WIN32
#include <MSWSock.h>
auto const close = ::closesocket;

#else
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

namespace ex {
template <typename T = v4> class udp {
  static_assert(std::is_base_of<ipv, T>::value, "T must inherit from ipv");

#ifdef _WIN32
#define CAST_CONST_CHAR_PTR (const char *)
#define CAST_CHAR_PTR (char *)
#define CAST_SOCKLEN_T
#define CAST_SOCKLEN_T_PTR
#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

#else
#define CAST_CONST_CHAR_PTR
#define CAST_CHAR_PTR
#define CAST_SOCKLEN_T (socklen_t)
#define CAST_SOCKLEN_T_PTR (socklen_t *)

#endif

public:
  // The socket file descriptor.
  ex::socket::sock_t fd;

  // This function initializes a UDP socket.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  explicit udp(size_t recv_buffer_size = 1024)
      : m_recv_buffer(recv_buffer_size) {
    auto res = ::socket(T::domain, SOCK_DGRAM, IPPROTO_UDP);
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("socket failed.", ERRNO);
#endif
    fd = res;
  }

  // This function associates a local address with a socket.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int bind(const ipaddr<T> &ia) {
    auto res = ::bind(fd, (const sockaddr *)&ia.sockaddr, sizeof(ia.sockaddr));
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("bind failed.", ERRNO);
#endif
    return res;
  }

  // This function receives a datagram, and stores the source address.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int recvfrom() {
    m_recv_buffer_len =
        recvfrom(m_recv_buffer.data(), m_recv_buffer.size(), m_rmt_ipaddr);
    return m_recv_buffer_len;
  }

  // This function receives a datagram, and stores the source address.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int recvfrom(uint8_t *recv_buffer, size_t recv_buffer_size,
               ipaddr<T> &rmt_ipaddr) {
    auto res = ::recvfrom(fd, CAST_CHAR_PTR recv_buffer, recv_buffer_size, 0,
                          (sockaddr *)&rmt_ipaddr.sockaddr, &rmt_ipaddr.size);
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("recvfrom failed.", ERRNO);
#endif
    return res;
  }

  // This function receives a datagram, and stores the source address.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int recvfrom(ex::buffer &recv_buffer, ipaddr<T> &rmt_ipaddr) {
    return recvfrom(recv_buffer.data(), recv_buffer.size(), rmt_ipaddr);
  }

  // The sendto function sends data to a specific destination.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int sendto(char const *str, const ipaddr<T> &dst_ipaddr) {
    auto res =
        ::sendto(fd, str, strlen(str), 0, (sockaddr *)&dst_ipaddr.sockaddr,
                 sizeof(dst_ipaddr.sockaddr));
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("sendto failed.", ERRNO);
#endif
    return res;
  }

  // The sendto function sends data to a specific destination.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  template <typename U>
  int sendto(U *buf, size_t size, const ipaddr<T> &dst_ipaddr) {
    auto res =
        ::sendto(fd, CAST_CONST_CHAR_PTR buf, size, 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("sendto failed.", ERRNO);
#endif
    return res;
  }

  // The sendto function sends data to a specific destination.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  template <typename U>
  int sendto(const U &t, const ipaddr<T> &dst_ipaddr) const {
    auto res =
        ::sendto(fd, CAST_CONST_CHAR_PTR t.data(), t.size(), 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("sendto failed.", ERRNO);
#endif
    return res;
  }

  // The sendto function sends data to a specific destination.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  template <typename U> int sendto(U &&t, const ipaddr<T> &dst_ipaddr) const {
    auto res =
        ::sendto(fd, CAST_CONST_CHAR_PTR t.data(), t.size(), 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("sendto failed.", ERRNO);
#endif
    return res;
  }

  // The sendto function sends data to a specific destination.
  //
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  template <typename U>
  int sendto(std::initializer_list<U> t, const ipaddr<T> &dst_ipaddr) {
    ex::buffer v(t.size());
    v.fill(t);
    auto res =
        ::sendto(fd, CAST_CONST_CHAR_PTR v.data(), t.size(), 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("sendto failed.", ERRNO);
#endif
    return res;
  }

  // This function returns the address of the buffer which receives datagram by
  // calling `recvfrom()`.
  uint8_t *buffer() { return m_recv_buffer.data(); }

  // This function retrieves the the received datagram which is received by
  // calling `recvfrom()`.
  ex::shared_buffer recv_buffer() {
    return ex::shared_buffer(m_recv_buffer, 0, m_recv_buffer_len);
  }

  // This function packs the received datagram which is received by calling
  // `recvfrom()` to an `ex::buffer`.
  ex::buffer pack_buffer() {
    return ex::buffer::from(m_recv_buffer.data(), m_recv_buffer_len);
  }

  // The source address
  ipaddr<T> &rmt_ipaddr() { return m_rmt_ipaddr; }

  // This function sets a socket option.
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int setsockopt(int lv, int opt, const void *optval, int optlen) {
    auto res = ::setsockopt(fd, lv, opt, CAST_CONST_CHAR_PTR optval,
                            CAST_SOCKLEN_T optlen);
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("setsockopt failed.", ERRNO);
#endif
    return res;
  }

  // This function retrieves a socket option.
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int getsockopt(int lv, int opt, void *optval, int *optlen) {
    auto res = ::getsockopt(fd, lv, opt, CAST_CHAR_PTR optval,
                            CAST_SOCKLEN_T_PTR optlen);
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("getsockopt failed.", ERRNO);
#endif
    return res;
  }

  // short for
  //
  // `setsockopt(SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));`
  int set_reuseaddr(int n) {
    return setsockopt(SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
  }

  // short for
  //
  // `setsockopt(SOL_SOCKET, SO_REUSEPORT, &n, sizeof(n));`
  //
  // *NOTE: Do nothing on Windows OS.
  int set_reuseport(int n) {
#ifdef _WIN32
    return 0;
#else
    return setsockopt(SOL_SOCKET, SO_REUSEPORT, &n, sizeof(n));
#endif
  }

  // short for
  //
  // `setsockopt(SOL_SOCKET, SO_BROADCAST, &n, sizeof(n));`
  int set_broadcast(int n) {
    return setsockopt(SOL_SOCKET, SO_BROADCAST, &n, sizeof(n));
  }

  // short for
  //
  // `setsockopt(SOL_SOCKET, SO_SNDBUF, &n, sizeof(n));`
  int set_send_buffer_size(int n) {
    return setsockopt(SOL_SOCKET, SO_SNDBUF, &n, sizeof(n));
  }

  // short for
  //
  // `setsockopt(SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));`
  int set_recv_buffer_size(int n) {
    return setsockopt(SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
  }

  // short for
  //
  // `setsockopt(SOL_SOCKET, SO_RCVTIMEO, &n, sizeof(n));`
  int set_recv_timeout(int timeout_ms) {
#ifdef _WIN32
    DWORD timeout = timeout_ms;
    return setsockopt(SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
#else
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    return setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif
  }

  // This function closes an existing socket.
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int close() {
    auto res = ::close(fd);
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("close failed.", ERRNO);
#endif
    return res;
  }

  // This function disables sends or receives on a socket.
  // - 0 Shutdown receive operations.
  // - 1 Shutdown send operations.
  // - 2 Shutdown both send and receive operations.
  // If no error occurs, this function returns zero. Otherwise, it returns a
  // value of SOCKET_ERROR if c++ exception disabled, or it throws an
  // ex::socket::exception if c++ exception enabled. The specific error code can
  // be retrieved by using macro `ERRNO`.
  int shutdown(int how = SHUT_RDWR) THROWS_SOCKET_EXCEPTION {
    auto res = ::shutdown(fd, how);
#ifdef USE_SOCKET_EXCEPTION
    if (res == -1)
      throw socket::exception("shutdown failed.", ERRNO);
#endif
    return res;
  }

private:
  ex::buffer m_recv_buffer;
  int m_recv_buffer_len = 0;
  ipaddr<T> m_rmt_ipaddr;
};

} // namespace ex