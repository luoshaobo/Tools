/**
 * @file
 *          eventstatemachinecontrol.h
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

#ifndef EVENT_STATEMACHINECONTROL_H__
#define EVENT_STATEMACHINECONTROL_H__

#include "event.h"
#include "operatingsystem.h"

class EventStatemachineControl : public Event
{
public:
  enum Trigger
  {
    EVENT_UNINITED,
    EVENT_START
  };

  EventStatemachineControl();
  EventStatemachineControl(Trigger value);
  virtual ~EventStatemachineControl();

  /**
  * toString is used to get a string for an arbitrary value.
  * It is static so no instantiated object must exists for that call.
  * @param[in]     value, for which a string should be retrieved
  * @return        const char* which consists of a zero terminated string representing that value
  */
  static const char* toString(Trigger trigger);

  /**
  * toString is used to get a string for current value of that instanted object.
  * @param[in]
  * @return        const char* which consists of a zero terminated string representing that value
  */
  const char *toString() const;

  /**
  * getValue is used to get the current event value
  * @param[in]
  * @return        Value, is the current set event value
  */
  Trigger getTrigger() const;
  void setTrigger(Trigger trigger);

  const char* getName() const { return "EventStatemachineControl"; }

  Event* clone() const
  {
    return new EventStatemachineControl(*this);
  }

protected:
  Trigger m_eTrigger;
};

#endif // EVENT_STATEMACHINECONTROL_H__
