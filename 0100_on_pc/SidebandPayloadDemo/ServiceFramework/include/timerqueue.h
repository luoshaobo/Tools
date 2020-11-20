#ifndef TIMER_QUEUE_H__
#define TIMER_QUEUE_H__

#include "timer.h"
#include <vector>
#include <algorithm>
#include "operatingsystem.h"

class TimerQueue;

struct SmallerTimer
{
  bool operator()(const Timer& lhs, const Timer& rhs)
  {
    return (lhs.getTimerLatestExpire() + lhs.getTimerElapse() < rhs.getTimerLatestExpire() + rhs.getTimerElapse());
  }
};

class Lib_API TimerQueue
{
public:
  TimerQueue()
  {
  };

  virtual ~TimerQueue()
  {
    m_vTimerQueue.clear();
  }

  int getSize() const
  {
    return m_vTimerQueue.size();
  }

  bool isEmpty() const
  {
    return m_vTimerQueue.empty();
  }

  void addTimer(Timer& timer)
  {
    if (timer.getId() != Timer::InvalidTimerId)
    {
      m_vTimerQueue.push_back(timer);
      std::sort(m_vTimerQueue.begin(), m_vTimerQueue.end(), SmallerTimer());
    }
  }

  Timer getNextTimer() const
  {
    Timer ret;
    if (m_vTimerQueue.empty() == false)
    {
      ret = *(m_vTimerQueue.begin());
    }

    return ret;
  }

  bool removeTimer(Timer::TimerId id)
  {
    bool bRemoved = false;
    for (TimerVectorIter it = m_vTimerQueue.begin(); it != m_vTimerQueue.end(); ++it)
    {
      if ((*it).getId() == id)
      {
        m_vTimerQueue.erase(it);
        bRemoved = true;
        break;
      }
    }

    std::sort(m_vTimerQueue.begin(), m_vTimerQueue.end(), SmallerTimer());

    return bRemoved;
  }

  bool removeTimer(Timer& timer)
  {
    return removeTimer(timer.getId());
  }

protected:
  typedef std::vector<Timer> TimerVector;
  typedef TimerVector::iterator TimerVectorIter;

  TimerVector m_vTimerQueue;
};

#endif // TIMER_QUEUE_H__
