#ifndef TIMEVALUE_H__
#define TIMERVALUE_H__


#include <time.h>
#include "Winsock2.h"
#include "operatingsystem.h"

class Lib_API TimeValue
{
public:

  // constant for conversion between nanoseconds and seconds.
  static const long long NANOSECONDS_PER_SECOND = 1000000000LL;

  // constant for conversion between microseconds and seconds.
  static const long MICROSECONDS_PER_SECOND = 1000000L;

  // constant for conversion between miliseconds and seconds.
  static const long MILISECONDS_PER_SECOND = 1000L;

  TimeValue()
  {
    m_Time.tv_sec = 0;
    m_Time.tv_usec = 0;
  }

  TimeValue(const timeval& tv)
  {
    m_Time.tv_sec = tv.tv_sec;
    m_Time.tv_usec = tv.tv_usec;
  }

  TimeValue(time_t tv_sec, long tv_usec)
  {
    m_Time.tv_sec = tv_sec;
    m_Time.tv_usec = tv_usec;
  }

  TimeValue(long msec)
  {
    m_Time.tv_sec = msec / 1000;
    m_Time.tv_usec = (msec % 1000) * 1000;
  }

  void set(long msec)
  {
    m_Time.tv_sec = msec / 1000;
    m_Time.tv_usec = (msec % 1000) * 1000;
  }

  void getTimeval(timeval& tv)
  {
    tv.tv_sec = m_Time.tv_sec;
    tv.tv_usec = m_Time.tv_usec;
  }

  void getTimespec(timespec& ts)
  {
    ts.tv_sec = m_Time.tv_sec;
    ts.tv_nsec = m_Time.tv_usec * 1000;
  }

  int setCurrentTime()
  {
    return ::gettimeofday(&m_Time, NULL);
  }

  int addCurrentTime() // converts relative to absolute time
  {
    TimeValue currentTime;
    int retVal = currentTime.setCurrentTime();
    *this += currentTime;
    return retVal;
  }

  static TimeValue getCurrentTime()
  {
    timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv;
  }

  long getMiliSec() const
  {
    return (m_Time.tv_sec * 1000) + (m_Time.tv_usec / 1000);
  }

  void normalize(timespec& sp)
  {
    while (sp.tv_nsec >= NANOSECONDS_PER_SECOND)
    {
      sp.tv_nsec -= NANOSECONDS_PER_SECOND;
      ++sp.tv_sec;
    }

    while (sp.tv_nsec < 0)
    {
      sp.tv_nsec -= NANOSECONDS_PER_SECOND;
      --sp.tv_sec;
    }
  }

  static int miliSleep(long delay)
  {
    timespec timeToDelay;
    timeToDelay.tv_sec = delay / 1000;
    timeToDelay.tv_nsec = (delay % 1000) * 1000000;
    return ::nanosleep(&timeToDelay, NULL);
  }

  TimeValue& operator+= (const TimeValue& tval)
  {
    m_Time.tv_sec += tval.m_Time.tv_sec;
    m_Time.tv_usec += tval.m_Time.tv_usec;

    if (m_Time.tv_usec >= 1000000)
    {
      m_Time.tv_usec -= 1000000;
      m_Time.tv_sec++;
    }
    return *this;
  }

  TimeValue operator+ (const TimeValue& tval) const
  {
    TimeValue ret = *this;
    ret += tval;
    return ret;
  }

  TimeValue& operator-= (const TimeValue& tval)
  {
    if (tval.m_Time.tv_usec > m_Time.tv_usec)
    {
      m_Time.tv_sec = m_Time.tv_sec - tval.m_Time.tv_sec - 1;
      m_Time.tv_usec = m_Time.tv_usec + 1000000 - tval.m_Time.tv_usec;
    }
    else
    {
      m_Time.tv_sec = m_Time.tv_sec - tval.m_Time.tv_sec;
      m_Time.tv_usec = m_Time.tv_usec - tval.m_Time.tv_usec;
    }

    return *this;
  }

  TimeValue operator- (const TimeValue& tval) const
  {
    TimeValue ret = *this;
    ret -= tval;
    return ret;
  }

  bool operator== (const TimeValue& tval) const
  {
    return ((m_Time.tv_sec == tval.m_Time.tv_sec) &&
      (m_Time.tv_usec == tval.m_Time.tv_usec));
  }

  bool operator!= (const TimeValue& tval) const
  {
    return ((m_Time.tv_sec != tval.m_Time.tv_sec) ||
      (m_Time.tv_usec != tval.m_Time.tv_usec));
  }

  bool operator< (const TimeValue& tval) const
  {
    return ((m_Time.tv_sec < tval.m_Time.tv_sec) ||
      ((m_Time.tv_sec == tval.m_Time.tv_sec) &&
      (m_Time.tv_usec < tval.m_Time.tv_usec)));
  }

  bool operator>(const TimeValue& tval) const
  {
    return (tval < *this);
  }

  bool operator<= (const TimeValue& tval) const
  {
    return !(*this > tval);
  }

  bool operator>= (const TimeValue& tval) const
  {
    return !(*this < tval);
  }

private:

  timeval m_Time;
};


#endif
