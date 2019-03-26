#ifndef APP_STATES_HPP
#define APP_STATES_HPP


#include "SMF_afx.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEngine.h"
#include "APP_afx.hpp"
#include "APP_BaseState.hpp"
#include "APP_EventHandler.hpp"
#include "APP_EventHandlerImpl.hpp"

namespace APP {

////////////////////////////////////////////////////////////////////////////////////////////////////
// State__InternalRoot_
//
class State__InternalRoot_ : public APP_BaseState
{
public:
    State__InternalRoot_(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State__InternalRoot_() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Root
//
class State_Root : public APP_BaseState
{
public:
    State_Root(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Root() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnPostEvtToSM2(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnPostEvtToSM2(rOpArg); }
    SMF_ErrorCode OnRpc1(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnRpc1(rOpArg); }
    SMF_ErrorCode OnThreadTimer1(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnThreadTimer1(rOpArg); }
    SMF_ErrorCode OnThreadTimer2(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnThreadTimer2(rOpArg); }
    SMF_ErrorCode OnThreadTimer3(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnThreadTimer3(rOpArg); }
    SMF_ErrorCode OnThreadCall1(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnThreadCall1(rOpArg); }
    SMF_ErrorCode OnThreadCall2(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnThreadCall2(rOpArg); }
    SMF_ErrorCode OnOutEvt1(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnOutEvt1(rOpArg); }
    SMF_ErrorCode OnInEvt1(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInEvt1(rOpArg); }
    SMF_ErrorCode OnOutEvt2(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnOutEvt2(rOpArg); }
    SMF_ErrorCode OnInEvt2(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInEvt2(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep
//
class State_Sleep : public APP_BaseState
{
public:
    State_Sleep(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Sleep() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt01(rOpArg); }
    SMF_ErrorCode OnEvt02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt02(rOpArg); }
    SMF_ErrorCode OnInternalEvt01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt01(rOpArg); }
    SMF_ErrorCode OnInternalEvt02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt02(rOpArg); }
    SMF_ErrorCode OnInternalEvt03(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt03(rOpArg); }
    SMF_ErrorCode OnInternalEvt04(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt04(rOpArg); }
    SMF_ErrorCode OnInternalEvt05(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt05(rOpArg); }
    SMF_ErrorCode OnInternalEvt06(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt06(rOpArg); }
    SMF_ErrorCode OnInternalEvt07(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt07(rOpArg); }
    SMF_ErrorCode OnInternalEvt08(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt08(rOpArg); }
    SMF_ErrorCode OnInternalEvt09(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt09(rOpArg); }
    SMF_ErrorCode OnInternalEvt10(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt10(rOpArg); }
    SMF_ErrorCode OnInternalEvt11(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnInternalEvt11(rOpArg); }
    SMF_ErrorCode OnGotoSleep03(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep03(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep01
//
class State_Sleep01 : public APP_BaseState
{
public:
    State_Sleep01(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Sleep01() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoSleep02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep02(rOpArg); }
    SMF_ErrorCode OnGotoAwake(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep02
//
class State_Sleep02 : public APP_BaseState
{
public:
    State_Sleep02(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Sleep02() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoSleep01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep01(rOpArg); }
    SMF_ErrorCode OnGotoAwake(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake
//
class State_Awake : public APP_BaseState
{
public:
    State_Awake(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Awake() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt11(rOpArg); }
    SMF_ErrorCode OnEvt12(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt12(rOpArg); }
    SMF_ErrorCode OnGotoSleep(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep(rOpArg); }
    SMF_ErrorCode OnGotoAwake02ByDefault(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake02ByDefault(rOpArg); }
    SMF_ErrorCode OnGotoSleep2(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep2(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake02
//
class State_Awake02 : public APP_BaseState
{
public:
    State_Awake02(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Awake02() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt12(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt12(rOpArg); }
    SMF_ErrorCode OnEvt02_01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt02_01(rOpArg); }
    SMF_ErrorCode OnEvt02_02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt02_02(rOpArg); }
    SMF_ErrorCode OnGotoAwake01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake01(rOpArg); }
    SMF_ErrorCode OnGotoSleep3(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep3(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake01
//
class State_Awake01 : public APP_BaseState
{
public:
    State_Awake01(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Awake01() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt11(rOpArg); }
    SMF_ErrorCode OnEvt01_01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt01_01(rOpArg); }
    SMF_ErrorCode OnEvt01_02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt01_02(rOpArg); }
    SMF_ErrorCode OnGotoAwake02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake02(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake01_01
//
class State_Awake01_01 : public APP_BaseState
{
public:
    State_Awake01_01(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Awake01_01() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoAwake01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake01(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Awake02_01
//
class State_Awake02_01 : public APP_BaseState
{
public:
    State_Awake02_01(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Awake02_01() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoAwake02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoAwake02(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_Sleep03
//
class State_Sleep03 : public APP_BaseState
{
public:
    State_Sleep03(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : APP_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Sleep03() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoSleep02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoSleep02(rOpArg); }
};

} // namespace APP {

#endif // #define APP_STATES_HPP
