#ifndef GAME_H
#define GAME_H

#include "Card.h"
#include "Player.h"
#include "GameState.h"
#include "Server.h"

#include <vector>
#include <algorithm> // for std::shuffle
#include <random>    // for std::default_random_engine
#include <chrono>
#include <stack>
#include <queue>
#include <mutex>
#include <memory>
#include <list>
#include <utility>

#define LEFT_PILE 1
#define RIGHT_PILE 2
#define MAIN_DECK 3

#define MAX_NUMBER_OF_PLAYER 6

#define GAME_ID_DEFAULT 1234

struct Bag
{
    std::vector<CardName> mBagCards;
    int mBagBribe;
    CardName mBagDeclared;
    std::string mBagOwner;

    int calculatePenalty();
    bool clearBag();
};
class Game
{
private:
    int mGameId;
    GameState *currentState;
    std::mutex mPlayerMutex;
    std::mutex mMerchantMutex;
    std::mutex mDeckMutex;

    int mNumberOfPlayers;
    int mPlayerSheriffTimes;
    int mSheriffSocketID;
    int mSheriffIndex;

    std::list<std::pair<int, std::unique_ptr<Player>>> mPlayers; /* socketID -> player object */
    std::queue<int> mMerchantOrder;                              /* Merchant's socketID */

    std::vector<CardName> mLeftPile;
    std::vector<CardName> mRightPile;
    std::vector<CardName> mDeck;
    std::queue<Player *> mPlayerOrder;
    Bag mBag;

public:
    Game(const int gameId);
    ~Game();

    void setState(GameState *newState);
    void handleRequest(const std::string &message, const int socketId);
    std::string getCurrentState();

    bool addPlayer(const int socketID, const int gameID, const std::string &playerName);
    bool removePlayer(const int socketId);
    Player &getPlayer(const int socketId);

    bool addMerchantOrder(const int socketID);
    int getMerchantTurnSocketID();

    bool isPlayerExists(const int socketId);
    bool isAllPlayerReady();

    int getGameId() const;
    const std::list<std::pair<int, std::unique_ptr<Player>>> &getAllPlayers();

    bool isPlayerNameTaken(const std::string &playerName);
    int getPlayerSize();

    bool createGameDetails();
    bool dealsCardToPlayers();

    bool isGameEnded();
    bool isSheriffTransfer();

    int getSheriffSocketID();

    std::vector<CardName> &getDeck();
    std::vector<CardName> &getPile(const int pile);

    CardName withdrawDeck();
    CardName withdrawPile(const int pile);
    bool insertPile(const CardName insertCard, const int pile);
    std::vector<Player *> findWinner();

    Bag &getBag();
    bool clearBag();
    bool setBag(std::vector<CardName> &bagCards, const int bribe, const CardName declared, const std::string owner);

    void sendMessageToClient(const std::string &message, const int socketId);
    void sendMessageToAll(const std::string &message);
    void sendMessageToAllExclude(const std::string &message, const int socketId);
};

#endif