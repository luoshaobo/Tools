/**
 * @file
 *
 * @brief
 *          Header file
 * @author  (last changes):
 *          - Elmar Weber
 *          - elmar.weber@continental-corporation.com
 *          - Continental AG
 * @par Project:
 * @par SW-Package:
 *
 * @par SW-Module:
 *
 * @note
 *
 * @par Module-History:
 *  Date        Author                   Reason
 *
 * @par Copyright Notice:
 * Copyright (C) Continental AG 2014
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#ifndef THREAD_EVENT_QUEUE_H__
#define THREAD_EVENT_QUEUE_H__

#include "semaphore.h"
#include "runnable.h"
#include "mutex.h"
#include "thread.h"
#include "event.h"
#include "timerqueue.h"
#include <vector>
#include "eventhandler.h"

class Lib_API ThreadEventQueue : public Runnable
{
public:
  ThreadEventQueue(std::string name);
  virtual ~ThreadEventQueue();

  //void insertEvent(Event* pEvent);
  void insertEvent(Event::Ptr event);

  void addTimer(Timer& timer);
  bool removeTimer(Timer::TimerId id);

  Thread& getThread();

  const std::string& getName() const { return m_name; }

  void stopThread();

protected:
  void run();

protected:
  struct EventShutdown : public Event
  {
    const char* getName() const { return "ThreadEventQueue::EventShutdown"; }
    Event* clone() const { return new EventShutdown(*this); }
  };

  //virtual void eventHandler(Event* pEvent) = 0;
  virtual void eventHandler(Event::Ptr event) = 0;
  EventHandler m_EventHandler;

  //std::vector<Event*> m_vEventQueue;
  std::vector<Event::Ptr> m_vEventQueue;
  std::vector<Event::Ptr> m_vInsertQueue;
private:
  std::string m_name;
  Semaphore m_semaphore;
  Mutex m_mutexInsertQueue;
  Mutex m_mutexTimerQueue;
  Thread m_thread;
  TimerQueue m_timerQueue;

  friend class StatemachineMaster;
};


#endif // THREAD_EVENT_QUEUE_H__
