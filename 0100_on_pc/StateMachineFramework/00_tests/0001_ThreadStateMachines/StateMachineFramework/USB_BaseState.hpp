#ifndef USB_BASESTATE_HPP
#define USB_BASESTATE_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "USB_EventHandlerImpl.hpp"

class USB_EventHandler;

class USB_BaseState : public SMF_BaseState
{
public:
    USB_BaseState(SMF_BaseEngine &rEngine, const std::string &sStateName, size_t nIndex)
        : SMF_BaseState(rEngine, sStateName, nIndex) {}
    virtual ~USB_BaseState() {}

    virtual USB_EventHandlerImpl &GetEventHandler();
    virtual USB_EventHandlerImpl &eh();

    virtual SMF_ErrorCode Reset() { return SMF_BaseState::Reset(); }
};

#endif // #define USB_BASESTATE_HPP
