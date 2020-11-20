#include "event.h"
#include "eventhandler.h"

EventHandler::~EventHandler()
{
  Handlers::iterator it = m_handlers.begin();
  while (it != m_handlers.end())
  {
    delete it->second;
    ++it;
  }
  m_handlers.clear();
}

/*void EventHandler::handleEvent(const Event* event)
{
  std::pair <Handlers::iterator, Handlers::iterator> ret;
  ret = m_handlers.equal_range(TypeInfo(typeid(*event)));

  for (Handlers::iterator it = ret.first; it != ret.second; ++it)
  {
    it->second->execute(event);
  }
}*/

void EventHandler::handleEvent(const Event::Ptr event)
{
  std::pair <Handlers::iterator, Handlers::iterator> ret;
  ret = m_handlers.equal_range(TypeInfo(typeid(*event.get())));

  for (Handlers::iterator it = ret.first; it != ret.second; ++it)
  {
    it->second->execute(event.get());
  }
}
