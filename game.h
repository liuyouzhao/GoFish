#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <iostream>

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
    AI(int player)
    {
        playerId = player;
    }

    virtual int askNumber(vector<Card*> cards);
    virtual int askPlayer(vector<Player*> players);

    int lastAsked;
    int playerId;
};

class Player
{
public:
    Player(int id)
    {
        playerId = id;
        ai = new AI(playerId);
    }

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

    void addCard(Card *newCard)
    {
        this->cards.push_back(newCard);
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
        vector<Card*> tmpCards;
        vector<Card*> winCards;
        int num = newCards[0]->number;

        for(int i = 0; i < newCards.size(); i ++)
        {
            winCards.push_back(newCards[i]);
        }
        for(int i = 0; i < this->cards.size(); i ++)
        {
            if(this->cards[i]->number == num)
            {
                winCards.push_back(this->cards[i]);
            }
            else
            {
                tmpCards.push_back(this->cards[i]);
            }
        }
        if(winCards.size() == 4)
        {
            cards = tmpCards;
            addWinCards(winCards);
            return true;
        }
        else
        {
            addCards(newCards);
            return false;
        }
    }

    bool tryIfWinCards(Card *newCard)
    {
        vector<Card*> tmpCards;
        vector<Card*> winCards;
        int num = newCard->number;
        winCards.push_back(newCard);
        for(int i = 0; i < this->cards.size(); i ++)
        {
            if(this->cards[i]->number == num)
            {
                winCards.push_back(this->cards[i]);
            }
            else
            {
                tmpCards.push_back(this->cards[i]);
            }
        }
        if(winCards.size() == 4)
        {
            cards = tmpCards;
            addWinCards(winCards);
            return true;
        }
        else
        {
            addCard(newCard);
            return false;
        }
    }

    void removeCards(vector<Card*> cardNumbers)
    {
        vector<Card*> newCards;
        for(int i = 0; i < cards.size(); i ++)
        {
            if(cards[i]->number == cardNumbers[0]->number)
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
    int playerId;
};

class Game : public TimerListener
{
public:
    Game();

    virtual void onTimerCallback();
    void start();
    void end();
    void nextPlayer();
    void againPlayer();


    void onClickPlayer(int playerId);
    void onClickCard(int number);
    void onClickFish();

    Card *removeTableCard(int index)
    {
        vector<Card*> newCards;
        Card *pop;
        for(int i = 0; i < tableCards.size(); i ++)
        {
            if(index == i)
            {
                pop = tableCards[i];
                continue;
            }
            newCards.push_back(tableCards[i]);
        }
        tableCards = newCards;
        return pop;
    }

    string getRequireString()
    {
        if(requireNumber == -1)
        {
            return string("");
        }
        string req = cardTexts[requireNumber - 3];
        return req;
    }


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

    inline void switchGameState(GameState gs)
    {
        gameState = gs;
        cout << "GS: " << gameStateString[gameState] << endl;
    }


    const char *gameStateString[8] = {
        "GS_ASKING",
        "GS_ANSWERING_CARD",
        "GS_ANSWERING_FISH",
        "GS_FISHING",
        "GS_PLAYER_ASKING_PLAYER",
        "GS_PLAYER_ASKING_CARD",
        "GS_COLLECTING",
        "GS_OVER"
    };

    vector<Player*> players;
    vector<Card*> givingCards;
    vector<Card*> tableCards;

    int currentPlayerIndex;
    int requireNumber;
    int requirePlayer;
    int humanPlayerIndex;
    int winnerPlayer;
    bool waitPlayerMotion;

    GameState gameState;
    Timer timer;

    /** Texts
      */
    string cardTexts[13];

private:
    int currentTimerId;
};

#endif // GAME_H
