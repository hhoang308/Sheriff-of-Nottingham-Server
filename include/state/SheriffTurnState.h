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
    void handleResponse(Game *curGame, const Json::Value& jsonMessage, const int socketID) override;
    void handleRequest(Game *curGame, const Json::Value& jsonMessage, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};