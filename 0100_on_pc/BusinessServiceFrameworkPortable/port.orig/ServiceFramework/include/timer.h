#ifndef TIMER_H__
#define TIMER_H__

#include "itimerexpired.h"
#include "timestamp.h"
#include "timeelapse.h"

class Lib_API Timer
{
public:
  typedef int TimerId;
  static const TimerId InvalidTimerId = -1;
  static       TimerId CurrentTimerId;

  Timer() : m_pTimerExpired(0),
    m_sTimerCreated(0),
    m_sTimerLatestExpire(0),
    m_sTimerElapse(0),
    m_bPeriodic(false),
    m_nTimerId(InvalidTimerId)
  {
  }

  Timer( ITimerExpired *pHandler, TimeElapse::Difference timeoutMicroSeconds,  bool bIsPeriodic, bool bTimeoutCorrection = false) :
    m_pTimerExpired(pHandler),
    m_sTimerCreated(),
    m_sTimerLatestExpire(),
    m_sTimerElapse(timeoutMicroSeconds),
    m_bPeriodic(bIsPeriodic),
    m_bUseTimeoutCorrection(bTimeoutCorrection)
  {
    //create new id
    ++CurrentTimerId;

    m_nTimerId = CurrentTimerId;
  }

  TimerId getId() const { return m_nTimerId;  }
  ITimerExpired * getITimerExpired() const { return m_pTimerExpired; }
  Timestamp getTimerCreated() const { return m_sTimerCreated; }
  Timestamp getTimerLatestExpire() const { return m_sTimerLatestExpire; }
  void setTimerLatestExpire(Timestamp t)  { m_sTimerLatestExpire = t; }
  TimeElapse::Difference getTimerElapse() const { return m_sTimerElapse; }
  bool isPeriodic() const { return m_bPeriodic; }
  TimerId getTimerId() const { return m_nTimerId; }

protected:
  // event handler that is notified if timer expires
  ITimerExpired *m_pTimerExpired;
  Timestamp m_sTimerCreated;
  Timestamp m_sTimerLatestExpire;
  //TimeElapse m_sTimerElapse;
  TimeElapse::Difference m_sTimerElapse;

  bool m_bPeriodic;

  TimerId m_nTimerId;

  // use correction for timeout
  bool m_bUseTimeoutCorrection;
};

#endif // TIMER_H__
