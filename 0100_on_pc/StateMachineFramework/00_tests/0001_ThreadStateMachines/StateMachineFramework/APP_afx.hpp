#ifndef APP_AFX_HPP
#define APP_AFX_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventQueue.h"

#ifndef APP_INSTANCE_COUNT
#define APP_INSTANCE_COUNT 1
#endif

namespace APP {

enum {
    SMF_D_EVENT_ID(Entry) = SMF_D_EVENT_ID(_ENTRY),
    SMF_D_EVENT_ID(Exit) = SMF_D_EVENT_ID(_EXIT),
    SMF_D_EVENT_ID(USR_INVALID) = SMF_EVTID_USR_BASE,
    SMF_D_EVENT_ID(Evt01),
    SMF_D_EVENT_ID(Evt01_01),
    SMF_D_EVENT_ID(Evt01_02),
    SMF_D_EVENT_ID(Evt02),
    SMF_D_EVENT_ID(Evt02_01),
    SMF_D_EVENT_ID(Evt02_02),
    SMF_D_EVENT_ID(Evt11),
    SMF_D_EVENT_ID(Evt12),
    SMF_D_EVENT_ID(GotoAwake),
    SMF_D_EVENT_ID(GotoAwake01),
    SMF_D_EVENT_ID(GotoAwake02),
    SMF_D_EVENT_ID(GotoAwake2),
    SMF_D_EVENT_ID(GotoSleep),
    SMF_D_EVENT_ID(GotoSleep01),
    SMF_D_EVENT_ID(GotoSleep02),
    SMF_D_EVENT_ID(GotoSleep03),
    SMF_D_EVENT_ID(GotoSleep2),
    SMF_D_EVENT_ID(GotoSleep3),
    SMF_D_EVENT_ID(InternalEvt01),
    SMF_D_EVENT_ID(InternalEvt02),
    SMF_D_EVENT_ID(InternalEvt03),
    SMF_D_EVENT_ID(InternalEvt04),
    SMF_D_EVENT_ID(InternalEvt05),
    SMF_D_EVENT_ID(InternalEvt06),
    SMF_D_EVENT_ID(InternalEvt07),
    SMF_D_EVENT_ID(InternalEvt08),
    SMF_D_EVENT_ID(InternalEvt09),
    SMF_D_EVENT_ID(InternalEvt10),
    SMF_D_EVENT_ID(InternalEvt11),
    SMF_D_EVENT_ID(PostEvtToSM2),
    SMF_D_EVENT_ID(Rpc1),
    SMF_D_EVENT_ID(ThreadCall1),
    SMF_D_EVENT_ID(ThreadCall2),
    SMF_D_EVENT_ID(ThreadTimer1),
    SMF_D_EVENT_ID(ThreadTimer2),
    SMF_D_EVENT_ID(ThreadTimer3),
};

SMF_DECLARE_ENGINE(MainEngine)

SMF_DECLARE_STATE(_InternalRoot_)
SMF_DECLARE_STATE(Root)
SMF_DECLARE_STATE(Sleep)
SMF_DECLARE_STATE(Sleep01)
SMF_DECLARE_STATE(Sleep02)
SMF_DECLARE_STATE(Awake)
SMF_DECLARE_STATE(Awake02)
SMF_DECLARE_STATE(Awake01)
SMF_DECLARE_STATE(Awake01_01)
SMF_DECLARE_STATE(Awake02_01)
SMF_DECLARE_STATE(Sleep03)

} // namespace APP {

#endif // #define APP_AFX_HPP
