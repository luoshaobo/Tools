#ifndef NT_ENGINES_HPP
#define NT_ENGINES_HPP

#include "SMF_BaseEngine.h"

namespace NT {

class Engine_MainEngine : public SMF_BaseEngine
{
public:
    Engine_MainEngine(const std::string &sEngineName, size_t nIndex) : SMF_BaseEngine(sEngineName, nIndex) {}

    virtual SMF_EventId GetEventIdByName(const std::string &sEventName) const;
    virtual std::string GetEventNameById(SMF_EventId nEventId) const;

protected:
    virtual SMF_ErrorCode OnInitStateMachine();
    virtual SMF_ErrorCode OnDeinitStateMachine();
};

} // namespace NT {

#endif // #define NT_ENGINES_HPP
