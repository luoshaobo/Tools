#include "istatemachine.h"
#include "statemachinemaster.h"
#include "eventstatemachinecontrol.h"
#include "typeinfo.h"
#include "comalog.h"

IStatemachine::IStatemachine(ITimerMaster<IStatemachine, Event> *pITimerMaster,
  IStatemachineMaster *pIStatemachineMaster,
  StatemachineId id, StatemachineType type) :
  m_eStatemachineType(type),
                                        m_pITimerMaster( pITimerMaster ),
                                        m_pIStatemachineMaster( pIStatemachineMaster ),
                                        m_StatemachineIdId(id),
                                        m_bTerminated( false ),
                                        m_bAbortWhenPossible( false ),
                                        m_bIsStarted( false ),
                                        m_bStatemachineResult(RESULT_UNDEFINED),
                                        m_bResultFired( false )
{
}

IStatemachine::~IStatemachine()
{
}

IStatemachineMaster* IStatemachine::getStatemachineMaster()
{
  return ( m_pIStatemachineMaster );
}

IStatemachine::StatemachineId  IStatemachine::getStatemachineId() const

{
  return (m_StatemachineIdId);
}

StatemachineType IStatemachine::getStatemachineType() const
{
  return (m_eStatemachineType);
}

bool IStatemachine::isTerminated() const
{
  return ( m_bTerminated );
}

void IStatemachine::onEvent(const Event *pEvent)
{
  TypeInfo info(typeid(*pEvent));

  if (typeid(*pEvent) == typeid(EventStatemachineControl))
  {
    const EventStatemachineControl& evStatemachine = static_cast<const EventStatemachineControl&>(*pEvent);

    COMALOG(COMALOG_VERBOSE, "%s[%d]::%s event = %s\n", this->getStatemachineName()
      , this->getStatemachineId()
      , this->getStateName()
      , evStatemachine.toString());

    if (isStarted() == true || evStatemachine.getTrigger() == EventStatemachineControl::EVENT_START)
    {
      //status is started in case a EVENT_START occurs
      m_bIsStarted = true;

      //call the concrete Start-method of the statemachine.
      onStart();
    }
  }

  //call concrete statemachine
  onEventHandler(pEvent);
}

Timer IStatemachine::startTimer(TimeElapse::Difference timeoutMicroSeconds, Event &expectedEvent)
{
  Timer ret = m_pITimerMaster->startTimer(this, timeoutMicroSeconds, expectedEvent);
  COMALOG(COMALOG_VERBOSE,  "%s[%d]::%s start timer %d expires in %lld milliseconds\n",this->getStatemachineName(),
                                                                                              this->getStatemachineId(),
                                                                                              this->getStateName(),
                                                                                              ret.getId(),
                                                                                              timeoutMicroSeconds/1000);

  return ret;
}

bool IStatemachine::stopTimer(Timer::TimerId id)
{
  //only trace valid ids
  if ( id != 0 )
  {
    COMALOG(COMALOG_VERBOSE, "%s[%d]::%s stop timerid=%d\n", this->getStatemachineName(),
      this->getStatemachineId(),
      this->getStateName(), id);
  }

  return m_pITimerMaster->stopTimer( this, id );
}


void IStatemachine::fireResult(StatemachineResult resultStatus)
{
  COMALOG(COMALOG_INFO,  "%s[%d]::%s notifies %s\n", this->getStatemachineName(),
    this->getStatemachineId(),
    this->getStateName(),
    (RESULT_SUCCESSFUL == resultStatus) ? "SUCCESS" : "FAILURE" );

  if (m_bResultFired == false)
  {
    //remember that we have already fired the result status
    m_bResultFired = true;

    //remember the sent status
    m_bStatemachineResult = resultStatus;

    //tell IStatemachineMaster about the result
    m_pIStatemachineMaster->fireStatemachineResult(getStatemachineId(), getStatemachineType(), resultStatus);
  }
  else
  {
    COMALOG(COMALOG_DEBUG, "%s[%d]::%s has already fired the result!\n",this->getStatemachineName(),
      this->getStatemachineId(),
      this->getStateName());
  }
}

bool IStatemachine::start()
{
  bool bIsStateMachineStarted = true;

  //check if the statemachine is already running
  if ( m_bIsStarted == false )
  {
    COMALOG(COMALOG_VERBOSE, "Starting %s[%d]::%s ....\n", this->getStatemachineName(),
      this->getStatemachineId(),
      this->getStateName());

    //we enqueue an event which starts the statemachine
    m_bIsStarted = true;

    //send a START-signal to the according state
    EventStatemachineControl ev(EventStatemachineControl::EVENT_START);
    onEvent( &ev );
  }

  return ( bIsStateMachineStarted );
}
