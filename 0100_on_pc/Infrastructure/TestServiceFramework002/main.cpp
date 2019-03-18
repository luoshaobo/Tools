#include <QCoreApplication>
#include <memory>
#include <service-framework/threadeventqueue.h>

class MyEvent1 : public Event
{
public:
    MyEvent1() : Event()
    {

    }

    virtual const char* getName() const
    {
        return "MyEvent1";
    }

    virtual Event* clone() const
    {
        return new MyEvent1();
    }
};

class MyTimerHandler1: public ITimerExpired
{
public:
    virtual void handleTimeout(int  timerId )
    {
        static int count = 0;
        printf("### MyTimerHandler1::handleTimeout(): count=%d\n", count++);
    }
};

class MyThreadEventQueue : public ThreadEventQueue
{
public:
    MyThreadEventQueue(std::string name): ThreadEventQueue(name), m_timerHandler1(), m_timer1(&m_timerHandler1, 5000 * 1000, false)
    {
        m_EventHandler.registerEventHandler(this, &MyThreadEventQueue::OnEvent1);
    }

public:
    void PostEvent1()
    {
        insertEvent(SharedPtr<MyEvent1>(new MyEvent1()));
    }

    void StartTimer1()
    {
        addTimer(m_timer1);
    }

private:
    virtual void eventHandler(Event::Ptr event)
    {
        m_EventHandler.handleEvent(event);
    }

private:
    void OnEvent1(const MyEvent1 *pEvent)
    {
        printf("### OnEvent1()\n");
    }

private:
    MyTimerHandler1 m_timerHandler1;
    Timer m_timer1;
};

class MyEventQueueHost
{
public:
    MyEventQueueHost() : m_threadEventQueue("MyEventQueue")
    {

    }

    void PostEvent1()
    {
        m_threadEventQueue.PostEvent1();
    }

    void StartTimer1()
    {
        m_threadEventQueue.StartTimer1();
    }

private:
    MyThreadEventQueue m_threadEventQueue;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyEventQueueHost myEventQueueHost;

    myEventQueueHost.StartTimer1();
    myEventQueueHost.PostEvent1();

    return a.exec();
}

