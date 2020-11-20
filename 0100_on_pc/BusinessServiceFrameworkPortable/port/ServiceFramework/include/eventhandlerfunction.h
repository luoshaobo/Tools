#ifndef EVENT_HANDLER_FUNCTION_H__
#define EVENT_HANDLER_FUNCTION_H__


#include "eventhandlerfunctionbase.h"

template <class T, class EventT>
class EventHandlerFunction : public EventHandlerFunctionBase
{
public:
  typedef void (T::*MemberFunc)(EventT*);
  EventHandlerFunction(T* instance, MemberFunc memFn) : m_instance(instance), m_function(memFn)
  {
  };

  void call( const Event* event )
  {
    (m_instance->*m_function)( static_cast< EventT* >( event ) );
  }

  bool operator == (const EventHandlerFunction& rhs) const
  {
      return m_instance == rhs.m_instance && m_function == rhs.m_function;
  }

private:
  T* m_instance;
  MemberFunc m_function;
};


#endif //EVENT_HANDLER_FUNCTION_H__