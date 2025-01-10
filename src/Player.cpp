#include "Player.h"

// Constructor implementation
Player::Player(const std::string& name, Role role)
    : mPlayerName(name), mPlayerRole(role), mPlayerGold(50) {
        mPlayerHand = std::vector<CardName>();
        mPlayerGoods = std::vector<CardName>();
    }  // Assuming each player starts with 50 gold

// Getters and setters implementation
std::string Player::getName() const {
    return mPlayerName;
}

Role Player::getRole() const {
    return mPlayerRole;
}

bool Player::setRole(Role newRole) {
    mPlayerRole = newRole;
    return true;
}

int Player::getGold() const {
    return mPlayerGold;
}

bool Player::addGold(int amount) {
    mPlayerGold += amount;
    return true;
}

bool Player::subtractGold(int amount) {
    int currentPlayerGold = mPlayerGold - amount;
    if(currentPlayerGold >= 0){
        mPlayerGold = currentPlayerGold;
        return true;
    }else{
        printf("Can't subtract player %s %d gold, mPlayerGold is %d\n", mPlayerName.c_str(), amount, mPlayerGold);
        return false;
    }
}

// Card management implementation
bool Player::addCardToHand(const CardName card) {
    mPlayerHand.push_back(card);
    return true;
}

bool Player::removeCardFromHand(const CardName card) {
    auto cardPosition = find(mPlayerHand.begin(), mPlayerHand.end(), card);
    if(cardPosition != mPlayerHand.end()){
        mPlayerHand.erase(cardPosition);
        return true;
    }
    printf("Can't find %s to delete\n!", getCardNameString(card).c_str());
    return false;
}

const std::vector<CardName>& Player::getHand() const {
    return mPlayerHand;
}

bool Player::addCardToGoods(const CardName card) {
    mPlayerGoods.push_back(card);
    return true;
}

const std::vector<CardName>& Player::getGoods() const {
    return mPlayerGoods;
}

bool Player::setName(std::string& playerName){
    mPlayerName = playerName;
    return true;
}

bool Player::setGold(int playerGold){
    mPlayerGold = playerGold;
    return true;
}