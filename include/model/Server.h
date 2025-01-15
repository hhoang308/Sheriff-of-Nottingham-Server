#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define MAX_CLIENTS 6
#define BUFFER_SIZE 1024

enum ServerState {
    SERVER_WAITING,
    SERVER_READY
};

class Server {
public:
    Server();
    ~Server();

    void start();
    void stop();

private:
    bool init();
    void handleClient(int clientSocket);
    void acceptClient();

    int mServerSocket;
    sockaddr_in mServerAddress;
    std::vector<std::thread> mClientThreads;
    std::mutex mClientMutex;
    ServerState mServerState;
};

#endif // SERVER_H
