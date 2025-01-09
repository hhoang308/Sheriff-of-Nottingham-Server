#include "gtest/gtest.h"
#include "Server.h"  // Include file Server.h để có thể gọi hàm createAndShuffleDeck

// Test case 1: Kiểm tra với số lượng người chơi là 3
TEST(CreateAndShuffleDeckTest, TestWithThreePlayers) {
    std::vector<Card> deck = createAndShuffleDeck(3);

    // Kiểm tra kích thước bộ bài
    EXPECT_EQ(deck.size(), expectedDeckSizeFor3Players);

    // Kiểm tra xem có đúng các loại bài cần có trong bộ bài không (ví dụ Pepper, Mead, Silk...)
    // EXPECT_TRUE(verifyCardCount(deck, "Pepper", expectedPepperCountFor3Players));
}

// Test case 2: Kiểm tra với số lượng người chơi là 4
TEST(CreateAndShuffleDeckTest, TestWithFourPlayers) {
    std::vector<Card> deck = createAndShuffleDeck(4);
    
    // Kiểm tra kích thước bộ bài
    EXPECT_EQ(deck.size(), expectedDeckSizeFor4Players);
}

// Test case 3: Kiểm tra với số lượng người chơi không hợp lệ (ví dụ 7)
TEST(CreateAndShuffleDeckTest, TestWithInvalidPlayerCount) {
    std::vector<Card> deck = createAndShuffleDeck(7);

    // Kiểm tra xem bộ bài có rỗng không (vì số người chơi không hợp lệ)
    EXPECT_TRUE(deck.empty());
}