#pragma once

#include "byte_order.h"
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

#ifdef _WIN32
#include <ws2tcpip.h> // for inet_ntop...

#else
#include <arpa/inet.h> // for inet_ntop...
#endif

namespace ex {

struct ipv {};

struct v4 : ipv {
  using sockaddr_t = sockaddr_in;
  static inline auto domain = AF_INET;

  static bool init_ip_addr(struct sockaddr_in *addr, const char *ip, int port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = hton16((uint16_t)port);
    return inet_pton(AF_INET, ip, &addr->sin_addr) == 1;
  }

  static std::string ip_str(const struct sockaddr_in *addr) {
    char s[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, (void *)&addr->sin_addr, s, sizeof(s));
    return std::string(s);
  }
};
struct v6 : ipv {
  using sockaddr_t = sockaddr_in6;
  static inline auto domain = AF_INET6;

  static bool init_ip_addr(struct sockaddr_in6 *addr, const char *ip, int port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin6_family = AF_INET6;
    addr->sin6_port = hton16((uint16_t)port);
    return inet_pton(AF_INET6, ip, &addr->sin6_addr) == 1;
  }

  static std::string ip_str(const struct sockaddr_in6 *addr) {
    char s[INET6_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET6, (void *)&addr->sin6_addr, s, sizeof(s));
    return std::string(s);
  }
};

template <typename T = v4> struct ipaddr {
  static_assert(std::is_base_of<ipv, T>::value, "T must inherit from ipv");
  explicit ipaddr() {}
  explicit ipaddr(const std::string &ip, uint16_t port) {
    T::init_ip_addr(&sockaddr, ip.c_str(), port);
  }
  explicit ipaddr(const typename T::sockaddr_t &addr) : sockaddr(addr) {}

  std::string_view ip() const { return T::ip_str(&sockaddr); }

  uint16_t port() const { return ntoh16(sockaddr.sin_port); }
  typename T::sockaddr_t sockaddr;
};

inline bool operator==(const ex::ipaddr<ex::v4> &c1,
                       const ex::ipaddr<ex::v4> &c2) {
  return (c1.sockaddr.sin_port == c2.sockaddr.sin_port &&
          c1.sockaddr.sin_addr.s_addr == c2.sockaddr.sin_addr.s_addr);
}

inline bool operator!=(const ex::ipaddr<ex::v4> &c1,
                       const ex::ipaddr<ex::v4> &c2) {
  return (c1.sockaddr.sin_port != c2.sockaddr.sin_port ||
          c1.sockaddr.sin_addr.s_addr != c2.sockaddr.sin_addr.s_addr);
}
inline bool operator==(const ex::ipaddr<ex::v6> &c1,
                       const ex::ipaddr<ex::v6> &c2) {
  return (c1.sockaddr.sin6_port == c2.sockaddr.sin6_port &&
          c1.sockaddr.sin6_scope_id == c2.sockaddr.sin6_scope_id &&
          c1.sockaddr.sin6_flowinfo == c2.sockaddr.sin6_flowinfo &&
          memcmp(&c1.sockaddr.sin6_addr, &c2.sockaddr.sin6_addr,
                 sizeof(c1.sockaddr.sin6_addr)) == 0);
}

inline bool operator!=(const ex::ipaddr<ex::v6> &c1,
                       const ex::ipaddr<ex::v6> &c2) {
  return (c1.sockaddr.sin6_port != c2.sockaddr.sin6_port ||
          c1.sockaddr.sin6_scope_id != c2.sockaddr.sin6_scope_id ||
          c1.sockaddr.sin6_flowinfo != c2.sockaddr.sin6_flowinfo ||
          memcmp(&c1.sockaddr.sin6_addr, &c2.sockaddr.sin6_addr,
                 sizeof(c1.sockaddr.sin6_addr)) != 0);
}
} // namespace ex