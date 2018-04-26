#include "NT_afx.hpp"
#include "NT_Engines.hpp"
#include "TK_Tools.h"

#define NT_EVENT_MAP_ITEM(event_name)  { #event_name, SMF_D_EVENT_ID(event_name) }

namespace NT {

static struct EventMap {
    const char *pEventName;
    SMF_EventId nEventId;
} g_arrEventMap[] = {
    NT_EVENT_MAP_ITEM(_INVALID),
    NT_EVENT_MAP_ITEM(_GOTO_SELF),
    NT_EVENT_MAP_ITEM(_GOTO_PARENT),
    NT_EVENT_MAP_ITEM(_GOTO_DEF_CHILD),
    NT_EVENT_MAP_ITEM(_GOTO_FIRST_CHILD),
    NT_EVENT_MAP_ITEM(_ENTRY),
    NT_EVENT_MAP_ITEM(_EXIT),

    NT_EVENT_MAP_ITEM(Entry),
    NT_EVENT_MAP_ITEM(Exit),
    NT_EVENT_MAP_ITEM(ReceivingFailed),
    NT_EVENT_MAP_ITEM(ReceivingOK),
    NT_EVENT_MAP_ITEM(ReceivingTimeout),
    NT_EVENT_MAP_ITEM(SendingFailed),
    NT_EVENT_MAP_ITEM(SendingOK),
    NT_EVENT_MAP_ITEM(SendingTimeout),
    NT_EVENT_MAP_ITEM(SessionEnd),
    NT_EVENT_MAP_ITEM(Type0cSessionBegin),
    NT_EVENT_MAP_ITEM(Type0cSessionEnd),
    NT_EVENT_MAP_ITEM(Type0sSessionBegin),
    NT_EVENT_MAP_ITEM(Type0sSessionEnd),
    NT_EVENT_MAP_ITEM(Type1cSessionBegin),
    NT_EVENT_MAP_ITEM(Type1cSessionEnd),
    NT_EVENT_MAP_ITEM(Type1sSessionBegin),
    NT_EVENT_MAP_ITEM(Type1sSessionEnd),
    NT_EVENT_MAP_ITEM(Type2cSessionBegin),
    NT_EVENT_MAP_ITEM(Type2cSessionEnd),
    NT_EVENT_MAP_ITEM(Type2sSessionBegin),
    NT_EVENT_MAP_ITEM(Type2sSessionEnd),
};

SMF_ErrorCode Engine_MainEngine::OnInitStateMachine()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    SMF_ADD_STATE_TO_PARENT_STATE(State_Root, _InternalRoot_, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1sSession, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type0sSession, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type0cSession, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1cSession, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Idle, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession, State_Root, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type0sSession_Receiving, State_Type0sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type0sSession_End, State_Type0sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type0cSession_Sending, State_Type0cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type0cSession_End, State_Type0cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1sSession_Receiving, State_Type1sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1sSession_End, State_Type1sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1sSession_Sending, State_Type1sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1cSession_Sending, State_Type1cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1cSession_End, State_Type1cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type1cSession_Receiving, State_Type1cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_End, State_Type2sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Main, State_Type2sSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_End, State_Type2cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Main, State_Type2cSession, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Phase0Sending, State_Type2sSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Phase0Receiving, State_Type2sSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Phase1Sending, State_Type2sSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Phase1Receiving, State_Type2sSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Phase2Sending, State_Type2sSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2sSession_Phase2Receiving, State_Type2sSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Phase0Receiving, State_Type2cSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Phase0Sending, State_Type2cSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Phase1Receiving, State_Type2cSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Phase1Sending, State_Type2cSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Phase2Receiving, State_Type2cSession_Main, GetIndex());
    SMF_ADD_STATE_TO_PARENT_STATE(State_Type2cSession_Phase2Sending, State_Type2cSession_Main, GetIndex());

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

} // namespace NT {
