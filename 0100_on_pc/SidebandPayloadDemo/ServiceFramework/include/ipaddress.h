/**
* @file
*          ipaddress.h
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


#ifndef IP_ADDRESS_H__
#define IP_ADDRESS_H__

#include <string>
#include <vector>
#include "operatingsystem.h"

#ifdef OS_WINDOWS
#include <Ws2tcpip.h>
#else
#include <cstring>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#endif

//http://www.gestioip.net/docu/ipv6_address_examples.html
/*
 IPAddress is responsible to store and handle an internet IP address.
 It supports the IPv4 and IPv6 versions. The class also implements
 binary operators and query methods that answers which kind of IP address it is.

 Usage:
 Examples of IPAddresses:
 IPAddress ipv4("1.2.3.4");  //initialise an IPv4 address
 IPAddress ipv6("2001:0db8:0001:0000:0000:0ab9:C0A8:0102"); //initialise an IPv6 address
 IPAddress ipv6_2("2001:db8:1::ab9:C0A8:102"); //initialise the same IPv6 address like above (short version of IPv6)

 Examples of IPMasks:
 IPAddress maskipv4_32(32, IPAddress::FAMILY_IPV4); //initialise mask 255.255.255.255
 IPAddress maskipv4_24(24, IPAddress::FAMILY_IPV4); //initialise mask 255.255.255.0
 IPAddress maskipv4_16(16, IPAddress::FAMILY_IPV4); //initialise mask 255.255.0.0
 IPAddress maskipv4_8(8, IPAddress::FAMILY_IPV4); //initialise mask 255.0.0.0

 IPAddress maskipv6(86, IPAddress::FAMILY_IPV6); //initialise mask ffff:ffff:ffff:ffff:ffff:fc00::

 Examples of init with parse:
 IPAddress result;
 bool b;
 b= IPAddress::parse("192.168.1.113", result);
 b= IPAddress::parse("fc00::7", result);

 Example of findIpAddress
 IPAddress result;
 std::string url = "https://192.168.1.112/?clientid=CONTI0006";
 bool b = IPAddress::findIpAddress(url, result);  //result contains 192.168.1.112

 url = "https://192.168.1.113:8080/?clientid=CONTI0006";
 bool b = IPAddress::findIpAddress(url, result);  //result contains 192.168.1.113

 url = "http://[2001:db8:a0b:12f0::1]/index.html";
 bool b = IPAddress::findIpAddress(url, result);  //result contains 2001:db8:a0b:12f0::1

 url = "http://[2001:db8:a0b:12f0::1]:80/index.html";
 bool b = IPAddress::findIpAddress(url, result);  //result contains 2001:db8:a0b:12f0::1

 url = "[2001:db8:a0b:12f0::1%eth0]:80";
 bool b = IPAddress::findIpAddress(url, result);  //result contains 2001:db8:a0b:12f0::1
*/
class Lib_API IPAddress
{
public:
  //scoped typedef for clients when they need to store different IPs
  typedef std::vector<IPAddress> List;

  //enumerate the supported formats
  enum Family
  {
    FAMILY_UNKNOWN,
    FAMILY_IPV4,
    FAMILY_IPV6
  };

  /**@brief construct an empty address
  *
  */
  IPAddress();

  /**@brief construct the IPAddress out of a sockadd
  *
  */
  IPAddress(const struct sockaddr& sockaddr);

  /**@brief construct a mask either for IPv4 or IPv6
  *
  */
  IPAddress(unsigned int prefix, Family family);

  /**@brief copy constructor
  *
  */
  IPAddress(const IPAddress& rhs);

  /**@brief create an empty IPAddress either for IPv4 or IPv6
  *
  */
  explicit IPAddress(Family family);

  /**@brief creates automatically either an IPv4 or IPv6 address by parsing the address
  *
  */
  explicit IPAddress(const std::string& address);

  virtual ~IPAddress();

  /**@brief assign operator
  *
  */
  IPAddress& operator = (const IPAddress& rhs);

  /**@brief get the current family-type
  *
  */
  Family getFamily() const;

  /**@brief get a string representing the address
  *
  */
  std::string toString() const;

  /**@brief check if address is wildcard
  *
  * @see also:
  *  https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing#IPv4_CIDR_blocks
  *  http://www.mathematik.uni-ulm.de/help/perl5/doc/NetAddr/IP.html
  *  http://www.retran.com/beej/ipstructsdata.html
  */
  bool isWildcard() const;

  /**@brief all bits one, no support for ipv6
  * @see also:
  * http://ipv6friday.org/blog/2011/12/ipv6-multicast/
  */
  bool isBroadcast() const;

  /**@brief check if address is loopback
  *
  * @note: IPv4, the loopback address is 127.0.0.1
  *        IPv6, the loopback address is ::1
  * @see also: http://www.omnisecu.com/tcpip/ipv6/ipv6-loopback-address-and-ipv6-unspecified-address.php
  */
  bool isLoopback() const;

  /**@brief assign operator
  *
  * @see also: https://en.wikipedia.org/wiki/Multicast_address
  */
  bool isMulticast() const;

  /**@brief check if not a wildcard broadcast or multicast address.
  *
  */
  bool isUnicast() const;

  /**@brief check if address is locally
  * IPv4 link local addresses are in the 169.254.0.0/16 range
  * and IPv6 starts with those 10 bits 1111 1110 10
  *
  * @see also: https://en.wikipedia.org/wiki/Link-local_address
  */
  bool isLinkLocal() const;

  /**@brief check if address is site local
  * site-local addresses are equivalent to private IP addresses in IPv4.
  * The address space reserved for these addresses, which are only routed within
  * an organization and not on the public Internet, is 10.0.0.0/8, 172.16.0.0/12, and 192.168.0.0/16.
  * In IPv6, the first 10 bits of a site-local address are set to 1111111011,
  * which is why these addresses always begin with FEC0. The following 54 bits are the subnet ID,
  * which you can use in your organization for hierarchical routing, and the last 64 bits are the interface ID,
  * which is the part that has to be unique on a link (local network on which hosts communicate
  * without intervening routers). Thus, the prefix of a site-local address is FEC0::/10
  */
  bool isSiteLocal() const;

  /**@brief check if it is a IPv4
  * Return true for IPv4 addresses. For IPv6 the first 96 bits must be zero
  */
  bool isIPv4Compatible() const;

  /**@brief check if it is mapped
  *
  * Return true if the address is an IPv4 mapped IPv6 address.
  * For IPv6 the address must be  ::FFFF:x:x range.
  */
  bool isIPv4Mapped() const;

  /**@brief compare operator
  *
  */
  bool operator == (const IPAddress& rhs) const;
  bool operator != (const IPAddress& rhs) const;

  /**@brief binary operators
  *
  */
  IPAddress operator & (const IPAddress& rhs) const;
  IPAddress operator | (const IPAddress& rhs) const;
  IPAddress operator ^ (const IPAddress& rhs) const;
  IPAddress operator ~ () const;

  /**@brief parse reads either a IPv4 or and IPv6 address and prepare the result-IPAddress accordingly
  *
  */
  static bool parse(const std::string& addr, IPAddress& result);

  /**@brief Searches an valid IPV4 or IPV6 in an url and prepare the result-IPAddress accordingly
  *
  */
  static bool findIpAddress(const std::string& url, IPAddress& result);

protected:
  // store the family type
  Family m_eFamily;

  // store the in_addr either for ipv4 or ipv6 depending on what family
  // Note that this structures are defined in network big endianess format
  OS_IN_ADDR m_inaddr4;
  in6_addr m_inaddr6;
};


#endif // IP_ADDRESS_H__
