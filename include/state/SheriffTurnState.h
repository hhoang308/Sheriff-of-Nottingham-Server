#include "GameState.h"
#include <iostream>
#include <unordered_map>

#define NUMBER_OF_UPDATE_INFO 2

class SheriffTurnState : public GameState
{
private:
    int mSheriffSocketID;
    std::unordered_map<int, int> mPlayerReceivedUpdateInfo; // socketID -> 0: not received, n: received n PLAYER_UPDATE_INFO

public:
    SheriffTurnState();
    ~SheriffTurnState();
    void handleResponse(Game *curGame, const Json::Value &jsonMessage, const int socketID) override;
    void handleRequest(Game *curGame, const Json::Value &jsonMessage, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};