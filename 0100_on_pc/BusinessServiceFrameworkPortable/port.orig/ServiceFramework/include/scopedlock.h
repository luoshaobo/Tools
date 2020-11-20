#ifndef SCOPEDLOCK_H__
#define SCOPEDLOCK_H__

/**	@brief including all necessary headers
 *
*/
#include "mutex.h"

/**
 * @class ScopedLock
 *
 * @brief Integration of mutex.lock in constructor and mutex.unlock in destructor
 *
 *
 */
class Lib_API ScopedLock
{
public:
	ScopedLock( Mutex &mutex ) : m_mutex( mutex )
	{
		m_mutex.lock();
	}

	ScopedLock( Mutex &mutex, long milliseconds ) : m_mutex( mutex )
	{
		m_mutex.lock( milliseconds );
	}

	~ScopedLock()
	{
		m_mutex.unlock();
	}

private:
	ScopedLock( ScopedLock& rhs );
	Mutex& m_mutex;
};


#endif //SCOPEDLOCK_H__