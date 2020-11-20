/**
* @file
*          StatemachineMaster.h
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

#ifndef STATE_MACHINE_MASTER_H__
#define STATE_MACHINE_MASTER_H__


#include "itimermaster.h"
#include "istatemachine.h"
#include "istatemachinemaster.h"
#include "threadeventqueue.h"
#include <list>
#include "eventstatemachineresult.h"


//@brief: StatemachineMaster is the component which is able to run abitrary number of statemachines in parallel.
//        Statemachine itself can create substatemachines (parent->child) for which they are waiting or they can start
//        parallel and unrelated Statemachines in parallel.

class Lib_API StatemachineMaster : public IStatemachineMaster,
                           public ITimerExpired,
                           public ITimerMaster<IStatemachine, Event>
{
public:
  StatemachineMaster(ThreadEventQueue *pThreadEventQueue);
  virtual ~StatemachineMaster();

  //start or stop statemachines
  void startStatemachine(IStatemachine *pStatemachine, TimeElapse::Difference timeoutMicroSeconds, IStatemachine::StatemachineId parentStatemachineId);
  void startStatemachine(IStatemachine *pStatemachine, TimeElapse::Difference timeoutMicroSeconds);
  void abortStatemachine(IStatemachine::StatemachineId id);

  IStatemachine* getStatemachine(IStatemachine::StatemachineId id);

  /**
  * getStatemachineFromList try to find the according statemachine from the list.
  * @param[in]
  * @return
  * @note
  * @see also  m_listStrategies
  */
  IStatemachine * getStatemachineFromList(IStatemachine::StatemachineId id) const;

  /**
  * getNewStatemachineId returns a new statemachineId
  * @param[in]
  * @return      IStatemachine::StatemachineId
  * @note        each instantiated statemachine object must have a unique id which identifies it
  * @see also
  */
  IStatemachine::StatemachineId getNewStatemachineId();

  /**
  * onEvent as well as onThreadEvent forward the Event to each single existing statemachine
  * What each statemachine is doing with it is the responsibility of each statemachine.
  * @param[in]   const Event* pEv, baseclass pointer of the concrete Event
  * @return      void
  * @note
  * @see also
  */
  void onEvent(const Event* pEv);
  void onThreadEvent(const Event* pEv);
  /**
  * removeStatemachine removes a statemachine-object out of the collection.
  * bDelete-parameter defines if the objectpointer is simply erased out of the collection
  * or if additionally the object itself will be deleted
  * @param[in]   IStatemachine::StatemachineId, is the unique id of the statemachine to remove
  * @param[in]   bDelete, indicates if the object itself shall be deleted
  * @return
  * @note
  * @see also
  */
  void removeStatemachine(IStatemachine::StatemachineId id, bool bDelete);

  /**
  * isStatemachineStillExisting checks if a IStatemachine-object is still enlisted in the collection-list
  * @param[in]   IStatemachine * , is the pointer to the object we are looking up in the collection
  * @return      bool, true when still enlisted, false if not
  * @note
  * @see also
  */
  bool isStatemachineStillExisting(IStatemachine *pStatemachine);
  bool isStatemachineStillExisting(StatemachineType type, IStatemachine *&pStatemachine);

protected:

  void handleEventStatemachineResult(const EventStatemachineResult * ev);

  Timer startTimer(IStatemachine * pStatemachine, TimeElapse::Difference timeoutMicroSeconds, Event &expectedEvent);
  bool stopTimer(IStatemachine* pStatemachine, Timer::TimerId id);
  void stopAllTimersOfStatemachine(IStatemachine::StatemachineId  id);
  void handleTimeout(int  timerId);

  long long getUtcTimestamp() const;

  void printStatemachines() const;


  void fireEvent(IStatemachine::StatemachineId id, Event &sendEvent);
  void fireStatemachineResult(IStatemachine::StatemachineId id, StatemachineType type, IStatemachine::StatemachineResult result);

    /**
  * addStatemachineToList adds a instantiated IStatemachine to a collection-list
  * @param[in]   IStatemachine*, is a object of a statemachine that is derived from that baseclass
  * @return
  * @note
  * @see also  m_listStrategies
  */
  void addStatemachineToList(IStatemachine *pStatemachine);

  /**
  * isStatemachineStarted checks whether a special type of statemachine is already in execution.
  * Also when it is possible to start an arbitrary number of strategies in parallel,
  * it makes not always sense to do that.
  * For instance if a Statemachine is started which job is to register the target
  * at the backend, there is not much sense to have a second instance which executes the same
  * in parallel.
  * @param[in]    type define the StatemachineType of which we are interessted
  * @parma[inout]
  * @return       bool, true indicates strStatemachine in execution, false means its not in the collection listed yet
  * @note
  * @see
  */
  bool isStatemachineStarted(StatemachineType type, IStatemachine *&pStatemachine) const;

protected:
  /**
  * isStatemachineAChild checks if a strategy-object of id is a child object from a parent strategy
  * @param[in]   id, is the unique identifier of a statemachine
  * @return      bool, true if it is a child of a parent
  * @note
  * @see also
  */
  bool isStatemachineAChild(const IStatemachine::StatemachineId id) const;

  /**
  * isStatemachineAParent checks if a strategy-object of id is a parent object from a child strategy
  * @param[in]   id, is the unique identifier of a statemachine
  * @return      bool, true if it is a parent
  * @note
  * @see also
  */
  bool isStatemachineAParent(IStatemachine::StatemachineId id) const;

  /**
  * findChildOfParent try to find the child of a parent
  * @param[in]   parentId, is the unique identifier of a parent statemachine
  * @param[out]  childId, is the unique identifier of a child statemachine
  * @return      bool, true if it is found
  * @note
  * @see also
  */
  bool findChildOfParent(const IStatemachine::StatemachineId parentId, IStatemachine::StatemachineId &childId) const;

  /**
  * findParentOfChild try to find the parent of a child
  * @param[in]   childId, is the unique identifier of a child statemachine
  * @param[out]  parentId, is the unique identifier of a parent statemachine
  * @return      bool, true if it is found
  * @note
  * @see also
  */
  bool findParentOfChild(const IStatemachine::StatemachineId childId, IStatemachine::StatemachineId &parentId) const;
  void testParentChild();

  /**
  * printParentChild relations for test purposes
  * @note
  * @see also
  */
  void printParentChild() const;

  /**
  * removeParentChild removes all child statemachines from the mentioned parent id
  * @note
  * @see also
  */
  void removeParentChild(IStatemachine::StatemachineId id);

protected:
  struct TimerCredentials
  {
     //constructor
    TimerCredentials(Event::Ptr ev) : timer(), pStatemachine(0), expectedEvent(ev) {};

    Timer timer;
    IStatemachine *pStatemachine;
    Event::Ptr expectedEvent;
  };

  struct StatemachineAffinity
  {
    IStatemachine::StatemachineId parentId;
    IStatemachine::StatemachineId childId;
  };

protected:
  //remember the current running strategies
  std::list< IStatemachine* >     m_listStrategies;

  //remember the current spawned timers
  std::list<TimerCredentials> m_timerCredentials;

  //remember the relations of the strategies parent<->child
  std::list<StatemachineAffinity> m_statemachineAffinity;

  //remember if there is a TheadEventQueue assigned. This is needed in case asynchronous events are necessary
  //         from some statemachines
  ThreadEventQueue *m_pThreadEventQueue;

  //remember the last used statemachine Id. Each new created statemachine needs to get a unqiue statemachineId
  IStatemachine::StatemachineId  m_statemachineId;
};

#endif // STATE_MACHINE_MASTER_H__
