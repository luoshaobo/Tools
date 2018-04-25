#include "APP_BaseState.hpp"
#include "APP_afx.hpp"
#include "APP_EventHandler.hpp"

APP_EventHandlerImpl &APP_BaseState::eh()
{
    return APP_EventHandler::GetInstance(GetIndex());
}

APP_EventHandlerImpl &APP_BaseState::GetEventHandler()
{
    return APP_EventHandler::GetInstance(GetIndex());
}
