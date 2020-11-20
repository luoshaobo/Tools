/**
* @file    ilifecycle.h
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
* Copyright (C) Continental AG 2017
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/

#ifndef ILIFE_CYCLE_H__
#define ILIFE_CYCLE_H__


#include "operatingsystem.h"

class Lib_API ILifeCycle
{
public:
  enum State
  {
    STATE_NOT_INIT,                // component not initialized

    STATE_INITIALIZE_REQ,          // component initialization requested
    STATE_ACTIVATE_REQ,            // component activation requested
    STATE_DEACTIVATE_REQ,          // component deactivation requested
    STATE_TERMINATE_REQ,           // component termination requested

    STATE_INITIALIZED,             // component initialization confirmed
    STATE_ACTIVATED,               // component activation confirmed
    STATE_DEACTIVATED,             // component deactivation confirmed
    STATE_TERMINATED,              // component termination confirmed

    STATE_INITIALIZE_FAILED,       // component initialization failed
    STATE_ACTIVATE_FAILED,         // component activation failed
    STATE_DEACTIVATE_FAILED,       // component deactivation failed
    STATE_TERMINATE_FAILED         // component termination failed
  };


  ILifeCycle();
  virtual ~ILifeCycle();

  //----- lifecycle interface
  virtual void initialize() = 0;
  virtual void activate() = 0;
  virtual void deactivate() = 0;
  virtual void terminate() = 0;
  virtual void setState(State state);
  virtual State getState() const;
  //virtual void changeConfigMode(int mode) = 0;

  //----- confirmation to supervisor
  //void confirmLifeCycle(STATE state);

  //----- control components power needs (stay awake reasons)
  /*
  // called by component to request 'stay-alive' for a dedicated reason;
  bool setStayAliveReason(SUPV_STAY_ALIVE reason);
  // called by component to revoke 'stay-alive-request' for a dedicated reason;
  // if all stay-alive-reasons are cleared the component does not request to stay alive
  bool clearStayAliveReason(SUPV_STAY_ALIVE reason);
  // return the actual 'stay-alive-reasons' for this component; without parameters this routine
  // returns all, otherwise only the reasons determined with the bitmask <reason>
  int getStayAliveReason(SUPV_STAY_ALIVE reason = SUPV_STAY_ALIVE_ALL_REASONS);
*/


protected:
  State m_eState;

};

#endif  // ILIFE_CYCLE_H__
