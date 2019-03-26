#include <queue>
#include <mutex>
#include <condition_variable>
#include "TK_Tools.h"
#include "SMF_PlatformMsgQ.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformTimer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformMsgQImpl
//
class SMF_PlatformMsgQImpl : private SMF_NonCopyable, public SMF_PlatformMsgQHandler
{
private:
    struct PlatformTimerEventArgs {
        PlatformTimerEventArgs(SMF_PlatformTimerHandler *a_pPlatformTimerHandler, SMF_PlatformTimer &a_rPlatformTimer, SMF_PlatformTimerHandler::TimerId a_nTimerId)
            : pPlatformTimerHandler(a_pPlatformTimerHandler), rPlatformTimer(a_rPlatformTimer), nTimerId(a_nTimerId) {}

        SMF_PlatformTimerHandler *pPlatformTimerHandler;
        SMF_PlatformTimer &rPlatformTimer;
        SMF_PlatformTimerHandler::TimerId nTimerId;
    };

    struct MsgWrapper {
        MsgWrapper() : oMsg(), bToSetEvent() {}
        MsgWrapper(const SMF_PlatformMsgQ::Msg &a_oMsg, bool a_bToSetEvent = false)
            : oMsg(a_oMsg), bToSetEvent(a_bToSetEvent) {}

        MsgWrapper(const MsgWrapper &another) 
            : oMsg(another.oMsg), bToSetEvent(another.bToSetEvent) {}

        SMF_PlatformMsgQ::Msg oMsg;
        bool bToSetEvent;
    };

public:
    SMF_PlatformMsgQImpl(SMF_PlatformMsgQ *m_pApi, const char *pName = NULL, bool bSharedInProcesses = false, bool bRemoteAgent = false);
    virtual ~SMF_PlatformMsgQImpl();

public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode PostMsg(const SMF_PlatformMsgQ::Msg &rMsg);
    SMF_ErrorCode SendMsg(const SMF_PlatformMsgQ::Msg &rMsg);

    SMF_ErrorCode EnterMsgLoop();
    SMF_ErrorCode ExitMsgLoop();

    SMF_ErrorCode PostQuitMsg();

public:
    SMF_ErrorCode SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler);

public:
    SMF_PlatformThread *GetConnectedThread();

public:
    virtual SMF_ErrorCode PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, Msg &rMsg);

private:
    SMF_PlatformMsgQHandler *GetMsgQHandler();

private:
    void Lock();
    void Unlock();

private:
    std::string GetLockName() const;
    std::string GetSendMsgLockName() const;
    std::string GetSendMsgEventName() const;

private:
    SMF_PlatformMsgQ *m_pApi;
    std::string m_sName;
    SMF_PlatformMsgQHandler *m_pMsgQHandler;
    SMF_PlatformLock m_oLock;
    SMF_PlatformThread *m_pConnectedThread;
    std::queue<MsgWrapper> m_msgQueue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    SMF_PlatformLock m_oSendMsgLock;
    SMF_PlatformEvent m_oSendMsgEvent;
};

SMF_PlatformMsgQImpl::SMF_PlatformMsgQImpl(SMF_PlatformMsgQ *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/, bool bRemoteAgent /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_sName(pName != NULL ? pName : "")
    , m_pMsgQHandler(NULL)
    , m_oLock(GetLockName().c_str())
    , m_pConnectedThread(NULL)
    , m_msgQueue()
    , m_mutex()
    , m_cv()
    , m_oSendMsgLock(GetSendMsgLockName().c_str(), bSharedInProcesses)
    , m_oSendMsgEvent(false, false, GetSendMsgEventName().c_str())
{

}

SMF_PlatformMsgQImpl::~SMF_PlatformMsgQImpl()
{

}

//
// NOTE:
// 1) This function should be thread-safe in single process when m_bRemoteAgent != true.
// 2) This function should be thread-safe among processes when m_bRemoteAgent == true.
//
SMF_ErrorCode SMF_PlatformMsgQImpl::PostMsg(const SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_msgQueue.push(MsgWrapper(rMsg, false));
        m_cv.notify_all();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PostQuitMsg()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(PLATFORM_MSG_ID_QUIT, 0, 0, true);
        nErrorCode = PostMsg(msg);
    }

    return nErrorCode;
}

//
// NOTE:
// 1) This function should be thread-safe in single process when m_bRemoteAgent != true.
// 2) This function should be thread-safe among processes when m_bRemoteAgent == true.
//
SMF_ErrorCode SMF_PlatformMsgQImpl::SendMsg(const SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformEvent::WaitResult nWaitResult;

    m_oSendMsgLock.Lock();

    if (nErrorCode == SMF_ERR_OK) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_msgQueue.push(MsgWrapper(rMsg, true));
        m_cv.notify_all();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, &m_oSendMsgEvent);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nWaitResult != SMF_PlatformEvent::WAIT_RESULT_OBJECT_0) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    m_oSendMsgLock.Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::EnterMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bQuitMsgLoop = false;
    SMF_PlatformMsgQHandler *pMsgQHandler;
    
    for (;;) {
        if (bQuitMsgLoop) {
            break;
        }

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock);
        }

        for (;;) {
            bool empty = true;
            MsgWrapper msgWrapper;
            SMF_PlatformMsgQ::Msg msg;

            {                                               // NOTE the lock is just used to protect m_msgQueue!
                std::unique_lock<std::mutex> lock(m_mutex);
                empty = m_msgQueue.empty();
                if (!empty) {
                    msgWrapper = m_msgQueue.front();
                    msg = msgWrapper.oMsg;
                    m_msgQueue.pop();
                }
            }

            if (empty) {
                break;
            }

            pMsgQHandler = GetMsgQHandler();

            if (pMsgQHandler != NULL) {
                nErrorCode = pMsgQHandler->PlatformMsgQPreGetMsg(*m_pApi);
            }

            if (msg.bSystemMsg && msg.nMsgId == PLATFORM_MSG_ID_TIMER) {
                PlatformTimerEventArgs *pPlatformTimerEventArgs = reinterpret_cast<PlatformTimerEventArgs *>(msg.lParam);
                if (pPlatformTimerEventArgs != NULL && pPlatformTimerEventArgs->pPlatformTimerHandler != NULL) {
                    nErrorCode = pPlatformTimerEventArgs->pPlatformTimerHandler->PlatformTimerProc(pPlatformTimerEventArgs->rPlatformTimer, pPlatformTimerEventArgs->nTimerId);
                    if (msgWrapper.bToSetEvent) {
                        m_oSendMsgEvent.SetEvent();
                    }
                }
            }
            else {
                if (pMsgQHandler != NULL) {
                    nErrorCode = pMsgQHandler->PlatformMsgQProc(*m_pApi, msg);
                }
                if (msgWrapper.bToSetEvent) {
                    m_oSendMsgEvent.SetEvent();
                }

                if (msg.bSystemMsg && msg.nMsgId == PLATFORM_MSG_ID_QUIT) {
                    bQuitMsgLoop = true;
                    break;
                }
            }

            if (pMsgQHandler != NULL) {
                nErrorCode = pMsgQHandler->PlatformMsgQPostGetMsg(*m_pApi);
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::ExitMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PostQuitMsg();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_pMsgQHandler = pMsgQHandler;
    }

    return nErrorCode;
}

SMF_PlatformMsgQHandler *SMF_PlatformMsgQImpl::GetMsgQHandler()
{
    SMF_PlatformMsgQHandler *pMsgQHandler = m_pApi;

    if (m_pMsgQHandler != NULL) {
        pMsgQHandler = m_pMsgQHandler;
    }

    return pMsgQHandler;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedMsgQ(NULL);
            Lock();
            m_pConnectedThread = NULL;
            Unlock();
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pConnectedThread = pThread;
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedMsgQ(this->m_pApi);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformMsgQImpl::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    Lock();
    pPlatformThread = m_pConnectedThread;
    Unlock();

    return pPlatformThread;
}

void SMF_PlatformMsgQImpl::Lock()
{
    m_oLock.Lock();
}

void SMF_PlatformMsgQImpl::Unlock()
{
    m_oLock.Unlock();
}

std::string SMF_PlatformMsgQImpl::GetLockName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformMsgQImpl_Lock_%s", m_sName.c_str());

    return sResult;
}

std::string SMF_PlatformMsgQImpl::GetSendMsgLockName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformMsgQImpl_SendMsgLock_%s", m_sName.c_str());

    return sResult;
}

std::string SMF_PlatformMsgQImpl::GetSendMsgEventName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformMsgQImpl_SendMsgEvent_%s", m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformMsgQ
//
SMF_PlatformMsgQ::SMF_PlatformMsgQ(const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/, bool bRemoteAgent /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformMsgQImpl(this, pName, bSharedInProcesses, bRemoteAgent);
}

SMF_PlatformMsgQ::~SMF_PlatformMsgQ()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformMsgQ::PostMsg(const Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PostMsg(rMsg);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::SendMsg(const Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SendMsg(rMsg);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::EnterMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->EnterMsgLoop();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::ExitMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ExitMsgLoop();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformMsgQPreGetMsg(rPlatformMsgQ);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformMsgQPostGetMsg(rPlatformMsgQ);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformMsgQProc(rPlatformMsgQ, rMsg);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetMsgQHandler(pMsgQHandler);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PostQuitMsg()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PostQuitMsg();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ConnectToThread(pThread);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformMsgQ::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    if (m_pImpl != NULL) {
        pPlatformThread = m_pImpl->GetConnectedThread();
    }

    return pPlatformThread;
}
