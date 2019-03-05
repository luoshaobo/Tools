#ifndef SMF_BASE_THREAD_H__7832873289582395094060349823784834684936054796954
#define SMF_BASE_THREAD_H__7832873289582395094060349823784834684936054796954

#include "SMF_afx.h"
#include "SMF_PlatformThread.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformMsgQ.h"
#include "SMF_PlatformTimer.h"
#include "SMF_PlatformSharedMem.h"
#include "SMF_CyclicBuf.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseThread
//
class SMF_BaseThread
    : private SMF_NonCopyable
    , public SMF_ThreadHandler
    , public SMF_ThreadCallHandler
    , public SMF_ThreadTimerHandler
    , public SMF_ThreadEventQueueHandler
    , public SMF_PlatformThreadHandler
    , public SMF_PlatformTimerHandler
    , public SMF_PlatformMsgQHandler
{
    //
    // constants
    //
public:
    enum {
        CALL_ID_SYS_START_TIMER = CALL_ID_SYS_BASE,
        CALL_ID_SYS_STOP_TIMER,
    };
    // Thread State
    enum {
        TS_INIT                 = 0x00000001,
        TS_CREATING             = 0x00000002,
        TS_CREATED              = 0x00000004,
        TS_ACTIVATING           = 0x00000008,
        TS_ACTIVATED            = 0x00000010,
        TS_DEACTIVING           = 0x00000020,
        TS_DEACTIVED            = 0x00000040,
        TS_DESTRORYING          = 0x00000080,
        TS_DESTRORYED           = 0x00000100,

        TS_ALL_STATES           = SMF_INFINITE
    };

private:
    enum {
        MSG_ID_THREAD_ASYNC_CALL = 0,
        MSG_ID_THREAD_SYNC_CALL,
        MSG_ID_THREAD_EVENT_QUEUE_EVENT_ID_BASE = 100,
    };

    enum {
        REMOTE_SHARED_SEND_MSG_BUF_SIZE = (SMF_GET_EQ_MASK_DATA_SIZE > SMF_GET_CALL_MASK_ARGS_SIZE ? SMF_GET_EQ_MASK_DATA_SIZE : SMF_GET_CALL_MASK_ARGS_SIZE)
    };
    enum {
        REMOTE_SHARED_POST_MSG_BUF_SIZE = REMOTE_SHARED_SEND_MSG_BUF_SIZE * 5
    };
    enum {
        THREAD_MSG_IN_BUF_SIZE_MAX = REMOTE_SHARED_SEND_MSG_BUF_SIZE - (SMF_ALIGNED_SIZE(ThreadCallArgs) > SMF_ALIGNED_SIZE(ThreadEventArgs) ? SMF_ALIGNED_SIZE(ThreadCallArgs) : SMF_ALIGNED_SIZE(ThreadEventArgs)),
        THREAD_MSG_OUT_BUF_SIZE_MAX = THREAD_MSG_IN_BUF_SIZE_MAX,
    };

    //
    // types
    //
public:
    typedef unsigned int ThreadDataId;
    typedef unsigned int TimerId;
    union ThreadData {
        ThreadData() { memset(this, 0, sizeof(ThreadData)); }

        void *pPtr;
        unsigned char nUChar;
        unsigned short nUShort;
        unsigned int nUInt;
        unsigned long nULong;
        unsigned long long nULongLong;
        char nChar;
        short nShort;
        int nInt;
        long nLong;
        long long nLongLong;
    };

private:
    typedef unsigned int ThreadState;
    typedef std::map<ThreadDataId, ThreadData> ThreadDataMap;
    typedef std::map<TimerId, SMF_ThreadTimerHandler *> TimerHandlerMap;
    typedef std::set<TimerId> EnabledTimerSet;
    typedef std::map<CallId, SMF_ThreadCallHandler *> CallHandlerMap;
    typedef std::map<QueueId, SMF_ThreadEventQueueHandler *> EventQueueHandlerMap;
    struct InternalCallArgs {
        union InputArgs {
            struct caStartTimer {
                TimerId nTimerId;
                unsigned int nElapse;
                bool bRepeat;
            } stStartTimer;
            struct caStopTimer {
                TimerId nTimerId;
            } stStopTimer;
        } unInputArgs;
        int nRet;
    };
    struct TimerAtrrib {
        TimerAtrrib() : bRepeat(false) {}
        TimerAtrrib(bool a_bRepeat) : bRepeat(a_bRepeat) {}

        bool bRepeat;
    };
    typedef std::map<TimerId, TimerAtrrib> TimerAttribMap;

    //
    // constructors and destructors
    //
public:
    SMF_BaseThread(const std::string &sThreadName, bool bSupportInterProcessEvent = false, bool bRemoteAgent = false);
    virtual ~SMF_BaseThread();

    //
    // operations
    //
public:
    SMF_ErrorCode Initialize();
    SMF_ErrorCode Finialize();

    SMF_ErrorCode Start();
    SMF_ErrorCode Stop();

    SMF_ErrorCode AsyncCall(CallId nCallId, ThreadCallArgs *pThreadCallArgs = NULL);
    SMF_ErrorCode SyncCall(CallId nCallId, ThreadCallArgs *pThreadCallArgs = NULL);

    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nElapse, bool bRepeat = false);
    SMF_ErrorCode StopTimer(TimerId nTimerId);

    SMF_ErrorCode PostEvent(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs = NULL);
    SMF_ErrorCode PostEvent(EventId nEventId, ThreadEventArgs *pThreadEventArgs = NULL) { return PostEvent(QUEUE_ID_USER_BASE, 0, 0, nEventId, pThreadEventArgs); }
    SMF_ErrorCode SendEvent(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs = NULL);
    SMF_ErrorCode SendEvent(EventId nEventId, ThreadEventArgs *pThreadEventArgs = NULL) { return SendEvent(QUEUE_ID_USER_BASE, 0, 0, nEventId, pThreadEventArgs); }

    SMF_ErrorCode WaitForThreadState(ThreadState nThreadStateSet, unsigned int nTimeout = SMF_INFINITE);

    //
    // setters
    //
public:
    SMF_ErrorCode SetThreadHandler(SMF_ThreadHandler *pThreadHandler);
    SMF_ErrorCode SetCallHandler(SMF_ThreadCallHandler *pCallHandler) { return SetCallHandler(CALL_ID_ALL, pCallHandler); }
    SMF_ErrorCode SetCallHandler(CallId nCallId, SMF_ThreadCallHandler *pCallHandler);
    SMF_ErrorCode SetTimerHandler(SMF_ThreadTimerHandler *pTimerHandler) { return SetTimerHandler(TIMER_ID_ALL, pTimerHandler); }
    SMF_ErrorCode SetTimerHandler(TimerId nTimerId, SMF_ThreadTimerHandler *pTimerHandler);
    SMF_ErrorCode SetEventQueueHandler(QueueId nQueueId, SMF_ThreadEventQueueHandler *pEventQueueHandler);

    SMF_ErrorCode SetThreadState(ThreadState nThreadState);
    SMF_ErrorCode SetThreadData(ThreadDataId nThreadDataId, ThreadData oThreadData);

    //
    // getters
    //
public:
    std::string GetThreadName() const { return m_sThreadName; }
    ThreadState GetThreadState() const { return m_nThreadState; }
    ThreadData GetThreadData(ThreadDataId nThreadDataId);

    //
    // interfaces (can be overridden on subclass)
    //
protected:
    virtual SMF_ErrorCode OnInitialize() { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnFinialize() { return SMF_ERR_OK; }
    virtual SMF_ErrorCode DoStart();
    virtual SMF_ErrorCode DoStop();

    //
    // interface implementations (can be overridden on subclass)
    //
protected:
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);
	virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs);
	virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId);
    virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData);

    //
    // implementations
    //
private:
    SMF_ErrorCode AsyncCall_Local(CallId nCallId, ThreadCallArgs *pThreadCallArgs);
    SMF_ErrorCode SyncCall_Local(CallId nCallId, ThreadCallArgs *pThreadCallArgs);
    SMF_ErrorCode AsyncCall_Remote(CallId nCallId, ThreadCallArgs *pThreadCallArgs);
    SMF_ErrorCode SyncCall_Remote(CallId nCallId, ThreadCallArgs *pThreadCallArgs);
    SMF_ErrorCode SyncCall_Local_InSameThread(CallId nCallId, ThreadCallArgs *pThreadCallArgs);
    SMF_ErrorCode SyncCall_Local_InOtherThread(CallId nCallId, ThreadCallArgs *pThreadCallArgs);

    SMF_ErrorCode PostEvent_Local(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);
    SMF_ErrorCode SendEvent_Local(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);
    SMF_ErrorCode PostEvent_Remote(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);
    SMF_ErrorCode SendEvent_Remote(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);
    SMF_ErrorCode SendEvent_Local_InSameThread(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);
    SMF_ErrorCode SendEvent_Local_InOtherThread(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);

    void SetTimerRepeat(TimerId nTimerId, bool bRepeat);
    bool GetTimerRepeat(TimerId nTimerId);

    SMF_ErrorCode NotSupportedInRemoteAgent();
    SMF_ErrorCode CheckSharedEnvironment();

    SMF_ErrorCode CheckThreadCallArgs(CallId nCallId, ThreadCallArgs *pThreadCallArgs);
    SMF_ErrorCode CheckThreadEventArgs(QueueId nQueueId, EngineId nEngineId, SenderId nSenderId, EventId nEventId, ThreadEventArgs *pThreadEventArgs);

    std::string GetThreadMsgQName() const;
    std::string GetThreadTimerName() const;
    std::string GetRemoteSharedPostMsgBufShmName() const;
    std::string GetRemoteSharedPostMsgBufCyclicBufName() const;
    std::string GetRemoteSharedPostMsgBufLockName() const;
    std::string GetRemoteSharedSendMsgBufShmName() const;
    std::string GetRemoteSharedSendMsgBufLockName() const;
    std::string GetRemotePostMsgWaitEventName() const;

    //
    // interface implementations
    //
private:
    virtual SMF_ErrorCode PlatformThreadProc(SMF_PlatformThread &rPlatformThread);
    virtual SMF_ErrorCode PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId);
    virtual SMF_ErrorCode PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, SMF_PlatformMsgQ::Msg &rMsg);

    //
    // utilities
    //
public:
    void Lock();
    void Unlock();

private:
    SMF_ThreadHandler *GetThreadHandler();
    SMF_ThreadCallHandler *GetCallHandler(CallId nCallId);
    SMF_ThreadTimerHandler *GetTimerHandler(TimerId nTimerId);
    SMF_ThreadEventQueueHandler *GetEventQueueHandler(QueueId nQueueId);

private:
    void SetTimerEnabled(TimerId nTimerId, bool bEnabled);
    bool GetTimerEnabled(TimerId nTimerId);

    //
    // data
    //
private:
    ThreadDataMap m_oThreadDataMap;
    ThreadState m_nThreadState;
    SMF_PlatformEvent m_oThreadStateEvent;
    std::string m_sThreadName;
    bool m_bSupportInterProcessEvent;
    bool m_bRemoteAgent;
    SMF_PlatformMsgQ *m_pPlatformMsgQ;
    SMF_PlatformTimer *m_pPlatformTimer;
    SMF_PlatformThread m_oPlatformThread;
    SMF_PlatformLock m_oLock;
    SMF_PlatformSharedMem *m_pRemoteSharedPostMsgBufShm;
    unsigned char *m_pRemoteSharedPostMsgBufAddr;
    SMF_CyclicBuf *m_pRemoteSharedPostMsgBufCyclicBuf;
    SMF_PlatformLock *m_pRemoteSharedPostMsgBufLock;
    SMF_PlatformSharedMem *m_pRemoteSharedSendMsgBufShm;
    unsigned char *m_pRemoteSharedSendMsgBufAddr;
    SMF_PlatformLock *m_pRemoteSharedSendMsgBufLock;
    SMF_PlatformEvent *m_pRemotePostMsgWaitEvent;
    SMF_ThreadHandler *m_pThreadHandler;
    TimerHandlerMap m_oTimerHandlerMap;
    EnabledTimerSet m_oEnabledTimerSet;
    CallHandlerMap m_oCallHandlerMap;
    EventQueueHandlerMap m_oEventQueueHandlerMap;
    TimerAttribMap m_oTimerAttribMap;
};

//
// Note: 
// The object of this class should be created in another process than in the process in which 
// the real SMF_BaseThread object with the same name is created.
//
class SMF_RemoteThreadAgent : public SMF_BaseThread
{
public:
    SMF_RemoteThreadAgent(const std::string &sRemoteThreadName)
        : SMF_BaseThread(sRemoteThreadName, true, true) {}
};

#ifdef SMF_BASE_THREAD_EXAMPLE_CODE

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleWorkThread
//
class SMF_ExampleWorkThread : public SMF_BaseThread
{
public:
    enum {
        CALL_ID_Call001 = CALL_ID_USER_BASE,
        CALL_ID_Call002,
        CALL_ID_Call003,
        CALL_ID_Call004,
    };
    enum {
        TIMER_ID_TIMER_001 = TIMER_ID_USER_BASE,
        TIMER_ID_TIMER_002,
    };
    struct CallArgs {
        union InputArgs {
            struct caCall001 {
                int nArg1;
            } stCall001;
            struct caCall002{
                int nArg1;
                char *pArg2;
            } stCall002;
            struct caCall003 {
                int nArg1;
                char pArg2[10];
                long nArg3;
            } stCall003;
            struct caCall004 {
                int nArg1;
                char pArg2[10];
                long nArg3;
                short nArg4;
            } stCall004;
        } unInputArgs;
        int nRet;
    };

private:
    typedef SMF_ErrorCode (SMF_ExampleWorkThread::*CallProc)(CallArgs &rCallArgs);

public:
    SMF_ExampleWorkThread(const std::string &sThreadName);
    virtual ~SMF_ExampleWorkThread();

private:
    virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs);
	virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId);

private:
    CallProc LookupCallProc(CallId nCallId);

    SMF_ErrorCode OnCall_Call001(CallArgs &rCallArgs);
    SMF_ErrorCode OnCall_Call002(CallArgs &rCallArgs);
    SMF_ErrorCode OnCall_Call003(CallArgs &rCallArgs);
    SMF_ErrorCode OnCall_Call004(CallArgs &rCallArgs);

    SMF_ErrorCode OnTimer_Timer1();
    SMF_ErrorCode OnTimer_Timer2();
};

class SMF_ExampleWorkThreadHost
{
public:
    SMF_ExampleWorkThreadHost()
        : m_oCallThread("SMF_ExampleWorkThreadHost_CallThread")
        , m_oTimerThread("SMF_ExampleWorkThreadHost_TimerThread")
        {}
    virtual ~SMF_ExampleWorkThreadHost() {}

public:
    SMF_ErrorCode Test();

private:
    SMF_ExampleWorkThread m_oCallThread;
    SMF_ExampleWorkThread m_oTimerThread;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleThreadHost
//
class SMF_ExampleThreadHost : public SMF_ThreadCallHandler, public SMF_ThreadTimerHandler
{
private:
    enum {
        CALL_ID_Call001 = CALL_ID_USER_BASE,
        CALL_ID_Call002,
        CALL_ID_Call003,
        CALL_ID_Call004,
    };
    enum {
        TIMER_ID_TIMER_001 = TIMER_ID_USER_BASE,
        TIMER_ID_TIMER_002,
    };
    struct CallArgs {
        union InputArgs {
            struct caCall001 {
                int nArg1;
            } stCall001;
            struct caCall002{
                int nArg1;
                char *pArg2;
            } stCall002;
            struct caCall003 {
                int nArg1;
                char pArg2[10];
                long nArg3;
            } stCall003;
            struct caCall004 {
                int nArg1;
                char pArg2[10];
                long nArg3;
                short nArg4;
            } stCall004;
        } unInputArgs;
        int nRet;
    };
    typedef SMF_ErrorCode (SMF_ExampleThreadHost::*CallProc)(SMF_BaseThread &rThread, CallArgs &rCallArgs);

public:
    SMF_ExampleThreadHost();
    virtual ~SMF_ExampleThreadHost();

public:
    virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs);
	virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId);

public:
    SMF_ErrorCode Test();

private:
    CallProc LookupCallProc(SMF_BaseThread &rThread, CallId nCallId);

    SMF_ErrorCode OnCall_Call001(SMF_BaseThread &rThread, CallArgs &rCallArgs);
    SMF_ErrorCode OnCall_Call002(SMF_BaseThread &rThread, CallArgs &rCallArgs);
    SMF_ErrorCode OnCall_Call003(SMF_BaseThread &rThread, CallArgs &rCallArgs);
    SMF_ErrorCode OnCall_Call004(SMF_BaseThread &rThread, CallArgs &rCallArgs);

    SMF_ErrorCode OnTimer_Timer1(SMF_BaseThread &rThread);
    SMF_ErrorCode OnTimer_Timer2(SMF_BaseThread &rThread);

private:
    SMF_BaseThread m_oCallThread;
    SMF_BaseThread m_oTimerThread;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleSimpleThreadHost
//
class SMF_ExampleSimpleThreadHost : public SMF_ThreadHandler
{
public:
    SMF_ExampleSimpleThreadHost();
    virtual ~SMF_ExampleSimpleThreadHost();

public:
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);

public:
    SMF_ErrorCode Test();

private:
    SMF_BaseThread m_oWorkThread;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleTimerThread
//
class SMF_ExampleTimerThread : public SMF_BaseThread
{
public:
    enum {
        TIMER_ID_TIMER_001 = TIMER_ID_USER_BASE,
        TIMER_ID_TIMER_002,
    };

public:
    SMF_ExampleTimerThread(const std::string &sThreadName) : SMF_BaseThread(sThreadName) {}
    virtual ~SMF_ExampleTimerThread() {}

public:
    virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId);

private:
    SMF_ErrorCode OnTimer_Timer001();
    SMF_ErrorCode OnTimer_Timer002();
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleNormalThread
//
class SMF_ExampleNormalThread : public SMF_BaseThread
{
public:
    SMF_ExampleNormalThread(const std::string &sThreadName);
    virtual ~SMF_ExampleNormalThread();

private:
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_ExampleEventThreadHost
//
class SMF_ExampleEventThreadHost : public SMF_ThreadEventQueueHandler
{
public:
    enum {
        EVENT_ID_001,
        EVENT_ID_002,
        EVENT_ID_003,
        EVENT_ID_004,
    };

public:
    SMF_ExampleEventThreadHost();
    virtual ~SMF_ExampleEventThreadHost();

private:
    virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData);

private:
    SMF_BaseThread m_oEventThread;
};

#endif // #ifdef SMF_BASE_THREAD_EXAMPLE_CODE

#endif // #ifndef SMF_BASE_THREAD_H__7832873289582395094060349823784834684936054796954
