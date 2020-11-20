/**
* @file
*          ITimerSlave.h
* @brief
*          Header file
* @author  (last changes):
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
*              Elmar Weber              Implementation of first version
* @par Copyright Notice:
* Copyright (C) Continental AG 2014
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/

#ifndef ITIMER_SLAVE_H__
#define ITIMER_SLAVE_H__

#include "operatingsystem.h"

/**
  * /interface ITimerSlave
  * define methods which a component has to implement in order to work with timers.
  * This slave is hereby connecting to a IStrategyTimerMaster which controls the
  * different timers and send a trigger when the according timer expires.
*/
template<class TExpectedEvent>
class Lib_API ITimerSlave
{
public:
  ITimerSlave(){};
  virtual ~ITimerSlave(){};

  /**
    * startTimer creates a new timer which informs the according strategy if expired with the expectedEvent
    * @param[in]   unsigned int timeInSeconds, the number of seconds to expiration
    * @param[in]   IStrategy::Events expectedEvent, the event the strategy expects
    * @return      TimerId, this timerId can be used to stopTimer
    * @note        resolution if 1 seconds
    * @see also   stopTimer, TimerCredentials
    */
  virtual Timer startTimer(TimeElapse::Difference timeoutMicroSeconds, TExpectedEvent &expectedEvent) = 0;

  /**
    * stopTimer stops a spawned timer
    * @param[in]   TimerId id, the timerId which shall be stopped
    * @return      bool, true when stopped, else false
    * @note
    * @see also   startTimer, TimerCredentials
    */
  virtual bool stopTimer(Timer::TimerId id) = 0;
};

#endif // ITIMER_SLAVE_H__
