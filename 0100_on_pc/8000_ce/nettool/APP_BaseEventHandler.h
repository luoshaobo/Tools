#ifndef APP_BASE_EVENT_HANDLER_H__873482398493284823949324783274723647326747237472384329498238423
#define APP_BASE_EVENT_HANDLER_H__873482398493284823949324783274723647326747237472384329498238423

#include "NT_EventHandler.hpp"

class NT_EventHandlerImpl;

class APP_BaseEventHandler : public NT_EventHandler
{
public:
    APP_BaseEventHandler(NT_EventHandlerImpl &rEventHandlerImpl);
    ~APP_BaseEventHandler();

public:
    virtual int OnRemoteInput(const std::vector<char> &arrContent);
    virtual int OnLocalInput(const std::vector<char> &arrContent);
    virtual int RemoteOutput(const std::vector<char> &arrContent);
    virtual int LocalOutput(const std::vector<char> &arrContent);
    virtual int EngineThread_OnRemoteInput(const std::vector<char> &arrContent);
    virtual int EngineThread_OnLocalInput(const std::vector<char> &arrContent);
    virtual int OutputThread_RemoteOutput(const std::vector<char> &arrContent);
    virtual int OutputThread_LocalOutput(const std::vector<char> &arrContent);

protected::
    NT_EventHandlerImpl &m_rEventHandlerImpl;
};

#endif // #ifndef APP_BASE_EVENT_HANDLER_H__873482398493284823949324783274723647326747237472384329498238423
