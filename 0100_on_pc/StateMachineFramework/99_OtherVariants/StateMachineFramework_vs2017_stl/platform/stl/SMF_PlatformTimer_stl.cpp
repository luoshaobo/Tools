#include "TK_Tools.h"
#include "SMF_PlatformTimer.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformMsgQ.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// TimerThread
//
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <memory>
#include <algorithm>

class TimerThread
{
public:
    typedef std::function<void(unsigned int, void *)> TimerCallback;

private:
    typedef std::chrono::high_resolution_clock clock_t;

    struct TimerInfo
    {
        unsigned int m_nTimerId;
        unsigned int m_nInternalMs;
        clock_t::time_point m_timePoint;
        TimerCallback m_timerCallback;
        void *m_pArg;
        

        TimerInfo(unsigned int nTimerId, unsigned int nInternalMs, TimerCallback timerCallback, void *pArg)
            : m_nTimerId(nTimerId)
            , m_nInternalMs(nInternalMs)
            , m_timePoint(clock_t::now() + std::chrono::milliseconds(nInternalMs))
            , m_timerCallback(timerCallback)
            , m_pArg(pArg)
        {
        }

        TimerInfo(const TimerInfo &another)
            : m_nTimerId(another.m_nTimerId)
            , m_nInternalMs(another.m_nInternalMs)
            , m_timePoint(another.m_timePoint)
            , m_timerCallback(another.m_timerCallback)
            , m_pArg(another.m_pArg)
        {
        }
    };

public:
    TimerThread()
        : m_nCurrentTimerSerialNo(0), m_thread(), m_timers(), m_mutex(), m_condition(), m_bStop(false), m_bSort(false)
    {
        m_thread.reset(new std::thread(std::bind(&TimerThread::TimerLoop, this)));
    }

    ~TimerThread()
    {
        m_bStop = true;
        m_condition.notify_all();
        m_thread->join();
    }

    unsigned int CreateTimer(unsigned int nInternalMs, TimerCallback timerCallback, void *pArg)
    {
        //printf("--- CreateTimer()...\n");
        std::unique_lock<std::mutex> lock(m_mutex);
        //printf("--- CreateTimer() locked\n");
        unsigned int nTimerId = m_nCurrentTimerSerialNo++;

        m_timers.emplace_back(TimerInfo(nTimerId, nInternalMs, timerCallback, pArg));
        m_bSort = true;

        // wake up
        m_condition.notify_all();

        return nTimerId;
    }

    void DestroyTimer(unsigned int nTimerId)
    {
        //printf("--- DestroyTimer()...\n");
        std::unique_lock<std::mutex> lock(m_mutex);
        //printf("--- DestroyTimer() locked\n");
        std::vector<TimerInfo>::iterator it;

        for (it = m_timers.begin(); it != m_timers.end(); ++it) {
            if (it->m_nTimerId == nTimerId) {
                m_timers.erase(it);
                break;
            }
        }
    }

private:
    void TimerLoop()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            while (!m_bStop && m_timers.empty())
            {
                //printf("--- m_condition.wait(lock)...\n");
                m_condition.wait(lock);
                //printf("--- m_condition.wait(lock) OK\n");
            }

            if (m_bStop)
            {
                return;
            }

            if (m_bSort)
            {
                //Sort could be done at insert
                //but probabily this thread has time to do
                std::sort(m_timers.begin(),
                    m_timers.end(),
                    [](const TimerInfo & ti1, const TimerInfo & ti2)
                {
                    return ti1.m_timePoint > ti2.m_timePoint;
                });
                m_bSort = false;
            }

            auto now = clock_t::now();
            auto expire = m_timers.back().m_timePoint;

            if (expire > now) //can I take a nap?
            {
                auto napTime = expire - now;
                //printf("--- m_condition.wait_for(lock, napTime)...\n");
                m_condition.wait_for(lock, napTime);
                //printf("--- m_condition.wait_for(lock, napTime) OK\n");

                continue;
            }
            else
            {
                TimerInfo timerInfo = m_timers.back();
                m_timers.pop_back();
                m_timers.emplace_back(
                    TimerInfo(                              // NOTE: the new TimerInfo instance is not same as the old one!
                        timerInfo.m_nTimerId,
                        timerInfo.m_nInternalMs,
                        timerInfo.m_timerCallback,
                        timerInfo.m_pArg
                    )
                );
                m_bSort = true;

                lock.unlock();                              // NOTE: the callback should be called on unlocked!
                timerInfo.m_timerCallback(timerInfo.m_nTimerId, timerInfo.m_pArg);
                lock.lock();
            }
        }
    }

private:
    unsigned int                 m_nCurrentTimerSerialNo;
    std::unique_ptr<std::thread> m_thread;
    std::vector<TimerInfo>       m_timers;
    std::mutex                   m_mutex;
    std::condition_variable      m_condition;
    bool                         m_bSort;
    bool                         m_bStop;
};

static TimerThread &GetTimerThread()
{
    static TimerThread timerThread;
    return timerThread;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimerImpl
//

class SMF_PlatformTimerImpl : private SMF_NonCopyable, public SMF_PlatformTimerHandler
{
private:
    typedef std::map<TimerId, bool> TimerRepeatMap;
    typedef TimerThread *timer_t;

    struct TimerContext {
        TimerContext(SMF_PlatformTimerImpl *a_pPlatformTimerImpl, TimerId a_nTimerId, timer_t a_oTimer, unsigned int nInternalTimerId)
            : pPlatformTimerImpl(a_pPlatformTimerImpl), nTimerId(a_nTimerId), oTimer(a_oTimer), m_nInternalTimerId(nInternalTimerId) {}

        SMF_PlatformTimerImpl *pPlatformTimerImpl;
        TimerId nTimerId;
        timer_t oTimer;
        unsigned int m_nInternalTimerId;
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
    SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName = NULL, bool bSharedInProcesses = false);
    virtual ~SMF_PlatformTimerImpl();

public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat = false);
    SMF_ErrorCode StopTimer(TimerId nTimerId);

public:
    SMF_ErrorCode SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler);

public:
    SMF_PlatformThread * GetConnectedThread();

public:
    virtual SMF_ErrorCode PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId);

private:
    SMF_ErrorCode PlatformCreateTimer(TimerId nTimerId, unsigned int nInterval);
    SMF_ErrorCode PlatformDestroyTimer(TimerId nTimerId);
    SMF_ErrorCode PlatformStartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat);
    SMF_ErrorCode PlatformStopTimer(TimerId nTimerId);

private:
    bool TimerContextExists(TimerId nTimerId);
    TimerContext *GetTimerContext(TimerId nTimerId);
    void SetTimerContext(TimerId nTimerId, TimerContext *pTimerContext);

private:
    SMF_PlatformTimerHandler * GetTimerHandler();

    void SetTimerRepeat(TimerId nTimerId, bool bRepeat);
    bool GetTimerRepeat(TimerId nTimerId);
    void RemoveTimerRepeat(TimerId nTimerId);

private:
    static void PTimerProcStatic(unsigned int nTimerId, void *arg);
    void PTimerProc(TimerId nTimerId);

private:
    void Lock();
    void Unlock();

private:
    std::string GetTimerName() const;
    std::string GetLockName() const;

private:
    SMF_PlatformTimer * m_pApi;
    std::string m_sName;
    SMF_PlatformTimerHandler *m_pTimerHandler;
    TimerRepeatMap m_mapTimeRepeat;
    SMF_PlatformLock m_oLock;
    TimerContextTable m_oTimerContextTable;
    SMF_PlatformThread *m_pConnectedThread;
};

SMF_PlatformTimerImpl::SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
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
            }
            else {
                break;
            }
        }
    }
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformCreateTimer(TimerId nTimerId, unsigned int nInterval)
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
        pTimerContext = new TimerContext(this, nTimerId, 0, -1);
        if (pTimerContext == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pTimerContext->oTimer = &GetTimerThread();
        if (pTimerContext->oTimer == 0) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_TIMER;
            delete pTimerContext;
        } else {
            pTimerContext->m_nInternalTimerId = pTimerContext->oTimer->CreateTimer(nInterval, &PTimerProcStatic, pTimerContext);
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
            pTimerContext->oTimer->DestroyTimer(pTimerContext->m_nInternalTimerId);
            SetTimerContext(nTimerId, NULL);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformStartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformStopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

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

void SMF_PlatformTimerImpl::PTimerProcStatic(unsigned int nTimerId, void *arg)
{
    TimerContext *pTimerContext = reinterpret_cast<TimerContext *>(arg);
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
            pMsgQ->SendMsg(SMF_PlatformMsgQHandler::Msg(SMF_PlatformMsgQHandler::PLATFORM_MSG_ID_TIMER, 0, (LPARAM)reinterpret_cast<void *>(&oPlatformTimerEventArgs), true));
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
            nErrorCode = PlatformCreateTimer(nTimerId, nInterval);
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
            return nErrorCode;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PlatformStopTimer(nTimerId);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PlatformDestroyTimer(nTimerId);
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
    }
    else {
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

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformTimerImpl_Timer_%s", SMF_GetDomainStr(false).c_str(), m_sName.c_str());

    return sResult;
}

std::string SMF_PlatformTimerImpl::GetLockName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformTimerImpl_Lock_%s", SMF_GetDomainStr(false).c_str(), m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimer
//
SMF_PlatformTimer::SMF_PlatformTimer(const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformTimerImpl(this, pName, bSharedInProcesses);
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
    }
    else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->StopTimer(nTimerId);
    }
    else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetTimerHandler(pTimerHandler);
    }
    else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformTimerProc(rPlatformTimer, nTimerId);
    }
    else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ConnectToThread(pThread);
    }
    else {
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
