#include "ex/socket.h"
#include "ex/buffer.h"
#include "ex/ipaddr.h"
#include <cassert>
#include <cstdio>
#include <ex/udp.h>
#include <iostream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

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

  std::vector<std::thread> threads;
  for (int i = 0; i < 8; ++i) {
    threads.push_back(std::thread([i, ia1] {
      ex::udp u;
      u.set_reuseaddr(1);
      u.set_reuseport(1);
      u.set_recv_timeout(3000);
      u.bind(ia1);
      std::cout << "worker " << i << " started." << std::endl;
      ex::buffer buffer(1024);
      ex::ipaddr<> ipaddr;
      for (;;) {
        try {
          auto n = u.recvfrom(buffer.data(), buffer.size(), ipaddr);
          std::cout << "worker " << i << " recv " << n << " bytes: ";
          for (int j = 0; j < n; ++j)
            printf("%02x ", buffer[j]);
          std::cout << std::endl;
        } catch (ex::socket::exception &e) {
          std::cout << e.code << ": " << e.msg << std::endl;
        }
      }

      u.close();
      u.shutdown(SHUT_RDWR);
    }));
  }
  for (auto &th : threads) {
    th.join();
  }

  ex::socket::cleanup();
  return 0;
}