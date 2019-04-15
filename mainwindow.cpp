#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QImage>
#include <stdio.h>
#include <stdlib.h>
#include <QMouseEvent>

#define DEBUG 0

using namespace std;
WindowTimer::WindowTimer(int interval, QWidget *widget)
{
    mpWidget = widget;
    startTimer(interval);
}

void WindowTimer::timerEvent(QTimerEvent *event)
{
    mpWidget->update();
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(50, dynamic_cast<QWidget*>(this))
{
    ui->setupUi(dynamic_cast<QMainWindow*>(this));
    loadImages();

    /* 2 is you */
    game.currentPlayerIndex = 2;
    game.start();

    mScreenWidth = this->size().width();
    mScreenHeight = this->size().height();

    for(int i = 0; i < game.tableCards.size(); i ++)
    {
        tableCardTrans.push_back(QPoint(mScreenWidth / 2 + (rand() % 50), mScreenHeight / 2 + (rand() % 50)));
        tableCardRots.push_back(rand() % 360);
    }

    for(int i = 0; i < 4; i ++)
    {
        vector<QRect> rects;
        cardInteracts.push_back(rects);
    }

    int playerPositions[4][2] = {
        {mScreenWidth / 2, mScreenHeight - 96},
        {32, mScreenHeight / 2 - 24},
        {mScreenWidth / 2, 24},
        {mScreenWidth - 96, mScreenHeight / 2}
    };

    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 2; j ++)
        {
            mPlayerPositions[i][j] = playerPositions[i][j];
        }
        playerInteracts.push_back(QRect(mPlayerPositions[i][0], mPlayerPositions[i][1], mPortrait.width() / 2, mPortrait.height() / 2));
    }

    int portraitClips[4][4] = {
        {0, 0, mPortrait.width() / 2, mPortrait.height() / 2},
        {mPortrait.width() / 2, 0, mPortrait.width() / 2, mPortrait.height() / 2},
        {0, mPortrait.height() / 2, mPortrait.width() / 2, mPortrait.height() / 2},
        {mPortrait.width() / 2, mPortrait.height() / 2, mPortrait.width() / 2, mPortrait.height() / 2},
    };
    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 4; j ++)
        {
            mPortraitClips[i][j] = portraitClips[i][j];
        }
    }

    fishRect = QRect(playerPositions[0][0] + portraitClips[0][2] * 3, playerPositions[0][1], 60, 60);

    int beginXY[4][2] = {
        {mPlayerPositions[0][0] - mCardImageWidth * 2, mPlayerPositions[0][1] - mCardImageHeight - 8},
        {mPlayerPositions[1][0] + mCardImageHeight + 8, mPlayerPositions[1][1] - mCardImageHeight},
        {mPlayerPositions[2][0] - mCardImageWidth * 2, mPlayerPositions[2][1] + mCardImageHeight + 8},
        {mPlayerPositions[3][0] - mCardImageHeight + 8, mPlayerPositions[3][1] - mCardImageHeight}
    };

    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 2; j ++)
        {
            mBeginXY[i][j] = beginXY[i][j];
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::cardIndex
 * @param num
 * @param pattern
 * @return
 *
 * 1, 2 Kings
 * 3-15
 * 16-28
 * ...
 * 0: black peach (black)
 * 1: red peach (red)
 * 2: blosom (black)
 * 3: diamond (red)
 */
int MainWindow::cardIndex(int num, int pattern)
{
    if(num <= 2)
        return num - 1;

    return num + pattern * 13 - 1;
}

int MainWindow::loadImages()
{
    for(int i = 1; i <= 54; i ++)
    {
        char filename[32] = {0};
        QImage img;
        sprintf(filename, "../cards/%d.jpg", i);
        bool ok = img.load(filename);
        img = img.scaled(img.width() * 0.7, img.height() * 0.7);
        if(ok)
        {
            mImagesVerticle.push_back(img);
            QMatrix mat;
            mat.reset();
            QImage imghori = img.transformed(mat.rotate(-90));
            mimagesHorizontal.push_back(imghori);

            QImage smallImg = img.scaled(img.width() * 0.4, img.height() * 0.4);
            mImagesVerticleSmall.push_back(smallImg);
        }
    }

    mCardImageWidth = mImagesVerticle[0].width();
    mCardImageHeight = mImagesVerticle[0].height();

    mPortrait.load(QString("../cards/portrait.png"));
    mPortrait = mPortrait.scaled(mPortrait.width() / 8, mPortrait.height() / 8);
    mPortraitWidth = mPortrait.width();
    mPortraitHeight = mPortrait.height();

    mPortraitSmall = mPortrait.scaled(mPortrait.width() / 2, mPortrait.height() / 2);

    mReverseSideVert.load(QString("../cards/b.jpg"));
    mReverseSideVert = mReverseSideVert.scaled(0.7 * mReverseSideVert.width(), 0.7 * mReverseSideVert.height());
    QMatrix mat;
    mReverseSideHori = mReverseSideVert.transformed(mat.rotate(-90));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();

    for(int i = 0; i < this->playerInteracts.size(); i ++)
    {
        if(playerInteracts[i].x() > x || playerInteracts[i].y() > y ||
           playerInteracts[i].x() + playerInteracts[i].width() < x ||
           playerInteracts[i].y() + playerInteracts[i].height() < y)
        {
            continue;
        }
        game.onClickPlayer(i);
        return;
    }

    for (int i = 0; i < this->cardInteracts.size(); i ++)
    {
        for(int j = this->cardInteracts[i].size() - 1; j >= 0; j --)
        {
            int _x = cardInteracts[i][j].x();
            int _y = cardInteracts[i][j].y();
            int _w = cardInteracts[i][j].width();
            int _h = cardInteracts[i][j].height();

            if(_x > x || _y > y ||
               _x + _w < x ||
               _y + _h < y)
            {
                continue;
            }
            game.onClickCard(game.players[i]->cards[j]->number);
            return;
        }
    }

    if(fishRect.contains(QRect(x, y, 1, 1)))
    {
        game.onClickFish();
    }
}

void MainWindow::drawPlayers(QPainter &painter, int n)
{

    painter.save();
    /* Player rect */
    /* Player name */

    for(int i = 0; i < n; i ++)
    {
        /* Portraits */
        painter.drawImage(  QRect(  mPlayerPositions[i][0],
                                    mPlayerPositions[i][1],
                                    mPortrait.width() / 2,
                                    mPortrait.height() / 2),
                            mPortrait,
                            QRect(  mPortraitClips[i][0],
                                    mPortraitClips[i][1],
                                    mPortraitClips[i][2],
                                    mPortraitClips[i][3]));

//        painter.drawText(mPlayerPositions[i][0], mPlayerPositions[i][1],
//                QString(game.players[i]->name.c_str()));

    }

#if DEBUG
    painter.setBrush(QBrush(QColor(255, 255, 0, 128)));
    painter.drawRect(playerInteracts[game.currentPlayerIndex]);

    painter.setPen(QColor(255, 0, 0, 255));
    painter.drawLine(QLine(mPlayerPositions[game.currentPlayerIndex][0],
                           mPlayerPositions[game.currentPlayerIndex][1],
                           mPlayerPositions[game.requirePlayer][0],
                           mPlayerPositions[game.requirePlayer][1]));
#endif
    painter.restore();
}

void MainWindow::drawPlayer(QPainter &painter, int i)
{

    painter.save();
    /* Player rect */
    /* Player name */


    /* Portraits */
    painter.drawImage(  QRect(  mPlayerPositions[i][0],
                                mPlayerPositions[i][1],
                                mPortrait.width() / 2,
                                mPortrait.height() / 2),
                        mPortrait,
                        QRect(  mPortraitClips[i][0],
                                mPortraitClips[i][1],
                                mPortraitClips[i][2],
                                mPortraitClips[i][3]));

    painter.drawText(mPlayerPositions[i][0], mPlayerPositions[i][1],
            QString(game.players[i]->name.c_str()));



#if DEBUG
    painter.setBrush(QBrush(QColor(255, 255, 0, 128)));
    painter.drawRect(playerInteracts[game.currentPlayerIndex]);

    painter.setPen(QColor(255, 0, 0, 255));
    painter.drawLine(QLine(mPlayerPositions[game.currentPlayerIndex][0],
                           mPlayerPositions[game.currentPlayerIndex][1],
                           mPlayerPositions[game.requirePlayer][0],
                           mPlayerPositions[game.requirePlayer][1]));
#endif
    painter.restore();
}

void MainWindow::drawTableCards(QPainter &painter)
{
    painter.save();
    for(int i = 0; i < game.tableCards.size(); i ++)
    {
        painter.resetTransform();
        painter.translate(tableCardTrans[i].x(), tableCardTrans[i].y());
        painter.rotate(tableCardRots[i]);
        painter.scale(0.9, 0.9);
        //painter.drawImage(QPointF(0, 0), mImagesVerticle[cardIndex(game.tableCards[i]->number, game.tableCards[i]->pattern)]);
        painter.drawImage(QPointF(0, 0), mReverseSideVert);
        painter.resetTransform();
    }
    painter.restore();
}

void MainWindow::drawPlayersCards(QPainter &painter, int n)
{

    int rotates[4] = {
        0, -90, 0, -90
    };

    int rects[4][2] = {
        {mCardImageWidth, mCardImageHeight},
        {mCardImageHeight, mCardImageWidth},
        {mCardImageWidth, mCardImageHeight},
        {mCardImageHeight, mCardImageWidth}
    };
    int offsets[4][2] = {
        {20, 0},
        {0, 20},
        {20, 0},
        {0, 20}
    };
    painter.save();
    for(int i = 0; i < n; i ++)
    {
        cardInteracts[i].clear();
        for(int j = 0; j < game.players[i]->cards.size(); j ++)
        {
            int x = mBeginXY[i][0] + offsets[i][0] * j;
            int y = mBeginXY[i][1] + offsets[i][1] * j;
            int offsetY = 0;
            if(rotates[i] != 0)
                offsetY = rects[i][1];

            /* Draw card */
            QImage cardImage;

            if(i == game.humanPlayerIndex)
            {
                cardImage = mImagesVerticle[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)];
            }
            else
            {
#if DEBUG
                if(rotates[i] == 0)
                    cardImage = mImagesVerticle[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)];
                else
                    cardImage = mimagesHorizontal[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)];
#else
                if(rotates[i] == 0)
                    cardImage = mReverseSideVert;
                else
                    cardImage = mReverseSideHori;
#endif
            }


            painter.drawImage(x, y, cardImage);

            QRect rect = QRect(x, y, rects[i][0], rects[i][1]);
            cardInteracts[i].push_back(rect);
#if DEBUG
            /* Draw rect */
            painter.setBrush(QBrush(QColor(255, 0, 0, 15)));
            painter.drawRect(rect);
#endif
        }
    }
    painter.restore();
}

void MainWindow::drawPlayerCards(QPainter &painter, int i)
{
    int rotates[4] = {
        0, -90, 0, -90
    };

    int rects[4][2] = {
        {mCardImageWidth, mCardImageHeight},
        {mCardImageHeight, mCardImageWidth},
        {mCardImageWidth, mCardImageHeight},
        {mCardImageHeight, mCardImageWidth}
    };
    int offsets[4][2] = {
        {20, 0},
        {0, 20},
        {20, 0},
        {0, 20}
    };
    painter.save();

    cardInteracts[i].clear();
    for(int j = 0; j < game.players[i]->cards.size(); j ++)
    {
        int x = mBeginXY[i][0] + offsets[i][0] * j;
        int y = mBeginXY[i][1] + offsets[i][1] * j;
        int offsetY = 0;
        if(rotates[i] != 0)
            offsetY = rects[i][1];

        /* Draw card */
        QImage cardImage;
        if(i == game.humanPlayerIndex)
        {
            cardImage = mImagesVerticle[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)];
        }
        else
        {
#if DEBUG
                if(rotates[i] == 0)
                    cardImage = mImagesVerticle[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)];
                else
                    cardImage = mimagesHorizontal[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)];
#else
                if(rotates[i] == 0)
                    cardImage = mReverseSideVert;
                else
                    cardImage = mReverseSideHori;
#endif
        }
        painter.drawImage(x, y, cardImage);

        QRect rect = QRect(x, y, rects[i][0], rects[i][1]);
        cardInteracts[i].push_back(rect);
#if DEBUG
        /* Draw rect */
        painter.setBrush(QBrush(QColor(255, 0, 0, 55)));
        painter.drawRect(rect);
#endif
    }

    painter.restore();
}


void MainWindow::drawPlayersMessage(QPainter &painter)
{
    painter.save();
    switch(game.gameState)
    {
    case Game::GS_ANSWERING_CARD:
    {
        painter.setBrush(QBrush(QColor(255, 255, 255)));
        painter.drawRect(   QRect(mPlayerPositions[game.requirePlayer][0],
                                  mPlayerPositions[game.requirePlayer][1],
                                  220, 40));
        painter.setFont(QFont(QString("bold"), 20, 4));
        painter.setPen(QColor(0, 0, 0));
        painter.drawText(QPoint(mPlayerPositions[game.requirePlayer][0] + 8,
                                mPlayerPositions[game.requirePlayer][1] + 24),
                                QString("OK.."));
    }
    case Game::GS_ANSWERING_FISH:
    {
        if(game.gameState == Game::GS_ANSWERING_FISH)
        {
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawRect(   QRect(mPlayerPositions[game.requirePlayer][0],
                                      mPlayerPositions[game.requirePlayer][1],
                                      220, 40));
            painter.setFont(QFont(QString("bold"), 20, 4));
            painter.setPen(QColor(0, 0, 0));
            painter.drawText(QPoint(mPlayerPositions[game.requirePlayer][0] + 8,
                                    mPlayerPositions[game.requirePlayer][1] + 24),
                                    QString("FISH!!!"));
        }
    }
    case Game::GS_ASKING:
    {
        painter.setBrush(QBrush(QColor(255, 255, 255)));
        painter.drawRect(   QRect(mPlayerPositions[game.currentPlayerIndex][0],
                                  mPlayerPositions[game.currentPlayerIndex][1],
                                  220, 80));
        painter.setFont(QFont(QString("bold"), 20, 4));
        painter.setPen(QColor(0, 0, 0));
        painter.drawText(QPoint(mPlayerPositions[game.currentPlayerIndex][0] + 8,
                                mPlayerPositions[game.currentPlayerIndex][1] + 24),
                                QString(game.getRequireString().c_str()) + QString("?"));

        painter.drawImage(  QRect(  mPlayerPositions[game.currentPlayerIndex][0] + 60,
                                    mPlayerPositions[game.currentPlayerIndex][1] + 10,
                                    mPortraitSmall.width() / 2,
                                    mPortraitSmall.height() / 2),
                            mPortraitSmall,
                            QRect(  mPortraitClips[game.requirePlayer][0]/2,
                                    mPortraitClips[game.requirePlayer][1]/2,
                                    mPortraitClips[game.requirePlayer][2]/2,
                                    mPortraitClips[game.requirePlayer][3]/2));
        break;
    }
    case Game::GS_PLAYER_ASKING_CARD:
    {
        if(game.requireNumber == -1)
        {
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawRect(   QRect(mPlayerPositions[game.currentPlayerIndex][0],
                                      mPlayerPositions[game.currentPlayerIndex][1],
                                      220, 40));
            painter.setFont(QFont(QString("bold"), 20, 4));
            painter.setPen(QColor(0, 0, 0));
            painter.drawText(QPoint(mPlayerPositions[game.currentPlayerIndex][0] + 8,
                                    mPlayerPositions[game.currentPlayerIndex][1] + 24),
                                    QString("Which CARD?"));
        }
        else
        {
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawRect(   QRect(mPlayerPositions[game.currentPlayerIndex][0],
                                      mPlayerPositions[game.currentPlayerIndex][1],
                                      220, 40));
            painter.setFont(QFont(QString("bold"), 20, 4));
            painter.setPen(QColor(0, 0, 0));
            painter.drawText(QPoint(mPlayerPositions[game.currentPlayerIndex][0] + 8,
                                    mPlayerPositions[game.currentPlayerIndex][1] + 24),
                                    QString(game.getRequireString().c_str()) + QString("?"));
        }
        break;
    }
    case Game::GS_PLAYER_ASKING_PLAYER:
    {
        painter.setBrush(QBrush(QColor(255, 255, 255)));
        painter.drawRect(   QRect(mPlayerPositions[game.currentPlayerIndex][0],
                                  mPlayerPositions[game.currentPlayerIndex][1],
                                  220, 40));
        painter.setFont(QFont(QString("bold"), 20, 4));
        painter.setPen(QColor(0, 0, 0));
        painter.drawText(QPoint(mPlayerPositions[game.currentPlayerIndex][0] + 8,
                                mPlayerPositions[game.currentPlayerIndex][1] + 24),
                                QString("Which PLAYER?"));
        break;
    }

    }
    painter.restore();
}

void MainWindow::drawFishButton(QPainter &painter)
{
    painter.save();
    painter.fillRect(fishRect, QBrush(QColor(0, 0, 255)));
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(QFont(QString("bold"), 20, 4));
    painter.drawText(fishRect.x(), fishRect.y() + 28, QString("FISH"));
    painter.restore();
}

void MainWindow::drawHumanPlayerAndCards(QPainter &painter)
{
    drawPlayers(painter, 1);
    drawPlayersCards(painter, 1);
}

void MainWindow::drawHumanMotion(QPainter &painter)
{
    if(game.waitPlayerMotion)
    {
        painter.fillRect(0, 0, mScreenWidth, mScreenHeight, QBrush(QColor(0, 0, 0, 168)));
        drawHumanPlayerAndCards(painter);
        if(game.requirePlayer != game.humanPlayerIndex && game.requirePlayer != -1)
        {
            drawPlayer(painter, game.requirePlayer);
        }
        if(game.currentPlayerIndex != game.humanPlayerIndex)
        {
            drawPlayer(painter, game.currentPlayerIndex);
            drawPlayerCards(painter, game.currentPlayerIndex);
        }
    }
}

void MainWindow::drawGivingCards(QPainter &painter)
{
    int rotates[4] = {
        0, -90, 0, -90
    };

    int rects[4][2] = {
        {mCardImageWidth, mCardImageHeight},
        {mCardImageHeight, mCardImageWidth},
        {mCardImageWidth, mCardImageHeight},
        {mCardImageHeight, mCardImageWidth}
    };
    int offsets[4][2] = {
        {20, 0},
        {0, 20},
        {20, 0},
        {0, 20}
    };
    painter.save();

    QImage cardImage;
    for(int i = 0; i < game.givingCards.size(); i ++)
    {
        if(rotates[game.requirePlayer] == 0)
            cardImage = mImagesVerticle[cardIndex(game.givingCards[i]->number, game.givingCards[i]->pattern)];
        else
            cardImage = mimagesHorizontal[cardIndex(game.givingCards[i]->number, game.givingCards[i]->pattern)];

        int x = mBeginXY[game.requirePlayer][0] + offsets[game.requirePlayer][0] * i;
        int y = mBeginXY[game.requirePlayer][1] + offsets[game.requirePlayer][1] * i;

        painter.drawImage(x, y, cardImage);
    }

    painter.restore();
}

void MainWindow::drawScoreCards(QPainter &painter)
{
    painter.save();
    for(int i = 0; i < game.players.size(); i ++)
    {
        painter.drawImage(  QRect(  0,
                                    i * mPortraitSmall.height()/2 + 64,
                                    mPortraitSmall.width() / 2,
                                    mPortraitSmall.height() / 2),
                            mPortraitSmall,
                            QRect(  mPortraitClips[i][0]/2,
                                    mPortraitClips[i][1]/2,
                                    mPortraitClips[i][2]/2,
                                    mPortraitClips[i][3]/2));
        for(int j = 0; j < game.players[i]->winCards.size(); j ++)
        {
            int number = game.players[i]->winCards[j]->number;
            int pattern = game.players[i]->winCards[j]->pattern;
            painter.drawImage(mPortraitSmall.width() / 2 + 16 * j, i * mPortraitSmall.height()/2 + 64,
                                mImagesVerticleSmall[this->cardIndex(number, pattern)]);
        }
    }
    painter.restore();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(0, 0, mScreenWidth, mScreenHeight, QBrush(QColor(0, 186, 0)));

    drawPlayers(painter);
    drawTableCards(painter);
    drawPlayersCards(painter);

    drawHumanMotion(painter);
    drawGivingCards(painter);
    drawPlayersMessage(painter);
    drawFishButton(painter);
    drawScoreCards(painter);
}
