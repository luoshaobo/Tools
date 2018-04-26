#include "NT_afx.hpp"
#include "NT_States.hpp"

namespace NT {

//
// NOTE:
// 1) The transitions can be defined by the macro:
//        SMF_DEFINE_STATE_TRANS_ITEM(event_id,condition,operation,dest_state_name)
//    with the arguments as below:
//        event_id: the event ID without prefix.
//        condition: the condition which will be not changed.
//        operation: the operation which will be executed on the event.
//        dest_state_name: the new state name without prefix.  
//

////////////////////////////////////////////////////////////////////////////////////////////////////
// State__InternalRoot_
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(_InternalRoot_)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_FIRST_CHILD, true, null, null)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Root
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Root)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, State_Idle)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1sSession)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type1sSessionEnd, true, OnType1sSessionEnd, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, OnType1sSessionBeginReceiving, State_Type1sSession_Receiving)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0sSession
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type0sSession)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type0sSessionEnd, true, OnType0sSessionEnd, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, OnType0sSessionBeginReceiving, State_Type0sSession_Receiving)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0cSession
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type0cSession)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type0cSessionEnd, true, OnType0cSessionEnd, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, OnType0cSessionBeginSending, State_Type0cSession_Sending)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1cSession)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type1cSessionEnd, true, OnType1cSessionEnd, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, OnType1cSessionBeginSending, State_Type1cSession_Sending)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Idle
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Idle)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Type0sSessionBegin, true, OnType0sSessionBegin, State_Type0sSession)
    SMF_DEFINE_STATE_TRANS_ITEM(Type1sSessionBegin, true, OnType1sSessionBegin, State_Type1sSession)
    SMF_DEFINE_STATE_TRANS_ITEM(Type2sSessionBegin, true, OnType2sSessionBegin, State_Type2sSession)
    SMF_DEFINE_STATE_TRANS_ITEM(Type0cSessionBegin, true, OnType0cSessionBegin, State_Type0cSession)
    SMF_DEFINE_STATE_TRANS_ITEM(Type1cSessionBegin, true, OnType1cSessionBegin, State_Type1cSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type2sSessionEnd, true, OnType2sSessionEnd, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, State_Type2sSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type2cSessionBegin, true, OnType2cSessionBegin, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(Type2cSessionEnd, true, OnType2cSessionEnd, State_Idle)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, State_Type2cSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0sSession_Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type0sSession_Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type0sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, null, State_Type0sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type0sSession_End)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0sSession_End
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type0sSession_End)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry_Type0sSession_End, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type0sSessionEnd, true, null, State_Type0sSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0cSession_Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type0cSession_Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type0cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, null, State_Type0cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type0cSession_End)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0cSession_End
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type0cSession_End)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry_Type0cSession_End, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type0cSessionEnd, true, null, State_Type0cSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession_Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1sSession_Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type1sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, OnType1sSessionBeginSending, State_Type1sSession_Sending)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type1sSession_End)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession_End
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1sSession_End)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry_Type1sSession_End, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type1sSessionEnd, true, null, State_Type1sSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession_Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1sSession_Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, null, State_Type1sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type1sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type1sSession_End)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession_Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1cSession_Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type1cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, OnType1cSessionBeginReceiving, State_Type1cSession_Receiving)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type1cSession_End)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession_End
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1cSession_End)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry_Type1cSession_End, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type1cSessionEnd, true, null, State_Type1cSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession_Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type1cSession_Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, OnType1cSessionReceivingOK, State_Type1cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type1cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type1cSession_End)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_End
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_End)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry_Type2sSession_End, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type2sSessionEnd, true, null, State_Type2sSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Main
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Main)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SessionEnd, true, null, State_Type2sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2sSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, OnType2sSessionPhase0BeginReceiving, State_Type2sSession_Phase0Receiving)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_End
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_End)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry_Type2cSession_End, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Type2cSessionEnd, true, null, State_Type2cSession)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Main
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Main)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SessionEnd, true, null, State_Type2cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2cSession_End)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, OnType2cSessionPhase0BeginSending, State_Type2cSession_Phase0Sending)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase0Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Phase0Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, OnType2sSessionPhase1BeginReceiving, State_Type2sSession_Phase1Receiving)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2sSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase0Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Phase0Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, OnType2sSessionPhase0BeginSending, State_Type2sSession_Phase0Sending)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2sSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase1Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Phase1Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, (!eh().bType2sSessionPhase1Finished), OnType2sSessionPhase1BeginReceiving, State_Type2sSession_Phase1Receiving)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true/*else*/, OnType2sSessionPhase2BeginReceiving, State_Type2sSession_Phase2Receiving)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase1Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Phase1Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, OnType2sSessionPhase1BeginSending, State_Type2sSession_Phase1Sending)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2sSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase2Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Phase2Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SessionEnd, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2sSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase2Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2sSession_Phase2Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, OnType2sSessionPhase2BeginSending, State_Type2sSession_Phase2Sending)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2sSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2sSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase0Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Phase0Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true, OnType2cSessionPhase1BeginSending, State_Type2cSession_Phase1Sending)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2cSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase0Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Phase0Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, OnType2cSessionPhase0BeginReceiving, State_Type2cSession_Phase0Receiving)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2cSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase1Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Phase1Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, (!eh().bType2cSessionPhase1Finished), OnType2cSessionPhase1BeginSending, State_Type2cSession_Phase1Sending)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingOK, true/*else*/, OnType2cSessionPhase2BeginSending, State_Type2cSession_Phase2Sending)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase1Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Phase1Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, OnType2cSessionPhase1BeginReceiving, State_Type2cSession_Phase1Receiving)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2cSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase2Receiving
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Phase2Receiving)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SessionEnd, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingFailed, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(ReceivingTimeout, true, null, State_Type2cSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase2Sending
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(State_Type2cSession_Phase2Sending)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingOK, true, OnType2cSessionPhase2BeginReceiving, State_Type2cSession_Phase2Receiving)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingFailed, true, null, State_Type2cSession_Main)
    SMF_DEFINE_STATE_TRANS_ITEM(SendingTimeout, true, null, State_Type2cSession_Main)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

} // namespace NT {
