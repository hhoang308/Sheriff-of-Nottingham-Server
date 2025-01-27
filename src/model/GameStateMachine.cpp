#include "GameStateMachine.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include <iostream>

std::string createErrorMessage(const std::string& messageType, const std::string& playerName, const std::string& reason) {
    Json::Value message;
    message["MessageType"] = messageType;
    message["PlayerName"] = playerName;
    message["Reason"] = reason;

    std::string jsonString = jsonToString(message);
    LOG(INFO, "createErrorMessage() %s", jsonString.c_str());
    return jsonString;
}

/**
 * @brief Construct a new Waiting For Players State:: Waiting For Players State object
 *
 */

WaitingForPlayersState::WaitingForPlayersState() {
    LOG(INFO, "WaitingForPlayersState initialized");
}

WaitingForPlayersState::~WaitingForPlayersState() {
    LOG(INFO, "WaitingForPlayersState destructor called");
}

void WaitingForPlayersState::handleRequest(Game* curGame, const std::string& message, const int socketID) {
    LOG(INFO, "WaitingForPlayersState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
    Json::Value curJson = stringToJson(message);
    if(curJson.empty()) {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }
    std::string messageType = curJson["MessageType"].asString();
    if(messageType == "PLAYER_JOIN") {

        std::string playerName = curJson["PlayerName"].asString();
        if(curGame->isPlayerNameTaken(playerName)) {
            LOG(ERROR, "Player name '%s' is already taken", playerName.c_str());
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", playerName, "SAME_NAME_EXISTS"), socketID);
            Server::getInstance().closeConnection(socketID);
            return;
        }

        // Move this section to handleClient() to avoid server crashed if more than 6 player connect
        if(curGame->getPlayerSize() >= MAX_NUMBER_OF_PLAYER) {
            LOG(ERROR, "Game is full, can't add playerName %s", playerName.c_str());
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", playerName, "FULL"), socketID);
            Server::getInstance().closeConnection(socketID);
            return;
        }

        LOG(INFO, "Player %s joined the game", playerName.c_str());

        /* TODO: Handle new client or reconnect clients */

        Json::Value acceptMessage;
        acceptMessage["MessageType"] = "GAME_ACCEPT_PLAYER";
        acceptMessage["PlayerName"] = playerName;
        curGame->sendMessageToClient(jsonToString(acceptMessage), socketID);

        if(curGame->getPlayerSize() >= 1) {

            Json::Value connectedRecentlyMessage;
            connectedRecentlyMessage["MessageType"] = "GAME_CONNECTED_PLAYER_RECENT";
            Json::Value playersName(Json::arrayValue);
            for (const auto& pair : curGame->getAllPlayers()) {
                playersName.append(pair.second->getName());
            }
            connectedRecentlyMessage["PlayersName"] = playersName;

            curGame->sendMessageToClient(jsonToString(connectedRecentlyMessage), socketID);

            Json::Value notifyMessage;
            notifyMessage["MessageType"] = "GAME_CONNECTED_PLAYER_NOW";
            notifyMessage["PlayerName"] = playerName;
            curGame->sendMessageToAllExclude(jsonToString(notifyMessage), socketID);
        }

        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);

    } else if(messageType == "PLAYER_READY") {
        mPlayerReadyCount++;
        std::string playerName = curJson["PlayerName"].asString();
        LOG(INFO, "Player %s is ready", playerName.c_str());
        Json::Value message;
        message["MessageType"] = "GAME_ACCEPT_READY";
        message["PlayerName"] = playerName;
        (void) curGame->getPlayer(socketID).setState(PLAYER_READY);
        curGame->sendMessageToAll(jsonToString(message));

        if(curGame->getPlayerSize() >= 3
        && mPlayerReadyCount == curGame->getPlayerSize()) {
            LOG(INFO, "All players are ready, starting the game");
            Json::Value readyMessage;
            readyMessage["MessageType"] = "GAME_START";
            curGame->sendMessageToAll(jsonToString(readyMessage));
            curGame->setState(new GameStartedState());
        }

    } else if(messageType == "PLAYER_UNREADY") {

        mPlayerReadyCount--;
        std::string playerName = curJson["PlayerName"].asString();
        LOG(INFO, "Player %s is ready", playerName.c_str());
        Json::Value message;
        message["MessageType"] = "GAME_ACCEPT_UNREADY";
        message["PlayerName"] = playerName;
        curGame->sendMessageToAll(jsonToString(message));

    } else if(messageType == "PLAYERRESPONSE") {
        LOG(INFO, "Player %s response", curGame->getPlayer(socketID).getName().c_str());
    } else {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
}

void WaitingForPlayersState::enterState() {
    LOG(INFO, "WaitingForPlayersState::enterState()");
}

std::string WaitingForPlayersState::getStateName() const {
    return "WaitingForPlayersState";
}

/**
 * @brief
 *
 * @param server
 * @param message
 * @param socketID
 */

/* TODO: Handle situation when player connects but the game is started */

GameStartedState::GameStartedState() {
    LOG(INFO, "GameStartedState initialized");
}

GameStartedState::~GameStartedState() {
    LOG(INFO, "GameStartedState destructor called");
}

void GameStartedState::handleRequest(Game* server, const std::string& message, const int socketID) {
    Json::Value curJson = stringToJson(message);
    if(curJson.empty()) {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }
    std::string messageType = curJson["MessageType"].asString();
    if(messageType == "MERCHANT_WITHDRAW_CARDS") {

    } else if(messageType == "MERCHANT_GIVE_BAG") {

    } else if(messageType == "SHERIFF_CHECK") {

    } else if(messageType == "SHERIFF_PASS") {

    } else {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
}

void GameStartedState::enterState() {
    LOG(INFO, "GameStartedState::enterState()");
}

std::string GameStartedState::getStateName() const {
    return "GameStartedState";
}

/**
 * @brief Construct a new Waiting For Player Response State:: Waiting For Player Response State object
 *
 * @param role
 */

WaitingForPlayerResponseState::WaitingForPlayerResponseState(const std::string &role) : currentRole(role) {
    std::cout << "WaitingForPlayerResponseState initialized with role: " << role << std::endl;
}

void WaitingForPlayerResponseState::handleRequest(Game* server, const std::string& message, const int socketID) {
    LOG(INFO, "WaitingForPlayerResponseState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
}

void WaitingForPlayerResponseState::enterState() {
    std::cout << "Waiting for " << currentRole << " response...\n";

    if (currentRole == "Sheriff") {
        // Logic dành riêng cho Sheriff
    } else if (currentRole == "Merchant") {
        // Logic dành riêng cho Merchant
    }
}

std::string WaitingForPlayerResponseState::getStateName() const {
    return "WaitingForPlayerResponseState";
}

// /**
//  * @brief
//  *
//  * @param server
//  * @param message
//  * @param socketID
//  */

// void RoundStartedState::handleRequest(Game* server, const std::string& message, const int socketID) {

// }

// std::string RoundStartedState::getStateName() const {
//     return "RoundStartedState";
// }

// /**
//  * @brief
//  *
//  * @param server
//  * @param message
//  * @param socketID
//  */

// void AssisgnRolesState::handleRequest(Game* server, const std::string& message, const int socketID) {

// }
// std::string AssisgnRolesState::getStateName() const {
//     return "AssisgnRolesState";
// }

// /**
//  * @brief
//  *
//  * @param server
//  * @param message
//  * @param socketID
//  */

// void ChooseMerchantState::handleRequest(Game* server, const std::string& message, const int socketID) {
// }

// std::string ChooseMerchantState::getStateName() const {
//     return "ChooseMerchantState";
// }

// /**
//  * @brief
//  *
//  * @param server
//  * @param message
//  * @param socketID
//  */

// void chooseSheriffState::handleRequest(Game* server, const std::string& message, const int socketID) {

// }

// std::string chooseSheriffState::getStateName() const {
//     return "chooseSheriffState";
// }