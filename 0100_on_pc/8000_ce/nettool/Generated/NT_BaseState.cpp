#include "NT_BaseState.hpp"
#include "NT_afx.hpp"
#include "NT_EventHandler.hpp"

NT_EventHandlerImpl &NT_BaseState::eh()
{
    return NT_EventHandler::GetInstance(GetIndex());
}

NT_EventHandlerImpl &NT_BaseState::GetEventHandler()
{
    return NT_EventHandler::GetInstance(GetIndex());
}
