#include "TK_Tools.h"
#include "SMF_BaseThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseThread
//
SMF_BaseThread::SMF_BaseThread(const std::string &sThreadName)
    : SMF_NonCopyable()
    , m_oThreadDataMap()
    , m_nThreadState(TS_INIT)
    , m_oThreadStateEvent(FALSE, FALSE)
    , m_sThreadName(sThreadName)
    , m_pPlatformMsgQ(NULL)
    , m_pPlatformTimer(NULL)
    , m_oPlatformThread(sThreadName.c_str())
    , m_oLock()
    , m_pThreadHandler(NULL)
    , m_oTimerHandlerMap()
    , m_oEnabledTimerSet()
    , m_oCallHandlerMap()
    , m_oEventQueueHandlerMap()
    , m_oTimerAttribMap()
{

}

SMF_BaseThread::~SMF_BaseThread()
{
    Stop();
}

SMF_ErrorCode SMF_BaseThread::Initialize()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = OnInitialize();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::Finialize()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = OnFinialize();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::Start()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

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

SMF_ErrorCode SMF_BaseThread::PostEvent(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;

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
        wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(nQueueId, nEngineId, nSenderId));
        lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pData);
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE + nEventId, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->PostMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData)
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
            nErrorCode = SendEvent_InSameThread(nQueueId, nEngineId, nSenderId, nEventId, pData);
        } else {
            nErrorCode = SendEvent_InOtherThread(nQueueId, nEngineId, nSenderId, nEventId, pData);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent_InSameThread(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData)
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
                pData
            );
            nErrorCode = pThreadEventQueueHandler->ThreadEventQueueProc(*this, nQueueId, oEventData);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SendEvent_InOtherThread(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData)
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
        wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_EQ_WPARAM(nQueueId, nEngineId, nSenderId));
        lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pData);
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE + nEventId, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->SendMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::AsyncCall(CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformMsgQ::WParam wParam = 0;
    SMF_PlatformMsgQ::LParam lParam = 0;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        wParam = static_cast<SMF_PlatformMsgQ::WParam>(SMF_MAKE_CALL_WPARAM(1, nCallId, nArgsSize));
        lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(reinterpret_cast<void *>(NULL));
        if (pArgs != NULL && nArgsSize > 0) {
            lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(new int[(nArgsSize + sizeof(int) - 1) / sizeof(int)]);
            if (lParam != 0) {
                ::memcpy(reinterpret_cast<void *>(lParam), pArgs, nArgsSize);
            } else {
                nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
            }
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPlatformMsgQ == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_ASYNC_CALL, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->PostMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall(CallId nCallId, void *pArgs, unsigned int nArgsSize)
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
            nErrorCode = SyncCall_InSameThread(nCallId, pArgs, nArgsSize);
        } else {
            nErrorCode = SyncCall_InOtherThread(nCallId, pArgs, nArgsSize);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall_InSameThread(CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_ThreadCallHandler *pCallHandler;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = SMF_ERR_NOT_PROCESSED;
        pCallHandler = GetCallHandler(nCallId);
        if (pCallHandler != NULL) {
            pCallHandler->ThreadCallProc(
                *this,
                nCallId,
                pArgs,
                nArgsSize
            );
            nErrorCode = SMF_ERR_OK;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SyncCall_InOtherThread(CallId nCallId, void *pArgs, unsigned int nArgsSize)
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
        wParam = static_cast<unsigned int>(SMF_MAKE_CALL_WPARAM(0, nCallId, nArgsSize));
        lParam = reinterpret_cast<SMF_PlatformMsgQ::LParam>(pArgs);
        SMF_PlatformMsgQ::Msg msg(MSG_ID_THREAD_SYNC_CALL, wParam, lParam);
        nErrorCode = m_pPlatformMsgQ->SendMsg(msg);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetThreadHandler(SMF_ThreadHandler *pThreadHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

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
        m_nThreadState = nThreadState;
        m_oThreadStateEvent.SetEvent();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::SetThreadData(ThreadDataId nThreadDataId, ThreadData oThreadData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

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
    CallArgs ca;

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
        nErrorCode = SyncCall(CALL_ID_SYS_START_TIMER, &ca, sizeof(CallArgs));
        if (nErrorCode != SMF_ERR_OK) {
            SetTimerEnabled(nTimerId, false);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseThread::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    CallArgs ca;

    if (nErrorCode == SMF_ERR_OK) {
        if (GetThreadState() != TS_ACTIVATED) {
            nErrorCode = SMF_ERR_EVENT_THREAD_NOT_READY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        SetTimerRepeat(nTimerId, false);
        SetTimerEnabled(nTimerId, false);

        ca.unInputArgs.stStopTimer.nTimerId = nTimerId;
        nErrorCode = SyncCall(CALL_ID_SYS_STOP_TIMER, &ca, sizeof(CallArgs));
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

    switch (rMsg.nMsgId) {
	case MSG_ID_THREAD_ASYNC_CALL:
		{
            pCallHandler = GetCallHandler(SMF_GET_CALL_ID(rMsg.wParam));
            if (pCallHandler != NULL) {
                nErrorCode = pCallHandler->ThreadCallProc(
                    *this,
                    SMF_GET_CALL_ID(rMsg.wParam),
                    reinterpret_cast<void *>(rMsg.lParam),
                    SMF_GET_ARGS_SIZE(rMsg.wParam)
                );
            }
            if (SMF_GET_CALL_IS_DYNAMIC(rMsg.wParam) && rMsg.lParam != 0) {
                delete [] (reinterpret_cast<int *>(rMsg.lParam));
            }
		}
		break;

	case MSG_ID_THREAD_SYNC_CALL:
		{
            pCallHandler = GetCallHandler(SMF_GET_CALL_ID(rMsg.wParam));
            if (pCallHandler != NULL) {
                nErrorCode = pCallHandler->ThreadCallProc(
                    *this,
                    SMF_GET_CALL_ID(rMsg.wParam),
                    reinterpret_cast<void *>(rMsg.lParam),
                    SMF_GET_ARGS_SIZE(rMsg.wParam)
                );
            }
		}
		break;

	default:
		{
            nErrorCode = SMF_ERR_NOT_PROCESSED;
            if (rMsg.nMsgId >= MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE) {
                QueueId nQueueId = SMF_GET_EQ_QUEUE_ID(rMsg.wParam);
                SMF_ThreadEventQueueHandler *pThreadEventQueueHandler = GetEventQueueHandler(nQueueId);
                if (pThreadEventQueueHandler != NULL) {
                    EventData oEventData(
                        SMF_GET_EQ_ENGINE_ID(rMsg.wParam),
                        SMF_GET_EQ_SENDER_ID(rMsg.wParam),
                        rMsg.nMsgId - MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE,
                        reinterpret_cast<void *>(rMsg.lParam)
                    );
                    nErrorCode = pThreadEventQueueHandler->ThreadEventQueueProc(*this, nQueueId, oEventData);
                }
            }
		}
		break;
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
        m_pPlatformMsgQ = new SMF_PlatformMsgQ((m_sThreadName + "_MsgQ").c_str());
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
        m_pPlatformTimer = new SMF_PlatformTimer((m_sThreadName + "_Timer").c_str());
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

SMF_ErrorCode SMF_BaseThread::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nCallId) {
    case CALL_ID_SYS_START_TIMER:
        {
            if (pArgs != 0 && nArgsSize == sizeof(CallArgs)) {
                CallArgs *pCallArgs = reinterpret_cast<CallArgs *>(pArgs);
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

            if (pArgs != 0 && nArgsSize == sizeof(CallArgs)) {
                CallArgs *pCallArgs = reinterpret_cast<CallArgs *>(pArgs);
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

SMF_ErrorCode SMF_ExampleWorkThread::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    CallProc pCallProc = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        if (pArgs == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        } else if (sizeof(CallArgs) != nArgsSize) {
            nErrorCode = SMF_ERR_DATA_SIZE_ERROR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pCallProc = LookupCallProc(nCallId);
        if (pCallProc == NULL) {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        CallArgs &rCallArgs = *reinterpret_cast<CallArgs*>(pArgs);
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

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call002(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call003(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnCall_Call004(CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnTimer_Timer1()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleWorkThread::OnTimer_Timer2()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

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
        m_oCallThread.AsyncCall(SMF_ExampleWorkThread::CALL_ID_Call001, &ca, sizeof(SMF_ExampleWorkThread::CallArgs));
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

SMF_ErrorCode SMF_ExampleThreadHost::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    CallProc pCallProc = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        if (pArgs == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        } else if (sizeof(CallArgs) != nArgsSize) {
            nErrorCode = SMF_ERR_DATA_SIZE_ERROR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pCallProc = LookupCallProc(rThread, nCallId);
        if (pCallProc == NULL) {
            nErrorCode = SMF_ERR_NOT_PROCESSED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        CallArgs &rCallArgs = *reinterpret_cast<CallArgs*>(pArgs);
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

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call002(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call003(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnCall_Call004(SMF_BaseThread &rThread, CallArgs &rCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnTimer_Timer1(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleThreadHost::OnTimer_Timer2(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

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
        m_oCallThread.AsyncCall(CALL_ID_Call001, &ca, sizeof(CallArgs));
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

        SMF_FUNC_LOG();

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

    SMF_FUNC_LOG();

    return nErrorCode;
}

SMF_ErrorCode SMF_ExampleTimerThread::OnTimer_Timer002()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_FUNC_LOG();

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

        SMF_FUNC_LOG();

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
