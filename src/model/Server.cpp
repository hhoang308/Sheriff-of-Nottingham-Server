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
        Game *curGame = nullptr;
        /* TODO: 1 Game in 1 Server, remove when expanding sourcecode multiple games in 1 Server */
        {
            std::lock_guard<std::mutex> lock(mGamesMutex);
            if (mGames.find(GAME_ID_DEFAULT) == mGames.end())
            {
                mGames.emplace(GAME_ID_DEFAULT, std::make_shared<Game>(GAME_ID_DEFAULT));
            }
            curGame = mGames[GAME_ID_DEFAULT].get();
        }

        /* Move create player to handleRequest() of Game, refer to https://github.com/hhoang308/Sheriff-of-Nottingham-Server/issues/8 */
        LOG(INFO, "Have a new connection at client_socket %d!", client_socket);

        /* Check if the game has started */
        if (curGame->getCurrentState() != "WaitingForPlayersState")
        {
            LOG(INFO, "Game has started, can't add player from socket %d", client_socket);
            Json::Value message;
            message["MessageType"] = "GAME_REJECT_PLAYER";
            message["Reason"] = "GAME_STARTED";
            sendToClient(jsonToString(message), client_socket);
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
            continue; // KHÔNG vào handleClient nữa
        }

        /* Set socket to non-blocking */
        {
            std::lock_guard<std::mutex> lock(mClientMutex);
            mSocketIDToGame[client_socket] = GAME_ID_DEFAULT;
            mClientRunningFlags[client_socket] = true;
            mClientThreads.emplace_back(&Server::handleClient, this, client_socket);
        }
    }
}

void Server::handleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(mClientFlagMutex);
            if (!mClientRunningFlags[clientSocket])
            {
                LOG(INFO, "Thread for client %d is asked to exit.", clientSocket);
                break;
            }
        }

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

        std::shared_ptr<Game> curGame;
        {
            std::lock_guard<std::mutex> lock(mClientMutex);
            auto it = mSocketIDToGame.find(clientSocket);
            if (it != mSocketIDToGame.end())
            {
                std::lock_guard<std::mutex> lock2(mGamesMutex);
                auto gameIt = mGames.find(it->second);
                if (gameIt != mGames.end())
                {
                    curGame = gameIt->second;
                }
            }
        }

        // /* Handle the situation player join but the game has started */
        // if (curGame && curGame->getCurrentState() != "WaitingForPlayersState" && mSocketIDToGame.find(clientSocket) == mSocketIDToGame.end())
        // {
        //     LOG(INFO, "Game has started, can't add playerName %d", clientSocket);
        //     Json::Value message;
        //     message["MessageType"] = "GAME_REJECT_PLAYER";
        //     message["Reason"] = "GAME_STARTED";
        //     sendToClient(jsonToString(message), clientSocket);
        //     shutdown(clientSocket, SHUT_RDWR);
        //     close(clientSocket);
        //     break;
        // }

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

void Server::finish(const int gameID)
{
    std::lock_guard<std::mutex> lock(mGamesMutex);
    if (mGames.find(gameID) == mGames.end())
    {
        LOG(ERROR, "Game with ID %d not found", gameID);
        return;
    }
    Game *curGame = mGames[gameID].get();

    LOG(INFO, "Finishing game with ID %d", gameID);
    {
        std::lock_guard<std::mutex> clientLock(mClientMutex);
        for (auto it = mSocketIDToGame.begin(); it != mSocketIDToGame.end();)
        {
            if (it->second == gameID)
            {
                LOG(INFO, "Removing socketID %d from game %d", it->first, gameID);
                {
                    std::lock_guard<std::mutex> lock(mClientFlagMutex);
                    if (mClientRunningFlags.find(it->first) != mClientRunningFlags.end())
                    {
                        mClientRunningFlags[it->first] = false;
                    }
                }
                shutdown(it->first, SHUT_RDWR);
                close(it->first);
                it = mSocketIDToGame.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    mGames.erase(gameID);
    LOG(INFO, "Game with ID %d has been removed", gameID);
}