#ifndef GAME_H
#define GAME_H

#include "Card.h"
#include "Player.h"

#include <vector>
#include <algorithm>  // for std::shuffle
#include <random>     // for std::default_random_engine
#include <chrono>
#include <stack>


#define LEFT_PILE 1
#define RIGHT_PILE 2

class Game {
private:
    std::stack<CardName> mLeftPile;
    std::stack<CardName> mRightPile;
    std::vector<CardName> mDeck;
    std::vector<CardName> createAndShuffleDeck(const int numberOfPlayer);

public:
    Game(const int numberOfPlayer);
    ~Game();
    
    std::vector<CardName>& getDeck(); 
    CardName withdrawDeck();
    CardName withdrawPile(const int pile);
    bool insertPile(const CardName insertCard, const int pile);
};

#endif