/**
 * @file
 *
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

#ifndef TIME_ELAPSE_H__
#define TIME_ELAPSE_H__

#include "timestamp.h"
#include "operatingsystem.h"

//@brief: TimeElapse represents and calculates the elapsed time in mircoseconds between 2 times in microseconds
//

/*
extern "C" int64_t MILLISECONDS; /// The number of microseconds in a millisecond.
extern "C" int64_t SECONDS;      /// The number of microseconds in a second.
extern "C" int64_t MINUTES;      /// The number of microseconds in a minute.
extern "C" int64_t HOURS;        /// The number of microseconds in a hour.
extern "C" int64_t DAYS;         /// The number of microseconds in a day.*/

class Lib_API TimeElapse

{
public:
  typedef Timestamp::Difference Difference;
  //static Timestamp::Difference milliToMircoSeconds;


  //Default constructor sets time elapse to zeo
  TimeElapse();

  //Create a time which elapses in parameter mention microseconds
  TimeElapse(Difference microseconds);

  //Create a time which elapses in parameter mention seconds and microseconds
  TimeElapse(long seconds, long microseconds);

  //Create a time which elapses in parameter mention days, hours, seconds and microseconds
  TimeElapse(int days, int hours, int minutes, int seconds, int microSeconds);

  //CopyConstructor
  TimeElapse(const TimeElapse& TimeElapse);

  ~TimeElapse();

  TimeElapse& operator = (const TimeElapse& TimeElapse);
  TimeElapse& operator = (Difference microseconds);


  //assign a timespan which elapses in parameter mention days, hours, seconds and microseconds
  TimeElapse& setElapse(int days, int hours, int minutes, int seconds, int microSeconds);

  //assign a timespan which elapses in parameter mention seconds and microseconds
  TimeElapse& setElapse(long seconds, long microseconds);

  void swap(TimeElapse& TimeElapse);

  //comparation operators for class
  bool operator == (const TimeElapse& ts) const;
  bool operator != (const TimeElapse& ts) const;
  bool operator >  (const TimeElapse& ts) const;
  bool operator >= (const TimeElapse& ts) const;
  bool operator <  (const TimeElapse& ts) const;
  bool operator <= (const TimeElapse& ts) const;

  //comparation operators for typedef Difference
  bool operator == (Difference microSeconds) const;
  bool operator != (Difference microSeconds) const;
  bool operator >  (Difference microSeconds) const;
  bool operator >= (Difference microSeconds) const;
  bool operator <  (Difference microSeconds) const;
  bool operator <= (Difference microSeconds) const;

  //mathematical operator for working with class
  TimeElapse operator + (const TimeElapse& d) const;
  TimeElapse operator - (const TimeElapse& d) const;
  TimeElapse& operator += (const TimeElapse& d);
  TimeElapse& operator -= (const TimeElapse& d);

//mathematical operator for working with the typedef Difference
  TimeElapse operator + (Difference microSeconds) const;
  TimeElapse operator - (Difference microSeconds) const;
  TimeElapse& operator += (Difference microSeconds);
  TimeElapse& operator -= (Difference microSeconds);

  int getDays() const;
  int getHours() const;
  int getTotalHours() const;

  int getMinutes() const;
  int getTotalMinutes() const;

  int getSeconds() const;
  int getTotalSeconds() const;

  int getMilliseconds() const;
  Difference getTotalMilliseconds() const;

  int getMicroseconds() const;
  Difference getTotalMicroseconds() const;

  int getFraction() const;

  static const Difference MILLISECONDS; /// The number of microseconds in a millisecond.
  static const Difference SECONDS;      /// The number of microseconds in a second.
  static const Difference MINUTES;      /// The number of microseconds in a minute.
  static const Difference HOURS;        /// The number of microseconds in a hour.
  static const Difference DAYS;         /// The number of microseconds in a day.

private:
  Difference m_timeSpan;
};


//
// inlines
//
inline int TimeElapse::getDays() const
{
  return int(m_timeSpan/DAYS);
}


inline int TimeElapse::getHours() const
{
  return int((m_timeSpan/HOURS) % 24);
}


inline int TimeElapse::getTotalHours() const
{
  return int(m_timeSpan/HOURS);
}


inline int TimeElapse::getMinutes() const
{
  return int((m_timeSpan/MINUTES) % 60);
}


inline int TimeElapse::getTotalMinutes() const
{
  return int(m_timeSpan/MINUTES);
}


inline int TimeElapse::getSeconds() const
{
  return int((m_timeSpan/SECONDS) % 60);
}


inline int TimeElapse::getTotalSeconds() const
{
  return int(m_timeSpan/SECONDS);
}


inline int TimeElapse::getMilliseconds() const
{
  return int((m_timeSpan/MILLISECONDS) % 1000);
}


inline TimeElapse::Difference TimeElapse::getTotalMilliseconds() const
{
  return m_timeSpan/MILLISECONDS;
}


inline int TimeElapse::getMicroseconds() const
{
  return int(m_timeSpan % 1000);
}

inline int TimeElapse::getFraction() const
{
  return int(m_timeSpan % 1000000);
}


inline TimeElapse::Difference TimeElapse::getTotalMicroseconds() const
{
  return m_timeSpan;
}


inline bool TimeElapse::operator == (const TimeElapse& ts) const
{
  return m_timeSpan == ts.m_timeSpan;
}


inline bool TimeElapse::operator != (const TimeElapse& ts) const
{
  return m_timeSpan != ts.m_timeSpan;
}


inline bool TimeElapse::operator >  (const TimeElapse& ts) const
{
  return m_timeSpan > ts.m_timeSpan;
}


inline bool TimeElapse::operator >= (const TimeElapse& ts) const
{
  return m_timeSpan >= ts.m_timeSpan;
}


inline bool TimeElapse::operator <  (const TimeElapse& ts) const
{
  return m_timeSpan < ts.m_timeSpan;
}


inline bool TimeElapse::operator <= (const TimeElapse& ts) const
{
  return m_timeSpan <= ts.m_timeSpan;
}


inline bool TimeElapse::operator == (Difference microSeconds) const
{
  return m_timeSpan == microSeconds;
}


inline bool TimeElapse::operator != (Difference microSeconds) const
{
  return m_timeSpan != microSeconds;
}


inline bool TimeElapse::operator >  (Difference microSeconds) const
{
  return m_timeSpan > microSeconds;
}


inline bool TimeElapse::operator >= (Difference microSeconds) const
{
  return m_timeSpan >= microSeconds;
}


inline bool TimeElapse::operator <  (Difference microSeconds) const
{
  return m_timeSpan < microSeconds;
}


inline bool TimeElapse::operator <= (Difference microSeconds) const
{
  return m_timeSpan <= microSeconds;
}


inline void swap(TimeElapse& s1, TimeElapse& s2)
{
  s1.swap(s2);
}

#endif  // TIME_ELAPSE_H__
