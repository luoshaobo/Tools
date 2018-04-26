#include "Tools.h"
#include "SMF_PlatformTimer.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformMsgQ.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimerImpl
//

class SMF_PlatformTimerImpl : private SMF_NonCopyable, public SMF_PlatformTimerHandler
{
private:
    typedef std::map<TimerId, bool> TimerRepeatMap;

    struct TimerContext {
        TimerContext(SMF_PlatformTimerImpl *a_pPlatformTimerImpl, TimerId a_nTimerId, timer_t a_oTimer)
            : pPlatformTimerImpl(a_pPlatformTimerImpl), nTimerId(a_nTimerId), oTimer(a_oTimer) {}

        SMF_PlatformTimerImpl *pPlatformTimerImpl;
        TimerId nTimerId;
        timer_t oTimer;
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

    void SetTimerRepeat(TimerId nTimerId, bool bRepeat);
    bool GetTimerRepeat(TimerId nTimerId);
    void RemoveTimerRepeat(TimerId nTimerId);

private:
    static void PTimerProcStatic(union sigval sigval);
    void PTimerProc(TimerId nTimerId);

private:
    void Lock();
    void Unlock();

private:
    std::string GetTimerName() const;
    std::string GetLockName() const;

private:
    SMF_PlatformTimer *m_pApi;
    const char *m_pName;
    std::string m_sName;
    SMF_PlatformTimerHandler *m_pTimerHandler;
    TimerRepeatMap m_mapTimeRepeat;
    SMF_PlatformLock m_oLock;
    TimerContextTable m_oTimerContextTable;
    SMF_PlatformThread *m_pConnectedThread;
};

SMF_PlatformTimerImpl::SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
    , m_pTimerHandler(NULL)
    , m_mapTimeRepeat()
    , m_oLock(/*GetLockName().c_str()*/)
    , m_oTimerContextTable()
    , m_pConnectedThread(NULL)
{

}

SMF_PlatformTimerImpl::~SMF_PlatformTimerImpl()
{
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
    int nRet;
    TimerContext *pTimerContext = NULL;
    struct sigevent se;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext != NULL) {
            nErrorCode = PlatformDestroyTimer(nTimerId);
            pTimerContext = NULL;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = new TimerContext(this, nTimerId, 0);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        memset(&se, 0, sizeof(struct sigevent));
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = reinterpret_cast<void *>(pTimerContext);
        se.sigev_notify_function = &PTimerProcStatic;
        nRet = ::timer_create(CLOCK_REALTIME, &se, &pTimerContext->oTimer);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_TIMER;
            delete pTimerContext;
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
    int nRet;
    TimerContext *pTimerContext = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext != NULL) {
            nRet = ::timer_delete(pTimerContext->oTimer);
            if (nRet != 0) {
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_TIMER;
            }
            SetTimerContext(nTimerId, NULL);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformStartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    TimerContext *pTimerContext = NULL;
    struct itimerspec its;
    struct timespec *ts;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        memset(&its, 0, sizeof(struct itimerspec));
        if (bRepeat) {
            ts = &its.it_interval;
            ts->tv_sec = nInterval / 1000;
            ts->tv_nsec = nInterval % 1000 * 1000000;

            ts = &its.it_value;
            ts->tv_sec = nInterval / 1000;
            ts->tv_nsec = nInterval % 1000 * 1000000;
        } else {
            ts = &its.it_value;
            ts->tv_sec = nInterval / 1000;
            ts->tv_nsec = nInterval % 1000 * 1000000;
        }

        nRet = ::timer_settime(pTimerContext->oTimer, 0, &its, NULL);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_FAILED_TO_START_TIMER;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformStopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    TimerContext *pTimerContext = NULL;
    struct itimerspec its;

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext = GetTimerContext(nTimerId);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        memset(&its, 0, sizeof(struct itimerspec));
        nRet = ::timer_settime(pTimerContext->oTimer, 0, &its, NULL);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_FAILED_TO_STOP_TIMER;
        }
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

void SMF_PlatformTimerImpl::PTimerProcStatic(union sigval sigval)
{
    TimerContext *pTimerContext = reinterpret_cast<TimerContext *>(sigval.sival_ptr);
    SMF_PlatformTimerImpl *pThis;

    if (pTimerContext != NULL) {
        pThis = pTimerContext->pPlatformTimerImpl;
        if (pThis != NULL) {
            pThis->PTimerProc(pTimerContext->nTimerId);
        }
    }
}

void SMF_PlatformTimerImpl::PTimerProc(TimerId nTimerId)
{
    SMF_PlatformThread *pThread = m_pApi->GetConnectedThread();
    SMF_PlatformMsgQ *pMsgQ;

    if (pThread != NULL) {
        pMsgQ = pThread->GetConnectedMsgQ();
        if (pMsgQ != NULL) {
            PlatformTimerEventArgs oPlatformTimerEventArgs(GetTimerHandler(), *m_pApi, nTimerId);
            pMsgQ->SendMsg(SMF_PlatformMsgQHandler::Msg(SMF_PlatformMsgQHandler::PLATFORM_MSG_ID_TIMER, 0, reinterpret_cast<void *>(&oPlatformTimerEventArgs), true));
        }
    }
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

void SMF_PlatformTimerImpl::SetTimerRepeat(SMF_PlatformTimer::TimerId nTimerId, bool bRepeat)
{
    m_oLock.Lock();
    TimerRepeatMap::iterator it = m_mapTimeRepeat.find(nTimerId);
    if (it != m_mapTimeRepeat.end()) {
        (*it).second = bRepeat;
    } else {
        m_mapTimeRepeat.insert(TimerRepeatMap::value_type(nTimerId, bRepeat));
    }
    m_oLock.Unlock();
}

bool SMF_PlatformTimerImpl::GetTimerRepeat(SMF_PlatformTimer::TimerId nTimerId)
{
    bool bRepeat = false;

    m_oLock.Lock();
    TimerRepeatMap::iterator it = m_mapTimeRepeat.find(nTimerId);
    if (it != m_mapTimeRepeat.end()) {
        bRepeat = (*it).second;
    }
    m_oLock.Unlock();

    return bRepeat;
}

void SMF_PlatformTimerImpl::RemoveTimerRepeat(SMF_PlatformTimer::TimerId nTimerId)
{
    m_oLock.Lock();
    TimerRepeatMap::iterator it = m_mapTimeRepeat.find(nTimerId);
    if (it != m_mapTimeRepeat.end()) {
        m_mapTimeRepeat.erase(it);
    }
    m_oLock.Unlock();
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

