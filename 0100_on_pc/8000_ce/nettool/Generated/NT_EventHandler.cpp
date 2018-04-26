#include "NT_afx.hpp"
#include "NT_EventHandler.hpp"
#include "NT_EventHandlerImpl.hpp"

NT_EventHandler::NT_EventHandler(size_t nIndex) : SMF_BaseEventHandler(nIndex)
{

}

NT_EventHandler::~NT_EventHandler()
{

}

NT_EventHandlerImpl &NT_EventHandler::GetInstance(size_t nIndex)
{
    return NT_EventHandlerImpl::GetInstance(nIndex);
}
