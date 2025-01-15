#include "Server.h"
#include <iostream>
#include <cstring>

Server::Server() : mServerState(SERVER_WAITING), mServerSocket(-1) {
    std::cout << "Server is booting up!" << std::endl;
}

Server::~Server() {
    stop();
    std::cout << "Server stopped!" << std::endl;
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

        std::cout << "New client connected!" << std::endl;
        std::lock_guard<std::mutex> lock(mClientMutex);
        mClientThreads.emplace_back(&Server::handleClient, this, client_socket);
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valRead = read(clientSocket, buffer, BUFFER_SIZE);
        if (valRead <= 0) {
            std::cout << "Client disconnected!" << std::endl;
            close(clientSocket);
            break;
        }
        buffer[valRead] = '\0';
        std::cout << "Message from client: " << buffer << std::endl;
        send(clientSocket, buffer, strlen(buffer), 0); // Echo message
    }
}

bool Server::init() {
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket < 0) {
        perror("Socket creation failed");
        return false;
    }

    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(SERVER_PORT);
    mServerAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(mServerSocket, (struct sockaddr *)&mServerAddress, sizeof(mServerAddress)) < 0) {
        perror("Bind failed");
        close(mServerSocket);
        return false;
    }

    if (listen(mServerSocket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(mServerSocket);
        return false;
    }

    std::cout << "Server listening on port " << SERVER_PORT << std::endl;
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
    if (mServerState >= SERVER_READY) {
        mServerState = SERVER_WAITING;
        close(mServerSocket);

        std::lock_guard<std::mutex> lock(mClientMutex);
        for (auto &t : mClientThreads) {
            if (t.joinable()) t.join();
        }
        mClientThreads.clear();
    }
}
