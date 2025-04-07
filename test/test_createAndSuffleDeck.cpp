#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"
#include "Log.h"

#define EXPECTED_DECK_SIZE_3PLAYER 156
#define EXPECTED_DECK_SIZE_4PLAYER 204

// Test case 1: Kiểm tra với số lượng người chơi là 3
TEST(createGameDetailsTest, TestWithThreePlayers)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();
    std::vector<CardName> deck = curGame->getDeck();
    std::vector<CardName> leftPile = curGame->getPile(LEFT_PILE);
    std::vector<CardName> rightPile = curGame->getPile(RIGHT_PILE);

    for (int i = 0; i < deck.size(); i++)
    {
        LOG(INFO, "Card %d. %s", i, cardNameToString.at(deck[i]).c_str());
    }

    for (int i = 0; i < leftPile.size(); i++)
    {
        LOG(INFO, "Left Pile %d. %s", i, cardNameToString.at(leftPile[i]).c_str());
    }

    for (int i = 0; i < rightPile.size(); i++)
    {
        LOG(INFO, "Right Pile %d. %s", i, cardNameToString.at(rightPile[i]).c_str());
    }

    EXPECT_EQ(deck.size(), EXPECTED_DECK_SIZE_3PLAYER - 5 * 2);
    EXPECT_EQ(leftPile.size(), 5);
    EXPECT_EQ(rightPile.size(), 5);
}

// Test case 2: Kiểm tra với số lượng người chơi là 4
TEST(createGameDetailsTest, TestWithFourPlayers)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 4; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();
    std::vector<CardName> deck = curGame->getDeck();
    std::vector<CardName> leftPile = curGame->getPile(LEFT_PILE);
    std::vector<CardName> rightPile = curGame->getPile(RIGHT_PILE);

    for (int i = 0; i < deck.size(); i++)
    {
        LOG(INFO, "Card %d. %s", i, cardNameToString.at(deck[i]).c_str());
    }

    for (int i = 0; i < leftPile.size(); i++)
    {
        LOG(INFO, "Left Pile %d. %s", i, cardNameToString.at(leftPile[i]).c_str());
    }

    for (int i = 0; i < rightPile.size(); i++)
    {
        LOG(INFO, "Right Pile %d. %s", i, cardNameToString.at(rightPile[i]).c_str());
    }

    EXPECT_EQ(deck.size(), EXPECTED_DECK_SIZE_4PLAYER - 5 * 2);
    EXPECT_EQ(leftPile.size(), 5);
    EXPECT_EQ(rightPile.size(), 5);
}

// Test case 3: Kiểm tra với số lượng người chơi không hợp lệ (ví dụ 7)
TEST(createGameDetailsTest, TestWithInvalidPlayerCount)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 7; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    std::vector<CardName> deck = curGame->getDeck();
    std::vector<CardName> leftPile = curGame->getPile(LEFT_PILE);
    std::vector<CardName> rightPile = curGame->getPile(RIGHT_PILE);
    // Kiểm tra xem bộ bài có rỗng không (vì số người chơi không hợp lệ)
    EXPECT_TRUE(deck.empty());
    EXPECT_TRUE(leftPile.empty());
    EXPECT_TRUE(rightPile.empty());
}

TEST(createGameDetailsTest, testRecreateDeckSuccess1)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();

    while (!curGame->getDeck().empty())
    {
        curGame->insertPile(curGame->withdrawDeck(), LEFT_PILE);
    }
    EXPECT_TRUE(curGame->recreateDeck());
    EXPECT_EQ(curGame->getDeck().size(), EXPECTED_DECK_SIZE_3PLAYER - 5 * 2);
    EXPECT_EQ(curGame->getPile(LEFT_PILE).size(), 5);
    EXPECT_EQ(curGame->getPile(RIGHT_PILE).size(), 5);
}

TEST(createGameDetailsTest, testRecreateDeckSuccess2)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();

    while (!curGame->getDeck().empty())
    {
        curGame->insertPile(curGame->withdrawDeck(), RIGHT_PILE);
    }
    EXPECT_TRUE(curGame->recreateDeck());
    EXPECT_EQ(curGame->getDeck().size(), EXPECTED_DECK_SIZE_3PLAYER - 5 * 2);
    EXPECT_EQ(curGame->getPile(LEFT_PILE).size(), 5);
    EXPECT_EQ(curGame->getPile(RIGHT_PILE).size(), 5);
}

TEST(createGameDetailsTest, testRecreateDeckSuccess3)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();

    while (!curGame->getDeck().empty())
    {
        curGame->insertPile(curGame->withdrawDeck(), RIGHT_PILE);
        if (!curGame->getDeck().empty())
        {
            curGame->insertPile(curGame->withdrawDeck(), LEFT_PILE);
        }
    }
    EXPECT_TRUE(curGame->recreateDeck());
    EXPECT_EQ(curGame->getDeck().size(), EXPECTED_DECK_SIZE_3PLAYER - 5 * 2);
    EXPECT_EQ(curGame->getPile(LEFT_PILE).size(), 5);
    EXPECT_EQ(curGame->getPile(RIGHT_PILE).size(), 5);
}

TEST(createGameDetailsTest, testRecreateDeckFail1)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();
    EXPECT_FALSE(curGame->recreateDeck());
}

TEST(createGameDetailsTest, testRecreateDeckFail2)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();

    while (!curGame->getDeck().empty())
    {
        CardName card = curGame->withdrawDeck();
    }
    EXPECT_FALSE(curGame->recreateDeck());
}

TEST(createGameDetailsTest, testRecreateDeckFail3)
{
    Game *curGame = new Game(GAME_ID_DEFAULT);

    for (int i = 0; i < 3; i++)
    {
        std::string playerName = "Player" + std::to_string(i);
        int socketID = i;
        curGame->addPlayer(socketID, GAME_ID_DEFAULT, playerName);
    }

    (void)curGame->createGameDetails();

    while (curGame->getDeck().size() > 1)
    {
        CardName card = curGame->withdrawDeck();
    }
    EXPECT_FALSE(curGame->recreateDeck());
}