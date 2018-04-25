#include "APP_afx.hpp"
#include "APP_States.hpp"
#include "APP_Engines.hpp"

namespace APP {

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
// State_Root
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Root)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(PostEvtToSM2, true, OnPostEvtToSM2, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(Rpc1, true, OnRpc1, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(ThreadTimer1, true, OnThreadTimer1, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(ThreadTimer2, true, OnThreadTimer2, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(ThreadTimer3, true, OnThreadTimer3, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(ThreadCall1, true, OnThreadCall1, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(ThreadCall2, true, OnThreadCall2, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(OutEvt1, true, OnOutEvt1, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(InEvt1, true, OnInEvt1, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(OutEvt2, true, OnOutEvt2, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(InEvt2, true, OnInEvt2, Root)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, Sleep)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Sleep)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt01, true, OnEvt01, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt02, true, OnEvt02, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt01, true, OnInternalEvt01, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt02, true, OnInternalEvt02, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt03, true, OnInternalEvt03, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt04, true, OnInternalEvt04, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt05, true, OnInternalEvt05, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt06, true, OnInternalEvt06, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt07, true, OnInternalEvt07, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt08, true, OnInternalEvt08, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt09, true, OnInternalEvt09, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt10, true, OnInternalEvt10, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(InternalEvt11, true, OnInternalEvt11, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake, true, null, Awake)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, (eh().nSleepValue==2), null, Sleep02)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake2, true, null, Awake)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep03, true, OnGotoSleep03, Sleep03)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true/*else*/, null, Sleep01)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep01
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Sleep01)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep02, true, OnGotoSleep02, Sleep02)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake, true, OnGotoAwake, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt02, true, null, Sleep)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep02
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Sleep02)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep01, true, OnGotoSleep01, Sleep01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake, true, OnGotoAwake, Sleep)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Awake)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt11, true, OnEvt11, Awake)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt12, true, OnEvt12, Awake)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, (eh().nAwakeValue==1), null, Awake01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep, true, OnGotoSleep, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep2, true, OnGotoSleep2, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true/*else*/, OnGotoAwake02ByDefault, Awake02)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake02
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Awake02)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt12, true, OnEvt12, Awake02)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt02_01, true, OnEvt02_01, Awake02)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt02_02, true, OnEvt02_02, Awake02)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake01, true, OnGotoAwake01, Awake01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep3, true, OnGotoSleep3, Sleep)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, Awake02_01)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake01
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Awake01)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt11, true, OnEvt11, Awake01)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt01_01, true, OnEvt01_01, Awake01)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt01_02, true, OnEvt01_02, Awake01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep, true, null, Awake)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake02, true, OnGotoAwake02, Awake02)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, Awake01_01)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake01_01
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Awake01_01)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake01, true, OnGotoAwake01, Awake01)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake02_01
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Awake02_01)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoAwake02, true, OnGotoAwake02, Awake02)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep03
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(Sleep03)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoSleep02, true, OnGotoSleep02, Sleep02)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

} // namespace APP {
