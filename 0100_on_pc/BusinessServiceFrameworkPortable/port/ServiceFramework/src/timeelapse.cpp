#include "timeelapse.h"
#include <algorithm>

//predefined values of most needed Units in millicseconds
const TimeElapse::Difference TimeElapse::MILLISECONDS = 1000;
const TimeElapse::Difference TimeElapse::SECONDS = 1000 * TimeElapse::MILLISECONDS;
const TimeElapse::Difference TimeElapse::MINUTES = 60 * TimeElapse::SECONDS;
const TimeElapse::Difference TimeElapse::HOURS = 60 * TimeElapse::MINUTES;
const TimeElapse::Difference TimeElapse::DAYS = 24 * TimeElapse::HOURS;

/*int64_t MILLISECONDS = 1000;
int64_t SECONDS = 1000 * MILLISECONDS;
int64_t MINUTES = 60 * SECONDS;
int64_t HOURS = 60 * MINUTES;
int64_t DAYS = 24 * HOURS;*/

//static Timestamp::Difference milliToMircoSeconds = 1000;


TimeElapse::TimeElapse() : m_timeSpan(0)
{
}


TimeElapse::TimeElapse(Difference microSeconds) : m_timeSpan(microSeconds)
{
}


TimeElapse::TimeElapse(long seconds, long microSeconds) : m_timeSpan( Difference(seconds) * SECONDS + microSeconds )
{
}


TimeElapse::TimeElapse(int days, int hours, int minutes, int seconds, int microSeconds) :
	m_timeSpan(
              Difference( microSeconds ) +
              Difference(seconds) * SECONDS + Difference(minutes) * MINUTES + Difference(hours) * HOURS + Difference(days) * DAYS)
{
}


TimeElapse::TimeElapse(const TimeElapse& TimeElapse) : m_timeSpan(TimeElapse.m_timeSpan)
{
}


TimeElapse::~TimeElapse()
{
}


TimeElapse& TimeElapse::operator = (const TimeElapse& TimeElapse)
{
	m_timeSpan = TimeElapse.m_timeSpan;
	return *this;
}


TimeElapse& TimeElapse::operator = (Difference microSeconds)
{
	m_timeSpan = microSeconds;
	return *this;
}


TimeElapse& TimeElapse::setElapse(int days, int hours, int minutes, int seconds, int microSeconds)
{
	m_timeSpan = Difference(microSeconds) +
               Difference(seconds) * SECONDS + Difference(minutes) * MINUTES + Difference(hours) * HOURS + Difference(days) * DAYS;
	return *this;
}


TimeElapse& TimeElapse::setElapse(long seconds, long microSeconds)
{
	m_timeSpan = Difference(seconds)*SECONDS + Difference(microSeconds);
	return *this;
}


void TimeElapse::swap(TimeElapse& TimeElapse)
{
	std::swap(m_timeSpan, TimeElapse.m_timeSpan);
}


TimeElapse TimeElapse::operator + (const TimeElapse& d) const
{
	return TimeElapse(m_timeSpan + d.m_timeSpan);
}


TimeElapse TimeElapse::operator - (const TimeElapse& d) const
{
	return TimeElapse(m_timeSpan - d.m_timeSpan);
}


TimeElapse& TimeElapse::operator += (const TimeElapse& d)
{
	m_timeSpan += d.m_timeSpan;
	return *this;
}


TimeElapse& TimeElapse::operator -= (const TimeElapse& d)
{
	m_timeSpan -= d.m_timeSpan;
	return *this;
}


TimeElapse TimeElapse::operator + (Difference microSeconds) const
{
	return TimeElapse(m_timeSpan + microSeconds);
}


TimeElapse TimeElapse::operator - (Difference microSeconds) const
{
	return TimeElapse(m_timeSpan - microSeconds);
}


TimeElapse& TimeElapse::operator += (Difference microSeconds)
{
	m_timeSpan += microSeconds;
	return *this;
}


TimeElapse& TimeElapse::operator -= (Difference microSeconds)
{
	m_timeSpan -= microSeconds;
	return *this;
}
