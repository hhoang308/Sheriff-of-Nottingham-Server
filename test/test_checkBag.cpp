#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

TEST(checkBagTest, TestSheriffCheck)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 2; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int sheriffSocketID = 0;
    int merchantSocketID = 1;

    std::vector<CardName> merchantCards = {CHICKEN, CHICKEN, SILK, SILK, SILK};
    curGame->setBag(merchantCards, "5", "", 0, CHICKEN, "Player 1", merchantSocketID);
    Bag curBag = curGame->getBag();

    curGame->calculatePenalty(sheriffSocketID, curBag, false);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50 - cardPenalty.at(SILK) * 3);
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50 + cardPenalty.at(SILK) * 3);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
}

TEST(checkBagTest, TestSheriffPass)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 2; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int sheriffSocketID = 0;
    int merchantSocketID = 1;

    std::vector<CardName> merchantCards = {CHICKEN, CHICKEN, APPLE, PEPPER, SILK};
    std::string bribe = "5";
    curGame->setBag(merchantCards, bribe, "", 0, CHICKEN, "Player 1", merchantSocketID);
    Bag curBag = curGame->getBag();

    curGame->calculatePenalty(sheriffSocketID, curBag, true);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50 - stoi(bribe));
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50 + stoi(bribe));
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(APPLE), 1);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(PEPPER), 1);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 1);
}