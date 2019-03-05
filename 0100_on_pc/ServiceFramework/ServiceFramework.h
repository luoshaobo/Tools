#ifndef SERVICE_FRAMEWORK_H
#define SERVICE_FRAMEWORK_H

#include "stdafx.h"
#include <vector>
#include <list>
#include <string>
#include "SMF_afx.h"
#include "SMF_BaseThread.h"
#include "SMF_PlatformLock.h"

////////////////////////////////////////////////////////////////////////////////
// Framework Code
//
interface IServiceEntity
{
    enum ResponseState {
        RESPSTA_SUCCESSFUL,
        RESPSTA_FAILED,
        RESPSTA_TIMEDOUT,
    };

    virtual ~IServiceEntity() {}

    virtual void OnNewRequest(void *pRequestData) = 0;
    virtual void OnSendResponse(const ResponseState &responseState) = 0;
};

interface IJobEntity
{
    enum ResponseState {
        RESPSTA_SUCCESSFUL,
        RESPSTA_FAILED,
        RESPSTA_TIMEDOUT,
    };

    virtual ~IJobEntity() {}

    virtual void OnSendRequest() = 0;
    virtual void OnCancelRequest() = 0;
};

interface IServiceStateMachine
{
    enum State {
        STATE_NOT_STARTED,
        STATE_PROCESSING,
        STATE_FINISHED,
    };
    enum Status {
        STATUS_OK,
        STATUS_FAILED,
        STATUS_TIMEDOUT,
    };

    virtual ~IServiceStateMachine() {}

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual void NewRequest(void *pRequestData) = 0;

    virtual State GetState() const = 0;
    virtual Status GetStatus() const = 0;

    virtual void OnJobStateMachineNotify(unsigned int nJobIndex) = 0;
};

interface IJobStateMachine
{
    enum State {
        STATE_NOT_STARTED,
        STATE_PROCESSING,
        STATE_FINISHED,
    };
    enum Status {
        STATUS_OK,
        STATUS_FAILED,
        STATUS_TIMEDOUT,
    };

    virtual ~IJobStateMachine() {}

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual State GetState() const = 0;
    virtual Status GetStatus() const = 0;

    virtual void OnResponseReceived() = 0;
};

interface IEntityFactor
{
    virtual ~IEntityFactor() {}

    virtual IServiceEntity *CreateServiceEntity(IServiceStateMachine *pServiceStateMachine) = 0;
    virtual IJobEntity *CreateJobEntity(IJobStateMachine *pJobStateMachine, unsigned int nIndex) = 0;

    virtual unsigned int GetJobCount() const = 0;
};

class JobStateMachine : public IJobStateMachine, public SMF_ThreadEventQueueHandler
{
private:
    enum StateMachineEventId {
        SMEID_ON_SEND_REQUEST = SMF_EVTID_USR_BASE,
        SMEID_ON_RESPONSE_RECEIVED,
    };
public:
    JobStateMachine(SMF_BaseThread *pStateMachineThread, IServiceStateMachine *pServiceStateMachine, IEntityFactor *pEntityFactor, unsigned int nIndex)
        : m_oLock()
        , m_state(STATE_NOT_STARTED), m_status(STATUS_OK)
        , m_pStateMachineThread(pStateMachineThread)
        , m_pServiceStateMachine(pServiceStateMachine), m_pEntityFactor(pEntityFactor)
        , m_nIndex(nIndex), m_pJobEntity(NULL)
    {
        m_pStateMachineThread->SetEventQueueHandler(nIndex + 1, this);
        m_pJobEntity = m_pEntityFactor->CreateJobEntity(this, nIndex);
    }

    ~JobStateMachine()
    {
        delete m_pJobEntity;
    }

    virtual void Start()
    {
        LOG_GEN();

        m_oLock.Lock();
        m_state = STATE_PROCESSING;
        m_oLock.Unlock();

        PostEvent(SMEID_ON_SEND_REQUEST);
    }

    virtual void Stop()
    {
        LOG_GEN();
        m_pJobEntity->OnCancelRequest();
    }

    virtual State GetState() const
    {
        return m_state;
    }

    virtual Status GetStatus() const
    {
        return m_status;
    }

    virtual void OnResponseReceived()
    {
        LOG_GEN();

        m_oLock.Lock();
        m_state = STATE_FINISHED;
        m_oLock.Unlock();

        PostEvent(SMEID_ON_RESPONSE_RECEIVED);
    }

    virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
    {
        LOG_GEN();
        switch (rEventData.nEventId) {
        case SMEID_ON_SEND_REQUEST:
            {
                m_pJobEntity->OnSendRequest();
            }
            break;
        case SMEID_ON_RESPONSE_RECEIVED:
            {
                m_oLock.Lock();
                m_status = STATUS_OK;
                m_oLock.Unlock();

                m_pServiceStateMachine->OnJobStateMachineNotify(m_nIndex);
            }
            break;
        default:
            break;
        }
        return SMF_ERR_OK;
    }

private:
    void PostEvent(EventId nEventId)
    {
        m_pStateMachineThread->PostEvent(m_nIndex + 1, 0, 0, nEventId, NULL);
    }

private:
    SMF_PlatformLock m_oLock;
    State m_state;
    Status m_status;
    SMF_BaseThread *m_pStateMachineThread;
    IServiceStateMachine *m_pServiceStateMachine;
    IEntityFactor *m_pEntityFactor;
    unsigned int m_nIndex;
    IJobEntity *m_pJobEntity;
};

class ServiceStateMachine : public IServiceStateMachine, public SMF_ThreadEventQueueHandler
{
private:
    enum StateMachineEventId {
        SMEID_ON_REQUEST_RECEIVED = SMF_EVTID_USR_BASE,
        SMEID_ON_SUB_JOB_FINISHED,
        SMEID_ON_GO_TO_NEXT_JOB,
    };
public:
    ServiceStateMachine(IEntityFactor *pEntityFactor)
        : m_oLock()
        , m_state(STATE_NOT_STARTED), m_status(STATUS_OK)
        , m_pStateMachineThread(NULL)
        , m_pEntityFactor(pEntityFactor)
        , m_pServiceEntity(NULL), m_jobStateMachines()
        , m_pendingRequestData()
        , m_bRequestProcessing(false)
        , m_nCurrentJobIndex(0)
    {
        m_pStateMachineThread = new SMF_BaseThread("Thread_ServiceStateMachine");
        m_pStateMachineThread->SetEventQueueHandler(0, this);

        m_pServiceEntity = m_pEntityFactor->CreateServiceEntity(this);
        for (unsigned int i = 0; i < m_pEntityFactor->GetJobCount(); ++i) {
            IJobStateMachine *pJobStateMachine = new JobStateMachine(m_pStateMachineThread, this, m_pEntityFactor, i);
            m_jobStateMachines.push_back(pJobStateMachine);
        }
    }

    ~ServiceStateMachine()
    {
        delete m_pStateMachineThread;
        m_pStateMachineThread = NULL;

        for (unsigned int i = 0; i < m_pEntityFactor->GetJobCount(); ++i) {
            IJobStateMachine *pJobStateMachine = m_jobStateMachines[i];
            delete pJobStateMachine;
        }
        delete m_pServiceEntity;
        m_pServiceEntity = NULL;
    }

    virtual void Start()
    {
        LOG_GEN();
        m_pStateMachineThread->Start();
    }

    virtual void Stop()
    {
        LOG_GEN();
        m_pStateMachineThread->Stop();
    }

    virtual void NewRequest(void *pRequestData)
    {
        LOG_GEN();

        m_oLock.Lock();
        m_pendingRequestData.push_back(pRequestData);
        if (!m_bRequestProcessing) {
            PostEvent(SMEID_ON_REQUEST_RECEIVED);
            m_bRequestProcessing = true;
        }
        m_oLock.Unlock();
    }

    virtual State GetState() const
    {
        return m_state;
    }

    virtual Status GetStatus() const
    {
        return m_status;
    }

    virtual void OnJobStateMachineNotify(unsigned int nJobIndex)
    {
        LOG_GEN();

        m_oLock.Lock();
        if (nJobIndex == m_nCurrentJobIndex) {
            PostEvent(SMEID_ON_SUB_JOB_FINISHED);
        }
        m_oLock.Unlock();
    }

    virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
    {
        LOG_GEN();
        switch (rEventData.nEventId) {
        case SMEID_ON_REQUEST_RECEIVED:
            {
                void *pRequestData = NULL;
                m_oLock.Lock();
                pRequestData = m_pendingRequestData.front();
                m_oLock.Unlock();

                m_pServiceEntity->OnNewRequest(pRequestData);
                m_jobStateMachines[m_nCurrentJobIndex]->Start();
            }
            break;
        case SMEID_ON_SUB_JOB_FINISHED:
            {
                if (m_jobStateMachines[m_nCurrentJobIndex]->GetStatus() == IJobStateMachine::STATUS_OK) {
                    PostEvent(SMEID_ON_GO_TO_NEXT_JOB);
                } else {
                    m_oLock.Lock();
                    m_nCurrentJobIndex = 0;
                    m_oLock.Unlock();

                    m_pServiceEntity->OnSendResponse(IServiceEntity::RESPSTA_FAILED);

                    m_oLock.Lock();
                    m_pendingRequestData.pop_front();
                    m_oLock.Unlock();

                    HandleNextRequest();
                }
            }
            break;
        case SMEID_ON_GO_TO_NEXT_JOB:
            {
                m_nCurrentJobIndex++;
                if (m_nCurrentJobIndex < m_pEntityFactor->GetJobCount()) {
                    m_jobStateMachines[m_nCurrentJobIndex]->Start();
                } else {
                    m_oLock.Lock();
                    m_nCurrentJobIndex = 0;
                    m_oLock.Unlock();

                    m_pServiceEntity->OnSendResponse(IServiceEntity::RESPSTA_SUCCESSFUL);
                    
                    m_oLock.Lock();
                    m_pendingRequestData.pop_front();
                    m_oLock.Unlock();

                    HandleNextRequest();
                }
            }
            break;
        default:
            break;
        }
        return SMF_ERR_OK;
    }

private:
    void PostEvent(EventId nEventId)
    {
        m_pStateMachineThread->PostEvent(0, 0, 0, nEventId, NULL);
    }

    void HandleNextRequest()
    {
        m_oLock.Lock();
        if (m_pendingRequestData.size() > 0) {
            PostEvent(SMEID_ON_REQUEST_RECEIVED);
        } else {
            m_bRequestProcessing = true;
        }
        m_oLock.Unlock();
    }

private:
    SMF_PlatformLock m_oLock;
    State m_state;
    Status m_status;
    SMF_BaseThread *m_pStateMachineThread;
    IEntityFactor *m_pEntityFactor;
    IServiceEntity *m_pServiceEntity;
    std::vector<IJobStateMachine *> m_jobStateMachines;
    std::list<void *> m_pendingRequestData;
    bool m_bRequestProcessing;

    unsigned int m_nCurrentJobIndex;
};

#endif // #ifndef SERVICE_FRAMEWORK_H
