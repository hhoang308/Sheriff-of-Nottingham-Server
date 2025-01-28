#include "Server.h"
#include "Utils.h"
#include "Log.h"
#include <iostream>
#include <cstring>

Server& Server::getInstance() {
    static Server instance;
    return instance;
}

Server::Server() : mServerState(SERVER_WAITING), mServerSocket(0) {
    LOG(INFO, "Server created!");
}

Server::~Server() {
    stop();
    LOG(INFO, "Server destroyed!");
}

Game& Server::getGame(const int gameID) {
    return *(mGames.at(gameID));
}

bool Server::init() {
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket < 0) {
        LOG(ERROR, "Socket creation failed");
        return false;
    }

    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(SERVER_PORT);
    mServerAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(mServerSocket, (struct sockaddr *)&mServerAddress, sizeof(mServerAddress)) < 0) {
        LOG(ERROR, "Bind failed");
        close(mServerSocket);
        return false;
    }

    if (listen(mServerSocket, MAX_CLIENTS) < 0) {
        LOG(ERROR, "Listen failed");
        close(mServerSocket);
        return false;
    }

    LOG(INFO, "Server initialized on port %d", SERVER_PORT);
    mServerState = SERVER_READY;
    return true;
}

void Server::start() {
    if (!init()) return;
    std::thread(&Server::acceptClient, this).detach();

    while (mServerState == SERVER_READY) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Server::stop() {
    mServerState = SERVER_WAITING;
    LOG(INFO, "Server is stopping");
    close(mServerSocket);

    std::lock_guard<std::mutex> lock(mClientMutex);
    for (auto &t : mClientThreads) {
        if (t.joinable()) t.join();
    }
    mClientThreads.clear();
}

void Server::acceptClient() {
    socklen_t addrlen = sizeof(mServerAddress);
    while (mServerState == SERVER_READY) {
        int client_socket = accept(mServerSocket, (struct sockaddr *)&mServerAddress, &addrlen);
        if (client_socket < 0) {
            if (mServerState == SERVER_WAITING) break; // Stop accepting if server state changes
            perror("Accept failed");
            continue;
        }
        /* TODO: 1 Game in 1 Server, remove when expanding sourcecode multiple games in 1 Server */
        {
            std::lock_guard<std::mutex> lock(mGamesMutex);
            if (mGames.find(GAME_ID_DEFAULT) == mGames.end()) {
                mGames.emplace(GAME_ID_DEFAULT, std::make_unique<Game>(GAME_ID_DEFAULT));
            }
        }

        /* Move create player to handleRequest() of Game, refer to https://github.com/hhoang308/Sheriff-of-Nottingham-Server/issues/8 */
        LOG(INFO, "Have a new connection!");

        /* TODO: Change to thread pool to optimize resources */
        std::lock_guard<std::mutex> lock(mClientMutex);
        mSocketIDToGame[client_socket] = GAME_ID_DEFAULT;
        mClientThreads.emplace_back(&Server::handleClient, this, client_socket);
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valRead = read(clientSocket, buffer, BUFFER_SIZE - 1); /* Ensure space for null-terminator */
        if (valRead < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG(WARNING, "Non-blocking socket has no data to read, retrying later.");
                continue;
            } else if (errno == EINTR) {
                LOG(WARNING, "Read interrupted by signal, retrying.");
                continue;
            } else {
                LOG(ERROR, "Read failed: %s", strerror(errno));
                // closeConnection(clientSocket);
                break;
            }
        }

        /* Suspicious connection : https://github.com/hhoang308/Sheriff-of-Nottingham-Server/issues/8 */
        std::string rawMessage(buffer);
        if(rawMessage.compare(0, 3, "GET") == 0) {
            LOG(INFO, "GET request from client %d, close and remove it", clientSocket);
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            break;
        }

        Game* curGame = nullptr;
        {
            std::lock_guard<std::mutex> lock(mClientMutex);
            if (mSocketIDToGame.find(clientSocket) != mSocketIDToGame.end()) {
                curGame = mGames[mSocketIDToGame[clientSocket]].get();
            }
        }

        /* Handle the situation player join but the game has started */
        if (curGame
        && curGame->getCurrentState() != "WaitingForPlayersState"
        && mSocketIDToGame.find(clientSocket) == mSocketIDToGame.end()) {
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
        if (valRead == 0) {
            if(curGame->getPlayerSize() == 1){
                LOG(INFO, "All players disconnected, game is over!");
                mGames.erase(mSocketIDToGame[clientSocket]);
                break;
            }
            if(curGame->isPlayerExists(clientSocket)){
                Json::Value message;
                message["MessageType"] = "GAME_DISCONNECT_PLAYER";
                message["PlayerName"] = curGame->getPlayer(clientSocket).getName();
                LOG(INFO, "Client disconnected!");
                {
                    std::lock_guard<std::mutex> lock(mClientMutex);
                    /* TODO : Find player in all game -> erase their data */
                    if (curGame) {
                        LOG(INFO, "Removing playerSocketID %d from game", clientSocket);
                        (void) curGame->removePlayer(clientSocket);
                    }
                    mSocketIDToGame.erase(clientSocket);
                }
                sendToAll(jsonToString(message));
            }
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            break;
        }
        buffer[valRead] = '\0'; /* null-terminate the buffer */
        LOG(INFO, "Message from client %d : %s", clientSocket, buffer);

        // if (send(clientSocket, buffer, valRead, 0) < 0) { // Echo message
        //     LOG(ERROR, "Send failed");
        //     break;
        // }

        if (curGame) {
            std::string str(buffer);
            LOG(INFO, "Handling request");
            curGame->handleRequest(str, clientSocket);
        } else {
            LOG(ERROR, "Game not found for client!");
        }
    }
}

void Server::addSocketIDToGame(const int socketID, const int gameID) {
    std::lock_guard<std::mutex> lock(mSocketIDToGameMutex);
    mSocketIDToGame[socketID] = gameID;
}

void Server::sendToClient(const std::string& message, const int socketID) {
    LOG(INFO, "Sending message to player %d: %s", socketID, message.c_str());
    if (send(socketID, message.c_str(), message.size(), 0) < 0) {
        LOG(ERROR, "Send failed");
    }
}

void Server::sendToAll(const std::string& message) {
    std::lock_guard<std::mutex> lock(mClientMutex);
    for (const auto& pair : mSocketIDToGame) {
        sendToClient(message, pair.first);
    }
}

void Server::sendToAllExcept(const std::string& message, const int socketID) {
    std::lock_guard<std::mutex> lock(mClientMutex);
    for (const auto& pair : mSocketIDToGame) {
        if (pair.first != socketID) {
            sendToClient(message, pair.first);
        }
    }
}

void Server::closeConnection(const int socketID) {
    LOG(INFO, "Closing connection for player %d", socketID);
    shutdown(socketID, SHUT_RDWR);
    close(socketID);
    std::lock_guard<std::mutex> lock(mClientMutex);
    mSocketIDToGame.erase(socketID);
}

// void Server::acceptClient() {
//     socklen_t addrlen = sizeof(mServerAddress);
//     while (true) {
//         int clientSocket = accept(mServerSocket, (struct sockaddr*)&mServerAddress, &addrlen);
//         if (clientSocket < 0) {
//             perror("Accept failed");
//             continue;
//         }

//         /* Player message */
//         char buffer[BUFFER_SIZE];
//         recv(clientSocket, buffer, BUFFER_SIZE, 0);
//         std::string clientMessage(buffer);

//         /* Parse JSON */
//         std::string playerName = parsePlayerName(clientMessage); // Hàm parse JSON
//         if (playerName.empty()) {
//             std::cout << "Invalid connection attempt!" << std::endl;
//             close(clientSocket);
//             continue;
//         }

//         /* Player exists */
//         std::lock_guard<std::mutex> lock(mClientMutex);
//         if (playerData.find(playerName) == playerData.end()) {
//             // Người chơi mới
//             Player* newPlayer = new Player(playerName, MERCHANT);
//             playerData[playerName] = newPlayer;
//             socketToPlayer[clientSocket] = playerName;
//             std::cout << "New player joined: " << playerName << std::endl;
//         } else {
//             // Người chơi cũ kết nối lại
//             Player* existingPlayer = playerData[playerName];
//             std::cout << "Player reconnected: " << playerName << std::endl;

//             // Cập nhật socket ID
//             int oldSocket = findSocketByPlayerName(playerName);
//             if (oldSocket != -1) {
//                 close(oldSocket); // Đóng kết nối cũ nếu cần
//                 socketToPlayer.erase(oldSocket);
//             }
//             socketToPlayer[clientSocket] = playerName;
//         }

//         // Khởi chạy thread xử lý client
//         mClientThreads.emplace_back(&Server::handleClient, this, clientSocket);
//     }
// }