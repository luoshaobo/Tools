#ifndef SEMAPHORE_POSIX_H__
#define SEMAPHORE_POSIX_H__

#include <pthread.h>
#include <errno.h>
#include "operatingsystem.h"

class Lib_API SemaphoreImpl
{
protected:
  SemaphoreImpl(int n, int max);
  ~SemaphoreImpl();

  void setImpl();
  void waitImpl();
  bool waitImpl(long milliseconds);

private:
  volatile int    m_n;
  int             m_max;
  pthread_mutex_t m_mutex;
  pthread_cond_t  m_cond;
};


//
// inlines
//
inline void SemaphoreImpl::setImpl()
{
  if (pthread_mutex_lock(&m_mutex))
    // throw SystemException("cannot signal semaphore (lock)");
    return;
  if (m_n < m_max)
  {
    ++m_n;
  }
  else
  {
    pthread_mutex_unlock(&m_mutex);
    //throw SystemException("cannot signal semaphore: count would exceed maximum");
    return;
  }
  if (pthread_cond_signal(&m_cond))
  {
    pthread_mutex_unlock(&m_mutex);
    //throw SystemException("cannot signal semaphore");
    return;
  }
  pthread_mutex_unlock(&m_mutex);
}

#endif // SEMAPHORE_POSIX_H__