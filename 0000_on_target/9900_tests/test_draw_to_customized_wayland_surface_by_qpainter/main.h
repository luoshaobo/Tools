#ifndef MAIN_H
#define MAIN_H

#include <QObject>
#include <QThread> 

class AnimationScreen : public QObject
{
    Q_OBJECT

public:
    AnimationScreen() : m_timerId(0) {}
    virtual ~AnimationScreen() {}

    void startShowing()
    {
        if (m_timerId != 0) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
        m_timerId = startTimer(66);
    }
    void stopShowing()
    {
        if (m_timerId != 0) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
    }

private:
    virtual void timerEvent(QTimerEvent *event);

private:
    int m_timerId;
};

class MyThread : public QThread
{
    Q_OBJECT

private:
    virtual void run();
};


#endif // MAIN_H
