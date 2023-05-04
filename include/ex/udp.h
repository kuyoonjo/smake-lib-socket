#pragma once
#include "ex/buffer.h"
#include "ipaddr.h"
#include "socket.h"
#include <ex/shared_buffer.h>
#include <exception>
#include <string>

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
#define ERRNO WSAGetLastError()
#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

#else
#define CAST_CONST_CHAR_PTR
#define CAST_CHAR_PTR
#define CAST_SOCKLEN_T (socklen_t)
#define CAST_SOCKLEN_T_PTR (socklen_t *)
#define ERRNO errno

#endif

public:
  class exception : public std::exception {
  public:
    explicit exception(std::string msg, int code) : msg(msg), code(code) {}
    virtual const char *what() const throw() { return msg.c_str(); }
    std::string msg;
    int code;
  };

  explicit udp(size_t recv_buffer_size = 1024)
      : m_recv_buffer(recv_buffer_size) {
    auto res = ::socket(T::domain, SOCK_DGRAM, IPPROTO_UDP);
    if (res == -1)
      throw exception("socket failed.", ERRNO);
    m_fd = res;
  }
  void bind(const ipaddr<T> &ia) {
    auto res =
        ::bind(m_fd, (const sockaddr *)&ia.sockaddr, sizeof(ia.sockaddr));
    if (res == -1)
      throw exception("bind failed.", ERRNO);
  }

  int recvfrom() {
    auto res = ::recvfrom(
        m_fd, CAST_CHAR_PTR m_recv_buffer.data(), m_recv_buffer.size(), 0,
        (sockaddr *)&m_rmt_ipaddr.sockaddr, &m_rmt_ipaddr_len);
    if (res == -1)
      throw exception("recvfrom failed.", ERRNO);
    m_recv_buffer_len = res;
    return m_recv_buffer_len;
  }

  int sendto(char const *str, const ipaddr<T> &dst_ipaddr) {
    auto res =
        ::sendto(m_fd, str, strlen(str), 0, (sockaddr *)&dst_ipaddr.sockaddr,
                 sizeof(dst_ipaddr.sockaddr));
    if (res == -1)
      throw exception("sendto failed.", ERRNO);
    return res;
  }

  template <typename U>
  int sendto(U *buf, size_t size, const ipaddr<T> &dst_ipaddr) {
    auto res =
        ::sendto(m_fd, CAST_CONST_CHAR_PTR buf, size, 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
    if (res == -1)
      throw exception("sendto failed.", ERRNO);
    return res;
  }

  template <typename U>
  int sendto(const U &t, const ipaddr<T> &dst_ipaddr) const {
    auto res =
        ::sendto(m_fd, CAST_CONST_CHAR_PTR t.data(), t.size(), 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
    if (res == -1)
      throw exception("sendto failed.", ERRNO);
    return res;
  }

  template <typename U> int sendto(U &&t, const ipaddr<T> &dst_ipaddr) const {
    auto res =
        ::sendto(m_fd, CAST_CONST_CHAR_PTR t.data(), t.size(), 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
    if (res == -1)
      throw exception("sendto failed.", ERRNO);
    return res;
  }

  template <typename U>
  int sendto(std::initializer_list<U> t, const ipaddr<T> &dst_ipaddr) {
    ex::buffer v(t.size());
    v.fill(t);
    auto res =
        ::sendto(m_fd, CAST_CONST_CHAR_PTR v.data(), t.size(), 0,
                 (sockaddr *)&dst_ipaddr.sockaddr, sizeof(dst_ipaddr.sockaddr));
    if (res == -1)
      throw exception("sendto failed.", ERRNO);
    return res;
  }

  uint8_t* buffer() {
    return m_recv_buffer.data();
  }

  ex::shared_buffer recv_buffer() {
    return ex::shared_buffer(m_recv_buffer, 0, m_recv_buffer_len);
  }

  ex::buffer pack_buffer() {
    return ex::buffer::from(m_recv_buffer.data(), m_recv_buffer_len);
  }

  ipaddr<T> &rmt_ipaddr() { return m_rmt_ipaddr; }

  void setsockopt(int lv, int opt, const void *optval, int optlen) {
    auto res = ::setsockopt(m_fd, lv, opt, CAST_CONST_CHAR_PTR optval,
                            CAST_SOCKLEN_T optlen);
    if (res == -1)
      throw exception("setsockopt failed.", ERRNO);
  }

  void getsockopt(int lv, int opt, void *optval, int *optlen) {
    auto res = ::getsockopt(m_fd, lv, opt, CAST_CHAR_PTR optval,
                            CAST_SOCKLEN_T_PTR optlen);
    if (res == -1)
      throw exception("getsockopt failed.", ERRNO);
  }

  void set_reuseaddr(int n) {
    setsockopt(SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
  }

  void set_reuseport(int n) {
    setsockopt(SOL_SOCKET, SO_REUSEPORT, &n, sizeof(n));
  }

  void set_broadcast(int n) {

    setsockopt(SOL_SOCKET, SO_BROADCAST, &n, sizeof(n));
  }

  void set_send_buffer_size(int n) {
    setsockopt(SOL_SOCKET, SO_SNDBUF, &n, sizeof(n));
  }

  void set_recv_buffer_size(int n) {
    setsockopt(SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
  }

  void close() {
    auto res = ::close(m_fd);
    if (res == -1)
      throw exception("close failed.", ERRNO);
  }

  void shutdown(int how = SHUT_RDWR) {
    auto res = ::shutdown(m_fd, how);
    if (res == -1)
      throw exception("shutdown failed.", ERRNO);
  }

private:
  ex::buffer m_recv_buffer;
  int m_recv_buffer_len = 0;
  ex::socket::sock_t m_fd;
  ipaddr<T> m_rmt_ipaddr;
  socklen_t m_rmt_ipaddr_len = sizeof(m_rmt_ipaddr.sockaddr);
};

} // namespace ex