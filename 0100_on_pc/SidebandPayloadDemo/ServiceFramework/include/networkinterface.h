/**
 * @file
 *          networkinterface.h
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

#ifndef NETWORK_INTERFACE_H__
#define NETWORK_INTERFACE_H__


#include <string>
#include <tuple>
#include <map>
#include "ipaddress.h"
#include "mutex.h"
#include "sharedptr.h"
#include "operatingsystem.h"

//https://en.wikipedia.org/wiki/Private_network
//https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing#IPv4_CIDR_blocks
//https://msdn.microsoft.com/de-de/library/windows/desktop/aa365917(v=vs.85).aspx
//http://man7.org/linux/man-pages/man3/getifaddrs.3.html
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa366062(v=vs.85).aspx
//https://en.wikipedia.org/wiki/Hosts_(file)
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx

/*
Example network adapter (windows)
ComboIndex:     11
Adapter Name:   {3D2D56C4-775C-47F0-BF64-FB9D08EA4B31}
Adapter Desc:   Intel(R) Ethernet Connection (2) I219-LM
Adapter Addr:   98-E7-F4-DB-21-B0
Index:  11
Type:   Ethernet
IP Address:     10.215.91.252
IP Mask:        255.255.248.0
Gateway:        10.215.88.1
***
DHCP Enabled: Yes
DHCP Server:  10.215.204.204
Lease Obtained: Mon Jan 29 07:39:28 2018
Lease Expires:  Wed Apr  8 11:26:08 1970
Have Wins: Yes
Primary Wins Server:    10.215.206.25
Secondary Wins Server:  144.145.109.95
*/

/*
Example network adapter (windows)
Calling GetAdaptersAddresses function with family = AF_INET
Length of the IP_ADAPTER_ADDRESS struct: 376
IfIndex (IPv4 interface): 19
Adapter name: {FC66CF71-3FA4-4C27-9F7D-AA5D082C3FE3}
Number of Unicast Addresses: 1
No Anycast Addresses
Number of Multicast Addresses: 1
No DNS Server Addresses
DNS Suffix:
Description: Broadcom NetXtreme Gigabit Ethernet
Friendly name: LAN-Verbindung 4
Physical address: 70-5A-0F-EA-85-FF
Flags: 453
Mtu: 1500
IfType: 6
OperStatus: 2
Ipv6IfIndex (IPv6 interface): 19
ZoneIndices (hex): 13 13 13 13 1 1 1 1 1 1 1 1 1 1 0 1
Transmit link speed: 18446744073709551615
Receive link speed: 18446744073709551615
Number of IP Adapter Prefix entries: 2*/

/*
General information about unicast, broadcast, multicast, mac address

Unicast
Unicast is the term used to describe communication where a piece of information is sent from one point to another point.
In this case there is just one sender, and one receiver.
Unicast transmission, in which a packet is sent from a single source to a specified destination,
is still the predominant form of transmission on LANs and within the Internet.
All LANs (e.g. Ethernet) and IP networks support the unicast transfer mode,
and most users are familiar with the standard
unicast applications (e.g. http, smtp, ftp and telnet) which employ the TCP transport protocol.

Broadcast
Broadcast is the term used to describe communication where a piece of information is sent from one point to all other points.
In this case there is just one sender, but the information is sent to all connected receivers.
Broadcast transmission is supported on most LANs (e.g. Ethernet),
and may be used to send the same message to all computers on the LAN (e.g. the address resolution protocol (arp)
uses this to send an address resolution query to all computers on a LAN).
Network layer protocols (such as IPv4) also support a form of broadcast that allows the same packet to be sent
to every system in a logical network (in IPv4 this consists of the IP network ID and an all 1's host number).

Multicast
Multicast is the term used to describe communication where a piece of information is sent
from one or more points to a set of other points.
In this case there is may be one or more senders, and the information is distributed to a set of receivers
there may be no receivers, or any other number of receivers).
One example
of an application which may use multicast is a video server sending out networked TV channels.
Simultaneous delivery of high quality video to each of a large number of delivery platforms will exhaust
the capability of even a high bandwidth network with a powerful video clip server.
This poses a major salability issue for applications which required sustained high bandwidth.
One way to significantly ease scaling to larger groups of clients is to employ multicast networking.
Multicasting is the networking technique of delivering the same packet simultaneously to a group of clients.
IP multicast provides dynamic many-to-many connectivity between a set of senders (at least 1) and a group of receivers.
The format of IP multicast packets is identical to that of unicast packets and is distinguished only
by the use of a special class of destination address (class D IPv4 address) which denotes a specific multicast group.
Since TCP supports only the unicast mode, multicast applications must use the UDP transport protocol.
Unlike
broadcast transmission (which is used on some local area networks), multicast clients receive a stream of packets only
if they have previously elect to do so (by joining the specific multicast group address).
Membership of a group is dynamic and controlled by the receivers (in turn informed by the local client applications).
The routers in a multicast network learn which sub-networks have active clients for each multicast group and attempt
to minimise the transmission of packets across parts of the network for which there are no active clients.
The multicast mode is useful if a group of clients require a common set of data at the same time,
or when the clients are able to receive and store (cache) common data until needed. Where there is a common need
for the same data required by a group of clients, multicast transmission may provide
significant bandwidth savings (up to 1/N of the bandwidth compared to N separate unicast clients).

The majority of installed LANs (e.g. Ethernet) are able to support the multicast transmission mode.
Shared LANs (using hubs/repeaters) inherently support multicast, since all packets reach
all network interface cards connected to the LAN. The earliest LAN network interface cards had no specific
support for multicast and introduced a big performance penalty by forcing the adaptor to receive all packets (promiscuous mode)
and perform software filtering to remove all unwanted packets. Most modern network interface cards
implement a set of multicast filters, relieving the host of the burden of performing excessive software filtering.

Ethernet MAC Address
The Ethernet network uses two hardware addresses which identify the source and destination of each frame sent by the Ethernet.
The MAC destination address (all 1 s) is used to identify a broadcast packet (sent to all connected computers in a broadcast domain)
or a multicast packet (lsb of 1st byte=1) (received by a selected group of computers).
The hardware address is also known as the Medium Access Control (MAC) address, in reference to the IEEE 802.x series of standards
that define Ethernet. Each computer network interface card is allocated a globally unique 6 byte address
when the factory manufactures the card (stored in a PROM). This is the normal source address used by an interface.
A computer sends all packets which it creates with its own hardware source address, and receives all packets which match its
hardware address or the broadcast address. When configured to use multicast, a selection of multicast hardware addresses may also be received.

see http://www.erg.abdn.ac.uk/users/gorry/course/intro-pages/uni-b-mcast.html
*/


/*
NetworkInterface is to retrieve the current available systems network interfaces like eth0, etc.
The interfaces are collected in a stl map container and a shared pointer tracks the dynamic allocated
memory. It gives also access to the different attributes of those network interfaces like
 - network interface name
 - isUP
 - isRunning
 - isUnicast
 - isMulticast
 - isBroadcast
 - MTU size
 - network type
 etc.

 Usage:
 //fill indexMap with existing network adapters
 NetworkInterface::IndexMap indexMap;
 NetworkInterface::create(true, false, NetworkInterface::IPVERSION_V4_V6, indexMap);

 //iterate over them
 for (auto it = indexMap.begin(); it != indexMap.end(); it++)
 {
 }
*/
class Lib_API NetworkInterface
{
public:
  enum IPVersion
  {
    IPVERSION_V4,
    IPVERSION_V6,
    IPVERSION_V4_V6
  };

  //shared pointer for automatic dynamic memory handling of a NetworkInterface instance
  typedef SharedPtr<NetworkInterface>   Ptr;

  //defintion for a map which takes a networkindex as key and the according NetworkInterface as value
  typedef std::map<unsigned int, Ptr>   IndexMap;

  //definiton for a storage of the macaddress which is a collection of single chars representing the mac address
  typedef std::vector<unsigned char>    MacAddress;

  //definition for an invalid networkindex
  static const unsigned int INVALID_INDEX = ~0;

  /**@brief getFriendlyNameInterface tries to find the according networkinterface
  *
  * @param[in]   name, which define the friendly name of the needed networkinterface
  * @return      sharedpointer of the networkinterface.
  *              Its only valid if the sharedpointer has a valid pointer
  * @note        In linux networkinterface-name and friendlyname are equal.
                 In Windows there is a difference
  * @see also
  */
  static Ptr getFriendlyNameInterface(const std::string& name);

  /**@brief create tries to find the described networkinterfaces and collectes them in the IndexMap
  *
  *
  * @param[in]
  * @see also
  */
  static bool create(bool ipOnly, bool upOnly, IPVersion ipVersion, NetworkInterface::IndexMap &indexMap);

  // enumeration of network interface types
  enum IFType
  {
    IFTYPE_OTHER,
    IFTYPE_ETHERNET,
    IFTYPE_TOKENRING,
    IFTYPE_FDDI,
    IFTYPE_PPP,
    IFTYPE_LOOPBACK,
    IFTYPE_SLIP,
    IFTYPE_TUNNEL
  };

  // get the according string for the enumeration of network interface types
  static const char *IFTypeStr[IFTYPE_TUNNEL +1];

  /**@brief getInterfaceName
  * The method returns the name of the ethernet interface
  *
  * @return      std::string
  * @see also
  */
  const std::string getInterfaceName() const;

  /**@brief getDescription
  *
  * @return      std::string
  * @note in windows a description is returned and in linux the description simply returns the ethernet interface name
  * @see also
  */
  const std::string getDescription() const;

  /**@brief getInterfaceFriendlyName
  * The method returns the friendly name of the ethernet interface
  *
  * @return      std::string
  * @note in windows a friendly name is returned and in linux simply returns the ethernet interface name
  * @see also
  */
  const std::string getInterfaceFriendlyName() const;

  /**@brief check if the network interface is up
  *
  * @return      true if it is up, otherwise false
  * @note the fact that the adapter is up, does not necessarily mean its also connected
  *       or plugged e.g. with an ethernet cable. Check also running for this.
  * @see also
  */
  bool isInterfaceUp() const;

  /**@brief get the index of the network interface
  *
  * @return       the according network interface index
  * @see also
  */
  unsigned int getIndex() const;

  /**@brief get the interface type of the network interface
  *
  * @return       the according network interface index
  * @see also IFTypeStr
  */
  IFType getInterfaceType() const;

  /**@brief get the MAC address (hardware address) of the network interface
  *
  * @return     vector of chars representing the MAC address
  * @see
  */
  MacAddress getMacAddress() const;

  /**@brief is network interface a loopback
  *
  * @return     true if it is the loopback network interface, otherwise false
  * @see
  */
  bool isLoopback() const;

  /**@brief is network interface supporting unicast/point2point
  *
  * @return     true if it does, otherwise false
  * @see
  */
  bool isUnicast() const;

  /**@brief is network interface supporting anycast/broadcast
  *
  * @return     true if it does, otherwise false
  * @see
  */
  bool isAnycast() const;

  /**@brief is network interface supporting multicast
  *
  * @return     true if it does, otherwise false
  * @see
  */
  bool isMulticast() const;

  /**@brief is network interface running
  *
  * This means that the network interface is up and running (e.g. cable connected)
  * @return     true if it does, otherwise false
  * @see
  */
  bool isRunning() const;

  IFType getType() const;

  /**@brief get the maximum transmission unit size
  *
  * @return     unsigned int show the size in bytes
  * @see
  */
  unsigned int getMtu() const;

  /**@brief get the IP Address of the ethernet interface
  *
  * @return     IPAddress, either IPv4 or IPv6
  * @see
  */
  const IPAddress& getIPAddress() const;

  /**@brief get the Mask of the ethernet interface
  *
  * @return     IPAddress, either IPv4 or IPv6
  * @see
  */
  const IPAddress& getMaskAddress() const;

  virtual ~NetworkInterface();

protected:
  //NOT SUPPORTED
  //unsigned int getNumberOfUnicasts() const;
  //unsigned int getNumberOfAnycasts() const;
  //unsigned int getNumberOfMulticasts() const;
  //unsigned int getNumberOfDnsServers() const;
  //unsigned int m_nNumberOfUnicasts;
  //unsigned int m_nNumberOfAnycasts;
  //unsigned int m_nNumberOfMulticasts;
  //unsigned int m_nNumberOfDnsServers;
  //std::vector<IPAddress> m_vDnsAddresses;
  //typedef std::tuple<IPAddress, IPAddress, IPAddress> AddressTypeTuple;
  //typedef std::vector<AddressTypeTuple>               AddressTypeTupleList;
  //AddressTypeTupleList m_lAddressList;
  //const std::vector<IPAddress>& getDnsAddress() const;

protected:
  /**@brief protected constructor
  *
  * Because the network interfaces are operating system resources it makes no sense that
  * clients instantiate a NetworkInterface. Instead a static create method ask the operating system
  * which network interaces exisits and prepare a list of NetworkInterfaces.
  * @return
  * @see  create, getFriendlyNameInterface
  */
  NetworkInterface(unsigned int index = INVALID_INDEX);


  void setType(unsigned int networktype);

#ifdef OS_LINUX
  /**@brief set the active flag word of the device.
  *
  * Get or set the active flag word of the device.  ifr_flags contains a bit mask of the following value:
  *                                   Device flags
  *            IFF_UP            Interface is running.
  *            IFF_BROADCAST     Valid broadcast address set.
  *            IFF_DEBUG         Internal debugging flag.
  *            IFF_LOOPBACK      Interface is a loopback interface.
  *            IFF_POINTOPOINT   Interface is a point-to-point link.
  *            IFF_RUNNING       Resources allocated.
  *            IFF_NOARP         No arp protocol, L2 destination address not set-
  *            IFF_PROMISC       Interface is in promiscuous mode.
  *            IFF_NOTRAILERS    Avoid use of trailers.
  *            IFF_ALLMULTI      Receive all multicast packets.
  *            IFF_MASTER        Master of a load balancing bundle.
  *            IFF_SLAVE         Slave of a load balancing bundle.
  *            IFF_MULTICAST     Supports multicast
  *            IFF_PORTSEL       Is able to select media type via ifmap.
  *            IFF_AUTOMEDIA     Auto media selection active.
  *            IFF_DYNAMIC       The addresses are lost when the interface goes down
  *            IFF_LOWER_UP      Driver signals L1 up (since Linux 2.6.17)
  *            IFF_DORMANT       Driver signals dormant (since Linux 2.6.17)
  *            IFF_ECHO          Echo sent packets (since Linux 2.6.25)
  * @return    void
  * @see    http://man7.org/linux/man-pages/man7/netdevice.7.html
  */

  void setIfFlags(int &sock, struct ifreq &ifr);
#endif

protected:
  //store a description for a network interface if available
  std::string m_sDescription;

  //store a friendly name if available, if not is also the interface name
  std::string m_sInterfaceFriendlyName;

  //store the interface name
  std::string m_sInterfaceName;

  //store the index of that network interface
  unsigned int m_nIndex;

  //store if the network interface is able to broadcast
  bool        m_bBroadcast;

  //store if the network interface is a loobback
  bool        m_bLoopback;

  //store if the network interface is able to multicast
  bool        m_bMulticast;

  //store if the network interface is able to unicast
  bool        m_bPointToPoint;

  //store if the network interface is up
  bool        m_bUp;

  //store if the network interface is running
  bool        m_bRunning;

  //store the network interface maximum transmission unit
  unsigned int m_nMtu;

  //store the network interface type
  IFType      m_eIfType;

  //store the network interface mac address
  MacAddress  m_sMacAddress;

  //store the network interface ip address
  IPAddress   m_cIpAddress;

  //store the network interface mask address
  IPAddress   m_cMaskAddress;

  //use mutex to create the network list to support concurrency
  static Mutex m_mutex;
};

#endif // NETWORK_INTERFACE_H__
