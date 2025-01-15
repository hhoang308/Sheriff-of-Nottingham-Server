#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <algorithm>
#include "Card.h"

/* TODO: consider change role to class sheriff and class merchant, write abtract function for each type */
enum Role {
    SHERIFF,
    MERCHANT
};

enum PlayerState {
    PLAYER_WAITING,
    PLAYER_READY,
    PLAYER_WITHDRAWING,
    PLAYER_DISCARDING,
    PLAYER_TRADING,
    PLAYER_RECEIVING,
    PLAYER_INVALID_STATE
};

class Player {
public:
    Player(const std::string& name, Role role);

    bool setName(std::string& name);
    std::string getName() const;
    bool setRole(Role newRole);
    bool setGold(int gold);

    Role getRole() const;
    int getGold() const;
    bool addGold(int amount);
    bool subtractGold(int amount);
    int getPlayerPoints();

    bool addCardToHand(const CardName card);
    bool removeCardFromHand(const CardName card);
    bool addCardToGoods(const CardName card);
    /* TODO: consider change bag to a different class*/
    bool addCardToBags(const CardName card);

    const std::unordered_map<CardName, int>& getGoods() const;
    const std::vector<CardName>& getHand() const;
    /* TODO: a funtion to create instance for each player */

private:
    int mPlayerColor; 
    PlayerState mPlayerState;
    std::string mPlayerName;
    Role mPlayerRole;
    int mPlayerGold;
    int mPlayerPoints;
    std::vector<CardName> mPlayerHand;
    std::vector<CardName> mPlayerBag;
    std::unordered_map<CardName, int> mPlayerGoods;
    int mPlayerSocket;
};

#endif // PLAYER_H