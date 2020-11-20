#include "mutex_POSIX.h"
#include "timeelapse.h"
#include <unistd.h>
#include <sys/time.h>



//visit
//http://pronix.linuxdelta.de/C/Linuxprogrammierung/Linuxsystemprogrammieren_C_Kurs_Kapitel8a.shtml
//http://www.boost.org/doc/libs/1_57_0/boost/thread/pthread/mutex.hpp

#if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS - 200112L) >= 0L
#if defined(_POSIX_THREADS) && (_POSIX_THREADS - 200112L) >= 0L
#define HAVE_MUTEX_TIMEOUT
#endif
#endif



MutexImpl::MutexImpl()
{
  //initialize the mutex attributes object
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init( &attributes );

  //set the mutex kind attributesibute in attributes to the value specified
#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
	pthread_mutexattr_settype_np( &attributes, PTHREAD_MUTEX_RECURSIVE_NP );
#else
	pthread_mutexattr_settype( &attributes, PTHREAD_MUTEX_RECURSIVE );
#endif

  //initialises the mutex referenced by mutex with attributes specified by attributes
  pthread_mutex_init( &m_mutex, &attributes );

	pthread_mutexattr_destroy( &attributes );
}


MutexImpl::MutexImpl(bool fast)
{
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);

  //set the mutex kind attributes in attributes to the value specified
#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
	pthread_mutexattr_settype_np( &attributes, fast ? PTHREAD_MUTEX_NORMAL_NP : PTHREAD_MUTEX_RECURSIVE_NP );
#else
	pthread_mutexattr_settype( &attributes, fast ? PTHREAD_MUTEX_NORMAL : PTHREAD_MUTEX_RECURSIVE );
#endif

  //initialises the mutex referenced by mutex with attributes specified by attributes
  pthread_mutex_init(&m_mutex, &attributes);

  pthread_mutexattr_destroy(&attributes);
}


MutexImpl::~MutexImpl()
{
	pthread_mutex_destroy(&m_mutex);
}


bool MutexImpl::tryLockImpl(long milliseconds)
{
#if defined(HAVE_MUTEX_TIMEOUT)
	struct timespec abstime;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
	abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;

	if (abstime.tv_nsec >= 1000000000)
	{
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}

	int rc = pthread_mutex_timedlock(&m_mutex, &abstime);
  if (rc == 0)
  {
    return true;
  }
  else //
  {
    //ETIMEDOUT
    return false;
  }
#else
	const int sleepMillis = 10;
  SteadyTimestamp now;

  TimeElapse::Difference difference(TimeElapse::Difference(milliseconds) * TimeElapse::MILLISECONDS);
	do
	{
		int rc = pthread_mutex_trylock(&m_mutex);

    if (rc == 0)
    {
      return true;
    }
    else if (rc != EBUSY)
    {
      break;
    }


		struct timeval tv;
		tv.tv_sec  = 0;
		tv.tv_usec = sleepMillis * 1000;
		select(0, NULL, NULL, NULL, &tv);

	}
  while ( !now.isElapsed(difference) );

	return false;
#endif
}

bool MutexImpl::tryLockImpl()
{
	bool bRet = false;

  int rc = pthread_mutex_trylock(&m_mutex);
  if (rc == 0)
  {
    bRet = true;
  }
  else /* if (rc == EBUSY)*/
  {
     bRet = false;
  }

  return ( bRet );
}

void MutexImpl::lockImpl()
{
	pthread_mutex_lock(&m_mutex);
}


void MutexImpl::unlockImpl()
{
	pthread_mutex_unlock(&m_mutex);
}
