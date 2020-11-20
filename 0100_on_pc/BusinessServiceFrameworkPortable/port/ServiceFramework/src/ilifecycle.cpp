#include "ilifecycle.h"

ILifeCycle::ILifeCycle() :m_eState(STATE_NOT_INIT)
{
}

ILifeCycle::~ILifeCycle()
{
}

void ILifeCycle::setState(State state)
{
  m_eState = state;
}

ILifeCycle::State ILifeCycle::getState() const
{
  return (m_eState);
}
