#include "eventstatemachineresult.h"

EventStatemachineResult::EventStatemachineResult() : Event(), m_statemachineId(-1),
                                                      m_statemachineType(-1),
                                                      m_result(IStatemachine::RESULT_UNDEFINED)
{
}

EventStatemachineResult::EventStatemachineResult(IStatemachine::StatemachineId id,
                                                 StatemachineType type,
                                                 IStatemachine::StatemachineResult result) : Event(),
                                                 m_statemachineId(id),
                                                 m_statemachineType(type),
                                                  m_result(result)
{
}

EventStatemachineResult::~EventStatemachineResult()
{
}

void EventStatemachineResult::setResult(IStatemachine::StatemachineId id, StatemachineType type, IStatemachine::StatemachineResult result)
{
  m_statemachineId = id;
  m_statemachineType = type;
  m_result = result;
}

IStatemachine::StatemachineResult EventStatemachineResult::getStatemachineResult() const
{
  return m_result;
}

StatemachineType EventStatemachineResult::getStatemachineType() const
{
  return m_statemachineType;
}

IStatemachine::StatemachineId EventStatemachineResult::getStatemachineId() const
{
  return m_statemachineId;
}
