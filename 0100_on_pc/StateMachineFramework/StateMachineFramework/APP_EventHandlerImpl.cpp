#include "TK_Tools.h"
#include "SMF_RpcTunnel.h"
#include "RpcDef.h"
#include "APP_afx.hpp"
#include "APP_States.hpp"
#include "APP_Engines.hpp"
#include "APP_EventHandler.hpp"
#include "APP_EventHandlerImpl.hpp"

using namespace APP;

extern SMF_BaseThread &GetMainStateMachineThread();
extern SMF_BaseEventQueue &GetMainEventQueue();
extern SMF_BaseEngine &GetMainEngine();

APP_EventHandlerImpl::APP_EventHandlerImpl(size_t nIndex)
    : APP_EventHandler(nIndex)
    , nSleepValue(1)
    , nAwakeValue(2)
    , m_oCallThread("RemoteThreadCallThread", true)
    , m_oEventThread("RemoteThreadEventThread", true)
    , m_oTimerThread("TimerThread")
    , m_oRpc()
    , m_rRpcClientHandler(m_oRpc.GetRpcClientHandler())
    , m_bTimer1Running(false)
{
    m_oCallThread.SetCallHandler(this);
    m_oCallThread.Start();
    m_oEventThread.SetEventQueueHandler(0, this);
    m_oEventThread.Start();
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
SMF_ErrorCode APP_EventHandlerImpl::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    switch (nCallId) {
    case CALL_ID_Call001:
        {
            printf("%s(): on CALL_ID_Call001\n", __FUNCTION__);

            rThreadCallArgs.nOutBufRetSize = 0;

            if (rThreadCallArgs.pInBuf != NULL && rThreadCallArgs.nInBufSize == sizeof(CallArgs::InputArgs::caCall001)) {
                CallArgs::InputArgs::caCall001 *pInputArgsCall001 = reinterpret_cast<CallArgs::InputArgs::caCall001 *>(rThreadCallArgs.pInBuf);
                printf("%s(): arg1=%d\n", __FUNCTION__, pInputArgsCall001->nArg1);
            }

            if (rThreadCallArgs.pOutBuf != NULL && rThreadCallArgs.nOutBufSize == sizeof(CallArgs::OutputArgs::caCall001)) {
                CallArgs::OutputArgs::caCall001 *pOutputArgsCall001 = reinterpret_cast<CallArgs::OutputArgs::caCall001 *>(rThreadCallArgs.pOutBuf);
                strncpy(pOutputArgsCall001->arrResult, "call001 returned data", sizeof(pOutputArgsCall001->arrResult));
                rThreadCallArgs.nOutBufRetSize = rThreadCallArgs.nOutBufSize;

                std::string sTmp(pOutputArgsCall001->arrResult, rThreadCallArgs.nOutBufRetSize);
                printf("%s(): %s\n", __FUNCTION__, sTmp.c_str());
            }

            rThreadCallArgs.nRet = 0;
        }
        break;

    case CALL_ID_Call002:
        {
            printf("%s(): on CALL_ID_Call002\n", __FUNCTION__);

            rThreadCallArgs.nOutBufRetSize = 0;

            if (rThreadCallArgs.pInBuf != NULL && rThreadCallArgs.nInBufSize == sizeof(CallArgs::InputArgs::caCall002)) {
                CallArgs::InputArgs::caCall002 *pInputArgsCall002 = reinterpret_cast<CallArgs::InputArgs::caCall002 *>(rThreadCallArgs.pInBuf);
                printf("%s(): arg1=%d; arg2=%s\n", __FUNCTION__, pInputArgsCall002->nArg1, pInputArgsCall002->arrArg2);
            }

            if (rThreadCallArgs.pOutBuf != NULL && rThreadCallArgs.nOutBufSize == sizeof(CallArgs::OutputArgs::caCall002)) {
                CallArgs::OutputArgs::caCall002 *pOutputArgsCall002 = reinterpret_cast<CallArgs::OutputArgs::caCall002 *>(rThreadCallArgs.pOutBuf);
                strncpy(pOutputArgsCall002->arrResult, "call002 returned data", sizeof(pOutputArgsCall002->arrResult));
                rThreadCallArgs.nOutBufRetSize = rThreadCallArgs.nOutBufSize;

                std::string sTmp(pOutputArgsCall002->arrResult, rThreadCallArgs.nOutBufRetSize);
                printf("%s(): %s\n", __FUNCTION__, sTmp.c_str());
            }

            rThreadCallArgs.nRet = 0;
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

//
// NOTE: this function is running in the thread m_oEventThread.
//
SMF_ErrorCode APP_EventHandlerImpl::ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    printf("%s(): nQueueId=%u, nEngineId=%u, nSenderId=%u, nEventId=%u\n", __FUNCTION__, 
        nQueueId, rEventData.nEngineId, rEventData.nSenderId, rEventData.nEventId);

    if (rEventData.pThreadEventArgs != NULL) {
        rEventData.pThreadEventArgs->nOutBufRetSize = 0;

        if (rEventData.pThreadEventArgs->pInBuf != NULL && rEventData.pThreadEventArgs->nInBufSize > 0) {
            printf("%s(): buf_in=%s\n", __FUNCTION__, std::string(reinterpret_cast<char *>(rEventData.pThreadEventArgs->pInBuf), rEventData.pThreadEventArgs->nInBufSize).c_str());
        }
        if (rEventData.pThreadEventArgs->pOutBuf != NULL && rEventData.pThreadEventArgs->nOutBufSize > 0) {
            std::string sTmp;
            TK_Tools::FormatStr(sTmp, "[remote_ret] for event_id %u", rEventData.nEventId);
            printf("%s(): %s\n", __FUNCTION__, sTmp.c_str());
            strncpy(reinterpret_cast<char *>(rEventData.pThreadEventArgs->pOutBuf), sTmp.c_str(), rEventData.pThreadEventArgs->nOutBufSize);
            rEventData.pThreadEventArgs->nOutBufRetSize = rEventData.pThreadEventArgs->nOutBufSize;
        }
        rEventData.pThreadEventArgs->nRet = 0;
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

    SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

    CallArgs ca;
    memset(&ca, 0, sizeof(CallArgs));
    ca.unInputArgs.stCall001.nArg1 = 10;

    ThreadCallArgs oThreadCallArgs;
    oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca.unInputArgs.stCall001);
    oThreadCallArgs.nInBufSize = sizeof(ca.unInputArgs.stCall001);
    oThreadCallArgs.pOutBuf = reinterpret_cast<unsigned char *>(&ca.unOutputArgs.stCall001);
    oThreadCallArgs.nOutBufSize = sizeof(ca.unOutputArgs.stCall001);

    m_oCallThread.AsyncCall(CALL_ID_Call001, &oThreadCallArgs);

    SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnThreadCall2(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    std::string sResult;

    SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

    CallArgs ca;
    memset(&ca, 0, sizeof(CallArgs));
    ca.unInputArgs.stCall002.nArg1 = 10;

    strncpy(ca.unInputArgs.stCall002.arrArg2, "arg2 input data", 40);

    ThreadCallArgs oThreadCallArgs;
    oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca.unInputArgs.stCall002);
    oThreadCallArgs.nInBufSize = sizeof(ca.unInputArgs.stCall002);
    oThreadCallArgs.pOutBuf = reinterpret_cast<unsigned char *>(&ca.unOutputArgs.stCall002);
    oThreadCallArgs.nOutBufSize = sizeof(ca.unOutputArgs.stCall002);

    m_oCallThread.SyncCall(CALL_ID_Call002, &oThreadCallArgs);

    if (oThreadCallArgs.nRet == 0 && oThreadCallArgs.nOutBufRetSize == oThreadCallArgs.nOutBufSize) {
        sResult = std::string(ca.unOutputArgs.stCall002.arrResult, sizeof(ca.unOutputArgs.stCall002.arrResult));
        printf("%s(): %s\n", __FUNCTION__, sResult.c_str());
    } else {
        printf("%s(): call is failed!\n", __FUNCTION__);
    }

    SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

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

SMF_ErrorCode APP_EventHandlerImpl::OnOutEvt1(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

    GetEngine().PostEvent(SMF_D_EVENT_ID(InEvt1));

    SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnInEvt1(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

    SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnOutEvt2(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

    GetEngine().SendEvent(SMF_D_EVENT_ID(InEvt1));

    SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

    return nErrorCode;
}

SMF_ErrorCode APP_EventHandlerImpl::OnInEvt2(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

    SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

    return nErrorCode;
}
