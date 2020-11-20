#ifndef SEMAPHORE_WIN32_H__
#define SEMAPHORE_WIN32_H__

#include <Windows.h>
#include <stdio.h>


class Lib_API SemaphoreImpl
{
protected:
	SemaphoreImpl(int n, int max);
	~SemaphoreImpl();
	void setImpl();
	void waitImpl();
	bool waitImpl(long milliseconds);

private:
	HANDLE m_sema;
};


inline void SemaphoreImpl::setImpl()
{
  BOOL b = ReleaseSemaphore(m_sema, 1, NULL);

  if ( !b )
	{
    printf("SemaphoreImpl::setImpl error\n");
	}
}


#endif // SEMAPHORE_WIN32_H__
