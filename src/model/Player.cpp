#include "Player.h"
#include "Log.h"

/* TODO: set player color in constructor, with color format */
Player::Player(const int socketID, const int gameID, const std::string &playerName)
    : mPlayerGold(50), mPlayerState(PLAYER_WAITING), mSocketID(socketID), mGameID(gameID), mPlayerName(playerName)
{
    LOG(INFO, "Player %s created with socketID %d, gameID %d", playerName.c_str(), socketID, gameID);
    mPlayerHand = std::vector<CardName>();
    mPlayerGoods = std::unordered_map<CardName, int>();
    mPlayerBag = std::vector<CardName>();
} // Assuming each player starts with 50 gold

std::string Player::getName() const
{
    return mPlayerName;
}

Role Player::getRole() const
{
    return mPlayerRole;
}

bool Player::setRole(Role newRole)
{
    mPlayerRole = newRole;
    return true;
}

bool Player::setState(PlayerState newState)
{
    // LOG(INFO, "Player %s state changed to %d", mPlayerName.c_str(), newState);
    mPlayerState = newState;
    return true;
}

PlayerState Player::getState() const
{
    return mPlayerState;
}

int Player::getGold() const
{
    return mPlayerGold;
}

int Player::getSocketID() const
{
    return mSocketID;
}

int Player::getGameID() const
{
    return mGameID;
}

bool Player::addGold(int amount)
{
    mPlayerGold += amount;
    mPlayerPoints += amount;
    return true;
}

bool Player::subtractGold(int amount)
{
    // int currentPlayerGold = mPlayerGold - amount;
    // if (currentPlayerGold >= 0)
    // {
    //     mPlayerGold = currentPlayerGold;
    //     mPlayerPoints -= amount;
    //     return true;
    // }
    // else
    // {
    //     printf("Can't subtract player %s %d gold, mPlayerGold is %d\n", mPlayerName.c_str(), amount, mPlayerGold);
    //     return false;
    // }

    /* Enable negative gold */
    mPlayerGold = mPlayerGold - amount;
    mPlayerPoints -= amount;
    return true;
}

bool Player::addCardToHand(const CardName card)
{
    if (card == INVALID_CARD)
    {
        return false;
    }
    LOG(INFO, "%s", cardNameToString.at(card).c_str());
    mPlayerHand.push_back(card);
    return true;
}

bool Player::removeCardFromHand(const CardName card)
{
    auto cardPosition = find(mPlayerHand.begin(), mPlayerHand.end(), card);
    if (cardPosition != mPlayerHand.end())
    {
        mPlayerHand.erase(cardPosition);
        return true;
    }
    printf("Can't find %s to delete\n!", getCardNameString(card).c_str());
    return false;
}

const std::vector<CardName> &Player::getHand() const
{
    return mPlayerHand;
}

bool Player::addCardToGoods(const CardName card)
{
    mPlayerGoods[card]++;
    mPlayerPoints += cardValue.at(card);
    return true;
}

const std::unordered_map<CardName, int> &Player::getGoods() const
{
    return mPlayerGoods;
}

bool Player::setName(std::string &playerName)
{
    LOG(INFO, "Player name set to %s", playerName.c_str());
    mPlayerName = playerName;
    return true;
}

bool Player::setGold(int playerGold)
{
    mPlayerGold = playerGold;
    return true;
}

int Player::getPlayerPoints()
{
    return mPlayerPoints;
}