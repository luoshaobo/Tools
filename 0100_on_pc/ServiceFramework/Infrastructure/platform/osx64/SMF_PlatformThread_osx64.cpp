#include "TK_Tools.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformMsgQ.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformThreadImpl
//

class SMF_PlatformThreadImpl : private SMF_NonCopyable, public SMF_PlatformThreadHandler
{
public:
    SMF_PlatformThreadImpl(SMF_PlatformThread *m_pApi, const char *pName = NULL, void *pThreadData = NULL);
    virtual ~SMF_PlatformThreadImpl();

public:
    SMF_ErrorCode Start();
    SMF_ErrorCode Stop();

public:
    SMF_ErrorCode SetThreadHandler(SMF_PlatformThreadHandler *pThreadHandler);
    SMF_ErrorCode SetConnectedMsgQ(SMF_PlatformMsgQ *pMsgQ);
    SMF_ErrorCode SetConnectedTimer(SMF_PlatformTimer *pTimer);

public:
    SMF_PlatformMsgQ *GetConnectedMsgQ();
    SMF_PlatformTimer *GetConnectedTimer();

public:
    static SMF_PlatformThread::ThreadId GetCurrentThreadId();
    static bool IsThreadEqual(SMF_PlatformThread::ThreadId nThreadId1, SMF_PlatformThread::ThreadId nThreadId2);
    SMF_PlatformThread::ThreadId GetThreadId();

public:
    virtual SMF_ErrorCode PlatformThreadProc(SMF_PlatformThread &rPlatformThread);

private:
    static void *PthreadProcStatic(void *pArgs);
    void *PthreadProc();

private:
    SMF_PlatformThreadHandler *GetThreadHandler();

private:
    void Lock();
    void Unlock();

private:
    SMF_PlatformThread *m_pApi;
    std::string m_sName;
    const char *m_pName;
    bool m_bThreadValid;
    pthread_t m_oThread;
    void *m_pThreadData;
    SMF_PlatformThreadHandler *m_pThreadHandler;
    SMF_PlatformLock m_oLock;
    SMF_PlatformMsgQ *m_pConnectedMsgQ;
    SMF_PlatformTimer *m_pConnectedTimer;
};

SMF_PlatformThreadImpl::SMF_PlatformThreadImpl(SMF_PlatformThread *pApi, const char *pName /*= NULL*/, void *pThreadData /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bThreadValid(false)
    , m_oThread(0)
    , m_pThreadData(pThreadData)
    , m_pThreadHandler(NULL)
    , m_oLock()
    , m_pConnectedMsgQ(NULL)
    , m_pConnectedTimer(NULL)
{

}

SMF_PlatformThreadImpl::~SMF_PlatformThreadImpl()
{
    Stop();
}

void *SMF_PlatformThreadImpl::PthreadProcStatic(void *pArgs)
{
    void *pRet = 0;
    SMF_PlatformThreadImpl *pThis = static_cast<SMF_PlatformThreadImpl *>(pArgs);

    if (pThis != NULL) {
        pRet = pThis->PthreadProc();
    }

    return pRet;
}

void *SMF_PlatformThreadImpl::PthreadProc()
{
    void *pRet = 0;
    SMF_PlatformThreadHandler *pThreadHandler = GetThreadHandler();

    if (pThreadHandler != NULL) {
        pRet = reinterpret_cast<void *>(pThreadHandler->PlatformThreadProc(*m_pApi));
    }

    return pRet;
}

SMF_ErrorCode SMF_PlatformThreadImpl::Start()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    if (nErrorCode == SMF_ERR_OK) {
        nRet = ::pthread_create(&m_oThread, NULL, &PthreadProcStatic, this);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_THREAD;
        } else {
            m_bThreadValid = true;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThreadImpl::Stop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pConnectedMsgQ != NULL) {
        nErrorCode = m_pConnectedMsgQ->PostQuitMsg();
    }
    m_bThreadValid = false;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThreadImpl::SetThreadHandler(SMF_PlatformThreadHandler *pThreadHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_pThreadHandler = pThreadHandler;
    }

    return nErrorCode;
}

SMF_PlatformThreadHandler *SMF_PlatformThreadImpl::GetThreadHandler()
{
    SMF_PlatformThreadHandler *pThreadHandler = m_pApi;

    if (m_pThreadHandler != NULL) {
        pThreadHandler = m_pThreadHandler;
    }

    return pThreadHandler;
}

SMF_ErrorCode SMF_PlatformThreadImpl::PlatformThreadProc(SMF_PlatformThread &rPlatformThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_PlatformThread::ThreadId SMF_PlatformThreadImpl::GetCurrentThreadId()
{
    SMF_PlatformThread::ThreadId nThreadId = 0;

    nThreadId = static_cast<SMF_PlatformThread::ThreadId>(::pthread_self());

    return nThreadId;
}

bool SMF_PlatformThreadImpl::IsThreadEqual(SMF_PlatformThread::ThreadId nThreadId1, SMF_PlatformThread::ThreadId nThreadId2)
{
    bool bEqual = false;

    bEqual = ::pthread_equal(static_cast<pthread_t>(nThreadId1), static_cast<pthread_t>(nThreadId2)) != 0 ? true : false;

    return bEqual;
}

SMF_PlatformThread::ThreadId SMF_PlatformThreadImpl::GetThreadId()
{
    return static_cast<SMF_PlatformThread::ThreadId>(m_oThread);
}

SMF_ErrorCode SMF_PlatformThreadImpl::SetConnectedMsgQ(SMF_PlatformMsgQ *pMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pConnectedMsgQ = pMsgQ;
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThreadImpl::SetConnectedTimer(SMF_PlatformTimer *pTimer)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pConnectedTimer = pTimer;
        Unlock();
    }

    return nErrorCode;
}

SMF_PlatformMsgQ *SMF_PlatformThreadImpl::GetConnectedMsgQ()
{
    SMF_PlatformMsgQ *pPlatformMsgQ = NULL;

    Lock();
    pPlatformMsgQ = m_pConnectedMsgQ;
    Unlock();

    return pPlatformMsgQ;
}

SMF_PlatformTimer *SMF_PlatformThreadImpl::GetConnectedTimer()
{
    SMF_PlatformTimer *pPlatformTimer = NULL;

    Lock();
    pPlatformTimer = m_pConnectedTimer;
    Unlock();

    return pPlatformTimer;
}

void SMF_PlatformThreadImpl::Lock()
{
    m_oLock.Lock();
}

void SMF_PlatformThreadImpl::Unlock()
{
    m_oLock.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformThread
//
SMF_PlatformThread::SMF_PlatformThread(const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformThreadImpl(this, pName, NULL);
}

SMF_PlatformThread::~SMF_PlatformThread()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformThread::Start()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->Start();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThread::Stop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->Stop();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThread::SetThreadHandler(SMF_PlatformThreadHandler *pThreadHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetThreadHandler(pThreadHandler);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThread::PlatformThreadProc(SMF_PlatformThread &rPlatformThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformThreadProc(rPlatformThread);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_PlatformThread::ThreadId SMF_PlatformThread::GetCurrentThreadId()
{
    ThreadId nThreadId = 0;

    nThreadId = SMF_PlatformThreadImpl::GetCurrentThreadId();

    return nThreadId;
}

bool SMF_PlatformThread::IsThreadEqual(ThreadId nThreadId1, ThreadId nThreadId2)
{
    bool bEqual = false;

    bEqual = SMF_PlatformThreadImpl::IsThreadEqual(nThreadId1, nThreadId2);

    return bEqual;
}

SMF_PlatformThread::ThreadId SMF_PlatformThread::GetThreadId()
{
    ThreadId nThreadId = 0;

    if (m_pImpl != NULL) {
        nThreadId = m_pImpl->GetThreadId();
    }

    return nThreadId;
}

SMF_ErrorCode SMF_PlatformThread::SetConnectedMsgQ(SMF_PlatformMsgQ *pMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetConnectedMsgQ(pMsgQ);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThread::SetConnectedTimer(SMF_PlatformTimer *pTimer)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetConnectedTimer(pTimer);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_PlatformMsgQ *SMF_PlatformThread::GetConnectedMsgQ()
{
    SMF_PlatformMsgQ *pPlatformMsgQ = NULL;

    if (m_pImpl != NULL) {
        pPlatformMsgQ = m_pImpl->GetConnectedMsgQ();
    }

    return pPlatformMsgQ;
}

SMF_PlatformTimer *SMF_PlatformThread::GetConnectedTimer()
{
    SMF_PlatformTimer *pPlatformTimer = NULL;

    if (m_pImpl != NULL) {
        pPlatformTimer = m_pImpl->GetConnectedTimer();
    }

    return pPlatformTimer;
}
