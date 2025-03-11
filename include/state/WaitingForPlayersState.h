#include "GameState.h"
#include <iostream>
#include <unordered_map>

class WaitingForPlayersState : public GameState
{
private:
    std::unordered_map<int, std::string> mPlayerNameTemp; /* socketID - playerName, before accept client */
public:
    WaitingForPlayersState();
    ~WaitingForPlayersState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};