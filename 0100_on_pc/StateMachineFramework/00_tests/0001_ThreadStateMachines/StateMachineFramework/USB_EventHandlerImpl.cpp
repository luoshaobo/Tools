#include "USB_afx.hpp"
#include "USB_States.hpp"
#include "USB_Engines.hpp"
#include "USB_EventHandler.hpp"
#include "USB_EventHandlerImpl.hpp"

using namespace USB;

USB_EventHandlerImpl::USB_EventHandlerImpl(size_t nIndex)
    : USB_EventHandler(nIndex)
{

}

USB_EventHandlerImpl::~USB_EventHandlerImpl()
{

}

class USB_EventHandleFactory
{
private:
    static SMF_MACHINE_WORD_TYPE arrObjBuff[USB_INSTANCE_COUNT][SMF_ALIGNED_SIZE(USB_EventHandlerImpl) / sizeof(SMF_MACHINE_WORD_TYPE)];

public:
    static USB_EventHandlerImpl *NewObject(size_t nIndex) {
        if (nIndex >= USB_INSTANCE_COUNT) {
            nIndex = 0;
        }
        return new(arrObjBuff[nIndex]) USB_EventHandlerImpl(nIndex);
    }
    static void DeleteObject(USB_EventHandlerImpl *pObject) {
        if (pObject != NULL) {
            pObject->~USB_EventHandlerImpl();
            pObject->operator delete(pObject, pObject);
        }
    }
};
SMF_MACHINE_WORD_TYPE USB_EventHandleFactory::arrObjBuff[USB_INSTANCE_COUNT][SMF_ALIGNED_SIZE(USB_EventHandlerImpl) / sizeof(SMF_MACHINE_WORD_TYPE)];

USB_EventHandlerImpl &USB_EventHandlerImpl::GetInstance(size_t nIndex)
{
    static SMF_AutoReleaseObjectPool<USB_EventHandlerImpl, USB_INSTANCE_COUNT, USB_EventHandleFactory> arop;
    return arop.At(nIndex);
}

SMF_ErrorCode USB_EventHandlerImpl::Reset()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = USB_EventHandler::Reset();
    }

    return nErrorCode;
}

