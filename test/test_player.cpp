#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

TEST(checkPlayer, testGetPlayerInfo)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");

    std::vector<CardName> merchantCards = {CHICKEN, CHICKEN, SILK, SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    Json::Value jsonValue;
    jsonValue["MessageType"] = "PLAYER_UPDATE_INFO";
    jsonValue["PlayerName"] = "Player 1";
    jsonValue["Money"] = "50";

    Json::Value cardList;
    cardList["Apple"] = "0";
    cardList["Cheese"] = "0";
    cardList["Bread"] = "0";
    cardList["Chicken"] = "2";
    cardList["Pepper"] = "0";
    cardList["Mead"] = "0";
    cardList["Silk"] = "3";
    cardList["Crossbow"] = "0";
    jsonValue["Cards"] = cardList;

    Json::Value blackCardList;
    blackCardList["Pepper"] = "0";
    blackCardList["Mead"] = "0";
    blackCardList["Silk"] = "0";
    jsonValue["BlackMarketBonus"] = blackCardList;

    ASSERT_EQ(jsonToString(curGame->getPlayer(1).getPlayerInfo()), jsonToString(jsonValue));
}