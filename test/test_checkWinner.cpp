#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

const int MAX_CARD = 7;
const int SECOND_MAX_CARD = 5;

/* Normal: 1 King, 1 Queen */
TEST(checkWinnerTest, TestKingQueenChicken)
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
TEST(checkWinnerTest, TestKingKingChicken)
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
TEST(checkWinnerTest, TestKingQueenQueenChicken)
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
TEST(checkWinnerTest, TestKingQueenQueenQueenChicken)
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
TEST(checkWinnerTest, TestKingKingKingChicken)
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
TEST(checkWinnerTest, TestKingChicken)
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
TEST(checkWinnerTest, TestChicken)
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