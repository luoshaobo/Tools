/**
* @file
*          ipaddress.cpp
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


#include "ipaddress.h"
#include "stringutils.h"
#include <sstream>
#include <iomanip>


IPAddress::IPAddress():m_eFamily(FAMILY_UNKNOWN)
{
}

IPAddress::IPAddress(Family family): m_eFamily(family)
{
}

IPAddress::IPAddress(const struct sockaddr& sockaddr)
{
  unsigned short family = sockaddr.sa_family;

  switch (family)
  {
  case AF_INET:
  {
    m_eFamily = IPAddress::FAMILY_IPV4;
    std::memcpy(&m_inaddr4, &sockaddr, sizeof(struct sockaddr_in));
    break;
  }
  case AF_INET6:
  {
    m_eFamily = IPAddress::FAMILY_IPV6;
    std::memcpy(&m_inaddr6, &sockaddr, sizeof(struct sockaddr_in6));
    break;
  }
  default:
  {
    m_eFamily = IPAddress::FAMILY_UNKNOWN;
    break;
  }
  }
}

IPAddress::~IPAddress()
{
}

IPAddress::IPAddress(unsigned int prefix, Family family) : m_eFamily(family)
{
  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    if (prefix <= 32)
    {
      //Set all 32 bits to Zero
      m_inaddr4.s_addr = 0;

      //Sets n Bits to '1'
      //n = prefix
      for (int i = 32 - prefix; i < 32; i++)
      {
        m_inaddr4.s_addr |= (1UL << i);
      }

      //convert ulong from host to TCP/IP network byte order
      m_inaddr4.s_addr = htonl(m_inaddr4.s_addr);
    }
    else
    {
      //Error!
      m_eFamily = IPAddress::FAMILY_UNKNOWN;
    }
    break;
  }
  case FAMILY_IPV6:
  {
    if (prefix <= 128)
    {
      unsigned uGroup = 0;
      //check for complete groups
      for (; prefix >= 16; ++uGroup, prefix -= 16)
      {
        m_inaddr6.s6_words[uGroup] = 0xffff;
      }

      //check for last defined group
      if ((prefix > 0)&&(uGroup < 8))
      {
        m_inaddr6.s6_words[uGroup++] = htons(~(0xffff >> prefix));
      }

      //clear the rest
      while (uGroup < 8)
      {
        m_inaddr6.s6_words[uGroup++] = 0;
      }
    }
    else
    {
      //Error!
      m_eFamily = IPAddress::FAMILY_UNKNOWN;
    }
    break;
  }
  default:
  {
    break;
  }
  }

}


IPAddress::IPAddress(const std::string& address) :m_eFamily(FAMILY_UNKNOWN)
{
  IPAddress value;
  if (IPAddress::parse(address, value) == true)
  {
    *this = value;
  }
}

IPAddress::IPAddress(const IPAddress& rhs) : m_eFamily(rhs.m_eFamily),
m_inaddr4(rhs.m_inaddr4),
m_inaddr6(rhs.m_inaddr6)
{

}

IPAddress& IPAddress::operator = (const IPAddress& rhs)
{
  if (this != &rhs)
  {
    m_eFamily = rhs.m_eFamily;
    std::memcpy(&m_inaddr4, &rhs.m_inaddr4, sizeof(m_inaddr4));
    std::memcpy(&m_inaddr6, &rhs.m_inaddr6, sizeof(m_inaddr6));
  }

  return *this;
}

bool IPAddress::parse(const std::string& ipaddress, IPAddress& result)
{
  bool bRet = false;

  OS_IN_ADDR ip_valueV4;
  int nRet = inet_pton(AF_INET, ipaddress.c_str(), &ip_valueV4);

  if (nRet == 1)
  {
    result.m_eFamily = FAMILY_IPV4;
    result.m_inaddr4 = ip_valueV4;
    bRet = true;
  }
  else
  {
    in6_addr ip_valueV6;

    nRet = inet_pton(AF_INET6, ipaddress.c_str(), &ip_valueV6);
    if (nRet == 1)
    {
      result.m_eFamily = FAMILY_IPV6;
      result.m_inaddr6 = ip_valueV6;
      bRet = true;
    }
  }

  return (bRet);
}

IPAddress::Family IPAddress::getFamily() const
{
  return m_eFamily;
}


bool IPAddress::isWildcard() const
{
  bool bRet = false;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    bRet = m_inaddr4.s_addr == INADDR_ANY;
    break;
  }
  case FAMILY_IPV6:
  {
    const unsigned short * words = reinterpret_cast<const unsigned short*>(&m_inaddr6);

    bRet = words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 &&
           words[4] == 0 && words[5] == 0 && words[6] == 0 && words[7] == 0;
    break;
  }
  default:
  {
    break;
  }
  }

  return (bRet);
}

bool IPAddress::isBroadcast() const
{
  bool bRet = false;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    bRet = m_inaddr4.s_addr == INADDR_BROADCAST;
    break;
  }
  case FAMILY_IPV6:
  {
    //not available
    bRet = false;
    break;
  }
  default:
  {
    break;
  }
  }

  return (bRet);
}

bool IPAddress::isLoopback() const
{
  //http://www.omnisecu.com/tcpip/ipv6/ipv6-loopback-address-and-ipv6-unspecified-address.php
  bool bRet = false;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    //In IPv4, we have an entire network (127.0.0.0/8) reserved as loopback addresses.
    //But almost all leading operating systems use the name "localhost" to represent
    //an IPv4 loopback address 127.0.0.1. Rest of the addresses in 127.0.0.0/8 are not widely used.
    // 127.0.0.1 to 127.255.255.255
    bRet = (ntohl(m_inaddr4.s_addr) & 0xFF000000) == 0x7F000000;
    break;
  }
  case FAMILY_IPV6:
  {
    //In IPv6, the IPv6 address reserved for loopback use is 0000:0000:0000:0000:0000:0000:0000:0001/128
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);

    bRet =  words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 &&
      words[4] == 0 && words[5] == 0 && words[6] == 0 && ntohs(words[7]) == 0x0001;
    break;
  }
  default:
  {
    break;
  }
  }

  return (bRet);
}

bool IPAddress::isMulticast() const
{
  //http://ipv6friday.org/blog/2011/12/ipv6-multicast/
  //http://study-ccna.com/ipv6-multicast-addresses/
  bool bRet = false;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    bRet = (ntohl(m_inaddr4.s_addr) & 0xF0000000) == 0xE0000000; // 224.0.0.0/24 to 239.0.0.0/24
    break;
  }
  case FAMILY_IPV6:
  {
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);
    bRet = (ntohs(words[0]) & 0xFFE0) == 0xFF00;
    break;
  }
  default:
  {
    break;
  }
  }

  return (bRet);
}

bool IPAddress::isUnicast() const
{
  bool bRet = !isWildcard() && !isBroadcast() && !isMulticast();

  return (bRet);
}

bool  IPAddress::isLinkLocal() const
{
  bool bRet = false;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    //In RFC 3927, the Internet Engineering Task Force has reserved the address block 169.254.0.0 / 16.
    //From this block, the address range 169.254.1.0 to 169.254.254.255 may be used for
    //link - local addressing in Internet Protocol Version 4. The first 256 addresses(169.254.0.0 / 24)
    //and last 256 addresses(169.254.255.0 / 24) in the 169.254.0.0 / 16 prefix are reserved for
    //future use and must not be selected by a host using this dynamic configuration mechanism[4].
    //Link - local addresses are assigned to interfaces by host - internal, i.e.stateless, address
    //autoconfiguration when other means of address assignment are not available.
    //169.254.0.0/16
    bRet = (ntohl(m_inaddr4.s_addr) & 0xFFFF0000) == 0xA9FE0000;
    break;
  }
  case FAMILY_IPV6:
  {
    //In the Internet Protocol Version 6 (IPv6), the address block fe80:: / 10
    //has been reserved for link - local unicast addressing.[1] Of the 64 bits of a link - local addresses'
    //network component, the most significant 10 bits (1111111010) correspond to the IANA-reserved
    //"global routing prefix" for link-local addresses, while the "subnet ID" (the remaining 54 bits) is zero.[8]
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);
    bRet = (ntohs(words[0]) & 0xFFE0) == 0xFE80;
    break;
  }
  default:
  {
    break;
  }
  }

  return (bRet);
}

bool  IPAddress::isSiteLocal() const
{
  bool bRet = false;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    unsigned long addr = ntohl(m_inaddr4.s_addr);
    bRet =  (addr & 0xFF000000) == 0x0A000000 ||     // 10.0.0.0/24
      (addr & 0xFFFF0000) == 0xC0A80000 ||           // 192.68.0.0/16
      (addr >= 0xAC100000 && addr <= 0xAC1FFFFF);    // 172.16.0.0 to 172.31.255.255
    break;
  }
  case FAMILY_IPV6:
  {
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);
    bRet = ((ntohs(words[0]) & 0xFFE0) == 0xFEC0) || ((ntohs(words[0]) & 0xFF00) == 0xFC00);
    break;
  }
  default:
  {
    break;
  }
  }

  return (bRet);
}

IPAddress IPAddress::operator & (const IPAddress& rhs) const
{
  IPAddress ret(rhs);

  if (m_eFamily == FAMILY_IPV4)
  {
    ret.m_inaddr4.s_addr &= rhs.m_inaddr4.s_addr;
  }
  else
  {
    ret.m_inaddr6.s6_words[0] &= rhs.m_inaddr6.s6_words[0];
    ret.m_inaddr6.s6_words[1] &= rhs.m_inaddr6.s6_words[1];
    ret.m_inaddr6.s6_words[2] &= rhs.m_inaddr6.s6_words[2];
    ret.m_inaddr6.s6_words[3] &= rhs.m_inaddr6.s6_words[3];
    ret.m_inaddr6.s6_words[4] &= rhs.m_inaddr6.s6_words[4];
    ret.m_inaddr6.s6_words[5] &= rhs.m_inaddr6.s6_words[5];
    ret.m_inaddr6.s6_words[6] &= rhs.m_inaddr6.s6_words[6];
    ret.m_inaddr6.s6_words[7] &= rhs.m_inaddr6.s6_words[7];
  }
  return ret;
}

IPAddress IPAddress::operator | (const IPAddress& rhs) const
{
  IPAddress ret(rhs);

  if (m_eFamily == FAMILY_IPV4)
  {
    ret.m_inaddr4.s_addr |= rhs.m_inaddr4.s_addr;
  }
  else
  {
    ret.m_inaddr6.s6_words[0] |= rhs.m_inaddr6.s6_words[0];
    ret.m_inaddr6.s6_words[1] |= rhs.m_inaddr6.s6_words[1];
    ret.m_inaddr6.s6_words[2] |= rhs.m_inaddr6.s6_words[2];
    ret.m_inaddr6.s6_words[3] |= rhs.m_inaddr6.s6_words[3];
    ret.m_inaddr6.s6_words[4] |= rhs.m_inaddr6.s6_words[4];
    ret.m_inaddr6.s6_words[5] |= rhs.m_inaddr6.s6_words[5];
    ret.m_inaddr6.s6_words[6] |= rhs.m_inaddr6.s6_words[6];
    ret.m_inaddr6.s6_words[7] |= rhs.m_inaddr6.s6_words[7];
  }
  return ret;
}

IPAddress IPAddress::operator ^ (const IPAddress& rhs) const
{
  IPAddress ret(rhs);

  if (m_eFamily == FAMILY_IPV4)
  {
    ret.m_inaddr4.s_addr ^= rhs.m_inaddr4.s_addr;
  }
  else
  {
    ret.m_inaddr6.s6_words[0] ^= rhs.m_inaddr6.s6_words[0];
    ret.m_inaddr6.s6_words[1] ^= rhs.m_inaddr6.s6_words[1];
    ret.m_inaddr6.s6_words[2] ^= rhs.m_inaddr6.s6_words[2];
    ret.m_inaddr6.s6_words[3] ^= rhs.m_inaddr6.s6_words[3];
    ret.m_inaddr6.s6_words[4] ^= rhs.m_inaddr6.s6_words[4];
    ret.m_inaddr6.s6_words[5] ^= rhs.m_inaddr6.s6_words[5];
    ret.m_inaddr6.s6_words[6] ^= rhs.m_inaddr6.s6_words[6];
    ret.m_inaddr6.s6_words[7] ^= rhs.m_inaddr6.s6_words[7];
  }
  return ret;
}

IPAddress IPAddress::operator ~ () const
{
  IPAddress ret(*this);

  if (m_eFamily == FAMILY_IPV4)
  {
    ret.m_inaddr4.s_addr ^= 0xffffffff;
  }
  else
  {
    ret.m_inaddr6.s6_words[0] ^= 0xffff;
    ret.m_inaddr6.s6_words[1] ^= 0xffff;
    ret.m_inaddr6.s6_words[2] ^= 0xffff;
    ret.m_inaddr6.s6_words[3] ^= 0xffff;
    ret.m_inaddr6.s6_words[4] ^= 0xffff;
    ret.m_inaddr6.s6_words[5] ^= 0xffff;
    ret.m_inaddr6.s6_words[6] ^= 0xffff;
    ret.m_inaddr6.s6_words[7] ^= 0xffff;
  }
  return ret;
}

bool IPAddress::operator != (const IPAddress& rhs) const
{
  return !(*this == rhs);
}

bool IPAddress::operator == (const IPAddress& rhs) const
{
  bool bRet = false;

  if (m_eFamily == rhs.m_eFamily)
  {
    if (m_eFamily == FAMILY_IPV4)
    {
      bRet = m_inaddr4.s_addr == rhs.m_inaddr4.s_addr;
    }
    else
    {
      bRet = true;
      bRet = bRet && m_inaddr6.s6_words[0] == rhs.m_inaddr6.s6_words[0];
      bRet = bRet && m_inaddr6.s6_words[1] == rhs.m_inaddr6.s6_words[1];
      bRet = bRet && m_inaddr6.s6_words[2] == rhs.m_inaddr6.s6_words[2];
      bRet = bRet && m_inaddr6.s6_words[3] == rhs.m_inaddr6.s6_words[3];
      bRet = bRet && m_inaddr6.s6_words[4] == rhs.m_inaddr6.s6_words[4];
      bRet = bRet && m_inaddr6.s6_words[5] == rhs.m_inaddr6.s6_words[5];
      bRet = bRet && m_inaddr6.s6_words[6] == rhs.m_inaddr6.s6_words[6];
      bRet = bRet && m_inaddr6.s6_words[7] == rhs.m_inaddr6.s6_words[7];
    }
  }
  else
  {
    bRet = false;
  }

  return (bRet);
}

bool IPAddress::isIPv4Compatible() const
{
  ////https://elinux.org/images/c/c6/Adapting_Your_Network_Code_For_IPv6_Support.pdf

  bool bRet = true;
  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    bRet = true;
    break;
  }
  case FAMILY_IPV6:
  {
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);
    bRet =  words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && words[4] == 0 && words[5] == 0;
    break;
  }
  default:
  {
    bRet = false;
    break;
  }
  }

  return (bRet);
}

bool IPAddress::isIPv4Mapped() const
{
  //https://elinux.org/images/c/c6/Adapting_Your_Network_Code_For_IPv6_Support.pdf
  bool bRet = true;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    bRet = true;
    break;
  }
  case FAMILY_IPV6:
  {
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);
    return words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && words[4] == 0 && words[5] == 0xFFFF;
    break;
  }
  default:
  {
    bRet = false;
    break;
  }
  }

  return (bRet);
}

std::string IPAddress::toString() const
{
  std::string result;

  switch (m_eFamily)
  {
  case FAMILY_IPV4:
  {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&m_inaddr4);
    result.reserve(16);

    std::stringstream ss;

    ss << (int)bytes[0] << "." << (int)bytes[1] << "." << (int)bytes[2] << "." << (int)bytes[3];
    result = ss.str();

    break;
  }
  case FAMILY_IPV6:
  {
    const unsigned short* words = reinterpret_cast<const unsigned short*>(&m_inaddr6);
    result.reserve(64);

    int position = -1, number = 0;
    //It will be common for addresses to contain long strings of zero bits.
    //In order to make writing addresses containing zero bits easier, a special syntax is available to compress the zeros.
    //The use of "::" indicates one or more groups of 16 bits of zeros. The "::" can only appear once in an address.
    //Example: FF01:0:0:0:0:0:0:101 may be represented as FF01::101
    //
    //The Method searches the postion and the count of the zeros and replaces them later with ::
    //The Value -1 at position indicates no zeros in the address
    //https://tools.ietf.org/html/rfc4291#section-2.2

    //Searches the position and the number of most consecutive zeros
    for (int i = 0; i < 8; i++)
    {
      if (words[i] == 0)
      {
        if (position + number == i)
        {
          number++;
        }
        else {
          position = i;
          number = 1;
        }
      }
    }

    //puts the string together and replaces the zeroes with ::
    std::stringstream ss;

    for (int i = 0; i < 8; i++)
    {
      if (position == i)
      {
        i += number - 1;
        ss << "::";
      }
      else
      {
        ss << std::hex << ntohs(words[i]);

        if (i < 7 && position != i + 1)
        {
          ss << ":";
        }
      }
    }

    result = ss.str();
    break;
  }
  default:
  {
    break;
  }
  }

  return result;
}


bool IPAddress::findIpAddress(const std::string& url, IPAddress& result)
{
  bool bFound = false;

  //Split the Url into parts whenever a "/", "%", "[" or "]" appears
  std::vector<std::string> token = StringUtils::tokenize(url, "/[]%");

  //runs for every part or until an valid Ip address has been found
  for (size_t i = 0; (i < token.size()) && (bFound == false); i++)
  {
    //check if there is an ipv4 with port probably like "192.168.1.113:8080"
    if (token[i].find(".") != std::string::npos && token[i].find(":") != std::string::npos)
    {
      //separate ip and port
      std::vector<std::string> split = StringUtils::tokenize(token[i], ":");

      //insert after end that this gets also checked
      token.insert(token.end(), split.begin(), split.end());
    }

    //Tests if it is empty -> no Ip
    //Most times its an empty string -> check for empty so that IPAddress::parse does not get called so often
    //
    //IPAress::parse checks, if it is an Valid IPV4 or IPV6
    if (token[i].empty() == false && IPAddress::parse(token[i], result) == true)
    {
      //bRet = true; stops the for() and the method returns true
      bFound = true;
    }
  }

  return bFound;
}
