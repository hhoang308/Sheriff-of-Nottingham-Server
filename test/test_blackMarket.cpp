#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

const int MAX_CARD = 7;
const int SECOND_MAX_CARD = 5;

TEST(blackMarketCards, testGetBlackMarketCardSuccess)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {SILK, SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }
    bool isTraded = curGame->tradeContrabandToCards(1);
    curGame->calculatePoints();
    EXPECT_EQ(true, isTraded);
    EXPECT_EQ(50 + bonusBlackMarketCardTop.at(SILK), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardFailed)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();

    std::vector<CardName> merchantCards = {SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    bool isTraded = curGame->tradeContrabandToCards(1);
    curGame->calculatePoints();
    EXPECT_EQ(false, isTraded);
    EXPECT_EQ(50 + 2 * cardValue.at(SILK), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess2)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {SILK, SILK, SILK, SILK};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    bool isTraded = curGame->tradeContrabandToCards(1);
    curGame->calculatePoints();
    EXPECT_EQ(true, isTraded);
    EXPECT_EQ(50 + cardValue.at(SILK) + bonusBlackMarketCardTop.at(SILK), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess3)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {SILK, SILK, SILK, SILK, PEPPER, PEPPER, PEPPER};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    /* auto get highest value of black market card */
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    curGame->calculatePoints();
    EXPECT_EQ(50 + 3 * cardValue.at(PEPPER) + cardValue.at(SILK) + bonusBlackMarketCardTop.at(SILK), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess4)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {SILK, SILK, SILK, SILK, PEPPER, PEPPER, PEPPER};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    /* auto get highest value of black market card */
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    curGame->calculatePoints();
    EXPECT_EQ(50 + cardValue.at(SILK) + bonusBlackMarketCardTop.at(SILK) + bonusBlackMarketCardTop.at(PEPPER), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess5)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {SILK, SILK, SILK, SILK, PEPPER, PEPPER, PEPPER};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    /* auto get highest value of black market card */
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(false, curGame->tradeContrabandToCards(1));
    curGame->calculatePoints();
    EXPECT_EQ(50 + cardValue.at(SILK) + bonusBlackMarketCardTop.at(SILK) + bonusBlackMarketCardTop.at(PEPPER), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess6)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {PEPPER, PEPPER, PEPPER, PEPPER, PEPPER, PEPPER, PEPPER};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    /* auto get highest value of black market card */
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(false, curGame->tradeContrabandToCards(1));
    curGame->calculatePoints();
    EXPECT_EQ(50 + cardValue.at(PEPPER) + bonusBlackMarketCardBot.at(PEPPER) + bonusBlackMarketCardTop.at(PEPPER), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess7)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {MEAD, MEAD, MEAD, SILK, SILK, SILK, PEPPER, PEPPER, PEPPER};
    Player &player = curGame->getPlayer(1);
    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
    }

    /* auto get highest value of black market card */
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(false, curGame->tradeContrabandToCards(1));
    curGame->calculatePoints();
    EXPECT_EQ(50 + bonusBlackMarketCardTop.at(MEAD) + bonusBlackMarketCardTop.at(SILK) + bonusBlackMarketCardTop.at(PEPPER), player.getPlayerPoints());
}

TEST(blackMarketCards, testGetBlackMarketCardSuccess8)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    curGame->addPlayer(1, GAME_ID_DEFAULT, "Player 1");
    curGame->addPlayer(2, GAME_ID_DEFAULT, "Player 2");
    curGame->addPlayer(3, GAME_ID_DEFAULT, "Player 3");

    (void)curGame->createGameDetails();
    std::vector<CardName> merchantCards = {PEPPER, PEPPER, PEPPER};
    Player &player = curGame->getPlayer(1);
    Player &player2 = curGame->getPlayer(2);
    Player &player3 = curGame->getPlayer(3);

    for (const CardName card : merchantCards)
    {
        player.addCardToGoods(card);
        player2.addCardToGoods(card);
        player3.addCardToGoods(card);
    }

    /* auto get highest value of black market card */
    EXPECT_EQ(true, curGame->tradeContrabandToCards(1));
    EXPECT_EQ(true, curGame->tradeContrabandToCards(2));
    EXPECT_EQ(false, curGame->tradeContrabandToCards(3));

    curGame->calculatePoints();
    EXPECT_EQ(50 + bonusBlackMarketCardTop.at(PEPPER), player.getPlayerPoints());
    EXPECT_EQ(50 + bonusBlackMarketCardBot.at(PEPPER), player2.getPlayerPoints());
    EXPECT_EQ(50 + 3 * cardValue.at(PEPPER), player3.getPlayerPoints());
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

/* Normal: 1 King, 1 Queen */
TEST(blackMarketCards, TestKingQueenChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player1.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= SECOND_MAX_CARD; i++)
    {
        player2.addCardToGoods(CHICKEN);
    }

    curGame->calculatePoints();

    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + bonusPointsChampion.at(CHICKEN));
    EXPECT_EQ(player2.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * SECOND_MAX_CARD + bonusPointRunnerUp.at(CHICKEN));
    EXPECT_EQ(player3.getPlayerPoints(), 50);
    EXPECT_EQ(player4.getPlayerPoints(), 50);
}

/* Abnormal: 2 King */
TEST(blackMarketCards, TestKingKingChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player1.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player2.addCardToGoods(CHICKEN);
    }

    curGame->calculatePoints();

    int sharedBonus = (bonusPointsChampion.at(CHICKEN) + bonusPointRunnerUp.at(CHICKEN)) / 2;

    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + sharedBonus);
    EXPECT_EQ(player2.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + sharedBonus);
    EXPECT_EQ(player3.getPlayerPoints(), 50);
    EXPECT_EQ(player4.getPlayerPoints(), 50);
}

/* Abnormal: 1 King, 2 Queen */
TEST(blackMarketCards, TestKingQueenQueenChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player1.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= SECOND_MAX_CARD; i++)
    {
        player2.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= SECOND_MAX_CARD; i++)
    {
        player3.addCardToGoods(CHICKEN);
    }

    curGame->calculatePoints();

    int sharedBonus = bonusPointRunnerUp.at(CHICKEN) / 2;

    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + bonusPointsChampion.at(CHICKEN));
    EXPECT_EQ(player2.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * SECOND_MAX_CARD + sharedBonus);
    EXPECT_EQ(player3.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * SECOND_MAX_CARD + sharedBonus);
    EXPECT_EQ(player4.getPlayerPoints(), 50);
}

/* Abnormal: 1 King, 3 Queen */
TEST(blackMarketCards, TestKingQueenQueenQueenChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player1.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= SECOND_MAX_CARD; i++)
    {
        player2.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= SECOND_MAX_CARD; i++)
    {
        player3.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= SECOND_MAX_CARD; i++)
    {
        player4.addCardToGoods(CHICKEN);
    }

    curGame->calculatePoints();

    int sharedBonus = bonusPointRunnerUp.at(CHICKEN) / 3;

    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + bonusPointsChampion.at(CHICKEN));
    EXPECT_EQ(player2.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * SECOND_MAX_CARD + sharedBonus);
    EXPECT_EQ(player3.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * SECOND_MAX_CARD + sharedBonus);
    EXPECT_EQ(player4.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * SECOND_MAX_CARD + sharedBonus);
}

/* Abnormal: 3 King */
TEST(blackMarketCards, TestKingKingKingChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player1.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player2.addCardToGoods(CHICKEN);
    }

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player3.addCardToGoods(CHICKEN);
    }

    curGame->calculatePoints();

    int sharedBonus = (bonusPointRunnerUp.at(CHICKEN) + bonusPointsChampion.at(CHICKEN)) / 3;

    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + sharedBonus);
    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + sharedBonus);
    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + sharedBonus);
    EXPECT_EQ(player4.getPlayerPoints(), 50);
}

/* Abnormal: 1 King, 0 Queen */
TEST(blackMarketCards, TestKingChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    for (int i = 1; i <= MAX_CARD; i++)
    {
        player1.addCardToGoods(CHICKEN);
    }

    curGame->calculatePoints();

    EXPECT_EQ(player1.getPlayerPoints(), 50 + cardValue.at(CHICKEN) * MAX_CARD + bonusPointsChampion.at(CHICKEN));
    EXPECT_EQ(player2.getPlayerPoints(), 50);
    EXPECT_EQ(player3.getPlayerPoints(), 50);
    EXPECT_EQ(player4.getPlayerPoints(), 50);
}

/* Abnormal: 0 King, 0 Queen */
TEST(blackMarketCards, TestChicken)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player " + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    int player1_id = 0;
    int player2_id = 1;
    int player3_id = 2;
    int player4_id = 3;

    Player &player1 = curGame->getPlayer(player1_id);
    Player &player2 = curGame->getPlayer(player2_id);
    Player &player3 = curGame->getPlayer(player3_id);
    Player &player4 = curGame->getPlayer(player4_id);

    curGame->calculatePoints();

    EXPECT_EQ(player1.getPlayerPoints(), 50);
    EXPECT_EQ(player2.getPlayerPoints(), 50);
    EXPECT_EQ(player3.getPlayerPoints(), 50);
    EXPECT_EQ(player4.getPlayerPoints(), 50);
}