/**
 *  Name: SimpleServer
 *  Author: Matt Slaney
 *  Description: A very simple multi-threaded web server written in C++
 */
#include <chrono>
#include <cstdint>
#include <exception>
#include <thread>
#include <random>

#include "SimpleWebServer.h"
#include "ThreadPool.h"

#define MAX_CONNECTIONS 5
#define PUBLIC_DIR "public"

/**
 * Simple Web Server
 */
namespace sws {

  Server::Server(const std::string &ipAddress, int port)
    : m_socket(-1), m_address({}){
    startServer(ipAddress, port);
  }

  Server::~Server() {
    stopServer();
  }

  void Server::handleRequest(int clientSocket) {
    auto threadId = std::this_thread::get_id();
    try {
      std::string path = receiveResponse(clientSocket);
      sendResponse(clientSocket, path);
    } catch (const std::exception &e) {
      std::cerr << "[" << threadId << "] "
        << "Exception handling request: " << e.what() << std::endl;
      std::string response = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/plain\n\nInternal Server Error";
      send(clientSocket, response.c_str(), response.size(), 0);
      close(clientSocket);
    }
  }

  std::string Server::receiveResponse(int clientSocket) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead < 0) {
      auto threadId = std::this_thread::get_id();
      std::cerr << "[" << threadId << "] "
        << "Could not receive data from the client" << std::endl;
      //close(clientSocket);
      return "/";
    }
    buffer[bytesRead] = '\0';

    std::string request(buffer);
    size_t pathStart  = request.find(' ') + 1;
    size_t pathEnd    = request.find(' ', pathStart);
    std::string path  = request.substr(pathStart, pathEnd - pathStart);

    auto threadId = std::this_thread::get_id();
    std::cout << "[" << threadId << "] "
      << "Client has requested path: " << path << std::endl;
    sanitisePath(path);
    return (PUBLIC_DIR + path);
  };

  void Server::sanitisePath(std::string &path) {
    // Remove leading "/.."'s from the path
    size_t startPos = 0;
    std::string from = "/..";
    std::string to = "";
    while( (startPos = path.find(from, startPos)) != std::string::npos ) {
      path.replace(startPos, from.length(), to);
      startPos += to.length();
    }
  }

  int Server::sendResponse(int clientSocket, std::string path) {

    std::string fileContents;
    int loaded = loadFile(path, fileContents);
    if (loaded < 0) {
        std::string response = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\nFile Not Found";
        send(clientSocket, response.c_str(), response.size(), 0);
        close(clientSocket);
        return 0;
    }

    // Construct the HTTP response
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " +
                           std::to_string(fileContents.size()) + "\n\n" + fileContents;

    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
    return 0;
  }

  int Server::loadFile(std::string &path, std::string &contents) {
    // Check if the requested file exists
    std::ifstream fileStream(path);
    if (!fileStream.good()) {
      return -1; //File Not Found
    }

    // Get the size of the file
    fileStream.seekg(0, std::ios::end);
    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    // Allocate memory for contentss
    contents.resize(static_cast<std::string::size_type>(fileSize));

    // Read file contents into variable contents
    if (!fileStream.read(&contents[0], fileSize)) {
        return -2; // Error while reading file
    }

    return 0;
  }

  int Server::createSocket() {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket < 0) {
      std::cerr << "Could not create socket" << std::endl;
      exit(1);
    } else {
      std::cout << "Created socket#: " << m_socket << "\n";
    }
    std::cout << std::endl;
    return 0;
  }

  int Server::bindSocket(const std::string &ipAddress, int port) {
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    m_address.sin_port = htons(static_cast<uint16_t>(port));
    if (bind(m_socket, (sockaddr*)&m_address, sizeof(m_address)) < 0) {
      std::cerr << "Could not bind to port: " << port << "\n";
      exit(1);
    }
    std::cout << std::endl;
    return 0;
  }

  void Server::run() {
    if(listen(m_socket, MAX_CONNECTIONS) < 0) {
      std::cerr << "Could not listen on port." << std::endl;
      exit(1);
    }

    ThreadPool pool(10);

    std::cout << "Server listening on port " << ntohs(m_address.sin_port) << "\n";

    std::mt19937 mt{};
    std::uniform_int_distribution<int64_t> rand1000{ 0, 1000 };

    while(true) {
      sockaddr_in clientAddress;
      socklen_t clientAddressSize = sizeof(clientAddress);
      int clientSocket = accept(m_socket, (sockaddr*)&clientAddress, &clientAddressSize);
      if(clientSocket < 0) {
        std::cerr << "Could not accept connection" << std::endl;
        exit(1);
      }

      pool.enqueue([this, &clientSocket, &mt, &rand1000](int threadNum) {
        auto threadId = std::this_thread::get_id();
 
        std::cout << "\n[" << threadId << "] "
          << "Handling request in thread # " << threadNum
          << " with Id : " << threadId
          << std::endl;

        handleRequest(clientSocket);

        // Sleep for a random duration between 0 and 1000
        // TODO: Remove this when completely satisfied with multithreading
        int64_t ms = rand1000(mt);
        std::cout << "[" << threadId << "] "
          << "Sleeping for " << ms << " milliseconds\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
      });
    }
  }

  void Server::startServer(const std::string &ipAddress, int port) {
    createSocket();
    bindSocket(ipAddress, port);
    run();
  }

  void Server::stopServer() {
    close(m_socket);

  }
}

