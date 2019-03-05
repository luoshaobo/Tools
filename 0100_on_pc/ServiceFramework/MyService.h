#ifndef MY_SERVICE_H
#define MY_SERVICE_H

#include "stdafx.h"
#include "ServiceFramework.h"

//
// NOTE: If no new thread created, all of the below code will run in a same thread.
//

////////////////////////////////////////////////////////////////////////////////
// Application Code
//

class MyServiceEntity;
class MyJobEntity1;
class MyJobEntity2;

struct MyServiceContext
{
    //
    // TODO: Put all inforation for the servic here.
    //

    MyServiceEntity *pMyServiceEntity;
    MyJobEntity1 *pMyJobEntity1;
    MyJobEntity2 *pMyJobEntity2;
};

class MyServiceEntity : public IServiceEntity
{
public:
    MyServiceEntity(MyServiceContext &rServiceContext,IServiceStateMachine *pServiceStateMachine)
        : m_rServiceContext(rServiceContext), m_pServiceStateMachine(pServiceStateMachine)
    {
    
    }

    virtual ~MyServiceEntity()
    {
    
    }

    virtual void OnNewRequest(void *pRequestData)
    {
        //
        // TODO: Parse the new request from the client
        //
        LOG_GEN();
        LOG_GEN_PRINTF("### [MyServiceEntity] Parsing request ...\n");
    }

    virtual void OnSendResponse(const ResponseState &responseState)
    {
        //
        // TODO: Send the response to the client
        //
        LOG_GEN();
        LOG_GEN_PRINTF("### [MyServiceEntity] Sending response ...\n");
    }

private:
    MyServiceContext &m_rServiceContext;
    IServiceStateMachine *m_pServiceStateMachine;
};

class MyJobEntity1 : public IJobEntity, public SMF_ThreadTimerHandler
{
private:
    enum {
        MY_TIMERID_1 = TIMER_ID_USER_BASE,
    };
public:
    MyJobEntity1(MyServiceContext &rServiceContext, IJobStateMachine *pJobStateMachine)
        : m_rServiceContext(rServiceContext)
        , m_pJobStateMachine(pJobStateMachine)
        , m_pWorkThread(NULL)
    {
        m_pWorkThread = new SMF_BaseThread("Thread_MyJobEntity1");
        m_pWorkThread->SetTimerHandler(this);
        m_pWorkThread->Start();
    }

    virtual ~MyJobEntity1()
    {
        delete m_pWorkThread;
    }

    virtual void OnSendRequest()
    {
        //
        // TODO: Call an API.
        //
        LOG_GEN();
        LOG_GEN_PRINTF("### [MyJobEntity1] Calling API ...\n");
        m_pWorkThread->StartTimer(MY_TIMERID_1, 4000);
    }

    virtual void OnCancelRequest()
    {
        //
        // TODO: Call calling the API.
        //
        LOG_GEN();
        LOG_GEN_PRINTF("### [MyJobEntity1] Cancel calling API ...\n");
        m_pWorkThread->StopTimer(MY_TIMERID_1);
    }

    virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
    {
        LOG_GEN();

        switch (nTimerId) {
        case MY_TIMERID_1:
            {
                //
                // TODO: Simulate a response received for the API.
                //
                LOG_GEN_PRINTF("### [MyJobEntity1] API response received.\n");
                m_pJobStateMachine->OnResponseReceived();
            }
            break;
        default:
            break;
        }   

        return SMF_ERR_OK;
    }

private:
    MyServiceContext &m_rServiceContext;
    IJobStateMachine *m_pJobStateMachine;
    SMF_BaseThread *m_pWorkThread;
};

class MyJobEntity2 : public IJobEntity, public SMF_ThreadTimerHandler
{
private:
    enum {
        MY_TIMERID_1 = TIMER_ID_USER_BASE,
    };
public:
    MyJobEntity2(MyServiceContext &rServiceContext, IJobStateMachine *pJobStateMachine)
        : m_rServiceContext(rServiceContext)
        , m_pJobStateMachine(pJobStateMachine)
        , m_pWorkThread(NULL)
    {
        m_pWorkThread = new SMF_BaseThread("Thread_MyJobEntity2");
        m_pWorkThread->SetTimerHandler(this);
        m_pWorkThread->Start();
    }

    virtual ~MyJobEntity2()
    {
        delete m_pWorkThread;
    }

    virtual void OnSendRequest()
    {
        //
        // TODO: Call an API.
        //
        LOG_GEN();
        LOG_GEN_PRINTF("### [MyJobEntity2] Calling API ...\n");
        m_pWorkThread->StartTimer(MY_TIMERID_1, 10000);
    }

        virtual void OnCancelRequest()
    {
        //
        // TODO: Call calling the API.
        //
        LOG_GEN();
        LOG_GEN_PRINTF("### [MyJobEntity1] Cancel calling API ...\n");
        m_pWorkThread->StopTimer(MY_TIMERID_1);
    }

    virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
    {
        LOG_GEN();

        switch (nTimerId) {
        case MY_TIMERID_1:
            {
                //
                // TODO: Simulate a response received for the API.
                //
                LOG_GEN_PRINTF("### [MyJobEntity2] API response received.\n");
                m_pJobStateMachine->OnResponseReceived();
            }
            break;
        default:
            break;
        }   

        return SMF_ERR_OK;
    }

private:
    MyServiceContext &m_rServiceContext;
    IJobStateMachine *m_pJobStateMachine;
    SMF_BaseThread *m_pWorkThread;
};

class MyEntityFactor : public IEntityFactor
{
public:
    MyEntityFactor()
    {
    
    }

    virtual ~MyEntityFactor()
    {
    
    }

    virtual IServiceEntity *CreateServiceEntity(IServiceStateMachine *pServiceStateMachine)
    {
        LOG_GEN();
        return new MyServiceEntity(GetServiceContext(), pServiceStateMachine);
    }

    virtual IJobEntity *CreateJobEntity(IJobStateMachine *pJobStateMachine, unsigned int nIndex)
    {
        IJobEntity *pJobEntity = NULL;
        LOG_GEN();

        switch (nIndex) {
        case 0:
            {
                pJobEntity = new MyJobEntity1(GetServiceContext(), pJobStateMachine);
            }
            break;
        case 1:
            {
                pJobEntity = new MyJobEntity2(GetServiceContext(), pJobStateMachine);
            }
            break;
        default:
            // do nothing
            break;
        }

        return pJobEntity;
    }

    virtual unsigned int GetJobCount() const
    {
        return 2;
    }

private:
    static MyServiceContext &GetServiceContext()
    {
        static MyServiceContext myServiceContext;
        return myServiceContext;
    }
};

#endif // #ifndef MY_SERVICE_H
