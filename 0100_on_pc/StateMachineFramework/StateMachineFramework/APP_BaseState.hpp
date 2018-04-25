#ifndef APP_BASESTATE_HPP
#define APP_BASESTATE_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "APP_EventHandlerImpl.hpp"

class APP_EventHandler;

class APP_BaseState : public SMF_BaseState
{
public:
    APP_BaseState(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex)
        : SMF_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~APP_BaseState() {}

    virtual APP_EventHandlerImpl &GetEventHandler();
    virtual APP_EventHandlerImpl &eh();

    virtual SMF_ErrorCode Reset() { return SMF_BaseState::Reset(); }
};

#endif // #define APP_BASESTATE_HPP
