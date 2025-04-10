#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <json/json.h>
#include "Game.h"
#include "GameState.h"
#include "MessageQueue.h"
#include <atomic>

#define SERVER_PORT 8080
#define MAX_CLIENTS 1000
#define BUFFER_SIZE 1024

enum ServerState
{
    SERVER_WAITING,
    SERVER_READY
};

class Server
{
public:
    static Server &getInstance();

    void start();
    void stop();
    void sendToClientInternal(const std::string &message, const int socketID);
    void closeConnection(const int socketID);

    void addSocketIDToGame(const int socketID, const int gameID);

    void startMessageDispatcher();
    void stopDispatcher();

    void sendToClient(const std::string &message, int socketID);
    void sendToAll(const std::string &message);
    void sendToAllExcept(const std::string &message, int socketID);

private:
    Server();
    ~Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    bool init();
    void acceptClient();
    void handleClient(int clientSocket);
    Game &getGame(const int gameID);

    int mServerSocket;
    sockaddr_in mServerAddress;

    std::vector<std::thread> mClientThreads;
    std::mutex mClientMutex;

    std::unordered_map<int, std::unique_ptr<Game>> mGames;
    std::mutex mGamesMutex;

    std::unordered_map<int, int> mSocketIDToGame;
    std::mutex mSocketIDToGameMutex;

    ServerState mServerState;

    std::thread dispatcherThread;
    std::atomic<bool> running{true};
    MessageQueue messageQueue;
};

#endif // SERVER_H
