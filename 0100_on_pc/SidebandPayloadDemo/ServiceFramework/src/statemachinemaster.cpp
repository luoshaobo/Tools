#include "statemachinemaster.h"
#include "eventstatemachinecontrol.h"
#include "comalog.h"

//for demo and its simplicity only !!
extern void printInfo();

StatemachineMaster::StatemachineMaster(ThreadEventQueue *pThreadEventQueue) : m_pThreadEventQueue(pThreadEventQueue),
                                                                              m_statemachineId(0)
{
  pThreadEventQueue->m_EventHandler.registerEventHandler(this, &StatemachineMaster::handleEventStatemachineResult);
};

StatemachineMaster::~StatemachineMaster()
{
};

void StatemachineMaster::handleEventStatemachineResult(const EventStatemachineResult * ev)
{
  COMALOG(COMALOG_INFO, "StatemachineMaster::handleEventStatemachineResult: id:%d result:%d \n", ev->getStatemachineId(), ev->getStatemachineResult());

  //tell all strategies, that a statemachine has finished.
  //Especially parent strategies could be interessted on some results
  //CHECK EventStatemachineResult evParallelResult(;
  onEvent(ev);

  //we do not automatically remove it. The responsible of the ThreadEventQueue will do it in order to have the change
  //to query some detailed results of the according statemachine object.
  //removeStatemachine(ev->getStatemachineId(), true);
}

void StatemachineMaster::onEvent(const Event* pEv)
{
  //inform strategies
  list< IStatemachine* >::iterator it;
  for (it = m_listStrategies.begin(); it != m_listStrategies.end(); ++it)
  {
    (*it)->onEvent(pEv);
  }
}

void StatemachineMaster::onThreadEvent(const Event* pEv)
{
  //inform strategies
  list< IStatemachine* >::iterator it;
  for (it = m_listStrategies.begin(); it != m_listStrategies.end(); ++it)
  {
    (*it)->onEvent(pEv);
  }
}

void StatemachineMaster::removeStatemachine(IStatemachine::StatemachineId id, bool bDelete)
{
  std::list< IStatemachine* >::iterator it;
  std::list< IStatemachine* >::iterator itErase;
  bool bFound = false;
  bool bStillAParent = false;

  std::string statemachineName;
  IStatemachine *p = getStatemachineFromList(id);

  if (p != 0)
  {
    statemachineName = p->getStatemachineName();
  }

  COMALOG(COMALOG_INFO, "removeStatemachine %s[%d]\n", statemachineName.c_str(), id);

  do
  {
    //query if the id is a parent of a child
    bStillAParent = isStatemachineAParent(id);

    //if yes, we delete the childs via recursion
    if (bStillAParent == true)
    {
      //if there is a child, we delete the childid, otherwise the current id
      IStatemachine::StatemachineId childId = 0;
      if (findChildOfParent(id, childId) == true)
      {
        //delete the child
        removeStatemachine(childId, true);
      }
      else
      {
        //delete the paramter
        removeStatemachine(id, true);
      }
    }
  } while (bStillAParent == true);

  //here we should have deleted all childs if there were some via recursion
  bool bDeleteChild = false;
  IStatemachine::StatemachineId parentId = 0;
  if (findParentOfChild(id, parentId) == true)
  {
    IStatemachine* p = getStatemachineFromList(id);
    if (p != 0 /*&& p->isTerminated()*/)
    {
      bDeleteChild = true;
    }
  }

  //if this object we want to delete now is a child of a parent statemachine
  //we won't delete it here, because it could be that the parent likes to know
  //some information. therefore we finally delete it when the parent will go
  if (isStatemachineAChild(id) == false || bDeleteChild == true)
  {
    //try to find according statemachine
    for (it = m_listStrategies.begin(); it != m_listStrategies.end() && bFound == false; it++)
    {
      if ((*it)->getStatemachineId() == id)
      {
        COMALOG(COMALOG_DEBUG, "Statemachinemaster remove %s[%d]\n", (*it)->getStatemachineName(), id);
        bFound = true;

        //ensure that all timers are stopped for that statemachine which will be deleted now
        stopAllTimersOfStatemachine(id);

        //remember the iterator to erase
        itErase = it;
      }
    }

    if (bFound == true && itErase != m_listStrategies.end())
    {
      IStatemachine *pDeleteObj = (*itErase);

      //and remove it from DataPacket queue
      m_listStrategies.erase(itErase);

      //we delete the statemachine itself here
      if (bDelete == true)
      {
        //remove the entry if exists
        removeParentChild(pDeleteObj->getStatemachineId());

        if (pDeleteObj->isTerminated() == false)
        {
          COMALOG(COMALOG_DEBUG, "Abort execution of %s[%d] in state=%s\n", pDeleteObj->getStatemachineName(), id, pDeleteObj->getStateName());
          pDeleteObj->abortExecution();
        }

        delete pDeleteObj;
      }
    }
  }

  printParentChild();
}

/*
void StatemachineMaster::removeStatemachine(RequestId id, bool bDelete)
{
  list< IStatemachine* >::iterator it;
  for (it = m_listStrategies.begin(); it != m_listStrategies.end(); ++it)
  {
    if ((*it)->getRequestId() == id)
    {
      removeStatemachine((*it)->getRequestId(), bDelete);
      break;
    }
  }
}*/

/*
void StatemachineMaster::testParentChild()
{

  IStrategy *p1 = 0;
  p1 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);

  IStrategy * p11 = 0;
  p11 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);
  startStrategy(p11, 100000, p1->getStrategyId());

  IStrategy * p111 = 0;
  p111 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);
  startStrategy(p111, 100000, p11->getStrategyId());

  IStrategy * p1111 = 0;
  p1111 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);
  startStrategy(p1111, 100000, p111->getStrategyId());

  IStrategy * p1112 = 0;
  p1112 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);
  startStrategy(p1112, 100000, p111->getStrategyId());

  IStrategy * p11121 = 0;
  p11121 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);
  startStrategy(p11121, 100000, p1112->getStrategyId());

  IStrategy * p112 = 0;
  p112 = new SubscribeMqttStrategy(this, 0, this->getNewStrategyId(), "", 0);
  startStrategy(p112, 100000, p11->getStrategyId());

  IStrategy::StrategyId id = p11->getStrategyId();

  removeStrategy(id, true);
}
*/

void StatemachineMaster::removeParentChild(IStatemachine::StatemachineId id)
{
  std::list<StatemachineAffinity>::iterator it = m_statemachineAffinity.begin();


  while (it != m_statemachineAffinity.end())
  {
    if ((*it).childId == id)
    {
      it = m_statemachineAffinity.erase(it);
    }
    else
    {
      it++;
    }
  }
}


void StatemachineMaster::stopAllTimersOfStatemachine(IStatemachine::StatemachineId id)
{
  COMALOG(COMALOG_INFO, "stopAllTimersOfStatemachine id:%d\n", id);

  //try to find according timer
  bool bFound = false;
  do
  {
    bFound = false;
    for (std::list< TimerCredentials >::iterator it = m_timerCredentials.begin(); it != m_timerCredentials.end(); it++)
    {
      //check if the timer belongs to the statemachineId
      if ((*it).pStatemachine != 0 && (*it).pStatemachine->getStatemachineId() == id)
      {
        bFound = stopTimer((*it).pStatemachine, (*it).timer.getId());
        break;
      }
    }
  } while (bFound == true);
}

IStatemachine::StatemachineId StatemachineMaster::getNewStatemachineId()
{
  return ++m_statemachineId;
}

void StatemachineMaster::startStatemachine(IStatemachine *pStatemachine, TimeElapse::Difference timeoutMicroSeconds, IStatemachine::StatemachineId parentStatemachineId)
{
  StatemachineAffinity currentAffinity;

  if (pStatemachine != 0)
  {
    //remember who is the parent of which starting statemachine
    //this is important in case the parent terminates or must be destroyed
    //to find out its relatives.
    currentAffinity.parentId = parentStatemachineId;
    currentAffinity.childId = pStatemachine->getStatemachineId();
    m_statemachineAffinity.push_back(currentAffinity);

    startStatemachine(pStatemachine, timeoutMicroSeconds);
  }
}

void StatemachineMaster::startStatemachine(IStatemachine *pStatemachine, TimeElapse::Difference timeoutMicroSeconds)
{
  //create a timer which sends the START-event to the statemachine which shall run after that while
  if (pStatemachine != 0)
  {
    COMALOG(COMALOG_INFO, "%s[%d] starts in %d milliseconds\n", pStatemachine->getStatemachineName()
      , pStatemachine->getStatemachineId()
      , timeoutMicroSeconds / 1000);

    //add statemachine to list
    addStatemachineToList(pStatemachine);

    if (timeoutMicroSeconds > 0)
    {

      //Timer t
      TimerCredentials credentials(new EventStatemachineControl(EventStatemachineControl::EVENT_START));
      credentials.timer = Timer(this, timeoutMicroSeconds, false);
      credentials.pStatemachine = pStatemachine;

      //remember that timer when be called back
      m_timerCredentials.push_back(credentials);

      //spawn timer
      m_pThreadEventQueue->addTimer(credentials.timer);
    }
    else
    {
      pStatemachine->start();
    }
  }
}


void StatemachineMaster::abortStatemachine(IStatemachine::StatemachineId id)
{
  IStatemachine *pStatemachine = getStatemachineFromList(id);
  if (pStatemachine != 0)
  {
    pStatemachine->abortExecution();
  }
}

IStatemachine* StatemachineMaster::getStatemachine(IStatemachine::StatemachineId id)
{
  return getStatemachineFromList(id);
}


bool StatemachineMaster::isStatemachineAChild(const IStatemachine::StatemachineId id) const
{
  bool bRet = false;

  for (std::list<StatemachineAffinity>::const_iterator it = m_statemachineAffinity.begin(); it != m_statemachineAffinity.end() && bRet == false; ++it)
  {
    if ((*it).childId == id)
    {
      bRet = true;
    }
  }

  return (bRet);
}

bool StatemachineMaster::isStatemachineAParent(IStatemachine::StatemachineId id) const
{
  bool bRet = false;

  for (std::list<StatemachineAffinity>::const_iterator it = m_statemachineAffinity.begin(); it != m_statemachineAffinity.end() && bRet == false; ++it)
  {
    if ((*it).parentId == id)
    {
      bRet = true;
    }
  }

  return (bRet);
}

bool StatemachineMaster::findChildOfParent(const IStatemachine::StatemachineId parentId, IStatemachine::StatemachineId &childId) const
{
  bool bRet = false;

  for (std::list<StatemachineAffinity>::const_iterator it = m_statemachineAffinity.begin(); it != m_statemachineAffinity.end() && bRet == false; ++it)
  {
    if ((*it).parentId == parentId)
    {
      childId = (*it).childId;
      bRet = true;
    }
  }

  return (bRet);
}

bool StatemachineMaster::findParentOfChild(const IStatemachine::StatemachineId childId, IStatemachine::StatemachineId &parentId) const
{
  bool bRet = false;

  for (std::list<StatemachineAffinity>::const_iterator it = m_statemachineAffinity.begin(); it != m_statemachineAffinity.end() && bRet == false; ++it)
  {
    if ((*it).childId == childId)
    {
      parentId = (*it).parentId;
      bRet = true;
    }
  }

  return (bRet);
}

void StatemachineMaster::printParentChild() const
{
  COMALOG(COMALOG_DEBUG, "--Statemachine affinities--\n");
  for (std::list<StatemachineAffinity>::const_iterator it = m_statemachineAffinity.begin(); it != m_statemachineAffinity.end(); ++it)
  {
    COMALOG(COMALOG_DEBUG,  "  Parent[%d] -> Child[%d]\n", (*it).parentId, (*it).childId);
  }
  COMALOG(COMALOG_DEBUG,  "--Statemachine affinities ends--\n");
}

void StatemachineMaster::handleTimeout(int  timerId)
{
  COMALOG(COMALOG_INFO, "StatemachineMaster::handleTimeout(%d)\n", timerId);

  //try to find according timer
  std::list< TimerCredentials >::iterator it;
  for (it = m_timerCredentials.begin(); it != m_timerCredentials.end(); it++)
  {
    //check if timer is expired now
    if ((*it).timer.getId() == timerId)
    {
      //call the expected event for the statemachine that spawned that timer
      //but check if the statemachine is still alive and wasn't deleted meanwhile
      if ((*it).pStatemachine != 0)
      {
        if (isStatemachineStillExisting((*it).pStatemachine))
        {
          //inform the statemachine about the expired timer
          (*it).pStatemachine->onEvent((*it).expectedEvent);
        }
        else
        {
          //remove the timer, because the according statemachine is not existing anymore
          COMALOG(COMALOG_WARN, "WARNING: This timer wasn't stopped by the spawned statemachine\n");
          m_timerCredentials.erase(it);
          break;
        }
      }
    }
  }
}

void StatemachineMaster::fireEvent(IStatemachine::StatemachineId /*id*/, Event &/*sendEvent*/)
{
  //TODO
}

void StatemachineMaster::fireStatemachineResult(IStatemachine::StatemachineId id, StatemachineType type, IStatemachine::StatemachineResult result)
{
  if (m_pThreadEventQueue != 0)
  {
    EventStatemachineResult *ev = new EventStatemachineResult(id, type, result);
    m_pThreadEventQueue->insertEvent(ev);
  }
}

Timer StatemachineMaster::startTimer(IStatemachine * pStatemachine, TimeElapse::Difference timeoutMicroSeconds, Event &expectedEvent)
{
  //create a timer
  Timer ret(this, timeoutMicroSeconds, false);

  if (pStatemachine != 0 && m_pThreadEventQueue != 0)
  {
    COMALOG(COMALOG_INFO, "startTimer add timer for %s[%d] time=%lld ev=%s\n", pStatemachine->getStatemachineName()
      , pStatemachine->getStatemachineId()
      , timeoutMicroSeconds/1000
      , expectedEvent.getName());

    TimerCredentials timer(expectedEvent.clone());
    timer.timer = ret;
    timer.pStatemachine = pStatemachine;

    //append new timer
    m_timerCredentials.push_back(timer);

    //spawn timer at threadqueue
    m_pThreadEventQueue->addTimer(ret);
  }

  return ret;
}

bool StatemachineMaster::stopTimer(IStatemachine * pStatemachine, Timer::TimerId id)
{
  std::list< TimerCredentials >::iterator it;
  std::list< TimerCredentials >::iterator itErase;
  bool bFound = false;

  //try to find according timer
  for (it = m_timerCredentials.begin(); it != m_timerCredentials.end() && bFound == false; it++)
  {
    if ((*it).pStatemachine == pStatemachine && (*it).timer.getId() == id)
    {
      COMALOG(COMALOG_INFO,  "StopTimer id=%d\n", id);
      bFound = true;

      //remember the iterator to erase
      itErase = it;
    }
  }

  if (bFound == true && m_pThreadEventQueue != 0)
  {
    m_pThreadEventQueue->removeTimer(id);
    //and remove it from DataPacket queue
    m_timerCredentials.erase(itErase);
  }

  return bFound;
}

long long StatemachineMaster::getUtcTimestamp() const
{
  //TODO :some statemachines will be try to get a UTC-time for their works.
  return 0;
}

bool StatemachineMaster::isStatemachineStillExisting(IStatemachine *pStatemachine)
{
  bool bRet = false;

  if (pStatemachine != 0)
  {
    list< IStatemachine* >::iterator it;

    for (it = m_listStrategies.begin(); it != m_listStrategies.end() && bRet == false; ++it)
    {
      bRet = (*it) == pStatemachine;
    }
  }

  return bRet;
}

bool StatemachineMaster::isStatemachineStillExisting(StatemachineType type, IStatemachine *&pStatemachine)
{
  bool bRet = false;

  list< IStatemachine* >::iterator it;

  for (it = m_listStrategies.begin(); it != m_listStrategies.end() && bRet == false; ++it)
  {
    bRet = (*it)->getStatemachineType() == type;
    if (bRet == true)
    {
      pStatemachine = (*it);
    }
  }

  return bRet;
}
void StatemachineMaster::addStatemachineToList(IStatemachine * pStatemachine)
{
  if (pStatemachine != 0)
  {
    m_listStrategies.push_back(pStatemachine);
    printStatemachines();
    printParentChild();
  }
}

IStatemachine* StatemachineMaster::getStatemachineFromList(IStatemachine::StatemachineId id) const
{
  IStatemachine* pRet = 0;
  list< IStatemachine* >::const_iterator it;

  for (it = m_listStrategies.begin(); it != m_listStrategies.end() && pRet == 0; ++it)
  {
    if ((*it)->getStatemachineId() == id)
    {
      pRet = (*it);
    }
  }

  return (pRet);
}

/*
IStatemachine* StatemachineMaster::getStatemachineOfRequestId(RequestId id) const
{
  IStatemachine* pRet = 0;
  list< IStatemachine* >::const_iterator it;
  for (it = m_listStrategies.begin(); it != m_listStrategies.end() && pRet == 0; ++it)
  {
    if ((*it)->getRequestId() == id)
    {
      pRet = (*it);
    }
  }
  return (pRet);
}*/
void StatemachineMaster::printStatemachines() const
{
  COMALOG(COMALOG_DEBUG,  "--Current Statemachines--\n" );

  for (std::list< IStatemachine* >::const_iterator it = m_listStrategies.begin(); it != m_listStrategies.end(); ++it)
  {
    COMALOG(COMALOG_DEBUG, "  id[%d] %s state[%s]\n", (*it)->getStatemachineId(), (*it)->getStatemachineName(), (*it)->getStateName() );
  }
}
