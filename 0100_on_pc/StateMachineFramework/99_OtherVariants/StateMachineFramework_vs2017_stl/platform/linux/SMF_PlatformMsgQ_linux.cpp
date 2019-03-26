#include "TK_Tools.h"
#include "SMF_PlatformMsgQ.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformTimer.h"
#include "SMF_PlatformThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformMsgQImpl
//

//
// (!!!)NOTE: SendMsg() should have the higher priority than PostMsg().
//
class SMF_PlatformMsgQImpl : private SMF_NonCopyable, public SMF_PlatformMsgQHandler
{
private:
    enum {
        PLATFORM_MSGQ_PRIORITY_LOW = 0,
        PLATFORM_MSGQ_PRIORITY_NORMAL,
        PLATFORM_MSGQ_PRIORITY_HIGH,
    };
    struct MqMsg {
        MqMsg() : oMsg(Msg()), bToSetEvent(false) {}
        MqMsg(Msg a_oMsg, bool a_bToSetEvent = false)
            : oMsg(a_oMsg), bToSetEvent(a_bToSetEvent) {}

        Msg oMsg;
        bool bToSetEvent;
    };

    struct PlatformTimerEventArgs {
        PlatformTimerEventArgs(SMF_PlatformTimerHandler *a_pPlatformTimerHandler, SMF_PlatformTimer &a_rPlatformTimer, SMF_PlatformTimerHandler::TimerId a_nTimerId)
            : pPlatformTimerHandler(a_pPlatformTimerHandler), rPlatformTimer(a_rPlatformTimer), nTimerId(a_nTimerId) {}

        SMF_PlatformTimerHandler *pPlatformTimerHandler;
        SMF_PlatformTimer &rPlatformTimer;
        SMF_PlatformTimerHandler::TimerId nTimerId;
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
    SMF_ErrorCode CreateMq();
    SMF_ErrorCode DestroyMq();

    SMF_ErrorCode OpenMq();
    SMF_ErrorCode CloseMq();

private:
    void Lock();
    void Unlock();

private:
    std::string GetMqName() const;
    std::string GetLockName() const;
    std::string GetSendMsgLockName() const;
    std::string GetSendMsgEventName() const;

private:
    SMF_PlatformMsgQ *m_pApi;
    bool m_bMqValid;
    mqd_t m_nMq;
    std::string m_sName;
    const char *m_pName;
    bool m_bSharedInProcesses;
    bool m_bRemoteAgent;
    SMF_PlatformMsgQHandler *m_pMsgQHandler;
    SMF_PlatformLock m_oLock;
    SMF_PlatformThread *m_pConnectedThread;
    SMF_PlatformLock m_oSendMsgLock;
    SMF_PlatformEvent m_oSendMsgEvent;
};

SMF_PlatformMsgQImpl::SMF_PlatformMsgQImpl(SMF_PlatformMsgQ *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/, bool bRemoteAgent /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_bMqValid(false)
    , m_nMq(-1)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bSharedInProcesses(bSharedInProcesses)
    , m_bRemoteAgent(bRemoteAgent)
    , m_pMsgQHandler(NULL)
    , m_oLock(GetLockName().c_str())
    , m_pConnectedThread(NULL)
    , m_oSendMsgLock(GetSendMsgLockName().c_str(), bSharedInProcesses)
    , m_oSendMsgEvent(false, false, GetSendMsgEventName().c_str(), bSharedInProcesses)
{
    if (!m_bRemoteAgent) {
        CreateMq();
    }
}

SMF_PlatformMsgQImpl::~SMF_PlatformMsgQImpl()
{
    if (!m_bRemoteAgent) {
        DestroyMq();
    } else {
        CloseMq();
    }
}

SMF_ErrorCode SMF_PlatformMsgQImpl::CreateMq()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    struct mq_attr mqa;
    std::string sMqName = GetMqName();
    bool bSuc;
    bool bRetained = false;

    if (nErrorCode == SMF_ERR_OK) {
        if (!TK_Tools::ObjectFileExists(sMqName)) {
            ::mq_unlink(sMqName.c_str());
        }

        bSuc = TK_Tools::RetainObjectFile(sMqName);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_MQ;
        } else {
            bRetained = true;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        memset(&mqa, 0, sizeof(struct mq_attr));
        mqa.mq_flags = 0;
        mqa.mq_maxmsg = 10; // NOTE: Bigger value will cause mq_open() to be failed in some linux distributions.
        mqa.mq_msgsize = sizeof(MqMsg);
        mqa.mq_curmsgs = 0;
        m_nMq = ::mq_open(sMqName.c_str(), O_CREAT | O_RDWR, 0666, &mqa);
        if (m_nMq == (mqd_t)-1) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_MQ;
            m_bMqValid = false;
        } else {
            m_bMqValid = true;
        }
    }

    if (nErrorCode != SMF_ERR_OK) {
        if (bRetained) {
            TK_Tools::ReleaseObjectFile(sMqName);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::DestroyMq()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    bool bSuc;
    std::string sMqName = GetMqName();

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bMqValid) {
            nRet = ::mq_close(m_nMq);
            if (nRet != 0) {
                nErrorCode = SMF_ERR_FAILED_TO_DESTROY_MQ;
            }
            m_bMqValid = false;
            m_nMq = (mqd_t)-1;

            bSuc = TK_Tools::ReleaseObjectFile(sMqName);
            if (!bSuc) {
                nErrorCode = SMF_ERR_FAILED_TO_DESTROY_MQ;
            }
            if (!TK_Tools::ObjectFileExists(sMqName)) {
                ::mq_unlink(sMqName.c_str());
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::OpenMq()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    struct mq_attr mqa;
    std::string sMqName = GetMqName();

    if (nErrorCode == SMF_ERR_OK) {
        if (!TK_Tools::ObjectFileExists(sMqName)) {
            nErrorCode = SMF_ERR_FAILED_TO_OPEN_MQ;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        memset(&mqa, 0, sizeof(struct mq_attr));
        mqa.mq_flags = O_NONBLOCK;
        mqa.mq_maxmsg = 1000;
        mqa.mq_msgsize = sizeof(MqMsg);
        mqa.mq_curmsgs = 0;
        m_nMq = ::mq_open(sMqName.c_str(), O_RDWR, 0666, &mqa);
        if (m_nMq == (mqd_t)-1) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_MQ;
            m_bMqValid = false;
        } else {
            m_bMqValid = true;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::CloseMq()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    std::string sMqName = GetMqName();

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bMqValid) {
            nRet = ::mq_close(m_nMq);
            if (nRet != 0) {
                nErrorCode = SMF_ERR_FAILED_TO_CLOSE_MQ;
            }
            m_bMqValid = false;
            m_nMq = (mqd_t)-1;
        }
    }

    return nErrorCode;
}

//
// NOTE:
// 1) This function should be thread-safe in single process when m_bRemoteAgent != true.
// 2) This function should be thread-safe among processes when m_bRemoteAgent == true.
//
SMF_ErrorCode SMF_PlatformMsgQImpl::PostMsg(const SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    Lock();

    if (m_bRemoteAgent) {
        nErrorCode = OpenMq();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (!m_bMqValid) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        MqMsg mqmsg(rMsg);
        nRet = ::mq_send(m_nMq, reinterpret_cast<const char *>(&mqmsg), sizeof(MqMsg), PLATFORM_MSGQ_PRIORITY_NORMAL);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_PLATFORM_API_CALL_FAILED;
        }
    }

    if (m_bRemoteAgent) {
        CloseMq();
    }

    Unlock();

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
    int nRet;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (m_bRemoteAgent) {
        Lock();
        nErrorCode = OpenMq();
        Unlock();
    }

    m_oSendMsgLock.Lock();

    if (nErrorCode == SMF_ERR_OK) {
        if (!m_bMqValid) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        MqMsg mqmsg(rMsg, true);
        nRet = ::mq_send(m_nMq, reinterpret_cast<const char *>(&mqmsg), sizeof(MqMsg), PLATFORM_MSGQ_PRIORITY_HIGH);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_PLATFORM_API_CALL_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, &m_oSendMsgEvent);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nWaitResult != SMF_PlatformEvent::WAIT_RESULT_OBJECT_0) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (m_bRemoteAgent) {
        CloseMq();
    }

    m_oSendMsgLock.Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::EnterMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQHandler *pMsgQHandler;
    ssize_t nRet;
    MqMsg mqmsg;
    unsigned nMsgPrio;
    struct timespec ts;

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            pMsgQHandler = GetMsgQHandler();

            if (pMsgQHandler != NULL) {
                nErrorCode = pMsgQHandler->PlatformMsgQPreGetMsg(*m_pApi);
            }

            if (!m_bMqValid) {
                nErrorCode = SMF_ERR_INVALID_HANDLE;
                break;
            }

            TK_Tools::GetAbsoluteTimeSpec(SMF_THREAD_AWAKEN_INTERVAL_MILLISECONDS, ts);
            do {
                nRet = ::mq_timedreceive(m_nMq, reinterpret_cast<char *>(&mqmsg), sizeof(MqMsg), &nMsgPrio, &ts);
                if (nRet == -1 && errno == EINTR) {
                    continue;
                }
            } while (false);
            if (nRet == -1 && errno == ETIMEDOUT) {
                continue;
            }
            if (nRet != sizeof(MqMsg)) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            }

            if (mqmsg.oMsg.bSystemMsg && mqmsg.oMsg.nMsgId == PLATFORM_MSG_ID_TIMER) {
                PlatformTimerEventArgs *pPlatformTimerEventArgs = reinterpret_cast<PlatformTimerEventArgs *>(mqmsg.oMsg.lParam);
                if (pPlatformTimerEventArgs != NULL && pPlatformTimerEventArgs->pPlatformTimerHandler != NULL) {
                    nErrorCode = pPlatformTimerEventArgs->pPlatformTimerHandler->PlatformTimerProc(pPlatformTimerEventArgs->rPlatformTimer, pPlatformTimerEventArgs->nTimerId);
                    if (mqmsg.bToSetEvent) {
                        m_oSendMsgEvent.SetEvent();
                    }
                }
            } else {
                nErrorCode = pMsgQHandler->PlatformMsgQProc(*m_pApi, mqmsg.oMsg);
                if (mqmsg.bToSetEvent) {
                    m_oSendMsgEvent.SetEvent();
                }

                if (mqmsg.oMsg.bSystemMsg && mqmsg.oMsg.nMsgId == PLATFORM_MSG_ID_QUIT) {
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
        if (!m_bMqValid) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PostQuitMsg();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PostQuitMsg()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (!m_bMqValid) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        MqMsg mqmsg(Msg(PLATFORM_MSG_ID_QUIT, 0, 0, true));
        nRet = ::mq_send(m_nMq, reinterpret_cast<const char *>(&mqmsg), sizeof(MqMsg), 0);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_PLATFORM_API_CALL_FAILED;
        }
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

std::string SMF_PlatformMsgQImpl::GetMqName() const
{
    std::string sResult;

    //
    // NOTE: The name of posix mq must begin with slash(/) in linux.
    //
    sResult = TK_Tools::FormatStr("/%s_SMF_PlatformMsgQImpl_mq_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());

    return sResult;
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
