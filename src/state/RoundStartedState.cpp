#include "RoundStartedState.h"
#include "MerchantTurnState.h"
#include "GameEndedState.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

/**
 * @brief Construct a new Round Started State:: Round Started State object
 *
 */

RoundStartedState::RoundStartedState()
{
    LOG(INFO, "RoundStartedState initialized");
}

RoundStartedState::~RoundStartedState()
{
    LOG(INFO, "RoundStartedState destructor called");
}

void RoundStartedState::enterState(Game *curGame)
{
    LOG(INFO, "RoundStartedState::enterState()");

    if (curGame->isGameEnded())
    {
        LOG(INFO, "Game ended");
        curGame->setState(new GameEndedState());
        return;
    }

    if (curGame->isSheriffTransfer())
    {
        LOG(INFO, "Game transfers roles to each players");

        int sheriffSocketID = curGame->getSheriffSocketID();
        Player &sheriffPlayer = curGame->getPlayer(sheriffSocketID);
        std::string sheriffName = sheriffPlayer.getName();
        Json::Value message;
        message["MessageType"] = "GAME_DEALS_ROLE";
        message["PlayerName"] = sheriffName;
        message["Role"] = "SHERIFF";
        sheriffPlayer.setRole(SHERIFF);
        for (auto &pair : curGame->getAllPlayers())
        {
            if (pair.first != sheriffSocketID)
            {
                pair.second->setRole(MERCHANT);
                curGame->addMerchantOrder(pair.first);
            }
        }
        curGame->sendMessageToAll(jsonToString(message));
    }
    else
    {
        curGame->dealsCardToPlayers();
    }
}

void RoundStartedState::handleResponse(Game *curGame, const Json::Value &jsonMessage, const int socketID)
{
    std::string reasonType = jsonMessage["ReasonType"].asString();
    if (reasonType == "GAME_DEALS_ROLE")
    {
        Player &curPlayer = curGame->getPlayer(socketID);
        curPlayer.setState(PLAYER_RECEIVE_ROLE);
        for (auto &pair : curGame->getAllPlayers())
        {
            if (pair.second->getState() != PLAYER_RECEIVE_ROLE)
            {
                return;
            }
        }
        curGame->dealsCardToPlayers();
    }
    else if (reasonType == "GAME_DEALS_CARDS")
    {
        Player &curPlayer = curGame->getPlayer(socketID);
        /* TODO: Handle the situation when a player diconnect */
        curPlayer.setState(PLAYER_RECEIVE_CARDS);
        for (auto &pair : curGame->getAllPlayers())
        {
            if (pair.second->getState() != PLAYER_RECEIVE_CARDS)
            {
                return;
            }
        }
        curGame->setState(new MerchantTurnState());
    }
    else
    {
        LOG(ERROR, "Invalid reasonType '%s'", reasonType.c_str());
    }
}

void RoundStartedState::handleRequest(Game *curGame, const Json::Value &jsonMessage, const int socketID)
{
    std::string messageType = jsonMessage["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        LOG(INFO, "Player %s response", curGame->getPlayer(socketID).getName().c_str());
        if (!jsonMessage.isMember("ReasonType") || jsonMessage["ReasonType"].isNull())
        {
            LOG(ERROR, "No ReasonType ");
            return;
        }
        std::string reasonType = jsonMessage["ReasonType"].asString();
        if (reasonType == "GAME_DEALS_ROLE")
        {
            Player &curPlayer = curGame->getPlayer(socketID);
            curPlayer.setState(PLAYER_RECEIVE_ROLE);
            for (auto &pair : curGame->getAllPlayers())
            {
                if (pair.second->getState() != PLAYER_RECEIVE_ROLE)
                {
                    return;
                }
            }
            curGame->dealsCardToPlayers();
        }
        else if (reasonType == "GAME_DEALS_CARDS")
        {
            Player &curPlayer = curGame->getPlayer(socketID);
            /* TODO: Handle the situation when a player diconnect */
            curPlayer.setState(PLAYER_RECEIVE_CARDS);
            for (auto &pair : curGame->getAllPlayers())
            {
                if (pair.second->getState() != PLAYER_RECEIVE_CARDS)
                {
                    return;
                }
            }
            curGame->setState(new MerchantTurnState());
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

std::string RoundStartedState::getStateName() const
{
    return "RoundStartedState";
}