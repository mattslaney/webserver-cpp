#ifndef simple_server_h
#define simple_server_h

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

    private:
      int m_socket;
      sockaddr_in m_address;

      void handleRequest(int clientSocket);
      std::string receiveResponse(int clientSocket);
      void sanitisePath(std::string &path);
      int sendResponse(int clientSocket, std::string path);
      int loadFile(std::string &path, std::string &fileContents);
      int createSocket();
      int bindSocket(const std::string &ipAddress, int port);
      void run();
      void startServer(const std::string &ipAddress, int port);
      void stopServer();
  };
}

#endif
