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
    mpCallback->onTimerCallback();
    killTimer(event->timerId());
}
