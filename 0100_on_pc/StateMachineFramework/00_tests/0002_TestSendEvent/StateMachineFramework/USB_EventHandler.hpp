#ifndef USB_EVENTHANDLER_HPP
#define USB_EVENTHANDLER_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventHandler.h"

class USB_EventHandlerImpl;

class USB_EventHandler : public SMF_BaseEventHandler, public SMF_PlacementNewDelete
{
protected:
    USB_EventHandler(size_t nIndex);

public:
    virtual ~USB_EventHandler();
    static USB_EventHandlerImpl &GetInstance(size_t nIndex);

public:
    virtual SMF_ErrorCode Reset() { return SMF_BaseEventHandler::Reset(); }

public:
    //
    // Event handlers
    //
    virtual SMF_ErrorCode PreProcessEvent(const SMF_EventInfo &rEventInfo, bool &bProcessed) { bProcessed = false; return SMF_ERR_OK; }
    virtual SMF_ErrorCode PostProcessEvent(const SMF_EventInfo &rEventInfo, bool bProcessed) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnUnhandledEvent(const SMF_EventInfo &rEventInfo) { return SMF_ERR_OK; }

    virtual SMF_ErrorCode GotoHighSpeed02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode GotoHighSpeed03(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt12(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt21(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEvt22(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoHighSpeed01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoHighSpeed02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoLowSpeed(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoLowSpeed01(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnGotoLowSpeed02(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
};

#endif // #define USB_EVENTHANDLER_HPP
