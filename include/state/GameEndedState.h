#include "GameState.h"
#include <iostream>
#include <unordered_map>

class GameEndedState : public GameState
{
public:
    GameEndedState();
    ~GameEndedState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};