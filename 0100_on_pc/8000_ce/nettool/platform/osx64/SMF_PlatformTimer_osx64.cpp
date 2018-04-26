#include "TK_Tools.h"
#include "SMF_PlatformTimer.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformMsgQ.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformAfx_osx64.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimerImpl
//

class SMF_PlatformTimerImpl : private SMF_NonCopyable, public SMF_PlatformTimerHandler, public SMF_PlatformThreadHandler
{
private:
    enum ThreadState {
        ST_INVALID = 0,
        ST_INIT,
        ST_ACTIVE,
        ST_TO_BE_DESTROYED,
        ST_DESTROYED,
    };
    struct TimerContext {
        TimerContext(SMF_PlatformTimerImpl *a_pPlatformTimerImpl, TimerId a_nTimerId, unsigned int a_nInterval, bool a_bRepeat)
            : pPlatformTimerImpl(a_pPlatformTimerImpl), nTimerId(a_nTimerId), nInterval(a_nInterval), bRepeat(a_bRepeat), nTimeLeft(0) {}

        SMF_PlatformTimerImpl *pPlatformTimerImpl;
        TimerId nTimerId;
        unsigned int nInterval;
        bool bRepeat;
        int nTimeLeft;
    };
    typedef std::map<TimerId, TimerContext *> TimerContextTable;

    struct PlatformTimerEventArgs {
        PlatformTimerEventArgs(SMF_PlatformTimerHandler *a_pPlatformTimerHandler, SMF_PlatformTimer &a_rPlatformTimer, TimerId a_nTimerId)
            : pPlatformTimerHandler(a_pPlatformTimerHandler), rPlatformTimer(a_rPlatformTimer), nTimerId(a_nTimerId) {}

        SMF_PlatformTimerHandler *pPlatformTimerHandler;
        SMF_PlatformTimer &rPlatformTimer;
        TimerId nTimerId;
    };

public:
    SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName = NULL);
    virtual ~SMF_PlatformTimerImpl();

public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat = false);
    SMF_ErrorCode StopTimer(TimerId nTimerId);

public:
    SMF_ErrorCode SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler);

public:
    SMF_PlatformThread *GetConnectedThread();
    TimerId GetTimerId();

public:
    virtual SMF_ErrorCode PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId);
    virtual SMF_ErrorCode PlatformThreadProc(SMF_PlatformThread &rPlatformThread);

private:
    SMF_ErrorCode PlatformCreateTimer(TimerId nTimerId);
    SMF_ErrorCode PlatformDestroyTimer(TimerId nTimerId);
    SMF_ErrorCode PlatformStartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat);
    SMF_ErrorCode PlatformStopTimer(TimerId nTimerId);

private:
    bool TimerContextExists(TimerId nTimerId);
    TimerContext *GetTimerContext(TimerId nTimerId);
    void SetTimerContext(TimerId nTimerId, TimerContext *pTimerContext);

private:
    SMF_PlatformTimerHandler *GetTimerHandler();

private:
    void Lock();
    void Unlock();

private:
    std::string GetTimerName() const;
    std::string GetTimerThreadName() const;
    std::string GetLockName() const;

private:
    SMF_PlatformTimer *m_pApi;
    const char *m_pName;
    std::string m_sName;
    SMF_PlatformTimerHandler *m_pTimerHandler;
    SMF_PlatformLock m_oLock;
    TimerContextTable m_oTimerContextTable;
    SMF_PlatformThread *m_pConnectedThread;
    SMF_PlatformThread m_oTimerThread;
    ThreadState m_oThreadState;
    SMF_PlatformEvent m_oWaitedEvent;
};

SMF_PlatformTimerImpl::SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
    , m_pTimerHandler(NULL)
    , m_oLock(/*GetLockName().c_str()*/)
    , m_oTimerContextTable()
    , m_pConnectedThread(NULL)
    , m_oTimerThread(GetTimerThreadName().c_str())
    , m_oThreadState(ST_INVALID)
    , m_oWaitedEvent(false, false)
{
    m_oTimerThread.SetThreadHandler(this);
    m_oTimerThread.Start();
}

SMF_PlatformTimerImpl::~SMF_PlatformTimerImpl()
{
    while (m_oThreadState != ST_DESTROYED) {
        m_oThreadState = ST_TO_BE_DESTROYED;
        TK_Tools::Sleep(SMF_PLATFORM_TIMER_THREAD_INTERVAL);
    }

    ConnectToThread(NULL);

    {
        TimerContext *pTimerContext = NULL;
        TimerContextTable::iterator it;

        while (true) {
            Lock();
            it = m_oTimerContextTable.begin();
            Unlock();
            if (it != m_oTimerContextTable.end()) {
                pTimerContext = (*it).second;
                if (pTimerContext != NULL) {
                    PlatformDestroyTimer(pTimerContext->nTimerId);
                }
            } else {
                break;
            }
        }
    }
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformCreateTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    TimerContext *pTimerContext = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext != NULL) {
            nErrorCode = PlatformDestroyTimer(nTimerId);
            pTimerContext = NULL;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = new TimerContext(this, nTimerId, 0, false);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetTimerContext(nTimerId, pTimerContext);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformDestroyTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    TimerContext *pTimerContext = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext != NULL) {
            SetTimerContext(nTimerId, NULL);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformStartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    TimerContext *pTimerContext = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        pTimerContext->nInterval = nInterval;
        pTimerContext->bRepeat = bRepeat;
        pTimerContext->nTimeLeft = nInterval;
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformStopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    TimerContext *pTimerContext = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        pTimerContext->bRepeat = false;
        pTimerContext->nTimeLeft = 0;
        Unlock();
    }

    return nErrorCode;
}

bool SMF_PlatformTimerImpl::TimerContextExists(TimerId nTimerId)
{
    bool bExists = false;
    TimerContextTable::iterator it;

    Lock();
    it = m_oTimerContextTable.find(nTimerId);
    if (it != m_oTimerContextTable.end()) {
        bExists = true;
    }
    Unlock();

    return bExists;
}

SMF_PlatformTimerImpl::TimerContext *SMF_PlatformTimerImpl::GetTimerContext(TimerId nTimerId)
{
    TimerContext *pTimerContext = NULL;
    TimerContextTable::iterator it;

    Lock();
    it = m_oTimerContextTable.find(nTimerId);
    if (it != m_oTimerContextTable.end()) {
        pTimerContext = (*it).second;
    }
    Unlock();

    return pTimerContext;
}

void SMF_PlatformTimerImpl::SetTimerContext(TimerId nTimerId, TimerContext *pTimerContext)
{
    TimerContext *pTimerContextOld = NULL;
    TimerContextTable::iterator it;

    Lock();
    it = m_oTimerContextTable.find(nTimerId);
    if (it != m_oTimerContextTable.end()) {
        pTimerContextOld = (*it).second;
        m_oTimerContextTable.erase(it);
    }
    if (pTimerContextOld != NULL) {
        delete pTimerContextOld;
    }

    if (pTimerContext != NULL) {
        m_oTimerContextTable.insert(TimerContextTable::value_type(nTimerId, pTimerContext));
    }
    Unlock();
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformThreadProc(SMF_PlatformThread &rPlatformThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformThread *pThread;
    SMF_PlatformMsgQ *pMsgQ;
    TimerContextTable::iterator it;
    TimerContext *pTimerContext;
    std::vector<TimerId> arrTimerIds;
    std::vector<TimerId>::iterator itTimerId;

    m_oThreadState = ST_INIT;

    while (true) {
        m_oThreadState = ST_ACTIVE;

        if (m_oThreadState == ST_TO_BE_DESTROYED) {
            break;
        }

        TK_Tools::Sleep(SMF_PLATFORM_TIMER_THREAD_INTERVAL);

        if (m_oThreadState == ST_TO_BE_DESTROYED) {
            break;
        }

        arrTimerIds.clear();
        Lock();
        for (it = m_oTimerContextTable.begin(); it != m_oTimerContextTable.end(); ++it) {
            pTimerContext = (*it).second;
            if (pTimerContext == NULL) {
                continue;
            }
            if (pTimerContext->nTimeLeft > 0) {
                pTimerContext->nTimeLeft -= SMF_PLATFORM_TIMER_THREAD_INTERVAL;
                if (pTimerContext->nTimeLeft <= 0) {
                    pTimerContext->nTimeLeft = 0;
                    arrTimerIds.push_back(pTimerContext->nTimerId);
                    if (pTimerContext->bRepeat) {
                        pTimerContext->nTimeLeft = pTimerContext->nInterval;
                    }
                }
            }
        }
        Unlock();

        pThread = m_pApi->GetConnectedThread();
        if (pThread != NULL) {
            pMsgQ = pThread->GetConnectedMsgQ();
        } else {
            pMsgQ = NULL;
        }
        if (pMsgQ == NULL) {
            continue;
        }

        for (itTimerId = arrTimerIds.begin(); itTimerId != arrTimerIds.end(); ++itTimerId) {
            PlatformTimerEventArgs oPlatformTimerEventArgs(GetTimerHandler(), *m_pApi, *itTimerId);
            pMsgQ->SendMsg(SMF_PlatformMsgQHandler::Msg(
                                                            SMF_PlatformMsgQHandler::PLATFORM_MSG_ID_TIMER,
                                                            0,
                                                            reinterpret_cast<SMF_PlatformMsgQ::LParam>(&oPlatformTimerEventArgs),
                                                            true
                                                       ),
                           &m_oWaitedEvent);
        }
    }

    m_oThreadState = ST_DESTROYED;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat /*= false*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (!TimerContextExists(nTimerId)) {
            nErrorCode = PlatformCreateTimer(nTimerId);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PlatformStartTimer(nTimerId, nInterval, bRepeat);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (!TimerContextExists(nTimerId)) {
            nErrorCode = PlatformCreateTimer(nTimerId);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PlatformStopTimer(nTimerId);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_pTimerHandler = pTimerHandler;
    }

    return nErrorCode;
}

SMF_PlatformTimerHandler *SMF_PlatformTimerImpl::GetTimerHandler()
{
    SMF_PlatformTimerHandler *pTimerHandler = m_pApi;

    if (m_pTimerHandler != NULL) {
        pTimerHandler = m_pTimerHandler;
    }

    return pTimerHandler;
}

SMF_ErrorCode SMF_PlatformTimerImpl::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedTimer(NULL);
            Lock();
            m_pConnectedThread = NULL;
            Unlock();
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pConnectedThread = pThread;
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedTimer(this->m_pApi);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformTimerImpl::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    Lock();
    pPlatformThread = m_pConnectedThread;
    Unlock();

    return pPlatformThread;
}

void SMF_PlatformTimerImpl::Lock()
{
    m_oLock.Lock();
}

void SMF_PlatformTimerImpl::Unlock()
{
    m_oLock.Unlock();
}

std::string SMF_PlatformTimerImpl::GetTimerName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformTimerImpl_Timer_%s", m_sName.c_str());

    return sResult;
}

std::string SMF_PlatformTimerImpl::GetTimerThreadName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformTimerImpl_TimerThread_%s", m_sName.c_str());

    return sResult;
}

std::string SMF_PlatformTimerImpl::GetLockName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformTimerImpl_Lock_%s", m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimer
//
SMF_PlatformTimer::SMF_PlatformTimer(const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformTimerImpl(this, pName);
}

SMF_PlatformTimer::~SMF_PlatformTimer()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformTimer::StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat /*= false*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->StartTimer(nTimerId, nInterval, bRepeat);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->StopTimer(nTimerId);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetTimerHandler(pTimerHandler);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformTimerProc(rPlatformTimer, nTimerId);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ConnectToThread(pThread);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformTimer::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    if (m_pImpl != NULL) {
        pPlatformThread = m_pImpl->GetConnectedThread();
    }

    return pPlatformThread;
}

