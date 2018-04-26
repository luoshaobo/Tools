#include "TK_Tools.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformMsgQ.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformThreadImpl
//
class SMF_PlatformThreadImpl : private SMF_NonCopyable, public SMF_PlatformThreadHandler
{
public:
    SMF_PlatformThreadImpl(SMF_PlatformThread *m_pApi, const char *pName = NULL);
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
    static DWORD CALLBACK WinThreadProcStatic(LPVOID pArgs);
    DWORD WINAPI WinThreadProc();

private:
    SMF_PlatformThreadHandler *GetThreadHandler();

private:
    void Lock();
    void Unlock();

private:
    SMF_PlatformThread *m_pApi;
    const char *m_pName;
    std::string m_sName;
    HANDLE m_hThread;
    DWORD m_nThreadId;
    SMF_PlatformThreadHandler *m_pThreadHandler;
    SMF_PlatformLock m_oLock;
    SMF_PlatformMsgQ *m_pConnectedMsgQ;
    SMF_PlatformTimer *m_pConnectedTimer;
};

SMF_PlatformThreadImpl::SMF_PlatformThreadImpl(SMF_PlatformThread *pApi, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
    , m_pApi(pApi)
    , m_hThread(NULL)
    , m_nThreadId(0)
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

DWORD CALLBACK SMF_PlatformThreadImpl::WinThreadProcStatic(LPVOID pArgs)
{
    DWORD nRet = 0;
    SMF_PlatformThreadImpl *pThis = static_cast<SMF_PlatformThreadImpl *>(pArgs);

    if (pThis != NULL) {
        nRet = pThis->WinThreadProc();
    }

    return nRet;
}

DWORD WINAPI SMF_PlatformThreadImpl::WinThreadProc()
{
    DWORD nRet = 0;
    SMF_PlatformThreadHandler *pThreadHandler = GetThreadHandler();
    
    if (pThreadHandler != NULL) {
        nRet = pThreadHandler->PlatformThreadProc(*m_pApi);
    }

    return nRet;
}

SMF_ErrorCode SMF_PlatformThreadImpl::Start()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_hThread = ::CreateThread(NULL, 0, &WinThreadProcStatic, this, 0, &m_nThreadId);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hThread == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformThreadImpl::Stop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_nThreadId != 0) {
            ::PostThreadMessage(m_nThreadId, WM_QUIT, 0, 0);
        }
    }

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

    nThreadId = ::GetCurrentThreadId();

    return nThreadId;
}

bool SMF_PlatformThreadImpl::IsThreadEqual(SMF_PlatformThread::ThreadId nThreadId1, SMF_PlatformThread::ThreadId nThreadId2)
{
    bool bEqual = false;

    bEqual = (nThreadId1 == nThreadId2);

    return bEqual;
}

SMF_PlatformThread::ThreadId SMF_PlatformThreadImpl::GetThreadId()
{
    return m_nThreadId;
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
    m_pImpl = new SMF_PlatformThreadImpl(this, pName);
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
