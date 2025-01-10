#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <algorithm>
#include "Card.h"

enum Role {
    SHERIFF,
    MERCHANT
};

class Player {
public:
    // Constructor
    Player(const std::string& name, Role role);

    // Getters and setters
    bool setName(std::string& name);
    std::string getName() const;
    bool setRole(Role newRole);
    bool setGold(int gold);

    Role getRole() const;
    int getGold() const;
    bool addGold(int amount);
    bool subtractGold(int amount);

    // Card management
    bool addCardToHand(const CardName card);
    bool removeCardFromHand(const CardName card);
    bool addCardToGoods(const CardName card);

    const std::vector<CardName>& getGoods() const;
    const std::vector<CardName>& getHand() const;
    /* TODO: a funtion to create instance for each player */

private:
    std::string mPlayerName;
    Role mPlayerRole;
    int mPlayerGold;
    std::vector<CardName> mPlayerHand;
    std::vector<CardName> mPlayerGoods;
};

#endif // PLAYER_H