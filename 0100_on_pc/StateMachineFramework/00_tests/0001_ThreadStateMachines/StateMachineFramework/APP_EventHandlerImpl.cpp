#include "SMF_RpcTunnel.h"
#include "RpcDef.h"
#include "APP_afx.hpp"
#include "APP_States.hpp"
#include "APP_Engines.hpp"
#include "APP_EventHandler.hpp"
#include "APP_EventHandlerImpl.hpp"
#include "USB_afx.hpp"

using namespace APP;

extern SMF_BaseThread &GetMainStateMachineThread();
extern SMF_BaseEventQueue &GetMainEventQueue();
extern SMF_BaseEngine &GetMainEngine();
extern SMF_BaseEngine &GetMainEngine21();
extern SMF_BaseEngine &GetMainEngine22();

APP_EventHandlerImpl::APP_EventHandlerImpl(size_t nIndex)
    : APP_EventHandler(nIndex)
    , nSleepValue(1)
    , nAwakeValue(2)
    , m_oCallThread("WorkThread")
    , m_oTimerThread("TimerThread")
    , m_oRpc()
    , m_rRpcClientHandler(m_oRpc.GetRpcClientHandler())
    , m_bTimer1Running(false)
{
    m_oCallThread.SetCallHandler(this);
    m_oCallThread.Start();
    m_oTimerThread.SetTimerHandler(this);
    m_oTimerThread.Start();

    //
    // NOTE: When an engine is to be attached to an event queue, and the event queue has been attached 
    //       to a thread, then the thread will be set as the timer thread for the engine. Otherwise, 
    //       the timer thread for the engine will be NULL, and it should be set manually.
    //
    //GetEngine().SetTimerThread(&GetThread()); // NOTE: All of the engines attached to a thread share it as the timer thread.
}

APP_EventHandlerImpl::~APP_EventHandlerImpl()
{

}

class APP_EventHandleFactory
{
private:
    static SMF_MACHINE_WORD_TYPE arrObjBuff[APP_INSTANCE_COUNT][SMF_ALIGNED_SIZE(APP_EventHandlerImpl) / sizeof(SMF_MACHINE_WORD_TYPE)];

public:
    static APP_EventHandlerImpl *NewObject(size_t nIndex) {
        if (nIndex >= APP_INSTANCE_COUNT) {
            nIndex = 0;
        }
        return new(arrObjBuff[nIndex]) APP_EventHandlerImpl(nIndex);
    }
    static void DeleteObject(APP_EventHandlerImpl *pObject) {
        if (pObject != NULL) {
            pObject->~APP_EventHandlerImpl();
            pObject->operator delete(pObject, pObject);
        }
    }
};
SMF_MACHINE_WORD_TYPE APP_EventHandleFactory::arrObjBuff[APP_INSTANCE_COUNT][SMF_ALIGNED_SIZE(APP_EventHandlerImpl) / sizeof(SMF_MACHINE_WORD_TYPE)];

APP_EventHandlerImpl &APP_EventHandlerImpl::GetInstance(size_t nIndex)
{
    static SMF_AutoReleaseObjectPool<APP_EventHandlerImpl, APP_INSTANCE_COUNT, APP_EventHandleFactory> arop;
    return arop.At(nIndex);
}

SMF_ErrorCode APP_EventHandlerImpl::Reset()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = APP_EventHandler::Reset();
    }

    return nErrorCode;
}

SMF_BaseEngine &APP_EventHandlerImpl::GetEngine()
{
    return GetMainEngine();
}

SMF_BaseEngine &APP_EventHandlerImpl::GetEngine21()
{
    return GetMainEngine21();
}

SMF_BaseEngine &APP_EventHandlerImpl::GetEngine22()
{
    return GetMainEngine22();
}

SMF_BaseThread &APP_EventHandlerImpl::GetThread()
{
    return GetMainStateMachineThread();
}

//
// NOTE: this function is running in the thread m_oTimerThread.
//
SMF_ErrorCode APP_EventHandlerImpl::ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nTimerId) {
    case TIMER_ID_TIMER_001:
        {
            printf("%s(): on TIMER_ID_TIMER_001\n", __FUNCTION__);
        }
        break;
    case TIMER_ID_TIMER_002:
        {
            printf("%s(): on TIMER_ID_TIMER_002\n", __FUNCTION__);
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

//
// NOTE: this function is running in the thread m_oCallThread.
//
SMF_ErrorCode APP_EventHandlerImpl::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nCallId) {
    case CALL_ID_Call001:
        {
            printf("%s(): on CALL_ID_Call001\n", __FUNCTION__);
        }
        break;

    case CALL_ID_Call002:
        {
            printf("%s(): on CALL_ID_Call002\n", __FUNCTION__);
        }
        break;

    case CALL_ID_Call003:
        {
            printf("%s(): on CALL_ID_Call003\n", __FUNCTION__);
        }
        break;

    case CALL_ID_Call004:
        {
            printf("%s(): on CALL_ID_Call004\n", __FUNCTION__);
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

SMF_ErrorCode APP_EventHandlerImpl::OnGotoAwake(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (SMF_STATE_NAME_EQUAL_TO(rOpArg.rSrcState, Sleep01, GetIndex())) {
        nAwakeValue = 1;
    } else if (SMF_STATE_NAME_EQUAL_TO(rOpArg.rSrcState, Sleep02, GetIndex())) {
        nAwakeValue = 2;
    } else if (SMF_STATE_NAME_EQUAL_TO(rOpArg.rSrcState, Sleep, GetIndex())) {
        // do nothing
    }

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnGotoSleep(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nSleepValue = 2;

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnGotoAwake02ByDefault(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_MSG("INFO: %s()\n", __FUNCTION__);

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnPostEvtToSM2(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (rOpArg.rSrcState.IsInState(&SMF_GET_STATE(Sleep, GetIndex()))) {
        GetEngine21().PostEvent(USB::SMF_D_EVENT_ID(GotoHighSpeed));
    }

    if (rOpArg.rSrcState.IsInState(&SMF_GET_STATE(Awake, GetIndex()))) {
        GetEngine22().PostEvent(USB::SMF_D_EVENT_ID(GotoHighSpeed));
    }

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnRpc1(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    unsigned char arrInBuf[6];
    arrInBuf[0] = '0';
    arrInBuf[1] = '1';
    arrInBuf[2] = '2';
    arrInBuf[3] = '3';
    arrInBuf[4] = '4';
    arrInBuf[5] = '\0';
    unsigned char arrOutBuf[10];
    SMF_BaseRpc::RpcCallArgs oRpcCallArgs(arrInBuf, sizeof(arrInBuf), arrOutBuf, sizeof(arrOutBuf));
    nErrorCode = m_rRpcClientHandler.CallRpc(RPC_HOST_001, RPC_HOST_001_IOCTL_001, oRpcCallArgs);
    if (nErrorCode == SMF_ERR_OK) {
        std::string sOutBuf(reinterpret_cast<char *>(oRpcCallArgs.pOutBuf), oRpcCallArgs.nOutBufRetSize);
        SMF_LOG_MSG("RPC result: nRet=%d, nOutBufRetSize=%u, pOutBuf=\"%s\"\n", oRpcCallArgs.nRet, oRpcCallArgs.nOutBufRetSize, sOutBuf.c_str());
    }

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnThreadCall1(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    CallArgs ca;
    ca.unInputArgs.stCall001.nArg1 = 10;
    m_oCallThread.AsyncCall(CALL_ID_Call001, &ca, sizeof(CallArgs));

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnThreadCall2(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    CallArgs ca;
    ca.unInputArgs.stCall001.nArg1 = 20;
    m_oCallThread.SyncCall(CALL_ID_Call002, &ca, sizeof(CallArgs));

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnThreadTimer1(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_bTimer1Running) {
        m_oTimerThread.StopTimer(TIMER_ID_TIMER_001);
        m_bTimer1Running = false;
    } else {
        m_oTimerThread.StartTimer(TIMER_ID_TIMER_001, 10000, true);
        m_bTimer1Running = true;
    }

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnThreadTimer2(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    m_oTimerThread.StartTimer(TIMER_ID_TIMER_002, 2000, false);

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnThreadTimer3(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    GetEngine().StartTimer(TIMER_ID_GOTO_AWAKE01_AFTER_5S, 5000, false, SMF_D_EVENT_ID(GotoAwake01));

    return nErrorCode;
}
