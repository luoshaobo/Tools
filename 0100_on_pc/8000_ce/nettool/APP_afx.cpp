#include "SMF_BaseThread.h"
#include "SMF_BaseEventQueue.h"
#include "SMF_BaseEngine.h"
#include "APP_afx.h"
#include "NT_afx.hpp"
#include "NT_EventHandlerImpl.hpp"

using namespace NT;

bool SMF_SM_bLogEnabled = false;

void InitStateMachines()
{
    size_t i;
    
    for (i = 0; i < NT_INSTANCE_COUNT; i++) {
        NT_EventHandlerImpl &rEventHandlerImpl = NT_EventHandlerImpl::GetInstance(i);
        SMF_BaseThread &rEngineThread = rEventHandlerImpl.GetEngineThread();
        SMF_BaseEventQueue &rEngineEventQueue = rEventHandlerImpl.GetEngineEventQueue();
        SMF_BaseEngine &rEngine = SMF_GET_ENGINE(MainEngine, i);

        rEngineEventQueue.AttachToThread(rEngineThread, 0);
        rEngine.AttachToEventQueue(rEngineEventQueue, 0);
        
        rEngine.Start();
        rEngineEventQueue.Start(0);
        rEngineThread.Start();

        rEngine.PostEvent(SMF_D_EVENT_ID(_GOTO_FIRST_CHILD));
    }
}
