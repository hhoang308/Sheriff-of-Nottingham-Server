#include "SheriffTurnState.h"
#include "MerchantTurnState.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

/**
 * @brief Construct a new Sheriff Turn State:: Sheriff Turn State object
 *
 */

SheriffTurnState::SheriffTurnState()
{
    LOG(INFO, "SheriffTurnState initialized");
}

SheriffTurnState::~SheriffTurnState()
{
    LOG(INFO, "SheriffTurnState destructor called");
}

void SheriffTurnState::enterState(Game *curGame)
{
    LOG(INFO, "SheriffTurnState::enterState()");
    mSheriffSocketID = curGame->getSheriffSocketID();
    Player sheriffPlayer = curGame->getPlayer(mSheriffSocketID);
    LOG(INFO, "Sheriff %s is checking bag", sheriffPlayer.getName().c_str());
    Json::Value message;
    message["MessageType"] = "GAME_START_TURN";
    message["PlayerName"] = sheriffPlayer.getName();
    curGame->sendMessageToAll(jsonToString(message));
}

void SheriffTurnState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    // LOG(INFO, "SheriffTurnState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
    Json::Value curJson = stringToJson(message);
    Player &curPlayer = curGame->getPlayer(socketID);

    if (curJson.empty())
    {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }

    if (!curJson.isMember("MessageType") || curJson["MessageType"].isNull())
    {
        LOG(ERROR, "No MessageType");
        return;
    }

    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        if (!curJson.isMember("ReasonType") || curJson["ReasonType"].isNull())
        {
            LOG(ERROR, "No ReasonType");
            return;
        }
        std::string reasonType = curJson["ReasonType"].asString();
        if (reasonType == "SHERIFF_CHECK_RESPONSE" || reasonType == "SHERIFF_PASS_RESPONSE")
        {
            curPlayer.setState(PLAYER_RECEIVED_CHECK);
            for (const auto &player : curGame->getAllPlayers())
            {
                if (player.second->getState() != PLAYER_RECEIVED_CHECK)
                {
                    return;
                }
            }
            curGame->setState(new MerchantTurnState());
            return;
        }
        else if (reasonType == "GAME_START_TURN" && socketID == mSheriffSocketID)
        {
            curPlayer.setState(PLAYER_INSPECTING);
            return;
        }
        else
        {
            LOG(ERROR, "Not handle reasonType '%s'", reasonType.c_str());
            return;
        }
    }

    if (socketID != mSheriffSocketID)
    {
        LOG(ERROR, "It isn't your turn");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curGame->getPlayer(socketID).getName(), "NOT_YOUR_TURN"), socketID);
        return;
    }

    if (curPlayer.getState() != PLAYER_INSPECTING)
    {
        LOG(ERROR, "Player %s is not in the right state (%d)", curPlayer.getName().c_str(), (int)curPlayer.getState());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_STATE"), socketID);
        return;
    }
    Bag curBag = curGame->getBag();

    Json::Value forwardMessage;
    forwardMessage["PlayerName"] = curPlayer.getName();
    // forwardMessage["Fee"] = curBag.mBagBribe;
    // forwardMessage["Report"] = cardNameToString.at(curBag.mBagDeclared);
    Json::Value bagCards(Json::arrayValue);
    for (const auto &card : curBag.mBagCards)
    {
        bagCards.append(cardNameToString.at(card));
    }
    forwardMessage["Bag"] = bagCards;

    if (messageType == "SHERIFF_CHECK")
    {
        forwardMessage["MessageType"] = "SHERIFF_CHECK_RESPONSE";
    }
    else if (messageType == "SHERIFF_PASS")
    {
        forwardMessage["MessageType"] = "SHERIFF_PASS_RESPONSE";
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
    // curPlayer.setState(PLAYER_RECEIVE_CARDS);
    curGame->sendMessageToAll(jsonToString(forwardMessage));
}

std::string SheriffTurnState::getStateName() const
{
    return "SheriffTurnState";
}