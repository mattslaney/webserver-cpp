#include <chrono>
#include <exception>
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <thread>

#include "../src/SimpleWebServer.h"

void testServerResponse() {
  std::cout << "starting testServerResponse\n";
  try {
    // Create a client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    assert(clientSocket != -1);

    // Connect to the server
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    int connResult = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    assert(connResult != -1);

    // Send an HTTP request
    const char* httpRequest = "GET /index.html HTTP/1.1\r\n\r\n";
    ssize_t sendResult = send(clientSocket, httpRequest, strlen(httpRequest), 0);
    assert(sendResult != -1);

    // Receive the server's response
    char responseBuffer[1024];
    ssize_t recvResult = recv(clientSocket, responseBuffer, sizeof(responseBuffer), 0);
    assert(recvResult != -1);

    // Ensure that the response contains "HTTP/1.1 200 OK"
    std::string response(responseBuffer, static_cast<size_t>(recvResult));
    assert(response.find("HTTP/1.1 200 OK") != std::string::npos);

    // Close the client socket
    close(clientSocket);

    assert(1 == 2);
  } catch (const std::exception &e) {
    std::cerr << "An exception occured in the test: " << e.what() << "\n";
  }

  std::cout << "[PASSED] testServerResponse\n";
}

int main() {
  using namespace sws;

  sws::Server server("127.0.0.1", 8080);
  std::thread serverThread([&server]() {
    server.start();
  });
  serverThread.detach();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "=== STARTING TESTS ===\n";
  testServerResponse();

  std::cout << "=== FINISHED TESTS ===\n";
  std::cout << std::endl;

  server.stop();
  return 0;
}
