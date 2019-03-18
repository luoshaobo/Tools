#include <QCoreApplication>
#include <memory>
#include <service-framework/threadeventqueue.h>
#include <service-framework/statemachinemaster.h>

#define UNUSEDVAR(var)         (&(var))

class BSEventDataBase
{
public:
    virtual ~BSEventDataBase() {}
};

class BSEvent : public Event
{
public:
    BSEvent(unsigned int nMasterIndex, unsigned int nStatemachineIndex, unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
        : m_nMasterIndex(nMasterIndex), m_nStatemachineIndex(nStatemachineIndex), m_nEventIndex(nEventIndex), m_data(data)
    {

    }

public:
    unsigned int GetMasterIndex () const { return m_nMasterIndex; }
    unsigned int GetStatemachineIndex () const { return m_nStatemachineIndex; }
    unsigned int GetEventIndex () const { return m_nEventIndex; }

public:
    virtual const char* getName() const
    {
        return "BSEvent";
    }

    virtual Event* clone() const
    {
        return new BSEvent(m_nMasterIndex, m_nStatemachineIndex, m_nEventIndex, m_data);
    }

private:
    unsigned int m_nMasterIndex;
    unsigned int m_nStatemachineIndex;
    unsigned int m_nEventIndex;
    SharedPtr<BSEventDataBase> m_data;
};

class BSServiceSM : public IStatemachine
{
public:
    enum {
        BSSSM_EI_EVENT1,
        BSSSM_EI_EVENT2,
        BSSSM_EI_TIMER1,
    };

public:
    BSServiceSM(unsigned int nMasterIndex, unsigned int nStatemachineIndex,
                ITimerMaster<IStatemachine, Event> *pITimerMaster,
                IStatemachineMaster *pIStatemachineMaster,
                StatemachineId id, StatemachineType type)
        : IStatemachine(pITimerMaster, pIStatemachineMaster, id, type)
        , m_nMasterIndex(nMasterIndex), m_nStatemachineIndex(nStatemachineIndex)
    {

    }

public:
    unsigned int GetMasterIndex () const { return m_nMasterIndex; }
    unsigned int GetStatemachineIndex () const { return m_nStatemachineIndex; }

    void PostEvent(unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
    {
        BSEvent *pBSEvent = new BSEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        fireEvent(*pBSEvent);
    }

    void StartTimer(TimeElapse::Difference timeoutMicroSeconds, unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
    {
        BSEvent bsEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        startTimer(timeoutMicroSeconds, bsEvent);
    }

private:
    void fireEvent(Event &sendEvent)
    {
        IStatemachineMaster* pStatemachineMaster = getStatemachineMaster();
        if (pStatemachineMaster != NULL) {
            pStatemachineMaster->fireEvent(getStatemachineId(), sendEvent);
        }
    }

public:
    void OnBSEvent(const BSEvent *pEvent)
    {
        printf("### BSServiceSM::OnBSEvent()[%d,%d,%d]\n", m_nMasterIndex, m_nStatemachineIndex, pEvent->GetEventIndex());

        switch (pEvent->GetEventIndex()) {
        case BSSSM_EI_EVENT1:
            {
                StartTimer(5 * 1000 * 1000, BSSSM_EI_TIMER1);
                //PostEvent(BSSSM_EI_EVENT2);
            }
            break;
        }
    }

public:
    virtual const char* getStateName()
    {
        return "(UnknownState)";
    }

    virtual const char* getStatemachineName()
    {
        return "BSServiceSM";
    }

    virtual void onEventHandler(const Event *pEvent)
    {
        printf("### BSServieSM::onEventHandler(): name=%s\n", pEvent->getName());

        const BSEvent *pBSEvent = dynamic_cast<const BSEvent *>(pEvent);
        if (pBSEvent != NULL) {
            OnBSEvent(pBSEvent);
        }
    }

    virtual void onStart()
    {

    }

    virtual bool abortExecution()
    {
        return true;
    }

private:
    unsigned int m_nMasterIndex;
    unsigned int m_nStatemachineIndex;
};

class BSJobSM : public IStatemachine
{
public:
    enum {
        BSJSM_EI_EVENT1,
        BSJSM_EI_EVENT2,
        BSJSM_EI_TIMER1,
        BSJSM_EI_TIMER2,
    };

public:
    BSJobSM(unsigned int nMasterIndex, unsigned int nStatemachineIndex,
                ITimerMaster<IStatemachine, Event> *pITimerMaster,
                IStatemachineMaster *pIStatemachineMaster,
                StatemachineId id, StatemachineType type)
        : IStatemachine(pITimerMaster, pIStatemachineMaster, id, type)
        , m_nMasterIndex(nMasterIndex), m_nStatemachineIndex(nStatemachineIndex)
    {

    }

public:
    unsigned int GetMasterIndex () const { return m_nMasterIndex; }
    unsigned int GetStatemachineIndex () const { return m_nStatemachineIndex; }

    void PostEvent(unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
    {
        BSEvent *pBSEvent = new BSEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        fireEvent(*pBSEvent);
    }

    void StartTimer(TimeElapse::Difference timeoutMicroSeconds, unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
    {
        BSEvent bsEvent(m_nMasterIndex, m_nStatemachineIndex, nEventIndex, data);
        startTimer(timeoutMicroSeconds, bsEvent);
    }

private:
    void fireEvent(Event &sendEvent)
    {
        IStatemachineMaster* pStatemachineMaster = getStatemachineMaster();
        if (pStatemachineMaster != NULL) {
            pStatemachineMaster->fireEvent(getStatemachineId(), sendEvent);
        }
    }

public:
    void OnBSEvent(const BSEvent *pEvent)
    {
        printf("### BSJobSM::OnBSEvent()[%d,%d,%d]\n", m_nMasterIndex, m_nStatemachineIndex, pEvent->GetEventIndex());
    }

public:
    virtual const char* getStateName()
    {
        return "(UnknownState)";
    }

    virtual const char* getStatemachineName()
    {
        return "BSJobSM";
    }

    virtual void onEventHandler(const Event *pEvent)
    {
        printf("### BSJobSM::onEventHandler(): name=%s\n", pEvent->getName());

        const BSEvent *pBSEvent = dynamic_cast<const BSEvent *>(pEvent);
        if (pBSEvent != NULL) {
            OnBSEvent(pBSEvent);
        }
    }

    virtual void onStart()
    {

    }

    virtual bool abortExecution()
    {
        return true;
    }

private:
    unsigned int m_nMasterIndex;
    unsigned int m_nStatemachineIndex;
};

class BSStatemachineMaster : public StatemachineMaster
{
public:
    static const unsigned int SERVICE_STATEMACHINE_INDEX = 10000;

public:
    BSStatemachineMaster(ThreadEventQueue *pThreadEventQueue, unsigned int nJobs)
        : StatemachineMaster(pThreadEventQueue)
        , m_nMasterIndex(MakeNewMasterIndex())
        , m_pThreadEventQueue(pThreadEventQueue)
        , m_pBSServiceSM(NULL)
        , m_BSJobSMs()
    {
        unsigned int nIndex;

        m_pBSServiceSM = new BSServiceSM(m_nMasterIndex, SERVICE_STATEMACHINE_INDEX, this, this, getNewStatemachineId(), 0);
        startStatemachine(m_pBSServiceSM, 0);

        for (nIndex = 0; nIndex < nJobs; nIndex++) {
            BSJobSM *pBSJobSM = new BSJobSM(m_nMasterIndex, nIndex, this, this, getNewStatemachineId(), 0);
            m_BSJobSMs.push_back(pBSJobSM);
            startStatemachine(pBSJobSM, 0);
        }
    }

    ~BSStatemachineMaster()
    {
        // TODO: to destrory all of the state machines.
    }

public:
    unsigned int GetMasterIndex () const { return m_nMasterIndex; }

    void PostJobEvent(unsigned int nJobIndex, unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
    {
        BSEvent *pBSEvent = new BSEvent(m_nMasterIndex, nJobIndex, nEventIndex, data);
        fireEvent(0, *pBSEvent);
    }

    void PostServiceEvent(unsigned int nEventIndex, SharedPtr<BSEventDataBase> data = 0)
    {
        BSEvent *pBSEvent = new BSEvent(m_nMasterIndex, SERVICE_STATEMACHINE_INDEX, nEventIndex, data);
        fireEvent(0, *pBSEvent);
    }

private:
    virtual void fireEvent(IStatemachine::StatemachineId id, Event &sendEvent)
    {
        UNUSEDVAR(id);
        if (m_pThreadEventQueue != NULL) {
            m_pThreadEventQueue->insertEvent(&sendEvent);
        }
    }

public:
    void OnBSEvent(const BSEvent *pEvent)
    {
        unsigned int nStatemachineIndex = pEvent->GetStatemachineIndex();
        if (nStatemachineIndex == SERVICE_STATEMACHINE_INDEX) {
            if (m_pBSServiceSM != NULL) {
                m_pBSServiceSM->OnBSEvent(pEvent);
            }
        } else if (nStatemachineIndex < m_BSJobSMs.size()) {
            BSJobSM *pBSJobSM = m_BSJobSMs[nStatemachineIndex];
            if (pBSJobSM != NULL) {
                pBSJobSM->OnBSEvent(pEvent);
            }
        } else {

        }
    }

private:
    static unsigned int m_nCurrentMasterIndex;

private:
    static unsigned int MakeNewMasterIndex()
    {
        return m_nCurrentMasterIndex++;
    }

private:
    unsigned int m_nMasterIndex;
    ThreadEventQueue *m_pThreadEventQueue;
    BSServiceSM *m_pBSServiceSM;
    std::vector<BSJobSM *> m_BSJobSMs;
};

unsigned int BSStatemachineMaster::m_nCurrentMasterIndex = 0;

class BSThreadEventQueue : public ThreadEventQueue
{
public:
    BSThreadEventQueue()
        : ThreadEventQueue("BSThreadEventQueue")
    {
        m_EventHandler.registerEventHandler(this, &BSThreadEventQueue::OnBSEvent);
    }

public:
    void AddStatemachineMaster(unsigned int nMasterIndex, BSStatemachineMaster *pBSStatemachineMaster)
    {
        std::map<unsigned int, BSStatemachineMaster*>::iterator it;
        it = m_statemachineMasters.find(nMasterIndex);
        if (it != m_statemachineMasters.end()) {
            it->second = pBSStatemachineMaster;
        } else {
            m_statemachineMasters.insert(std::pair<unsigned int, BSStatemachineMaster *>(nMasterIndex, pBSStatemachineMaster));
        }
    }

private:
    virtual void eventHandler(Event::Ptr event)
    {
        m_EventHandler.handleEvent(event);
    }

private:
    void OnBSEvent(const BSEvent *pEvent)
    {
        unsigned int nMasterIndex = pEvent->GetMasterIndex();
        std::map<unsigned int, BSStatemachineMaster*>::iterator it;
        it = m_statemachineMasters.find(nMasterIndex);
        if (it != m_statemachineMasters.end()) {
            BSStatemachineMaster *pBSStatemachineMaster = it->second;
            pBSStatemachineMaster->OnBSEvent(pEvent);
        }
    }

private:
    std::map<unsigned int, BSStatemachineMaster *> m_statemachineMasters;
};

class BSEventData001 : public BSEventDataBase
{
public:
    ~BSEventData001()
    {
        printf("### BSEventData001::~BSEventData001()\n");
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BSThreadEventQueue bsThreadEventQueue;
    BSStatemachineMaster bsStatemachineMaster1(&bsThreadEventQueue, 3);
    BSStatemachineMaster bsStatemachineMaster2(&bsThreadEventQueue, 2);

    bsThreadEventQueue.AddStatemachineMaster(bsStatemachineMaster1.GetMasterIndex(), &bsStatemachineMaster1);
    bsThreadEventQueue.AddStatemachineMaster(bsStatemachineMaster2.GetMasterIndex(), &bsStatemachineMaster2);

    bsStatemachineMaster1.PostServiceEvent(BSServiceSM::BSSSM_EI_EVENT1, new BSEventData001());
    bsStatemachineMaster2.PostServiceEvent(BSServiceSM::BSSSM_EI_EVENT1, new BSEventData001());

    return a.exec();
}

