#include "eventstatemachinecontrol.h"
#include "comalog.h"

EventStatemachineControl::EventStatemachineControl() :m_eTrigger(EVENT_UNINITED)
{
}

EventStatemachineControl::EventStatemachineControl(Trigger trigger) : Event(), m_eTrigger(trigger)
{
}

EventStatemachineControl::~EventStatemachineControl()
{
}

EventStatemachineControl::Trigger EventStatemachineControl::getTrigger() const
{
  return (m_eTrigger);
}

void EventStatemachineControl::setTrigger(EventStatemachineControl::Trigger trigger)
{
  m_eTrigger = trigger;
}

const char* EventStatemachineControl::toString(Trigger value)
{
  switch (value)
  {
  case EVENT_UNINITED:
  {
    return "EVENT_UNINITED";
    break;
  }
  case EVENT_START:
  {
    return "EVENT_START";
    break;
  }
  default:
  {
    return "EventStatemachineControl UNKNOWN Trigger";
    break;
  }
  }

  return "UNKNOWN";
}

const char * EventStatemachineControl::toString() const
{
  return (toString(m_eTrigger));
}
