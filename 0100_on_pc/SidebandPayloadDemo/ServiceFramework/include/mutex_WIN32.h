#ifndef MUTEX_WIN32_H__
#define MUTEX_WIN32_H__

#include "operatingsystem.h"
#include <Windows.h>
#include <process.h>

class Lib_API MutexImpl
{
protected:
	MutexImpl();
	~MutexImpl();
	void lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();

private:
	CRITICAL_SECTION m_cs;
};

inline void MutexImpl::lockImpl()
{
  EnterCriticalSection(&m_cs);
}

inline bool MutexImpl::tryLockImpl()
{
  return ( TryEnterCriticalSection( &m_cs ) != 0 );
}

inline void MutexImpl::unlockImpl()
{
  LeaveCriticalSection( &m_cs );
}

#endif // MUTEX_WIN32_H__
