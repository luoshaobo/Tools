#include "USB_afx.hpp"
#include "USB_States.hpp"
#include "USB_Engines.hpp"

namespace USB {

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
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, LowSpeed)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_LowSpeed
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(LowSpeed)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt01, true, OnEvt01, LowSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt02, true, OnEvt02, LowSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, LowSpeed01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoHighSpeed, true, null, HighSpeed)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_LowSpeed01
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(LowSpeed01)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoLowSpeed02, true, OnGotoLowSpeed02, LowSpeed02)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt02, true, null, LowSpeed)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_LowSpeed02
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(LowSpeed02)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoLowSpeed01, true, OnGotoLowSpeed01, LowSpeed01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoLowSpeed, true, OnGotoLowSpeed, LowSpeed)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(HighSpeed)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt11, true, OnEvt11, HighSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt12, true, OnEvt12, HighSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt21, true, OnEvt21, HighSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt22, true, OnEvt22, HighSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(_GOTO_DEF_CHILD, true, null, HighSpeed01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoHighSpeed03, true, GotoHighSpeed03, HighSpeed03)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed02
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(HighSpeed02)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt12, true, OnEvt12, HighSpeed02)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt21, true, OnEvt21, HighSpeed02)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoHighSpeed01, true, OnGotoHighSpeed01, HighSpeed01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoLowSpeed, true, OnGotoLowSpeed, LowSpeed)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed01
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(HighSpeed01)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt11, true, OnEvt11, HighSpeed01)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoHighSpeed, true, null, HighSpeed)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoHighSpeed02, true, OnGotoHighSpeed02, HighSpeed02)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed03
//
#define SMF_CURRENT_STATE_CLASS           SMF_D_STATE_CLASS(HighSpeed03)

SMF_DEFINE_STATE_TRANS_TABLE_BEGIN()
    SMF_DEFINE_STATE_TRANS_ITEM(Entry, true, OnEntry, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Exit, true, OnExit, null)
    SMF_DEFINE_STATE_TRANS_ITEM(Evt11, true, OnEvt11, HighSpeed03)
    SMF_DEFINE_STATE_TRANS_ITEM(GotoHighSpeed02, true, GotoHighSpeed02, HighSpeed02)
SMF_DEFINE_STATE_TRANS_TABLE_END()

#undef SMF_CURRENT_STATE_CLASS

} // namespace USB {
