#include "SMF_BaseEventQueue.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseThread.h"

SMF_BaseEventQueue::SMF_BaseEventQueue(const std::string &sEventQueueName)
    : SMF_NonCopyable()
    , m_sEventQueueName(sEventQueueName)
    , m_pThread(NULL)
    , m_nQueueId(0)
    , m_oEngineEventHandlerMap()
{

}

SMF_BaseEventQueue::~SMF_BaseEventQueue()
{

}

SMF_ErrorCode SMF_BaseEventQueue::AttachToThread(SMF_BaseThread &rThread, QueueId nQueueId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pThread != NULL) {
            nErrorCode = SMF_ERR_ALREADY_ATTACHED_TO_THREAD;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = rThread.SetEventQueueHandler(nQueueId, this);
        if (nErrorCode == SMF_ERR_OK) {
            m_pThread = &rThread;
            m_nQueueId = nQueueId;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::DetachFromThread()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pThread != NULL) {
            nErrorCode = m_pThread->SetEventQueueHandler(m_nQueueId, NULL);
            m_pThread = NULL;
            m_nQueueId = 0;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::Start(EngineId nEngineId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        EngineEventHandlerInfo *pEngineEventHandlerInfo = GetEngineEventHandlerInfo(nEngineId);
        if (pEngineEventHandlerInfo != NULL) {
            pEngineEventHandlerInfo->bStarted = true;
        } else {
            nErrorCode = SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::Stop(EngineId nEngineId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        EngineEventHandlerInfo *pEngineEventHandlerInfo = GetEngineEventHandlerInfo(nEngineId);
        if (pEngineEventHandlerInfo != NULL) {
            pEngineEventHandlerInfo->bStarted = false;
        } else {
            nErrorCode = SMF_ERR_NOT_ATTACHED_TO_EVENT_QUEUE;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::PostEvent(EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pThread == NULL) {
            nErrorCode = SMF_ERR_NOT_ATTACHED_TO_THREAD;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pThread->PostEvent(m_nQueueId, nEngineId, nSenderId, nEventId, pData);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::SendEvent(EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pThread == NULL) {
            nErrorCode = SMF_ERR_NOT_ATTACHED_TO_THREAD;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_pThread->SendEvent(m_nQueueId, nEngineId, nSenderId, nEventId, pData);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = SMF_ERR_NOT_PROCESSED;
        EngineEventHandlerInfo *pEngineEventHandlerInfo = GetEngineEventHandlerInfo(rEventData.nEngineId);
        if (pEngineEventHandlerInfo != NULL && pEngineEventHandlerInfo->bStarted) {
            nErrorCode = pEngineEventHandlerInfo->pEngineEventHandler->HandleEvent(rEventData.nSenderId, rEventData.nEventId, rEventData.pData);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseEventQueue::SetEngineEventHandler(EngineId nEngineId, SMF_EngineEventHandler *pEngineEventHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        EngineEventHandlerMap::iterator it = m_oEngineEventHandlerMap.find(nEngineId);
        if (pEngineEventHandler != NULL) {
            if (it != m_oEngineEventHandlerMap.end()) {
                (*it).second = EngineEventHandlerInfo(pEngineEventHandler, false);
            } else {
                m_oEngineEventHandlerMap.insert(EngineEventHandlerMap::value_type(nEngineId, EngineEventHandlerInfo(pEngineEventHandler, false)));
            }
        } else {
            if (it != m_oEngineEventHandlerMap.end()) {
                m_oEngineEventHandlerMap.erase(it);
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_BaseEventQueue::EngineEventHandlerInfo *SMF_BaseEventQueue::GetEngineEventHandlerInfo(EngineId nEngineId)
{
    EngineEventHandlerInfo *pEngineEventHandlerInfo = NULL;

    Lock();
    EngineEventHandlerMap::iterator it = m_oEngineEventHandlerMap.find(nEngineId);
    if (it != m_oEngineEventHandlerMap.end()) {
        pEngineEventHandlerInfo = &(*it).second;
    }
    Unlock();

    return pEngineEventHandlerInfo;
}

void SMF_BaseEventQueue::Lock()
{
    if (m_pThread != NULL) {
        m_pThread->Lock();
    }
}

void SMF_BaseEventQueue::Unlock()
{
    if (m_pThread != NULL) {
        m_pThread->Unlock();
    }
}
