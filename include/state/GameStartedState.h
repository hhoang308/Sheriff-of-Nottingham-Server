#include "GameState.h"
#include <iostream>
#include <unordered_map>

class GameStartedState : public GameState
{
private:
    int mNumberOfPlayers;
    int mTotalRounds;
    int mCurrentRound;
    int mSheriffIndex;

public:
    GameStartedState();
    ~GameStartedState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};