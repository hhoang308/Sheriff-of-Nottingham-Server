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

        /* TODO: Handle new client or reconnect clients */

        Json::Value acceptMessage;
        acceptMessage["MessageType"] = "GAME_ACCEPT_PLAYER";
        acceptMessage["PlayerName"] = playerName;
        curGame->sendMessageToClient(jsonToString(acceptMessage), socketID);

        if (curGame->getPlayerSize() >= 1)
        {

            Json::Value connectedRecentlyMessage;
            connectedRecentlyMessage["MessageType"] = "GAME_CONNECTED_PLAYER_RECENT";
            Json::Value playersName(Json::arrayValue);
            for (const auto& pair : curGame->getAllPlayers())
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
        LOG(INFO, "Player %s response", curGame->getPlayer(socketID).getName().c_str());
        // std::string reasonType = curJson["ReasonType"].asString();
        // if(reasonType == "GAME_CONNECTED_PLAYER_RECENT"){
        //     /* The new player can know the status (READY/UNREADY) of the previous players. */
        //     for(const auto &pair : curGame->getAllPlayers())
        //     {
        //         if(pair.second->getState() == PLAYER_READY
        //         && socketID != pair.first)
        //         {
        //             Json::Value playerConnectedState;
        //             playerConnectedState["MessageType"] = "PLAYER_ACCEPT_READY";
        //             playerConnectedState["PlayerName"] = pair.second->getName();
        //             curGame->sendMessageToClient(jsonToString(playerConnectedState), socketID);
        //         }
        //     }
        // }
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
            for(auto& pair : curGame->getAllPlayers())
            {
                if(pair.second->getState() != PLAYER_RECEIVE_ROLE)
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

    if (socketID != mMerchantSocketID)
    {
        LOG(ERROR, "It isn't your turn");
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curGame->getPlayer(socketID).getName(), "NOT_YOUR_TURN"), socketID);
        return;
    }

    Player &curPlayer = curGame->getPlayer(socketID);
    if (curPlayer.getState() != PLAYER_TRADING)
    {
        LOG(ERROR, "Player %s is not in the right state", curPlayer.getName().c_str());
        curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_STATE"), socketID);
        return;
    }
    std::string messageType = curJson["MessageType"].asString();
    if (messageType == "PLAYER_RESPONSE")
    {
        std::string reasonType = curJson["ReasonType"].asString();
        if (reasonType == "GAME_START_TURN")
        {
            curPlayer.setState(PLAYER_TRADING);
            LOG(INFO, "Player %s is trading", curPlayer.getName().c_str());
            return;
        }
    }
    else if (messageType == "MERCHANT_WITHDRAW_CARDS")
    {
        std::string pile = curJson["Pile"].asString();
        Json::Value message;
        message["MessageType"] = "MERCHANT_WITHDRAW_CARDS_RESPONSE";
        message["PlayerName"] = curPlayer.getName();
        message["Pile"] = pile;
        CardName card;
        if (pile == "MAIN_PILE")
        {
            card = curGame->withdrawDeck();
        }
        else if (pile == "LEFT_DISCARD_PILE")
        {
            card = curGame->withdrawPile(LEFT_PILE);
        }
        else if (pile == "RIGHT_DISCARD_PILE")
        {
            card = curGame->withdrawPile(RIGHT_PILE);
        }
        else
        {
            LOG(ERROR, "Invalid pile '%s'", pile.c_str());
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_PILE"), socketID);
            return;
        }
        if (curPlayer.addCardToHand(card))
        {
            LOG(ERROR, "Player withdraws maximum cards");
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "ENOUGH_CARD"), socketID);
            return;
        }
        message["Card"] = getCardNameString(card);
        curGame->sendMessageToAll(jsonToString(message));
    }
    else if (messageType == "MERCHANT_DISCARD_CARDS")
    {
        std::string pile = curJson["Pile"].asString();
        /* TODO: Handle situation card is invalid */
        CardName discardCard = stringToCardName.at(curJson["Card"].asString());
        if(curPlayer.removeCardFromHand(discardCard)){
            LOG(ERROR, "Invalid Card from hand");
            curGame->sendMessageToClient(createErrorMessage("GAME_REJECT_PLAYER", curPlayer.getName(), "INVALID_DISCARD_CARD"), socketID);
            return;
        }
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

        Json::Value message;
        message["MessageType"] = "MERCHANT_DISCARD_CARDS_RESPONSE";
        message["PlayerName"] = curPlayer.getName();
        message["Pile"] = pile;
        message["Card"] = cardNameToString.at(discardCard);
        curGame->sendMessageToAll(jsonToString(message));
    }
    else if (messageType == "MERCHANT_GIVE_BAG")
    {
        curGame->setState(new SheriffTurnState());
    }
    else
    {
        LOG(ERROR, "Invalid message type '%s'", messageType.c_str());
    }
}
std::string MerchantTurnState::getStateName() const
{
    LOG(INFO, "MerchantTurnState::getStateName()");
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
}
void SheriffTurnState::enterState(Game *curGame)
{
    LOG(INFO, "SheriffTurnState::enterState()");
}
std::string SheriffTurnState::getStateName() const
{
    LOG(INFO, "SheriffTurnState::getStateName()");
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
}
std::string GameEndedState::getStateName() const
{
    LOG(INFO, "GameEndedState::getStateName()");
}