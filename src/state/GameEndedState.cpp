#include "GameEndedState.h"
#include "Log.h"
#include "Utils.h"
#include "Game.h"
#include "Card.h"
#include <iostream>

/**
 * @brief Construct a new Game Ended State:: Game Ended State object
 *
 */

GameEndedState::GameEndedState()
{
    LOG(INFO, "GameEndedState initialized");
}

GameEndedState::~GameEndedState()
{
    LOG(INFO, "GameEndedState destructor called");
}

void GameEndedState::handleRequest(Game *curGame, const std::string &message, const int socketID)
{
    LOG(INFO, "GameEndedState::handleRequest() socketID %d message '%s' ", socketID, message.c_str());
}

void GameEndedState::enterState(Game *curGame)
{
    LOG(INFO, "GameEndedState::enterState()");
    /* TODO: Calculate who is the winner */
    curGame->calculatePoints();
    Json::Value endGameMessage;
    endGameMessage["MessageType"] = "GAME_END";
    /* TODO: change state of all players and disconnect all of them */
    curGame->sendMessageToAll(jsonToString(endGameMessage));
}

std::string GameEndedState::getStateName() const
{
    return "GameEndedState";
}