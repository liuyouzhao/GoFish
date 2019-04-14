#include "game.h"
#include <vector>
#include <iostream>

using namespace std;

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
        if(maxPlayerCards < players[i]->cards.size())
        {
            maxPlayerCards = players[i]->cards.size();
            maxIndex = i;
        }
    }
    return maxIndex;
}


Game::Game():
    currentTimerId(0),
    humanPlayerIndex(0),
    winnerPlayer(-1),
    timer(this)
{
}

/**
 * Timer driver
 * @brief Game::onTimerCallback
 * @param timerId
 */
void Game::onTimerCallback(int timerId)
{
    switch(gameState)
    {
    case GS_ANSWERING_CARD:
        gameState = GS_COLLECTING;
        timer.startTimeout(1000 * 3);
        break;
    case GS_ANSWERING_FISH:
        gameState = GS_FISHING;
        timer.startTimeout(1000 * 3);
        break;
    case GS_ASKING:
    {
        timer.startTimeout(1000 * 3);
        vector<Card*> reqs = players[requirePlayer]->getRequiredCards(requireNumber);
        if(reqs.size() == 0)
        {
            /* Fish! */
            gameState = GS_ANSWERING_FISH;
        }
        else
        {
            /* Give the cards */
            gameState = GS_ANSWERING_CARD;
            players[requirePlayer]->removeCards(reqs);
            givingCards = reqs;
        }
        break;
    }
    case GS_FISHING:
    {
        int randomCardIndex = rand() % this->tableCards.size();
        this->removeTableCard(randomCardIndex);

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
            gameState = GS_OVER;
            return;
        }

        currentPlayerIndex = (currentPlayerIndex + 1) / players.size();
        if(currentPlayerIndex == humanPlayerIndex)
            gameState = GS_PLAYER_ASKING_PLAYER;
        else
            gameState = GS_ASKING;
        break;
    }
    case GS_COLLECTING:
        gameState = GS_ASKING;

        players[currentPlayerIndex]->tryIfWinCards(givingCards);
        givingCards.clear();

        if(players[currentPlayerIndex]->cards.size() == 0)
        {
            winnerPlayer = currentPlayerIndex;
            gameState = GS_OVER;
            return;
        }

        requirePlayer = players[currentPlayerIndex]->ai->askPlayer(players);
        requireNumber = players[currentPlayerIndex]->ai->askNumber(players[currentPlayerIndex]->cards);

        if(requirePlayer != humanPlayerIndex)
        {
            timer.startTimeout(1000 * 3);
        }
        break;
    }
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
    {
        if(playerId == humanPlayerIndex)
            return;

        requirePlayer = playerId;
        gameState = GS_PLAYER_ASKING_CARD;
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
        timer.startTimeout(1000 * 3);
        vector<Card*> reqs = players[requirePlayer]->getRequiredCards(requireNumber);
        if(reqs.size() == 0)
        {
            /* Fish! */
            gameState = GS_ANSWERING_FISH;
        }
        else
        {
            /* Give the cards */
            gameState = GS_ANSWERING_CARD;
            players[requirePlayer]->removeCards(reqs);
            givingCards = reqs;
        }
        break;
    }
    case GS_PLAYER_ASKING_CARD:
    {
        requireNumber = number;
        gameState = GS_ASKING;
        timer.startTimeout(500);
        break;
    }
    }
}


void Game::start()
{
    Player *p1 = new Player();
    Player *p2 = new Player();
    Player *p3 = new Player();
    Player *p4 = new Player();
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
