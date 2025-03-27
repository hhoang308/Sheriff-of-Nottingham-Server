#include "Game.h"
#include "WaitingForPlayersState.h"
#include "Utils.h"
#include <iostream>
#include <Log.h>
#include <memory>
#include <string>
#include <algorithm>

/**
 * @brief Construct a new Game:: Game object
 *
 * @param gameId
 */
Game::Game(const int gameId) : mGameId(gameId), isBlackMarketCardsApplied(true)
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
    LOG(INFO, "previousState %s", currentState->getStateName().c_str());
    delete currentState;
    currentState = newState;
    if (currentState)
    {
        currentState->enterState(this);
    }
    LOG(INFO, "currentState %s", newState->getStateName().c_str());
}

void Game::handleMessage(const std::string &message, const int socketId)
{
    if (currentState)
    {
        currentState->receiveMessage(this, message, socketId);
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
    // LOG(DEBUG, "getPlayer() %d", socketId);
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
 * @brief create mNumberOfPlayer, mTotalRounds, mCurrentRound, mDeck, mLeftPile, mRightPile, mBlackMarketCards
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

    if (isBlackMarketCardsApplied)
    {
        mBlackMarketCards[CardName::PEPPER] = 2;
        mBlackMarketCards[CardName::MEAD] = 2;
        mBlackMarketCards[CardName::SILK] = 2;
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
    LOG(DEBUG, "Withdraw Deck %s", cardNameToString.at(topCardOfDeck).c_str());
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
    return {};
}

void Game::calculatePoints()
{
    /* Calculate points from Legal Cards */
    std::vector<CardName> cardList = {APPLE, CHEESE, BREAD, CHICKEN, PEPPER, MEAD, SILK, CROSSBOW};
    for (auto &card : cardList)
    {
        int kingCount = 0;  /* Most card player have */
        int queenCount = 0; /* Second most card player have */

        int numberOfKing = 0;  /* number of player have the most card */
        int numberOfQueen = 0; /* number of player have the second most card */

        for (auto &player : mPlayers)
        {
            for (auto &good : player.second->getGoods())
            {
                if (good.first == card)
                {
#ifdef ENABLE_DEBUG
                    LOG(INFO, "Player %s have %d %s", player.second->getName().c_str(), good.second, cardNameToString.at(card).c_str());
#endif
                    player.second->addPlayerPoints(cardValue.at(card) * good.second);
                    if (good.second > kingCount)
                    {
                        queenCount = kingCount;
                        numberOfQueen = numberOfKing;
                        kingCount = good.second;
                        numberOfKing = 1;
                    }
                    else if (good.second == kingCount)
                    {
                        numberOfKing++;
                    }
                    else if (good.second > queenCount)
                    {
                        queenCount = good.second;
                        numberOfQueen = 1;
                    }
                    else if (good.second == queenCount)
                    {
                        numberOfQueen++;
                    }
                }
            }
        }

#ifdef ENABLE_DEBUG
        LOG(INFO, "King: %d, numberOfKing: %d, Queen: %d. numberOfQueen: %d", kingCount, numberOfKing, queenCount, numberOfQueen);
#endif

        /* No one have this card */
        if (kingCount == 0)
        {
            continue;
        }

        if (numberOfKing > 1) /* >= 2 King */
        {
            int sharedPrice = (bonusPointsChampion.at(card) + bonusPointRunnerUp.at(card)) / numberOfKing;
            for (auto &player : mPlayers)
            {
                for (auto &good : player.second->getGoods())
                {
                    if (good.first == card && good.second == kingCount)
                    {
                        player.second->addPlayerPoints(sharedPrice);
#ifdef ENABLE_DEBUG
                        LOG(INFO, "2 King: Player %s have %d %s, got %d points", player.second->getName().c_str(), good.second, cardNameToString.at(card).c_str(), sharedPrice);
#endif
                    }
                }
            }
        }
        else /* 1 King */
        {
            for (auto &player : mPlayers)
            {
                for (auto &good : player.second->getGoods())
                {
                    if (good.first == card && good.second == kingCount)
                    {
                        player.second->addPlayerPoints(bonusPointsChampion.at(card));
#ifdef ENABLE_DEBUG
                        LOG(INFO, "1 King: Player %s have %d %s, got %d points", player.second->getName().c_str(), good.second, cardNameToString.at(card).c_str(), bonusPointsChampion.at(card));
#endif
                    }
                }
            }
            if (queenCount > 1) /* >= 2 Queen */
            {
                int sharedPrice = bonusPointRunnerUp.at(card) / numberOfQueen;
                for (auto &player : mPlayers)
                {
                    for (auto &good : player.second->getGoods())
                    {
                        if (good.first == card && good.second == queenCount)
                        {
                            player.second->addPlayerPoints(sharedPrice);
#ifdef ENABLE_DEBUG
                            LOG(INFO, "2 Queen: Player %s have %d %s, got %d points", player.second->getName().c_str(), good.second, cardNameToString.at(card).c_str(), sharedPrice);
#endif
                        }
                    }
                }
            }
            else /* 1 Queen */
            {
                for (auto &player : mPlayers)
                {
                    for (auto &good : player.second->getGoods())
                    {
                        if (good.first == card && good.second == queenCount)
                        {
                            player.second->addPlayerPoints(bonusPointRunnerUp.at(card));
#ifdef ENABLE_DEBUG
                            LOG(INFO, "1 Queen: Player %s have %d %s, got %d points", player.second->getName().c_str(), good.second, cardNameToString.at(card).c_str(), bonusPointRunnerUp.at(card));
#endif
                        }
                    }
                }
            }
        }
    }

    /* Calculate points from Black Market Card */
    if (isBlackMarketCardsApplied)
    {
        for (auto &player : mPlayers)
        {
            for (auto &blackMarketCard : player.second->getBlackMarketCard())
            {
                if (blackMarketCard.second == BLACK_MARKET_CARD_BOT)
                {
                    player.second->addPlayerPoints(bonusBlackMarketCardBot.at(blackMarketCard.first));
                }
                else if (blackMarketCard.second == BLACK_MARKET_CARD_TOP)
                {
                    player.second->addPlayerPoints(bonusBlackMarketCardTop.at(blackMarketCard.first));
                }
                else if (blackMarketCard.second == BLACK_MARKET_CARD_BOTH)
                {
                    player.second->addPlayerPoints(bonusBlackMarketCardBot.at(blackMarketCard.first));
                    player.second->addPlayerPoints(bonusBlackMarketCardTop.at(blackMarketCard.first));
                }
                else
                {
                    LOG(ERROR, "Invalid Black Market Card value %d", blackMarketCard.second);
                }
            }
        }
    }
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
 * @brief The return value is 2 if Merchant is the first one get this type of Black Market Card, 1 if Merchant is the second one get this type of Black Market Card, 0 otherwise
 *
 * @param card
 * @return int
 */
int Game::getBlackMarketCards(const CardName card)
{
    std::lock_guard<std::mutex> lock(mBlackMarketCardsMutex);
    int result = mBlackMarketCards[card];
    if (result <= 0)
    {
        return 0;
    }
    mBlackMarketCards[card]--;
    return result;
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

int Game::getMerchantSocketID()
{
    return mMerchantSocketID;
}

/**
 * @brief Determine the next merchant, this function should be used once each turn/round
 *
 * @return int - mMerchantSocketID
 */
int Game::getMerchantTurnSocketID()
{
    if (!mMerchantOrder.empty())
    {
        mMerchantSocketID = mMerchantOrder.front();
        mMerchantOrder.pop();
        return mMerchantSocketID;
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

bool Game::setBag(std::vector<CardName> &bagCards, const std::string bribe, const CardName declared, const std::string owner, const int ownerSocketID)
{
    mBag.mBagBribe = bribe;
    mBag.mBagDeclared = declared;
    mBag.mBagCards = bagCards;
    mBag.mBagOwner = owner;
    mBag.mBagOwnerSocketID = ownerSocketID;
#ifdef ENABLE_DEBUG
    std::string bagCardsString = "";
    for (const auto &card : bagCards)
    {
        bagCardsString += cardNameToString.at(card) + ", ";
    }
    bagCardsString.pop_back();
    bagCardsString.pop_back();

    LOG(INFO, "Bag's info: mBagBribe %d, mBagDeclared %s, mBagCards [%s], mBagOwner %s", stoi(bribe), cardNameToString.at(declared).c_str(), bagCardsString.c_str(), owner.c_str());
#endif
    return true;
}

bool Game::clearBag()
{
    (void)mBag.clearBag();
    return true;
}

void Game::calculatePenalty(const int sheriffSocketID, Bag &bag, bool isPass)
{
    if (bag.isEmpty())
    {
        LOG(ERROR, "Invalid bag");
        return;
    }

    int bagOwnerSocketID = bag.mBagOwnerSocketID;
    Player &bagOwnerPlayer = getPlayer(bagOwnerSocketID);
    Player &sheriffPlayer = getPlayer(sheriffSocketID);

    if (isPass)
    {
        /* Sheriff receives money and passes this bag */
        bagOwnerPlayer.subtractGold(stoi(bag.mBagBribe));
        for (const auto &card : bag.mBagCards)
        {
            bagOwnerPlayer.addCardToGoods(card);
        }
        sheriffPlayer.addGold(stoi(bag.mBagBribe));
    }
    else
    {
        /* Sheriff wants to check the bag */
        for (const auto &card : bag.mBagCards)
        {
            if (card != bag.mBagDeclared)
            {
                sheriffPlayer.addGold(cardPenalty.at(card));
                bagOwnerPlayer.subtractGold(cardPenalty.at(card));
            }
            else
            {
                sheriffPlayer.subtractGold(cardPenalty.at(card));
                bagOwnerPlayer.addGold(cardPenalty.at(card));
                bagOwnerPlayer.addCardToGoods(card);
            }
        }
    }
}

bool Bag::clearBag()
{
    mBagCards.clear();
    mBagBribe = "0";
    mBagOwner = "";
    mBagDeclared = INVALID_CARD;
    return true;
}

bool Bag::isEmpty()
{
    return mBagCards.empty();
}

bool Game::tradeContrabandToCards(const int socketID)
{
    if (!isBlackMarketCardsApplied)
    {
        LOG(INFO, "Black market cards are not applied");
        return false;
    }
    const std::vector<CardName> contrabandCard = {SILK, MEAD, PEPPER};
    Player &player = getPlayer(socketID);
    for (const CardName &card : contrabandCard)
    {
        if (player.getGoods().count(card) > 0 && player.getGoods().at(card) >= CONTRABAND_TO_MARKET_CARDS_LIMIT)
        {
            for (int i = 0; i < CONTRABAND_TO_MARKET_CARDS_LIMIT; i++)
            {
                player.removeCardFromGoods(card);
            }
            player.addBlackMarketCard(card, getBlackMarketCards(card));
            return true;
        }
    }
    return false;
}