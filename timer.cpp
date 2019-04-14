#include "timer.h"
#include <iostream>
#include <QTimerEvent>
#include <QWidget>

using namespace std;
Timer::Timer(TimerListener *tl)
{
    mpCallback = tl;
}

void Timer::timerEvent(QTimerEvent *event)
{
    cout << "Timer ID:" << event->timerId();
    mpCallback->onTimerCallback(this->timerId);
    killTimer(timerId);
}
