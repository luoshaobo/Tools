
#include "thread_POSIX.h"
#include "timeelapse.h"
#include "timestamp.h"
#include <signal.h>
#if defined(OS_LINUX)
#	include <time.h>
#endif
#include "comalog.h"

//
// Block SIGPIPE in main thread.
//
#if defined(OS_LINUX)
namespace
{
  class SignalBlocker
  {
  public:
    SignalBlocker()
    {
      sigset_t sset;
      sigemptyset(&sset);
      sigaddset(&sset, SIGPIPE);
      pthread_sigmask(SIG_BLOCK, &sset, 0);
    }
    ~SignalBlocker()
    {
    }
  };

  static SignalBlocker signalBlocker;
}
#endif

ThreadImpl::CurrentThreadHolder ThreadImpl::_currentThreadHolder;

ThreadImpl::ThreadImpl() :
  m_pData(new ThreadData)
{
}

ThreadImpl::~ThreadImpl()
{
  if (m_pData != 0)
  {
    if (m_pData->started && !m_pData->joined)
    {
      pthread_detach(m_pData->thread);
    }

    delete m_pData;
    m_pData = 0;
  }

  delete m_pData;
}


void ThreadImpl::setPriorityImpl(int prio)
{
  if (prio != m_pData->prio)
  {
    m_pData->prio = prio;
    if (isRunningImpl())
    {
      struct sched_param par;
      par.sched_priority = mapPrio(m_pData->prio, SCHED_OTHER);
      if (pthread_setschedparam(m_pData->thread, SCHED_OTHER, &par))
        COMALOG(COMALOG_WARN, "cannot set thread priority\n");
    }
  }
}


void ThreadImpl::setOSPriorityImpl(int prio, int policy)
{
  if (prio != m_pData->osPrio || policy != m_pData->policy)
  {
    if (m_pData->pRunnableTarget || m_pData->pCallbackTarget)
    {
      struct sched_param par;
      par.sched_priority = prio;
      if (pthread_setschedparam(m_pData->thread, policy, &par))
        COMALOG(COMALOG_WARN, "cannot set thread priority\n");
    }
    m_pData->prio = reverseMapPrio(prio, policy);
    m_pData->osPrio = prio;
    m_pData->policy = policy;
  }
}


int ThreadImpl::getMinOSPriorityImpl(int policy)
{
  return sched_get_priority_min(policy);
}


int ThreadImpl::getMaxOSPriorityImpl(int policy)
{
  return sched_get_priority_max(policy);
}


void ThreadImpl::setStackSizeImpl(int size)
{
  m_pData->stackSize = size;
}


void ThreadImpl::startImpl(Runnable& target)
{
  if (m_pData->pRunnableTarget)
    COMALOG(COMALOG_WARN, "thread already running\n");

  pthread_attr_t attributes;
  pthread_attr_init(&attributes);

  if (m_pData->stackSize != 0)
  {
    if (0 != pthread_attr_setstacksize(&attributes, m_pData->stackSize))
    {
      pthread_attr_destroy(&attributes);
      COMALOG(COMALOG_WARN, "cannot set thread stack size\n");
    }
  }

  m_pData->pRunnableTarget = &target;
  if (pthread_create(&m_pData->thread, &attributes, runnableEntry, this))
  {
    m_pData->pRunnableTarget = 0;
    pthread_attr_destroy(&attributes);
    COMALOG(COMALOG_WARN, "cannot start thread\n");
  }
  m_pData->started = true;
  pthread_attr_destroy(&attributes);

  if (m_pData->policy == SCHED_OTHER)
  {
    if (m_pData->prio != PRIO_NORMAL_IMPL)
    {
      struct sched_param par;
      par.sched_priority = mapPrio(m_pData->prio, SCHED_OTHER);
      if (pthread_setschedparam(m_pData->thread, SCHED_OTHER, &par))
        COMALOG(COMALOG_WARN, "cannot set thread priority\n");
    }
  }
  else
  {
    struct sched_param par;
    par.sched_priority = mapPrio(m_pData->prio, m_pData->policy);
    if (pthread_setschedparam(m_pData->thread, m_pData->policy, &par))
      COMALOG(COMALOG_WARN, "cannot set thread priority\n");
  }
}


void ThreadImpl::startImpl(Callable target, void* pData)
{
  if (m_pData->pCallbackTarget && m_pData->pCallbackTarget->callback)
    COMALOG(COMALOG_WARN, "thread already running\n");

  pthread_attr_t attributes;
  pthread_attr_init(&attributes);

  if (m_pData->stackSize != 0)
  {
    if (0 != pthread_attr_setstacksize(&attributes, m_pData->stackSize))
      COMALOG(COMALOG_WARN, "can not set thread stack size\n");
  }


  if (0 == m_pData->pCallbackTarget)
    m_pData->pCallbackTarget = new CallbackData;

  m_pData->pCallbackTarget->callback = target;
  m_pData->pCallbackTarget->pData = pData;

  if (pthread_create(&m_pData->thread, &attributes, callableEntry, this))
  {
    m_pData->pCallbackTarget->callback = 0;
    m_pData->pCallbackTarget->pData = 0;
    pthread_attr_destroy(&attributes);
    COMALOG(COMALOG_WARN, "cannot start thread\n");
  }
  m_pData->started = true;
  pthread_attr_destroy(&attributes);

  if (m_pData->policy == SCHED_OTHER)
  {
    if (m_pData->prio != PRIO_NORMAL_IMPL)
    {
      struct sched_param par;
      par.sched_priority = mapPrio(m_pData->prio, SCHED_OTHER);
      if (pthread_setschedparam(m_pData->thread, SCHED_OTHER, &par))
        COMALOG(COMALOG_WARN, "cannot set thread priority\n");
    }
  }
  else
  {
    struct sched_param par;
    par.sched_priority = m_pData->osPrio;
    if (pthread_setschedparam(m_pData->thread, m_pData->policy, &par))
      COMALOG(COMALOG_WARN, "cannot set thread priority\n");
  }
}


void ThreadImpl::joinImpl()
{
  void* result;
  if (pthread_join(m_pData->thread, &result))
    COMALOG(COMALOG_WARN, "cannot join thread\n");
  m_pData->joined = true;
}


bool ThreadImpl::joinImpl(long /*milliseconds*/)
{
  //TODO implement milliseconds somewhen
  void* result;
  if (pthread_join(m_pData->thread, &result))
    COMALOG(COMALOG_WARN, "cannot join thread\n");
  m_pData->joined = true;
  return true;
}


ThreadImpl* ThreadImpl::currentImpl()
{
  return _currentThreadHolder.get();
}


ThreadImpl::TIDImpl ThreadImpl::currentTidImpl()
{
  return pthread_self();
}


void ThreadImpl::sleepImpl(long milliseconds)
{
#if defined(OS_LINUX)

  TimeElapse remainingTime(TimeElapse::MILLISECONDS * TimeElapse::Difference(milliseconds));
  int rc;
  do
  {
    struct timespec ts;
    ts.tv_sec = (long)remainingTime.getTotalSeconds();
    ts.tv_nsec = (long)remainingTime.getFraction() * 1000;
    Timestamp start;
    rc = ::nanosleep(&ts, 0);
    if (rc < 0 && errno == EINTR)
    {
      Timestamp end;
      TimeElapse waited = start.elapsed();
      if (waited < remainingTime)
        remainingTime -= waited;
      else
        remainingTime = 0;
    }
  } while (remainingTime > 0 && rc < 0 && errno == EINTR);
  if (rc < 0 && remainingTime > 0)
    COMALOG(COMALOG_WARN, "Thread::sleep(): nanosleep() failed\n");
#else
  TimeElapse remainingTime(TimeElapse::MILLISECONDS * TimeElapse::Difference(milliseconds));
  int rc;
  do
  {
    struct timeval tv;
    tv.tv_sec = (long)remainingTime.getTotalSeconds();
    tv.tv_usec = (long)remainingTime.getFraction();
    Timestamp start;
    rc = ::select(0, NULL, NULL, NULL, &tv);
    if (rc < 0 && errno == EINTR)
    {
      Timestamp end;
      TimeElapse waited = start.elapsed();
      if (waited < remainingTime)
        remainingTime -= waited;
      else
        remainingTime = 0;
    }
  } while (remainingTime > 0 && rc < 0 && errno == EINTR);
  if (rc < 0 && remainingTime > 0) COMALOG(COMALOG_WARN, "Thread::sleep(): select() failed\n");
#endif
}


void* ThreadImpl::runnableEntry(void* pThread)
{
  _currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));

  /*#if
    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGQUIT);
    sigaddset(&sset, SIGTERM);
    sigaddset(&sset, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &sset, 0);
  #endif*/

  ThreadImpl* pThreadImpl = reinterpret_cast<ThreadImpl*>(pThread);

  ThreadData *pData = pThreadImpl->m_pData;

  if (pData)
  {
    pData->pRunnableTarget->run();
    pData->pRunnableTarget = 0;
    //pData->done.set();
  }

  return 0;
}


void* ThreadImpl::callableEntry(void* pThread)
{
  _currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));


  ThreadImpl* pThreadImpl = reinterpret_cast<ThreadImpl*>(pThread);
#if defined(POCO_POSIX_DEBUGGER_THREAD_NAMES)
  setThreadName(pThreadImpl->m_pData->thread, reinterpret_cast<Thread*>(pThread)->getName().c_str());
#endif
  ThreadData *pData = pThreadImpl->m_pData;

  if (pData)
  {
    pData->pCallbackTarget->callback(pData->pCallbackTarget->pData);
    pData->pCallbackTarget->callback = 0;
    pData->pCallbackTarget->pData = 0;
    //pData->done.set();
  }

  return 0;
}


int ThreadImpl::mapPrio(int prio, int policy)
{
  int pmin = getMinOSPriorityImpl(policy);
  int pmax = getMaxOSPriorityImpl(policy);

  switch (prio)
  {
  case PRIO_LOWEST_IMPL:
    return pmin;
  case PRIO_LOW_IMPL:
    return pmin + (pmax - pmin) / 4;
  case PRIO_NORMAL_IMPL:
    return pmin + (pmax - pmin) / 2;
  case PRIO_HIGH_IMPL:
    return pmin + 3 * (pmax - pmin) / 4;
  case PRIO_HIGHEST_IMPL:
    return pmax;
  default:
    COMALOG(COMALOG_WARN, "invalid thread priority\n");
  }
  return -1;
}


int ThreadImpl::reverseMapPrio(int prio, int policy)
{
  if (policy == SCHED_OTHER)
  {
    int pmin = getMinOSPriorityImpl(policy);
    int pmax = getMaxOSPriorityImpl(policy);
    int normal = pmin + (pmax - pmin) / 2;
    if (prio == pmax)
      return PRIO_HIGHEST_IMPL;
    if (prio > normal)
      return PRIO_HIGH_IMPL;
    else if (prio == normal)
      return PRIO_NORMAL_IMPL;
    else if (prio > pmin)
      return PRIO_LOW_IMPL;
    else
      return PRIO_LOWEST_IMPL;
  }
  else return PRIO_HIGHEST_IMPL;
}
