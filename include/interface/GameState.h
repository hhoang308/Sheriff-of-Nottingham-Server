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
    virtual ~GameState()
    {
        std::cout << "GameState destructor called" << std::endl;
    };
    virtual std::string getStateName() const = 0;

    virtual void enterState(Game *server) = 0;

    virtual void handleRequest(Game *server, const Json::Value &jsonMessage, const int socketID) = 0;
    virtual void handleResponse(Game *server, const Json::Value &jsonMessage, const int socketID) = 0;

    virtual void receiveMessage(Game *server, const std::string &message, const int socketID)
    {
        // LOG(INFO, "socketID %d message '%s' ", socketID, message.c_str());

        Json::Value curJson = stringToJson(message);

        if (curJson.empty() || !curJson.isMember("MessageType") || curJson["MessageType"].isNull())
        {
            LOG(ERROR, "No MessageType");
            return;
        }

        std::string messageType = curJson["MessageType"].asString();
        if (messageType == "PLAYER_RESPONSE")
        {
            if (!curJson.isMember("ReasonType") || curJson["ReasonType"].isNull())
            {
                LOG(ERROR, "No ReasonType ");
                return;
            }
            handleResponse(server, curJson, socketID);
        }
        else
        {
            handleRequest(server, curJson, socketID);
        }
    }

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