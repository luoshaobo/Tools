#ifndef USB_AFX_HPP
#define USB_AFX_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventQueue.h"

#ifndef USB_INSTANCE_COUNT
#define USB_INSTANCE_COUNT 2
#endif

namespace USB {

enum {
    SMF_D_EVENT_ID(Entry) = SMF_D_EVENT_ID(_ENTRY),
    SMF_D_EVENT_ID(Exit) = SMF_D_EVENT_ID(_EXIT),
    SMF_D_EVENT_ID(USR_INVALID) = SMF_EVTID_USR_BASE,
    SMF_D_EVENT_ID(Evt01),
    SMF_D_EVENT_ID(Evt02),
    SMF_D_EVENT_ID(Evt11),
    SMF_D_EVENT_ID(Evt12),
    SMF_D_EVENT_ID(Evt21),
    SMF_D_EVENT_ID(Evt22),
    SMF_D_EVENT_ID(GotoHighSpeed),
    SMF_D_EVENT_ID(GotoHighSpeed01),
    SMF_D_EVENT_ID(GotoHighSpeed02),
    SMF_D_EVENT_ID(GotoHighSpeed03),
    SMF_D_EVENT_ID(GotoLowSpeed),
    SMF_D_EVENT_ID(GotoLowSpeed01),
    SMF_D_EVENT_ID(GotoLowSpeed02),
};

SMF_DECLARE_ENGINE(MainEngine)

SMF_DECLARE_STATE(_InternalRoot_)
SMF_DECLARE_STATE(Root)
SMF_DECLARE_STATE(LowSpeed)
SMF_DECLARE_STATE(LowSpeed01)
SMF_DECLARE_STATE(LowSpeed02)
SMF_DECLARE_STATE(HighSpeed)
SMF_DECLARE_STATE(HighSpeed02)
SMF_DECLARE_STATE(HighSpeed01)
SMF_DECLARE_STATE(HighSpeed03)

} // namespace USB {

#endif // #define USB_AFX_HPP
