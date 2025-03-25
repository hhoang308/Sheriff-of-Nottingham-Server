#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

TEST(blackMarketCards, testGetBlackMarketCardSuccess)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");

    std::vector<CardName> merchantCards = {SILK, SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    ASSERT_EQ(true, curGame->tradeContrabandToCards(1));
}

TEST(blackMarketCards, testGetBlackMarketCardFailed)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");

    std::vector<CardName> merchantCards = {SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    ASSERT_EQ(false, curGame->tradeContrabandToCards(1));
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess2)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");

    std::vector<CardName> merchantCards = {SILK, SILK, SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    ASSERT_EQ(true, curGame->tradeContrabandToCards(1));
}

TEST(blackMarketCards, testGetBlackMarketCardInit)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();

    EXPECT_EQ(BLACK_MARKET_CARD_TOP, curGame->getBlackMarketCards(SILK));
    EXPECT_EQ(BLACK_MARKET_CARD_TOP, curGame->getBlackMarketCards(PEPPER));
    EXPECT_EQ(BLACK_MARKET_CARD_TOP, curGame->getBlackMarketCards(MEAD));

    EXPECT_EQ(BLACK_MARKET_CARD_BOT, curGame->getBlackMarketCards(SILK));
    EXPECT_EQ(BLACK_MARKET_CARD_BOT, curGame->getBlackMarketCards(PEPPER));
    EXPECT_EQ(BLACK_MARKET_CARD_BOT, curGame->getBlackMarketCards(MEAD));

    EXPECT_EQ(NO_BLACK_MARKET_CARD, curGame->getBlackMarketCards(SILK));
    EXPECT_EQ(NO_BLACK_MARKET_CARD, curGame->getBlackMarketCards(PEPPER));
    EXPECT_EQ(NO_BLACK_MARKET_CARD, curGame->getBlackMarketCards(MEAD));

    EXPECT_EQ(NO_BLACK_MARKET_CARD, curGame->getBlackMarketCards(SILK));
    EXPECT_EQ(NO_BLACK_MARKET_CARD, curGame->getBlackMarketCards(PEPPER));
    EXPECT_EQ(NO_BLACK_MARKET_CARD, curGame->getBlackMarketCards(MEAD));
}

// TEST(checkPlayer, testGetPlayerInfo1)
// {
//     Game *curGame = new Game(GAME_ID_DEFAULT);

//     for (int i = 0; i < 2; i++)
//     {
//         std::string playerName = "Player " + std::to_string(i);
//         int socketID = i;
//         curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
//     }

//     int sheriffSocketID = 0;
//     int merchantSocketID = 1;

//     std::vector<CardName> merchantCards = {CHICKEN, CHICKEN, SILK, SILK, SILK};
//     curGame->setBag(merchantCards, "5", CHICKEN, "Player 1", merchantSocketID);
//     Bag curBag = curGame->getBag();

//     curGame->calculatePenalty(sheriffSocketID, curBag, true);

//     EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
//     EXPECT_EQ(50 - 5, curGame->getPlayer(1).getGold());
//     EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 0);
//     EXPECT_EQ(curGame->getPlayer(merchantSocketID).getBlackMarketCard().at(SILK), BLACK_MARKET_CARD_TOP);
// }