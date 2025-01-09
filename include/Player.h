#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <algorithm>
#include "Card.h"

enum class Role {
    SHERIFF,
    MERCHANT
};

class Player {
public:
    // Constructor
    Player(const std::string& name, Role role);

    // Getters and setters
    void setName(std::string& name);
    std::string getName() const;
    void setRole(Role newRole);
    void setGold(int gold);

    Role getRole() const;
    int getGold() const;
    void addGold(int amount);
    void subtractGold(int amount);

    // Card management
    void addCardToHand(const CardName card);
    bool removeCardFromHand(const CardName card);
    void addCardToGoods(const CardName card);

    const std::vector<CardName>& getGoods() const;
    const std::vector<CardName>& getHand() const;
    /* TODO: a funtion to create instance for each player */

private:
    std::string name;
    Role role;
    int gold;
    std::vector<CardName> hand;
    std::vector<CardName> goods;

};

#endif // PLAYER_H