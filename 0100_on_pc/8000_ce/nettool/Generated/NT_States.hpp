#ifndef NT_STATES_HPP
#define NT_STATES_HPP


#include "SMF_afx.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEngine.h"
#include "NT_afx.hpp"
#include "NT_BaseState.hpp"
#include "NT_EventHandler.hpp"
#include "NT_EventHandlerImpl.hpp"

namespace NT {

////////////////////////////////////////////////////////////////////////////////////////////////////
// State__InternalRoot_
//
class State__InternalRoot_ : public NT_BaseState
{
public:
    State__InternalRoot_(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State__InternalRoot_() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Root
//
class State_State_Root : public NT_BaseState
{
public:
    State_State_Root(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Root() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession
//
class State_State_Type1sSession : public NT_BaseState
{
public:
    State_State_Type1sSession(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1sSession() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType1sSessionEnd(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1sSessionEnd(rOpArg); }
    SMF_ErrorCode OnType1sSessionBeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1sSessionBeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0sSession
//
class State_State_Type0sSession : public NT_BaseState
{
public:
    State_State_Type0sSession(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type0sSession() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType0sSessionEnd(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType0sSessionEnd(rOpArg); }
    SMF_ErrorCode OnType0sSessionBeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType0sSessionBeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0cSession
//
class State_State_Type0cSession : public NT_BaseState
{
public:
    State_State_Type0cSession(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type0cSession() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType0cSessionEnd(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType0cSessionEnd(rOpArg); }
    SMF_ErrorCode OnType0cSessionBeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType0cSessionBeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession
//
class State_State_Type1cSession : public NT_BaseState
{
public:
    State_State_Type1cSession(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1cSession() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType1cSessionEnd(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1cSessionEnd(rOpArg); }
    SMF_ErrorCode OnType1cSessionBeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1cSessionBeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Idle
//
class State_State_Idle : public NT_BaseState
{
public:
    State_State_Idle(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Idle() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnType0sSessionBegin(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType0sSessionBegin(rOpArg); }
    SMF_ErrorCode OnType1sSessionBegin(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1sSessionBegin(rOpArg); }
    SMF_ErrorCode OnType2sSessionBegin(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionBegin(rOpArg); }
    SMF_ErrorCode OnType0cSessionBegin(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType0cSessionBegin(rOpArg); }
    SMF_ErrorCode OnType1cSessionBegin(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1cSessionBegin(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession
//
class State_State_Type2sSession : public NT_BaseState
{
public:
    State_State_Type2sSession(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionEnd(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionEnd(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession
//
class State_State_Type2cSession : public NT_BaseState
{
public:
    State_State_Type2cSession(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionBegin(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionBegin(rOpArg); }
    SMF_ErrorCode OnType2cSessionEnd(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionEnd(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0sSession_Receiving
//
class State_State_Type0sSession_Receiving : public NT_BaseState
{
public:
    State_State_Type0sSession_Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type0sSession_Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0sSession_End
//
class State_State_Type0sSession_End : public NT_BaseState
{
public:
    State_State_Type0sSession_End(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type0sSession_End() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry_Type0sSession_End(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry_Type0sSession_End(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0cSession_Sending
//
class State_State_Type0cSession_Sending : public NT_BaseState
{
public:
    State_State_Type0cSession_Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type0cSession_Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type0cSession_End
//
class State_State_Type0cSession_End : public NT_BaseState
{
public:
    State_State_Type0cSession_End(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type0cSession_End() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry_Type0cSession_End(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry_Type0cSession_End(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession_Receiving
//
class State_State_Type1sSession_Receiving : public NT_BaseState
{
public:
    State_State_Type1sSession_Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1sSession_Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType1sSessionBeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1sSessionBeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession_End
//
class State_State_Type1sSession_End : public NT_BaseState
{
public:
    State_State_Type1sSession_End(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1sSession_End() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry_Type1sSession_End(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry_Type1sSession_End(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1sSession_Sending
//
class State_State_Type1sSession_Sending : public NT_BaseState
{
public:
    State_State_Type1sSession_Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1sSession_Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession_Sending
//
class State_State_Type1cSession_Sending : public NT_BaseState
{
public:
    State_State_Type1cSession_Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1cSession_Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType1cSessionBeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1cSessionBeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession_End
//
class State_State_Type1cSession_End : public NT_BaseState
{
public:
    State_State_Type1cSession_End(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1cSession_End() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry_Type1cSession_End(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry_Type1cSession_End(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type1cSession_Receiving
//
class State_State_Type1cSession_Receiving : public NT_BaseState
{
public:
    State_State_Type1cSession_Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type1cSession_Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType1cSessionReceivingOK(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType1cSessionReceivingOK(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_End
//
class State_State_Type2sSession_End : public NT_BaseState
{
public:
    State_State_Type2sSession_End(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_End() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry_Type2sSession_End(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry_Type2sSession_End(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Main
//
class State_State_Type2sSession_Main : public NT_BaseState
{
public:
    State_State_Type2sSession_Main(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Main() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase0BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase0BeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_End
//
class State_State_Type2cSession_End : public NT_BaseState
{
public:
    State_State_Type2cSession_End(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_End() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry_Type2cSession_End(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry_Type2cSession_End(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Main
//
class State_State_Type2cSession_Main : public NT_BaseState
{
public:
    State_State_Type2cSession_Main(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Main() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase0BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase0BeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase0Sending
//
class State_State_Type2sSession_Phase0Sending : public NT_BaseState
{
public:
    State_State_Type2sSession_Phase0Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Phase0Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase1BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase1BeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase0Receiving
//
class State_State_Type2sSession_Phase0Receiving : public NT_BaseState
{
public:
    State_State_Type2sSession_Phase0Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Phase0Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase0BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase0BeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase1Sending
//
class State_State_Type2sSession_Phase1Sending : public NT_BaseState
{
public:
    State_State_Type2sSession_Phase1Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Phase1Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase1BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase1BeginReceiving(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase2BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase2BeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase1Receiving
//
class State_State_Type2sSession_Phase1Receiving : public NT_BaseState
{
public:
    State_State_Type2sSession_Phase1Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Phase1Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase1BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase1BeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase2Sending
//
class State_State_Type2sSession_Phase2Sending : public NT_BaseState
{
public:
    State_State_Type2sSession_Phase2Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Phase2Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2sSession_Phase2Receiving
//
class State_State_Type2sSession_Phase2Receiving : public NT_BaseState
{
public:
    State_State_Type2sSession_Phase2Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2sSession_Phase2Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2sSessionPhase2BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2sSessionPhase2BeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase0Receiving
//
class State_State_Type2cSession_Phase0Receiving : public NT_BaseState
{
public:
    State_State_Type2cSession_Phase0Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Phase0Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase1BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase1BeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase0Sending
//
class State_State_Type2cSession_Phase0Sending : public NT_BaseState
{
public:
    State_State_Type2cSession_Phase0Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Phase0Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase0BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase0BeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase1Receiving
//
class State_State_Type2cSession_Phase1Receiving : public NT_BaseState
{
public:
    State_State_Type2cSession_Phase1Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Phase1Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase1BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase1BeginSending(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase2BeginSending(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase2BeginSending(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase1Sending
//
class State_State_Type2cSession_Phase1Sending : public NT_BaseState
{
public:
    State_State_Type2cSession_Phase1Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Phase1Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase1BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase1BeginReceiving(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase2Receiving
//
class State_State_Type2cSession_Phase2Receiving : public NT_BaseState
{
public:
    State_State_Type2cSession_Phase2Receiving(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Phase2Receiving() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_State_Type2cSession_Phase2Sending
//
class State_State_Type2cSession_Phase2Sending : public NT_BaseState
{
public:
    State_State_Type2cSession_Phase2Sending(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : NT_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_State_Type2cSession_Phase2Sending() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnType2cSessionPhase2BeginReceiving(SMF_OpArg &rOpArg) { SMF_SM_LOG(); return GetEventHandler().OnType2cSessionPhase2BeginReceiving(rOpArg); }
};

} // namespace NT {

#endif // #define NT_STATES_HPP
