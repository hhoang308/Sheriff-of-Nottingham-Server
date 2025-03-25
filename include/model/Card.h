#ifndef CARD_H
#define CARD_H

#include <string>
#include <unordered_map>

#define CARD_SIZE 8
#define MAX_CARD_OF_PLAYER 6
#define CONTRABAND_TO_MARKET_CARDS_LIMIT 3

enum CardType
{
    LEGAL,
    CONTRABAND
};

enum BlackMarketCard
{
    NO_BLACK_MARKET_CARD = 0, /* Merchant doesn't have any black market card */
    BLACK_MARKET_CARD_BOT,    /* Merchant has the second black market card */
    BLACK_MARKET_CARD_TOP,    /* Merchant has the first black market card */
    BLACK_MARKET_CARD_BOTH    /* Merchant has both the first and the second of the black market cards */
};

// Enum for card names, as there are only 8 types
enum CardName
{
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

extern const std::unordered_map<std::string, CardName> stringToCardName;

extern const std::unordered_map<CardName, CardType> cardNameToCardType;

extern const std::unordered_map<CardType, std::string> cardTypeToString;

// Map to store card values and counts for each card type
extern const std::unordered_map<CardName, int> cardValue;

// Map to store card values and counts for each card type
extern const std::unordered_map<CardName, int> cardPenalty;

extern const std::unordered_map<CardName, int> bonusPointsChampion;

extern const std::unordered_map<CardName, int> bonusPointRunnerUp;

extern const std::unordered_map<CardName, int> bonusBlackMarketCardTop; /* First Merchant gets 3 cards of the same Contraband type */

extern const std::unordered_map<CardName, int> bonusBlackMarketCardBot; /* Second Merchant gets 3 cards of the same Contraband type */

std::string getCardNameString(const CardName card);

std::string getCardTypeString(const CardName card);

#endif // CARD_H