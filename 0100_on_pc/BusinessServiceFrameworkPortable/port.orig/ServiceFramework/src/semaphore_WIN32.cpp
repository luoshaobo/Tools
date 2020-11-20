#include "operatingsystem.h"
#ifdef OS_WINDOWS
#include "semaphore_WIN32.h"
#include <stdio.h>
#include "comalog.h"

SemaphoreImpl::SemaphoreImpl(int n, int max)
{
	m_sema = CreateSemaphoreW( NULL, n, max, NULL );

	if ( !m_sema )
	{
    COMALOG(COMALOG_ERROR, "Semaphore construct error\n");
	}
}

SemaphoreImpl::~SemaphoreImpl()
{
	CloseHandle( m_sema );
}


void SemaphoreImpl::waitImpl()
{
  DWORD rc = WaitForSingleObject(m_sema, INFINITE);
	switch ( rc )
	{
    case WAIT_OBJECT_0:
    {
      return;
    }
    default:
    {
      COMALOG(COMALOG_ERROR, "Semaphore waitImpl error\n");
      break;
    }
	}
}


bool SemaphoreImpl::waitImpl(long milliseconds)
{
  bool bRet = false;
  DWORD rc = WaitForSingleObject(m_sema, milliseconds /*+ 1*/);
	switch ( rc )
	{
    case WAIT_TIMEOUT:
    {
      bRet = false;
      break;
    }
    case WAIT_OBJECT_0:
    {
      bRet = true;
      break;
    }
    default:
    {
      COMALOG(COMALOG_ERROR, "Semaphore waitImpl(milliseconds) error\n");
      bRet = false;
      break;
    }
	}

  return (bRet);
}

#endif
