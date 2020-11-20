#include "thread.h"
#include "scopedlock.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

Thread::Thread(const std::string &name) : ThreadImpl(),  m_id(uniqueId()), m_name(name), m_run(false), m_mutex()
{
}

Thread::Thread() : ThreadImpl(), m_id(uniqueId()), m_name(makeName()), m_run(false), m_mutex()
{
}

Thread::~Thread()
{
}

Thread::TID Thread::getTid() const
{
  return getCurrentTid();
}

std::string Thread::getName() const
{
  return m_name;
}

void Thread::setName(std::string name)
{
  m_name = name;
}

void Thread::setStackSize(int size)
{
  setStackSizeImpl(size);
}

int Thread::getStackSize()const
{
  return getStackSizeImpl();
}

void Thread::start(Runnable &runnable)
{
  startImpl(runnable);
}

void Thread::join()
{
  joinImpl();
}

bool Thread::isRunning() const
{

  return isRunningImpl();
}

void Thread::sleep(long milliseconds)
{
  sleepImpl(milliseconds);
}

void Thread::yield()
{
  yieldImpl();
}

Thread::TID Thread::getCurrentTid()
{
  return currentTidImpl();
}

std::string Thread::makeName()
{
  std::ostringstream name;
  name << '#' << m_id;
  return name.str();
}

int Thread::uniqueId()
{
  static Mutex uniqueIdMutex;

  ScopedLock lock(uniqueIdMutex);

  static unsigned count = 0;
  ++count;
  return count;
}
