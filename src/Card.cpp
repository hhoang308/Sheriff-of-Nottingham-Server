#include "Card.h"

// Constructor implementation
Card::Card(const std::string& name, CardType type, int value, int penalty)
    : name(name), type(type), value(value), penalty(penalty) {}

// Getter implementations
std::string Card::getName() const {
    return name;
}

CardType Card::getType() const {
    return type;
}

int Card::getValue() const {
    return value;
}

int Card::getPenalty() const {
    return penalty;
}