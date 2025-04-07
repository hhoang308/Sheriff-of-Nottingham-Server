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
    void handleResponse(Game *curGame, const Json::Value &jsonMessage, const int socketID) override;
    void handleRequest(Game *curGame, const Json::Value &jsonMessage, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};