#include "MerchantTurnState.h"
#include "SheriffTurnState.h"
#include "RoundStartedState.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

/**
 * @brief Construct a new Merchant Turn State:: Merchant Turn State object
 *
 */

MerchantTurnState::MerchantTurnState()
{
    LOG(INFO, "MerchantTurnState initialized");
}

MerchantTurnState::~MerchantTurnState()
{
    LOG(INFO, "MerchantTurnState destructor called");
}

void MerchantTurnState::enterState(Game *curGame)
{
    LOG(INFO, "MerchantTurnState::enterState()");
    mMerchantSocketID = curGame->getMerchantTurnSocketID();
    if (mMerchantSocketID == -1)
    {
        LOG(ERROR, "Start next round");
        curGame->setState(new RoundStartedState());
        return;
    }
    curGame->getPlayer(mMerchantSocketID).setState(PLAYER_TRADING);
    mMerchantState = MERCHANT_IDLE;
    if (mMerchantSocketID == -1)
    {
        /* TODO: Handle this situation */
        LOG(ERROR, "No merchant found");
        return;
    }
    Json::Value message;
    message["MessageType"] = "GAME_START_TURN";
    message["PlayerName"] = curGame->getPlayer(mMerchantSocketID).getName();
    curGame->sendMessageToAll(jsonToString(message));
}

void MerchantTurnState::handleResponse(Game *curGame, const Json::Value &jsonMessage, const int socketID)
{
    Player &curPlayer = curGame->getPlayer(socketID);

    std::string reasonType = jsonMessage["ReasonType"].asString();
    if (reasonType == "GAME_START_TURN")
    {
        LOG(INFO, "Player %s is trading", curGame->getPlayer(mMerchantSocketID).getName().c_str());
        return;
    }
    else if (reasonType == "MERCHANT_WITHDRAW_CARDS_RESPONSE" && socketID == mMerchantSocketID)
    {
        LOG(INFO, "Merchant render done");
        mMerchantState = MERCHANT_READY_TO_RECEIVE;
        return;
    }
    else if (reasonType == "MERCHANT_GIVE_BAG" || reasonType == "MERCHANT_GIVE_BAG_RESPONSE")
    {
        LOG(INFO, "Merchant give bag done");
        curPlayer.setState(PLAYER_RECEIVE_BAG);
        for (const auto &player : curGame->getAllPlayers())
        {
            if (player.second->getState() != PLAYER_RECEIVE_BAG)
            {
                return;
            }
        }
        curGame->setState(new SheriffTurnState());
        return;
    }
    else
    {
        LOG(ERROR, "Not handle reasonType '%s'", reasonType.c_str());
        return;
    }
}

void MerchantTurnState::handleRequest(Game *curGame, const Json::Value &jsonMessage, const int socketID)
{
    Player &curPlayer = curGame->getPlayer(socketID);
    std::string messageType = jsonMessage["MessageType"].asString();

    if (socketID != mMerchantSocketID)
    {
        LOG(ERROR, "It isn't your turn");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curGame->getPlayer(socketID).getName(), "NOT_YOUR_TURN"), socketID);
        return;
    }
    /* socketID = mMerchantSocketID */
    if (curPlayer.getState() != PLAYER_TRADING)
    {
        LOG(ERROR, "Player %s is not in the right state", curPlayer.getName().c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_STATE"), socketID);
        return;
    }

    if (messageType == "MERCHANT_DISCARD_REQUEST")
    {
        handleDiscardRequest(curGame, jsonMessage, socketID, curPlayer);
    }
    else if (messageType == "MERCHANT_WITHDRAW_CARDS")
    {
        handleWithdrawCards(curGame, jsonMessage, socketID, curPlayer);
    }
    else if (messageType == "MERCHANT_DISCARD_CARDS")
    {
        handleDiscardCards(curGame, jsonMessage, socketID, curPlayer);
    }
    else if (messageType == "MERCHANT_GIVE_BAG")
    {
        handleGiveBag(curGame, jsonMessage, socketID, curPlayer);
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
}

std::string MerchantTurnState::getStateName() const
{
    return "MerchantTurnState";
}

void MerchantTurnState::handleDiscardRequest(Game *curGame, const Json::Value& jsonMessage, const int socketID, Player& curPlayer)
{
    if (!jsonMessage.isMember("NumberOfCards") || jsonMessage["NumberOfCards"].isNull())
    {
        LOG(ERROR, "No NumberOfCards");
        return;
    }
    mNumberOfCards = stoi(jsonMessage["NumberOfCards"].asString());
    if (mNumberOfCards < 1 || mNumberOfCards > MAX_CARD_OF_PLAYER)
    {
        LOG(ERROR, "Invalid number of cards %d", mNumberOfCards);
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_NUMBER_OF_CARDS"), socketID);
        return;
    }

    mMerchantState = MERCHANT_READY_TO_RECEIVE;
    Json::Value discardMessage;
    discardMessage["MessageType"] = "MERCHANT_DISCARD_REQUEST_RESPONSE";
    discardMessage["PlayerName"] = curPlayer.getName();
    discardMessage["Cards"] = jsonMessage["Cards"];
    curGame->sendMessageToAll(jsonToString(discardMessage));
}
void MerchantTurnState::handleWithdrawCards(Game *curGame, const Json::Value& jsonMessage, const int socketID, Player& curPlayer)
{
    if (mMerchantState != MERCHANT_READY_TO_RECEIVE)
    {
        LOG(INFO, "Merchant requests too fast");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "TOO_FAST"), socketID);
        return;
    }
    mMerchantState = MERCHANT_IS_RENDERING;

    if (curPlayer.getHand().size() >= MAX_CARD_OF_PLAYER + mNumberOfCards)
    {
        LOG(ERROR, "Player %s withdraws maximum cards", curPlayer.getName().c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "ENOUGH_CARD"), socketID);
        return;
    }

    if (!jsonMessage.isMember("Pile") || jsonMessage["Pile"].isNull())
    {
        LOG(ERROR, "No Pile");
        return;
    }

    std::string pile = jsonMessage["Pile"].asString();
    Json::Value withdrawResponse;
    withdrawResponse["MessageType"] = "MERCHANT_WITHDRAW_CARDS_RESPONSE";
    withdrawResponse["PlayerName"] = curPlayer.getName();
    withdrawResponse["Pile"] = pile;
    CardName card;
    if (pile == "MAIN_DECK")
    {
        card = curGame->withdrawDeck();
    }
    else if (pile == "LEFT_DISCARD_PILE")
    {
        card = curGame->withdrawPile(LEFT_PILE);
        withdrawResponse["Card"] = getCardNameString(card); /* Other player have to know if withdraw from DISCARD_PILE */
    }
    else if (pile == "RIGHT_DISCARD_PILE")
    {
        card = curGame->withdrawPile(RIGHT_PILE);
        withdrawResponse["Card"] = getCardNameString(card); /* Other player have to know if withdraw from DISCARD_PILE */
    }
    else
    {
        LOG(ERROR, "Invalid pile '%s'", pile.c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_PILE"), socketID);
        return;
    }
    if (!curPlayer.addCardToHand(card))
    {
        LOG(ERROR, "Player can't withdraws card");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_CARD"), socketID);
        return;
    }
    curGame->sendMessageToAllExclude(jsonToString(withdrawResponse), socketID);

    /* Hide Card from other players */
    withdrawResponse["Card"] = getCardNameString(card);
    curGame->sendMessageToClient(jsonToString(withdrawResponse), socketID);
}
void MerchantTurnState::handleDiscardCards(Game *curGame, const Json::Value& jsonMessage, const int socketID, Player& curPlayer)
{
    /*  Handle situation card is invalid */

    if (!jsonMessage.isMember("Card") || jsonMessage["Card"].isNull())
    {
        LOG(ERROR, "No Card");
        return;
    }

    std::string cardString = jsonMessage["Card"].asString();
    CardName discardCard = INVALID_CARD;
    auto findCard = stringToCardName.find(cardString);
    if (findCard == stringToCardName.end())
    {
        LOG(ERROR, "Invalid card '%s'", cardString.c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_DISCARD_CARD"), socketID);
        return;
    }
    else
    {
        discardCard = findCard->second;
    }

    if (!curPlayer.removeCardFromHand(discardCard))
    {
        LOG(ERROR, "Invalid Card from hand");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_DISCARD_CARD_IN_HAND"), socketID);
        return;
    }

    if (!jsonMessage.isMember("Pile") || jsonMessage["Pile"].isNull())
    {
        LOG(ERROR, "No Pile");
        return;
    }

    std::string pile = jsonMessage["Pile"].asString();
    if (pile == "LEFT_DISCARD_PILE")
    {
        curGame->insertPile(discardCard, LEFT_PILE);
    }
    else if (pile == "RIGHT_DISCARD_PILE")
    {
        curGame->insertPile(discardCard, RIGHT_PILE);
    }
    else
    {
        LOG(ERROR, "Invalid pile");
        return;
    }

    Json::Value discardResponse;
    discardResponse["MessageType"] = "MERCHANT_DISCARD_CARDS_RESPONSE";
    discardResponse["PlayerName"] = curPlayer.getName();
    discardResponse["Pile"] = pile;
    discardResponse["Card"] = cardNameToString.at(discardCard);
    curGame->sendMessageToAll(jsonToString(discardResponse));
}
void MerchantTurnState::handleGiveBag(Game *curGame, const Json::Value& jsonMessage, const int socketID, Player& curPlayer)
{
    if (!jsonMessage.isMember("Fee") || jsonMessage["Fee"].isNull())
    {
        LOG(ERROR, "No Fee");
        return;
    }
    if (!jsonMessage.isMember("Report") || jsonMessage["Report"].isNull())
    {
        LOG(ERROR, "No Report");
        return;
    }
    if (!jsonMessage.isMember("Bag") || jsonMessage["Bag"].isNull())
    {
        LOG(ERROR, "No Bag");
        return;
    }

    const std::string owner = jsonMessage["PlayerName"].asString();
    const int ownerSocketID = mMerchantSocketID;
    const std::string bribe = jsonMessage["Fee"].asString();
    const CardName declared = stringToCardName.at(jsonMessage["Report"].asString());
    int totalCard = 0;
    std::vector<CardName> bagCards;
    /* TODO: Merchant can only trade upto 5 cards */
    for (const auto &item : jsonMessage["Bag"])
    {
        totalCard++;
        bagCards.push_back(stringToCardName.at(item.asString()));
        curPlayer.removeCardFromHand(stringToCardName.at(item.asString()));
    }
    if (!curGame->setBag(bagCards, bribe, declared, owner, ownerSocketID))
    {
        LOG(ERROR, "Invalid bag");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_BAG"), socketID);
        return;
    }
    curPlayer.setState(PLAYER_RECEIVE_CARDS);

    Json::Value bagResponseAll;
    bagResponseAll["MessageType"] = "MERCHANT_GIVE_BAG";
    bagResponseAll["PlayerName"] = curPlayer.getName();
    bagResponseAll["Amount"] = std::to_string(totalCard);
    bagResponseAll["Report"] = jsonMessage["Report"].asString();
    bagResponseAll["Fee"] = bribe;
    curGame->sendMessageToAllExclude(jsonToString(bagResponseAll), socketID);

    Json::Value bagResponseClient;
    bagResponseClient["MessageType"] = "MERCHANT_GIVE_BAG_RESPONSE";
    bagResponseClient["PlayerName"] = curPlayer.getName();
    curGame->sendMessageToClient(jsonToString(bagResponseClient), socketID);
}