#ifndef MUTEX_POSIX_H__
#define MUTEX_POSIX_H__

#include <pthread.h>
#include <errno.h>
#include "operatingsystem.h"

class Lib_API MutexImpl
{
protected:
	MutexImpl();
	MutexImpl( bool fast );
	~MutexImpl();

	void lockImpl();
	bool tryLockImpl();
	bool tryLockImpl( long milliseconds );
	void unlockImpl();

private:
	pthread_mutex_t m_mutex;
};

#endif // MUTEX_POSIX_H__
