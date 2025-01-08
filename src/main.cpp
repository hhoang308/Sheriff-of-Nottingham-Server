#include "Server.h"
#include "Card.h"
#include "Player.h"
#include <iostream> 
#include <algorithm>  // for std::shuffle
#include <random>     // for std::default_random_engine
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

// Assuming Card and Player classes are already defined

// Function to create and shuffle the deck
/* TODO: create and shuffle deck based on the number of player */
/* TODO: verify this in game's rule */
std::vector<Card> createAndShuffleDeck(const int numberOfPlayer) {
    if(numberOfPlayer < 3
    || numberOfPlayer > 6){
        printf("numberOfPlayer %d is invalid\n", numberOfPlayer);
        return std::vector<Card>();
    }
    std::vector<Card> deck;

    int appleCount = 48;
    int cheeseCount = 36;
    int breadCount = 36;
    int chickenCount = 24;

    int meadCount = 21;
    int silkCount = 12;
    int crossbowCount = 5;
    int pepperCount = 22;

    if(numberOfPlayer == 3){
        // cheeseCount = 0;
        breadCount -= 36;
        // chickenCount = 0;

        pepperCount -= 4;
        meadCount -= 5;
        silkCount -= 3;
        // crossbowCount = 0;
    }

    // Add cards to the deck
    for (int i = 0; i < appleCount; ++i) {
        deck.emplace_back("Apple", CardType::LEGAL, 2, 2);
    }
    for (int i = 0; i < cheeseCount; ++i) {
        deck.emplace_back("Cheese", CardType::LEGAL, 3, 2);
    }
    for (int i = 0; i < breadCount; ++i) {
        deck.emplace_back("Bread", CardType::LEGAL, 3, 2);
    }
    for (int i = 0; i < chickenCount; ++i) {
        deck.emplace_back("Chicken", CardType::LEGAL, 4, 2);
    }
    for (int i = 0; i < pepperCount; ++i) {
        deck.emplace_back("Pepper", CardType::CONTRABAND, 8, 4);
    }
    for (int i = 0; i < meadCount; ++i) {
        deck.emplace_back("Mead", CardType::CONTRABAND, 7, 4);
    }
    for (int i = 0; i < silkCount; ++i) {
        deck.emplace_back("Silk", CardType::CONTRABAND, 9, 4);
    }
    for (int i = 0; i < crossbowCount; ++i) {
        deck.emplace_back("Crossbow", CardType::CONTRABAND, 10, 4);
    }

    // Shuffle the deck
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

    return deck;
}

int main(int argc, char* argv[]) {
    printf("Welcome to Sheriff of Nottingham!\n");

    // Create and shuffle the deck
    std::vector<Card> deck = createAndShuffleDeck(3);

    // Print the shuffled deck to verify
    for (int i = 0; i < deck.size(); i++) {
        printf("%d. Card Name: %s, Type: %s\n", i + 1, deck[i].getName().c_str(), (deck[i].getType() == CardType::LEGAL ? "Legal" : "Contraband"));
    }

    return EXIT_SUCCESS;
}