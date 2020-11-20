/**
* @file
*          networkinterface.cpp
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

#include "networkinterface.h"
#include "scopedlock.h"


#ifdef OS_WINDOWS
#include <wincrypt.h>
#include <iphlpapi.h>
#include <ipifcons.h>

//force linking for that needed library
#pragma comment(lib, "IPHLPAPI.lib")

#else
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include "stringutils.h"
#include <fstream>
#include <iostream>
#endif

//define mutex
Mutex NetworkInterface::m_mutex;

//support for enum to string
const char* NetworkInterface::IFTypeStr[]
{
  "IFTYPE_OTHER",
  "IFTYPE_ETHERNET",
  "IFTYPE_TOKENRING",
  "IFTYPE_FDDI",
  "IFTYPE_PPP",
  "IFTYPE_LOOPBACK",
  "IFTYPE_SLIP",
  "IFTYPE_TUNNEL"
};

NetworkInterface::NetworkInterface(unsigned int index) :
  m_sDescription(),
  m_sInterfaceFriendlyName(),
  m_sInterfaceName(),
  m_nIndex(index),
  m_bBroadcast(false),
  m_bLoopback(false),
  m_bMulticast(false),
  m_bPointToPoint(false),
  m_bUp(false),
  m_bRunning(false),
  m_nMtu(0),
  m_eIfType(NetworkInterface::IFTYPE_OTHER),
  m_sMacAddress()
{
}

NetworkInterface::~NetworkInterface()
{

}

NetworkInterface::Ptr NetworkInterface::getFriendlyNameInterface(const std::string& name)
{
  NetworkInterface::Ptr ret;
  NetworkInterface::IndexMap indexMap;
  NetworkInterface::create(false, false, NetworkInterface::IPVERSION_V4_V6, indexMap);

  for (auto it = indexMap.begin(); it != indexMap.end() && ret.isNull(); it++)
  {
    if ((*it).second->getInterfaceFriendlyName() == name)
    {
      ret = (*it).second;
    }
  }

  return (ret);
}

const std::string NetworkInterface::getDescription() const
{
  return (m_sDescription);
}

const std::string NetworkInterface::getInterfaceName() const
{
  return (m_sInterfaceName);
}

const std::string NetworkInterface::getInterfaceFriendlyName() const
{
  return (m_sInterfaceFriendlyName);
}

unsigned int NetworkInterface::getIndex() const
{
  return (m_nIndex);
}

//unsigned int NetworkInterface::getNumberOfUnicasts() const
//{
//  return (m_nNumberOfUnicasts);
//}
//
//unsigned int NetworkInterface::getNumberOfAnycasts() const
//{
//  return (m_nNumberOfAnycasts);
//}
//
//unsigned int NetworkInterface::getNumberOfMulticasts() const
//{
//  return (m_nNumberOfMulticasts);
//}
//
//unsigned int NetworkInterface::getNumberOfDnsServers() const
//{
//  return (m_nNumberOfDnsServers);
//}
//
//const std::vector<IPAddress>& NetworkInterface::getDnsAddress() const
//{
//  return (m_vDnsAddresses);
//}

NetworkInterface::IFType NetworkInterface::getInterfaceType() const
{
  return (m_eIfType);
}

NetworkInterface::MacAddress NetworkInterface::getMacAddress() const
{
  return (m_sMacAddress);
}

unsigned int NetworkInterface::getMtu() const
{
  return (m_nMtu);
}

const IPAddress& NetworkInterface::getIPAddress() const
{
  return (m_cIpAddress);
}

const IPAddress& NetworkInterface::getMaskAddress() const
{
  return (m_cMaskAddress);
}

bool NetworkInterface::isLoopback() const
{
  return (m_bLoopback);
}

bool NetworkInterface::isAnycast() const
{
  return (m_bBroadcast);
}


bool NetworkInterface::isMulticast() const
{
  return (m_bMulticast);
}


bool NetworkInterface::isUnicast() const
{
  return (m_bPointToPoint);
}

bool NetworkInterface::isInterfaceUp() const
{
  return (m_bUp);
}

bool NetworkInterface::isRunning() const
{
  return (m_bRunning);
}

NetworkInterface::IFType NetworkInterface::getType() const
{
  return (m_eIfType);
}


#ifdef OS_WINDOWS
void NetworkInterface::setType(unsigned int networktype)
{
  switch (networktype)
  {
  case IF_TYPE_ETHERNET_CSMACD:
  {
    m_eIfType = NetworkInterface::IFTYPE_ETHERNET;
    break;
  }
  case IF_TYPE_ISO88025_TOKENRING:
  {
    m_eIfType = NetworkInterface::IFTYPE_TOKENRING;
    break;
  }
  case IF_TYPE_FDDI:
  {
    m_eIfType = NetworkInterface::IFTYPE_FDDI;
    break;
  }
  case IF_TYPE_PPP:
  {
    m_eIfType = NetworkInterface::IFTYPE_PPP;
    break;
  }
  case IF_TYPE_SOFTWARE_LOOPBACK:
  {
    m_eIfType = NetworkInterface::IFTYPE_LOOPBACK;
    break;
  }
  case IF_TYPE_SLIP:
  {
    m_eIfType = NetworkInterface::IFTYPE_SLIP;
    break;
  }
  case IF_TYPE_TUNNEL:
  {
    m_eIfType = NetworkInterface::IFTYPE_TUNNEL;
    break;
  }
  default:
  {
    m_eIfType = NetworkInterface::IFTYPE_OTHER;
    break;
  }
  }
}

IPAddress subnetMaskForInterface(const std::string& interfaceName, bool isLoopback)
{
  IPAddress ret;
  if (isLoopback)
  {
    IPAddress::parse("255.0.0.0", ret);
  }
  else
  {
    std::string subKey("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\");
    subKey += interfaceName;

    //open registry and read key
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
      char netmask[16];
      DWORD size = sizeof(netmask);

      //search DchpSubnetMask or SubnetMask
      std::string sNetmask;
      if (RegQueryValueExA(hKey, "DhcpSubnetMask", NULL, NULL, (LPBYTE)&netmask, &size) == ERROR_SUCCESS ||
        RegQueryValueExA(hKey, "SubnetMask", NULL, NULL, (LPBYTE)&netmask, &size) == ERROR_SUCCESS)
      {
        sNetmask = netmask;
      }

      RegCloseKey(hKey);
      IPAddress::parse(sNetmask, ret);
    }
  }

  return (ret);
}

bool NetworkInterface::create(bool ipOnly, bool upOnly, NetworkInterface::IPVersion ipVersion, NetworkInterface::IndexMap &indexMap)
{
  bool bRet = true;
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

  // Declare and initialize variables
  DWORD dwSize = 0;
  DWORD dwRetVal = 0;

  // Set the flags to pass to GetAdaptersAddresses
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

  LPVOID lpMsgBuf = NULL;

  PIP_ADAPTER_ADDRESSES pAddresses = 0;

  PIP_ADAPTER_ADDRESSES pCurrAddresses = 0;
  PIP_ADAPTER_UNICAST_ADDRESS pUnicast = 0;
  PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = 0;
  PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = 0;
  IP_ADAPTER_DNS_SERVER_ADDRESS *pDnsServer = 0;
  IP_ADAPTER_PREFIX *pPrefix = NULL;

  //specified address family (both)
  ULONG family = AF_UNSPEC;
  switch (ipVersion)
  {
  case IPVERSION_V4:
  {
    family = AF_INET;
    break;
  }
  case IPVERSION_V6:
  {
    family = AF_INET6;
    break;
  }
  case IPVERSION_V4_V6:
  default:
  {
    family = AF_UNSPEC;
    break;
  }
  }

  ULONG outBufLen = 0;
  outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
  pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);

  // Make an initial call to GetAdaptersAddresses to get the
  // size needed into the outBufLen variable
  if (GetAdaptersAddresses(family, flags, 0, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
  {
    FREE(pAddresses);
    pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
  }

  //do we have a valid address
  if (pAddresses != 0)
  {
    dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

    if (dwRetVal == NO_ERROR)
    {
      // If successful, output some information from the data we received
      pCurrAddresses = pAddresses;
      while (pCurrAddresses)
      {
        //
        // OperStatus values from RFC 2863
        // ifdef.h enumerations
     /*   typedef enum {
          IfOperStatusUp = 1,
          IfOperStatusDown,
          IfOperStatusTesting,
          IfOperStatusUnknown,
          IfOperStatusDormant,
          IfOperStatusNotPresent,
          IfOperStatusLowerLayerDown
        } IF_OPER_STATUS;*/


        bool isUp = (pCurrAddresses->OperStatus == IfOperStatusUp);
        bool isIP = (pCurrAddresses->FirstUnicastAddress != 0);

        if (((ipOnly && isIP) || !ipOnly) && ((upOnly && isUp) || !upOnly))
        {
          //create NetworkInterface
          Ptr pNetworkInterface = new NetworkInterface(pCurrAddresses->IfIndex);

          pNetworkInterface->m_bUp = isUp;
          pNetworkInterface->setType(static_cast<unsigned int>(pCurrAddresses->IfType));
          pNetworkInterface->m_sInterfaceName = pCurrAddresses->AdapterName;

          //pNetworkInterface->m = subnetMaskForInterface(pNetworkInterface->m_sInterfaceName, false);

          //get  "Mobile Breitbandverbindung 6"
          char nameBuffer[1024];
          int rc = WideCharToMultiByte(CP_ACP, 0, pCurrAddresses->FriendlyName, -1, nameBuffer, sizeof(nameBuffer), NULL, NULL);
          if (rc)
          {
            pNetworkInterface->m_sInterfaceFriendlyName = nameBuffer;
          }

          //get "Continental HS-USB WWAN Adapter 9025 #6"
          char displayNameBuffer[1024];
          rc = WideCharToMultiByte(CP_ACP, 0, pCurrAddresses->Description, -1, displayNameBuffer, sizeof(displayNameBuffer), NULL, NULL);
          if (rc)
          {
            pNetworkInterface->m_sDescription = displayNameBuffer;
          }

          unsigned nNetworkInterfaceIndex = INVALID_INDEX;
          if (family == IPVERSION_V4 || family == IPVERSION_V4_V6)
          {
            nNetworkInterfaceIndex = pCurrAddresses->IfIndex;
          }
          else
          {
            nNetworkInterfaceIndex = pCurrAddresses->Ipv6IfIndex;
          }

          //add to map if not already there
          IndexMap::iterator it = indexMap.find(nNetworkInterfaceIndex);
          if (it == indexMap.end())
          {
            it = indexMap.insert(IndexMap::value_type(nNetworkInterfaceIndex, pNetworkInterface)).first;
          }

          //handle unicast address
          IPAddress address;
          if (pCurrAddresses->FirstUnicastAddress != 0)
          {
            pNetworkInterface->m_bPointToPoint = true;
          }

          for (PIP_ADAPTER_UNICAST_ADDRESS pUniAddr = pCurrAddresses->FirstUnicastAddress; pUniAddr != 0; pUniAddr = pUniAddr->Next)
          {
            //count unicast addresses
            //pNetworkInterface->m_nNumberOfUnicasts++;

            SOCKET_ADDRESS sockaddr = pUniAddr->Address;
            ADDRESS_FAMILY family = pUniAddr->Address.lpSockaddr->sa_family;

            //https://msdn.microsoft.com/de-de/library/windows/desktop/ms740507(v=vs.85).aspx
            switch (family)
            {
            case AF_INET:
            {
              IN_ADDR addr = reinterpret_cast<const struct sockaddr_in*>(pUniAddr->Address.lpSockaddr)->sin_addr;

              char str[INET_ADDRSTRLEN];
              inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
              pNetworkInterface->m_cIpAddress = IPAddress(str);
              break;
            }
            case AF_INET6:
            {
              IN6_ADDR addr = reinterpret_cast<const struct sockaddr_in6*>(pUniAddr->Address.lpSockaddr)->sin6_addr;

              char str[INET6_ADDRSTRLEN];
              inet_ntop(AF_INET6, &addr, str, INET6_ADDRSTRLEN);
              pNetworkInterface->m_cIpAddress = IPAddress(str);
              break;
            }
            default:
            {
              break;
            }
            }
          }

          pAnycast = pCurrAddresses->FirstAnycastAddress;
          unsigned int i = 0;
          if (pAnycast)
          {
            pNetworkInterface->m_bBroadcast = true;

            for (i = 0; pAnycast != 0; i++)
            {
              pAnycast = pAnycast->Next;
            }
          }
          //pNetworkInterface->m_nNumberOfAnycasts = i;

          pMulticast = pCurrAddresses->FirstMulticastAddress;
          i = 0;
          if (pMulticast)
          {
              pNetworkInterface->m_bMulticast = true;

            for (i = 0; pMulticast != 0; i++)
            {
              pMulticast = pMulticast->Next;
            }
          }
          //pNetworkInterface->m_nNumberOfMulticasts = i;

          //pDnsServer = pCurrAddresses->FirstDnsServerAddress;
          //i = 0;
          //if (pDnsServer)
          //{
          //  for (i = 0; pDnsServer != 0; i++)
          //  {
          //    //count dns server
          //    //pNetworkInterface->m_nNumberOfDnsServers++;

          //    SOCKET_ADDRESS sockaddr = pDnsServer->Address;
          //    ADDRESS_FAMILY family = pDnsServer->Address.lpSockaddr->sa_family;

          //    //https://msdn.microsoft.com/de-de/library/windows/desktop/ms740507(v=vs.85).aspx
          //    switch (family)
          //    {
          //    case AF_INET:
          //    {
          //      //IN_ADDR addr = reinterpret_cast<const struct sockaddr_in*>(sockaddr.lpSockaddr)->sin_addr;
          //      IN_ADDR addr = reinterpret_cast<const struct sockaddr_in*>(pDnsServer->Address.lpSockaddr)->sin_addr;

          //      char str[INET_ADDRSTRLEN];
          //      inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
          //      pNetworkInterface->m_vDnsAddresses.push_back(IPAddress(str));
          //      break;
          //    }
          //    case AF_INET6:
          //    {
          //      //IN6_ADDR addr = reinterpret_cast<const struct sockaddr_in6*>(sockaddr.lpSockaddr)->sin6_addr;
          //      IN6_ADDR addr = reinterpret_cast<const struct sockaddr_in6*>(pDnsServer->Address.lpSockaddr)->sin6_addr;

          //      char str[INET6_ADDRSTRLEN];
          //      inet_ntop(AF_INET6, &addr, str, INET6_ADDRSTRLEN);
          //      pNetworkInterface->m_vDnsAddresses.push_back(IPAddress(str));
          //      break;
          //    }
          //    default:
          //    {
          //      break;
          //    }
          //    }
          //    pDnsServer = pDnsServer->Next;
          //  }
          //}

            switch (pCurrAddresses->IfType)
            {
            case IF_TYPE_ETHERNET_CSMACD:
            case IF_TYPE_ISO88025_TOKENRING:
            case IF_TYPE_IEEE80211:
            {
              pNetworkInterface->m_bMulticast = true;
              pNetworkInterface->m_bBroadcast = true;
              break;
            }
            case IF_TYPE_SOFTWARE_LOOPBACK:
            {
              pNetworkInterface->m_bLoopback = true;
              break;
            }
            case IF_TYPE_PPP:
            case IF_TYPE_ATM:
            case IF_TYPE_TUNNEL:
            case IF_TYPE_IEEE1394:
            {
              pNetworkInterface->m_bPointToPoint = true;
              break;
            }
            default:
            {
              break;
            }
            }

            if (!(pCurrAddresses->Flags & IP_ADAPTER_NO_MULTICAST))
            {
              pNetworkInterface->m_bBroadcast = true;
            }


          if (pCurrAddresses->PhysicalAddressLength != 0)
          {
            for (i = 0; i < pCurrAddresses->PhysicalAddressLength; i++)
            {
              pNetworkInterface->m_sMacAddress.push_back(pCurrAddresses->PhysicalAddress[i]);
            }
          }

          pNetworkInterface->m_nMtu = pCurrAddresses->Mtu;
        }

        pCurrAddresses = pCurrAddresses->Next;
      }
    }
    else
    {
      bRet = false;
    }
    FREE(pAddresses);
  }

  return (bRet);
}
#else

//see if_arp.h
void NetworkInterface::setType(unsigned int networktype)
{
  switch (networktype)
  {
    case ARPHRD_ETHER:
    {
    m_eIfType = NetworkInterface::IFTYPE_ETHERNET;
    break;
    }
    case ARPHRD_IEEE802:
    {
    m_eIfType = NetworkInterface::IFTYPE_TOKENRING;
    break;
    }
    case ARPHRD_PPP:
    {
    m_eIfType = NetworkInterface::IFTYPE_PPP;
    break;
    }
    case ARPHRD_FDDI:
    {
      m_eIfType = NetworkInterface::IFTYPE_FDDI;
      break;
    }
    case ARPHRD_SLIP:
    {
      m_eIfType = NetworkInterface::IFTYPE_SLIP;
      break;
    }
    case ARPHRD_LOOPBACK:
    {
    m_eIfType = NetworkInterface::IFTYPE_LOOPBACK;
    break;
    }
    case ARPHRD_TUNNEL:
    case ARPHRD_TUNNEL6:
    {
      m_eIfType = NetworkInterface::IFTYPE_TUNNEL;
      break;
    }
  default:
  {
    m_eIfType = NetworkInterface::IFTYPE_OTHER;
    break;
  }
  }
}

void NetworkInterface::setIfFlags(int &sock, struct ifreq &ifr)
{
  //get flags
  if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
  {
    m_bUp = false;
    m_bLoopback = false;
    m_bMulticast = false;
    m_bPointToPoint = false;
    m_bBroadcast = false;
    m_bRunning = false;
  }
  else
  {
    //check according bit
    m_bUp = (ifr.ifr_flags & IFF_UP) != 0;
    m_bLoopback = (ifr.ifr_flags & IFF_LOOPBACK) != 0;
    m_bMulticast = (ifr.ifr_flags & IFF_MULTICAST) != 0;
    m_bPointToPoint = (ifr.ifr_flags & IFF_POINTOPOINT) != 0;
    m_bBroadcast = (ifr.ifr_flags & IFF_BROADCAST) != 0;
    m_bRunning = (ifr.ifr_flags & IFF_RUNNING) != 0;
  }
}

bool NetworkInterface::create(bool /*ipOnly*/, bool upOnly, NetworkInterface::IPVersion ipVersion, NetworkInterface::IndexMap &indexMap)
{
  //ensure concurrent access
  ScopedLock lock(m_mutex);
  bool bRet = true;

  unsigned nNetworkInterfaceIndex = INVALID_INDEX;
  NetworkInterface networkInterface;
  NetworkInterface::IndexMap::iterator indexMapIterator;

  struct ifaddrs* pInterfaceAddresses = 0;
  struct ifaddrs* pCurrentInterfaceAddress = 0;

  //The getifaddrs() function creates a linked list of structures
  //describing the network interfaces of the local system, and stores the
  //address of the first item of the list in pInterfaceAddresses
  //The data returned by getifaddrs() is dynamically allocated and should
  //be freed using freeifaddrs() when no longer needed
  if (getifaddrs(&pInterfaceAddresses) >= 0)
  {
    //iterate over all interfaces
    for (pCurrentInterfaceAddress = pInterfaceAddresses; pCurrentInterfaceAddress != 0; pCurrentInterfaceAddress = pCurrentInterfaceAddress->ifa_next)
    {
      //get for valid address
      if (pCurrentInterfaceAddress->ifa_addr != 0)
      {
        IPAddress address, subnetMask, broadcastAddress;

        //indicator if the network interface should be added to the indexmap or even not
        bool supportedType = true;

        //get family
        unsigned family = pCurrentInterfaceAddress->ifa_addr->sa_family;

        //get index
        nNetworkInterfaceIndex = if_nametoindex(pCurrentInterfaceAddress->ifa_name);
        indexMapIterator = indexMap.find(nNetworkInterfaceIndex);
        networkInterface = NetworkInterface(nNetworkInterfaceIndex);

        //assign interface name
        networkInterface.m_sInterfaceName = pCurrentInterfaceAddress->ifa_name;
        networkInterface.m_sInterfaceFriendlyName = pCurrentInterfaceAddress->ifa_name;
        networkInterface.m_sDescription = pCurrentInterfaceAddress->ifa_name;

        struct ifreq ifr;
        int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);

        memset(&ifr, 0, sizeof(ifr));
        //std::strncpy(ifr.ifr_name, networkInterface.m_sInterfaceName.c_str(), IFNAMSIZ);

        // fix klockwork
        memcpy(ifr.ifr_name, networkInterface.m_sInterfaceName.c_str(), IFNAMSIZ);

        //getFlags
        networkInterface.setIfFlags(sock, ifr);

        //check parameter for upOnly
        if ((upOnly && networkInterface.isInterfaceUp()) || !upOnly)
        {
          //get mtu size
          if (ioctl(sock, SIOCGIFMTU, &ifr) < 0)
          {
            networkInterface.m_nMtu = 0;
          }
          else
          {
            networkInterface.m_nMtu = ifr.ifr_mtu;
          }
          close(sock);

          switch (family)
          {
          case AF_INET:
          {
            if (ipVersion == IPVERSION_V4 || ipVersion == IPVERSION_V4_V6)
            {
              //ipaddress
              void * tmpAddrPtr = &((struct sockaddr_in *)pCurrentInterfaceAddress->ifa_addr)->sin_addr;
              char addressBuffer[INET_ADDRSTRLEN];
              inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
              networkInterface.m_cIpAddress = IPAddress(addressBuffer);

              //mask
              void *tmpMaskAddrPtr = &((struct sockaddr_in *)pCurrentInterfaceAddress->ifa_netmask)->sin_addr;
              if (pCurrentInterfaceAddress->ifa_netmask != 0)
              {
                inet_ntop(AF_INET, tmpMaskAddrPtr, addressBuffer, sizeof(addressBuffer));
                networkInterface.m_cMaskAddress = IPAddress(addressBuffer);
              }
            }
            else
            {
              supportedType = false;
            }
            break;
          }
          case AF_INET6:
          {
            if (ipVersion == IPVERSION_V6 || ipVersion == IPVERSION_V4_V6)
            {
              //ipaddress
              void * tmpAddrPtr = &((struct sockaddr_in6 *)pCurrentInterfaceAddress->ifa_addr)->sin6_addr;
              char addressBuffer[INET6_ADDRSTRLEN];
              inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
              networkInterface.m_cIpAddress = IPAddress(addressBuffer);

              //mask
              void *tmpMaskAddrPtr = &((struct sockaddr_in6 *)pCurrentInterfaceAddress->ifa_netmask)->sin6_addr;
              if (pCurrentInterfaceAddress->ifa_netmask != 0)
              {
                inet_ntop(AF_INET6, tmpMaskAddrPtr, addressBuffer, sizeof(addressBuffer));
                networkInterface.m_cMaskAddress = IPAddress(addressBuffer);
              }
            }
            else
            {
              supportedType = false;
            }
            break;
          }
          default:
          {
            //we are not interessted on that type so we do NOT want to add it to the indexmap
            supportedType = false;
            break;
          }
          }

          //add interface to map in case it is up and requested
          if ((indexMapIterator == indexMap.end()) && ((upOnly && networkInterface.m_bUp) || !upOnly) && supportedType)
          {
            //collect macaddress
            std::string ifPath("/sys/class/net/");
            ifPath += pCurrentInterfaceAddress->ifa_name;

            std::string addrPath(ifPath);
            addrPath += "/address";

            std::ifstream addrStream(addrPath.c_str());
            if (addrStream.good())
            {
              std::string addr;
              std::getline(addrStream, addr);

              std::vector<std::string> token = StringUtils::tokenize(addr, ":");
              for (auto it = token.begin(); it != token.end(); ++it)
              {
                long value = std::stol((*it), 0, 16);
                networkInterface.m_sMacAddress.push_back(static_cast<unsigned char>(value));
              }
              addrStream.close();
            }

            //get interface type PPP, TOKENRING, ETHERNET, etc.
            std::string typePath(ifPath);
            typePath += "/type";
            std::ifstream typeStream(typePath.c_str());
            if (typeStream.good())
            {
              unsigned int type;
              typeStream >> type;
              networkInterface.setType(type);
              typeStream.close();
            }

            //add it to the indexmap
            indexMapIterator = indexMap.insert(IndexMap::value_type(nNetworkInterfaceIndex, new NetworkInterface(networkInterface))).first;
          }
        }
      }  //if
    }  //for

    //release memory
    if (pInterfaceAddresses != 0)
    {
      freeifaddrs(pInterfaceAddresses);
    }
  }
  else
  {
    bRet = false;
  }

  return (bRet);
}
#endif

#ifdef OS_LINUX_NETINTERFACE_TEST

//To test this execute following:
//uid05xxx@rbv6be0u:~/development/vcc-tcam-3.y-latest-2/package/vcc-tcam/ServiceFramework/src$
//g++ -DOS_LINUX_NETINTERFACE_TEST -DCOMALOG_NO_DLT -D__WORDSIZE=64 -I./../include -std=c++11 ipaddress.cpp networkinterface.cpp atomiccounter.cpp comalog.cpp mutex.cpp mutex_POSIX.cpp stringutils.cpp -lpthread
int main()
{
  NetworkInterface::IndexMap indexMap;
  NetworkInterface::create(true, false, NetworkInterface::IPVERSION_V4_V6, indexMap);

  for (auto it = indexMap.begin(); it != indexMap.end(); it++)
  {
    printf("Index:                       %d\n", (*it).first);
    printf("InterfaceName:               %s\n", (*it).second->getInterfaceName().c_str());
    printf("InterfaceFriendlyName:       %s\n", (*it).second->getInterfaceFriendlyName().c_str());
    printf("Description:                 %s\n", (*it).second->getDescription().c_str());
    printf("Type:                        %s\n", NetworkInterface::IFTypeStr[(*it).second->getType()]);

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

    printf("Interface Type:              %s\n", NetworkInterface::IFTypeStr[(*it).second->getInterfaceType()]);
    printf("MacAddress:                  ");

    NetworkInterface::MacAddress mac = (*it).second->getMacAddress();
    int i = 0;
    for (auto macIt = mac.cbegin(); macIt != mac.cend(); macIt++, i++)
    {
      printf("%2x", (*macIt));
      if (i < mac.size()-1)
      {
        printf(":");
      }
    }
    printf("\n");
    printf("IP Address:                  %s\n", (*it).second->getIPAddress().toString().c_str());
    printf("Mask Address:                %s\n", (*it).second->getMaskAddress().toString().c_str());

    printf("Get MTU:                     %u\n", (*it).second->getMtu());

    printf("\n\n");
  }

  return 0;
}
#endif
