#ifndef NT_AFX_HPP
#define NT_AFX_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventQueue.h"
#include "NT_States.hpp"
#include "NT_Engines.hpp"

#ifndef NT_INSTANCE_COUNT
#define NT_INSTANCE_COUNT 1
#endif

namespace NT {

enum {
    SMF_D_EVENT_ID(Entry) = SMF_D_EVENT_ID(_ENTRY),
    SMF_D_EVENT_ID(Exit) = SMF_D_EVENT_ID(_EXIT),
    SMF_D_EVENT_ID(USR_INVALID) = SMF_EVTID_USR_BASE,
    SMF_D_EVENT_ID(ReceivingFailed),
    SMF_D_EVENT_ID(ReceivingOK),
    SMF_D_EVENT_ID(ReceivingTimeout),
    SMF_D_EVENT_ID(SendingFailed),
    SMF_D_EVENT_ID(SendingOK),
    SMF_D_EVENT_ID(SendingTimeout),
    SMF_D_EVENT_ID(SessionEnd),
    SMF_D_EVENT_ID(Type0cSessionBegin),
    SMF_D_EVENT_ID(Type0cSessionEnd),
    SMF_D_EVENT_ID(Type0sSessionBegin),
    SMF_D_EVENT_ID(Type0sSessionEnd),
    SMF_D_EVENT_ID(Type1cSessionBegin),
    SMF_D_EVENT_ID(Type1cSessionEnd),
    SMF_D_EVENT_ID(Type1sSessionBegin),
    SMF_D_EVENT_ID(Type1sSessionEnd),
    SMF_D_EVENT_ID(Type2cSessionBegin),
    SMF_D_EVENT_ID(Type2cSessionEnd),
    SMF_D_EVENT_ID(Type2sSessionBegin),
    SMF_D_EVENT_ID(Type2sSessionEnd),
};

SMF_DECLARE_ENGINE(MainEngine)

SMF_DECLARE_STATE(_InternalRoot_)
SMF_DECLARE_STATE(State_Root)
SMF_DECLARE_STATE(State_Type1sSession)
SMF_DECLARE_STATE(State_Type0sSession)
SMF_DECLARE_STATE(State_Type0cSession)
SMF_DECLARE_STATE(State_Type1cSession)
SMF_DECLARE_STATE(State_Idle)
SMF_DECLARE_STATE(State_Type2sSession)
SMF_DECLARE_STATE(State_Type2cSession)
SMF_DECLARE_STATE(State_Type0sSession_Receiving)
SMF_DECLARE_STATE(State_Type0sSession_End)
SMF_DECLARE_STATE(State_Type0cSession_Sending)
SMF_DECLARE_STATE(State_Type0cSession_End)
SMF_DECLARE_STATE(State_Type1sSession_Receiving)
SMF_DECLARE_STATE(State_Type1sSession_End)
SMF_DECLARE_STATE(State_Type1sSession_Sending)
SMF_DECLARE_STATE(State_Type1cSession_Sending)
SMF_DECLARE_STATE(State_Type1cSession_End)
SMF_DECLARE_STATE(State_Type1cSession_Receiving)
SMF_DECLARE_STATE(State_Type2sSession_End)
SMF_DECLARE_STATE(State_Type2sSession_Main)
SMF_DECLARE_STATE(State_Type2cSession_End)
SMF_DECLARE_STATE(State_Type2cSession_Main)
SMF_DECLARE_STATE(State_Type2sSession_Phase0Sending)
SMF_DECLARE_STATE(State_Type2sSession_Phase0Receiving)
SMF_DECLARE_STATE(State_Type2sSession_Phase1Sending)
SMF_DECLARE_STATE(State_Type2sSession_Phase1Receiving)
SMF_DECLARE_STATE(State_Type2sSession_Phase2Sending)
SMF_DECLARE_STATE(State_Type2sSession_Phase2Receiving)
SMF_DECLARE_STATE(State_Type2cSession_Phase0Receiving)
SMF_DECLARE_STATE(State_Type2cSession_Phase0Sending)
SMF_DECLARE_STATE(State_Type2cSession_Phase1Receiving)
SMF_DECLARE_STATE(State_Type2cSession_Phase1Sending)
SMF_DECLARE_STATE(State_Type2cSession_Phase2Receiving)
SMF_DECLARE_STATE(State_Type2cSession_Phase2Sending)

} // namespace NT {

#endif // #define NT_AFX_HPP
