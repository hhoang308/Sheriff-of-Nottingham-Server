#ifndef GAME_H
#define GAME_H

#include "Card.h"
#include "Player.h"
#include "GameState.h"
#include "Server.h"

#include <vector>
#include <algorithm>  // for std::shuffle
#include <random>     // for std::default_random_engine
#include <chrono>
#include <stack>
#include <queue>
#include <mutex>
#include <memory>
#include <list>
#include <utility>

#define LEFT_PILE 1
#define RIGHT_PILE 2

#define MAX_NUMBER_OF_PLAYER 6
#define MAX_CARD_OF_PLAYER 6

#define GAME_ID_DEFAULT 1234

class Game {
private:
    int mGameId;
    GameState* currentState;
    std::mutex mPlayerMutex;

    // std::unordered_map<std::string, Player*> playerData; /* playerName -> Player object */
    // std::unordered_map<int, std::string> socketToPlayer; /* socketID -> playerName */

    std::list<std::pair<int, std::unique_ptr<Player>>> mPlayers; /* socketID -> Player object */

    std::stack<CardName> mLeftPile;
    std::stack<CardName> mRightPile;
    std::vector<CardName> mDeck;
    std::queue<Player*> mPlayerOrder;

public:
    Game(const int gameId);
    ~Game();

    void setState(GameState* newState);
    void handleRequest(const std::string& message, const int socketId);

    bool addPlayer(const int socketID, const int gameID, const std::string& playerName);
    bool removePlayer(const int socketId);
    Player& getPlayer(const int socketId);

    int getGameId() const;
    const std::list<std::pair<int, std::unique_ptr<Player>>>& getAllPlayers();

    bool isPlayerNameTaken(const std::string& playerName);
    int getPlayerSize();

    std::vector<CardName> createAndShuffleDeck();
    bool dealCardToEveryone();
    std::vector<CardName>& getDeck();
    CardName withdrawDeck();
    CardName withdrawPile(const int pile);
    bool insertPile(const CardName insertCard, const int pile);
    std::vector<Player*> findWinner();

    void sendMessageToClient(const std::string& message, const int socketId);
    void sendMessageToAll(const std::string& message);
    void sendMessageToAllExclude(const std::string& message, const int socketId);
};

#endif