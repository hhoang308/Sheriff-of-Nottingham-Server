#include "Server.h"
#include "Utils.h"
#include "Log.h"
#include <iostream>
#include <cstring>

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

Server::Server() : mServerState(SERVER_WAITING), mServerSocket(0)
{
    LOG(INFO, "Server created!");
}

Server::~Server()
{
    stop();
    LOG(INFO, "Server destroyed!");
}

Game &Server::getGame(const int gameID)
{
    return *(mGames.at(gameID));
}

bool Server::init()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (mServerSocket < 0)
    {
        LOG(ERROR, "Socket creation failed");
        return false;
    }

#ifdef ENABLE_DEBUG
    /* For DEBUG: reduce TCP TIME_WAIT */
    int opt = 1;
    if (setsockopt(mServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
#endif

    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(SERVER_PORT);
    mServerAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(mServerSocket, (struct sockaddr *)&mServerAddress, sizeof(mServerAddress)) < 0)
    {
        LOG(ERROR, "Bind failed");
        close(mServerSocket);
        return false;
    }

    if (listen(mServerSocket, MAX_CLIENTS) < 0)
    {
        LOG(ERROR, "Listen failed");
        close(mServerSocket);
        return false;
    }

    LOG(INFO, "Server initialized on port %d", SERVER_PORT);
    mServerState = SERVER_READY;
    return true;
}

void Server::start()
{
    if (!init())
        return;
    std::thread(&Server::acceptClient, this).detach();

    while (mServerState == SERVER_READY)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Server::stop()
{
    mServerState = SERVER_WAITING;
    LOG(INFO, "Server is stopping");
    close(mServerSocket);

    std::lock_guard<std::mutex> lock(mClientMutex);
    for (auto &t : mClientThreads)
    {
        if (t.joinable())
            t.join();
    }
    mClientThreads.clear();
}

void Server::acceptClient()
{
    socklen_t addrlen = sizeof(mServerAddress);
    while (mServerState == SERVER_READY)
    {
        int client_socket = accept(mServerSocket, (struct sockaddr *)&mServerAddress, &addrlen);
        if (client_socket < 0)
        {
            if (mServerState == SERVER_WAITING)
                break; // Stop accepting if server state changes
            perror("Accept failed");
            continue;
        }
        /* TODO: 1 Game in 1 Server, remove when expanding sourcecode multiple games in 1 Server */
        {
            std::lock_guard<std::mutex> lock(mGamesMutex);
            if (mGames.find(GAME_ID_DEFAULT) == mGames.end())
            {
                mGames.emplace(GAME_ID_DEFAULT, std::make_unique<Game>(GAME_ID_DEFAULT));
            }
        }

        /* Move create player to handleRequest() of Game, refer to https://github.com/hhoang308/Sheriff-of-Nottingham-Server/issues/8 */
        LOG(INFO, "Have a new connection at client_socket %d!", client_socket);

        /* TODO: Change to thread pool to optimize resources */
        std::lock_guard<std::mutex> lock(mClientMutex);
        mSocketIDToGame[client_socket] = GAME_ID_DEFAULT;
        mClientThreads.emplace_back(&Server::handleClient, this, client_socket);
    }
}

void Server::handleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        int valRead = read(clientSocket, buffer, BUFFER_SIZE - 1); /* Ensure space for null-terminator */
        if (valRead < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                LOG(WARNING, "Non-blocking socket has no data to read, retrying later.");
                continue;
            }
            else if (errno == EINTR)
            {
                LOG(WARNING, "Read interrupted by signal, retrying.");
                continue;
            }
            else
            {
                LOG(ERROR, "Read failed: %s", strerror(errno));
                closeConnection(clientSocket);
                break;
            }
        }

        /* Suspicious connection : https://github.com/hhoang308/Sheriff-of-Nottingham-Server/issues/8 */
        std::string rawMessage(buffer);
        if (rawMessage.compare(0, 3, "GET") == 0 || rawMessage.compare(0, 3, "POST") == 0)
        {
            LOG(INFO, "Unknown request from client %d, close and remove it", clientSocket);
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            break;
        }

        Game *curGame = nullptr;
        {
            std::lock_guard<std::mutex> lock(mClientMutex);
            if (mSocketIDToGame.find(clientSocket) != mSocketIDToGame.end())
            {
                curGame = mGames[mSocketIDToGame[clientSocket]].get();
            }
        }

        /* Handle the situation player join but the game has started */
        if (curGame && curGame->getCurrentState() != "WaitingForPlayersState" && mSocketIDToGame.find(clientSocket) == mSocketIDToGame.end())
        {
            LOG(INFO, "Game has started, can't add playerName %d", clientSocket);
            Json::Value message;
            message["MessageType"] = "GAME_REJECT_PLAYER";
            message["Reason"] = "GAME_STARTED";
            sendToClient(jsonToString(message), clientSocket);
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            break;
        }

        /* TODO: Handle the situation when all players disconect */
        if (valRead == 0)
        {
            closeConnection(clientSocket);
            break;
        }
        buffer[valRead] = '\0'; /* null-terminate the buffer */
        LOG(INFO, "Message from socketID %d : %s", clientSocket, buffer);

        // if (send(clientSocket, buffer, valRead, 0) < 0) { // Echo message
        //     LOG(ERROR, "Send failed");
        //     break;
        // }

        if (curGame)
        {
            std::string str(buffer);
            // LOG(INFO, "Handling request");
            curGame->handleMessage(str, clientSocket);
        }
        else
        {
            LOG(ERROR, "Game not found for client!");
        }
    }
}

void Server::addSocketIDToGame(const int socketID, const int gameID)
{
    std::lock_guard<std::mutex> lock(mSocketIDToGameMutex);
    mSocketIDToGame[socketID] = gameID;
}

void Server::sendToClient(const std::string &message, const int socketID)
{
    LOG(INFO, "Sending message to player %d: %s", socketID, message.c_str());
    if (send(socketID, message.c_str(), message.size(), 0) < 0)
    {
        LOG(ERROR, "Send failed");
    }
}

void Server::sendToAll(const std::string &message)
{
    std::lock_guard<std::mutex> lock(mClientMutex);
    for (const auto &pair : mSocketIDToGame)
    {
        sendToClient(message, pair.first);
    }
}

void Server::sendToAllExcept(const std::string &message, const int socketID)
{
    std::lock_guard<std::mutex> lock(mClientMutex);
    for (const auto &pair : mSocketIDToGame)
    {
        if (pair.first != socketID)
        {
            sendToClient(message, pair.first);
        }
    }
}

void Server::closeConnection(const int socketID)
{
    LOG(INFO, "Closing connection for player %d", socketID);
    Game *curGame = nullptr;
    if (mSocketIDToGame.find(socketID) != mSocketIDToGame.end())
    {
        curGame = mGames[mSocketIDToGame[socketID]].get();
    }
    /* TODO : Find player in all game -> erase their data */
    if (curGame != nullptr)
    {
        if (curGame->getPlayerSize() == 1)
        {
            LOG(INFO, "All players disconnected, game is over!");
            mGames.erase(mSocketIDToGame[socketID]);
        }
        else if (curGame->isPlayerExists(socketID))
        {
            Json::Value message;
            message["MessageType"] = "GAME_DISCONNECT_PLAYER";
            message["PlayerName"] = curGame->getPlayer(socketID).getName();
            /* TODO : Find player in all game -> erase their data */
            LOG(INFO, "Removing playerSocketID %d from game", socketID);
            (void)curGame->removePlayer(socketID);
            mSocketIDToGame.erase(socketID);
            sendToAll(jsonToString(message));
        }
    }
    shutdown(socketID, SHUT_RDWR);
    close(socketID);
}