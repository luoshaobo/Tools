#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventQueue.h"
#include "SMF_BaseThread.h"
#include "TK_Tools.h"

SMF_DEFINE_ENGINE(null)

SMF_BaseEngine::SMF_BaseEngine(const std::string &sEngineName)
    : SMF_NonCopyable()
    , m_oLock()
    , m_sEngineName(sEngineName)
    , m_pStateMachine(NULL)
    , m_pCurrentState(NULL)
    , m_nEngineFeatures(SMF_EF_DEFAULT)
    , m_pBaseEventQueue(NULL)
    , m_nEngineId(0)
    , m_oTimerMap()
    , m_pTimerThread(NULL)
{

}

SMF_BaseEngine::~SMF_BaseEngine()
{

}

SMF_ErrorCode SMF_BaseEngine::HandleEventImpl(const SMF_EventInfo &rEventInfo)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    do {
        if (rEventInfo.nEventId == SMF_D_EVENT_ID(_ENTRY)) {
            SMF_LOG_WARING("### Waring: Event ENTRY can't not be posted, and it will be ignored!\n");
            break;
        } else if (rEventInfo.nEventId == SMF_D_EVENT_ID(_EXIT)) {
            SMF_LOG_WARING("### Waring: Event EXIT can't not be posted, and it will be ignored!\n");
            break;
        }

        nErrorCode = ProcessEvent(rEventInfo);
        if (nErrorCode == SMF_ERR_OK) {
            break;
        } else if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
            SMF_LOG_WARING("### Waring: [CurrentState=%s] Event[%s] is not processed!\n",
                m_pCurrentState == NULL ? "(Unknown State)" : m_pCurrentState->GetStateName().c_str(),
                GetEventNameById(rEventInfo.nEventId).c_str()
            );
            break;
        } else if (nErrorCode == SMF_ERR_NULL_PARENT_STATE) {
            SMF_LOG_WARING("### Waring: [CurrentState=%s] [Event=%s] parent state doesn't exist!\n",
                m_pCurrentState == NULL ? "(Unknown State)" : m_pCurrentState->GetStateName().c_str(),
                GetEventNameById(rEventInfo.nEventId).c_str()
            );
            break;
        } else if (nErrorCode == SMF_ERR_NULL_DEF_CHILD_STATE) {
            SMF_LOG_WARING("### Waring: [CurrentState=%s] [Event=%s] default child doesn't exist!\n",
                m_pCurrentState == NULL ? "(Unknown State)" : m_pCurrentState->GetStateName().c_str(),
                GetEventNameById(rEventInfo.nEventId).c_str()
            );
            break;
        } else if (nErrorCode == SMF_ERR_NO_CHILD_STATE) {
            SMF_LOG_WARING("### Waring: [CurrentState=%s] [Event=%s] first child doesn't exist!\n",
                m_pCurrentState == NULL ? "(Unknown State)" : m_pCurrentState->GetStateName().c_str(),
                GetEventNameById(rEventInfo.nEventId).c_str()
            );
            break;
        } else {
            SMF_LOG_ERROR("*** Error: [CurrentState=%s] %s() is exited for unhandled event[%s]!\n",
                m_pCurrentState == NULL ? "(Unknown State)" : m_pCurrentState->GetStateName().c_str(),
                __FUNCTION__,
                GetEventNameById(rEventInfo.nEventId).c_str()
            );
            nErrorCode = SMF_ERR_UNHANDLED_EVENT;
            break;
        }
    } while (false);

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ProcessEvent_INVALID(const SMF_EventInfo &rEventInfo) const
{
    SMF_ErrorCode nErrorCode = SMF_ERR_FAILED;
    SMF_BaseState *pParentState;
    SMF_BaseState *pDefChildState;
    //SMF_BaseState *pDestState = NULL;
    //SMF_BaseState *pLastState = NULL;

    pParentState = m_pCurrentState->GetParentState();
    pDefChildState = m_pCurrentState->GetDefChildState();

    nErrorCode = SMF_ERR_INVALID_EVENT_ID;

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ProcessEvent_GOTO_SELF(const SMF_EventInfo &rEventInfo) const
{
    SMF_ErrorCode nErrorCode = SMF_ERR_FAILED;
    SMF_BaseState *pParentState;
    SMF_BaseState *pDefChildState;
    SMF_BaseState *pDestState = NULL;
    //SMF_BaseState *pLastState = NULL;

    pParentState = m_pCurrentState->GetParentState();
    pDefChildState = m_pCurrentState->GetDefChildState();

    nErrorCode = m_pCurrentState->ProcessEvent(SMF_D_EVENT_ID(_GOTO_SELF), rEventInfo.pData, rEventInfo.nSenderId, &pDestState);
    if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
        nErrorCode = SMF_ERR_OK;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ProcessEvent_GOTO_PARENT(const SMF_EventInfo &rEventInfo)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_FAILED;
    SMF_BaseState *pParentState;
    SMF_BaseState *pDefChildState;
    SMF_BaseState *pDestState = NULL;
    SMF_BaseState *pLastState = NULL;

    pParentState = m_pCurrentState->GetParentState();
    pDefChildState = m_pCurrentState->GetDefChildState();

    pLastState = m_pCurrentState;
    nErrorCode = pLastState->Exit(rEventInfo.nSenderId);
    if (nErrorCode == SMF_ERR_OK) {
        if (pParentState != NULL) {
            nErrorCode = m_pCurrentState->ProcessEvent(SMF_D_EVENT_ID(_GOTO_PARENT), rEventInfo.pData, rEventInfo.nSenderId, &pDestState);
            if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
                nErrorCode = SMF_ERR_OK;
            }
            if (nErrorCode == SMF_ERR_OK) {
                m_pCurrentState = pParentState;
            }
        } else {
            nErrorCode = SMF_ERR_NULL_PARENT_STATE;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ProcessEvent_GOTO_DEF_CHILD(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_FAILED;
    SMF_BaseState *pParentState;
    SMF_BaseState *pDefChildState;
    SMF_BaseState *pDestState = NULL;
    SMF_BaseState *pLastState = NULL;

    pParentState = m_pCurrentState->GetParentState();
    pDefChildState = m_pCurrentState->GetDefChildState();

    nErrorCode = m_pCurrentState->ProcessEvent(SMF_D_EVENT_ID(_GOTO_DEF_CHILD), rEventInfo.pData, rEventInfo.nSenderId, &pDestState);
    if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
        nErrorCode = SMF_ERR_OK;
    }

    if (nErrorCode == SMF_ERR_OK) {
        pLastState = m_pCurrentState;
        if (pDefChildState != NULL) {
            m_pCurrentState = pDefChildState;
            nErrorCode = m_pCurrentState->Enter(rEventInfo.nSenderId);
            if (nErrorCode == SMF_ERR_OK) {
                bCheckIfEnterDefChildState = true;
            }
        } else {
            nErrorCode = SMF_ERR_NULL_DEF_CHILD_STATE;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ProcessEvent_GOTO_FIRST_CHILD(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_FAILED;
    SMF_BaseState *pParentState;
    SMF_BaseState *pDefChildState;
    SMF_BaseState *pDestState = NULL;
    SMF_BaseState *pLastState = NULL;

    pParentState = m_pCurrentState->GetParentState();
    pDefChildState = m_pCurrentState->GetDefChildState();

    nErrorCode = m_pCurrentState->ProcessEvent(SMF_D_EVENT_ID(_GOTO_FIRST_CHILD), rEventInfo.pData, rEventInfo.nSenderId, &pDestState);
    if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
        nErrorCode = SMF_ERR_OK;
    }

    if (nErrorCode == SMF_ERR_OK) {
        pLastState = m_pCurrentState;
        if (pLastState->GetChildStates().size() >= 1) {
            m_pCurrentState = pLastState->GetChildStates()[0];
            nErrorCode = m_pCurrentState->Enter(rEventInfo.nSenderId);
            if (nErrorCode == SMF_ERR_OK) {
                bCheckIfEnterDefChildState = true;
            }
        } else {
            nErrorCode = SMF_ERR_NO_CHILD_STATE;
        }
    }

    return nErrorCode;
}

//
// NOTE: (on SMF_EF_PROCESS_EVENT_IN_PARENT_STATE is set)
// 1) Event handling priorities are as below:
//        m_pCurrentState
//        m_pCurrentState->GetParentState()
//        m_pCurrentState->GetParentState()->GetParentState()
//        m_pCurrentState->GetParentState()->GetParentState()->GetParentState()
//        ...
// 2) Once handled, the handling should be stopped.
//
SMF_ErrorCode SMF_BaseEngine::ProcessEvent_Others(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState, bool &bNeedEventProcessedByParent)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_FAILED;
    SMF_BaseState *pParentState;
    SMF_BaseState *pDefChildState;
    SMF_BaseState *pDestState = NULL;
    SMF_BaseState *pLastState = NULL;

    pParentState = m_pCurrentState->GetParentState();
    pDefChildState = m_pCurrentState->GetDefChildState();

    SMF_BaseState *pNewCurrentState = m_pCurrentState;
    if (m_nEngineFeatures & SMF_EF_PROCESS_EVENT_IN_PARENT_STATE) {
        SMF_TransItem oTransItem;
        while (pNewCurrentState != NULL) {
            nErrorCode = pNewCurrentState->GetTransItemByEventId(rEventInfo.nEventId, oTransItem);
            if (nErrorCode == SMF_ERR_OK) {
                break;
            } else if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
                pNewCurrentState = pNewCurrentState->GetParentState();
                continue;
            } else {
                break;
            }
        }
        if (nErrorCode == SMF_ERR_OK) {
            if (pNewCurrentState != NULL) {
                for (SMF_BaseState *pbs = m_pCurrentState; pbs != pNewCurrentState; pbs = pbs->GetParentState()) {
                    nErrorCode = pbs->Exit(rEventInfo.nSenderId);
                    if (nErrorCode != SMF_ERR_OK) {
                        break;
                    }
                }
            }
        }
    } else {
        nErrorCode = SMF_ERR_OK;
    }
    if (nErrorCode == SMF_ERR_OK && pNewCurrentState != NULL) {
        pLastState = m_pCurrentState;
        m_pCurrentState = pNewCurrentState;
        pDefChildState = m_pCurrentState->GetDefChildState();

        nErrorCode = m_pCurrentState->ProcessEvent(rEventInfo.nEventId, rEventInfo.pData, rEventInfo.nSenderId, &pDestState);
        if (nErrorCode == SMF_ERR_OK) {
            if (pDestState != NULL) {
                pLastState = m_pCurrentState;
                m_pCurrentState = pDestState;

                if (pDestState != NULL && pLastState != NULL) {
                    nErrorCode = ProcessEvent_Others_PostProcessEvent(rEventInfo, *pDestState, *pLastState, bCheckIfEnterDefChildState, bNeedEventProcessedByParent);
                } else {
                    nErrorCode = SMF_ERR_NULL_PTR;
                }
            } else {
                nErrorCode = SMF_ERR_NULL_DEST_STATE;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ProcessEvent_Others_PostProcessEvent(const SMF_EventInfo &rEventInfo, SMF_BaseState &oDestState, SMF_BaseState &oLastState, bool &bCheckIfEnterDefChildState, bool &bNeedEventProcessedByParent)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_StateRelationship rs = oDestState.RelationshipTo(&oLastState);
    switch (rs) {
    case SMF_SR_SELF:
        {
            // do nothing
        }
        break;
    case SMF_SR_PARENT:
        {
            nErrorCode = oLastState.Exit(rEventInfo.nSenderId);
            if (nErrorCode == SMF_ERR_OK) {
                if (!(m_nEngineFeatures & SMF_EF_PROCESS_EVENT_IN_PARENT_STATE)) {
                    bNeedEventProcessedByParent = true;
                }
            }
        }
        break;
    case SMF_SR_CHILD:
        {
            nErrorCode = m_pCurrentState->Enter(rEventInfo.nSenderId);
            if (nErrorCode == SMF_ERR_OK) {
                bCheckIfEnterDefChildState = true;
            }
        }
        break;
    case SMF_SR_BROTHER:
        {
            nErrorCode = oLastState.Exit(rEventInfo.nSenderId);
            if (nErrorCode == SMF_ERR_OK) {
                nErrorCode = m_pCurrentState->Enter(rEventInfo.nSenderId);
                if (nErrorCode == SMF_ERR_OK) {
                    bCheckIfEnterDefChildState = true;
                }
            }
        }
        break;
    case SMF_SR_OHTER:
        {
            nErrorCode = SMF_ERR_NOT_SUPPORTED;
        }
        break;
    case SMF_SR_INVALID:
    default:
        {
            nErrorCode = SMF_ERR_INVALID_STATE_RELATIONSHIP;
        }
        break;
    }

    return nErrorCode;
}

//
// NOTE:
// 1) On a state being entered, if the state has a default child state, the default child state should be entered.
//
SMF_ErrorCode SMF_BaseEngine::ProcessEvent_EnterDefChildDeeply(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_BaseState *pDefChildState = NULL;
    SMF_BaseState *pDestState = NULL;

    while (nErrorCode == SMF_ERR_OK && bCheckIfEnterDefChildState) {
        bCheckIfEnterDefChildState = false;
        pDefChildState = m_pCurrentState->GetDefChildState();
        if (pDefChildState != NULL) {
            nErrorCode = m_pCurrentState->ProcessEvent(SMF_D_EVENT_ID(_GOTO_DEF_CHILD), rEventInfo.pData, rEventInfo.nSenderId, &pDestState);
            if (nErrorCode == SMF_ERR_NOT_PROCESSED) {
                nErrorCode = SMF_ERR_OK;
            }
            m_pCurrentState = pDefChildState;
            nErrorCode = m_pCurrentState->Enter(rEventInfo.nSenderId);
            if (nErrorCode == SMF_ERR_OK) {
                bCheckIfEnterDefChildState = true;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::HandleEvent(SMF_SenderId nSenderId, SMF_EventId nEventId, void *pData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        SMF_EventInfo oEventInfo(nEventId, pData, nSenderId);
        nErrorCode = HandleEventImpl(oEventInfo);
    }

    return nErrorCode;
}

//
// NOTE: (on SMF_EF_PROCESS_EVENT_IN_PARENT_STATE is not set)
// 1) Event handling priorities are as below:
//        m_pCurrentState
//        m_pCurrentState->GetParentState()
//        m_pCurrentState->GetParentState()->GetParentState()
//        m_pCurrentState->GetParentState()->GetParentState()->GetParentState()
//        ...
// 2) The event should be handled by all of the ancestors respectively, until not being handled.
//
SMF_ErrorCode SMF_BaseEngine::ProcessEvent(const SMF_EventInfo &rEventInfo)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bCheckIfEnterDefChildState = false;
    bool bNeedEventProcessedByParent;
    unsigned int i;

    if (m_pCurrentState == NULL) {
        nErrorCode = SMF_ERR_NULL_CURRENT_STATE;
    }

    if (nErrorCode == SMF_ERR_OK) {
        switch (rEventInfo.nEventId) {
        case SMF_D_EVENT_ID(_INVALID):
            {
                nErrorCode = ProcessEvent_INVALID(rEventInfo);
            }
            break;
        case SMF_D_EVENT_ID(_GOTO_SELF):
            {
                nErrorCode = ProcessEvent_GOTO_SELF(rEventInfo);
            }
            break;
        case SMF_D_EVENT_ID(_GOTO_PARENT):
            {
                nErrorCode = ProcessEvent_GOTO_PARENT(rEventInfo);
            }
            break;
        case SMF_D_EVENT_ID(_GOTO_DEF_CHILD):
            {
                nErrorCode = ProcessEvent_GOTO_DEF_CHILD(rEventInfo, bCheckIfEnterDefChildState);
            }
            break;
        case SMF_D_EVENT_ID(_GOTO_FIRST_CHILD):
            {
                nErrorCode = ProcessEvent_GOTO_FIRST_CHILD(rEventInfo, bCheckIfEnterDefChildState);
            }
            break;
        default:
            {
                for (i = 0; ; i++) {
                    bNeedEventProcessedByParent = false;
                    nErrorCode = ProcessEvent_Others(rEventInfo, bCheckIfEnterDefChildState, bNeedEventProcessedByParent);
                    if (nErrorCode != SMF_ERR_OK) {
                        break;
                    }
                    if (!bNeedEventProcessedByParent) {
                        break;
                    }
                }
                if (i > 0 && nErrorCode == SMF_ERR_NOT_PROCESSED) {
                    nErrorCode = SMF_ERR_OK;
                }
            }
            break;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = ProcessEvent_EnterDefChildDeeply(rEventInfo, bCheckIfEnterDefChildState);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::AttachToEventQueue(SMF_BaseEventQueue &rEventQueue, SMF_EngineId nEngineId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBaseEventQueue != NULL) {
            nErrorCode = SMF_ERR_ALREADY_ATTACHED_TO_EVENT_QUEUE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = rEventQueue.SetEngineEventHandler(nEngineId, this);
        if (nErrorCode == SMF_ERR_OK) {
            m_pBaseEventQueue = &rEventQueue;
            m_nEngineId = nEngineId;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::DetachFromEventQueue()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBaseEventQueue != NULL) {
            nErrorCode = m_pBaseEventQueue->SetEngineEventHandler(m_nEngineId, NULL);
            m_pBaseEventQueue = NULL;
            m_nEngineId = 0;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::Start()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = OnInitStateMachine();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBaseEventQueue == NULL) {
            nErrorCode = SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pBaseEventQueue->Start(m_nEngineId);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::Stop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pBaseEventQueue == NULL) {
        nErrorCode = SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE;
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pBaseEventQueue->Stop(m_nEngineId);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = OnDeinitStateMachine();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::PostEvent(SMF_EventId nEventId, void *pData /*= NULL*/, SMF_SenderId nSenderId /*= SMF_SENDER_ID_DEF*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pBaseEventQueue == NULL) {
        nErrorCode = SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE;
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pBaseEventQueue->PostEvent(m_nEngineId, nSenderId, nEventId, pData);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::SendEvent(SMF_EventId nEventId, void *pData /*= NULL*/, SMF_SenderId nSenderId /*= SMF_SENDER_ID_DEF*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pBaseEventQueue == NULL) {
        nErrorCode = SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE;
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pBaseEventQueue->SendEvent(m_nEngineId, nSenderId, nEventId, pData);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::SetStateMachine(SMF_BaseState *pStateMachine)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    do {
        if (pStateMachine == NULL) {
            nErrorCode = SMF_ERR_INVALID_STATE;
            break;
        }

        m_pStateMachine = pStateMachine;
        m_pCurrentState = m_pStateMachine;
    } while (0);

    return nErrorCode;
}

std::string SMF_BaseEngine::GetEventNameById(SMF_EventId nEventId) const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%lu", nEventId);

    return sResult;
}

SMF_BaseThread *SMF_BaseEngine::GetThread() const
{
    SMF_BaseThread *pThread = NULL;

    if (m_pBaseEventQueue != NULL) {
        pThread = m_pBaseEventQueue->GetThread();
    }

    return pThread;
}

SMF_ErrorCode SMF_BaseEngine::StartTimer(TimerId nTimerId, unsigned int nElapse, bool bRepeat, SMF_EventId nEventId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();

        TimerData oTimerData(nEventId, NULL, SMF_SENDER_ID_DEF, bRepeat);
        TimerMap::iterator it = m_oTimerMap.find(nTimerId);
        if (it != m_oTimerMap.end()) {
            (*it).second = oTimerData;
        } else {
            m_oTimerMap.insert(TimerMap::value_type(nTimerId, oTimerData));
        }

        Unlock();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pTimerThread == NULL) {
            nErrorCode = SMF_ERR_ENGINE_TIMER_IS_NULL;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pTimerThread->StartTimer(nTimerId, nElapse, bRepeat);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::StartTimer(TimerId nTimerId, unsigned int nElapse, bool bRepeat, const EventInfo &rEventInfo)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();

        TimerData oTimerData(rEventInfo.nEventId, rEventInfo.pData, rEventInfo.nSenderId, bRepeat);
        TimerMap::iterator it = m_oTimerMap.find(nTimerId);
        if (it != m_oTimerMap.end()) {
            (*it).second = oTimerData;
        } else {
            m_oTimerMap.insert(TimerMap::value_type(nTimerId, oTimerData));
        }

        Unlock();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pTimerThread == NULL) {
            nErrorCode = SMF_ERR_ENGINE_TIMER_IS_NULL;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pTimerThread->StartTimer(nTimerId, nElapse, bRepeat);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();

        TimerMap::iterator it = m_oTimerMap.find(nTimerId);
        if (it != m_oTimerMap.end()) {
            m_oTimerMap.erase(it);
        }

        Unlock();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pTimerThread == NULL) {
            nErrorCode = SMF_ERR_ENGINE_TIMER_IS_NULL;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pTimerThread->StopTimer(nTimerId);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::SetTimerThread(SMF_BaseThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pTimerThread != NULL) {
            nErrorCode = m_pTimerThread->SetTimerHandler(NULL);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_pTimerThread = pThread;
        if (m_pTimerThread != NULL) {
            nErrorCode = m_pTimerThread->SetTimerHandler(this);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEngine::ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    TimerData oTimerData;
    bool bFound = false;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();

        TimerMap::iterator it = m_oTimerMap.find(nTimerId);
        if (it != m_oTimerMap.end()) {
            bFound = true;
            oTimerData = (*it).second;
            if (!oTimerData.bRepeat) {
                m_oTimerMap.erase(it);
            }
        }

        Unlock();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (bFound) {
            if (GetThread() == m_pTimerThread) {
                SendEvent(oTimerData.nEventId, oTimerData.pData, oTimerData.nSenderId);
            } else {
                PostEvent(oTimerData.nEventId, oTimerData.pData, oTimerData.nSenderId);
            }
        }
    }

    return nErrorCode;
}

void SMF_BaseEngine::Lock()
{
    m_oLock.Lock();
}

void SMF_BaseEngine::Unlock()
{
    m_oLock.Unlock();
}
