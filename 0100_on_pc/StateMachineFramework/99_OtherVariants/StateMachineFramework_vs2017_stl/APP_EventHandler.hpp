#ifndef APP_EVENTHANDLER_HPP
#define APP_EVENTHANDLER_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventHandler.h"

class APP_EventHandlerImpl;

class APP_EventHandler : public SMF_BaseEventHandler, public SMF_PlacementNewDelete
{
protected:
    APP_EventHandler(size_t nIndex);

public:
    virtual ~APP_EventHandler();
    static APP_EventHandlerImpl &GetInstance(size_t nIndex);

public:
    virtual SMF_ErrorCode Reset() { return SMF_BaseEventHandler::Reset(); }

public:
    //
    // Event handlers
    //
    virtual SMF_ErrorCode PreProcessEvent(const SMF_EventInfo &rEventInfo, bool &bProcessed) { bProcessed = false; return SMF_ERR_OK; }
    virtual SMF_ErrorCode PostProcessEvent(const SMF_EventInfo &rEventInfo, bool bProcessed) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnUnhandledEvent(const SMF_EventInfo &rEventInfo) { return SMF_ERR_OK; }

    virtual SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt01_01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt01_02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt02_01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt02_02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt12(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoAwake(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoAwake01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoAwake02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoAwake02ByDefault(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoSleep(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoSleep01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoSleep02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoSleep03(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoSleep2(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoSleep3(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInEvt1(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInEvt2(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt03(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt04(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt05(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt06(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt07(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt08(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt09(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt10(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnInternalEvt11(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnOutEvt1(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnOutEvt2(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnPostEvtToSM2(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnRpc1(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnThreadCall1(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnThreadCall2(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnThreadTimer1(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnThreadTimer2(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnThreadTimer3(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
};

#endif // #define APP_EVENTHANDLER_HPP
