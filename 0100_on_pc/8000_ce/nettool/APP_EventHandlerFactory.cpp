#include "APP_EventHandlerFactory.h"

APP_EventHandlerFactory::APP_EventHandlerFactory()
    : m_oLock()
{
    unsigned int i;

    for (i = 0; i < NT_INSTANCE_COUNT; ++i) {
        m_arrEngineInUse[i] = false;
    }
}

APP_EventHandlerFactory &APP_EventHandlerFactory::GetInstance()
{
    static APP_EventHandlerFactory oAPP_EventHandlerFactory;
    return oAPP_EventHandlerFactory;
}

APP_EventHandlerFactory::~APP_EventHandlerFactory()
{

}

NT_EventHandlerImpl *APP_EventHandlerFactory::Allocate()
{
    NT_EventHandlerImpl *pEventHandlerImpl = NULL;
    unsigned int i;

    m_oLock.Lock();
    for (i = 0; i < NT_INSTANCE_COUNT; ++i) {
        if (!m_arrEngineInUse[i]) {
            pEventHandlerImpl = &NT_EventHandlerImpl::GetInstance(i);
            m_arrEngineInUse[i] = true;
            break;
        }
    }
    m_oLock.Unlock();

    return pEventHandlerImpl;
}

void APP_EventHandlerFactory::Free(NT_EventHandlerImpl *pEventHandlerImpl)
{
    unsigned int i;

    m_oLock.Lock();
    for (i = 0; i < NT_INSTANCE_COUNT; ++i) {
        if (m_arrEngineInUse[i] && pEventHandlerImpl == &NT_EventHandlerImpl::GetInstance(i)) {
            m_arrEngineInUse[i] = false;
            break;
        }
    }
    m_oLock.Unlock();
}
