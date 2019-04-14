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

private:

    Game game;

    int loadImages();

    Ui::MainWindow *ui;

    vector<QImage> mImages;

    WindowTimer timer;

    //QPainter painter;

    vector<QPoint> tableCardTrans;
    vector<int> tableCardRots;

    vector<QRect> playerInteracts;
    vector< vector<QRect> > cardInteracts;
};

#endif // MAINWINDOW_H
