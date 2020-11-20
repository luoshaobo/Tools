#ifndef THREAD_WIN32_H__
#define THREAD_WIN32_H__

#include "Windows.h"
#include "runnable.h"
#include <stdio.h>
#include "operatingsystem.h"

class Lib_API ThreadImpl
{
public:
	typedef DWORD TIDImpl;
	typedef void (*Callable)(void*);

#if defined(_DLL)
	typedef DWORD (WINAPI *Entry)(LPVOID);
#else
	typedef unsigned (__stdcall *Entry)(void*);
#endif

	struct CallbackData
	{
		CallbackData(): callback(0), pData(0)
		{
		}

		Callable  callback;
		void*     pData;
	};

	enum Priority
	{
		PRIO_LOWEST_IMPL  = THREAD_PRIORITY_LOWEST,
		PRIO_LOW_IMPL     = THREAD_PRIORITY_BELOW_NORMAL,
		PRIO_NORMAL_IMPL  = THREAD_PRIORITY_NORMAL,
		PRIO_HIGH_IMPL    = THREAD_PRIORITY_ABOVE_NORMAL,
		PRIO_HIGHEST_IMPL = THREAD_PRIORITY_HIGHEST
	};

	enum Policy
	{
		POLICY_DEFAULT_IMPL = 0
	};

	ThreadImpl();
	~ThreadImpl();

        ThreadImpl(ThreadImpl const&) = delete;
        ThreadImpl& operator=(ThreadImpl const&) = delete;

	TIDImpl tidImpl() const;
	void setPriorityImpl(int prio);
	int getPriorityImpl() const;
	void setOSPriorityImpl(int prio, int policy = 0);
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
#if defined(_DLL)
	static DWORD WINAPI runnableEntry(LPVOID pThread);
#else
	static unsigned __stdcall runnableEntry(void* pThread);
#endif

#if defined(_DLL)
	static DWORD WINAPI callableEntry(LPVOID pThread);
#else
	static unsigned __stdcall callableEntry(void* pThread);
#endif

	void createImpl(Entry ent, void* pData);
	void threadCleanup();

private:
	class CurrentThreadHolder
	{
	public:
		CurrentThreadHolder(): m_slot( TlsAlloc() )
		{
      if (m_slot == TLS_OUT_OF_INDEXES)
      {
        printf("cannot allocate thread context\n");;
      }
		}
		~CurrentThreadHolder()
		{
			TlsFree(m_slot);
		}
		ThreadImpl* get() const
		{
			return reinterpret_cast<ThreadImpl*>(TlsGetValue(m_slot));
		}
		void set(ThreadImpl* pThread)
		{
			TlsSetValue(m_slot, pThread);
		}

	private:
		DWORD m_slot;
	};

	Runnable*    m_pRunnableTarget;
	CallbackData m_callbackTarget;
	HANDLE       m_thread;
	DWORD        m_threadId;
	int          m_prio;
	int          m_stackSize;

	static CurrentThreadHolder _currentThreadHolder;
};


inline int ThreadImpl::getPriorityImpl() const
{
	return m_prio;
}


inline int ThreadImpl::getOSPriorityImpl() const
{
	return m_prio;
}


inline int ThreadImpl::getMinOSPriorityImpl(int /* policy */)
{
	return PRIO_LOWEST_IMPL;
}


inline int ThreadImpl::getMaxOSPriorityImpl(int /* policy */)
{
	return PRIO_HIGHEST_IMPL;
}


inline void ThreadImpl::sleepImpl(long milliseconds)
{
	Sleep(DWORD(milliseconds));
}


inline void ThreadImpl::yieldImpl()
{
	Sleep(0);
}


inline void ThreadImpl::setStackSizeImpl(int size)
{
	m_stackSize = size;
}


inline int ThreadImpl::getStackSizeImpl() const
{
	return m_stackSize;
}


inline ThreadImpl::TIDImpl ThreadImpl::tidImpl() const
{
	return m_threadId;
}

#endif // THREAD_WIN32_H__
