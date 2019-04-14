#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

#include "timer.h"

using namespace std;

class Player;

class Card
{
public:
    int x;
    int y;

    int pattern;
    int number;
};

class AI
{
public:
    int askNumber(vector<Card*> cards);
    int askPlayer(vector<Player*> players);
};

class Player
{
public:

    vector<Card*> getRequiredCards(int cardNumber)
    {
        vector<Card*> requires;
        for(int i = 0; i < cards.size(); i ++)
        {
            if(cardNumber == cards[i]->number)
            {
                requires.push_back(cards[i]);
            }
        }
        return requires;
    }

    void addCards(vector<Card*> newCards)
    {
        for(int i = 0; i < newCards.size(); i ++)
        {
            this->cards.push_back(newCards[i]);
        }
    }

    void addWinCards(vector<Card*> newCards)
    {
        for(int i = 0; i < newCards.size(); i ++)
        {
            this->winCards.push_back(newCards[i]);
        }
    }

    bool tryIfWinCards(vector<Card*> newCards)
    {
        vector<Card*> winCards;
        int num = newCards[0]->number;
        for(int i = 0; i < this->cards.size(); i ++)
        {
            if(this->cards[i]->number == num)
            {
                winCards.push_back(this->cards[i]);
            }
        }
        if(winCards.size() == 4)
        {
            addWinCards(winCards);
        }
        else
        {
            addCards(newCards);
        }
    }

    void removeCards(vector<Card*> cardNumbers)
    {
        vector<Card*> newCards;
        for(int i = 0; i < cards.size(); i ++)
        {
            if(cards[i]->number == cardNumbers[i]->number)
            {
                continue;
            }
            else
            {
                newCards.push_back(cards[i]);
            }
        }
        cards = newCards;
    }

    string name;
    vector<Card*> cards;
    vector<Card*> winCards;
    AI *ai;
};

class Game : public TimerListener
{
public:
    Game();

    virtual void onTimerCallback(int timerId);


    enum GameState {
        GS_ASKING,
        GS_ANSWERING_CARD,
        GS_ANSWERING_FISH,
        GS_FISHING,
        GS_PLAYER_ASKING_PLAYER,
        GS_PLAYER_ASKING_CARD,
        GS_COLLECTING,
        GS_OVER
    };

    vector<Player*> players;
    vector<Card*> givingCards;
    vector<Card*> tableCards;

    int currentPlayerIndex;
    int requireNumber;
    int requirePlayer;
    int humanPlayerIndex;
    int winnerPlayer;


    GameState gameState;

    void start();
    void end();


    void onClickPlayer(int playerId);
    void onClickCard(int number);

    void removeTableCard(int index)
    {
        vector<Card*> newCards;
        for(int i = 0; i < tableCards.size(); i ++)
        {
            if(index == i)
            {
                continue;
            }
            newCards.push_back(tableCards[i]);
        }
        tableCards = newCards;
    }

    Timer timer;
private:
    int currentTimerId;
};

#endif // GAME_H
