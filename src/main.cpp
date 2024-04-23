#include <iostream>

#include "SimpleWebServer.h"

int main() {
  using namespace sws;

  std::cout << "Starting Simple Web Server" << std::endl;

  Server server = Server("127.0.0.1", 8080);
  server.start();

  return 0;
}
