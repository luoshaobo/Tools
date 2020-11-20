/**
 * @file
 *          istatemachine.h
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

#ifndef ISTATE_MACHINE_H__
#define ISTATE_MACHINE_H__

#include "statemachinetype.h"
#include "event.h"
#include "timer.h"
#include "timeelapse.h"
#include "itimerslave.h"
#include "itimermaster.h"
#include "operatingsystem.h"


//forward declararion
class IStatemachineMaster;

/**
* /interface IStatemachine
* @brief The class IStatemachine is the baseclass from which all implemented statemachines/strategies
*        must be derived.
*
* The class is an abstract class and can therefore not be instantiated.
* It provides the basic contract how the derived statemachines have to "look like".
* It provides the general methods which are necessary
*/
class Lib_API IStatemachine : public ITimerSlave<Event>
{
public:
  //type to uniquely identifiy a statemachine
  typedef int StatemachineId;

  enum StatemachineResult
  {
    RESULT_UNDEFINED,
    RESULT_SUCCESSFUL,
    RESULT_FAILED
  };

public:
  /**
    * IStatemachine is the constructor for this class.
    * It needs the pointer to the associated DataFusionManager. Additionally
    * the RegistrationData  as well as the assigned statemachineId that the object can
    * uniquely identified by different executed requests from the different states.
    * The RegistrationData is a protobuf-message which is serialized into a file.
    * It stores information which registering data was sent to the backend and
    * after successful registering it stores also the retrieved data which have
    * to be used in subsequential calls to the backend services.
    * @param[in]   pDataFusionManager
    * @param[in]   RegistrationData
    * @param[in]   id, a unique id identifying this statemachine
    * @return
    * @note
    * @see also
    */
  IStatemachine(ITimerMaster<IStatemachine, Event> *pITimerMaster,
             IStatemachineMaster *pIStatemachineMaster,
             StatemachineId id, StatemachineType type);
  virtual ~IStatemachine();

  /**
  * onEvent is the main event handler of the strategies which takes all kinds of events
  * @param[in]   Event, reference to the baseclass of the event message
  * @return      void
  * @note
  * @see also
  */
  void onEvent( const Event *pEvent );

  /**
  * getStateName returns the name of the current state
  * @return      const char* is the zero terminated string of the statename
  * @note
  * @see also   getStatemachineName
  */
  virtual const char* getStateName() = 0;

  /**
  * getStatemachineName returns the name of the statemachine
  * @return      const char* is the zero terminated string of that statemachine
  * @note
  * @see also   getStateName
  */
  virtual const char* getStatemachineName() = 0;

  /**
  * getStatemachineId returns the unique id assigned to that Statemachine.
  * @return     StatemachineId, the unique id of that instance
  * @note       Each statemachine which is instantiated should get an unique StatemachineId in order being able
  *             to distinguish between them.
  * @see also   getStatemachineType
  */
  virtual StatemachineId getStatemachineId() const;

  /**
  * getStatemachineType returns the type of that statemachine. This is usually used to be able making downcasts without having
  * RTTI (RunTimeTypeInformation).
  * @return     StatemachineType, identifying the type of the statemachine
  * @note       Each statemachine constructor must init that value properly
  * @see also   StatemachineType
  */
  virtual StatemachineType getStatemachineType() const;

  /**
  * start is starting the statemachine.
  * Usually strategies are after instantiating in IDLE state. In order to start the statemachine
  * the statemachine expects the EventStatemachine:EVENT_START-event. This is inserted to the statemachine
  * by calling the start-method
  * @return     bool, signals if the statemachine is started
  * @note
  * @see also   getStateName
  */
  virtual bool start();

  /**
  * isStarted is returning if the statemachine is still in IDLE or already in execution.
  * Here it at least once get the EVENT_START either via event or via start()-method.
  * @return     bool, signals if the statemachine is started
  * @note
  * @see also   start
  */
  virtual bool isStarted() const { return m_bIsStarted; }

  /**
  * isTerminated is returning if the statemachine is terminated/finished/aborted.
  * @return     bool, signals if the statemachine is terminated
  * @note
  * @see also   setTerminated
  */
  virtual bool isTerminated() const;


  /**
  * abortExecution is aborting the statemachine.
  * This method also handles the aborting of current requests probably in execution, like DataCommunication-requests.
  * Finally it also fires the result to the DataFusionManager that it knows that the statemachine is terminated/aborted now.
  * Usually this is done when system-states like "system in shutdown" are appearing
  * @return     bool, signals if the statemachine is aborted
  * @note
  * @see also   fireResult, setTerminated
  */
  virtual bool abortExecution()=0;


  virtual StatemachineResult getResult() const { return m_bStatemachineResult; };

  //void setRequestor(CommunicatorId communicatorId, RequestId requestId);
  //CommunicatorId getCommunicatorId() const;
  //RequestId getRequestId() const;



  /**
  * getStatemachineMaster returns the instance which is responsible for handling the strategies.
  * @param[in]  IStatemachineMaster*, returns the instance of the StatemachineMaster
  * @note
  * @see also   getResult
  */
  virtual IStatemachineMaster* getStatemachineMaster();

protected:
  /**
  * fireResult notifies the DataCommunication-service about the result of that statemachine.
  * After that notification the DataCommunication will delete that object usually.
  * This method also handles the aborting of current requests probably in execution, like DataCommunication-requests.
  * Finally it also fires the result to the DataFusionManager that it knows that the statemachine is terminated/aborted now.
  * Usually this is done when system-states like "system in shutdown" are appearing
  * @param[in]  bool, signals if the status of that statemachine
  * @note
  * @see also   getResult
  */
  virtual void fireResult(StatemachineResult resultStatus);

  virtual void onEventHandler(const Event *pEvent) = 0;

  /**
  * onStart is called when the StatemachineMaster starts the statemachine.
  * The Statemachine itself can use this function to trigger its initial-state and launch the statemachine.
  * @param[in]
  * @note
  * @see also
  */
  virtual void onStart() = 0;

  virtual void setTerminated() { m_bTerminated = true; }

  /**
  * startTimer creates a new timer which informs the according statemachine if expired with the expectedEvent
  * @param[in]   unsigned int timeInSeconds, the number of seconds to expiration
  * @param[in]   Event expectedEvent, the event the statemachine expects
  * @return      TimerId, this timerId can be used to stopTimer
  * @note        resolution if 1 seconds
  * @see also   stopTimer, TimerCredentials
  */
  Timer startTimer(TimeElapse::Difference timeoutMicroSeconds, Event &expectedEvent);

  /**
  * stopTimer stops a spawned timer
  * @param[in]   TimerId id, the timerId which shall be stopped
  * @return      bool, true when stopped, else false
  * @note
  * @see also   startTimer, TimerCredentials
  */
  bool stopTimer(Timer::TimerId id);

protected:


  StatemachineType                             m_eStatemachineType;
  ITimerMaster<IStatemachine, Event>          *m_pITimerMaster;
  IStatemachineMaster                         *m_pIStatemachineMaster;
  StatemachineId                               m_StatemachineIdId;
  //interface which is provide important instances
  //IInstanceFactory*    m_pInstanceFactory;
  bool                                     m_bTerminated;
  bool                                     m_bAbortWhenPossible;
  bool                                     m_bIsStarted;
  StatemachineResult                       m_bStatemachineResult;
  bool                                     m_bResultFired;
};


#endif // ISTATE_MACHINE_H__
