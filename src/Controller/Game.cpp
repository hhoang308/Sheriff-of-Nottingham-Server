#include "Game.h"

/* constructor */
Game::Game(const int numberOfPlayer){
    printf("Game created!\n");
    mDeck = createAndShuffleDeck(numberOfPlayer);
    mPlayerList = std::vector<Player*>();
    if(mDeck.empty()){
        printf("Can't create deck because of invalid size of player!\n");
    }else{
        printf("Deck created!\n");
    }
}

/* destructor */
Game::~Game(){
    printf("Game is deleted!\n");
}

// Function to create and shuffle the deck
/* TODO: create and shuffle deck based on the number of player */
/* TODO: verify this in game's rule */
std::vector<CardName> Game::createAndShuffleDeck(const int numberOfPlayer) {
    if(numberOfPlayer < 3
    || numberOfPlayer > 6){
        printf("numberOfPlayer %d is invalid\n", numberOfPlayer);
        return std::vector<CardName>();
    }
    std::vector<CardName> deck;

    int appleCount = 48;
    int cheeseCount = 36;
    int breadCount = 36;
    int chickenCount = 24;

    int meadCount = 21;
    int silkCount = 12;
    int crossbowCount = 5;
    int pepperCount = 22;

    if(numberOfPlayer == 3){
        breadCount -= 36;
        pepperCount -= 4;
        meadCount -= 5;
        silkCount -= 3;
    }

    // Add cards to the deck
    for (int i = 0; i < appleCount; ++i) {
        deck.emplace_back(CardName::APPLE);
    }
    for (int i = 0; i < cheeseCount; ++i) {
        deck.emplace_back(CardName::CHEESE);
    }
    for (int i = 0; i < breadCount; ++i) {
        deck.emplace_back(CardName::BREAD);
    }
    for (int i = 0; i < chickenCount; ++i) {
        deck.emplace_back(CardName::CHICKEN);
    }
    for (int i = 0; i < pepperCount; ++i) {
        deck.emplace_back(CardName::PEPPER);
    }
    for (int i = 0; i < meadCount; ++i) {
        deck.emplace_back(CardName::MEAD);
    }
    for (int i = 0; i < silkCount; ++i) {
        deck.emplace_back(CardName::SILK);
    }
    for (int i = 0; i < crossbowCount; ++i) {
        deck.emplace_back(CardName::CROSSBOW);
    }

    // Shuffle the deck
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

    return deck;
}

CardName Game::withdrawDeck(){
    if(mDeck.empty()){
        printf("mDeck is empty, can't withdraw!\n");
        return INVALID_CARD;
    }
    CardName topCardOfDeck = mDeck.front();
    mDeck.pop_back();
    return topCardOfDeck;
}

CardName Game::withdrawPile(const int pile){
    CardName topCardOfPile = INVALID_CARD;
    if(pile == LEFT_PILE){
        if(mLeftPile.empty()){
            printf("mLeftPile is empty, can't withdraw!\n");
            return INVALID_CARD;
        }
        topCardOfPile = mLeftPile.top();
        mLeftPile.pop();
    }else if(pile == RIGHT_PILE){
        if(mRightPile.empty()){
            printf("mRightPile is empty, can't withdraw!\n");
            return INVALID_CARD;
        }
        topCardOfPile = mRightPile.top();
        mRightPile.pop();
    }else{
        printf("invalid pile %d, can't withdraw!\n", pile);
        return CardName::INVALID_CARD;
    }
    return topCardOfPile;
}

bool Game::insertPile(const CardName insertCard, const int pile){
    if(insertCard > INVALID_CARD
    || insertCard < APPLE){
        return false;
    }
    if(pile == LEFT_PILE){
        mLeftPile.push(insertCard);
    }else if(pile == RIGHT_PILE){
        mRightPile.push(insertCard);
    }else{
        printf("invalid pile %d, can't insert!\n", pile);
        return false;
    }
    return true;
}

std::vector<CardName>& Game::getDeck(){
    return mDeck;
}

bool Game::insertPlayer(Player* player){
    if(mPlayerList.size() > MAX_NUMBER_OF_PLAYER){
        printf("Can't insert player, reached %d of player in a game", MAX_NUMBER_OF_PLAYER);
        return false;
    }
    mPlayerList.emplace_back(player);
    return true;
}

/* TODO: sort player by their collected point
top chicken, top bread,...got bonus point, please consider this situation
 */
std::vector<Player*> Game::findWinner(){
    std::vector<Player*> sortedPlayer = mPlayerList;

    std::sort(sortedPlayer.begin(), sortedPlayer.end(), 
        [](Player* lhs, Player* rhs) {
            return lhs->getPlayerPoints() > rhs->getPlayerPoints(); 
        }
    );

    return sortedPlayer;
}

bool Game::dealCardToEveryone(){
    for(int i = 0; i < MAX_CARD_OF_PLAYER; i++){
        for(auto player : mPlayerList){
            if(!mDeck.empty()){
                player->addCardToHand(mDeck.back());
                mDeck.pop_back();
            }else{
                printf("mDeck is empty, can't deal to player!\n");
                return false;
            }
        }
    }
    return true;
}