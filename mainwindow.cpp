#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QImage>
#include <stdio.h>
#include <stdlib.h>
#include <QMouseEvent>

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

    int screenWidth = this->size().width();
    int screenHeight = this->size().height();

    for(int i = 0; i < game.tableCards.size(); i ++)
    {
        tableCardTrans.push_back(QPoint(screenWidth / 2 + (rand() % 50), screenHeight / 2 + (rand() % 50)));
        tableCardRots.push_back(rand() % 360);
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
        if(ok)
            mImages.push_back(img);
    }
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
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int screenWidth = this->size().width();
    int screenHeight = this->size().height();

    painter.fillRect(0, 0, screenWidth, screenHeight, QBrush(QColor(0, 255, 0)));

    int playerPositions[4][2] = {
        {screenWidth / 2, 64},
        {64, screenHeight / 2},
        {screenWidth / 2, screenHeight - 64},
        {screenWidth - 128, screenHeight / 2}
    };

    /* Player rect */
    /* Player name */
    cardInteracts.clear();
    playerInteracts.clear();
    for(int i = 0; i < game.players.size(); i ++)
    {
        if(game.currentPlayerIndex == i)
        {
            painter.setBrush(QBrush(QColor(255, 255, 255, 55)));
            painter.drawRect(playerPositions[i][0], playerPositions[i][1] - 8, 32, 16);
        }
        painter.drawText(playerPositions[i][0], playerPositions[i][1],
                QString(game.players[i]->name.c_str()));

        vector<QRect> temp;
        cardInteracts.push_back(temp);
        playerInteracts.push_back(QRect(playerPositions[i][0], playerPositions[i][1], 200, 100));
    }


    //static int x = 0, y = 0;
    //painter.rotate(15);

    int beginXY[4][2] = {
        {400, 600},
        {200, 300},
        {400, 200},
        {800, 200}
    };
    int rotates[4] = {
        0, -90, 0, -90
    };

    int rects[4][2] = {
        {105, 150},
        {150, 105},
        {105, 150},
        {150, 105}
    };

    int offsets1[4][2] = {
        {40, 0},
        {-40, 0},
        {40, 0},
        {-40, 0}
    };
    int offsets2[4][2] = {
        {40, 0},
        {0, 40},
        {40, 0},
        {0, 40}
    };

    for(int i = 0; i < game.tableCards.size(); i ++)
    {
        painter.resetTransform();
        painter.translate(tableCardTrans[i].x(), tableCardTrans[i].y());
        painter.rotate(tableCardRots[i]);
        painter.scale(0.9, 0.9);
        painter.drawImage(QPointF(0, 0), mImages[cardIndex(game.tableCards[i]->number, game.tableCards[i]->pattern)]);
        painter.resetTransform();
    }

    for(int i = 0; i < game.players.size(); i ++)
    {
        for(int j = 0; j < game.players[i]->cards.size(); j ++)
        {
            int x = beginXY[i][0];
            int y = beginXY[i][1];
            int offsetY = 0;
            if(rotates[i] != 0)
                offsetY = rects[i][1];

            /* Draw card */
            painter.resetTransform();
            painter.translate(x, y);
            painter.rotate(rotates[i]);
            painter.drawImage(QPointF(offsets1[i][0] * j, offsets1[i][1] * j), mImages[cardIndex(game.players[i]->cards[j]->number, game.players[i]->cards[j]->pattern)]);
            painter.resetTransform();

            QRect rect = QRect(x + offsets2[i][0] * j, y - offsetY + offsets2[i][1] * j, rects[i][0], rects[i][1]);
            cardInteracts[i].push_back(rect);
            /* Draw rect */
            painter.setBrush(QBrush(QColor(255, 0, 0, 55)));
            painter.drawRect(rect);

        }
    }
    //x ++;
}
