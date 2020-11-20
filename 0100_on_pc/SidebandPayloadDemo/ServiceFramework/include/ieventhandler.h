/**
 * @file
 *
 * @brief
 *          Header file
 * @author  (last changes):
 *          - Elmar Weber
 *          - elmar.weber@continental-corporation.com
 *          - Continental AG
 * @par Project:
 * @par SW-Package:
 *
 * @par SW-Module:
 *
 * @note
 *
 * @par Module-History:
 *  Date        Author                   Reason
 *
 * @par Copyright Notice:
 * Copyright (C) Continental AG 2014
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#ifndef INTERFACE_EVENT_HANDLER_H__
#define INTERFACE_EVENT_HANDLER_H__

#include "event.h"

/*
 EventHandler is responsible for mapping from the type of event to the proper method that handles that event.
*/
class IEventHandler
{
public:
  virtual ~IEventHandler(){};
  virtual void handleEvent( const Event* ) = 0;

    /*
      registerEventHandler is registering a method which is responsible to handle a concrete event type when it is
      notified by a subject. Therefore the TypeInfo for that event-type is used as key and the belonging eventhandler-
      method is stored as a value in a std::map.
    */
  template <class T, class EventT>
  virtual void registerEventHandler(T*, void (T::*memFn)(EventT*)) = 0;
};

#endif // INTERFACE_EVENT_HANDLER_H__
