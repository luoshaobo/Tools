#ifndef EVENT_HANDLER_FUNCTION_BASE_H__
#define EVENT_HANDLER_FUNCTION_BASE_H__

#include "operatingsystem.h"

class Event;

class Lib_API  EventHandlerFunctionBase
{
public:
  virtual ~EventHandlerFunctionBase() {};
  void execute( const Event* event )
  {
    call( event );
  }

private:
  virtual void call( const Event* ) = 0;
};

#endif  // EVENT_HANDLER_FUNCTION_BASE_H__
