#include "gtest/gtest.h"
#include "Game.h"
#include "Card.h"

#define EXPECTED_DECK_SIZE_3PLAYER 156
#define EXPECTED_DECK_SIZE_4PLAYER 204

// Test case 1: Kiểm tra với số lượng người chơi là 3
TEST(CreateAndShuffleDeckTest, TestWithThreePlayers) {
    Game* curGame = new Game(3);
    std::vector<CardName> deck = curGame->getDeck();

    // Kiểm tra kích thước bộ bài
    EXPECT_EQ(deck.size(), EXPECTED_DECK_SIZE_3PLAYER);

    // Kiểm tra xem có đúng các loại bài cần có trong bộ bài không (ví dụ Pepper, Mead, Silk...)
    // EXPECT_TRUE(verifyCardCount(deck, "Pepper", expectedPepperCountFor3Players));
}

// Test case 2: Kiểm tra với số lượng người chơi là 4
TEST(CreateAndShuffleDeckTest, TestWithFourPlayers) {
    Game* curGame = new Game(4);
    std::vector<CardName> deck = curGame->getDeck();

    // Kiểm tra kích thước bộ bài
    EXPECT_EQ(deck.size(), EXPECTED_DECK_SIZE_4PLAYER);
}

// Test case 3: Kiểm tra với số lượng người chơi không hợp lệ (ví dụ 7)
TEST(CreateAndShuffleDeckTest, TestWithInvalidPlayerCount) {
    Game* curGame = new Game(7);
    std::vector<CardName> deck = curGame->getDeck();

    // Kiểm tra xem bộ bài có rỗng không (vì số người chơi không hợp lệ)
    EXPECT_TRUE(deck.empty());
}