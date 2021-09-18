# C++ Socket 扩平台

## 例子
```c++
#include <cassert>
#include <iostream>
#include <ex/udp.h>

int main() {
  ex::ipaddr ia1("0.0.0.0", 8080);
  ex::ipaddr ia2("0.0.0.0", 8081);
  ex::ipaddr ia3("127.0.0.2", 8080);
  ex::ipaddr ia4("0.0.0.0", 8080);
  ex::ipaddr<ex::v6> i6a1("::1", 8080);
  ex::ipaddr<ex::v6> i6a2("::1", 8081);
  ex::ipaddr<ex::v6> i6a3("::2", 8080);
  ex::ipaddr<ex::v6> i6a4("::1", 8080);

  assert(ia1 != ia2);
  assert(ia1 != ia3);
  assert(ia1 == ia4);

  assert(i6a1 != i6a2);
  assert(i6a1 != i6a3);
  assert(i6a1 == i6a4);

  std::cout << "IP Pass!" << std::endl;
  
  ex::socket::startup();

  ex::udp u1;
  u1.bind(ia1);

  ex::udp<ex::v6> u2;
  u2.bind(i6a2);

  auto &u = u1;
  while (1) {
    u.recvfrom();
    auto rb = u.recv_buffer();
    std::cout << rb << std::endl;
    u.sendto(rb, u.rmt_ipaddr());
    u.sendto({2, 1, 1}, u.rmt_ipaddr());
    u.sendto("456", u.rmt_ipaddr());
  }

  u.close();
  u.shutdown(SHUT_RDWR);
  ex::socket::cleanup();
  return 0;
}
```