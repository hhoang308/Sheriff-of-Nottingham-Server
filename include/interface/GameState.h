#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <iostream>
#include <Log.h>
#include <Utils.h>

class Game;

class GameState
{
public:
    /* TODO: Handle the situation when 'message' doesn't contain type, which will crash the system*/
    virtual void handleRequest(Game *server, const std::string &message, const int socketID) = 0;
    virtual void enterState(Game *server) = 0;
    virtual std::string getStateName() const = 0;
    virtual ~GameState()
    {
        std::cout << "GameState destructor called" << std::endl;
    };

    std::string createErrorMessage(const std::string &messageType, const std::string &playerName, const std::string &reason)
    {
        Json::Value message;
        message["MessageType"] = messageType;
        message["PlayerName"] = playerName;
        message["Reason"] = reason;

        std::string jsonString = jsonToString(message);
        LOG(INFO, "createErrorMessage() %s", jsonString.c_str());
        return jsonString;
    }
};

#endif