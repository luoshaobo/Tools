#include "TK_Tools.h"
#include "SMF_BaseThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseThread
//
SMF_BaseThread::SMF_BaseThread(const std::string &sThreadName, bool bSupportInterProcessEvent /*= false*/, bool bRemoteAgent /*= false*/)
    : SMF_NonCopyable()
    , m_oThreadDataMap()
    , m_nThreadState(TS_INIT)
    , m_oThreadStateEvent(FALSE, FALSE)
    , m_sThreadName(sThreadName)
    , m_bSupportInterProcessEvent(bRemoteAgent ? true : bSupportInterProcessEvent)
    , m_bRemoteAgent(bRemoteAgent)
    , m_pPlatformMsgQ(NULL)
    , m_pPlatformTimer(NULL)
    , m_oPlatformThread(sThreadName.c_str())
    , m_oLock()
    , m_pRemoteSharedPostMsgBufShm(NULL)
    , m_pRemoteSharedPostMsgBufAddr(NULL)
    , m_pRemoteSharedPostMsgBufCyclicBuf(NULL)
    , m_pRemoteSharedPostMsgBufLock(NULL)
    , m_pRemoteSharedSendMsgBufShm(NULL)
    , m_pRemoteSharedSendMsgBufAddr(NULL)
    , m_pRemoteSharedSendMsgBufLock(NULL)
    , m_pRemotePostMsgWaitEvent(NULL)
    , m_pThreadHandler(NULL)
    , m_oTimerHandlerMap()
    , m_oEnabledTimerSet()
    , m_oCallHandlerMap()
    , m_oEventQueueHandlerMap()
    , m_oTimerAttribMap()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_bSupportInterProcessEvent) {
        m_pRemoteSharedPostMsgBufShm = new SMF_PlatformSharedMem(REMOTE_SHARED_POST_MSG_BUF_SIZE, GetRemoteSharedPostMsgBufShmName().c_str(), true);
        if (m_pRemoteSharedPostMsgBufShm != NULL) {
            nErrorCode = m_pRemoteSharedPostMsgBufShm->Map();
            if (nErrorCode != SMF_ERR_OK) {
                delete m_pRemoteSharedPostMsgBufShm;
                m_pRemoteSharedPostMsgBufShm = NULL;
            } else {
                m_pRemoteSharedPostMsgBufAddr = static_cast<unsigned char *>(m_pRemoteSharedPostMsgBufShm->GetSharedMemAddr());
                if (m_pRemoteSharedPostMsgBufAddr == NULL) {
                    delete m_pRemoteSharedPostMsgBufShm;
                    m_pRemoteSharedPostMsgBufShm = NULL;
                }
            }
        }
        if (m_pRemoteSharedPostMsgBufShm != NULL) {
            m_pRemoteSharedPostMsgBufCyclicBuf = new SMF_CyclicBuf(m_pRemoteSharedPostMsgBufAddr, REMOTE_SHARED_POST_MSG_BUF_SIZE, GetRemoteSharedPostMsgBufCyclicBufName().c_str(), true, !m_pRemoteSharedPostMsgBufShm->AlreadyExists());
        }
        m_pRemoteSharedPostMsgBufLock = new SMF_PlatformLock(GetRemoteSharedPostMsgBufLockName().c_str(), true);

        m_pRemoteSharedSendMsgBufShm = new SMF_PlatformSharedMem(REMOTE_SHARED_SEND_MSG_BUF_SIZE, GetRemoteSharedSendMsgBufShmName().c_str(), true);
        if (m_pRemoteSharedSendMsgBufShm != NULL) {
            nErrorCode = m_pRemoteSharedSendMsgBufShm->Map();
            if (nErrorCode != SMF_ERR_OK) {
                delete m_pRemoteSharedSendMsgBufShm;
                m_pRemoteSharedSendMsgBufShm = NULL;
            } else {
                m_pRemoteSharedSendMsgBufAddr = static_cast<unsigned char *>(m_pRemoteSharedSendMsgBufShm->GetSharedMemAddr());
                if (m_pRemoteSharedSendMsgBufAddr == NULL) {
                    delete m_pRemoteSharedSendMsgBufShm;
                    m_pRemoteSharedSendMsgBufShm = NULL;
                }
            }
        }
        m_pRemoteSharedSendMsgBufLock = new SMF_PlatformLock(GetRemoteSharedSendMsgBufLockName().c_str(), true);
        m_pRemotePostMsgWaitEvent = new SMF_PlatformEvent(false, false, GetRemotePostMsgWaitEventName().c_str(), true);
    }

    if (bRemoteAgent) {
        m_pPlatformMsgQ = new SMF_PlatformMsgQ(GetThreadMsgQName().c_str(), m_bSupportInterProcessEvent, true);
    }
}

SMF_BaseThread::~SMF_BaseThread()
{
    if (!m_bRemoteAgent) {
        Stop();
    }

    if (m_bSupportInterProcessEvent) {
        if (m_pPlatformMsgQ != NULL) {
            delete m_pPlatformMsgQ;
            m_pPlatformMsgQ = NULL;
        }

        if (m_pRemotePostMsgWaitEvent != NULL) {
            delete m_pRemotePostMsgWaitEvent;
            m_pRemotePostMsgWaitEvent = NULL;
        }

        if (m_pRemoteSharedSendMsgBufLock != NULL) {
            delete m_pRemoteSharedSendMsgBufLock;
            m_pRemoteSharedSendMsgBufLock = NULL;
        }

        if (m_pRemoteSharedSendMsgBufShm != NULL) {
            delete m_pRemoteSharedSendMsgBufShm;
            m_pRemoteSharedSendMsgBufShm = NULL;
        }
        m_pRemoteSharedSendMsgBufAddr = NULL;

        if (m_pRemoteSharedPostMsgBufLock != NULL) {
            delete m_pRemoteSharedPostMsgBufLock;
            m_pRemoteSharedPostMsgBufLock = NULL;
        }

        if (m_pRemoteSharedPostMsgBufCyclicBuf != NULL) {
            delete m_pRemoteSharedPostMsgBufCyclicBuf;
            m_pRemoteSharedPostMsgBufCyclicBuf = NULL;
        }

        if (m_pRemoteSharedPostMsgBufShm != NULL) {
            delete m_pRemoteSharedPostMsgBufShm;
            m_pRemoteSharedPostMsgBufShm = NULL;
        }
        m_pRemoteSharedPostMsgBufAddr = NULL;
    }
}

SMF_ErrorCode SMF_BaseThread::Initialize()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = OnInitialize();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::Finialize()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = OnFinialize();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::Start()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = DoStart();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::DoStart()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_INIT && GetThreadState() != TS_DESTRORYED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_ALREADY_STARTED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetThreadState(TS_CREATING);
        nErrorCode = m_oPlatformThread.SetThreadHandler(this);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oPlatformThread.Start();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED && GetThreadState() != TS_DESTRORYED) {
            WaitForThreadState(TS_ACTIVATED | TS_DESTRORYED);
        }
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_THREAD;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::Stop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = DoStop();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::DoStop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetThreadState(TS_DESTRORYING);
        nErrorCode = m_oPlatformThread.Stop();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_DESTRORYED) {
            WaitForThreadState(TS_DESTRORYED);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PostEvent(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs /*= NULL*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckThreadEventArgs(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bRemoteAgent) {
            nErrorCode = PostEvent_Remote(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
        } else {
            nErrorCode = PostEvent_Local(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PostEvent_Remote(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;
    ThreadEventArgs *pThreadEventArgsBuf = NULL;
    unsigned int nThreadEventArgsBufSize = 0;
    unsigned char *pInBuf = NULL;
    unsigned int nInBufSize = 0;
    unsigned int nTotalBufSize = 0;
    bool bPutSuc = false;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckSharedEnvironment();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    m_pRemoteSharedPostMsgBufLock->Lock();

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadEventArgs != NULL) {
            if ((pThreadEventArgs->pInBuf != NULL && pThreadEventArgs->nInBufSize > 0) || (pThreadEventArgs->pOutBuf != NULL && pThreadEventArgs->nOutBufSize > 0)) {
                nTotalBufSize = SMF_ALIGNED(sizeof(ThreadEventArgs));
                if (pThreadEventArgs->pInBuf != NULL && pThreadEventArgs->nInBufSize > 0) {
                    nTotalBufSize += SMF_ALIGNED(pThreadEventArgs->nInBufSize);
                }

                pThreadEventArgsBuf = reinterpret_cast<ThreadEventArgs *>(new unsigned char [nTotalBufSize]);
                if (pThreadEventArgsBuf != NULL) {
                    nThreadEventArgsBufSize = SMF_ALIGNED(sizeof(ThreadEventArgs));
                    *pThreadEventArgsBuf = *pThreadEventArgs;
                    if (pThreadEventArgsBuf->pInBuf != NULL && pThreadEventArgsBuf->nInBufSize > 0) {
                        pInBuf = reinterpret_cast<unsigned char *>(pThreadEventArgsBuf) + nThreadEventArgsBufSize;
                        nInBufSize = SMF_ALIGNED(pThreadEventArgs->nInBufSize);
                        memcpy(pInBuf, pThreadEventArgsBuf->pInBuf, pThreadEventArgsBuf->nInBufSize);
                        pThreadEventArgsBuf->pInBuf = reinterpret_cast<unsigned char *>(nThreadEventArgsBufSize);
                    } else {
                        pThreadEventArgsBuf->pInBuf = NULL;
                        pThreadEventArgsBuf->nInBufSize = 0;
                    }
                    if (pThreadEventArgsBuf->pOutBuf == NULL || pThreadEventArgsBuf->nOutBufSize == 0) {
                        pThreadEventArgsBuf->pOutBuf = NULL;
                        pThreadEventArgsBuf->nOutBufSize = 0;
                    }
                    nErrorCode = m_pRemoteSharedPostMsgBufCyclicBuf->PutData(reinterpret_cast<unsigned char *>(pThreadEventArgsBuf), nTotalBufSize);
                    if (nErrorCode == SMF_ERR_OK) {
                        bPutSuc = true;
                    }

                    delete [] (reinterpret_cast<unsigned char *>(pThreadEventArgsBuf));

                    if (nErrorCode == SMF_ERR_OK) {
                        wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(1, 0, 1, nQueueId, nEngineId, nSenderId, nTotalBufSize));
                        lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadEventArgsBuf);
                    }
                } else {
                    nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
                }
            } else {
                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(1, 0, 0, nQueueId, nEngineId, nSenderId, 0));
                lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
            }
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(1, 0, 0, nQueueId, nEngineId, nSenderId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE + nEventId, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->PostMsg(msg);
    }

    if (nErrorCode != SMF_ERR_OK) {
        if (bPutSuc) {
            m_pRemoteSharedPostMsgBufCyclicBuf->UnPutData(nTotalBufSize);
        }
    }

    m_pRemoteSharedPostMsgBufLock->Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PostEvent_Local(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;
    unsigned int nThreadEventArgsBuf;
    ThreadEventArgs *pThreadEventArgsBuf;
    unsigned char *pInBuf;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadEventArgs != NULL && pThreadEventArgs->pInBuf != NULL && pThreadEventArgs->nInBufSize > 0) {
            nThreadEventArgsBuf = SMF_ALIGNED(sizeof(ThreadEventArgs)) + SMF_ALIGNED(pThreadEventArgs->nInBufSize);
            pThreadEventArgsBuf = reinterpret_cast<ThreadEventArgs *>(new SMF_MACHINE_WORD_TYPE[nThreadEventArgsBuf / sizeof(SMF_MACHINE_WORD_TYPE)]);
            pInBuf = reinterpret_cast<unsigned char *>(&pThreadEventArgsBuf[1]) + SMF_ALIGNED(sizeof(ThreadEventArgs));
            if (pThreadEventArgsBuf != 0) {
                ::memcpy(reinterpret_cast<void *>(pInBuf), pThreadEventArgs->pInBuf, pThreadEventArgs->nInBufSize);

                *pThreadEventArgsBuf = *pThreadEventArgs;
                pThreadEventArgsBuf->pInBuf = pInBuf;

                if (pThreadEventArgsBuf->pOutBuf == NULL || pThreadEventArgsBuf->nOutBufSize == 0) {
                    pThreadEventArgsBuf->pOutBuf = NULL;
                    pThreadEventArgsBuf->nOutBufSize = 0;
                }

                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(0, 0, 1, nQueueId, nEngineId, nSenderId, nThreadEventArgsBuf));
                lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadEventArgsBuf);
            } else {
                nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
            }
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(0, 0, 0, nQueueId, nEngineId, nSenderId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE + nEventId, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->PostMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs /*= NULL*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckThreadEventArgs(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bRemoteAgent) {
            nErrorCode = SendEvent_Remote(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
        } else {
            nErrorCode = SendEvent_Local(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent_Remote(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;
    ThreadEventArgs *pThreadEventArgsBuf = NULL;
    unsigned int nThreadEventArgsBufSize = 0;
    unsigned char *pInBuf = NULL;
    unsigned int nInBufSize = 0;
    unsigned char *pOutBuf = NULL;
    unsigned int nOutBufSize = 0;
    unsigned int nTotalBufSize = 0;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckSharedEnvironment();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    m_pRemoteSharedSendMsgBufLock->Lock();

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadEventArgs != NULL) {
            if ((pThreadEventArgs->pInBuf != NULL && pThreadEventArgs->nInBufSize > 0) || (pThreadEventArgs->pOutBuf != NULL && pThreadEventArgs->nOutBufSize > 0)) {
                pThreadEventArgsBuf = reinterpret_cast<ThreadEventArgs *>(m_pRemoteSharedSendMsgBufAddr);
                nThreadEventArgsBufSize = SMF_ALIGNED(sizeof(ThreadEventArgs));
                *pThreadEventArgsBuf = *pThreadEventArgs;
                if (pThreadEventArgsBuf->pInBuf != NULL && pThreadEventArgsBuf->nInBufSize > 0) {
                    pInBuf = reinterpret_cast<unsigned char *>(pThreadEventArgsBuf) + nThreadEventArgsBufSize;
                    nInBufSize = SMF_ALIGNED(pThreadEventArgs->nInBufSize);
                    memcpy(pInBuf, pThreadEventArgsBuf->pInBuf, pThreadEventArgsBuf->nInBufSize);
                }
                if (pThreadEventArgsBuf->pOutBuf != NULL && pThreadEventArgsBuf->nOutBufSize > 0) {
                    pOutBuf = reinterpret_cast<unsigned char *>(pThreadEventArgsBuf) + nThreadEventArgsBufSize + nInBufSize;
                    nOutBufSize = SMF_ALIGNED(pThreadEventArgsBuf->nOutBufSize);
                }

                //
                // Fix addr before sending to another process
                //
                if (pThreadEventArgsBuf->pInBuf != NULL && pThreadEventArgsBuf->nInBufSize > 0) {
                    pThreadEventArgsBuf->pInBuf = reinterpret_cast<unsigned char *>(nThreadEventArgsBufSize);
                } else {
                    pThreadEventArgsBuf->pInBuf = NULL;
                    pThreadEventArgsBuf->nInBufSize = 0;
                }
                if (pThreadEventArgsBuf->pOutBuf != NULL && pThreadEventArgsBuf->nOutBufSize > 0) {
                    pThreadEventArgsBuf->pOutBuf = reinterpret_cast<unsigned char *>(nThreadEventArgsBufSize + nInBufSize);
                } else {
                    pThreadEventArgsBuf->pOutBuf = NULL;
                    pThreadEventArgsBuf->nOutBufSize = 0;
                }

                nTotalBufSize = SMF_ALIGNED(sizeof(ThreadEventArgs)) + nInBufSize  + nOutBufSize;
                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(1, 0, 0, nQueueId, nEngineId, nSenderId, nTotalBufSize));
                lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadEventArgsBuf);
            } else {
                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(1, 0, 0, nQueueId, nEngineId, nSenderId, 0));
                lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
            }
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(1, 0, 0, nQueueId, nEngineId, nSenderId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE + nEventId, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->SendMsg(msg);
    }

    if (nErrorCode == SMF_ERR_OK) {
        //
        // Fix addr after the process being proecessed
        //
        if (pThreadEventArgsBuf->pInBuf != NULL) {
            pThreadEventArgsBuf->pInBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadEventArgsBuf) + nThreadEventArgsBufSize);
        }
        if (pThreadEventArgsBuf->pOutBuf != NULL) {
            pThreadEventArgsBuf->pOutBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadEventArgsBuf)+ nThreadEventArgsBufSize + nInBufSize);
        }

        if (pThreadEventArgs != NULL && pThreadEventArgsBuf != NULL) {
            pThreadEventArgs->nOutBufRetSize = pThreadEventArgsBuf->nOutBufRetSize < pThreadEventArgs->nOutBufSize ? pThreadEventArgsBuf->nOutBufRetSize : pThreadEventArgs->nOutBufSize;
            pThreadEventArgs->nRet = pThreadEventArgsBuf->nRet;
            if (pThreadEventArgs->pOutBuf != NULL && pThreadEventArgsBuf->pOutBuf != NULL) {
                memcpy(pThreadEventArgs->pOutBuf, pThreadEventArgsBuf->pOutBuf, pThreadEventArgs->nOutBufRetSize);
            }
        }
    }

    m_pRemoteSharedSendMsgBufLock->Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent_Local(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformThread::ThreadId nCurrentThreadId = SMF_PlatformThread::GetCurrentThreadId();

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (SMF_PlatformThread::IsThreadEqual(nCurrentThreadId, m_oPlatformThread.GetThreadId())) {
            nErrorCode = SendEvent_Local_InSameThread(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
        } else {
            nErrorCode = SendEvent_Local_InOtherThread(nQueueId, nEngineId, nSenderId, nEventId, pThreadEventArgs);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent_Local_InSameThread(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = SMF_ERR_NOT_PROCESSED;
        SMF_ThreadEventQueueHandler *pThreadEventQueueHandler = GetEventQueueHandler(nQueueId);
        if (pThreadEventQueueHandler != NULL) {
            EventData oEventData(
                nEngineId,
                nSenderId,
                nEventId,
                pThreadEventArgs
            );
            nErrorCode = pThreadEventQueueHandler->ThreadEventQueueProc(*this, nQueueId, oEventData);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent_Local_InOtherThread(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadEventArgs != NULL) {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(0, 0, 0, nQueueId, nEngineId, nSenderId, sizeof(ThreadEventArgs)));
            lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadEventArgs);
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(0, 0, 0, nQueueId, nEngineId, nSenderId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE + nEventId, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->SendMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::AsyncCall(CallId nCallId, ThreadCallArgs *pThreadCallArgs /*= NULL*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckThreadCallArgs(nCallId, pThreadCallArgs);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bRemoteAgent) {
            nErrorCode = AsyncCall_Remote(nCallId, pThreadCallArgs);
        } else {
            nErrorCode = AsyncCall_Local(nCallId, pThreadCallArgs);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::AsyncCall_Remote(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;
    ThreadCallArgs *pThreadCallArgsBuf = NULL;
    unsigned int nThreadCallArgsBufSize = 0;
    unsigned char *pInBuf = NULL;
    unsigned int nInBufSize = 0;
    unsigned int nTotalBufSize = 0;
    bool bPutSuc = false;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckSharedEnvironment();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    m_pRemoteSharedPostMsgBufLock->Lock();

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadCallArgs != NULL) {
            if ((pThreadCallArgs->pInBuf != NULL && pThreadCallArgs->nInBufSize > 0) || (pThreadCallArgs->pOutBuf != NULL && pThreadCallArgs->nOutBufSize > 0)) {
                nTotalBufSize = SMF_ALIGNED(sizeof(ThreadCallArgs));
                if (pThreadCallArgs->pInBuf != NULL && pThreadCallArgs->nInBufSize > 0) {
                    nTotalBufSize += SMF_ALIGNED(pThreadCallArgs->nInBufSize);
                }

                pThreadCallArgsBuf = reinterpret_cast<ThreadCallArgs *>(new unsigned char [nTotalBufSize]);
                if (pThreadCallArgsBuf != NULL) {
                    nThreadCallArgsBufSize = SMF_ALIGNED(sizeof(ThreadCallArgs));
                    *pThreadCallArgsBuf = *pThreadCallArgs;
                    if (pThreadCallArgsBuf->pInBuf != NULL && pThreadCallArgsBuf->nInBufSize > 0) {
                        pInBuf = reinterpret_cast<unsigned char *>(pThreadCallArgsBuf) + nThreadCallArgsBufSize;
                        nInBufSize = SMF_ALIGNED(pThreadCallArgs->nInBufSize);
                        memcpy(pInBuf, pThreadCallArgsBuf->pInBuf, pThreadCallArgsBuf->nInBufSize);
                        pThreadCallArgsBuf->pInBuf = reinterpret_cast<unsigned char *>(nThreadCallArgsBufSize);
                    } else {
                        pThreadCallArgsBuf->pInBuf = NULL;
                        pThreadCallArgsBuf->nInBufSize = 0;
                    }
                    if (pThreadCallArgsBuf->pOutBuf == NULL || pThreadCallArgsBuf->nOutBufSize == 0) {
                        pThreadCallArgsBuf->pOutBuf = NULL;
                        pThreadCallArgsBuf->nOutBufSize = 0;
                    }
                    nErrorCode = m_pRemoteSharedPostMsgBufCyclicBuf->PutData(reinterpret_cast<unsigned char *>(pThreadCallArgsBuf), nTotalBufSize);
                    if (nErrorCode == SMF_ERR_OK) {
                        bPutSuc = true;
                    }

                    delete [] (reinterpret_cast<unsigned char *>(pThreadCallArgsBuf));

                    if (nErrorCode == SMF_ERR_OK) {
                        wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, 0, 1, nCallId, nTotalBufSize));
                        lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadCallArgsBuf);
                    }
                } else {
                    nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
                }
            } else {
                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, 0, 0, nCallId, 0));
                lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
            }
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, 0, 0, nCallId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_ASYNC_CALL, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->PostMsg(msg);
    }

    if (nErrorCode != SMF_ERR_OK) {
        if (bPutSuc) {
            m_pRemoteSharedPostMsgBufCyclicBuf->UnPutData(nTotalBufSize);
        }
    }

    m_pRemoteSharedPostMsgBufLock->Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::AsyncCall_Local(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;
    unsigned int nThreadCallArgsBuf;
    ThreadCallArgs *pThreadCallArgsBuf;
    unsigned char *pInBuf;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadCallArgs != NULL && pThreadCallArgs->pInBuf != NULL && pThreadCallArgs->nInBufSize > 0) {
            nThreadCallArgsBuf = SMF_ALIGNED(sizeof(ThreadCallArgs)) + SMF_ALIGNED(pThreadCallArgs->nInBufSize);
            pThreadCallArgsBuf = reinterpret_cast<ThreadCallArgs *>(new SMF_MACHINE_WORD_TYPE[nThreadCallArgsBuf / sizeof(SMF_MACHINE_WORD_TYPE)]);
            pInBuf = reinterpret_cast<unsigned char *>(pThreadCallArgsBuf) + SMF_ALIGNED(sizeof(ThreadCallArgs));
            if (pThreadCallArgsBuf != 0) {
                ::memcpy(reinterpret_cast<void *>(pInBuf), pThreadCallArgs->pInBuf, pThreadCallArgs->nInBufSize);

                *pThreadCallArgsBuf = *pThreadCallArgs;
                pThreadCallArgsBuf->pInBuf = pInBuf;

                if (pThreadCallArgsBuf->pOutBuf == NULL || pThreadCallArgsBuf->nOutBufSize == 0) {
                    pThreadCallArgsBuf->pOutBuf = NULL;
                    pThreadCallArgsBuf->nOutBufSize = 0;
                }

                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(0, 0, 1, nCallId, nThreadCallArgsBuf));
                lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadCallArgsBuf);
            } else {
                nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
            }
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(0, 0, 0, nCallId, 0));
            lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(reinterpret_cast<void *>(NULL));
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_ASYNC_CALL, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->PostMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall(CallId nCallId, ThreadCallArgs *pThreadCallArgs /*= NULL*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckThreadCallArgs(nCallId, pThreadCallArgs);
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bRemoteAgent) {
            nErrorCode = SyncCall_Remote(nCallId, pThreadCallArgs);
        } else {
            nErrorCode = SyncCall_Local(nCallId, pThreadCallArgs);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall_Remote(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;
    ThreadCallArgs *pThreadCallArgsBuf = NULL;
    unsigned int nThreadCallArgsBufSize = 0;
    unsigned char *pInBuf = NULL;
    unsigned int nInBufSize = 0;
    unsigned char *pOutBuf = NULL;
    unsigned int nOutBufSize = 0;
    unsigned int nTotalBufSize = 0;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = CheckSharedEnvironment();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    m_pRemoteSharedSendMsgBufLock->Lock();

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadCallArgs != NULL) {
            if ((pThreadCallArgs->pInBuf != NULL && pThreadCallArgs->nInBufSize > 0) || (pThreadCallArgs->pOutBuf != NULL && pThreadCallArgs->nOutBufSize > 0)) {
                pThreadCallArgsBuf = reinterpret_cast<ThreadCallArgs *>(m_pRemoteSharedSendMsgBufAddr);
                nThreadCallArgsBufSize = SMF_ALIGNED(sizeof(ThreadCallArgs));
                *pThreadCallArgsBuf = *pThreadCallArgs;
                if (pThreadCallArgsBuf->pInBuf != NULL && pThreadCallArgsBuf->nInBufSize > 0) {
                    pInBuf = reinterpret_cast<unsigned char *>(pThreadCallArgsBuf) + nThreadCallArgsBufSize;
                    nInBufSize = SMF_ALIGNED(pThreadCallArgs->nInBufSize);
                    memcpy(pInBuf, pThreadCallArgsBuf->pInBuf, pThreadCallArgsBuf->nInBufSize);
                }
                if (pThreadCallArgsBuf->pOutBuf != NULL && pThreadCallArgsBuf->nOutBufSize > 0) {
                    pOutBuf = reinterpret_cast<unsigned char *>(pThreadCallArgsBuf) + nThreadCallArgsBufSize + nInBufSize;
                    nOutBufSize = SMF_ALIGNED(pThreadCallArgsBuf->nOutBufSize);
                }

                //
                // Fix addr before sending to another process
                //
                if (pThreadCallArgsBuf->pInBuf != NULL && pThreadCallArgsBuf->nInBufSize > 0) {
                    pThreadCallArgsBuf->pInBuf = reinterpret_cast<unsigned char *>(nThreadCallArgsBufSize);
                } else {
                    pThreadCallArgsBuf->pInBuf = NULL;
                    pThreadCallArgsBuf->nInBufSize = 0;
                }
                if (pThreadCallArgsBuf->pOutBuf != NULL && pThreadCallArgsBuf->nOutBufSize > 0) {
                    pThreadCallArgsBuf->pOutBuf = reinterpret_cast<unsigned char *>(nThreadCallArgsBufSize + nInBufSize);
                } else {
                    pThreadCallArgsBuf->pOutBuf = NULL;
                    pThreadCallArgsBuf->nOutBufSize = 0;
                }

                nTotalBufSize = SMF_ALIGNED(sizeof(ThreadCallArgs)) + nInBufSize  + nOutBufSize;
                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, 0, 0, nCallId, nTotalBufSize));
                lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadCallArgsBuf);
            } else {
                wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, 0, 0, nCallId, 0));
                lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
            }
        } else {
            wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, 0, 0, nCallId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_SYNC_CALL, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->SendMsg(msg);
    }

    if (nErrorCode == SMF_ERR_OK) {
        //
        // Fix addr after the process being proecessed
        //
        if (pThreadCallArgsBuf->pInBuf != NULL) {
            pThreadCallArgsBuf->pInBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadCallArgsBuf) + nThreadCallArgsBufSize);
        }
        if (pThreadCallArgsBuf->pOutBuf != NULL) {
            pThreadCallArgsBuf->pOutBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadCallArgsBuf)+ nThreadCallArgsBufSize + nInBufSize);
        }

        if (pThreadCallArgs != NULL && pThreadCallArgsBuf != NULL) {
            pThreadCallArgs->nOutBufRetSize = pThreadCallArgsBuf->nOutBufRetSize < pThreadCallArgs->nOutBufSize ? pThreadCallArgsBuf->nOutBufRetSize : pThreadCallArgs->nOutBufSize;
            pThreadCallArgs->nRet = pThreadCallArgsBuf->nRet;
            if (pThreadCallArgs->pOutBuf != NULL && pThreadCallArgsBuf->pOutBuf != NULL) {
                memcpy(pThreadCallArgs->pOutBuf, pThreadCallArgsBuf->pOutBuf, pThreadCallArgs->nOutBufRetSize);
            }
        }
    }

    m_pRemoteSharedSendMsgBufLock->Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall_Local(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformThread::ThreadId nCurrentThreadId = SMF_PlatformThread::GetCurrentThreadId();

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (SMF_PlatformThread::IsThreadEqual(nCurrentThreadId, m_oPlatformThread.GetThreadId())) {
            nErrorCode = SyncCall_Local_InSameThread(nCallId, pThreadCallArgs);
        } else {
            nErrorCode = SyncCall_Local_InOtherThread(nCallId, pThreadCallArgs);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall_Local_InSameThread(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_ThreadCallHandler *pCallHandler;
    ThreadCallArgs oNullThreadCallArgs;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = SMF_ERR_NOT_PROCESSED;
        pCallHandler = GetCallHandler(nCallId);
        if (pCallHandler != NULL) {
            pCallHandler->ThreadCallProc(
                *this,
                nCallId,
                pThreadCallArgs != NULL ? *pThreadCallArgs : oNullThreadCallArgs
            );
            nErrorCode = SMF_ERR_OK;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall_Local_InOtherThread(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadCallArgs != NULL) {
            wParam = static_cast<unsigned int>(SMF_MAKE_CALL_WPARAM(0, 0, 0, nCallId, sizeof(ThreadCallArgs)));
            lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pThreadCallArgs);
        } else {
            wParam = static_cast<unsigned int>(SMF_MAKE_CALL_WPARAM(0, 0, 0, nCallId, 0));
            lParam = static_cast<SMF_PlatformMsgQ::LParam>(NULL);
        }

        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_SYNC_CALL, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->SendMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetThreadHandler(SMF_ThreadHandler *pThreadHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pThreadHandler = pThreadHandler;
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetTimerHandler(TimerId nTimerId, SMF_ThreadTimerHandler *pTimerHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        TimerHandlerMap::iterator it = m_oTimerHandlerMap.find(nTimerId);
        if (pTimerHandler != NULL) {
            if (it != m_oTimerHandlerMap.end()) {
                (*it).second = pTimerHandler;
            } else {
                m_oTimerHandlerMap.insert(TimerHandlerMap::value_type(nTimerId, pTimerHandler));
            }
        } else {
            if (it != m_oTimerHandlerMap.end()) {
                m_oTimerHandlerMap.erase(it);
            }
        }
        Unlock();
    }

    return nErrorCode;
}

void SMF_BaseThread::SetTimerRepeat(TimerId nTimerId, bool bRepeat)
{
    Lock();
    TimerAttribMap::iterator it = m_oTimerAttribMap.find(nTimerId);
    if (it != m_oTimerAttribMap.end()) {
        (*it).second = TimerAtrrib(bRepeat);
    } else {
        m_oTimerAttribMap.insert(TimerAttribMap::value_type(nTimerId, TimerAtrrib(bRepeat)));
    }
    Unlock();
}

bool SMF_BaseThread::GetTimerRepeat(TimerId nTimerId)
{
    bool bRepeat = false;

    Lock();
    TimerAttribMap::iterator it = m_oTimerAttribMap.find(nTimerId);
    if (it != m_oTimerAttribMap.end()) {
        TimerAtrrib &oTimerAtrrib = (*it).second;
        bRepeat = oTimerAtrrib.bRepeat;
    }
    Unlock();

    return bRepeat;
}

SMF_ErrorCode SMF_BaseThread::SetCallHandler(CallId nCallId, SMF_ThreadCallHandler *pCallHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        CallHandlerMap::iterator it = m_oCallHandlerMap.find(nCallId);
        if (pCallHandler != NULL) {
            if (it != m_oCallHandlerMap.end()) {
                (*it).second = pCallHandler;
            } else {
                m_oCallHandlerMap.insert(CallHandlerMap::value_type(nCallId, pCallHandler));
            }
        } else {
            if (it != m_oCallHandlerMap.end()) {
                m_oCallHandlerMap.erase(it);
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetEventQueueHandler(QueueId nQueueId, SMF_ThreadEventQueueHandler *pEventQueueHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        EventQueueHandlerMap::iterator it = m_oEventQueueHandlerMap.find(nQueueId);
        if (pEventQueueHandler != NULL) {
            if (it != m_oEventQueueHandlerMap.end()) {
                (*it).second = pEventQueueHandler;
            } else {
                m_oEventQueueHandlerMap.insert(EventQueueHandlerMap::value_type(nQueueId, pEventQueueHandler));
            }
        } else {
            if (it != m_oEventQueueHandlerMap.end()) {
                m_oEventQueueHandlerMap.erase(it);
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetThreadState(ThreadState nThreadState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_nThreadState = nThreadState;
        m_oThreadStateEvent.SetEvent();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetThreadData(ThreadDataId nThreadDataId, ThreadData oThreadData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        ThreadDataMap::iterator it = m_oThreadDataMap.find(nThreadDataId);
        if (it != m_oThreadDataMap.end()) {
            (*it).second = oThreadData;
        } else {
            m_oThreadDataMap.insert(ThreadDataMap::value_type(nThreadDataId, oThreadData));
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_BaseThread::ThreadData SMF_BaseThread::GetThreadData(ThreadDataId nThreadDataId)
{
    ThreadData oThreadData;

    Lock();
    ThreadDataMap::iterator it = m_oThreadDataMap.find(nThreadDataId);
    if (it != m_oThreadDataMap.end()) {
        oThreadData = (*it).second;
    }
    Unlock();

    return oThreadData;
}

SMF_ErrorCode SMF_BaseThread::WaitForThreadState(ThreadState nThreadStateSet, unsigned int nTimeout/* = SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bWaitFinished = false;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (nErrorCode == SMF_ERR_OK) {
            nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, &m_oThreadStateEvent, nTimeout);
            if (nErrorCode != SMF_ERR_OK) {
                break;
            }
            switch (nWaitResult) {
            case SMF_PlatformEvent::WAIT_RESULT_OBJECT_0:
                {
                    if (nThreadStateSet & m_nThreadState) {
                        bWaitFinished = true;
                        break;
                    }
                }
                break;
            case SMF_PlatformEvent::WAIT_RESULT_TIMEOUT:
            default:
                {
                    nErrorCode = SMF_ERR_TIMEOUT;
                }
                break;
            }
            if (bWaitFinished) {
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::StartTimer(TimerId nTimerId, unsigned int nElapse, bool bRepeat /*= false*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    InternalCallArgs ca;
    ThreadCallArgs oThreadCallArgs;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetTimerRepeat(nTimerId, bRepeat);
        SetTimerEnabled(nTimerId, true);

        ca.unInputArgs.stStartTimer.nTimerId = nTimerId;
        ca.unInputArgs.stStartTimer.nElapse = nElapse;
        ca.unInputArgs.stStartTimer.bRepeat = bRepeat;
        oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca);
        oThreadCallArgs.nInBufSize = sizeof(InternalCallArgs);
        nErrorCode = SyncCall(CALL_ID_SYS_START_TIMER, &oThreadCallArgs);
        if (nErrorCode != SMF_ERR_OK) {
            SetTimerEnabled(nTimerId, false);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    InternalCallArgs ca;
    ThreadCallArgs oThreadCallArgs;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetTimerRepeat(nTimerId, false);
        SetTimerEnabled(nTimerId, false);

        ca.unInputArgs.stStopTimer.nTimerId = nTimerId;
        oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca);
        oThreadCallArgs.nInBufSize = sizeof(InternalCallArgs);
        nErrorCode = SyncCall(CALL_ID_SYS_STOP_TIMER, &oThreadCallArgs);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = rPlatformMsgQ.ExitMsgLoop();
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, SMF_PlatformMsgQ::Msg &rMsg)
{
	SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_ThreadCallHandler *pCallHandler;
    ThreadCallArgs oNullThreadCallArgs;
    ThreadCallArgs *pThreadCallArgs;
    ThreadEventArgs *pThreadEventArgs;
    bool bIsRemote = false;
    bool bRemoteWaiting = false;
    bool bIsDynamiac = false;
    unsigned int nArgsSize;
    unsigned char *pMsgQDynamicBuf = NULL;
    unsigned int nMsgQDynamicBufSize = 0;
    unsigned char *pMsgQPostedOutBuf = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bSupportInterProcessEvent) {
            nErrorCode = CheckSharedEnvironment();
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        switch (rMsg.nMsgId) {
	    case MSG_ID_THREAD_ASYNC_CALL:
        case MSG_ID_THREAD_SYNC_CALL:
            {
                pThreadCallArgs = reinterpret_cast<ThreadCallArgs *>(rMsg.lParam);
                nArgsSize = SMF_GET_CALL_ARGS_SIZE(rMsg.wParam);
                bIsRemote = SMF_GET_CALL_IS_REMOTE(rMsg.wParam);
                bRemoteWaiting = SMF_GET_CALL_REMOTE_WAITING(rMsg.wParam);
                bIsDynamiac = SMF_GET_CALL_IS_DYNAMIC(rMsg.wParam);

                if (bIsRemote) {
                    if (pThreadCallArgs != NULL) {
                        if (!bIsDynamiac) {
                            //
                            // Fix buffer address
                            //
                            pThreadCallArgs = reinterpret_cast<ThreadCallArgs *>(m_pRemoteSharedSendMsgBufAddr);
                            if (pThreadCallArgs->pInBuf != NULL) {
                                pThreadCallArgs->pInBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadCallArgs) + reinterpret_cast<unsigned long>(pThreadCallArgs->pInBuf));
                            }
                            if (pThreadCallArgs->pOutBuf != NULL) {
                                pThreadCallArgs->pOutBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadCallArgs)+ reinterpret_cast<unsigned long>(pThreadCallArgs->pOutBuf));
                            }
                        } else {
                            pThreadCallArgs = NULL;

                            nMsgQDynamicBufSize = nArgsSize;
                            pMsgQDynamicBuf = new unsigned char [nMsgQDynamicBufSize];
                            if (pMsgQDynamicBuf != NULL) {
                                memset(pMsgQDynamicBuf, 0, nMsgQDynamicBufSize);
                                nErrorCode = m_pRemoteSharedPostMsgBufCyclicBuf->GetData(pMsgQDynamicBuf, nMsgQDynamicBufSize);
                                if (nErrorCode == SMF_ERR_OK) {
                                    pThreadCallArgs = reinterpret_cast<ThreadCallArgs *>(pMsgQDynamicBuf);
                                    if (pThreadCallArgs->pInBuf != NULL) {
                                        pThreadCallArgs->pInBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadCallArgs) + reinterpret_cast<unsigned long>(pThreadCallArgs->pInBuf));
                                    }

                                    if (pThreadCallArgs->pOutBuf != NULL && pThreadCallArgs->nOutBufSize != 0) {
                                        pThreadCallArgs->pOutBuf = new unsigned char [pThreadCallArgs->nOutBufSize];
                                    }
                                    if (pThreadCallArgs->pOutBuf == NULL || pThreadCallArgs->nOutBufSize == 0) {
                                        pThreadCallArgs->pOutBuf = NULL;
                                        pThreadCallArgs->nOutBufSize = 0;
                                    }
                                }
                            } else {
                                nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
                            }
                        }
                    }
                } else {
                    if (pThreadCallArgs != NULL) {
                        if (bIsDynamiac) {
                            if (pThreadCallArgs->pOutBuf != NULL && pThreadCallArgs->nOutBufSize != 0) {
                                pThreadCallArgs->pOutBuf = new unsigned char [pThreadCallArgs->nOutBufSize];
                            }
                            if (pThreadCallArgs->pOutBuf == NULL || pThreadCallArgs->nOutBufSize == 0) {
                                pThreadCallArgs->pOutBuf = NULL;
                                pThreadCallArgs->nOutBufSize = 0;
                            }
                        }
                    }
                }
                pCallHandler = GetCallHandler(SMF_GET_CALL_ID(rMsg.wParam));
                if (pCallHandler != NULL) {
                    nErrorCode = pCallHandler->ThreadCallProc(
                        *this,
                        SMF_GET_CALL_ID(rMsg.wParam),
                        pThreadCallArgs != NULL ? *pThreadCallArgs : oNullThreadCallArgs
                    );
                }
                if (!bIsRemote && bIsDynamiac && rMsg.lParam != 0) {
                    delete [] (reinterpret_cast<SMF_MACHINE_WORD_TYPE *>(rMsg.lParam));
                }

                if (bIsRemote && bRemoteWaiting) {
                    m_pRemotePostMsgWaitEvent->SetEvent();
                }

                if (bIsRemote && bIsDynamiac && pMsgQDynamicBuf != NULL) {
                    delete [] pMsgQDynamicBuf;
                    pMsgQDynamicBuf = NULL;
                }

                if (pMsgQPostedOutBuf != NULL) {
                    delete [] pMsgQPostedOutBuf;
                    pMsgQPostedOutBuf = NULL;
                }
            }
            break;
        default:
		    {
                nErrorCode = SMF_ERR_NOT_PROCESSED;
                if (rMsg.nMsgId >= MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE) {
                    pThreadEventArgs = reinterpret_cast<ThreadEventArgs *>(rMsg.lParam);
                    nArgsSize = SMF_GET_CALL_ARGS_SIZE(rMsg.wParam);
                    bIsRemote = SMF_GET_EQ_IS_REMOTE(rMsg.wParam);
                    bRemoteWaiting = SMF_GET_EQ_REMOTE_WAITING(rMsg.wParam);
                    bIsDynamiac = SMF_GET_EQ_IS_DYNAMIC(rMsg.wParam);

                    if (bIsRemote) {
                        if (pThreadEventArgs != NULL) {
                            if (!bIsDynamiac) {
                                //
                                // Fix buffer address
                                //
                                pThreadEventArgs = reinterpret_cast<ThreadEventArgs *>(m_pRemoteSharedSendMsgBufAddr);
                                if (pThreadEventArgs->pInBuf != NULL) {
                                    pThreadEventArgs->pInBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadEventArgs) + reinterpret_cast<unsigned long>(pThreadEventArgs->pInBuf));
                                }
                                if (pThreadEventArgs->pOutBuf != NULL) {
                                    pThreadEventArgs->pOutBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadEventArgs)+ reinterpret_cast<unsigned long>(pThreadEventArgs->pOutBuf));
                                }
                            } else {
                                pThreadEventArgs = NULL;

                                nMsgQDynamicBufSize = nArgsSize;
                                pMsgQDynamicBuf = new unsigned char [nMsgQDynamicBufSize];
                                if (pMsgQDynamicBuf != NULL) {
                                    memset(pMsgQDynamicBuf, 0, nMsgQDynamicBufSize);
                                    nErrorCode = m_pRemoteSharedPostMsgBufCyclicBuf->GetData(pMsgQDynamicBuf, nMsgQDynamicBufSize);
                                    if (nErrorCode == SMF_ERR_OK) {
                                        pThreadEventArgs = reinterpret_cast<ThreadEventArgs *>(pMsgQDynamicBuf);
                                        if (pThreadEventArgs->pInBuf != NULL) {
                                            pThreadEventArgs->pInBuf = reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(pThreadEventArgs) + reinterpret_cast<unsigned long>(pThreadEventArgs->pInBuf));
                                        }

                                        if (pThreadEventArgs->pOutBuf != NULL && pThreadEventArgs->nOutBufSize != 0) {
                                            pThreadEventArgs->pOutBuf = new unsigned char [pThreadEventArgs->nOutBufSize];
                                        }
                                        if (pThreadEventArgs->pOutBuf == NULL || pThreadEventArgs->nOutBufSize == 0) {
                                            pThreadEventArgs->pOutBuf = NULL;
                                            pThreadEventArgs->nOutBufSize = 0;
                                        }
                                    }
                                } else {
                                    nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
                                }
                            }
                        }
                    } else {
                        if (pThreadEventArgs != NULL) {
                            if (bIsDynamiac) {
                                if (pThreadEventArgs->pOutBuf != NULL && pThreadEventArgs->nOutBufSize != 0) {
                                    pThreadEventArgs->pOutBuf = new unsigned char [pThreadEventArgs->nOutBufSize];
                                }
                                if (pThreadEventArgs->pOutBuf == NULL || pThreadEventArgs->nOutBufSize == 0) {
                                    pThreadEventArgs->pOutBuf = NULL;
                                    pThreadEventArgs->nOutBufSize = 0;
                                }
                            }
                        }
                    }
                    QueueId nQueueId = SMF_GET_EQ_QUEUE_ID(rMsg.wParam);
                    SMF_ThreadEventQueueHandler *pThreadEventQueueHandler = GetEventQueueHandler(nQueueId);
                    if (pThreadEventQueueHandler != NULL) {
                        EventData oEventData(
                            SMF_GET_EQ_ENGINE_ID(rMsg.wParam),
                            SMF_GET_EQ_SENDER_ID(rMsg.wParam),
                            rMsg.nMsgId - MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE,
                            pThreadEventArgs
                        );
                        nErrorCode = pThreadEventQueueHandler->ThreadEventQueueProc(*this, nQueueId, oEventData);
                    }
                    if (!bIsRemote && bIsDynamiac && rMsg.lParam != 0) {
                        delete [] (reinterpret_cast<SMF_MACHINE_WORD_TYPE *>(rMsg.lParam));
                    }

                    if (bIsRemote && bRemoteWaiting) {
                        m_pRemotePostMsgWaitEvent->SetEvent();
                    }

                    if (bIsRemote && bIsDynamiac && pMsgQDynamicBuf != NULL) {
                        delete [] pMsgQDynamicBuf;
                        pMsgQDynamicBuf = NULL;
                    }

                    if (pMsgQPostedOutBuf != NULL) {
                        delete [] pMsgQPostedOutBuf;
                        pMsgQPostedOutBuf = NULL;
                    }
                }
		    }
		    break;
	    }
    }

	return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::Msg oMsg;

    SetThreadState(TS_CREATED);
    SetThreadState(TS_ACTIVATING);

    if (nErrorCode == SMF_ERR_OK) {
        m_pPlatformMsgQ = new SMF_PlatformMsgQ(GetThreadMsgQName().c_str(), m_bSupportInterProcessEvent);
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_THREAD;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pPlatformMsgQ->SetMsgQHandler(this);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pPlatformMsgQ->ConnectToThread(&m_oPlatformThread);
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_pPlatformTimer = new SMF_PlatformTimer(GetThreadTimerName().c_str(), m_bSupportInterProcessEvent);
        if (m_pPlatformTimer == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_THREAD;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pPlatformTimer->SetTimerHandler(this);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pPlatformTimer->ConnectToThread(&m_oPlatformThread);
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetThreadState(TS_ACTIVATED);
        nErrorCode = m_pPlatformMsgQ->EnterMsgLoop();
        SetThreadState(TS_DEACTIVING);
        SetThreadState(TS_DEACTIVED);
    }

    SetThreadState(TS_DESTRORYING);

    if (m_pPlatformTimer != NULL) {
        delete m_pPlatformTimer;
        m_pPlatformTimer = NULL;
    }

    if (m_pPlatformMsgQ != NULL) {
        delete m_pPlatformMsgQ;
        m_pPlatformMsgQ = NULL;
    }

    SetThreadState(TS_DESTRORYED);

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nCallId) {
    case CALL_ID_SYS_START_TIMER:
        {
            if (rThreadCallArgs.pInBuf != NULL && rThreadCallArgs.nInBufSize == sizeof(InternalCallArgs)) {
                InternalCallArgs *pCallArgs = reinterpret_cast<InternalCallArgs *>(rThreadCallArgs.pInBuf);
                pCallArgs->nRet = 0;

                if (nErrorCode == SMF_ERR_OK) {
                    if (m_pPlatformTimer == NULL) {
                        nErrorCode = SMF_ERR_NULL_PTR;
                        break;
                    }
                }

                pCallArgs->nRet = m_pPlatformTimer->StartTimer(
                    pCallArgs->unInputArgs.stStartTimer.nTimerId,
                    pCallArgs->unInputArgs.stStartTimer.nElapse,
                    pCallArgs->unInputArgs.stStartTimer.bRepeat
                );
            }
        }
        break;

    case CALL_ID_SYS_STOP_TIMER:
        {
            if (nErrorCode == SMF_ERR_OK) {
                if (m_pPlatformTimer == NULL) {
                    nErrorCode = SMF_ERR_NULL_PTR;
                    break;
                }
            }

            if (rThreadCallArgs.pInBuf != NULL && rThreadCallArgs.nInBufSize == sizeof(InternalCallArgs)) {
                InternalCallArgs *pCallArgs = reinterpret_cast<InternalCallArgs *>(rThreadCallArgs.pInBuf);
                pCallArgs->nRet = m_pPlatformTimer->StopTimer(
                    pCallArgs->unInputArgs.stStopTimer.nTimerId
                );
            }
        }
        break;

    default:
        {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
        break;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PlatformThreadProc(SMF_PlatformThread &rPlatformThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_ThreadHandler *pThreadHandler = GetThreadHandler();

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadHandler != NULL) {
            nErrorCode = pThreadHandler->ThreadProc(*this);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_ThreadTimerHandler *pTimerHandler = GetTimerHandler(nTimerId);

    if (nErrorCode == SMF_ERR_OK) {
        if (pTimerHandler != NULL) {
            if (!GetTimerRepeat(nTimerId)) {
                rPlatformTimer.StopTimer(nTimerId);
            }
            if (GetTimerEnabled(nTimerId)) {
                pTimerHandler->ThreadTimerProc(*this, nTimerId);
            }
        } else {
            rPlatformTimer.StopTimer(nTimerId);
        }
    }

    return nErrorCode;
}

SMF_ThreadHandler *SMF_BaseThread::GetThreadHandler()
{
    SMF_ThreadHandler *pThreadHandler = this;

    Lock();
    if (m_pThreadHandler != NULL) {
        pThreadHandler = m_pThreadHandler;
    }
    Unlock();

    return pThreadHandler;
}

SMF_ThreadTimerHandler *SMF_BaseThread::GetTimerHandler(TimerId nTimerId)
{
    SMF_ThreadTimerHandler *pTimerHandler = this;

    Lock();
    TimerHandlerMap::iterator it = m_oTimerHandlerMap.find(TIMER_ID_ALL);
    if (it != m_oTimerHandlerMap.end()) {
        pTimerHandler = (*it).second;
    } else {
        TimerHandlerMap::iterator it = m_oTimerHandlerMap.find(nTimerId);
        if (it != m_oTimerHandlerMap.end()) {
            pTimerHandler = (*it).second;
        }
    }
    Unlock();

    return pTimerHandler;
}

void SMF_BaseThread::SetTimerEnabled(TimerId nTimerId, bool bEnabled)
{
    EnabledTimerSet::iterator it;

    Lock();
    it = m_oEnabledTimerSet.find(nTimerId);
    if (it == m_oEnabledTimerSet.end()) {
        if (bEnabled) {
            m_oEnabledTimerSet.insert(nTimerId);
        }
    } else {
        if (!bEnabled) {
            m_oEnabledTimerSet.erase(it);
        }
    }
    Unlock();
}

bool SMF_BaseThread::GetTimerEnabled(TimerId nTimerId)
{
    bool bRet = false;
    EnabledTimerSet::iterator it;

    Lock();
    it = m_oEnabledTimerSet.find(nTimerId);
    if (it == m_oEnabledTimerSet.end()) {
        bRet = false;
    } else {
        bRet = true;
    }
    Unlock();

    return bRet;
}

SMF_ThreadEventQueueHandler *SMF_BaseThread::GetEventQueueHandler(QueueId nQueueId)
{
    SMF_ThreadEventQueueHandler *pEventQueueHandler = this;

    Lock();
    EventQueueHandlerMap::iterator it = m_oEventQueueHandlerMap.find(QUEUE_ID_ALL);
    if (it != m_oEventQueueHandlerMap.end()) {
        pEventQueueHandler = (*it).second;
    } else {
        EventQueueHandlerMap::iterator it = m_oEventQueueHandlerMap.find(nQueueId);
        if (it != m_oEventQueueHandlerMap.end()) {
            pEventQueueHandler = (*it).second;
        }
    }
    Unlock();

    return pEventQueueHandler;
}

SMF_ThreadCallHandler *SMF_BaseThread::GetCallHandler(CallId nCallId)
{
    SMF_ThreadCallHandler *pCallHandler = this;

    if (nCallId != CALL_ID_SYS_START_TIMER && nCallId != CALL_ID_SYS_STOP_TIMER) {
        Lock();
        CallHandlerMap::iterator it = m_oCallHandlerMap.find(CALL_ID_ALL);
        if (it != m_oCallHandlerMap.end()) {
            pCallHandler = (*it).second;
        } else {
            CallHandlerMap::iterator it = m_oCallHandlerMap.find(nCallId);
            if (it != m_oCallHandlerMap.end()) {
                pCallHandler = (*it).second;
            }
        }
        Unlock();
    }

    return pCallHandler;
}

void SMF_BaseThread::Lock()
{
    m_oLock.Lock();
}

void SMF_BaseThread::Unlock()
{
    m_oLock.Unlock();
}

SMF_ErrorCode SMF_BaseThread::NotSupportedInRemoteAgent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bRemoteAgent) {
            nErrorCode = SMF_ERR_NOT_SUPPORTED;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::CheckSharedEnvironment()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (!m_bSupportInterProcessEvent) {
            nErrorCode = SMF_ERR_NOT_SHARED_OBJECT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemoteSharedPostMsgBufShm == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_PLATFORM_SHARED_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemoteSharedPostMsgBufAddr == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemoteSharedPostMsgBufCyclicBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemoteSharedSendMsgBufShm == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_PLATFORM_SHARED_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemoteSharedSendMsgBufAddr == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemoteSharedSendMsgBufLock == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_PLATFORM_LOCK;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRemotePostMsgWaitEvent == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_PLATFORM_EVENT;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::CheckThreadCallArgs(CallId nCallId, ThreadCallArgs *pThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (nCallId >= SMF_GET_CALL_MASK_CALL_ID) {
            nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadCallArgs != NULL) {
            if (pThreadCallArgs->nInBufSize > THREAD_MSG_IN_BUF_SIZE_MAX) {
                nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
            }
            if (pThreadCallArgs->nOutBufSize > THREAD_MSG_OUT_BUF_SIZE_MAX) {
                nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::CheckThreadEventArgs(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (nQueueId >= SMF_GET_EQ_MASK_QUEUE_ID) {
            nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nEngineId >= SMF_GET_EQ_MASK_ENGINE_ID) {
            nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nSenderId >= SMF_GET_EQ_MASK_SENDER_ID) {
            nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (pThreadEventArgs != NULL) {
            if (pThreadEventArgs->nInBufSize > THREAD_MSG_IN_BUF_SIZE_MAX) {
                nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
            }
            if (pThreadEventArgs->nOutBufSize > THREAD_MSG_OUT_BUF_SIZE_MAX) {
                nErrorCode = SMF_ERR_ARG_OUT_OF_RANGE;
            }
        }
    }

    return nErrorCode;
}

std::string SMF_BaseThread::GetThreadMsgQName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_ThreadMsgQ", m_sThreadName.c_str());

    return sRet;
}
std::string SMF_BaseThread::GetThreadTimerName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_ThreadTimer", m_sThreadName.c_str());

    return sRet;
}

std::string SMF_BaseThread::GetRemoteSharedPostMsgBufShmName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_RemoteSharedPostMsgBufShm", m_sThreadName.c_str());

    return sRet;
}

std::string SMF_BaseThread::GetRemoteSharedPostMsgBufCyclicBufName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_RemoteSharedPostMsgBufCyclicBuf", m_sThreadName.c_str());

    return sRet;
}

std::string SMF_BaseThread::GetRemoteSharedPostMsgBufLockName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_RemoteSharedPostMsgBufLock", m_sThreadName.c_str());

    return sRet;
}

std::string SMF_BaseThread::GetRemoteSharedSendMsgBufShmName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_RemoteSharedSendMsgBufShm", m_sThreadName.c_str());

    return sRet;
}

std::string SMF_BaseThread::GetRemoteSharedSendMsgBufLockName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_RemoteSharedSendMsgBufLock", m_sThreadName.c_str());

    return sRet;
}

std::string SMF_BaseThread::GetRemotePostMsgWaitEventName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_RemotePostMsgWaitEvent", m_sThreadName.c_str());

    return sRet;
}

#ifdef SMF_BASE_THREAD_EXAMPLE_CODE

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleWorkThread
//
SMF_ExampleWorkThread::SMF_ExampleWorkThread(const std::string &sThreadName)
    : SMF_BaseThread(sThreadName)
{

}

SMF_ExampleWorkThread::~SMF_ExampleWorkThread()
{

}

SMF_ErrorCode SMF_ExampleWorkThread::ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nTimerId) {
    case TIMER_ID_TIMER_001:
        {
            nErrorCode = OnTimer_Timer1();
        }
        break;
    case TIMER_ID_TIMER_002:
        {
            nErrorCode = OnTimer_Timer2();
        }
        break;
    default:
        {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
        break;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    CallProc pCallProc = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pCallProc = LookupCallProc(nCallId);
        if (pCallProc == NULL) {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        CallArgs &rCallArgs = *reinterpret_cast<CallArgs*>(rThreadCallArgs.pInBuf);
        nErrorCode = (this->*pCallProc)(rCallArgs);
    }

    return nErrorCode;
}

SMF_ExampleWorkThread::CallProc SMF_ExampleWorkThread::LookupCallProc(CallId nCallId)
{
#define DEFINE_CALL_MAP_ITEM(call_name) { CALL_ID_ ## call_name, &SMF_ExampleWorkThread::OnCall_ ## call_name }
    static struct {
        CallId nCallId;
        CallProc pCallProc;
    } oCallMap[] = {
        DEFINE_CALL_MAP_ITEM(Call001),
        DEFINE_CALL_MAP_ITEM(Call002),
        DEFINE_CALL_MAP_ITEM(Call003),
        DEFINE_CALL_MAP_ITEM(Call004),
    };
#undef DEFINE_CALL_MAP_ITEM
    CallProc pCallProc = NULL;
    int nMapCount = sizeof(oCallMap) / sizeof(oCallMap[0]);
    int i;

    for (i = 0; i < nMapCount; i++) {
        if (oCallMap[i].nCallId == nCallId) {
            pCallProc = oCallMap[i].pCallProc;
            break;
        }
    }

    return pCallProc;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call001(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call002(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call003(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call004(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnTimer_Timer1()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnTimer_Timer2()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThreadHost::Test()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oTimerThread.Start();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oCallThread.Start();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oTimerThread.StartTimer(SMF_ExampleWorkThread::TIMER_ID_TIMER_001, 3000);
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_ExampleWorkThread::CallArgs ca;
        ca.unInputArgs.stCall001.nArg1 = 100;

        SMF_ThreadCallHandler::ThreadCallArgs tca(reinterpret_cast<unsigned char *>(&ca), sizeof(ca.unInputArgs.stCall001), NULL, 0);
        m_oCallThread.AsyncCall(SMF_ExampleWorkThread::CALL_ID_Call001, &tca);
    }

    return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleThreadHost
//
SMF_ExampleThreadHost::SMF_ExampleThreadHost()
    : m_oCallThread("ExampleThreadHost_CallThread")
    , m_oTimerThread("ExampleThreadHost_TimerThread")
{
    m_oCallThread.SetCallHandler(this);
    m_oTimerThread.SetTimerHandler(this);
}

SMF_ExampleThreadHost::~SMF_ExampleThreadHost()
{

}

SMF_ErrorCode SMF_ExampleThreadHost::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    CallProc pCallProc = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        pCallProc = LookupCallProc(rThread, nCallId);
        if (pCallProc == NULL) {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        CallArgs &rCallArgs = *reinterpret_cast<CallArgs*>(rThreadCallArgs.pInBuf);
        nErrorCode = (this->*pCallProc)(rThread, rCallArgs);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nTimerId) {
    case TIMER_ID_TIMER_001:
        {
            nErrorCode = OnTimer_Timer1(rThread);
        }
        break;
    case TIMER_ID_TIMER_002:
        {
            nErrorCode = OnTimer_Timer2(rThread);
        }
        break;
    default:
        {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
        break;
    }

    return nErrorCode;
}

SMF_ExampleThreadHost::CallProc SMF_ExampleThreadHost::LookupCallProc(SMF_BaseThread &rThread, CallId nCallId)
{
#define DEFINE_CALL_MAP_ITEM(call_name) { CALL_ID_ ## call_name, &SMF_ExampleThreadHost::OnCall_ ## call_name }
    static struct {
        CallId nCallId;
        CallProc pCallProc;
    } oCallMap[] = {
        DEFINE_CALL_MAP_ITEM(Call001),
        DEFINE_CALL_MAP_ITEM(Call002),
        DEFINE_CALL_MAP_ITEM(Call003),
        DEFINE_CALL_MAP_ITEM(Call004),
    };
#undef DEFINE_CALL_MAP_ITEM
    CallProc pCallProc = NULL;
    int nMapCount = sizeof(oCallMap) / sizeof(oCallMap[0]);
    int i;

    for (i = 0; i < nMapCount; i++) {
        if (oCallMap[i].nCallId == nCallId) {
            pCallProc = oCallMap[i].pCallProc;
            break;
        }
    }

    return pCallProc;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call001(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call002(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call003(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call004(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnTimer_Timer1(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnTimer_Timer2(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::Test()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oTimerThread.Start();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oCallThread.Start();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oTimerThread.StartTimer(TIMER_ID_TIMER_001, 3000);
    }

    if (nErrorCode == SMF_ERR_OK) {
        CallArgs ca;
        ca.unInputArgs.stCall001.nArg1 = 100;

        SMF_ThreadCallHandler::ThreadCallArgs tca(reinterpret_cast<unsigned char *>(&ca), sizeof(ca.unInputArgs.stCall001), NULL, 0);
        m_oCallThread.AsyncCall(CALL_ID_Call001, &tca);
    }

    return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleSimpleThreadHost
//
SMF_ExampleSimpleThreadHost::SMF_ExampleSimpleThreadHost()
    : m_oWorkThread("ExampleSimpleThreadHost_WorkThread")
{
    m_oWorkThread.SetThreadHandler(this);
}

SMF_ExampleSimpleThreadHost::~SMF_ExampleSimpleThreadHost()
{

}

SMF_ErrorCode SMF_ExampleSimpleThreadHost::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    // NOTE: add some code here to create some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);

    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);
    // NOTE: add some code here to activate some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);

    for (;;) {
        if (rThread.GetThreadState() != SMF_BaseThread::TS_ACTIVATED)
        {
            rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
            break;
        }

        SMF_LOG_FUNC();

        TK_Tools::Sleep(SMF_THREAD_AWAKEN_INTERVAL_MILLISECONDS);
    }

    // NOTE: add some code here to deactivate any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVED);

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYING);
    // NOTE: add some code here to destroy any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYED);

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleSimpleThreadHost::Test()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oWorkThread.Start();
    }

    return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleTimerThread
//
SMF_ErrorCode SMF_ExampleTimerThread::ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch(nTimerId) {
    case TIMER_ID_TIMER_001:
        {
            nErrorCode = OnTimer_Timer001();
        }
        break;

    case TIMER_ID_TIMER_002:
        {
            nErrorCode = OnTimer_Timer002();
        }
        break;

    default:
        {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
        break;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleTimerThread::OnTimer_Timer001()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleTimerThread::OnTimer_Timer002()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_FUNC();

    return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleNormalThread
//
SMF_ExampleNormalThread::SMF_ExampleNormalThread(const std::string &sThreadName)
    : SMF_BaseThread(sThreadName)
{

}

SMF_ExampleNormalThread::~SMF_ExampleNormalThread()
{

}

SMF_ErrorCode SMF_ExampleNormalThread::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    // NOTE: add some code here to create some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);

    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);
    // NOTE: add some code here to activate some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);

    for (;;) {
        if (rThread.GetThreadState() != SMF_BaseThread::TS_ACTIVATED)
        {
            rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
            break;
        }

        SMF_LOG_FUNC();

        TK_Tools::Sleep(SMF_THREAD_AWAKEN_INTERVAL_MILLISECONDS);
    }

    // NOTE: add some code here to deactivate any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVED);

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYING);
    // NOTE: add some code here to destroy any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYED);

    return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleEventThreadHost
//
SMF_ExampleEventThreadHost::SMF_ExampleEventThreadHost()
    : SMF_ThreadEventQueueHandler()
    , m_oEventThread("EventThread")
{
    m_oEventThread.SetEventQueueHandler(0, this);
    m_oEventThread.Start();
}

SMF_ExampleEventThreadHost::~SMF_ExampleEventThreadHost()
{

}

SMF_ErrorCode SMF_ExampleEventThreadHost::ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (rEventData.nEventId) {
    case EVENT_ID_001:
        {
            // do something here
        }
        break;
    case EVENT_ID_002:
        {
            // do something here
        }
        break;
    case EVENT_ID_003:
        {
            // do something here
        }
        break;
    case EVENT_ID_004:
        {
            // do something here
        }
        break;
    default:
        {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
        break;
    }

    return nErrorCode;
}

#endif // #ifdef SMF_BASE_THREAD_EXAMPLE_CODE
