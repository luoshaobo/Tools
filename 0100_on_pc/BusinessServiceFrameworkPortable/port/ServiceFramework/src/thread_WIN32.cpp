#include "thread_WIN32.h"
#include <process.h>
#include "operatingsystem.h"
#include "comalog.h"


ThreadImpl::CurrentThreadHolder ThreadImpl::_currentThreadHolder;


ThreadImpl::ThreadImpl():
	m_pRunnableTarget( 0 ),
	m_thread( 0 ),
	m_threadId( 0 ),
	m_prio( PRIO_NORMAL_IMPL ),
	m_stackSize( THREAD_STACK_SIZE )
{
}


ThreadImpl::~ThreadImpl()
{
	if (m_thread) CloseHandle(m_thread);
}


void ThreadImpl::setPriorityImpl(int prio)
{
	if (prio != m_prio)
	{
		m_prio = prio;
		if (m_thread)
		{
      if (SetThreadPriority(m_thread, m_prio) == 0)
      {
        COMALOG(COMALOG_WARN, "cannot set thread priority\n");
      }
		}
	}
}


void ThreadImpl::setOSPriorityImpl(int prio, int /* policy */)
{
	setPriorityImpl(prio);
}


void ThreadImpl::startImpl(Runnable& target)
{
  if (!isRunningImpl())
  {
    m_pRunnableTarget = &target;

    createImpl(runnableEntry, this);
  }
  else
  {
    COMALOG(COMALOG_WARN, "thread already running\n");
  }
}


void ThreadImpl::startImpl(Callable target, void* pData)
{
  if (!isRunningImpl())
  {

    threadCleanup();
    m_callbackTarget.callback = target;
    m_callbackTarget.pData = pData;

    createImpl(callableEntry, this);
  }
  else
  {
    COMALOG(COMALOG_WARN, "thread already running\n");
  }
}


void ThreadImpl::createImpl(Entry ent, void* pData)
{
#if defined(_DLL)
	m_thread = CreateThread(NULL, m_stackSize, ent, pData, 0, &m_threadId);
#else
	unsigned threadId;
	m_thread = (HANDLE) _beginthreadex(NULL, m_stackSize, ent, this, 0, &threadId);
	m_threadId = static_cast<DWORD>(threadId);
#endif
	if (!m_thread)
	  COMALOG(COMALOG_WARN, "cannot create thread\n");

	if (m_prio != PRIO_NORMAL_IMPL && !SetThreadPriority(m_thread, m_prio))
	  COMALOG(COMALOG_WARN, "cannot set thread priority\n");
}


void ThreadImpl::joinImpl()
{
	if (!m_thread) return;

	switch (WaitForSingleObject(m_thread, INFINITE))
	{
    case WAIT_OBJECT_0:
    {
      threadCleanup();
      return;
    }
    default:
    {
      COMALOG(COMALOG_WARN, "cannot joinImpl thread\n");
      break;
    }
	}
}


bool ThreadImpl::joinImpl(long milliseconds)
{
  bool bRet = false;

  if (!m_thread)
  {
    DWORD rc = WaitForSingleObject(m_thread, milliseconds /*+ 1*/);
    switch (rc)
    {
    case WAIT_TIMEOUT:
    {
      bRet = false;
      break;
    }
    case WAIT_OBJECT_0:
    {
      threadCleanup();
      bRet = true;
      break;
    }
    default:
    {
      bRet = false;
      COMALOG(COMALOG_WARN, "cannot join milliseconds thread\n");
      break;
    }
    }
  }

  return (bRet);
}


bool ThreadImpl::isRunningImpl() const
{
	if (m_thread)
	{
		DWORD ec = 0;
		return GetExitCodeThread(m_thread, &ec) && ec == STILL_ACTIVE;
	}
	return false;
}


void ThreadImpl::threadCleanup()
{
	if (!m_thread) return;
  if (CloseHandle(m_thread)) m_thread = 0;
}


ThreadImpl* ThreadImpl::currentImpl()
{
	return _currentThreadHolder.get();
}


ThreadImpl::TIDImpl ThreadImpl::currentTidImpl()
{
	return GetCurrentThreadId();
}


#if defined(_DLL)
DWORD WINAPI ThreadImpl::runnableEntry(LPVOID pThread)
#else
unsigned __stdcall ThreadImpl::runnableEntry(void* pThread)
#endif
{
  if (pThread != 0)
  {
  _currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));

  reinterpret_cast<ThreadImpl*>(pThread)->m_pRunnableTarget->run();
  }

	return 0;
}


#if defined(_DLL)
DWORD WINAPI ThreadImpl::callableEntry(LPVOID pThread)
#else
unsigned __stdcall ThreadImpl::callableEntry(void* pThread)
#endif
{
	_currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));

  ThreadImpl* pTI = reinterpret_cast<ThreadImpl*>(pThread);

  if (pTI != 0)
  {
	pTI->m_callbackTarget.callback(pTI->m_callbackTarget.pData);
	}

	return 0;
}
