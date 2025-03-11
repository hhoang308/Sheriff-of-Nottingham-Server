#include "GameStartedState.h"
#include "RoundStartedState.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

/**
 * @brief Construct a new Game Started State:: Game Started State object
 *
 */

GameStartedState::GameStartedState()
{
    LOG(INFO, "GameStartedState initialized");
}

GameStartedState::~GameStartedState()
{
    LOG(INFO, "GameStartedState destructor called");
}

void GameStartedState::enterState(Game *curGame)
{
    LOG(INFO, "GameStartedState::enterState()");

    /* Create Withdraw Deck and Discard Decks */

    if (!curGame->createGameDetails())
    {
        LOG(ERROR, "Failed to create and shuffle deck");
        return;
    }

    Json::Value readyMessage;
    readyMessage["MessageType"] = "GAME_START";
    curGame->sendMessageToAll(jsonToString(readyMessage));

    /* TODO: Send 2 discard piles */
}

void GameStartedState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    Json::Value curJson = stringToJson(message);

    if (curJson.empty() || !curJson.isMember("MessageType") || curJson["MessageType"].isNull())
    {
        LOG(ERROR, "No MessageType");
        return;
    }

    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        LOG(INFO, "Player %s response", curGame->getPlayer(socketID).getName().c_str());
        if (curJson["ReasonType"].isNull() || curJson["ReasonType"].isNull())
        {
            LOG(ERROR, "No ReasonType ");
            return;
        }
        std::string reasonType = curJson["ReasonType"].asString();
        if (reasonType == "GAME_START")
        {
            curGame->getPlayer(socketID).setState(PLAYER_START);
            /* TODO: Handle the situation when a player disconnect then game can't start */
            for (auto &pair : curGame->getAllPlayers())
            {
                if (pair.second->getState() != PLAYER_START)
                {
                    return;
                }
            }
            Json::Value message;
            message["MessageType"] = "DISCARD_PILES";
            Json::Value leftPile(Json::arrayValue);
            Json::Value rightPile(Json::arrayValue);

            for (auto card : curGame->getPile(LEFT_PILE))
            {
                leftPile.append(cardNameToString.at(card));
            }

            for (auto card : curGame->getPile(RIGHT_PILE))
            {
                rightPile.append(cardNameToString.at(card));
            }

            message["LeftPile"] = leftPile;
            message["RightPile"] = rightPile;
            curGame->sendMessageToAll(jsonToString(message));
        }
        else if (reasonType == "DISCARD_PILES")
        {
            curGame->getPlayer(socketID).setState(PLAYER_RECEIVE_INITIAL_DISCARD_PILE);
            for (auto &pair : curGame->getAllPlayers())
            {
                if (pair.second->getState() != PLAYER_RECEIVE_INITIAL_DISCARD_PILE)
                {
                    return;
                }
            }
            curGame->setState(new RoundStartedState());
        }
        else
        {
            LOG(ERROR, "Invalid reasonType '%s'", reasonType.c_str());
        }
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
}

std::string GameStartedState::getStateName() const
{
    return "GameStartedState";
}