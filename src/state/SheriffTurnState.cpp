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

void SheriffTurnState::handleResponse(Game *curGame, const Json::Value &jsonMessage, const int socketID)
{
    Player &curPlayer = curGame->getPlayer(socketID);

    std::string reasonType = jsonMessage["ReasonType"].asString();
    if (reasonType == "SHERIFF_CHECK_RESPONSE" || reasonType == "SHERIFF_PASS_RESPONSE")
    {
        Player &merchantPlayer = curGame->getPlayer(curGame->getMerchantSocketID());
        curGame->sendMessageToClient(jsonToString(merchantPlayer.getPlayerInfo()), socketID);

        return;
    }
    else if (reasonType == "GAME_START_TURN" && socketID == mSheriffSocketID)
    {
        curPlayer.setState(PLAYER_INSPECTING);
        return;
    }
    else if (reasonType == "PLAYER_UPDATE_INFO")
    {
        Player &sheriffPlayer = curGame->getPlayer(curGame->getSheriffSocketID());
        curGame->sendMessageToClient(jsonToString(sheriffPlayer.getPlayerInfo()), socketID);

        curPlayer.setState(PLAYER_RECEIVED_INFO);
        for (const auto &player : curGame->getAllPlayers())
        {
            if (player.second->getState() != PLAYER_RECEIVED_INFO)
            {
                return;
            }
        }
        curGame->setState(new MerchantTurnState());
        return;
    }
    else
    {
        LOG(ERROR, "Not handle reasonType '%s'", reasonType.c_str());
        return;
    }
}

void SheriffTurnState::handleRequest(Game *curGame, const Json::Value &jsonMessage, const int socketID)
{
    Player &curPlayer = curGame->getPlayer(socketID);

    std::string messageType = jsonMessage["MessageType"].asString();

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
        curGame->calculatePenalty(mSheriffSocketID, curBag, false);
    }
    else if (messageType == "SHERIFF_PASS")
    {
        forwardMessage["MessageType"] = "SHERIFF_PASS_RESPONSE";
        curGame->calculatePenalty(mSheriffSocketID, curBag, true);
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
        return;
    }
    // curPlayer.setState(PLAYER_RECEIVE_CARDS);
    (void)curBag.clearBag();
    (void)curGame->tradeContrabandToCards(curGame->getMerchantSocketID()); /* This function will execute if isBlackMarketCard rule is applied */
    curGame->sendMessageToAll(jsonToString(forwardMessage));
}

std::string SheriffTurnState::getStateName() const
{
    return "SheriffTurnState";
}