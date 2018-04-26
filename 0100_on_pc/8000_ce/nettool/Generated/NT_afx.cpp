#include "stdafx.h"
#include "SMF_afx.h"
#include "NT_afx.hpp"

namespace NT {

SMF_DEFINE_ENGINE(MainEngine, NT_INSTANCE_COUNT)

SMF_DEFINE_STATE(MainEngine, _InternalRoot_, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Root, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1sSession, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type0sSession, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type0cSession, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1cSession, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Idle, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type0sSession_Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type0sSession_End, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type0cSession_Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type0cSession_End, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1sSession_Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1sSession_End, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1sSession_Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1cSession_Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1cSession_End, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type1cSession_Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_End, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Main, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_End, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Main, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Phase0Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Phase0Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Phase1Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Phase1Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Phase2Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2sSession_Phase2Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Phase0Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Phase0Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Phase1Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Phase1Sending, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Phase2Receiving, NT_INSTANCE_COUNT)
SMF_DEFINE_STATE(MainEngine, State_Type2cSession_Phase2Sending, NT_INSTANCE_COUNT)

} // namespace NT {
