#ifndef GAME_H
#define GAME_H

#include "Card.h"
#include "Player.h"

#include <vector>
#include <algorithm>  // for std::shuffle
#include <random>     // for std::default_random_engine
#include <chrono>
#include <stack>
#include <queue>

#define LEFT_PILE 1
#define RIGHT_PILE 2

#define MAX_NUMBER_OF_PLAYER 6
#define MAX_CARD_OF_PLAYER 6

enum GameState {
    GAME_WAITING,
    GAME_IN_PROGRESS,
    GAME_INVALID_STATE
};

class Game {
private:
    std::stack<CardName> mLeftPile;
    std::stack<CardName> mRightPile;
    std::vector<CardName> mDeck;
    std::vector<Player*> mPlayerList;
    std::queue<Player*> mPlayerOrder;
    
    std::vector<CardName> createAndShuffleDeck(const int numberOfPlayer);
    bool dealCardToEveryone();
    
public:
    Game(const int numberOfPlayer);
    ~Game();
    
    std::vector<CardName>& getDeck(); 
    CardName withdrawDeck();
    CardName withdrawPile(const int pile);
    bool insertPile(const CardName insertCard, const int pile);
    bool insertPlayer(Player* player);
    std::vector<Player*> findWinner();
};

#endif