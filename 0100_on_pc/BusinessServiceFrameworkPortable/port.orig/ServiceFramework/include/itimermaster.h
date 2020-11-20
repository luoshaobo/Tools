/**
* @file
*          ITimerMaster.h
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

#ifndef ITIMER_MASTER_H__
#define ITIMER_MASTER_H__


#include "timer.h"

/**
  * /interface ITimerMaster
  * defines method which a component has to implement that is responsible to handle
  * timers which a IStatemachineTimerSlave will start/stop/update.
  * It has to implement the logic to handle the spawned timers from the slave and
  * trigger them when a timer expires. Therefore it has also structure which are
  * helpful to organize the timers.
*/
template<class TSlave, class TExpectedEvent>
class ITimerMaster
{
public:
  ITimerMaster(){};
  virtual ~ITimerMaster(){};

  /**
    * startTimer creates a new timer which informs the according strategy if expired with the expectedEvent
    * @param[in]   IStatemachine * pStatemachine, is the pointer to the object which shall be informed after expiration
    * @param[in]   unsigned int timeInSeconds, the number of seconds to expiration
    * @param[in]   IStatemachine::Events expectedEvent, the event the strategy expects
    * @return      TimerId, this timerId can be used to stopTimer
    * @note        resolution if 1 seconds
    * @see also   stopTimer, TimerCredentials
    */
  virtual Timer startTimer(TSlave * pStatemachine, TimeElapse::Difference timeoutMicroSeconds, TExpectedEvent &expectedEvent) = 0;

  /**
    * stopTimer stops a spawned timer
    * @param[in]   IStatemachine * pStatemachine, is the pointer to the object which spawned the timer
    * @param[in]   TimerId id, the timerId which shall be stopped
    * @return      bool, true when stopped, else false
    * @note
    * @see also   startTimer, TimerCredentials
    */
  virtual bool stopTimer(TSlave* pStragtegy, Timer::TimerId id) = 0;

  /**
    * getUtcTimestamp returns the 64bit timestamp for the current UTC time
    * @return      time_t, which represent the UTC timestamp
    * @note
    * @see also   startTimer, TimerCredentials
    */
  virtual long long getUtcTimestamp() const = 0;

protected:
};

#endif // ITIMER_MASTER_H__
