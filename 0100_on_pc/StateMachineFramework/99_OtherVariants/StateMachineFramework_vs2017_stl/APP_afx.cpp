#include "stdafx.h"
#include "SMF_afx.h"
#include "APP_afx.hpp"
#include "APP_States.hpp"
#include "APP_Engines.hpp"

namespace APP {

SMF_DEFINE_ENGINE(MainEngine, APP_INSTANCE_COUNT)

SMF_DEFINE_STATE(MainEngine, _InternalRoot_, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Root, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Sleep, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Sleep01, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Sleep02, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Awake, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Awake02, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Awake01, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Awake01_01, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Awake02_01, APP_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, Sleep03, APP_INSTANCE_COUNT)

} // namespace APP {
