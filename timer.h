#ifndef TIMER_H
#define TIMER_H

#include <QObject>

class TimerListener
{
public:
    virtual void onTimerCallback() = 0;
};

class Timer : public QObject
{
public:
    Timer(TimerListener *tl);
    void startTimeout(int timeout)
    {
        timerId = startTimer(timeout);
    }
protected:
    void timerEvent(QTimerEvent *event);

    TimerListener *mpCallback;
    int timerId;
};

#endif // TIMER_H
