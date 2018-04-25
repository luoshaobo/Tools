#ifndef USB_ENGINES_HPP
#define USB_ENGINES_HPP

#include "SMF_BaseEngine.h"

namespace USB {

class Engine_MainEngine : public SMF_BaseEngine
{
public:
    Engine_MainEngine(const std::string &sEngineName, size_t nIndex) : SMF_BaseEngine(sEngineName, nIndex) {}

    virtual SMF_EventId GetEventIdByName(const std::string &sEventName) const;
    virtual std::string GetEventNameById(SMF_EventId nEventId) const;
    virtual SMF_ErrorCode PreProcessEvent(const SMF_EventInfo &rEventInfo, bool &bProcessed);
    virtual SMF_ErrorCode PostProcessEvent(const SMF_EventInfo &rEventInfo, bool bProcessed);
    virtual SMF_ErrorCode OnUnhandledEvent(const SMF_EventInfo &rEventInfo);

protected:
    virtual SMF_ErrorCode OnInitStateMachine();
    virtual SMF_ErrorCode OnDeinitStateMachine();
};

} // namespace USB {

#endif // #define USB_ENGINES_HPP
