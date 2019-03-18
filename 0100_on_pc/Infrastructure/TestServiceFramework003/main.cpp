#include <QCoreApplication>
#include <memory>
#include <service-framework/threadeventqueue.h>

class GSMEvent : public Event
{
public:
    unsigned int GetGroupId() const { return m_nGroupId; }
    unsigned int GetStateMachineId() const { return m_nStateMachineId; }

protected:
    GSMEvent(unsigned int nGroupId, unsigned int nStateMachineId)
        : m_nGroupId(nGroupId), m_nStateMachineId(nStateMachineId)
    {

    }

    unsigned int m_nGroupId;
    unsigned int m_nStateMachineId;
};

class MyEvent1 : public GSMEvent
{
public:
    MyEvent1(unsigned int nGroupId, unsigned int nStateMachineId)
        : GSMEvent(nGroupId, nStateMachineId)
    {

    }

    virtual const char* getName() const
    {
        return "MyEvent1";
    }

    virtual Event* clone() const
    {
        return new MyEvent1(m_nGroupId, m_nStateMachineId);
    }
};

class MyEvent2 : public GSMEvent
{
public:
    MyEvent2(unsigned int nGroupId, unsigned int nStateMachineId)
        : GSMEvent(nGroupId, nStateMachineId)
    {

    }

    virtual const char* getName() const
    {
        return "MyEvent2";
    }

    virtual Event* clone() const
    {
        return new MyEvent2(m_nGroupId, m_nStateMachineId);
    }
};


class MyThreadEventQueue : public ThreadEventQueue, public ITimerExpired
{
public:
    MyThreadEventQueue(std::string name)
        : ThreadEventQueue(name)
        , m_timer1(this, 5000 * 1000, true)
        , m_timer2(this, 8000 * 1000, false)
    {
        m_EventHandler.registerEventHandler(this, &MyThreadEventQueue::OnEvent1);
        m_EventHandler.registerEventHandler(this, &MyThreadEventQueue::OnEvent2);
    }

public:
    void PostEvent1()
    {
        insertEvent(SharedPtr<MyEvent1>(new MyEvent1(0, 0)));
    }

    void PostEvent2()
    {
        insertEvent(SharedPtr<MyEvent2>(new MyEvent2(1, 1)));
    }

    void StartTimer1()
    {
        addTimer(m_timer1);
    }

    void StartTimer2()
    {
        addTimer(m_timer2);
    }

private:
    virtual void eventHandler(Event::Ptr event)
    {
        m_EventHandler.handleEvent(event);
    }

    virtual void handleTimeout(int  timerId )
    {
        if (timerId == m_timer1.getId()) {
            static int count = 0;
            printf("### handleTimeout() for m_timer1: count=%d\n", count++);
        } else if (timerId == m_timer2.getId()) {
            static int count = 0;
            printf("### handleTimeout() for m_timer2: count=%d\n", count++);
        } else {

        }
    }

private:
    void OnEvent1(const MyEvent1 *pEvent)
    {
        printf("### OnEvent1(): GroupId=%d, StateMachineId=%d\n", pEvent->GetGroupId(), pEvent->GetStateMachineId());
    }

    void OnEvent2(const MyEvent2 *pEvent)
    {
        printf("### OnEvent2(): GroupId=%d, StateMachineId=%d\n", pEvent->GetGroupId(), pEvent->GetStateMachineId());
    }

private:
    Timer m_timer1;
    Timer m_timer2;
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

    void PostEvent2()
    {
        m_threadEventQueue.PostEvent2();
    }

    void StartTimer1()
    {
        m_threadEventQueue.StartTimer1();
    }

    void StartTimer2()
    {
        m_threadEventQueue.StartTimer2();
    }

private:
    MyThreadEventQueue m_threadEventQueue;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyEventQueueHost myEventQueueHost;

    myEventQueueHost.StartTimer1();
    myEventQueueHost.StartTimer2();
    myEventQueueHost.PostEvent1();
    myEventQueueHost.PostEvent2();

    return a.exec();
}

