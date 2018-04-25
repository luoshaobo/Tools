#ifndef USB_STATES_HPP
#define USB_STATES_HPP


#include "SMF_afx.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEngine.h"
#include "USB_afx.hpp"
#include "USB_BaseState.hpp"
#include "USB_EventHandler.hpp"
#include "USB_EventHandlerImpl.hpp"

namespace USB {

////////////////////////////////////////////////////////////////////////////////////////////////////
// State__InternalRoot_
//
class State__InternalRoot_ : public USB_BaseState
{
public:
    State__InternalRoot_(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
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
class State_Root : public USB_BaseState
{
public:
    State_Root(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_Root() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_LowSpeed
//
class State_LowSpeed : public USB_BaseState
{
public:
    State_LowSpeed(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_LowSpeed() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt01(rOpArg); }
    SMF_ErrorCode OnEvt02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt02(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_LowSpeed01
//
class State_LowSpeed01 : public USB_BaseState
{
public:
    State_LowSpeed01(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_LowSpeed01() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoLowSpeed02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoLowSpeed02(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_LowSpeed02
//
class State_LowSpeed02 : public USB_BaseState
{
public:
    State_LowSpeed02(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_LowSpeed02() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnGotoLowSpeed01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoLowSpeed01(rOpArg); }
    SMF_ErrorCode OnGotoLowSpeed(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoLowSpeed(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed
//
class State_HighSpeed : public USB_BaseState
{
public:
    State_HighSpeed(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_HighSpeed() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt11(rOpArg); }
    SMF_ErrorCode OnEvt12(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt12(rOpArg); }
    SMF_ErrorCode OnEvt21(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt21(rOpArg); }
    SMF_ErrorCode OnEvt22(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt22(rOpArg); }
    SMF_ErrorCode GotoHighSpeed03(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().GotoHighSpeed03(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed02
//
class State_HighSpeed02 : public USB_BaseState
{
public:
    State_HighSpeed02(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_HighSpeed02() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt12(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt12(rOpArg); }
    SMF_ErrorCode OnEvt21(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt21(rOpArg); }
    SMF_ErrorCode OnGotoHighSpeed01(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoHighSpeed01(rOpArg); }
    SMF_ErrorCode OnGotoLowSpeed(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoLowSpeed(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed01
//
class State_HighSpeed01 : public USB_BaseState
{
public:
    State_HighSpeed01(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_HighSpeed01() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt11(rOpArg); }
    SMF_ErrorCode OnGotoHighSpeed02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnGotoHighSpeed02(rOpArg); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// State_HighSpeed03
//
class State_HighSpeed03 : public USB_BaseState
{
public:
    State_HighSpeed03(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex) : USB_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~State_HighSpeed03() {}

    SMF_DECLARE_STATE_TRANS_TABLE()

public:
    //
    // Event handlers
    //
    SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEntry(rOpArg); }
    SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnExit(rOpArg); }
    SMF_ErrorCode OnEvt11(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().OnEvt11(rOpArg); }
    SMF_ErrorCode GotoHighSpeed02(SMF_OpArg &rOpArg) { SMF_LOG_STATE(); return GetEventHandler().GotoHighSpeed02(rOpArg); }
};

} // namespace USB {

#endif // #define USB_STATES_HPP
