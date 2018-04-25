#include "APP_afx.hpp"
#include "APP_EventHandler.hpp"
#include "APP_EventHandlerImpl.hpp"

APP_EventHandler::APP_EventHandler(size_t nIndex) : SMF_BaseEventHandler(nIndex)
{

}

APP_EventHandler::~APP_EventHandler()
{

}

APP_EventHandlerImpl &APP_EventHandler::GetInstance(size_t nIndex)
{
    return APP_EventHandlerImpl::GetInstance(nIndex);
}
