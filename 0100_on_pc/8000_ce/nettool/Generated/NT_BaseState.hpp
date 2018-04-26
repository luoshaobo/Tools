#ifndef NT_BASESTATE_HPP
#define NT_BASESTATE_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "NT_EventHandlerImpl.hpp"

class NT_EventHandler;

class NT_BaseState : public SMF_BaseState
{
public:
    NT_BaseState(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex)
        : SMF_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~NT_BaseState() {}

    virtual NT_EventHandlerImpl &GetEventHandler();
    virtual NT_EventHandlerImpl &eh();

    virtual SMF_ErrorCode Reset() { return SMF_BaseState::Reset(); }
};

#endif // #define NT_BASESTATE_HPP
