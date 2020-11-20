/**
* @file   threadeventqueuetest.cpp
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
* Copyright (C) Continental AG 2017
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/

#include <algorithm>
#include "tut/tinyunittest.hpp"
#include "threadeventqueue.h"
#include "timestamp.h"
#include "eventstatemachinecontrol.h"


TUT_TESTSUITE(ThreadEventQueueTest);

class Service : public ThreadEventQueue
{
public:
  Service() : ThreadEventQueue("Service")
  {
  }

  void eventHandler(Event::Ptr event) {}
};

class ServiceStress : public ThreadEventQueue, public ITimerExpired
{
public:
  ServiceStress() : ThreadEventQueue("ServiceStress")
  {
  }

  void handleTimeout(int  timer)
  {
    printf("%d expired\n", timer);
  }

  void eventHandler(Event::Ptr event)
  {
    result = result * 3.1415927 / 180.0;
  }

  double result;
};


class TimerExpiredHandler : public ITimerExpired
{
public:
  void handleTimeout(int  timerId)
  {
    //get timestamp when expired
    timestamp.push_back(Timestamp());
    timerid.push_back(timerId);
  }

  std::vector<Timestamp> timestamp;
  std::vector<int> timerid;
};


struct Stress : public Runnable
{

  void run()
  {
    Thread::sleep(1000);
    if (m_pServiceStress != 0)
    {
      EventStatemachineControl*  pStart = new EventStatemachineControl(EventStatemachineControl::EVENT_START);
      m_pServiceStress->insertEvent(pStart);
      Thread::sleep(1);
    }
  }
  ServiceStress *m_pServiceStress;
};

/*
//This is a very long running test which evaluates if ThreadEventQueue is threadsafe
//It is commented because tests should be quickly evalute code. Uncomment if you want to test it again
TUT_TESTCASE(ThreadEventQueue_StressTest)
{

  ServiceStress stress;
  Stress stress2;
  stress2.m_pServiceStress = &stress;

  Thread thread;
  thread.start(stress2);

  int i = 0;

  do
  {
    EventStatemachineControl*  pStart = new EventStatemachineControl(EventStatemachineControl::EVENT_START);
    stress.insertEvent(pStart);

    int test = i % 1000;
    if (i % 1000 == 0)
    {
      Thread::sleep(3000);
    }

    if (i % 100 == 0)
    {
      Timer timer(&stress, 1000000, false);
      stress.addTimer(timer);
    }


  } while (i++ < 100000000);
}
*/

TUT_TESTCASE(ThreadEventQueue_Timer)
{

  Service service;
  TimerExpiredHandler timerHandler;

  TUT_CHECK(service.getName() == "Service");
  TUT_CHECK(service.getThread().getName() == "Service");
  TUT_CHECK(service.getThread().isRunning() == true);

  //periodic timer 100 ms
  Timestamp ts;
  Timer t1(&timerHandler, 100 * 1000, true, true);
  service.addTimer(t1);

  Thread::sleep(500);
  service.removeTimer(t1.getId());

  size_t nTimers = timerHandler.timerid.size();

  TUT_CHECK(timerHandler.timestamp.size() >= 4 && timerHandler.timestamp.size() <= 6);

  //check if all collected expired timers are from the according timerId
  for_each(timerHandler.timerid.begin(), timerHandler.timerid.end(), [t1](int &id) {
    TUT_CHECK(id == t1.getId());
  });

  //we removed the timer, check if there are no timeouts anymore.
  Thread::sleep(200);
  TUT_CHECK(timerHandler.timestamp.size()== nTimers);

  //check if diffs are approx. 100 ms
  for_each(timerHandler.timestamp.begin(), timerHandler.timestamp.end(), [&ts](Timestamp &t) {
    Timestamp diff = t - ts;
    ts = t;
    TUT_CHECK(diff.epochMicroseconds() >= 80000 && diff.epochMicroseconds() <= 120000);
  });

  //TODO implementation for stop thread is missing yet

}
