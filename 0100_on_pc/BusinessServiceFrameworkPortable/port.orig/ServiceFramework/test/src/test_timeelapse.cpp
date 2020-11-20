/**
* @file   timeelapsetest.cpp
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
* Copyright (C) Continental AG 2016
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/


#include "tut/tinyunittest.hpp"
#include "timeelapse.h"


TUT_TESTSUITE(TimeElapseTest);

TUT_TESTCASE(TimeElapse_CheckInit)
{
  TimeElapse t;

  TUT_CHECK(t.getMicroseconds() == 0);
  TUT_CHECK(t.getDays() == 0);
  TUT_CHECK(t.getFraction() == 0);
  TUT_CHECK(t.getHours() == 0);
  TUT_CHECK(t.getMilliseconds() == 0);
  TUT_CHECK(t.getMinutes() == 0);
  TUT_CHECK(t.getSeconds() == 0);
  TUT_CHECK(t.getTotalHours() == 0);
  TUT_CHECK(t.getTotalMicroseconds() == 0);
  TUT_CHECK(t.getTotalMilliseconds() == 0);
  TUT_CHECK(t.getTotalMinutes() == 0);
  TUT_CHECK(t.getTotalSeconds() == 0);
}


TUT_TESTCASE(TimeElapse_CheckConversion)
{
  TimeElapse t;

  t = TimeElapse::DAYS;

  //Static calculations
  TUT_CHECK(t.getTotalMicroseconds() == TimeElapse::DAYS);
  TUT_CHECK(t.getTotalMilliseconds() == 24 * 60 * 60 * 1000);
  TUT_CHECK(t.getTotalSeconds() == 24 * 60 * 60);
  TUT_CHECK(t.getTotalMinutes() == 24 * 60);
  TUT_CHECK(t.getTotalHours() == 24);
  TUT_CHECK(t.getDays() == 1);

  //Still no Elapse calculated, all should be 0
  TUT_CHECK(t.getMicroseconds() == 0);
  TUT_CHECK(t.getMilliseconds() == 0);
  TUT_CHECK(t.getSeconds() == 0);
  TUT_CHECK(t.getMinutes() == 0);
  TUT_CHECK(t.getHours() == 0);



  t.setElapse(1, 2, 3, 4, 555666);
  TUT_CHECK(t.getMicroseconds() == 666);
  TUT_CHECK(t.getMilliseconds() == 555);
  TUT_CHECK(t.getSeconds() == 4);
  TUT_CHECK(t.getMinutes() == 3);
  TUT_CHECK(t.getHours() == 2);
  TUT_CHECK(t.getDays() == 1);
  TUT_CHECK(t.getFraction() == 555666);


  t.setElapse(0, 1, 2, 3, 444555);
  TUT_CHECK(t.getMicroseconds() == 555);
  TUT_CHECK(t.getMilliseconds() == 444);
  TUT_CHECK(t.getSeconds() == 3);
  TUT_CHECK(t.getMinutes() == 2);
  TUT_CHECK(t.getHours() == 1);
  TUT_CHECK(t.getDays() == 0);
  TUT_CHECK(t.getFraction() == 444555);

  t.setElapse(0, 23, 59, 59, 987654);
  TUT_CHECK(t.getMicroseconds() == 654);
  TUT_CHECK(t.getMilliseconds() == 987);
  TUT_CHECK(t.getSeconds() == 59);
  TUT_CHECK(t.getMinutes() == 59);
  TUT_CHECK(t.getHours() == 23);
  TUT_CHECK(t.getDays() == 0);
  TUT_CHECK(t.getFraction() == 987654);
}


TUT_TESTCASE(TimeElapse_CheckComparison)
{
  TimeElapse t1(1 * TimeElapse::HOURS);
  TimeElapse t2(2 * TimeElapse::HOURS);
  TimeElapse t3(3 * TimeElapse::HOURS);
  TimeElapse t3m(3*60 * TimeElapse::MINUTES);

  TUT_CHECK(t1 != t2);
  TUT_CHECK(t2 != t3);
  TUT_CHECK(t1 != t3);

  TUT_CHECK(!(t1 == t2));
  TUT_CHECK(!(t2 == t3));
  TUT_CHECK(!(t1 == t3));

  TUT_CHECK(t1 < t3);
  TUT_CHECK(t2 < t3);
  TUT_CHECK(t1 <= t3);
  TUT_CHECK(t2 <= t3);
  TUT_CHECK(t3m <= t3);
  TUT_CHECK(t3 <= t3m);

  TUT_CHECK(t3 > t1);
  TUT_CHECK(t3 > t2);
  TUT_CHECK(t3 >= t1);
  TUT_CHECK(t3 >= t2);
  TUT_CHECK(t3m >= t3);
  TUT_CHECK(t3 >= t3m);

  TUT_CHECK(t3 == t3m);
  TUT_CHECK(t3 == 3 * TimeElapse::HOURS);
  TUT_CHECK(t2 == 2 * TimeElapse::HOURS);
  TUT_CHECK(t1 == 1 * TimeElapse::HOURS);
  TUT_CHECK(t3 < 4 * TimeElapse::HOURS);
  TUT_CHECK(t3 > 2 * TimeElapse::HOURS);
  TUT_CHECK(t3 >= 3 * TimeElapse::HOURS);
  TUT_CHECK(t3 <= 3 * TimeElapse::HOURS);

}

TUT_TESTCASE(TimeElapse_CheckArithmetic)
{
  TimeElapse t1(1 * TimeElapse::DAYS + 1 * TimeElapse::HOURS);
  TimeElapse t2(1 * TimeElapse::HOURS);

  TimeElapse t3 = t1 + t2;
  TUT_CHECK(t3 == 1 * TimeElapse::DAYS + 2 * TimeElapse::HOURS);

  t3 = t1 + 1 * TimeElapse::DAYS + 1 * TimeElapse::HOURS;
  TUT_CHECK(t3 == 2 * TimeElapse::DAYS + 2 * TimeElapse::HOURS);

  t3 = t1 - t2;
  TUT_CHECK(t3 == 1 * TimeElapse::DAYS + 0 * TimeElapse::HOURS);

  t3 = t1 - 2 * TimeElapse::HOURS;
  TUT_CHECK(t3 == 23 * TimeElapse::HOURS);

  t3 += 25 * TimeElapse::HOURS;
  TUT_CHECK(t3 == 2 * TimeElapse::DAYS);
  TUT_CHECK(t3 == 48 * TimeElapse::HOURS);

  t3 -= t2;
  TUT_CHECK(t3 == 47 * TimeElapse::HOURS);
  TUT_CHECK(t3 == 1 * TimeElapse::DAYS + 23 * TimeElapse::HOURS);
}


TUT_TESTCASE(TimeElapse_CheckSwap)
{
  TimeElapse t1(1 * TimeElapse::HOURS);
  TimeElapse t2(2 * TimeElapse::HOURS);

  TUT_CHECK(t1 < t2);
  t1.swap(t2);
  TUT_CHECK(t2 < t1);

  TUT_CHECK(t1 == 2 * TimeElapse::HOURS);
  TUT_CHECK(t2 == 1 * TimeElapse::HOURS);
}

/*
#include "timestamp.h"
TUT_TESTCASE(TimeStamp_Test)
{
  std::time_t t;
  t.
  Timestamp Timestamp::fromEpochTime();

  Timestamp s1();
  TimeElapse t1(1 * TimeElapse::HOURS);
  TimeElapse t2(2 * TimeElapse::HOURS);

  TUT_CHECK(t1 < t2);
  t1.swap(t2);
  TUT_CHECK(t2 < t1);

  TUT_CHECK(t1 == 2 * TimeElapse::HOURS);
  TUT_CHECK(t2 == 1 * TimeElapse::HOURS);
}*/