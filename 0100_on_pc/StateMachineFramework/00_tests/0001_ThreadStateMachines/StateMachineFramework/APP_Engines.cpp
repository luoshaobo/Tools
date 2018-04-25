#include "TK_Tools.h"
#include "APP_afx.hpp"
#include "APP_States.hpp"
#include "APP_Engines.hpp"
#include "APP_EventHandler.hpp"
#include "APP_EventHandlerImpl.hpp"

#define APP_EVENT_MAP_ITEM(event_name)  { #event_name, SMF_D_EVENT_ID(event_name) }

namespace APP {

static struct EventMap {
    const char *pEventName;
    SMF_EventId nEventId;
} g_arrEventMap[] = {
    APP_EVENT_MAP_ITEM(_INVALID),
    APP_EVENT_MAP_ITEM(_GOTO_SELF),
    APP_EVENT_MAP_ITEM(_GOTO_PARENT),
    APP_EVENT_MAP_ITEM(_GOTO_DEF_CHILD),
    APP_EVENT_MAP_ITEM(_GOTO_FIRST_CHILD),
    APP_EVENT_MAP_ITEM(_ENTRY),
    APP_EVENT_MAP_ITEM(_EXIT),

    APP_EVENT_MAP_ITEM(Entry),
    APP_EVENT_MAP_ITEM(Evt01),
    APP_EVENT_MAP_ITEM(Evt01_01),
    APP_EVENT_MAP_ITEM(Evt01_02),
    APP_EVENT_MAP_ITEM(Evt02),
    APP_EVENT_MAP_ITEM(Evt02_01),
    APP_EVENT_MAP_ITEM(Evt02_02),
    APP_EVENT_MAP_ITEM(Evt11),
    APP_EVENT_MAP_ITEM(Evt12),
    APP_EVENT_MAP_ITEM(Exit),
    APP_EVENT_MAP_ITEM(GotoAwake),
    APP_EVENT_MAP_ITEM(GotoAwake01),
    APP_EVENT_MAP_ITEM(GotoAwake02),
    APP_EVENT_MAP_ITEM(GotoAwake2),
    APP_EVENT_MAP_ITEM(GotoSleep),
    APP_EVENT_MAP_ITEM(GotoSleep01),
    APP_EVENT_MAP_ITEM(GotoSleep02),
    APP_EVENT_MAP_ITEM(GotoSleep03),
    APP_EVENT_MAP_ITEM(GotoSleep2),
    APP_EVENT_MAP_ITEM(GotoSleep3),
    APP_EVENT_MAP_ITEM(InternalEvt01),
    APP_EVENT_MAP_ITEM(InternalEvt02),
    APP_EVENT_MAP_ITEM(InternalEvt03),
    APP_EVENT_MAP_ITEM(InternalEvt04),
    APP_EVENT_MAP_ITEM(InternalEvt05),
    APP_EVENT_MAP_ITEM(InternalEvt06),
    APP_EVENT_MAP_ITEM(InternalEvt07),
    APP_EVENT_MAP_ITEM(InternalEvt08),
    APP_EVENT_MAP_ITEM(InternalEvt09),
    APP_EVENT_MAP_ITEM(InternalEvt10),
    APP_EVENT_MAP_ITEM(InternalEvt11),
    APP_EVENT_MAP_ITEM(PostEvtToSM2),
    APP_EVENT_MAP_ITEM(Rpc1),
    APP_EVENT_MAP_ITEM(ThreadCall1),
    APP_EVENT_MAP_ITEM(ThreadCall2),
    APP_EVENT_MAP_ITEM(ThreadTimer1),
    APP_EVENT_MAP_ITEM(ThreadTimer2),
    APP_EVENT_MAP_ITEM(ThreadTimer3),
};

SMF_ErrorCode Engine_MainEngine::OnInitStateMachine()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_ADD_STATE_TO_PARENT_STATE(Root, _InternalRoot_, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Sleep, Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Sleep01, Sleep, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Sleep02, Sleep, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Awake, Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Awake02, Awake, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Awake01, Awake, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Awake01_01, Awake01, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Awake02_01, Awake02, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(Sleep03, Sleep, GetIndex());

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
    APP_EventHandler &rEventHandler = APP_EventHandler::GetInstance(GetIndex());

    nErrorCode = rEventHandler.PreProcessEvent(rEventInfo, bProcessed);

    return nErrorCode;
}

SMF_ErrorCode Engine_MainEngine::PostProcessEvent(const SMF_EventInfo &rEventInfo, bool bProcessed)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    APP_EventHandler &rEventHandler = APP_EventHandler::GetInstance(GetIndex());

    nErrorCode = rEventHandler.PostProcessEvent(rEventInfo, bProcessed);

    return nErrorCode;
}

SMF_ErrorCode Engine_MainEngine::OnUnhandledEvent(const SMF_EventInfo &rEventInfo)
{
    SMF_LOG_ENGINE();

    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    APP_EventHandler &rEventHandler = APP_EventHandler::GetInstance(GetIndex());

    nErrorCode = rEventHandler.OnUnhandledEvent(rEventInfo);

    return nErrorCode;
}

} // namespace APP {
