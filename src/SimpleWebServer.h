#ifndef simple_server_h
#define simple_server_h

#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

namespace sws {
  class Server {
    public:
      Server(const std::string &ipAddress, int port);
      ~Server();
      void start();
      void stop();

    private:
      std::string m_ip;
      int m_port;
      int m_socket;
      sockaddr_in m_address;
      std::atomic<bool> running;

      void handleRequest(int clientSocket);
      std::string receiveResponse(int clientSocket);
      void sanitisePath(std::string &path);
      int sendResponse(int clientSocket, std::string path);
      int loadFile(std::string &path, std::string &fileContents);
      int createSocket();
      int bindSocket(const std::string &ipAddress, int port);
      void run();
      void startServer();
      void stopServer();
  };
}

#endif
