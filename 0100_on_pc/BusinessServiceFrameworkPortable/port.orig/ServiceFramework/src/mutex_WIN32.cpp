#include "mutex_WIN32.h"
#include "timeelapse.h"

MutexImpl::MutexImpl()
{
  InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
}


MutexImpl::~MutexImpl()
{
  DeleteCriticalSection(&m_cs);
}


bool MutexImpl::tryLockImpl(long milliseconds)
{
	const int sleepMillis = 10;
	Timestamp now;
  TimeElapse::Difference difference(TimeElapse::Difference(milliseconds) * TimeElapse::MILLISECONDS);
	do
	{
    if (TryEnterCriticalSection(&m_cs) == TRUE)
    {
      return ( true );
    }
		Sleep(sleepMillis);
	}
  while ( !now.isElapsed(difference) );
	return ( false );
}
