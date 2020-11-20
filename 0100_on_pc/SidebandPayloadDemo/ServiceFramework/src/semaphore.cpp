#include "semaphore.h"


Semaphore::Semaphore(unsigned int n) : SemaphoreImpl( n, n)
{
}				//n=current value

Semaphore::Semaphore(unsigned int n, unsigned int max) : SemaphoreImpl(n, max)
{
}//max= max value for semaphore

Semaphore::~Semaphore()
{
}

void Semaphore::set()
{
  setImpl();
}

void Semaphore::wait()
{
  waitImpl();
}

void Semaphore::wait(long milliseconds)
{
  waitImpl(milliseconds);
}
