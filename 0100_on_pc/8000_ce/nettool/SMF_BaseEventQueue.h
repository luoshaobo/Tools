#ifndef SMF_BASE_EVENT_QUEUE_H__37265788932780921783876578389784369887327842387
#define SMF_BASE_EVENT_QUEUE_H__37265788932780921783876578389784369887327842387

#include "SMF_afx.h"

class SMF_BaseThread;

class SMF_BaseEventQueue : private SMF_NonCopyable, public SMF_ThreadEventQueueHandler
{
private:
    struct EngineEventHandlerInfo {
        EngineEventHandlerInfo(SMF_EngineEventHandler *a_pEngineEventHandler, bool a_bStarted)
            : pEngineEventHandler(a_pEngineEventHandler), bStarted(a_bStarted) {}

        SMF_EngineEventHandler *pEngineEventHandler;
        bool bStarted;
    };
    typedef std::map<EngineId, EngineEventHandlerInfo> EngineEventHandlerMap;

public:
    SMF_BaseEventQueue(const std::string &sEventQueueName);
    virtual ~SMF_BaseEventQueue();
    
public:
    SMF_ErrorCode AttachToThread(SMF_BaseThread &rThread, QueueId nQueueId);
    SMF_ErrorCode DetachFromThread();

    SMF_ErrorCode Start(EngineId nEngineId);
    SMF_ErrorCode Stop(EngineId nEngineId);
    
    SMF_ErrorCode PostEvent(EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData);
    SMF_ErrorCode SendEvent(EngineId nEngineId, SenderId nSenderId, EventId nEventId, void *pData);
    
public:
    SMF_ErrorCode SetEngineEventHandler(EngineId nEngineId, SMF_EngineEventHandler *pEngineEventHandler);
    
public:
    std::string GetEventQueueName() const { return m_sEventQueueName; }
    SMF_BaseThread *GetThread() const { return m_pThread; }
    
private:
    virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData);

private:
    EngineEventHandlerInfo *GetEngineEventHandlerInfo(EngineId nEngineId);
    void Lock();
    void Unlock();
    
private:
    std::string m_sEventQueueName;
    SMF_BaseThread *m_pThread;
    QueueId m_nQueueId;
    EngineEventHandlerMap m_oEngineEventHandlerMap;
};

#endif // #ifndef SMF_BASE_EVENT_QUEUE_H__37265788932780921783876578389784369887327842387
