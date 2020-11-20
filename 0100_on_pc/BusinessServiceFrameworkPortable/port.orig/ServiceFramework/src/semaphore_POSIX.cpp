
#include "semaphore_POSIX.h"
#if defined(POCO_VXWORKS)
#include <timers.h>
#include <cstring>
#else
#include <sys/time.h>
#endif


SemaphoreImpl::SemaphoreImpl(int n, int max) : m_n(n), m_max(max)
{
  //poco_assert (n >= 0 && max > 0 && n <= max);
  if (!(n >= 0 && max > 0 && n <= max))
    return;

#if defined(POCO_VXWORKS)
  // This workaround is for VxWorks 5.x where
  // pthread_mutex_init() won't properly initialize the mutex
  // resulting in a subsequent freeze in pthread_mutex_destroy()
  // if the mutex has never been used.
  std::memset(&m_mutex, 0, sizeof(m_mutex));
#endif
  if (pthread_mutex_init(&m_mutex, NULL))
    //throw SystemException("cannot create semaphore (mutex)");
    return;
  if (pthread_cond_init(&m_cond, NULL))
    //throw SystemException("cannot create semaphore (condition)");
    return;
}


SemaphoreImpl::~SemaphoreImpl()
{
  pthread_cond_destroy(&m_cond);
  pthread_mutex_destroy(&m_mutex);
}


void SemaphoreImpl::waitImpl()
{
  if (pthread_mutex_lock(&m_mutex))
    //throw SystemException("wait for semaphore failed (lock)");
    return;
  while (m_n < 1)
  {
    if (pthread_cond_wait(&m_cond, &m_mutex))
    {
      pthread_mutex_unlock(&m_mutex);
      //throw SystemException("wait for semaphore failed");
      return;
    }
  }
  --m_n;
  pthread_mutex_unlock(&m_mutex);
}


bool SemaphoreImpl::waitImpl(long milliseconds)
{
  int rc = 0;
  struct timespec abstime;

#if defined(__VMS)
  struct timespec delta;
  delta.tv_sec = milliseconds / 1000;
  delta.tv_nsec = (milliseconds % 1000) * 1000000;
  pthread_get_expiration_np(&delta, &abstime);
#elif defined(POCO_VXWORKS)
  clock_gettime(CLOCK_REALTIME, &abstime);
  abstime.tv_sec += milliseconds / 1000;
  abstime.tv_nsec += (milliseconds % 1000) * 1000000;
  if (abstime.tv_nsec >= 1000000000)
  {
    abstime.tv_nsec -= 1000000000;
    abstime.tv_sec++;
  }
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  abstime.tv_sec = tv.tv_sec + milliseconds / 1000;
  abstime.tv_nsec = tv.tv_usec * 1000 + (milliseconds % 1000) * 1000000;
  if (abstime.tv_nsec >= 1000000000)
  {
    abstime.tv_nsec -= 1000000000;
    abstime.tv_sec++;
  }
#endif

  if (pthread_mutex_lock(&m_mutex) != 0)
    //throw SystemException("wait for semaphore failed (lock)");
    return false;
  while (m_n < 1)
  {
    if ((rc = pthread_cond_timedwait(&m_cond, &m_mutex, &abstime)))
    {
      if (rc == ETIMEDOUT) break;
      pthread_mutex_unlock(&m_mutex);
      //throw SystemException("cannot wait for semaphore");
      return false;
    }
  }
  if (rc == 0) --m_n;
  pthread_mutex_unlock(&m_mutex);
  return rc == 0;
}
