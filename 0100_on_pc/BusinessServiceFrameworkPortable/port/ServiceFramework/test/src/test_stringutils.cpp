/**
* @file   stringutilstest.cpp
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
#include "stringutils.h"


TUT_TESTSUITE(StringUtilsTest);


TUT_TESTCASE(StringUtils_Trim)
{
  std::string s = "Hello World";
  std::string ret;

  std::string s1 = StringUtils::trim(s);

  TUT_CHECK(s == "Hello World");

  std::string s2 = "  Hello World    ";
  ret = StringUtils::trim(s2);
  TUT_CHECK(ret == "Hello World");

  std::string s3 = "\t\r  Hello World   \r\t";
  ret = StringUtils::trim(s3);
  TUT_CHECK(ret == "Hello World");

  std::string s4 = "  Hello World  ";
  ret = StringUtils::trimLeft(s4);
  TUT_CHECK(ret == "Hello World  ");

  ret = StringUtils::trimRight(s4);
  TUT_CHECK(ret == "  Hello World");

}


TUT_TESTCASE(StringUtils_Upper)
{
  std::string s1 = " aBCde123Fg";
  StringUtils::toUpper(s1);
  TUT_CHECK(s1 == " ABCDE123FG");

}

TUT_TESTCASE(StringUtils_Lower)
{
  std::string s2 = " aBCde123Fg";
  StringUtils::toLower(s2);
  TUT_CHECK(s2 == " abcde123fg");

}
