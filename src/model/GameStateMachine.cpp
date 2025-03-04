#include "GameStateMachine.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

std::string createErrorMessage(const std::string &messageType, const std::string &playerName, const std::string &reason)
{
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
    LOG(INFO, "WaitingForPlayersState socketID %d message '%s' ", socketID, message.c_str());
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
    if (curJson.empty())
    {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }
    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        LOG(INFO, "Player %s response", curGame->getPlayer(socketID).getName().c_str());
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

void RoundStartedState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    Json::Value curJson = stringToJson(message);

    if (curJson.empty())
    {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }

    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        LOG(INFO, "Player %s response", curGame->getPlayer(socketID).getName().c_str());
        std::string reasonType = curJson["ReasonType"].asString();
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

/**
 * @brief Construct a new Waiting For Player Response State:: Waiting For Player Response State object
 *
 */

WaitingForPlayerResponseState::WaitingForPlayerResponseState()
{
    LOG(INFO, "WaitingForPlayerResponseState initialized");
}

WaitingForPlayerResponseState::~WaitingForPlayerResponseState()
{
    LOG(INFO, "WaitingForPlayerResponseState destructor called");
}

void WaitingForPlayerResponseState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    LOG(INFO, "WaitingForPlayerResponseState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
}

void WaitingForPlayerResponseState::enterState(Game *curGame)
{
    LOG(INFO, "WaitingForPlayerResponseState::enterState()");
}

std::string WaitingForPlayerResponseState::getStateName() const
{
    return "WaitingForPlayerResponseState";
}

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
void MerchantTurnState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    LOG(INFO, "MerchantTurnState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
    Json::Value curJson = stringToJson(message);

    if (curJson.empty())
    {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }

    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        std::string reasonType = curJson["ReasonType"].asString();
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
        else
        {
            LOG(ERROR, "Not handle reasonType '%s'", reasonType.c_str());
            return;
        }
    }

    if (socketID != mMerchantSocketID)
    {
        LOG(ERROR, "It isn't your turn");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curGame->getPlayer(socketID).getName(), "NOT_YOUR_TURN"), socketID);
        return;
    }
    /* socketID = mMerchantSocketID */
    Player &curPlayer = curGame->getPlayer(socketID);
    if (curPlayer.getState() != PLAYER_TRADING)
    {
        LOG(ERROR, "Player %s is not in the right state", curPlayer.getName().c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_STATE"), socketID);
        return;
    }

    if (messageType == "MERCHANT_DISCARD_REQUEST")
    {
        mNumberOfCards = curJson["NumberOfCards"].asInt();
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
        curGame->sendMessageToClient(jsonToString(discardMessage), socketID);
    }
    else if (messageType == "MERCHANT_WITHDRAW_CARDS")
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

        std::string pile = curJson["Pile"].asString();
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
    else if (messageType == "MERCHANT_DISCARD_CARDS")
    {
        /*  Handle situation card is invalid */
        std::string cardString = curJson["Card"].asString();
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

        std::string pile = curJson["Pile"].asString();
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
    else if (messageType == "MERCHANT_GIVE_BAG")
    {
        const std::string owner = curJson["PlayerName"].asString();
        const int bribe = curJson["Fee"].asInt();
        const CardName declared = stringToCardName.at(curJson["Report"].asString());
        std::vector<CardName> bagCards;
        for (const auto &item : curJson["Bag"])
        {
            bagCards.push_back(stringToCardName.at(item.asString()));
        }
        if (!curGame->setBag(bagCards, bribe, declared, owner))
        {
            LOG(ERROR, "Invalid bag");
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_BAG"), socketID);
            return;
        }
        curPlayer.setState(PLAYER_RECEIVE_CARDS);
        Json::Value bagResponse;
        bagResponse["MessageType"] = "MERCHANT_GIVE_BAG_RESPONSE";
        bagResponse["PlayerName"] = curPlayer.getName();
        curGame->sendMessageToClient(jsonToString(bagResponse), socketID);
        curGame->setState(new SheriffTurnState());
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
void SheriffTurnState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    LOG(INFO, "SheriffTurnState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
    Json::Value curJson = stringToJson(message);

    if (curJson.empty())
    {
        LOG(ERROR, "Invalid JSON message '%s'", message.c_str());
        return;
    }

    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        std::string reasonType = curJson["ReasonType"].asString();
        if (reasonType == "SHERIFF_CHECK_RESPONSE")
        {
            return;
        }
        else if (reasonType == "SHERIFF_PASS_REPONSE")
        {
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
    /* socketID = mSheriffSocketID */
    Player &curPlayer = curGame->getPlayer(socketID);
    if (curPlayer.getState() != PLAYER_INSPECTING)
    {
        LOG(ERROR, "Player %s is not in the right state", curPlayer.getName().c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_STATE"), socketID);
        return;
    }
    Bag curBag = curGame->getBag();

    Json::Value forwardMessage;
    forwardMessage["PlayerName"] = curBag.mBagOwner;
    forwardMessage["Fee"] = curBag.mBagBribe;
    forwardMessage["Report"] = cardNameToString.at(curBag.mBagDeclared);
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
        forwardMessage["MessageType"] = "SHERIFF_PASS_REPONSE";
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
    curPlayer.setState(PLAYER_RECEIVE_CARDS);
    curGame->sendMessageToAll(jsonToString(forwardMessage));
    curGame->setState(new MerchantTurnState());
}
void SheriffTurnState::enterState(Game *curGame)
{
    LOG(INFO, "SheriffTurnState::enterState()");
    mSheriffSocketID = curGame->getSheriffSocketID();
    Player curPlayer = curGame->getPlayer(mSheriffSocketID);
    LOG(INFO, "Sheriff %s is checking bag", curPlayer.getName().c_str());
    curPlayer.setState(PLAYER_INSPECTING);
    Json::Value message;
    message["MessageType"] = "GAME_START_TURN";
    message["PlayerName"] = curPlayer.getName();
    curGame->sendMessageToAll(jsonToString(message));
}
std::string SheriffTurnState::getStateName() const
{
    return "SheriffTurnState";
}

/**
 * @brief Construct a new Game Ended State:: Game Ended State object
 *
 */

GameEndedState::GameEndedState()
{
    LOG(INFO, "GameEndedState initialized");
}
GameEndedState::~GameEndedState()
{
    LOG(INFO, "GameEndedState destructor called");
}
void GameEndedState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    LOG(INFO, "GameEndedState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
}
void GameEndedState::enterState(Game *curGame)
{
    LOG(INFO, "GameEndedState::enterState()");
    /* TODO: Calculate who is the winner */
    Json::Value endGameMessage;
    endGameMessage["MessageType"] = "GAME_END";
    /* TODO: change state of all players and disconnect all of them */
    curGame->sendMessageToAll(jsonToString(endGameMessage));
}
std::string GameEndedState::getStateName() const
{
    return "GameEndedState";
}