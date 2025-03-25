#include "Card.h"

const std::unordered_map<CardName, std::string> cardNameToString = {
    {APPLE, "Apple"},
    {CHEESE, "Cheese"},
    {BREAD, "Bread"},
    {CHICKEN, "Chicken"},
    {PEPPER, "Pepper"},
    {MEAD, "Mead"},
    {SILK, "Silk"},
    {CROSSBOW, "Crossbow"}};

const std::unordered_map<std::string, CardName> stringToCardName = {
    {"Apple", APPLE},
    {"Cheese", CHEESE},
    {"Bread", BREAD},
    {"Chicken", CHICKEN},
    {"Pepper", PEPPER},
    {"Mead", MEAD},
    {"Silk", SILK},
    {"Crossbow", CROSSBOW}};

const std::unordered_map<CardName, CardType> cardNameToCardType = {
    {APPLE, LEGAL},
    {CHEESE, LEGAL},
    {BREAD, LEGAL},
    {CHICKEN, LEGAL},
    {PEPPER, CONTRABAND},
    {MEAD, CONTRABAND},
    {SILK, CONTRABAND},
    {CROSSBOW, CONTRABAND}};

const std::unordered_map<CardType, std::string> cardTypeToString = {
    {LEGAL, "Legal"},
    {CONTRABAND, "Contraband"}};

// Map to store card values and counts for each card type
const std::unordered_map<CardName, int> cardValue = {
    {APPLE, 2},
    {CHEESE, 3},
    {BREAD, 3},
    {CHICKEN, 4},
    {PEPPER, 8},
    {MEAD, 7},
    {SILK, 9},
    {CROSSBOW, 10}};

// Map to store card values and counts for each card type
const std::unordered_map<CardName, int> cardPenalty = {
    {APPLE, 2},
    {CHEESE, 2},
    {BREAD, 2},
    {CHICKEN, 2},
    {PEPPER, 4},
    {MEAD, 4},
    {SILK, 4},
    {CROSSBOW, 4}};

const std::unordered_map<CardName, int> bonusPointsChampion = {
    {APPLE, 20},
    {CHEESE, 15},
    {BREAD, 15},
    {CHICKEN, 10},
};

const std::unordered_map<CardName, int> bonusPointRunnerUp = {
    {APPLE, 10},
    {CHEESE, 10},
    {BREAD, 10},
    {CHICKEN, 5},
};

const std::unordered_map<CardName, int> bonusBlackMarketCardTop = {
    {PEPPER, 30},
    {MEAD, 32},
    {SILK, 34},
};

const std::unordered_map<CardName, int> bonusBlackMarketCardBot = {
    {PEPPER, 27},
    {MEAD, 29},
    {SILK, 31},
};

std::string getCardNameString(const CardName card)
{
    if (cardNameToString.find(card) == cardNameToString.end())
    {
        return "INVALID_CARD";
    }
    return cardNameToString.at(card);
}

std::string getCardTypeString(const CardName card)
{
    if (cardNameToCardType.find(card) == cardNameToCardType.end())
    {
        return "INVALID_CARD";
    }
    return cardTypeToString.at(cardNameToCardType.at(card));
}