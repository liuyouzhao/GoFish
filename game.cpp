#include "game.h"
#include <vector>
#include <iostream>

using namespace std;

#define WAIT_SECONDS 1

int AI::askNumber(vector<Card*> cards)
{
    int hash[15] = {0};
    int max = 0;
    int maxIndex = 0;
    for(int i = 0; i < cards.size(); i ++)
    {
        hash[cards[i]->number] += 1;
    }

    for(int i = 0; i < 15; i ++)
    {
        if(max < hash[i])
        {
            max = hash[i];
            maxIndex = i;
        }
    }

    return maxIndex;
}

int AI::askPlayer(vector<Player*> players)
{
    int maxPlayerCards = 0;
    int maxIndex = 0;
    for(int i = 0; i < players.size(); i ++)
    {
        if(i == playerId || i == this->lastAsked)
            continue;
        if(maxPlayerCards < players[i]->cards.size())
        {
            maxPlayerCards = players[i]->cards.size();
            maxIndex = i;
        }
    }

    this->lastAsked = maxIndex;

    return lastAsked;
}

Game::Game():
    currentTimerId(0),
    humanPlayerIndex(0),
    winnerPlayer(-1),
    timer(this)
{
    char *cardsT[13] = {
        "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", "2"
    };
    for(int i = 0; i < 13; i ++)
    {
        cardTexts[i] = string(cardsT[i]);
    }
}

void Game::nextPlayer()
{
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    if(currentPlayerIndex == humanPlayerIndex)
    {
        requirePlayer = -1;
        requireNumber = -1;
        switchGameState(GS_PLAYER_ASKING_PLAYER);
        waitPlayerMotion = true;
    }
    else
    {
        requirePlayer = players[currentPlayerIndex]->ai->askPlayer(players);
        requireNumber = players[currentPlayerIndex]->ai->askNumber(players[currentPlayerIndex]->cards);
        switchGameState(GS_ASKING);
        if(requirePlayer != humanPlayerIndex)
        {
            timer.startTimeout(1000 * WAIT_SECONDS);
        }
        else
        {
            waitPlayerMotion = true;
        }
    }
}

void Game::againPlayer()
{
    if(currentPlayerIndex != humanPlayerIndex)
    {
        requirePlayer = players[currentPlayerIndex]->ai->askPlayer(players);
        requireNumber = players[currentPlayerIndex]->ai->askNumber(players[currentPlayerIndex]->cards);
        switchGameState(GS_ASKING);
        if(requirePlayer != humanPlayerIndex)
        {
            timer.startTimeout(1000 * WAIT_SECONDS);
        }
        else
        {
            waitPlayerMotion = true;
        }
    }
    else
    {
        requireNumber = -1;
        requirePlayer = -1;
        switchGameState(GS_PLAYER_ASKING_PLAYER);
    }

}

/**
 * Timer driver
 * @brief Game::onTimerCallback
 */
void Game::onTimerCallback()
{
    waitPlayerMotion = false;
    switch(gameState)
    {
    case GS_ANSWERING_CARD:
        switchGameState(GS_COLLECTING);
        timer.startTimeout(1000 * WAIT_SECONDS);
        break;
    case GS_ANSWERING_FISH:
        switchGameState(GS_FISHING);
        timer.startTimeout(1000 * WAIT_SECONDS);
        break;
    case GS_ASKING:
    {
        if(requirePlayer != humanPlayerIndex)
        {
            timer.startTimeout(1000 * WAIT_SECONDS);
            vector<Card*> reqs = players[requirePlayer]->getRequiredCards(requireNumber);
            if(reqs.size() == 0)
            {
                /* Fish! */
                switchGameState(GS_ANSWERING_FISH);
            }
            else
            {
                /* Give the cards */
                switchGameState(GS_ANSWERING_CARD);
                players[requirePlayer]->removeCards(reqs);
                givingCards = reqs;
            }
        }
        break;
    }
    case GS_FISHING:
    {
        int randomCardIndex = rand() % this->tableCards.size();
        Card *newCard = this->removeTableCard(randomCardIndex);

        players[currentPlayerIndex]->tryIfWinCards(newCard);
        if(this->tableCards.size() == 0)
        {
            int maxCards = 0;
            int maxIndex = 0;
            for(int i = 0; i < players.size(); i ++)
            {
                if(maxCards < players[i]->winCards.size())
                {
                    maxCards = players[i]->winCards.size();
                    maxIndex = i;
                }
            }
            winnerPlayer = maxIndex;
            switchGameState(GS_OVER);
            return;
        }

        nextPlayer();
        break;
    }
    case GS_COLLECTING:


        bool gotScore = players[currentPlayerIndex]->tryIfWinCards(givingCards);
        givingCards.clear();

        if(players[currentPlayerIndex]->cards.size() == 0)
        {
            winnerPlayer = currentPlayerIndex;
            switchGameState(GS_OVER);
            return;
        }

        if(!gotScore)
        {
            nextPlayer();
        }
        else
        {
            againPlayer();
        }
        break;
    }
    if(gameState == GS_PLAYER_ASKING_CARD || gameState == GS_PLAYER_ASKING_PLAYER)
        waitPlayerMotion = true;
}

/**
 * Mouse event driver
 * @brief Game::onClickPlayer
 * @param playerId
 */
void Game::onClickPlayer(int playerId)
{
    cout << "onClickPlayer " << playerId << endl;
    switch(gameState)
    {
    case GS_PLAYER_ASKING_PLAYER:
    case GS_PLAYER_ASKING_CARD:
    {
        if(playerId == humanPlayerIndex)
            return;

        requirePlayer = playerId;
        switchGameState(GS_PLAYER_ASKING_CARD);
        break;
    }
    }
}

void Game::onClickCard(int number)
{
    cout << "onClickCard " << number << endl;
    switch(gameState)
    {
    case GS_ASKING:
    {
        timer.startTimeout(1000 * WAIT_SECONDS);
        vector<Card*> reqs = players[requirePlayer]->getRequiredCards(requireNumber);
        if(reqs.size() == 0)
        {
            /* Fish! */
            switchGameState(GS_ANSWERING_FISH);
        }
        else
        {
            /* Give the cards */
            switchGameState(GS_ANSWERING_CARD);
            players[requirePlayer]->removeCards(reqs);
            givingCards = reqs;
        }
        break;
    }
    case GS_PLAYER_ASKING_CARD:
    {
        requireNumber = number;
        switchGameState(GS_ASKING);
        timer.startTimeout(1000 * WAIT_SECONDS);
        break;
    }
    }
}

void Game::onClickFish()
{
    switch(gameState)
    {
    case GS_ASKING:
    {
        timer.startTimeout(500);
        vector<Card*> reqs = players[requirePlayer]->getRequiredCards(requireNumber);
        if(reqs.size() == 0)
        {
            /* Fish! */
            switchGameState(GS_ANSWERING_FISH);
        }
        break;
    }
    }
}


void Game::start()
{
    Player *p1 = new Player(0);
    Player *p2 = new Player(1);
    Player *p3 = new Player(2);
    Player *p4 = new Player(3);
    p1->name = string("You");
    p2->name = string("Green");
    p3->name = string("Join");
    p4->name = string("Steven");
    this->players.push_back(p1);
    this->players.push_back(p2);
    this->players.push_back(p3);
    this->players.push_back(p4);

    /* 2 is you */
    currentPlayerIndex = 0;
    requireNumber = -1;
    requirePlayer = -1;

    gameState = GS_PLAYER_ASKING_PLAYER;
    waitPlayerMotion = true;

    int cardsTypes[13] = {
        3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };
    /* Init table cards  */
    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 13; j ++)
        {
            Card *card = new Card();
            card->number = cardsTypes[j];
            card->pattern = i;
            tableCards.push_back(card);
        }
    }

    /* Disburse cards */
    for(int j = 0; j < 4; j ++)
    {
        for(int i = 0; i < players.size(); i ++)
        {
            int index = rand() % tableCards.size();
            players[i]->cards.push_back(tableCards[index]);
            this->removeTableCard(index);
        }
    }

    for(int i = 0; i < players.size(); i ++)
    {
        for(int j = 0; j < players[i]->cards.size(); j ++)
        {
            cout << players[i]->cards[j]->number << "|" <<
                    players[i]->cards[j]->pattern << ",";
        }
        cout << endl;
    }
}

void Game::end()
{
}
