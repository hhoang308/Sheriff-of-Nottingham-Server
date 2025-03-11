#include "GameState.h"
#include <iostream>
#include <unordered_map>

class SheriffTurnState : public GameState
{
private:
    int mSheriffSocketID;

public:
    SheriffTurnState();
    ~SheriffTurnState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};