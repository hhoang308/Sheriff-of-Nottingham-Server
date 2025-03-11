#include "WaitingForPlayersState.h"
#include "GameStartedState.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

/**
 * @brief Construct a new Waiting For Players State:: Waiting For Players State object
 *
 */

WaitingForPlayersState::WaitingForPlayersState()
{
    LOG(INFO, "WaitingForPlayersState initialized");
}

WaitingForPlayersState::~WaitingForPlayersState()
{
    LOG(INFO, "WaitingForPlayersState destructor called");
}

void WaitingForPlayersState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    // LOG(INFO, "WaitingForPlayersState socketID %d message '%s' ", socketID, message.c_str());
    /* TODO: Handle error when parse string failed */
    Json::Value curJson = stringToJson(message);
    if (curJson.empty())
    {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }
    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_JOIN")
    {

        std::string playerName = curJson["PlayerName"].asString();
        if (curGame->isPlayerNameTaken(playerName))
        {
            LOG(ERROR, "Player name '%s' is already taken", playerName.c_str());
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", playerName, "SAME_NAME_EXISTS"), socketID);
            Server::getInstance().closeConnection(socketID);
            return;
        }

        // Move this section to handleClient() to avoid server crashed if more than 6 player connect
        if (curGame->getPlayerSize() >= MAX_NUMBER_OF_PLAYER)
        {
            LOG(ERROR, "Game is full, can't add playerName %s", playerName.c_str());
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", playerName, "FULL"), socketID);
            Server::getInstance().closeConnection(socketID);
            return;
        }

        LOG(INFO, "Player %s joined the game", playerName.c_str());
        mPlayerNameTemp[socketID] = playerName;

        /* TODO: Handle new client or reconnect clients */

        Json::Value acceptMessage;
        acceptMessage["MessageType"] = "GAME_ACCEPT_PLAYER";
        acceptMessage["PlayerName"] = playerName;
        curGame->sendMessageToClient(jsonToString(acceptMessage), socketID);
    }
    else if (messageType == "PLAYER_READY")
    {
        std::string playerName = curJson["PlayerName"].asString();
        LOG(INFO, "Player %s is ready", playerName.c_str());
        Json::Value message;
        message["MessageType"] = "GAME_ACCEPT_READY";
        message["PlayerName"] = playerName;
        (void)curGame->getPlayer(socketID).setState(PLAYER_READY);
        curGame->sendMessageToAll(jsonToString(message));

        if (curGame->getPlayerSize() >= 3 && curGame->isAllPlayerReady())
        {
            LOG(INFO, "All players are ready, starting the game");
            curGame->setState(new GameStartedState());
        }
    }
    else if (messageType == "PLAYER_UNREADY")
    {
        std::string playerName = curJson["PlayerName"].asString();
        LOG(INFO, "Player %s is ready", playerName.c_str());
        Json::Value message;
        message["MessageType"] = "GAME_ACCEPT_UNREADY";
        message["PlayerName"] = playerName;
        (void)curGame->getPlayer(socketID).setState(PLAYER_UNREADY);
        curGame->sendMessageToAll(jsonToString(message));
    }
    else if (messageType == "PLAYER_RESPONSE")
    {
        std::string reasonType = curJson["ReasonType"].asString();
        if (reasonType == "GAME_ACCEPT_PLAYER")
        {
            std::string playerName = mPlayerNameTemp[socketID];

            LOG(INFO, "Player %s response", playerName.c_str());

            if (curGame->getPlayerSize() >= 1)
            {
                Json::Value connectedRecentlyMessage;
                connectedRecentlyMessage["MessageType"] = "GAME_CONNECTED_PLAYER_RECENT";
                Json::Value playersName(Json::arrayValue);
                for (const auto &pair : curGame->getAllPlayers())
                {
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
        }
        else if (reasonType == "GAME_CONNECTED_PLAYER_RECENT")
        {
            for (auto &pair : curGame->getAllPlayers())
            {
                if (pair.second->getState() == PLAYER_READY)
                {
                    Json::Value message;
                    message["MessageType"] = "GAME_ACCEPT_READY";
                    message["PlayerName"] = pair.second->getName();
                    curGame->sendMessageToClient(jsonToString(message), socketID);
                }
            }
        }
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
}

void WaitingForPlayersState::enterState(Game *server)
{
    LOG(INFO, "WaitingForPlayersState::enterState()");
}

std::string WaitingForPlayersState::getStateName() const
{
    return "WaitingForPlayersState";
}
