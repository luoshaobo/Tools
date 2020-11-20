#include "scopedlock.h"
#include "threadeventqueue.h"
#include "comalog.h"


ThreadEventQueue::ThreadEventQueue(std::string name) : Runnable(),
  m_EventHandler(name),
  m_vEventQueue(),
  m_vInsertQueue(),
  m_name(name),
  m_semaphore(0, 1000000),
  m_mutexInsertQueue(),
  m_mutexTimerQueue(),
  m_thread(name),
  m_timerQueue()
{
  m_thread.start(*this);
}

ThreadEventQueue::~ThreadEventQueue()
{
}

Thread& ThreadEventQueue::getThread()
{
  return m_thread;
}

void ThreadEventQueue::addTimer(Timer& timer)
{
  {
    ScopedLock lock(m_mutexTimerQueue);
    m_timerQueue.addTimer(timer);
  }
  m_semaphore.set();
}

bool ThreadEventQueue::removeTimer(Timer::TimerId id)
{
  return m_timerQueue.removeTimer(id);
}

void ThreadEventQueue::run()
{
  COMALOG(COMALOG_DEBUG, "%s for name %s threadName %s threadId %u\n", __FUNCTION__, getName().c_str(), getThread().getName().c_str(), getThread().getCurrentTid());
  bool active = m_thread.isRunning();

  unsigned int wait = OS_INFINITE;

  while (active == true)
  {
    //find out semaphore wait time
    if (!m_timerQueue.isEmpty())
    {
      //handle timers
      Timestamp timestamp;
      Timer t = m_timerQueue.getNextTimer();
      TimeElapse elapse = t.getTimerLatestExpire() + t.getTimerElapse() - timestamp;
      //COMALOG(COMALOG_DEBUG, "%s TimeElapse:%u\n", m_thread.getName().c_str(), elapse.getTotalMicroseconds());

      //query how long we shall sleep
      wait = static_cast<long>(elapse.getTotalMilliseconds());
    }
    else
    {
      wait = OS_INFINITE;
    }

    //in case a timer is expired we have negative waittime!
    if (wait > 0 || wait == OS_INFINITE)
    {
      m_semaphore.wait(wait);
    }

    //move appeared events from insertQueue to eventQueue
    {
      ScopedLock scopedLock(m_mutexInsertQueue);
      COMALOG(COMALOG_DEBUG, "insertqueue items:%d eventqueue items:%d\n", m_vInsertQueue.size(), m_vEventQueue.size());
      m_vEventQueue.reserve(m_vEventQueue.size() + m_vInsertQueue.size());
      m_vEventQueue.insert(m_vEventQueue.end(), m_vInsertQueue.begin(), m_vInsertQueue.end());
      m_vInsertQueue.clear();
    }

    //handle events
    COMALOG(COMALOG_DEBUG, "eventqueue items:%d\n", m_vEventQueue.size());
    if (m_vEventQueue.empty() == false)
    {
      std::vector<Event::Ptr>::iterator firstIt = m_vEventQueue.begin();
      Event::Ptr first = *firstIt;

      //erase firstIt from list because we will now handle it and therefore it is out of the queue
      m_vEventQueue.erase(firstIt);

      //handle events not from type "EventShutdown"
      if (typeid(*(first.get())) != typeid(const ThreadEventQueue::EventShutdown))
      {
        eventHandler(first);
      }
      else
      {
        COMALOG(COMALOG_DEBUG, "EventShutdown appeared -> %s stopping!\n", this->getName().c_str());
        active = false;
      }
    }

    bool bHandled = false;
    do
    {
      //COMALOG(COMALOG_DEBUG, "%s m_timerQueue nr: %u\n", m_thread.getName().c_str(), m_timerQueue.getSize());
      ScopedLock scopedLock(m_mutexTimerQueue);
      if (!m_timerQueue.isEmpty())
      {
        Timer t = m_timerQueue.getNextTimer();

        Timestamp elapseTime = t.getTimerLatestExpire();
        bool isElapsed = elapseTime.isElapsed(t.getTimerElapse());
        //COMALOG(COMALOG_DEBUG, "%s t.getTimerLatestExpire().elapsed() %lld t.getTimerElapse():%d\n", m_thread.getName().c_str(), t.getTimerLatestExpire().elapsed(), t.getTimerElapse());

        //check if timer is expired. accept 2 ms difference
        if (isElapsed==true)
        {
          //COMALOG(COMALOG_DEBUG, "%s nextTimer id:%u isElapsed:%d\n", m_thread.getName().c_str(), t.getId(), isElapsed);
          if (t.getITimerExpired() != 0)
          {
            COMALOG(COMALOG_DEBUG, "t.getITimerExpired()->handleTimeout(t.getId());\n");
            t.getITimerExpired()->handleTimeout(t.getId());

            //remove expired one
            m_timerQueue.removeTimer(t);

            if (t.isPeriodic() == true)
            {
              //add again if periodic
              Timestamp timestamp;
              t.setTimerLatestExpire(timestamp);
              m_timerQueue.addTimer(t);
            }
          }
        }
        else
        {
          //no further timers available which are expired
          bHandled = true;
        }
      }
      else
      {
        //no further timers available which are expired
        bHandled = true;
      }
    }
    while (bHandled == false);
  }
  COMALOG(COMALOG_DEBUG, "%s stopped!\n", this->getName().c_str());
}

void ThreadEventQueue::insertEvent(Event::Ptr event)
{
  {
    COMALOG(COMALOG_DEBUG, "%s insertEvent %s\n", m_thread.getName().c_str(), event.get()->getName());

    ScopedLock lock(m_mutexInsertQueue);
    m_vInsertQueue.push_back(event);
  }

  //wakeup thread
  m_semaphore.set();
}


void ThreadEventQueue::stopThread()
{
  insertEvent(new EventShutdown());
}