#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <iostream>

class Game;

class GameState
{
public:
    virtual void handleRequest(Game* server, const std::string& message, const int socketID) = 0;
    virtual void enterState() = 0;
    virtual std::string getStateName() const = 0;
    virtual ~GameState() {
        std::cout << "GameState destructor called" << std::endl;
    };
};

#endif