#ifndef SUBJECT_H__
#define SUBJECT_H__

#include <list>
#include <iostream>
#include <vector>
#include <map>
#include "eventhandler.h"
#include "threadeventqueue.h"
#include "comalog.h"

class Lib_API Subject
{
public:
  Subject();
  virtual ~Subject();

  void notifyEvent(Event::Ptr);

  template <class EventT>
  void attach(EventT&, EventHandler*);

  template <class EventT>
  void detach(EventT&, EventHandler*);

  template <class EventT>
  void attach(EventT&, ThreadEventQueue*);

  template <class EventT>
  void detach(EventT&, ThreadEventQueue*);

protected:
  void detachAll();

private:

  //typedefs for needed type
  typedef std::multimap< TypeInfo, EventHandler* > Observer;
  typedef Observer::iterator ObserverIter;

  typedef std::multimap< TypeInfo, ThreadEventQueue* > ObserverThreadEventQueue;
  typedef ObserverThreadEventQueue::iterator ObserverThreadEventQueueIter;

  //queue for general observers and a threadobserver
  Observer observers_;
  ObserverThreadEventQueue observersThreadEventQueue_;

private:
  template <class EventT>
  ObserverIter isAttached(EventT&, EventHandler*);

  template <class EventT>
  ObserverThreadEventQueueIter isAttached(EventT&, ThreadEventQueue*);
};


template <class EventT>
Subject::ObserverIter Subject::isAttached(EventT& obj, EventHandler* pEventHandler)
{
  //find list of eventhandlers listening for EventT-Type
  std::pair < ObserverIter, ObserverIter> ret;
  ret = observers_.equal_range(TypeInfo(typeid(obj)));

  //iterate over all and check the according pointer if it is matching the queried
  ObserverIter itRet = observers_.end();
  for (ObserverIter it = ret.first; it != ret.second; ++it)
  {
    if (it->second == pEventHandler )
    {
      itRet = it;
      break;
    }
  }

  return itRet;
}


template < class EventT >
void Subject::attach(EventT& obj, EventHandler* pEventHandler)
{
  if (isAttached(obj, pEventHandler) == observers_.end())
  {
    COMALOG(COMALOG_INFO, "Subject::attach %s eventHandler[%s] %8x\n", TypeInfo(typeid(EventT)).getName(),  pEventHandler->getName().c_str(), pEventHandler);
    observers_.insert(std::pair< TypeInfo, EventHandler*>(TypeInfo(typeid(EventT)), pEventHandler));
  }
  else
  {
    COMALOG(COMALOG_INFO, "Subject::attach %s eventHandler[%s] %8x already here!! -> no attach again\n", TypeInfo(typeid(EventT)).getName(), pEventHandler->getName().c_str(), pEventHandler);
  }
}


template < class EventT >
void Subject::detach(EventT& obj, EventHandler* pEventHandler)
{
  ObserverIter it = isAttached(obj, pEventHandler);

  if (it != observers_.end())
  {
    COMALOG(COMALOG_INFO, "remove observer\n");
    observers_.erase(it);
  }
  else
  {
    COMALOG(COMALOG_INFO, "remove observer failed, because not found\n");
  }
}


template <class EventT>
Subject::ObserverThreadEventQueueIter Subject::isAttached(EventT& obj, ThreadEventQueue* pThreadEventQueue)
{
  std::pair < ObserverThreadEventQueueIter, ObserverThreadEventQueueIter> ret;
  ret = observersThreadEventQueue_.equal_range(TypeInfo(typeid(obj)));

  ObserverThreadEventQueueIter itRet = observersThreadEventQueue_.end();
  for (ObserverThreadEventQueueIter it = ret.first; it != ret.second; ++it)
  {
    if (it->second == pThreadEventQueue)
    {
      itRet = it;
      break;
    }
  }

  return itRet;
}


template < class EventT >
void Subject::attach(EventT& obj, ThreadEventQueue* pThreadEventQueue)
{
  if (isAttached(obj, pThreadEventQueue) == observersThreadEventQueue_.end())
  {
    COMALOG(COMALOG_INFO, "Subject::attach %s ThreadEventQueue[%s] %8x\n", TypeInfo(typeid(EventT)).getName(), pThreadEventQueue->getName().c_str(),  pThreadEventQueue);
    observersThreadEventQueue_.insert(std::pair< TypeInfo, ThreadEventQueue*>(TypeInfo(typeid(EventT)), pThreadEventQueue));
  }
  else
  {
    COMALOG(COMALOG_INFO, "Subject::attach %s ThreadEventQueue[%s] %8x already here!! -> no attach again\n", pThreadEventQueue->getName().c_str(), TypeInfo(typeid(EventT)).getName(), pThreadEventQueue);
  }
}

template < class EventT >
void Subject::detach(EventT& obj, ThreadEventQueue* pThreadEventQueue)
{
  ObserverThreadEventQueueIter it = isAttached(obj, pThreadEventQueue);

  if (it != observersThreadEventQueue_.end())
  {
    COMALOG(COMALOG_INFO, "remove observerThreadEventQueue\n");
    observersThreadEventQueue_.erase(it);
  }
  else
  {
    COMALOG(COMALOG_INFO, "remove observerThreadEventQueue failed, because not found\n");
  }
}
#endif // SUBJECT_H__
