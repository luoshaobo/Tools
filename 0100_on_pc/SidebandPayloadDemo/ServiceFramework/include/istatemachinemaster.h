/**
 * @file
 *          istatemachinemaster.h
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
 * Copyright (C) Continental AG 2015
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#ifndef ISTATE_MACHINE_MASTER_H__
#define ISTATE_MACHINE_MASTER_H__

#include "istatemachine.h"
#include "operatingsystem.h"

/**
* @class IStatemachineMaster
* @brief This interface has to be implemented by a class which takes responsibilities
*        to manage and operate strategy-objects.
*        By passing that IStatemachineMaster pointer to a strategy-object,
*        that strategy is able to create substrategies which either run completely
*        in parallel for its own or as a parent-child relation, which means as soon as the
*        parent will be destroyed for some reason, also the child
*        and child-childs will be destroyed.
*        An example could be, that a strategy has to do some arbitrary work. As
*        a result it has to send a result message to a server. Here that strategy
*        could create a child-strategy which sends that message. The parent
*        will be notified when the child terminates and continue its work.
*/

class Lib_API IStatemachineMaster
{
public:
    /**
    * The methods startStatemachine is used to start a new strategy out of a executed strategy.
    * It has two options.
    * Either the parent strategy starts the new strategy with
    * telling the parent-StatemachineId, then as soon as the parent terminates for some reasons
    * the childs will be terminated by the master as well.
    * Or the parent strategy simply start the new strategy without mention the parent
    * StatemachineId, then this new strategy has no relation to that "parent" and run
    * as long it terminates by itself. If that "parent" terminates it does not influence
    * the created one.
    * @param[in]   pStatemachine, the new (child) created strategy
    * @param[in]   secondsToStart, define when the created strategy shall start running.
    *              Zero means immediately
    * @param[in]   parentStatemachineId, if used a parent-child relation is created
    * @return      void
    * @note        In a parent-child relation the child will be destroyed when the parent
    *              gets destroyed
    * @see also
    */
  virtual void startStatemachine(IStatemachine *pStatemachine, TimeElapse::Difference timeoutMicroSeconds, IStatemachine::StatemachineId parentStatemachineId) = 0;
  virtual void startStatemachine(IStatemachine *pStatemachine, TimeElapse::Difference timeoutMicroSeconds) = 0;
  virtual void abortStatemachine( IStatemachine::StatemachineId id ) = 0;

  virtual void fireEvent(IStatemachine::StatemachineId id, Event &sendEvent) = 0;

  //inform master
  virtual void fireStatemachineResult(IStatemachine::StatemachineId id, StatemachineType type, IStatemachine::StatemachineResult result) = 0;

    /**
    * The methods getStatemachine is used to retrieve the pointer to the object of an
    * instantiated strategy, for instance the parent or a child strategy.
    * If the return value is null then the according strategy does not exist anymore.
    * @param[in]   id, tells which pointer to a strategy shall be retrieved
    * @return      IStatemachine*, pointer to an instantiated strategy.
    * @note
    * @see also
    */

  virtual IStatemachine* getStatemachine(IStatemachine::StatemachineId id) = 0;

    /**
    * The methods getStatemachineId should be used to get a strategy id for a
    * new instantiated strategy. The master generates unique ids to ensure
    * that no strategies with same id exists, which would make trouble!
    * @return      StatemachineId, a unique id for an new instantiated strategy
    * @note
    * @see also
    */
  virtual IStatemachine::StatemachineId getNewStatemachineId() = 0;

  /**
  * isStrategyStillExisting checks if a strategy-object is still enlisted in the collection-list
  * @param[in]   IStrategy * pStrategy, is the pointer to the object we are looking up in the collection
  * @return      bool, true when still enlisted, false if not
  * @note
  * @see also
  */
  virtual bool isStatemachineStillExisting(IStatemachine *pStatemachine) = 0;
  virtual bool isStatemachineStillExisting(StatemachineType type, IStatemachine *&pStatemachine) = 0;
};


#endif // ISTATE_MACHINE_MASTER_H__
