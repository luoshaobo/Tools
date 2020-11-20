
#include "subject.h"
#include "event.h"

Subject::Subject() : observers_(), observersThreadEventQueue_()
{
}

Subject::~Subject()
{
  detachAll();
}

void Subject::notifyEvent(Event::Ptr event)
{
  //find observers for that EventType
  std::pair <ObserverIter, ObserverIter> ret;
  ret = observers_.equal_range(TypeInfo(typeid(*event.get())));

  //inform all observers of that appeared event
  for (ObserverIter it = ret.first; it != ret.second; ++it)
  {
    it->second->handleEvent(event);
  }

  //find threadEventQueues which observing that EventType
  std::pair <ObserverThreadEventQueueIter, ObserverThreadEventQueueIter> ret2;
  ret2 = observersThreadEventQueue_.equal_range(TypeInfo(typeid(*event)));

  //inform all of the threadEventQueues about that appeared event
  for (ObserverThreadEventQueueIter it = ret2.first; it != ret2.second; ++it)
  {
    it->second->insertEvent(event);
  }
}

void Subject::detachAll()
{
  //clear observers
  observers_.clear();

  //clear threadEventQueues
  observersThreadEventQueue_.clear();
}
