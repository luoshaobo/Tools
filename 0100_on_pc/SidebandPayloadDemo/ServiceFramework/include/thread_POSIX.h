#ifndef THREAD_POSIX_H__
#define THREAD_POSIX_H__

#include "runnable.h"
#include "referencecounter.h"
#include <pthread.h>
// must be limits.h (not <climits>) for PTHREAD_STACK_MIN on Solaris
#include <limits.h>
//#if !defined(POCO_NO_SYS_SELECT_H)
//#include <sys/select.h>
//#endif
#include <errno.h>

#include "runnable.h"
#include <stdio.h>
#include "operatingsystem.h"


class Lib_API ThreadImpl
{
public:
  typedef pthread_t TIDImpl;
  typedef void(*Callable)(void*);

  enum Priority
  {
    PRIO_LOWEST_IMPL,
    PRIO_LOW_IMPL,
    PRIO_NORMAL_IMPL,
    PRIO_HIGH_IMPL,
    PRIO_HIGHEST_IMPL
  };

  enum Policy
  {
    POLICY_DEFAULT_IMPL = SCHED_OTHER
  };

  struct CallbackData : public ReferenceCounter
  {
    CallbackData() : callback(0), pData(0)
    {
    }

    Callable  callback;
    void*     pData;
  };

  ThreadImpl();
  ~ThreadImpl();

  ThreadImpl & operator=(const ThreadImpl&) = delete;
  ThreadImpl(const ThreadImpl&) = delete;

  TIDImpl tidImpl() const;
  void setPriorityImpl(int prio);
  int getPriorityImpl() const;
  void setOSPriorityImpl(int prio, int policy = SCHED_OTHER);
  int getOSPriorityImpl() const;
  static int getMinOSPriorityImpl(int policy);
  static int getMaxOSPriorityImpl(int policy);
  void setStackSizeImpl(int size);
  int getStackSizeImpl() const;
  void startImpl(Runnable& target);
  void startImpl(Callable target, void* pData = 0);

  void joinImpl();
  bool joinImpl(long milliseconds);
  bool isRunningImpl() const;
  static void sleepImpl(long milliseconds);
  static void yieldImpl();
  static ThreadImpl* currentImpl();
  static TIDImpl currentTidImpl();

protected:
  static void* runnableEntry(void* pThread);
  static void* callableEntry(void* pThread);
  static int mapPrio(int prio, int policy = SCHED_OTHER);
  static int reverseMapPrio(int osPrio, int policy = SCHED_OTHER);

private:
  class CurrentThreadHolder
  {
  public:
    CurrentThreadHolder()
    {
      if (pthread_key_create(&_key, NULL))
        printf("cannot allocate thread context key\n");
    }
    ~CurrentThreadHolder()
    {
      pthread_key_delete(_key);
    }
    ThreadImpl* get() const
    {
      return reinterpret_cast<ThreadImpl*>(pthread_getspecific(_key));
    }
    void set(ThreadImpl* pThread)
    {
      pthread_setspecific(_key, pThread);
    }

  private:
    pthread_key_t _key;
  };

  struct ThreadData
  {
    ThreadData() :
      pRunnableTarget(0),
      pCallbackTarget(0),
      thread(0),
      prio(PRIO_NORMAL_IMPL),
      osPrio(PRIO_NORMAL_IMPL),
      policy(SCHED_OTHER),
      stackSize(THREAD_STACK_SIZE),
      started(false),
      joined(false)
    {
    }

    Runnable*     pRunnableTarget;
    CallbackData* pCallbackTarget;
    pthread_t     thread;
    int           prio;
    int           osPrio;
    int           policy;
    //Event         done;
    std::size_t   stackSize;
    bool          started;
    bool          joined;
  };

  ThreadData*     m_pData;

  static CurrentThreadHolder _currentThreadHolder;
};

inline int ThreadImpl::getPriorityImpl() const
{
  return m_pData->prio;
}

inline int ThreadImpl::getOSPriorityImpl() const
{
  return m_pData->osPrio;
}

inline bool ThreadImpl::isRunningImpl() const
{
  return m_pData->pRunnableTarget != 0 ||
    (m_pData->pCallbackTarget->pData != 0 && m_pData->pCallbackTarget->callback != 0);
}

inline void ThreadImpl::yieldImpl()
{
  //causes the calling thread to relinquish the CPU.  The
  //thread is moved to the end of the queue for its static priority and a
  //new thread gets to run.
  sched_yield();
}

inline int ThreadImpl::getStackSizeImpl() const
{
  return static_cast<int>(m_pData->stackSize);
}

inline ThreadImpl::TIDImpl ThreadImpl::tidImpl() const
{
  return m_pData->thread;
}

#endif // THREAD_POSIX_H__
