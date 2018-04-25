#ifndef APP_EVENTHANDLERIMPL_HPP
#define APP_EVENTHANDLERIMPL_HPP

#include "SMF_BaseThread.h"
#include "SMF_BaseRpc.h"
#include "SMF_BaseThread.h"
#include "APP_EventHandler.hpp"

struct SMF_OpArg;
class SMF_BaseEngine;

class APP_EventHandlerImpl 
    : public APP_EventHandler
    , public SMF_ThreadCallHandler
    , public SMF_ThreadTimerHandler
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

    enum {
        TIMER_ID_GOTO_AWAKE01_AFTER_5S = TIMER_ID_USER_BASE,
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

public:
    APP_EventHandlerImpl(size_t nIndex);

public:
    virtual ~APP_EventHandlerImpl();
    static APP_EventHandlerImpl &GetInstance(size_t nIndex);

public:
    virtual SMF_ErrorCode Reset();

private:
    virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId);
    virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize);

public:
    //
    // implementation interfaces
    //
    virtual SMF_ErrorCode OnNotHandledEvent(const SMF_EventInfo &rEventInfo) { return SMF_ERR_OK; }

    virtual SMF_ErrorCode OnGotoAwake(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnGotoSleep(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnGotoAwake02ByDefault(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnPostEvtToSM2(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnRpc1(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnThreadCall1(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnThreadCall2(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnThreadTimer1(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnThreadTimer2(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnThreadTimer3(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnOutEvt1(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnInEvt1(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnOutEvt2(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnInEvt2(SMF_OpArg &rOpArg);


public:
    //
    // conditions
    //
    int nSleepValue;
    int nAwakeValue;

private:
    //
    // implementations
    //
    SMF_BaseEngine &GetEngine();
    SMF_BaseEngine &GetEngine21();
    SMF_BaseEngine &GetEngine22();
    SMF_BaseThread &GetThread();

private:
    //
    // data
    //
    SMF_BaseThread m_oCallThread;
    SMF_BaseThread m_oTimerThread;
    SMF_BaseRpc m_oRpc;
    SMF_BaseRpcClientHandler &m_rRpcClientHandler;
    bool m_bTimer1Running;
};

#endif // #define APP_EVENTHANDLERIMPL_HPP
