#include "GameState.h"
#include <iostream>
#include <unordered_map>

class MerchantTurnState : public GameState
{
private:
    enum MerchantState
    {
        MERCHANT_IDLE,
        MERCHANT_READY_TO_RECEIVE,
        MERCHANT_IS_RENDERING,
    };

    int mMerchantSocketID;
    int mNumberOfCards;
    MerchantState mMerchantState;

public:
    MerchantTurnState();
    ~MerchantTurnState();
    void handleResponse(Game *curGame, const Json::Value& jsonMessage, const int socketID) override;
    void handleRequest(Game *curGame, const Json::Value& jsonMessage, const int socketID) override;
    void enterState(Game *curGame) override;
    std::string getStateName() const override;
};