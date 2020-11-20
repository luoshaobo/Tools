/**
* @file
*          test_ipaddress.cpp
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
* Copyright (C) Continental AG 2018
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/


#include "tut/tinyunittest.hpp"
#include "ipaddress.h"


TUT_TESTSUITE(IpAddressTest);


TUT_TESTCASE(IpAddress_Parse)
{
  IPAddress result;

  //Test IP Address Version 4
  TUT_CHECK(IPAddress::parse("192.168.1.1", result) == true);

  //Test IP Address Version 6
  TUT_CHECK(IPAddress::parse("ffaa:123:aaa:110:3242:2341:142:124", result) == true);

  IPAddress i1_4("192.168.1.1");
  IPAddress i1_6("ffaa:123:aaa:110:3242:2341:142:124");

  TUT_CHECK(i1_4.getFamily() == IPAddress::FAMILY_IPV4);
  TUT_CHECK(i1_6.getFamily() == IPAddress::FAMILY_IPV6);
}

TUT_TESTCASE(IpAddress_ToString)
{
  IPAddress result;

  IPAddress  prefixTest(9, IPAddress::FAMILY_IPV4);
  printf("%s\n", prefixTest.toString().c_str());

  TUT_CHECK(IPAddress::parse("255.128.0.0", result) == true);

  //Test IP Address Version 4
  TUT_CHECK(IPAddress::parse("192.168.1.1", result) == true);
  printf("%s\n", result.toString().c_str());

  //Test IP Address Version 6
  TUT_CHECK(IPAddress::parse("ffaa:123:aaa:110:3242:2341:142:124", result) == true);
  printf("%s\n", result.toString().c_str());

  IPAddress i1_4("192.168.1.1");
  IPAddress i1_6("ffaa:123:aaa:110:3242:2341:142:124");
  printf("%s\n", i1_4.toString().c_str());
  printf("%s\n", i1_6.toString().c_str());

  TUT_CHECK(i1_4.getFamily() == IPAddress::FAMILY_IPV4);
  TUT_CHECK(i1_6.getFamily() == IPAddress::FAMILY_IPV6);
}


TUT_TESTCASE(IpAddress_IsWildcard)
{
  IPAddress result;

  //Test IP Address Version 4
  TUT_CHECK(IPAddress::parse("0.0.0.0", result) == true);

  TUT_CHECK(result.isWildcard() == true);

  TUT_CHECK(IPAddress::parse("1.0.0.1", result) == true);

  TUT_CHECK(result.isWildcard() == false);
}

TUT_TESTCASE(IpAddress_IsLoopback)
{
  IPAddress result;

  //Test IP Address Version 4
  TUT_CHECK(IPAddress::parse("0.0.0.0", result) == true);
  TUT_CHECK(result.isLoopback() == false);
  TUT_CHECK(IPAddress::parse("127.0.0.1", result) == true);
  TUT_CHECK(result.isLoopback() == true);
  TUT_CHECK(IPAddress::parse("127.1.1.1", result) == true);
  TUT_CHECK(result.isLoopback() == true);

  //Ipv6
  TUT_CHECK(IPAddress::parse("0000:0000:0000:0000:0000:0000:0000:0001", result) == true);
  TUT_CHECK(result.isLoopback() == true);

}


TUT_TESTCASE(IpAddress_IsMulticast)
{
  IPAddress result;

  //Ipv4
  TUT_CHECK(IPAddress::parse("224.0.0.0", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("232.0.0.0", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("233.0.0.0", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("234.0.0.0", result) == true);
  TUT_CHECK(result.isMulticast() == true);


  //Test IP Address Version 6
  TUT_CHECK(IPAddress::parse("FF02::1", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("FF02::2", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("FF02::5", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("FF02::6", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("FF02::9", result) == true);
  TUT_CHECK(result.isMulticast() == true);
  TUT_CHECK(IPAddress::parse("FF02::A", result) == true);
  TUT_CHECK(result.isMulticast() == true);

}


TUT_TESTCASE(IpAddress_IsSiteLocal)
{
  IPAddress result;

  //Test IP Address Version 4
  TUT_CHECK(IPAddress::parse("192.168.1.113", result) == true);
  TUT_CHECK(result.isSiteLocal() == true);
  TUT_CHECK(IPAddress::parse("192.168.2.113", result) == true);
  TUT_CHECK(result.isSiteLocal() == true);
  TUT_CHECK(IPAddress::parse("192.168.3.113", result) == true);
  TUT_CHECK(result.isSiteLocal() == true);
  TUT_CHECK(IPAddress::parse("216.58.192.196", result) == true); //www.google.com
  TUT_CHECK(result.isSiteLocal() == false);

  //Ipv6
  TUT_CHECK(IPAddress::parse("fc00::7", result) == true);
  TUT_CHECK(result.isSiteLocal() == true);

}

TUT_TESTCASE(IpAddress_AssignOperator)
{
  IPAddress rhs("1.2.3.4");

  //assign
  IPAddress leftValue = rhs;

  TUT_CHECK(leftValue.getFamily() == rhs.getFamily());
  TUT_CHECK(leftValue == rhs);

  IPAddress compare("1.2.3.5");
  TUT_CHECK(leftValue != compare);


  IPAddress rhsv6("2001:0db8:0001:0000:0000:0ab9:C0A8:0102");
  IPAddress lhsv6("2001:db8:1::ab9:C0A8:102");
  TUT_CHECK(lhsv6 == rhsv6);

  IPAddress comparev6("2001:db8:1::ab9:C0A8:103");
  TUT_CHECK(lhsv6 != comparev6);
  TUT_CHECK(comparev6 != rhsv6);

}

TUT_TESTCASE(IpAddress_FindIpAddress)
{
  IPAddress result;
  std::string url = "https://192.168.1.112/?clientid=CONTI0006";
  TUT_CHECK(IPAddress::findIpAddress(url, result) == true );
  TUT_CHECK(result == IPAddress("192.168.1.112"));

  url = "https://192.168.1.113:8080/?clientid=CONTI0006";
  TUT_CHECK(IPAddress::findIpAddress(url, result) == true);
  TUT_CHECK(result == IPAddress("192.168.1.113"));

  url = "http://[2001:db8:a0b:12f0::1]/index.html";
  TUT_CHECK(IPAddress::findIpAddress(url, result) == true);
  TUT_CHECK(result == IPAddress("2001:db8:a0b:12f0::1"));

  url = "http://[2001:db8:a0b:12f0::1]:80/index.html";
  TUT_CHECK(IPAddress::findIpAddress(url, result) == true);
  TUT_CHECK(result == IPAddress("2001:db8:a0b:12f0::1"));

  url = "[2001:db8:a0b:12f0::1%eth0]:80";
  TUT_CHECK(IPAddress::findIpAddress(url, result) == true);
  TUT_CHECK(result == IPAddress("2001:db8:a0b:12f0::1"));

  url = "https://backend.continental.com/?clientid=CONTI0006";
  TUT_CHECK(IPAddress::findIpAddress(url, result) == false);  //no ip-adress in url
}


TUT_TESTCASE(IpAddress_PrefixKonstruktor)
{
  IPAddress Prefixtest32(32, IPAddress::FAMILY_IPV4);
  TUT_CHECK(Prefixtest32.toString() == "255.255.255.255");

  IPAddress Prefixtest24(24, IPAddress::FAMILY_IPV4);
  TUT_CHECK(Prefixtest24.toString() == "255.255.255.0");

  IPAddress Prefixtest16(16, IPAddress::FAMILY_IPV4);
  TUT_CHECK(Prefixtest16.toString() == "255.255.0.0");

  IPAddress Prefixtest8(8, IPAddress::FAMILY_IPV4);
  TUT_CHECK(Prefixtest8.toString() == "255.0.0.0");

  //Test IPV4
  IPAddress Prefixtest(5, IPAddress::FAMILY_IPV4);

  IPAddress Prefixresult;
  IPAddress::parse("248.0.0.0", Prefixresult);

  TUT_CHECK(Prefixtest.getFamily() == IPAddress::FAMILY_IPV4);
  TUT_CHECK(Prefixtest == Prefixresult);

  //printf("Prefix: %s\n", Prefixtest.toString().c_str());
  //printf("Prefix: %s\n", Prefixresult.toString().c_str());

  //Test IPV6
  IPAddress Prefixtest2(86, IPAddress::FAMILY_IPV6);

  IPAddress Prefixresult2;
  IPAddress::parse("ffff:ffff:ffff:ffff:ffff:fc00::", Prefixresult2);

  TUT_CHECK(Prefixtest2.getFamily() == IPAddress::FAMILY_IPV6);
  TUT_CHECK(Prefixtest2 == Prefixresult2);

  //printf("Prefix: %s\n", Prefixtest2.toString().c_str());
  //printf("Prefix: %s\n", Prefixresult2.toString().c_str());
}
