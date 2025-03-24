#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

TEST(blackMarketCards, testGetBlackMarketCard)
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