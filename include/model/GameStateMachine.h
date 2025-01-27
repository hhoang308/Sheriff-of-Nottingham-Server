#include "GameState.h"
#include <iostream>

void sendErrorMessage(int socketId, const std::string& messageType, const std::string& playerName, const std::string& reason);

class WaitingForPlayersState : public GameState
{
private:
    int mPlayerReadyCount;
public:
    WaitingForPlayersState();
    ~WaitingForPlayersState();
    void handleRequest(Game* server, const std::string& message, const int socketID) override;
    void enterState() override;
    std::string getStateName() const override;
};

class GameStartedState : public GameState
{
public:
    GameStartedState();
    ~GameStartedState();
    void handleRequest(Game* server, const std::string& message, const int socketID) override;
    void enterState() override;
    std::string getStateName() const override;
};

class WaitingForPlayerResponseState : public GameState
{
private:
    std::string currentRole;

public:
    WaitingForPlayerResponseState(const std::string &role);
    ~WaitingForPlayerResponseState() {};
    void handleRequest(Game* server, const std::string& message, const int socketID) override;
    void enterState() override;
    std::string getStateName() const override;
};

// class RoundStartedState : public GameState
// {
// public:
//     RoundStartedState();
//     ~RoundStartedState();
//     void handleRequest(Game* server, const std::string& message, const int socketID) override;
//     void enterState() override;
//     std::string getStateName() const override;
// };

// class AssisgnRolesState : public GameState
// {
// public:
//     AssisgnRolesState();
//     ~AssisgnRolesState();
//     void handleRequest(Game* server, const std::string& message, const int socketID) override;
//     void enterState() override;
//     std::string getStateName() const override;
// };

// class ChooseMerchantState : public GameState
// {
// public:
//     ChooseMerchantState();
//     ~ChooseMerchantState();
//     void handleRequest(Game* server, const std::string& message, const int socketID) override;
//     void enterState() override;
//     std::string getStateName() const override;
// };

// class chooseSheriffState : public GameState
// {
// public:
//     chooseSheriffState();
//     ~chooseSheriffState();
//     void handleRequest(Game* server, const std::string& message, const int socketID) override;
//     void enterState() override;
//     std::string getStateName() const override;
// };

// class RoundEndedState : public GameState
// {
// public:
//     RoundEndedState();
//     ~RoundEndedState();
//     void handleRequest(Game* server, const std::string& message, const int socketID) override;
//     void enterState() override;
//     std::string getStateName() const override;
// };

// class GameEndedState : public GameState
// {
// public:
//     GameEndedState();
//     ~GameEndedState();
//     void handleRequest(Game* server, const std::string& message, const int socketID) override;
//     void enterState() override;
//     std::string getStateName() const override;
// };