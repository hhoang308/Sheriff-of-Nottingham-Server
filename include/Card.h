#ifndef CARD_H
#define CARD_H

#include <string>
#include <unordered_map>

#define CARD_SIZE 8

enum CardType {
    LEGAL,
    CONTRABAND
};

// Enum for card names, as there are only 8 types
enum CardName {
    APPLE,
    CHEESE,
    BREAD,
    CHICKEN,
    PEPPER,
    MEAD,
    SILK,
    CROSSBOW,
    INVALID_CARD
};

extern const std::unordered_map<CardName, std::string> cardNameToString;

extern const std::unordered_map<CardName, CardType> cardNameToCardType;

extern const std::unordered_map<CardType, std::string> cardTypeToString;

// Map to store card values and counts for each card type
extern const std::unordered_map<CardName, int> cardValue;

// Map to store card values and counts for each card type
extern const std::unordered_map<CardName, int> cardPenalty;

std::string getCardNameString(const CardName card);

std::string getCardTypeString(const CardName card);

#endif // CARD_H