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
std::vector<Card> createAndShuffleDeck() {
    std::vector<Card> deck;

    // Add cards to the deck
    for (int i = 0; i < 48; ++i) {
        deck.emplace_back("Apple", CardType::LEGAL, 2, 2);
    }
    for (int i = 0; i < 36; ++i) {
        deck.emplace_back("Cheese", CardType::LEGAL, 3, 2);
    }
    for (int i = 0; i < 24; ++i) {
        deck.emplace_back("Chicken", CardType::LEGAL, 4, 2);
    }
    for (int i = 0; i < 18; ++i) {
        deck.emplace_back("Pepper", CardType::CONTRABAND, 8, 4);
    }
    for (int i = 0; i < 16; ++i) {
        deck.emplace_back("Mead", CardType::CONTRABAND, 7, 4);
    }
    for (int i = 0; i < 9; ++i) {
        deck.emplace_back("Silk", CardType::CONTRABAND, 9, 4);
    }
    for (int i = 0; i < 5; ++i) {
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
    std::vector<Card> deck = createAndShuffleDeck();

    // Print the shuffled deck to verify
    for (int i = 0; i < deck.size(); i++) {
        printf("%d. Card Name: %s, Type: %s\n", i + 1, deck[i].getName().c_str(), (deck[i].getType() == CardType::LEGAL ? "Legal" : "Contraband"));
    }

    return EXIT_SUCCESS;
}