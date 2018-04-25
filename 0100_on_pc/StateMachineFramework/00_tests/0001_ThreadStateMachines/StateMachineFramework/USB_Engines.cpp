#include "TK_Tools.h"
#include "USB_afx.hpp"
#include "USB_States.hpp"
#include "USB_Engines.hpp"
#include "USB_EventHandler.hpp"
#include "USB_EventHandlerImpl.hpp"

#define USB_EVENT_MAP_ITEM(event_name)  { #event_name, SMF_D_EVENT_ID(event_name) }

namespace USB {

static struct EventMap {
    const char *pEventName;
    SMF_EventId nEventId;
} g_arrEventMap[] = {
    USB_EVENT_MAP_ITEM(_INVALID),
    USB_EVENT_MAP_ITEM(_GOTO_SELF),
    USB_EVENT_MAP_ITEM(_GOTO_PARENT),
    USB_EVENT_MAP_ITEM(_GOTO_DEF_CHILD),
    USB_EVENT_MAP_ITEM(_GOTO_FIRST_CHILD),
    USB_EVENT_MAP_ITEM(_ENTRY),
    USB_EVENT_MAP_ITEM(_EXIT),

    USB_EVENT_MAP_ITEM(Entry),
    USB_EVENT_MAP_ITEM(Evt01),
    USB_EVENT_MAP_ITEM(Evt02),
    USB_EVENT_MAP_ITEM(Evt11),
    USB_EVENT_MAP_ITEM(Evt12),
    USB_EVENT_MAP_ITEM(Evt21),
    USB_EVENT_MAP_ITEM(Evt22),
    USB_EVENT_MAP_ITEM(Exit),
    USB_EVENT_MAP_ITEM(GotoHighSpeed),
    USB_EVENT_MAP_ITEM(GotoHighSpeed01),
    USB_EVENT_MAP_ITEM(GotoHighSpeed02),
    USB_EVENT_MAP_ITEM(GotoHighSpeed03),
    USB_EVENT_MAP_ITEM(GotoLowSpeed),
    USB_EVENT_MAP_ITEM(GotoLowSpeed01),
    USB_EVENT_MAP_ITEM(GotoLowSpeed02),
};

SMF_ErrorCode Engine_MainEngine::OnInitStateMachine()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_ADD_STATE_TO_PARENT_STATE(Root, _InternalRoot_, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(LowSpeed, Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(LowSpeed01, LowSpeed, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(LowSpeed02, LowSpeed, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(HighSpeed, Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(HighSpeed02, HighSpeed, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(HighSpeed01, HighSpeed, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(HighSpeed03, HighSpeed, GetIndex());

    SMF_SET_STATE_MACHINE_TO_ENGINE(_InternalRoot_, MainEngine, GetIndex());

    return nErrorCode;
}

SMF_ErrorCode Engine_MainEngine::OnDeinitStateMachine()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_EventId Engine_MainEngine::GetEventIdByName(const std::string &sEventName) const
{
    unsigned int i;
    unsigned int nCount;
    SMF_EventId nEventId = SMF_D_EVENT_ID(_INVALID);

    nCount = TK_ARR_LEN(g_arrEventMap);
    for (i = 0; i < nCount; i++) {
        if (TK_Tools::CompareNoCase(g_arrEventMap[i].pEventName, sEventName) == 0) {
            nEventId = g_arrEventMap[i].nEventId;
            break;
        }
    }
    return nEventId;
}

std::string Engine_MainEngine::GetEventNameById(SMF_EventId nEventId) const
{
    std::string sResult;
    unsigned int i;
    unsigned int nCount;

    nCount = TK_ARR_LEN(g_arrEventMap);
    for (i = 0; i < nCount; i++) {
        if (g_arrEventMap[i].nEventId == nEventId) {
            sResult = g_arrEventMap[i].pEventName;
            break;
        }
    }

    if (sResult.empty()) {
        sResult = TK_Tools::FormatStr("%lu", nEventId);
    }

    return sResult;
}

SMF_ErrorCode Engine_MainEngine::PreProcessEvent(const SMF_EventInfo &rEventInfo, bool &bProcessed)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    USB_EventHandler &rEventHandler = USB_EventHandler::GetInstance(GetIndex());

    nErrorCode = rEventHandler.PreProcessEvent(rEventInfo, bProcessed);

    return nErrorCode;
}

SMF_ErrorCode Engine_MainEngine::PostProcessEvent(const SMF_EventInfo &rEventInfo, bool bProcessed)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    USB_EventHandler &rEventHandler = USB_EventHandler::GetInstance(GetIndex());

    nErrorCode = rEventHandler.PostProcessEvent(rEventInfo, bProcessed);

    return nErrorCode;
}

SMF_ErrorCode Engine_MainEngine::OnUnhandledEvent(const SMF_EventInfo &rEventInfo)
{
    SMF_LOG_ENGINE();

    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    USB_EventHandler &rEventHandler = USB_EventHandler::GetInstance(GetIndex());

    nErrorCode = rEventHandler.OnUnhandledEvent(rEventInfo);

    return nErrorCode;
}

} // namespace USB {
