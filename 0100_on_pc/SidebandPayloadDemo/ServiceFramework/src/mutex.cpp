#include "mutex.h"

Mutex::Mutex() : MutexImpl(), m_mutexMilliseconds (0)
{
}

Mutex::~Mutex()
{
}

void Mutex::lock()
{
  lockImpl();
}

void Mutex::lock(long milliseconds)
{
  tryLockImpl(milliseconds);
}



void Mutex::unlock()
{
  unlockImpl();
}
