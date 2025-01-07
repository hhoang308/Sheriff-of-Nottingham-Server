#ifndef CARD_H
#define CARD_H

#include <string>

enum class CardType {
    LEGAL,
    CONTRABAND
};

class Card {
public:
    // Constructor
    Card(const std::string& name, CardType type, int value, int penalty);

    // Getters
    std::string getName() const;
    CardType getType() const;
    int getValue() const;
    int getPenalty() const;

private:
    std::string name;
    CardType type;
    int value;
    int penalty;
};

#endif // CARD_H