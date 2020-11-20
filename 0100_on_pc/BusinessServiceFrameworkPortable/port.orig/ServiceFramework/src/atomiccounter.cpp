#include "atomiccounter.h"
#include "scopedlock.h"
#include <stdio.h>
#include "comalog.h"

/**	@brief implementation constructor that creates a new AtomicCounter and initializes it to zero
 *
*/
AtomicCounter::AtomicCounter() :m_bVerbose(false)
{
	m_value=0;
}

/**	@brief implementation constructor that creates a new AtomicCounter and initializes it with the given value
 *
*/
AtomicCounter::AtomicCounter(AtomicCounter::ValueType initialValue) :m_bVerbose(false)
{
	m_value=initialValue;

}

/**	@brief implementation constructor that creates the counter by copying another one.
 *
*/
AtomicCounter::AtomicCounter(const AtomicCounter& counter)
{
	m_value= counter.m_value;
  m_bVerbose = counter.m_bVerbose;

}

/**	@brief implementation destructor.
 *
*/
AtomicCounter::~AtomicCounter()
{

}

/**	@brief implementation overwriting the operator = for the assignment of the value of another AtomicCounter.
 *
*/
AtomicCounter& AtomicCounter::operator = (const AtomicCounter& counter)
{
	{
		ScopedLock scopedLock( m_mutex );
		m_value =counter.m_value;

    if (m_bVerbose)
    {
      COMALOG(COMALOG_DEBUG, "---->  AtomicCounter => %d\n", m_value);
    }
	}

	return ( *this );
}

/**	@brief implementation overwriting the operator = for the assignment of a value to the counter.
 *
*/
AtomicCounter& AtomicCounter::operator = (ValueType value)
{

	{
		ScopedLock scopedLock( m_mutex );
		m_value =value;
	}

	return ( *this );
}

/**	@brief implementation of the method value() that returns the value of the counter.
 *
*/
AtomicCounter::ValueType AtomicCounter::value() const
{
	return m_value;
}

/**	@brief implementation overwriting the operator ++ that increments the counter and returns the result (prefix).
 *
*/
AtomicCounter::ValueType AtomicCounter::operator ++ ()
{
	//AtomicCounter::ValueType ret = 0;
	{
		ScopedLock scopedLock( m_mutex );
		++m_value;

    if (m_bVerbose)
    {
      COMALOG(COMALOG_DEBUG, "---->  AtomicCounter => %d\n", m_value);
    }

	}

	return m_value;
}

/**	@brief implementation overwriting the operator ++ that increments the counter and returns the previous value (postfix).
 *
*/
AtomicCounter::ValueType AtomicCounter::operator ++ (int)
{
	//AtomicCounter::ValueType ret = 0;
	{
		ScopedLock scopedLock( m_mutex );
		m_value++;

    if (m_bVerbose)
    {
      COMALOG(COMALOG_DEBUG, "---->  AtomicCounter => %d\n", m_value);
    }
	}
	return m_value;
}

/**	@brief implementation overwriting the operator -- that decrements the counter and returns the result (prefix).
 *
*/
AtomicCounter::ValueType AtomicCounter::operator -- ()
{
	//AtomicCounter::ValueType ret = 0;
	{
		ScopedLock scopedLock( m_mutex );
		 --m_value;

     if (m_bVerbose)
     {
       COMALOG(COMALOG_DEBUG, "---->  AtomicCounter => %d\n", m_value);
     }
	}
	return m_value;
}

/**	@brief implementation overwriting the operator -- that decrements the counter and returns the previous value (postfix).
 *
*/
AtomicCounter::ValueType AtomicCounter::operator -- (int /* a */)
{
		//AtomicCounter::ValueType ret = 0;
	{
		ScopedLock scopedLock( m_mutex );
		m_value--;

    if (m_bVerbose)
    {
      COMALOG(COMALOG_DEBUG, "---->  AtomicCounter => %d\n", m_value);
    }
	}
	return m_value;
}

/**	@brief implementation overwriting the operator ! that returns true if the counter is zero, false otherwise.
 *
*/
bool AtomicCounter::operator !() const
{
	bool ret = 0;
	{
		ScopedLock scopedLock( m_mutex );
		if(m_value==0)
		{
			ret=true;
		}
		else
		{
			ret=false;
		}

		//ret = m_value == 0;
	}

	return ret;
}
