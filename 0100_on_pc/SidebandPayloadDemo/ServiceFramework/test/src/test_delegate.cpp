/**
* @file   delegatetest.cpp
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

#include <string>
#include "tut/tinyunittest.hpp"
#include "delegate.h"


class TestDelegate
{
public:
  TestDelegate() : bCalled(false)
  {

  }

  void handleResult(std::string &result)
  {
    bCalled = true;
    sResult = result;
  }

  bool bCalled;
  std::string sResult;
};


TUT_TESTSUITE(DelegateTest);


TUT_TESTCASE(Delegate_General)
{
  TestDelegate test;
  Delegate<TestDelegate, std::string> delegate(&test, &TestDelegate::handleResult);

  TUT_CHECK(test.bCalled == false);
  TUT_CHECK(test.sResult.empty() == true);

  bool ret = delegate.notify(0, std::string("Test"));

  TUT_CHECK(ret == true);
  TUT_CHECK(test.bCalled == true);
  TUT_CHECK(test.sResult == "Test");

  Delegate<TestDelegate, std::string> delegate2 = delegate;
  TUT_CHECK(delegate2.equals(delegate) == true);

  TUT_CHECK(delegate.unwrap() != delegate2.unwrap());

  IDelegate<std::string> *newDelegate = delegate.clone();
  TUT_CHECK(newDelegate != 0);
  TUT_CHECK(newDelegate->notify(0, std::string("Test again")));
  TUT_CHECK(test.sResult == "Test again");
  delete newDelegate;

  delegate.disable();
  ret = delegate.notify(0, std::string("Test disabled"));
  TUT_CHECK(ret == false);
  TUT_CHECK(test.sResult == "Test again"); //no call was done, so still have old value!



}
