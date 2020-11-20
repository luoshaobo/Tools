/**
* @file   sempahoretest.cpp
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


#include "tut/tinyunittest.hpp"
#include "runnable.h"
#include "semaphore.h"
#include "timestamp.h"
#include "thread.h"


TUT_TESTSUITE(SemaphoreTest);


class SemaphoreRunnalbe : public Runnable
{
public:
  SemaphoreRunnalbe(int n, int max) : m_bRan(false), m_semaphore(n, max)
  {
  }

  void run()
  {
    m_semaphore.wait();
    m_bRan = true;
  }

  bool isRan() const
  {
    return m_bRan;
  }

  void set()
  {
    m_semaphore.set();
  }

  void wait()
  {
    m_semaphore.wait();
  }

  void wait(long milliseconds)
  {
    m_semaphore.wait(milliseconds);
  }

private:
  bool m_bRan;
  Semaphore m_semaphore;
};





TUT_TESTCASE(Semaphore_testInitZero)
{
  SemaphoreRunnalbe r(0, 3);

  //counter is zero
  {
    //wait must block
    Timestamp t1;
    r.wait(10);
    Timestamp t2;


    Timestamp t3 = t2 - t1;
    TUT_CHECK(t3.epochMicroseconds() >= 8000 && t3.epochMicroseconds() <= 12000);
  }

  //counter is set to 1 and wait should not block
  {
    Timestamp t1;
    r.set();
    r.wait();
    Timestamp t2;
    Timestamp t3 = t2 - t1;
    TUT_CHECK(t3.epochMicroseconds() <= 2000);
  }

  //counter is zero wait should block
  {
    Timestamp t1;
    r.wait(10);
    Timestamp t2;
    Timestamp t3 = t2 - t1;
    TUT_CHECK(t3.epochMicroseconds() >= 8000 && t3.epochMicroseconds() <= 12000);
  }

  {
    Thread t;

    //sempahore is zero
    t.start(r);

    //change semaphore to one, so that the run-method does not block
    r.set();

    //check if the bool was set to true
    Thread::sleep(100);
    TUT_CHECK(r.isRan() == true);

    t.join();
  }
}
