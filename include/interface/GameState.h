#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <iostream>

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
};

#endif