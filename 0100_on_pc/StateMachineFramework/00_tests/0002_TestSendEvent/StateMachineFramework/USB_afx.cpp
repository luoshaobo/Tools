#include "stdafx.h"
#include "SMF_afx.h"
#include "USB_afx.hpp"
#include "USB_States.hpp"
#include "USB_Engines.hpp"

namespace USB {

SMF_DEFINE_ENGINE(MainEngine, USB_INSTANCE_COUNT)

SMF_DEFINE_STATE(MainEngine, _InternalRoot_, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Root, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, LowSpeed, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, LowSpeed01, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, LowSpeed02, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, HighSpeed, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, HighSpeed02, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, HighSpeed01, USB_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, HighSpeed03, USB_INSTANCE_COUNT)

} // namespace USB {
