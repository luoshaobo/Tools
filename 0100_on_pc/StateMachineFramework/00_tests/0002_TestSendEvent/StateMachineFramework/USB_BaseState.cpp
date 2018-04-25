#include "USB_BaseState.hpp"
#include "USB_afx.hpp"
#include "USB_EventHandler.hpp"

USB_EventHandlerImpl &USB_BaseState::eh()
{
    return USB_EventHandler::GetInstance(GetIndex());
}

USB_EventHandlerImpl &USB_BaseState::GetEventHandler()
{
    return USB_EventHandler::GetInstance(GetIndex());
}
