#include "USB_afx.hpp"
#include "USB_EventHandler.hpp"
#include "USB_EventHandlerImpl.hpp"

USB_EventHandler::USB_EventHandler(size_t nIndex) : SMF_BaseEventHandler(nIndex)
{

}

USB_EventHandler::~USB_EventHandler()
{

}

USB_EventHandlerImpl &USB_EventHandler::GetInstance(size_t nIndex)
{
    return USB_EventHandlerImpl::GetInstance(nIndex);
}
