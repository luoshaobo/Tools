#ifndef EVENT_HANDLER_H__
#define EVENT_HANDLER_H__

#include <map>
#include <string>
#include "eventhandlerfunction.h"
#include "typeinfo.h"
#include "event.h"
#include "comalog.h"

/*
 EventHandler is responsible for mapping from the type of event to the proper method that handles that event.
*/
class Lib_API EventHandler
{
public:
  EventHandler(std::string name) :m_name(name)
  {

  }
  virtual ~EventHandler();
 /**
  * registerEventHandler is registering a method which is responsible to handle a concrete event type when it is
  * notified by a subject. Therefore the TypeInfo for that event-type is used as key and the belonging eventhandler-
  * method is stored as a value in a std::map.
  * @param[in]   T*, this pointer of the specific object
  * @param[in]   memFn*, function pointer of the specific function handling the "EventT*"
  * @return      void
  * @note        EventHandler does not allow several methodhandler for the same EventT*
  * @see also
  */
  template <class T, class EventT>
  void registerEventHandler(T*, void (T::*memFn)(EventT*));

  /**
  * isRegistered checks whether the object T* has already a registered memberfunction memFn*
  * @param[in]   T*, this pointer of the specific object
  * @param[in]   memFn*, function pointer of the specific function handling the "EventT*"
  * @return      void
  * @note
  * @see also
  */
  template <class T, class EventT>
  bool isRegistered(T*, void (T::*memFn)(EventT*));

  /**
  * handleEvent inserts a new Event which should be dispatched to an according eventhandler
  * @param[in]   Event*, pointer to the baseclass of an event
  * @return      void
  * @note
  * @see also
  */
  //void handleEvent(const Event*);

  void handleEvent(const Event::Ptr event);

  const std::string& getName() const { return m_name;  }

private:
  std::string m_name;
  typedef std::multimap<TypeInfo, EventHandlerFunctionBase*> Handlers;
  Handlers m_handlers;
};


template <class T, class EventT>
void EventHandler::registerEventHandler(T* obj, void (T::*memFn)(EventT*))
{
  if (isRegistered(obj, memFn) == false)
  {
    COMALOG(COMALOG_INFO, "EventHandler[%s]::registerEventHandler %s  obj:%8x\n",m_name.c_str(), TypeInfo(typeid(EventT)).getName(), obj);
    m_handlers.insert(std::pair< TypeInfo, EventHandlerFunctionBase*>(TypeInfo(typeid(EventT)), new EventHandlerFunction<T, EventT>(obj, memFn)));
  }
  else
  {
    COMALOG(COMALOG_INFO, "EventHandler[%s]::registerEventHandler %s  obj:%8x already registered\n", m_name.c_str(), TypeInfo(typeid(EventT)).getName(), obj);
  }

}

template <class T, class EventT>
bool EventHandler::isRegistered(T* obj, void (T::*memFn)(EventT*))
{
  bool bRet = false;
  std::pair <Handlers::iterator, Handlers::iterator> ret;
  ret = m_handlers.equal_range(TypeInfo(typeid(*obj)));

  //create tmp object in order to use the comparision operator of EventHandlerFunction
  EventHandlerFunction<T, EventT> tmp(obj, memFn);

  if ( ret.first != m_handlers.end())
  {
    Handlers::iterator it = ret.first;
    do
    {
      if (tmp == *(static_cast<EventHandlerFunction<T, EventT>*>(it->second)))
      {
        bRet = true;
      }

    } while (it++ != ret.second && bRet == false);
  }

  return bRet;
}

#endif // EVENT_HANDLER_H__
