#include "Game.h"
#include "GameStateMachine.h"
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

void Game::setState(GameState* newState) {
    LOG(INFO, "previousState %s", newState->getStateName().c_str());
    delete currentState;
    currentState = newState;
    if(currentState) {
        currentState->enterState();
    }
    LOG(INFO, "currentState %s", currentState->getStateName().c_str());
}

void Game::handleRequest(const std::string& message, const int socketId) {
    if (currentState) {
        currentState->handleRequest(this, message, socketId);
    } else {
        LOG(ERROR, "No valid state to handle the message!");
    }
}

bool Game::addPlayer(const int socketID, const int gameID, const std::string& playerName) {
    LOG(INFO, "socketID %d gameID %d", socketID, gameID);
    {
        std::lock_guard<std::mutex> lock(mPlayerMutex);
        if (mPlayers.size() > MAX_NUMBER_OF_PLAYER) {
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

    for (auto it = mPlayers.begin(); it != mPlayers.end(); ++it) {
        if (it->first == socketId) {
            LOG(INFO, "Player with socketID %d removed", socketId);
            mPlayers.erase(it);
            return true;
        }
    }

    LOG(ERROR, "Player with socketID %d not found", socketId);
    return false;
}

Player& Game::getPlayer(int socketId)
{
    LOG(INFO, "getPlayer() %d", socketId);
    std::lock_guard<std::mutex> lock(mPlayerMutex);

    for (auto& pair : mPlayers) {
        if (pair.first == socketId) {
            LOG(INFO, "Player with socketID %d found", socketId);
            return *(pair.second);
        }
    }

    LOG(ERROR, "Player with socketID %d not found", socketId);
    throw std::runtime_error("Player with socketID " + std::to_string(socketId) + " not found");
}

// Function to create and shuffle the deck
/* TODO: create and shuffle deck based on the number of player */
/* TODO: verify this in game's rule */
std::vector<CardName> Game::createAndShuffleDeck()
{
    int numberOfPlayer = mPlayers.size();
    if (numberOfPlayer < 3 || numberOfPlayer > 6)
    {
        printf("numberOfPlayer %d is invalid\n", numberOfPlayer);
        return std::vector<CardName>();
    }
    std::vector<CardName> deck;

    int appleCount = 48;
    int cheeseCount = 36;
    int breadCount = 36;
    int chickenCount = 24;

    int meadCount = 21;
    int silkCount = 12;
    int crossbowCount = 5;
    int pepperCount = 22;

    if (numberOfPlayer == 3)
    {
        breadCount -= 36;
        pepperCount -= 4;
        meadCount -= 5;
        silkCount -= 3;
    }

    // Add cards to the deck
    for (int i = 0; i < appleCount; ++i)
    {
        deck.emplace_back(CardName::APPLE);
    }
    for (int i = 0; i < cheeseCount; ++i)
    {
        deck.emplace_back(CardName::CHEESE);
    }
    for (int i = 0; i < breadCount; ++i)
    {
        deck.emplace_back(CardName::BREAD);
    }
    for (int i = 0; i < chickenCount; ++i)
    {
        deck.emplace_back(CardName::CHICKEN);
    }
    for (int i = 0; i < pepperCount; ++i)
    {
        deck.emplace_back(CardName::PEPPER);
    }
    for (int i = 0; i < meadCount; ++i)
    {
        deck.emplace_back(CardName::MEAD);
    }
    for (int i = 0; i < silkCount; ++i)
    {
        deck.emplace_back(CardName::SILK);
    }
    for (int i = 0; i < crossbowCount; ++i)
    {
        deck.emplace_back(CardName::CROSSBOW);
    }

    // Shuffle the deck
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

    return deck;
}

CardName Game::withdrawDeck()
{
    if (mDeck.empty())
    {
        printf("mDeck is empty, can't withdraw!\n");
        return INVALID_CARD;
    }
    CardName topCardOfDeck = mDeck.front();
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
            printf("mLeftPile is empty, can't withdraw!\n");
            return INVALID_CARD;
        }
        topCardOfPile = mLeftPile.top();
        mLeftPile.pop();
    }
    else if (pile == RIGHT_PILE)
    {
        if (mRightPile.empty())
        {
            printf("mRightPile is empty, can't withdraw!\n");
            return INVALID_CARD;
        }
        topCardOfPile = mRightPile.top();
        mRightPile.pop();
    }
    else
    {
        printf("invalid pile %d, can't withdraw!\n", pile);
        return CardName::INVALID_CARD;
    }
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
        mLeftPile.push(insertCard);
    }
    else if (pile == RIGHT_PILE)
    {
        mRightPile.push(insertCard);
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

bool Game::dealCardToEveryone()
{
    for (int i = 0; i < MAX_CARD_OF_PLAYER; i++)
    {
        for (const auto& player : mPlayers)
        {
            if (!mDeck.empty())
            {
                player.second->addCardToHand(mDeck.back());
                mDeck.pop_back();
            }
            else
            {
                LOG(ERROR, "mDeck is empty, can't deal to player!");
                return false;
            }
        }
    }
    return true;
}

bool Game::isPlayerNameTaken(const std::string& playerName) {
    for (const auto& pair : mPlayers) {
        if (pair.second->getName() == playerName) {
            return true;
        }
    }
    return false;
}

void Game::sendMessageToClient(const std::string& message, const int socketId) {
    LOG(INFO, "Sending message to player %d: %s", socketId, message.c_str());
    Server::getInstance().sendToClient(message, socketId);
}

void Game::sendMessageToAll(const std::string& message) {
    for (const auto& pair : mPlayers) {
        Server::getInstance().sendToClient(message, pair.first);
    }
}

void Game::sendMessageToAllExclude(const std::string& message, const int socketId) {
    for (const auto& pair : mPlayers) {
        if (pair.first != socketId) {
            Server::getInstance().sendToClient(message, pair.first);
        }
    }
}

int Game::getPlayerSize(){
    std::lock_guard<std::mutex> lock(mPlayerMutex);
    return mPlayers.size();
}

const std::list<std::pair<int, std::unique_ptr<Player>>>& Game::getAllPlayers() {
    std::lock_guard<std::mutex> lock(mPlayerMutex);
    return mPlayers;
}