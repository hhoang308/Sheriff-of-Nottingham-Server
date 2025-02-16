#include "GameState.h"
#include <iostream>

void sendErrorMessage(int socketId, const std::string &messageType, const std::string &playerName, const std::string &reason);

class WaitingForPlayersState : public GameState
{
public:
    WaitingForPlayersState();
    ~WaitingForPlayersState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};

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

class WaitingForPlayerResponseState : public GameState
{
private:
public:
    WaitingForPlayerResponseState();
    ~WaitingForPlayerResponseState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};

class RoundStartedState : public GameState
{
public:
    RoundStartedState();
    ~RoundStartedState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};
class MerchantTurnState : public GameState
{
private:
    int mMerchantSocketID;

public:
    MerchantTurnState();
    ~MerchantTurnState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};

class SheriffTurnState : public GameState
{
public:
    SheriffTurnState();
    ~SheriffTurnState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};

class GameEndedState : public GameState
{
public:
    GameEndedState();
    ~GameEndedState();
    void handleRequest(Game *curGame, const std::string &message, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};