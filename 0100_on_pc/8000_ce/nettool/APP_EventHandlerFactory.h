#ifndef EVENT_HANDLER_FACTORY_H__832892394923040234023985895899359398543895984
#define EVENT_HANDLER_FACTORY_H__832892394923040234023985895899359398543895984

#include "SMF_afx.h"
#include "SMF_PlatformLock.h"
#include "APP_afx.h"
#include "NT_afx.hpp"
#include "NT_EventHandlerImpl.hpp"

class APP_EventHandlerFactory
{
private:
    APP_EventHandlerFactory();

public:
    static APP_EventHandlerFactory &GetInstance();
    virtual ~APP_EventHandlerFactory();

public:
    NT_EventHandlerImpl *Allocate();
    void Free(NT_EventHandlerImpl *pEventHandlerImpl);

private:
    SMF_PlatformLock m_oLock;
    bool m_arrEngineInUse[NT_INSTANCE_COUNT];
};

#endif // #ifndef EVENT_HANDLER_FACTORY_H__832892394923040234023985895899359398543895984
