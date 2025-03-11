#include "Game.h"
#include "WaitingForPlayersState.h"
#include "Utils.h"
#include <iostream>
#include <Log.h>
#include <memory>

/**
 * @brief Construct a new Game:: Game object
 *
 * @param gameId
 */
Game::Game(const int gameId) : mGameId(gameId)
{
    currentState = new WaitingForPlayersState();
    printf("Game created! Game ID %d, currentState %s\n", gameId, currentState->getStateName().c_str());
}

/**
 * @brief Destroy the Game:: Game object
 *
 */
Game::~Game()
{
    printf("Game is deleted!\n");
    delete currentState;
}

void Game::setState(GameState *newState)
{
    LOG(INFO, "previousState %s", newState->getStateName().c_str());
    delete currentState;
    currentState = newState;
    if (currentState)
    {
        currentState->enterState(this);
    }
    LOG(INFO, "currentState %s", currentState->getStateName().c_str());
}

void Game::handleRequest(const std::string &message, const int socketId)
{
    if (currentState)
    {
        currentState->handleRequest(this, message, socketId);
    }
    else
    {
        LOG(ERROR, "No valid state to handle the message!");
    }
}

std::string Game::getCurrentState()
{
    if (currentState == nullptr)
    {
        return "InvalidState";
    }
    return currentState->getStateName();
}

bool Game::addPlayer(const int socketID, const int gameID, const std::string &playerName)
{
    // LOG(INFO, "socketID %d gameID %d", socketID, gameID);
    {
        std::lock_guard<std::mutex> lock(mPlayerMutex);
        if (mPlayers.size() > MAX_NUMBER_OF_PLAYER)
        {
            return false;
        }
        mPlayers.emplace(mPlayers.end(), socketID, std::make_unique<Player>(socketID, gameID, playerName));
    }
    Server::getInstance().addSocketIDToGame(socketID, GAME_ID_DEFAULT);
    return true;
}

bool Game::removePlayer(int socketId)
{
    LOG(INFO, "removePlayer() %d", socketId);
    std::lock_guard<std::mutex> lock(mPlayerMutex);

    for (auto it = mPlayers.begin(); it != mPlayers.end(); ++it)
    {
        if (it->first == socketId)
        {
            LOG(INFO, "Player with socketID %d removed", socketId);
            mPlayers.erase(it);
            return true;
        }
    }

    LOG(ERROR, "Player with socketID %d not found", socketId);
    return false;
}

Player &Game::getPlayer(int socketId)
{
    // LOG(INFO, "getPlayer() %d", socketId);
    std::lock_guard<std::mutex> lock(mPlayerMutex);

    for (auto &pair : mPlayers)
    {
        if (pair.first == socketId)
        {
            // LOG(INFO, "Player with socketID %d found", socketId);
            return *(pair.second);
        }
    }

    LOG(ERROR, "Player with socketID %d not found", socketId);
    throw std::runtime_error("Player with socketID " + std::to_string(socketId) + " not found");
}

bool Game::addMerchantOrder(const int socketID)
{
    LOG(INFO, "%d", socketID);
    {
        std::lock_guard<std::mutex> lock(mMerchantMutex);
        if (mMerchantOrder.size() >= mNumberOfPlayers)
        {
            return false;
        }
        mMerchantOrder.push(socketID);
    }
    return true;
}

/**
 * @brief create mNumberOfPlayer, mTotalRounds, mCurrentRound, mDeck, mLeftPile, mRightPile
 *
 * @return true
 * @return false
 */
bool Game::createGameDetails()
{
    mNumberOfPlayers = mPlayers.size();
    /* There are 3 players, each player will become sheriff 3 times, otherwise, each player will become sheriff 2 times */
    mPlayerSheriffTimes = (mNumberOfPlayers == 3) ? mNumberOfPlayers * 3 : mNumberOfPlayers * 2;
    mSheriffIndex = 0;
    if (mNumberOfPlayers < 3 || mNumberOfPlayers > 6)
    {
        printf("numberOfPlayer %d is invalid\n", mNumberOfPlayers);
        return false;
    }

    mDeck.clear();

    int appleCount = 48;
    int cheeseCount = 36;
    int breadCount = 36;
    int chickenCount = 24;

    int meadCount = 21;
    int silkCount = 12;
    int crossbowCount = 5;
    int pepperCount = 22;

    if (mNumberOfPlayers == 3)
    {
        breadCount -= 36;
        pepperCount -= 4;
        meadCount -= 5;
        silkCount -= 3;
    }

    // Add cards to the deck
    for (int i = 0; i < appleCount; ++i)
    {
        mDeck.emplace_back(CardName::APPLE);
    }
    for (int i = 0; i < cheeseCount; ++i)
    {
        mDeck.emplace_back(CardName::CHEESE);
    }
    for (int i = 0; i < breadCount; ++i)
    {
        mDeck.emplace_back(CardName::BREAD);
    }
    for (int i = 0; i < chickenCount; ++i)
    {
        mDeck.emplace_back(CardName::CHICKEN);
    }
    for (int i = 0; i < pepperCount; ++i)
    {
        mDeck.emplace_back(CardName::PEPPER);
    }
    for (int i = 0; i < meadCount; ++i)
    {
        mDeck.emplace_back(CardName::MEAD);
    }
    for (int i = 0; i < silkCount; ++i)
    {
        mDeck.emplace_back(CardName::SILK);
    }
    for (int i = 0; i < crossbowCount; ++i)
    {
        mDeck.emplace_back(CardName::CROSSBOW);
    }

    // Shuffle the deck
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(mDeck.begin(), mDeck.end(), g);

    // Take 5 random cards for mLeftPile and mRightPile
    for (int i = 0; i < 5; ++i)
    {
        mLeftPile.push_back(mDeck.back());
        mDeck.pop_back();
    }

    for (int i = 0; i < 5; ++i)
    {
        mRightPile.push_back(mDeck.back());
        mDeck.pop_back();
    }

    return true;
}

/* TODO: Handle the situation when Deck is empty */

CardName Game::withdrawDeck()
{
    std::lock_guard<std::mutex> lock(mDeckMutex);
    if (mDeck.empty())
    {
        printf("mDeck is empty, can't withdraw!\n");
        return INVALID_CARD;
    }
    CardName topCardOfDeck = mDeck.back();
    LOG(INFO, "Withdraw Deck %s", cardNameToString.at(topCardOfDeck).c_str());
    mDeck.pop_back();
    return topCardOfDeck;
}

CardName Game::withdrawPile(const int pile)
{
    CardName topCardOfPile = INVALID_CARD;
    if (pile == LEFT_PILE)
    {
        if (mLeftPile.empty())
        {
            LOG(ERROR, "mLeftPile is empty, can't withdraw!");
            return INVALID_CARD;
        }
        topCardOfPile = mLeftPile.back();
        mLeftPile.pop_back();
    }
    else if (pile == RIGHT_PILE)
    {
        if (mRightPile.empty())
        {
            LOG(ERROR, "mRightPile is empty, can't withdraw!");
            return INVALID_CARD;
        }
        topCardOfPile = mRightPile.back();
        mRightPile.pop_back();
    }
    else
    {
        LOG(ERROR, "invalid pile %d, can't withdraw!", pile);
        return INVALID_CARD;
    }
    LOG(INFO, "Withdraw %s Pile %s", (pile == LEFT_PILE ? "LeftPile" : "RightPile"), cardNameToString.at(topCardOfPile).c_str());
    return topCardOfPile;
}

bool Game::insertPile(const CardName insertCard, const int pile)
{
    if (insertCard > INVALID_CARD || insertCard < APPLE)
    {
        return false;
    }
    if (pile == LEFT_PILE)
    {
        mLeftPile.push_back(insertCard);
    }
    else if (pile == RIGHT_PILE)
    {
        mRightPile.push_back(insertCard);
    }
    else if (pile == MAIN_DECK)
    {
        mDeck.emplace(mDeck.begin(), insertCard);
    }
    else
    {
        printf("invalid pile %d, can't insert!\n", pile);
        return false;
    }
    return true;
}

std::vector<CardName> &Game::getDeck()
{
    return mDeck;
}

/* TODO: sort player by their collected point
top chicken, top bread,...got bonus point, please consider this situation
 */
std::vector<Player *> Game::findWinner()
{
}

bool Game::dealsCardToPlayers()
{
    LOG(INFO, "Deals cards to players");
    /* TODO: Conceal Cards value is safer, currently I'm exposing it */
    for (auto &pair : getAllPlayers())
    {
        Json::Value message;
        message["MessageType"] = "GAME_DEALS_CARDS";
        Json::Value cardsName(Json::arrayValue);
        const int curPlayerHandSize = pair.second->getHand().size();
        LOG(INFO, "Player %s is having %d cards in hand, need to deal %d more", pair.second->getName().c_str(), curPlayerHandSize, MAX_CARD_OF_PLAYER - curPlayerHandSize);
        for (int i = 0; i < MAX_CARD_OF_PLAYER - curPlayerHandSize; i++)
        {
            const CardName card = withdrawDeck();
            if (!pair.second->addCardToHand(card))
            {
                return false;
            }
            cardsName.append(getCardNameString(card));
        }
        message["Cards"] = cardsName;
        sendMessageToClient(jsonToString(message), pair.first);
    }
    return true;
}

bool Game::isPlayerNameTaken(const std::string &playerName)
{
    for (const auto &pair : mPlayers)
    {
        if (pair.second->getName() == playerName)
        {
            return true;
        }
    }
    return false;
}

void Game::sendMessageToClient(const std::string &message, const int socketId)
{
    // LOG(INFO, "Sending message to player %d: %s", socketId, message.c_str());
    Server::getInstance().sendToClient(message, socketId);
}

void Game::sendMessageToAll(const std::string &message)
{
    for (const auto &pair : mPlayers)
    {
        Server::getInstance().sendToClient(message, pair.first);
    }
}

void Game::sendMessageToAllExclude(const std::string &message, const int socketId)
{
    for (const auto &pair : mPlayers)
    {
        if (pair.first != socketId)
        {
            Server::getInstance().sendToClient(message, pair.first);
        }
    }
}

int Game::getPlayerSize()
{
    std::lock_guard<std::mutex> lock(mPlayerMutex);
    return mPlayers.size();
}
/**
 * @brief socketID to Player object
 *
 * @return const std::list<std::pair<int, std::unique_ptr<Player>>>&
 */
const std::list<std::pair<int, std::unique_ptr<Player>>> &Game::getAllPlayers()
{
    return mPlayers;
}

bool Game::isPlayerExists(const int socketId)
{
    std::lock_guard<std::mutex> lock(mPlayerMutex);
    for (const auto &pair : mPlayers)
    {
        if (pair.first == socketId)
        {
            return true;
        }
    }
    return false;
}

bool Game::isAllPlayerReady()
{
    std::lock_guard<std::mutex> lock(mPlayerMutex);
    for (const auto &pair : mPlayers)
    {
        if (pair.second->getState() != PLAYER_READY)
        {
            return false;
        }
    }
    return true;
}

std::vector<CardName> &Game::getPile(const int pile)
{
    if (pile == LEFT_PILE)
    {
        return mLeftPile;
    }
    else if (pile == RIGHT_PILE)
    {
        return mRightPile;
    }
    else
    {
        LOG(ERROR, "invalid pile %d, can't get!", pile);
        throw std::runtime_error("invalid pile " + std::to_string(pile) + ", can't get!");
    }
}

/**
 * @brief Increase mCurrentRound by 1. Check mPlayerOrder is empty and playerSheriffTimes is 0
 *
 * @return true
 * @return false
 */
bool Game::isGameEnded()
{
    if (mPlayerSheriffTimes == 0 && mMerchantOrder.empty())
    {
        return true;
    }
    return false;
}

/**
 * @brief Change the sheriff to the next player
 *
 * @return int
 */

bool Game::isSheriffTransfer()
{
    /* TODO: Handle the situation when Player is playing then disconnect to internet */
    if (mPlayerOrder.empty())
    {
        mPlayerSheriffTimes--;
        LOG(INFO, "mPlayerSheriffTimes %d", mPlayerSheriffTimes);
        int sheriffIndex = mSheriffIndex % mNumberOfPlayers;
        mSheriffIndex++;

        /* Get mSheriffSocketID */
        auto it = mPlayers.begin();
        std::advance(it, sheriffIndex);
        if (it->second != nullptr)
        {
            mSheriffSocketID = it->first;
            return true;
        }
        else
        {
            throw std::runtime_error("No valid players found to be sheriff");
        }

        return true;
    }
    return false;
}

int Game::getSheriffSocketID()
{
    return mSheriffSocketID;
}

int Game::getMerchantTurnSocketID()
{
    if (!mMerchantOrder.empty())
    {
        int merchantSocketID = mMerchantOrder.front();
        mMerchantOrder.pop();
        return merchantSocketID;
    }
    else
    {
        LOG(ERROR, "mMerchantOrder is empty, can't get merchant turn!");
        return -1;
    }
}

Bag &Game::getBag()
{
    return mBag;
}

bool Game::setBag(std::vector<CardName> &bagCards, const std::string bribe, const CardName declared, const std::string owner)
{
    mBag.mBagBribe = bribe;
    mBag.mBagDeclared = declared;
    mBag.mBagCards = bagCards;
    mBag.mBagOwner = owner;
#ifdef ENABLE_DEBUG
    std::string bagCardsString = "";
    for (const auto &card : bagCards)
    {
        bagCardsString += cardNameToString.at(card) + ", ";
    }
    bagCardsString.pop_back();
    bagCardsString.pop_back();

    LOG(INFO, "Bag's info: mBagBribe %d, mBagDeclared %s, mBagCards [%s], mBagOwner %s", bribe, cardNameToString.at(declared).c_str(), bagCardsString.c_str(), owner.c_str());
#endif
    return true;
}

bool Game::clearBag()
{
    (void)mBag.clearBag();
    return true;
}

/**
 * @brief This function will return 0 if the Merchant tells the truth, otherwise return the penalty
 *
 * @return int
 */
int Bag::calculatePenalty()
{
    int penalty = 0;
    for (const auto &pair : mBagCards)
    {
        if (pair != mBagDeclared)
        {
            penalty += cardPenalty.at(pair);
        }
    }
    return penalty;
}

bool Bag::clearBag()
{
    mBagCards.clear();
    mBagBribe = "0";
    mBagOwner = "";
    mBagDeclared = INVALID_CARD;
    return true;
}