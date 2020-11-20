/**
 * @file
 *          eventsatemachineresult.h
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

#ifndef EVENT_STATEMACHINE_RESULT_H__
#define EVENT_STATEMACHINE_RESULT_H__

#include "event.h"
#include "istatemachine.h"
#include "operatingsystem.h"


class Lib_API EventStatemachineResult : public Event
{
public:

  EventStatemachineResult();
  EventStatemachineResult(IStatemachine::StatemachineId id, StatemachineType type, IStatemachine::StatemachineResult result);
  virtual ~EventStatemachineResult();


  void setResult(IStatemachine::StatemachineId id, StatemachineType type, IStatemachine::StatemachineResult result);
  IStatemachine::StatemachineResult getStatemachineResult() const;
  StatemachineType getStatemachineType() const;
  IStatemachine::StatemachineId getStatemachineId() const;

  const char* getName() const { return "EventStatemachineResult"; }

  Event* clone() const
  {
    return new EventStatemachineResult(*this);
  }

protected:
  // StatemachineId carries the information from which strategy this message origins
  IStatemachine::StatemachineId m_statemachineId;

  // StatemachineType define the type of the statemachine
  StatemachineType m_statemachineType;

  // result tells the datafusionmanger whether the strategy was executed with success or not
  IStatemachine::StatemachineResult    m_result;
};

#endif // EVENT_STATEMACHINE_RESULT_H__
