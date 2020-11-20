/**
* @file   atomiccountertest.cpp
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
#include "atomiccounter.h"


TUT_TESTSUITE(AtomicCounterTest);


TUT_TESTCASE(AtomicCounter_Init)
{
  AtomicCounter c;
  TUT_CHECK(c.value() == 0);

  AtomicCounter c1(1);
  TUT_CHECK(c1.value() == 1);

  AtomicCounter c2(1000);
  TUT_CHECK(c2.value() == 1000);
}


TUT_TESTCASE(AtomicCounter_Assign)
{
  AtomicCounter c;
  TUT_CHECK(c.value() == 0);

  AtomicCounter c2(1);
  TUT_CHECK(c2.value() == 1);

  c = c2;
  TUT_CHECK(c.value() == 1);

  c2 = 5;
  TUT_CHECK(c2.value() == 5);

}

TUT_TESTCASE(AtomicCounter_Count)
{
  AtomicCounter c;
  TUT_CHECK(c.value() == 0);

  AtomicCounter c2;
  c2 = 5;
  TUT_CHECK(c2.value() == 5);

  c2++;
  TUT_CHECK(c2.value() == 6);

  ++c2;
  TUT_CHECK(c2.value() == 7);

  --c2;
  TUT_CHECK(c2.value() == 6);

  c2--;
  TUT_CHECK(c2.value() == 5);
}

TUT_TESTCASE(AtomicCounter_Bool)
{
  AtomicCounter c;
  TUT_CHECK(!c);
}