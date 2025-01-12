#include "Player.h"

// Constructor implementation
Player::Player(const std::string& name, Role role)
    : name(name), role(role), gold(50) {}  // Assuming each player starts with 50 gold

// Getters and setters implementation
std::string Player::getName() const {
    return name;
}

Role Player::getRole() const {
    return role;
}

void Player::setRole(Role newRole) {
    role = newRole;
}

int Player::getGold() const {
    return gold;
}

void Player::addGold(int amount) {
    gold += amount;
}

void Player::subtractGold(int amount) {
    gold -= amount;
}

// Card management implementation
void Player::addCardToHand(const Card& card) {
    hand.push_back(card);
}

bool Player::removeCardFromHand(const Card& card) {
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        if (it->getName() == card.getName()) {
            hand.erase(it);
            return true;
        }
    }
    return false;
}

const std::vector<Card>& Player::getHand() const {
    return hand;
}

void Player::addCardToGoods(const Card& card) {
    goods.push_back(card);
}

const std::vector<Card>& Player::getGoods() const {
    return goods;
}

void Player::setName(std::string& playerName){
    name = playerName;
}

void Player::setGold(int playerGold){
    gold = playerGold;
}