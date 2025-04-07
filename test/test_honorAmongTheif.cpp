#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"
#include "MerchantTurnState.h"
#include "SheriffTurnState.h"

TEST(honorAmongTheif, honorAmongTheifWithCard)
{
    /*  Money : valid, Card : valid */
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 2; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int sheriffSocketID = 0;
    int merchantSocketID = 1;

    std::vector<CardName> merchantCards = {APPLE, CHICKEN, CHICKEN, SILK, SILK, SILK};
    curGame->setBag(merchantCards, "5", "Apple", 1, CHICKEN, "Player 1", merchantSocketID);
    Bag curBag = curGame->getBag();

    EXPECT_EQ(curGame->calculatePenalty(sheriffSocketID, curBag, true), 1);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50 - 5);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 3);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(APPLE), 0);

    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50 + 5);
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGoods().at(APPLE), 1);
}

TEST(honorAmongTheif, honorAmongTheifWithoutCard)
{
    /* Money : valid, Card : invalid */
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
    curGame->setBag(merchantCards, "5", "Bread", 1, CHICKEN, "Player 1", merchantSocketID);
    Bag curBag = curGame->getBag();

    curGame->calculatePenalty(sheriffSocketID, curBag, true);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50 - 5);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 3);

    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50 + 5);
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGoods().count(BREAD), 0);
}

TEST(honorAmongTheif, honorAmongTheifWithoutCard2)
{
    /* Money : valid, Card : 0 */
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 2; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int sheriffSocketID = 0;
    int merchantSocketID = 1;

    Player &player = curGame->getPlayer(merchantSocketID);
    player.addCardToGoods(BREAD);
    player.addCardToGoods(BREAD);

    std::vector<CardName> merchantCards = {CHICKEN, CHICKEN, SILK, SILK, SILK};
    curGame->setBag(merchantCards, "5", "Bread", 1, CHICKEN, "Player 1", merchantSocketID);
    Bag curBag = curGame->getBag();

    curGame->calculatePenalty(sheriffSocketID, curBag, true);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50 - 5);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 3);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(BREAD), 1);

    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50 + 5);
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGoods().at(BREAD), 1);
}

TEST(honorAmongTheif, honorAmongTheifWithoutMoney)
{
    /* Money : 0, Card : valid */
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 2; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int sheriffSocketID = 0;
    int merchantSocketID = 1;

    Player &player = curGame->getPlayer(merchantSocketID);
    player.addCardToGoods(BREAD);
    player.addCardToGoods(BREAD);

    std::vector<CardName> merchantCards = {CHICKEN, CHICKEN, SILK, SILK, SILK};
    curGame->setBag(merchantCards, "0", "Bread", 1, CHICKEN, "Player 1", merchantSocketID);
    Bag curBag = curGame->getBag();

    curGame->calculatePenalty(sheriffSocketID, curBag, true);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 3);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(BREAD), 1);

    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50);
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGoods().at(BREAD), 1);
}

TEST(honorAmongTheif, honorAmongTheifWithCardStateMachine)
{
    /*  Money : valid, Card : valid */
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = -1; i < 1; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int sheriffSocketID = 0;
    int merchantSocketID = -1;

    GameState *state = new MerchantTurnState();
    Player &player = curGame->getPlayer(merchantSocketID);
    Player &sheriff = curGame->getPlayer(sheriffSocketID);
    player.setState(PLAYER_TRADING);
    std::vector<CardName> merchantCards = {APPLE, CHICKEN, CHICKEN, SILK, SILK, SILK};
    for (const CardName card : merchantCards)
    {
        player.addCardToHand(card);
    }

    Json::Value message;
    message["MessageType"] = "MERCHANT_GIVE_BAG";
    message["PlayerName"] = player.getName();
    message["Amount"] = std::to_string(merchantCards.size());
    message["Report"] = "Chicken";
    Json::Value fee;
    fee["Money"] = "5";
    fee["Apple"] = "1";
    message["Fee"] = fee;
    Json::Value bag(Json::arrayValue);
    for (const CardName card : merchantCards)
    {
        bag.append(cardNameToString.at(card));
    }
    message["Bag"] = bag;

    state->handleRequest(curGame, message, merchantSocketID);

    sheriff.setState(PLAYER_INSPECTING);
    state = new SheriffTurnState();

    Json::Value message_s;
    message_s["MessageType"] = "SHERIFF_PASS";
    message_s["PlayerName"] = sheriff.getName();

    state->handleRequest(curGame, message_s, sheriffSocketID);

    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGold(), 50 - 5);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(CHICKEN), 2);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(SILK), 3);
    EXPECT_EQ(curGame->getPlayer(merchantSocketID).getGoods().at(APPLE), 0);

    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGold(), 50 + 5);
    EXPECT_EQ(curGame->getPlayer(sheriffSocketID).getGoods().at(APPLE), 1);
}

// TEST(honorAmongTheif, honorAmongTheifWithoutMoney2)
// {
//     /* Money : 0, Card : exceed limit */
// }

// TEST(honorAmongTheif, honorAmongTheifWithCardExceedLimit)
// {
//     /* Money : valid, Card : exceed limit */
// }