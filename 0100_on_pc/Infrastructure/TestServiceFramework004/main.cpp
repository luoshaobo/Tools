#include <QCoreApplication>
#include <memory>
#include <service-framework/threadeventqueue.h>
#include <service-framework/statemachinemaster.h>

#define UNUSEDVAR(var)         (&(var))

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

class MyTimeoutEvent : public GSMEvent
{
public:
    MyTimeoutEvent(unsigned int nGroupId, unsigned int nStateMachineId)
        : GSMEvent(nGroupId, nStateMachineId)
    {

    }

    virtual const char* getName() const
    {
        return "MyTimeoutEvent";
    }

    virtual Event* clone() const
    {
        return new MyTimeoutEvent(m_nGroupId, m_nStateMachineId);
    }
};

class MyStateMachine_01_01 : public IStatemachine
{
public:
    MyStateMachine_01_01(ITimerMaster<IStatemachine, Event> *pITimerMaster,
                         IStatemachineMaster *pIStatemachineMaster,
                         StatemachineId id, StatemachineType type)
        : IStatemachine(pITimerMaster, pIStatemachineMaster, id, type)
    {

    }

public:
    void fireEvent(Event &sendEvent)
    {
        IStatemachineMaster* pStatemachineMaster = getStatemachineMaster();
        if (pStatemachineMaster != NULL) {
            pStatemachineMaster->fireEvent(getStatemachineId(), sendEvent);
        }
    }

public:
    virtual const char* getStateName()
    {
        return "(unknown state)";
    }

    virtual const char* getStatemachineName()
    {
        return "MyStateMachine_01_01";
    }

    virtual void onEventHandler(const Event *pEvent)
    {
        UNUSEDVAR(pEvent);
        printf("### MyStateMachine_01_01::onEventHandler(): Event=%s\n", pEvent->getName());
        const MyTimeoutEvent *pMyTimeoutEvent = dynamic_cast<const MyTimeoutEvent *>(pEvent);
        if (pMyTimeoutEvent != NULL) {
            MyTimeoutEvent *pMyTimeoutEvent = (MyTimeoutEvent *)pEvent;
            printf("### MyStateMachine_01_01::onEventHandler(MyTimeoutEvent): GroupId=%d, StateMachineId=%d\n", pMyTimeoutEvent->GetGroupId(), pMyTimeoutEvent->GetStateMachineId());
        }
    }

    virtual void onStart()
    {
        printf("### MyStateMachine_01_01::onStart()\n");

        MyTimeoutEvent myTimeoutEvent(1, 1);
        startTimer(10 * 1000 * 1000, myTimeoutEvent);
    }

    virtual bool abortExecution()
    {
        printf("### MyStateMachine_01_01::onStart()\n");
        return true;
    }
};

class MyStateMachine_01_02 : public IStatemachine
{
public:
    MyStateMachine_01_02(ITimerMaster<IStatemachine, Event> *pITimerMaster,
                         IStatemachineMaster *pIStatemachineMaster,
                         StatemachineId id, StatemachineType type)
        : IStatemachine(pITimerMaster, pIStatemachineMaster, id, type)
    {

    }

public:
    void fireEvent(Event &sendEvent)
    {
        IStatemachineMaster* pStatemachineMaster = getStatemachineMaster();
        if (pStatemachineMaster != NULL) {
            pStatemachineMaster->fireEvent(getStatemachineId(), sendEvent);
        }
    }

public:
    virtual const char* getStateName()
    {
        return "(unknown state)";
    }

    virtual const char* getStatemachineName()
    {
        return "MyStateMachine_01_02";
    }

    virtual void onEventHandler(const Event *pEvent)
    {
        UNUSEDVAR(pEvent);
        printf("### MyStateMachine_01_02::onEventHandler(): Event=%s\n", pEvent->getName());

        const MyTimeoutEvent *pMyTimeoutEvent = dynamic_cast<const MyTimeoutEvent *>(pEvent);
        if (pMyTimeoutEvent != NULL) {
            MyTimeoutEvent *pMyTimeoutEvent = (MyTimeoutEvent *)pEvent;
            printf("### MyStateMachine_01_02::onEventHandler(MyTimeoutEvent): GroupId=%d, StateMachineId=%d\n", pMyTimeoutEvent->GetGroupId(), pMyTimeoutEvent->GetStateMachineId());
        }
    }

    virtual void onStart()
    {
        printf("### MyStateMachine_01_02::onStart()\n");

        MyTimeoutEvent myTimeoutEvent(1, 2);
        startTimer(15 * 1000 * 1000, myTimeoutEvent);
    }

    virtual bool abortExecution()
    {
        printf("### MyStateMachine_01_02::onStart()\n");
        return true;
    }
};

class MyStatemachineMaster_01 : public StatemachineMaster
{
public:
    MyStatemachineMaster_01(ThreadEventQueue *pThreadEventQueue)
        : StatemachineMaster(pThreadEventQueue)
        , m_pThreadEventQueue(pThreadEventQueue)
        , m_pMyStateMachine_01_01(NULL)
        , m_pMyStateMachine_01_02(NULL)
    {

    }

public:
    virtual void fireEvent(IStatemachine::StatemachineId id, Event &sendEvent)
    {
        UNUSEDVAR(id);
        if (m_pThreadEventQueue != NULL) {
            m_pThreadEventQueue->insertEvent(&sendEvent);
        }
    }

public:
    void OnEvent1(const MyEvent1 *pEvent)
    {
        printf("### MyStatemachineMaster_01::OnEvent1(): GroupId=%d, StateMachineId=%d\n", pEvent->GetGroupId(), pEvent->GetStateMachineId());

        if (m_pMyStateMachine_01_01 == NULL) {
            m_pMyStateMachine_01_01 = new MyStateMachine_01_01(this, this, 1, 1);
            startStatemachine(m_pMyStateMachine_01_01, 1000);
        }

        if (m_pMyStateMachine_01_02 == NULL) {
            m_pMyStateMachine_01_02 = new MyStateMachine_01_02(this, this, 1, 2);
            startStatemachine(m_pMyStateMachine_01_02, 2000);
        }
    }

    void OnEvent2(const MyEvent2 *pEvent)
    {
        printf("### MyStatemachineMaster_01::OnEvent2(): GroupId=%d, StateMachineId=%d\n", pEvent->GetGroupId(), pEvent->GetStateMachineId());
    }

private:
    ThreadEventQueue *m_pThreadEventQueue;
    MyStateMachine_01_01 *m_pMyStateMachine_01_01;
    MyStateMachine_01_02 *m_pMyStateMachine_01_02;
};

class MyThreadEventQueue : public ThreadEventQueue, public ITimerExpired
{
public:
    MyThreadEventQueue(std::string mq_name, MyStatemachineMaster_01 *pMyStatemachineMaster_01)
        : ThreadEventQueue(mq_name)
        , m_timer1(this, 5 * 1000 * 1000, false)
        , m_timer2(this, 8 * 1000 * 1000, false)
        , m_pMyStatemachineMaster_01(pMyStatemachineMaster_01)
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
            printf("### MyThreadEventQueue::handleTimeout() for m_timer1: count=%d\n", count++);
        } else if (timerId == m_timer2.getId()) {
            static int count = 0;
            printf("### MyThreadEventQueue::handleTimeout() for m_timer2: count=%d\n", count++);
        } else {

        }
    }

private:
    void OnEvent1(const MyEvent1 *pEvent)
    {
        printf("### MyThreadEventQueue::OnEvent1(): GroupId=%d, StateMachineId=%d\n", pEvent->GetGroupId(), pEvent->GetStateMachineId());
        m_pMyStatemachineMaster_01->OnEvent1(pEvent);
    }

    void OnEvent2(const MyEvent2 *pEvent)
    {
        printf("### MyThreadEventQueue::OnEvent2(): GroupId=%d, StateMachineId=%d\n", pEvent->GetGroupId(), pEvent->GetStateMachineId());
        m_pMyStatemachineMaster_01->OnEvent2(pEvent);
    }

private:
    Timer m_timer1;
    Timer m_timer2;
    MyStatemachineMaster_01 *m_pMyStatemachineMaster_01;
};

class MyEventQueueHost
{
public:
    MyEventQueueHost()
        : m_threadEventQueue("MyEventQueue", &m_myStatemachineMaster_01)
        , m_myStatemachineMaster_01(&m_threadEventQueue)
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
    MyStatemachineMaster_01 m_myStatemachineMaster_01;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyEventQueueHost myEventQueueHost;

    //myEventQueueHost.StartTimer1();
    //myEventQueueHost.StartTimer2();
    myEventQueueHost.PostEvent1();
    myEventQueueHost.PostEvent2();

    return a.exec();
}

