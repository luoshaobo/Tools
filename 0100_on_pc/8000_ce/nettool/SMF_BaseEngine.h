#ifndef SMF_ENGINE_H__3823849578375495785783905783476347853895893489
#define SMF_ENGINE_H__3823849578375495785783905783476347853895893489

#include "SMF_afx.h"
#include "SMF_PlatformLock.h"

class SMF_BaseState;
class SMF_BaseEventQueue;
class SMF_BaseThread;

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseEngine
//
class SMF_BaseEngine : private SMF_NonCopyable, public SMF_EngineEventHandler, public SMF_ThreadTimerHandler
{
private:
    typedef unsigned int TimerId;
    struct TimerData {
        TimerData() : nEventId(0), pData(NULL), nSenderId(0), bRepeat(false) {}
        TimerData(SMF_EventId a_nEventId, void *a_pData, SMF_SenderId a_nSenderId, bool a_bRepeat)
            : nEventId(a_nEventId), pData(a_pData), nSenderId(a_nSenderId), bRepeat(a_bRepeat) {}

        SMF_EventId nEventId;
        void *pData;
        SMF_SenderId nSenderId;
        bool bRepeat;
    };
    typedef std::map<TimerId, TimerData> TimerMap;

public:
    struct EventInfo {
        EventInfo(SMF_EventId a_nEventId, void *a_pData/* = NULL*/, SMF_SenderId a_nSenderId/* = SMF_SENDER_ID_DEF*/)
            : nEventId(a_nEventId), pData(a_pData), nSenderId(a_nSenderId) {}

        SMF_EventId nEventId;
        void *pData;
        SMF_SenderId nSenderId;
    };

protected:
    SMF_BaseEngine(const std::string &sEngineName, size_t nIndex);

public:
    virtual ~SMF_BaseEngine();

    //
    // operations
    //
    SMF_ErrorCode AttachToEventQueue(SMF_BaseEventQueue &rEventQueue, SMF_EngineId nEngineId);
    SMF_ErrorCode DetachFromEventQueue();

    SMF_ErrorCode Start();
    SMF_ErrorCode Stop();

    SMF_ErrorCode PostEvent(SMF_EventId nEventId, void *pData = NULL, SMF_SenderId nSenderId = SMF_SENDER_ID_DEF);
    SMF_ErrorCode SendEvent(SMF_EventId nEventId, void *pData = NULL, SMF_SenderId nSenderId = SMF_SENDER_ID_DEF);

    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nElapse, bool bRepeat, SMF_EventId nEventId);
    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nElapse, bool bRepeat, const EventInfo &rEventInfo);
    SMF_ErrorCode StopTimer(TimerId nTimerId);
    SMF_ErrorCode StopAllTimers();

    SMF_ErrorCode Reset();

    //
    // setter
    //
    void SetEngineFeature(SMF_EngineFeature nEngineFeatures) { m_nEngineFeatures = nEngineFeatures; }
    SMF_ErrorCode SetTimerThread(SMF_BaseThread *pThread); 

    //
    // getter
    //
    size_t GetIndex() const { return m_nIndex; }
    SMF_EngineFeature GetEngineFeature() const { return m_nEngineFeatures; }
    SMF_BaseState *GetCurrentState() const { return m_pCurrentState; }
    SMF_BaseThread *GetThread() const;

    //
    // interfaces
    //
    virtual SMF_EventId GetEventIdByName(const std::string &sEventName) const { return SMF_D_EVENT_ID(_INVALID); }
    virtual std::string GetEventNameById(SMF_EventId nEventId) const;

    //
    // implemented interfaces
    //
    virtual SMF_ErrorCode HandleEvent(SMF_SenderId nSenderId, SMF_EventId nEventId, void *pData);

protected:
    //
    // setter
    //
    SMF_ErrorCode SetStateMachine(SMF_BaseState *pStateMachine);

    //
    // getter
    //
    SMF_BaseState *GetStateMachine() const { return m_pStateMachine; }

    //
    // interfaces
    //
    virtual SMF_ErrorCode OnInitStateMachine() { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnDeinitStateMachine() { return SMF_ERR_OK; }

    //
    // implementation
    //
private:
    virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId);

    SMF_ErrorCode ProcessEvent(const SMF_EventInfo &rEventInfo);

    SMF_ErrorCode ProcessEvent_INVALID(const SMF_EventInfo &rEventInfo) const;
    SMF_ErrorCode ProcessEvent_GOTO_SELF(const SMF_EventInfo &rEventInfo) const;
    SMF_ErrorCode ProcessEvent_GOTO_PARENT(const SMF_EventInfo &rEventInfo);
    SMF_ErrorCode ProcessEvent_GOTO_DEF_CHILD(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState);
    SMF_ErrorCode ProcessEvent_GOTO_FIRST_CHILD(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState);
    SMF_ErrorCode ProcessEvent_Others(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState, bool &bNeedEventProcessedByParent);
    SMF_ErrorCode ProcessEvent_Others_PostProcessEvent(const SMF_EventInfo &rEventInfo, SMF_BaseState &oDestState, SMF_BaseState &oLastState, bool &bCheckIfEnterDefChildState, bool &bNeedEventProcessedByParent);
    SMF_ErrorCode ProcessEvent_EnterDefChildDeeply(const SMF_EventInfo &rEventInfo, bool &bCheckIfEnterDefChildState);

    SMF_ErrorCode HandleEventImpl(const SMF_EventInfo &rEventInfo);

    void Lock();
    void Unlock();

    //
    // data
    //
private:
    size_t m_nIndex;
    SMF_PlatformLock m_oLock;
    std::string m_sEngineName;
    SMF_BaseState *m_pStateMachine;
    SMF_BaseState *m_pCurrentState;
    SMF_EngineFeature m_nEngineFeatures;
    SMF_BaseEventQueue *m_pBaseEventQueue;
    SMF_EngineId m_nEngineId;
    TimerMap m_oTimerMap;
    SMF_BaseThread *m_pTimerThread;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine_null
//
class Engine_null : public SMF_BaseEngine
{
public:
    Engine_null(const std::string &sEngineName, size_t nIndex) : SMF_BaseEngine(sEngineName, nIndex) {}
};

SMF_DECLARE_ENGINE(null)

#endif // #ifndef SMF_ENGINE_H__3823849578375495785783905783476347853895893489
