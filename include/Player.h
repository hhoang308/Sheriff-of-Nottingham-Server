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
    void addCardToHand(const Card& card);
    bool removeCardFromHand(const Card& card);
    void addCardToGoods(const Card& card);

    const std::vector<Card>& getGoods() const;
    const std::vector<Card>& getHand() const;

private:
    std::string name;
    Role role;
    int gold;
    std::vector<Card> hand;
    std::vector<Card> goods;

};

#endif // PLAYER_H