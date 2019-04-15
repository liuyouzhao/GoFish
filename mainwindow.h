#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <hash_map>
#include <string>
#include <timer.h>
#include <QPainter>
#include "game.h"

using namespace std;
using namespace __gnu_cxx;
namespace Ui {
class MainWindow;
}

class WindowTimer : public QObject
{
public:
    WindowTimer(int interval, QWidget *widget);

protected:
    void timerEvent(QTimerEvent *event);

    QWidget *mpWidget;

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void paintEvent(QPaintEvent *event);

    int cardIndex(int num, int pattern);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);

    void drawPlayers(QPainter &painter, int n = 4);
    void drawPlayer(QPainter &painter, int i);
    void drawTableCards(QPainter &painter);
    void drawPlayersCards(QPainter &painter, int n = 4);
    void drawPlayerCards(QPainter &painter, int i);
    void drawPlayersMessage(QPainter &painter);
    void drawSelections(QPainter &painter);
    void drawFishButton(QPainter &painter);

    void drawHumanPlayerAndCards(QPainter &painter);

    void drawHumanMotion(QPainter &painter);
    void drawGivingCards(QPainter &painter);

    void drawScoreCards(QPainter &painter);
private:

    Game game;

    int loadImages();

    Ui::MainWindow *ui;

    vector<QImage>  mImagesVerticle;
    vector<QImage>  mimagesHorizontal;
    vector<QImage>  mImagesVerticleSmall;

    QImage          mPortrait;
    QImage          mPortraitSmall;
    QImage          mReverseSideVert;
    QImage          mReverseSideHori;
    int mCardImageWidth;
    int mCardImageHeight;
    int mPortraitWidth;
    int mPortraitHeight;

    int mScreenWidth;
    int mScreenHeight;

    WindowTimer timer;

    //QPainter painter;

    vector<QPoint> tableCardTrans;
    vector<int> tableCardRots;

    vector<QRect> playerInteracts;
    vector< vector<QRect> > cardInteracts;

    QRect fishRect;

    int mPlayerPositions[4][2];
    int mPortraitClips[4][4];
    int mBeginXY[4][2];
};

#endif // MAINWINDOW_H
