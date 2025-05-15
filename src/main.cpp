#include "Server.h"
#include "Card.h"
#include "Player.h"
#include "Game.h"
#include <iostream>
#include <thread>
#include <string>
#include <iostream>
#include <Log.h>
#include <filesystem>
#include <fstream>

int main(int argc, char *argv[])
{
    std::filesystem::create_directory("log");
    std::string filename = getLogFileName();
    freopen(filename.c_str(), "w", stdout);
    LOG(INFO, "Welcome to Sheriff of Nottingham!");
    Server &server = Server::getInstance();
    server.start();
    // int numberOfPlayer = 3;
    // Game* currentGame = new Game(numberOfPlayer);
    // std::vector<CardName> deck = currentGame->getDeck();

    // // Print the shuffled deck to verify
    // for (int i = 0; i < deck.size(); i++) {
    //     printf("%d. Card Name: %s, Type: %s\n", i + 1, getCardNameString(deck[i]).c_str(), getCardTypeString(deck[i]).c_str());
    // }
    // delete currentGame;

    return EXIT_SUCCESS;
}