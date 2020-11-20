
#ifndef TIMER_STAMP_H__
#define TIMER_STAMP_H__

#include <ctime>
#include "operatingsystem.h"

class TimeElapse;

class Lib_API Timestamp
	/// A Timestamp stores a monotonic* time value
	/// with (theoretical) microseconds resolution.
	/// Timestamps can be compared with each other
	/// and simple arithmetics are supported.
	///
	/// [*] Note that Timestamp values are only monotonic as
	/// long as the systems's clock is monotonic as well
	/// (and not, e.g. set back).
	///
	/// Timestamps are UTC (Coordinated Universal Time)
	/// based and thus independent of the timezone
	/// in effect on the system.
{
public:
  // monotonic UTC time value in microsecond resolution
  typedef int64_t TimeVal;

  // monotonic UTC time value in 100 nanosecond resolution
  typedef int64_t UtcTimeVal;

  // difference between two timestamps in microseconds
  typedef int64_t Difference;

  // Creates a timestamp with the current time.
  Timestamp();

	Timestamp(TimeVal tv);
		/// Creates a timestamp from the given time value.

	Timestamp(const Timestamp& other);
		/// Copy constructor.

	~Timestamp();
		/// Destroys the timestamp

	Timestamp& operator = (const Timestamp& other);
	Timestamp& operator = (TimeVal tv);

	void swap(Timestamp& timestamp);
		/// Swaps the Timestamp with another one.

	void update();
		/// Updates the Timestamp with the current time.

	bool operator == (const Timestamp& ts) const;
	bool operator != (const Timestamp& ts) const;
	bool operator >  (const Timestamp& ts) const;
	bool operator >= (const Timestamp& ts) const;
	bool operator <  (const Timestamp& ts) const;
	bool operator <= (const Timestamp& ts) const;

	Timestamp  operator +  (Difference d) const;
  Timestamp  operator +  (const TimeElapse& span) const;
	Timestamp  operator -  (Difference d) const;
  Timestamp  operator -  (const TimeElapse& span) const;
	Difference   operator -  (const Timestamp& ts) const;
	Timestamp& operator += (Difference d);
  Timestamp& operator += (const TimeElapse& span);
	Timestamp& operator -= (Difference d);
  Timestamp& operator -= (const TimeElapse& span);

	std::time_t epochTime() const;
		/// Returns the timestamp expressed in time_t.
		/// time_t base time is midnight, January 1, 1970.
		/// Resolution is one second.

	UtcTimeVal utcTime() const;
		/// Returns the timestamp expressed in UTC-based
		/// time. UTC base time is midnight, October 15, 1582.
		/// Resolution is 100 nanoseconds.

	TimeVal epochMicroseconds() const;
		/// Returns the timestamp expressed in microseconds
		/// since the Unix epoch, midnight, January 1, 1970.

	Difference elapsed() const;
		/// Returns the time elapsed since the time denoted by
		/// the timestamp. Equivalent to Timestamp() - *this.

	bool isElapsed(Difference interval) const;
		/// Returns true iff the given interval has passed
		/// since the time denoted by the timestamp.

	static Timestamp fromEpochTime(std::time_t t);
		/// Creates a timestamp from a std::time_t.

	static Timestamp fromUtcTime(UtcTimeVal val);
		/// Creates a timestamp from a UTC time value.

	static TimeVal resolution();
		/// Returns the resolution in units per second.
		/// Since the timestamp has microsecond resolution,
		/// the returned value is always 1000000.

#if defined(_WIN32)
  static Timestamp fromFileTimeNP(unsigned int fileTimeLow, unsigned int fileTimeHigh);
  void toFileTimeNP(unsigned int& fileTimeLow, unsigned int& fileTimeHigh) const;
#endif

private:
	TimeVal m_ts;
};


//
// inlines
//
inline bool Timestamp::operator == (const Timestamp& ts) const
{
	return m_ts == ts.m_ts;
}


inline bool Timestamp::operator != (const Timestamp& ts) const
{
	return m_ts != ts.m_ts;
}


inline bool Timestamp::operator >  (const Timestamp& ts) const
{
	return m_ts > ts.m_ts;
}


inline bool Timestamp::operator >= (const Timestamp& ts) const
{
	return m_ts >= ts.m_ts;
}


inline bool Timestamp::operator <  (const Timestamp& ts) const
{
	return m_ts < ts.m_ts;
}


inline bool Timestamp::operator <= (const Timestamp& ts) const
{
	return m_ts <= ts.m_ts;
}


inline Timestamp Timestamp::operator + (Timestamp::Difference d) const
{
	return Timestamp(m_ts + d);
}


inline Timestamp Timestamp::operator - (Timestamp::Difference d) const
{
	return Timestamp(m_ts - d);
}


inline Timestamp::Difference Timestamp::operator - (const Timestamp& ts) const
{
	return m_ts - ts.m_ts;
}


inline Timestamp& Timestamp::operator += (Timestamp::Difference d)
{
	m_ts += d;
	return *this;
}


inline Timestamp& Timestamp::operator -= (Timestamp::Difference d)
{
	m_ts -= d;
	return *this;
}


inline std::time_t Timestamp::epochTime() const
{
	return std::time_t(m_ts/resolution());
}


inline Timestamp::UtcTimeVal Timestamp::utcTime() const
{
	return m_ts*10 + (Difference(0x01b21dd2) << 32) + 0x13814000;
}


inline Timestamp::TimeVal Timestamp::epochMicroseconds() const
{
	return m_ts;
}


inline Timestamp::Difference Timestamp::elapsed() const
{
	Timestamp now;
	return now - *this;
}


inline bool Timestamp::isElapsed(Timestamp::Difference interval) const
{
	Timestamp now;
	Timestamp::Difference diff = now - *this;
	return diff >= interval;
}


inline Timestamp::TimeVal Timestamp::resolution()
{
	return 1000000;
}


inline void swap(Timestamp& s1, Timestamp& s2)
{
	s1.swap(s2);
}



#endif // TIMER_STAMP_H__
