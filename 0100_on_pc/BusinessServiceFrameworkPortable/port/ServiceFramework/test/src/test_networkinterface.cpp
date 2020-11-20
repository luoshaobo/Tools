/**
* @file
*          test_networkinterface.cpp
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
#include "networkinterface.h"


TUT_TESTSUITE(NetworkInterfaceTest);


TUT_TESTCASE(NetworkInterface_Create)
{
  NetworkInterface::IndexMap indexMap;
  NetworkInterface::create(true, true, NetworkInterface::IPVERSION_V4, indexMap);

  std::string friendlyName = "";

  for (auto it = indexMap.begin(); it != indexMap.end(); it++)
  {

    //for next test we need a friendly name of an ethernet
    if (friendlyName.empty() && (*it).second->getType() == NetworkInterface::IFTYPE_ETHERNET)
    {
      friendlyName = (*it).second->getInterfaceFriendlyName();
    }

    printf("Index:                       %d\n", (*it).first);
    printf("InterfaceName:               %s\n", (*it).second->getInterfaceName().c_str());
    printf("InterfaceFriendlyName:       %s\n", (*it).second->getInterfaceFriendlyName().c_str());
    printf("Description:                 %s\n", (*it).second->getDescription().c_str());

    if ((*it).second->isLoopback())
    {
      printf("Loopback: YES\n");
    }

    if ((*it).second->isUnicast())
    {
      printf("Unicast: YES\n");
    }

    if ((*it).second->isMulticast())
    {
      printf("Multicast: YES\n");
    }

    if ((*it).second->isAnycast())
    {
      printf("Anycast: YES\n");
    }

    if ((*it).second->isRunning())
    {
      printf("Running: YES\n");
    }

    if ((*it).second->isInterfaceUp())
    {
      printf("Interface is UP\n");
    }
    else
    {
      printf("Interface is DOWN\n");
    }

    printf("Interface Type:             %s\n", NetworkInterface::IFTypeStr[(*it).second->getInterfaceType()]);
    printf("MacAddress:                 ");

    NetworkInterface::MacAddress mac = (*it).second->getMacAddress();
    size_t i = 0;
    for (auto macIt = mac.cbegin(); macIt != mac.cend(); macIt++, i++)
    {
      printf("%2x", (*macIt));
      if (i < mac.size() - 1)
      {
        printf(":");
      }
    }
    printf("\n");
    printf("IP Address:                 %s\n", (*it).second->getIPAddress().toString().c_str());

    printf("\n\n");
  }



  NetworkInterface::Ptr pInterface = NetworkInterface::getFriendlyNameInterface(friendlyName);
  //we should have find the NetworkInterface::IFTYPE_ETHERNET of the friendlyName we remembered
  TUT_CHECK(pInterface.isNull() == false);

  if (pInterface.isNull() == true)
  {
    printf("Interface :%s not found\n", friendlyName.c_str());
  }
  else
  {
    printf("Interface :%s found\n", friendlyName.c_str());
  }

  printf("*********************************************\n");

  NetworkInterface::create(true, false, NetworkInterface::IPVERSION_V6, indexMap);


  for (auto it = indexMap.begin(); it != indexMap.end(); it++)
  {

    //for next test we need a friendly name of an ethernet
    if (friendlyName.empty() && (*it).second->getType() == NetworkInterface::IFTYPE_ETHERNET)
    {
      friendlyName = (*it).second->getInterfaceFriendlyName();
    }

    printf("Index:                       %d\n", (*it).first);
    printf("InterfaceName:               %s\n", (*it).second->getInterfaceName().c_str());
    printf("InterfaceFriendlyName:       %s\n", (*it).second->getInterfaceFriendlyName().c_str());
    printf("Description:                 %s\n", (*it).second->getDescription().c_str());

    if ((*it).second->isLoopback())
    {
      printf("Loopback: YES\n");
    }

    if ((*it).second->isUnicast())
    {
      printf("Unicast: YES\n");
    }

    if ((*it).second->isMulticast())
    {
      printf("Multicast: YES\n");
    }

    if ((*it).second->isAnycast())
    {
      printf("Anycast: YES\n");
    }

    if ((*it).second->isRunning())
    {
      printf("Running: YES\n");
    }

    if ((*it).second->isInterfaceUp())
    {
      printf("Interface is UP\n");
    }
    else
    {
      printf("Interface is DOWN\n");
    }

    printf("Interface Type:             %s\n", NetworkInterface::IFTypeStr[(*it).second->getInterfaceType()]);
    printf("MacAddress:                 ");

    NetworkInterface::MacAddress mac = (*it).second->getMacAddress();
    size_t i = 0;
    for (auto macIt = mac.cbegin(); macIt != mac.cend(); macIt++, i++)
    {
      printf("%2x", (*macIt));
      if (i < mac.size() - 1)
      {
        printf(":");
      }
    }
    printf("\n");
    printf("IP Address:                 %s\n", (*it).second->getIPAddress().toString().c_str());

    printf("\n\n");
  }
}
