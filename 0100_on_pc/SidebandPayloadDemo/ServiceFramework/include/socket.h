/**
* @file
*          socket.h
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

#ifndef SOCKET_H__
#define SOCKET_H__

#include <string>            // For string
#include <exception>         // For exception class
#include <vector>
#include "operatingsystem.h"
#include "sharedptr.h"
#include "timeelapse.h"
#include "sharedptr.h"
#include "binaryringbuffer.h"

#ifdef OS_WINDOWS
// socket(), connect(), send(), and recv()
#include <winsock.h>
#undef min
#else
// socket(), connect(), send(), and recv()
#include <sys/socket.h>
#endif

/**
*   Signals a problem with the execution of a socket call.
*   http://www.cplusplus.com/doc/tutorial/exceptions/
*/
class Lib_API SocketException : public std::exception
{
public:
  /**
  *SocketException will be thrown by the different socket functions.
  *Because all methods which uses socket functionality includes a potential risk to raise errors,
  *those socket classes works with exceptions, because this makes it much more easy to work with.
  *Not every return code must be evaluated, but instead, a try-catch block encapsulates several
  *methods which also makes code much more clear.
  *
  *@param message, contains the error message
  *@param useErrno, tells if we also want to inlcude the system error code errno
  *
  */
  SocketException( const std::string &message, bool useErrno = false ) throw();

  ~SocketException() throw();

  /**
  *   get the exception message
  *   @see also http://www.cplusplus.com/doc/tutorial/exceptions/
  *   @return exception message
  */
  const char *what() const throw();

private:
  // string for containing exception message
  std::string m_sExceptionMessage;
};

class Lib_API  SocketResource //: public ReferenceCounter
{
public:
  SocketResource(os_socket_t fd, int type, short family, int protocol);
  virtual ~SocketResource();

  os_socket_t getSockFD() const;
  void        setSockFD(os_socket_t fd);
  short       getSockFamily() const;
  int         getSockType() const;
  int         getProtocol() const;

protected:
  SocketResource();

protected:
  // Socket descriptor
  os_socket_t m_socketFileDescriptor;
  int m_type;
  short m_socketFamily;
  int m_protocol;

};

inline SocketResource::SocketResource(os_socket_t fd, int type, short family, int protocol) : m_socketFileDescriptor(fd),
                                                                                m_type(type),
                                                                                m_socketFamily(family),
                                                                                m_protocol(protocol)
{
}

inline os_socket_t SocketResource::getSockFD() const
{
  return (m_socketFileDescriptor);
}

inline void  SocketResource::setSockFD(os_socket_t fd)
{
  m_socketFileDescriptor = fd;
}

inline short SocketResource::getSockFamily() const
{
  return (m_socketFamily);
}

inline int SocketResource::getSockType() const
{
  return (m_type);
}

inline int SocketResource::getProtocol() const
{
  return (m_protocol);
}

inline SocketResource::~SocketResource()
{
  if (m_socketFileDescriptor != OS_INVALID_SOCKET)
  {
    os_closesocket(m_socketFileDescriptor);
    m_socketFileDescriptor = OS_INVALID_SOCKET;
  }
}


/**
*   Wrapper for common socket functionality
*/
class Lib_API Socket
{
public:

  /**
  *   Create the socket like it was inited by the constructor values.
  *   @return bool signals if the socket is valid or not.
  *   @exception SocketException thrown if create fails
  *   @note if socket already exisits, then createSocket does nothing except return true
  *   @see setReuseAddress
  */
  bool createSocket() throw(SocketException);

  /**
  *   Activate the SO_REUSEADDR when creating the socket.
  *   https://hea-www.harvard.edu/~fine/Tech/addrinuse.html
  *   @note if socket already exisits, then createSocket does nothing except return true
  *   @see setReuseAddress
  */
  void setReuseAddress(bool b);

  /**
  *   Get the local address
  *   @return local address of socket
  *   @exception SocketException thrown if fetch fails
  */
  std::string getLocalAddress() const throw( SocketException );

  /**
  *   Get the local port
  *   @return local port of socket
  *   @exception SocketException thrown if fetch fails
  */
  unsigned short getLocalPort() const throw( SocketException );

  /**
  *   Set the local port to the specified port and the local address
  *   to any interface
  *   @param localPort local port
  *   @exception SocketException thrown if setting local port fails
  */
  void setLocalPort( unsigned short localPort ) throw( SocketException );



  /**
  *   Set the local port to the specified port and the local address
  *   to the specified address.  If you omit the port, a random port
  *   will be selected.
  *   @param localAddress local address
  *   @param localPort local port
  *   @exception SocketException thrown if setting local port or address fails
  */
  void setLocalAddressAndPort( const std::string &localAddress, unsigned short localPort = 0 ) throw( SocketException );

  static int lastError();

  /**
  *   If WinSock, unload the WinSock DLLs; otherwise do nothing.  We ignore
  *   this in our sample client code but include it in the library for
  *   completeness.  If you are running on Windows and you are concerned
  *   about DLL resource consumption, call this after you are done with all
  *   Socket instances.  If you execute this on Windows while some instance of
  *   Socket exists, you are toast.  For portability of client code, this is
  *   an empty function on non-Windows platforms so you can always include it.
  *   @param buffer buffer to receive the data
  *   @param bufferLen maximum number of bytes to read into buffer
  *   @return number of bytes read, 0 for EOF, and -1 for error
  *   @exception SocketException thrown WinSock clean up fails
  */
  static void cleanUp() throw( SocketException );

  /**
  *   Resolve the specified service for the specified protocol to the
  *   corresponding port number in host byte order
  *   @param service service to resolve (e.g., "http")
  *   @param protocol protocol of service to resolve.  Default is "tcp".
  */
  static unsigned short resolveService( const std::string &service, const std::string &protocol = "tcp" );

  os_socket_t getSockFD() const;

private:
  //do not implement

protected:
  /**
  *   Constructors for creating a socket.
  *   NOTE: Because this is a baseclass only derivations should be able for instantiations, therefore they are protected here
  */
  Socket(short socketFamily, int type, int protocol);
  Socket(short socketFamily, int type, int protocol, os_socket_t fd);
  Socket(SocketResource &socket);
  //Socket(short socketFamily, int socketFileDescriptor);

  //copy
  Socket(const Socket &rhs);
  Socket& operator=(const Socket &rhs);
  virtual ~Socket();



protected:
  SharedPtr<SocketResource> m_socket;
  bool m_bReuseAddress;
};

inline int Socket::lastError()
{
#if defined(OS_WINDOWS)
  return WSAGetLastError();
#else
  return errno;
#endif
}


/**
*   Socket which is able to connect, send, and receive
*/
class Lib_API CommunicatingSocket : public Socket
{
public:
  typedef SharedPtr<CommunicatingSocket> Ptr;
  typedef std::vector<Ptr> SocketList;

  void close();

  /**
  *   Establish a socket connection with the given foreign
  *   address and port
  *   @param foreignAddress foreign address (IP address or name)
  *   @param foreignPort foreign port
  *   @exception SocketException thrown if unable to establish connection
  */
  void connect( const std::string &foreignAddress, unsigned short foreignPort ) throw( SocketException );

  /**
  *   Write the given buffer to this socket.  Call connect() before
  *   calling send()
  *   @param buffer buffer to be written
  *   @param bufferLen number of bytes from buffer to be written
  *   @exception SocketException thrown if unable to send data
  */
  int send( const void *buffer, int bufferLen ) throw( SocketException );


  unsigned long recvBytesAvailable() const;

  /**
  *   Read into the given buffer up to bufferLen bytes data from this
  *   socket.  Call connect() before calling recv()
  *   @param buffer buffer to receive the data
  *   @param bufferLen maximum number of bytes to read into buffer
  *   @return number of bytes read, 0 for EOF, and -1 for error
  *   @exception SocketException thrown if unable to receive data
  */
  int recv( void *buffer, int bufferLen ) throw( SocketException );

  /**
  *   Get the foreign address.  Call connect() before calling recv()
  *   @return foreign address
  *   @exception SocketException thrown if unable to fetch foreign address
  */
  std::string getForeignAddress() const throw( SocketException );

  /**
  *   Get the foreign port.  Call connect() before calling recv()
  *   @return foreign port
  *   @exception SocketException thrown if unable to fetch foreign port
  */
  unsigned short getForeignPort() const throw( SocketException );

  bool setBlockingMode(bool blocking);


  static int select(SocketList& readList, SocketList& writeList, SocketList& exceptList, const TimeElapse& timeout) throw( SocketException );


protected:
  CommunicatingSocket(short socketfamily, int type, int protocol);
  CommunicatingSocket(short socketfamily, int type, int protocol, os_socket_t fd);
  //CommunicatingSocket(short socketFamily, int newConnectionFileDescriptor);
};

/**
 * Extended CommunicatingSocket supporting a basic messaging protocol
 * (length + variable message container)
 */
class Lib_API MessagingSocket : public CommunicatingSocket
{

private:
  enum MessageState
  {
    CLEAN,            //< fresh start or after fully parsed message
    PARTIAL_HEADER,   //< only a partial header has been received
    FULL_HEADER,      //< collecting the header has finished
    PARTIAL_PAYLOAD,  //< a full header and partial payload has been received
    PARTIAL_LARGE_PAYLOAD  //< a full header and partial large payload (needing dynamic memory) has been received
  };

  /**
   * Maximum allowed message size.
   */
  static const unsigned int MAX_MESSAGE_SIZE = 10 * 1024 * 1024;

  /**
   * This buffer must be able to store at least the most, not yet completely finished
   * messages (== max_size - 1), except for the rather rare large ones (http responses etc.).
   * The large ones will be dealt with with a separate dynamically allocated buffer.
   */
  static const unsigned int RINGBUF_SIZE = 10000;
//  static const unsigned int RINGBUF_SIZE = 50; //for testing large messages
  BinaryRingBuffer m_rawbuf{RINGBUF_SIZE};

  /**
   * Size of the temporary buffer per recv() call. Make sure this is <= RINGBUF_SIZE.
   */
  static const unsigned int RECVBUF_SIZE = RINGBUF_SIZE / 2;

  /**
   * Pointer to a dynamically allocated byte/char buffer to deal with special extra-large messages.
   */
  char* m_messagebuf_dynamic = nullptr;

  /**
   * Value of the fully parsed length header.
   */
  uint32_t m_expected_message_length = 0;

  /**
   * Buffer for collecting the header bytes.
   */
  char m_headerbuf[sizeof(uint32_t)];
  uint32_t m_collected_header_length = 0;

  /**
   * Buffer for collecting a variable-length payload/message.
   */
  char m_messagebuf[RINGBUF_SIZE];
  uint32_t m_collected_message_length = 0;
  /**
   * Buffer with complete message to return to message recipient and protobuf parser
   */
  std::string m_messagebuf_export;

  MessageState m_state = CLEAN;

  /**
   * Reset buffers and states for header/message parsing.
   */
  void resetSession();

  /**
   * Clean up dynamically allocated buffers. Beware that this method does not make sure that
   * the memory has really been allocated properly before.
   */
  void cleanupDynamicBuffers();

  void printAsHex(const char* prefix, char* buf, unsigned int length);
  void printState(const char* prefix);


public:
  typedef SharedPtr<MessagingSocket> Ptr;

  /**
   *   Reads all available complete messages (i.e. length indicator and subsequent full
   *   message/payload container) and puts them in a given vector. Partial messages will be
   *   tracked and used in combination with fresh incoming recv data.
   *
   *   Call connect() before using this method.
   *
   *   @param[out] messages vector with byte strings returning fully retrieved messages.
   *
   *   @return number of messages read, 0 if nothing or only a partial message was received
   *   @exception SocketException thrown if unable to receive data
   */
  //int receiveMessages(std::vector<SharedPtr<MessageContainer> >& messages) throw (SocketException);
  int receiveMessages(std::vector<std::string>& messages) throw (SocketException); //generic interface

  /**
   * Sends a serialized message over the socket. It will first send a 32bit fixed length indicator,
   * followed the message/payload itself.
   *
   * @param[in] serialized message, a byte string
   *
   * @return number of sent bytes
   */
  int sendMessage(const std::string& message);

protected:
  MessagingSocket(short socketfamily, int type, int protocol);
  MessagingSocket(short socketfamily, int type, int protocol, os_socket_t fd);
};


/**
*   TCP socket for communication with other TCP sockets
*  //TODO: TCPSocket should NOT derive from MessagingSocket!!!!!! because TCPSocket should not have a protocol inside
*  // like MessagingSocket implemenmts!! CLEAN UP in order that socket.h remains reusable for others
*/
class Lib_API TCPSocket : public MessagingSocket
{
public:
  typedef SharedPtr<TCPSocket> Ptr;
  /**
  *   Construct a TCP socket with no connection
  *   Use createSocket to finally create the socket
  */
  TCPSocket(short socketfamily);

  /**
  *   Construct a TCP socket with a connection to the given foreign address
  *   and port
  *   Use createSocket to finally create the socket
  *   @param foreignAddress foreign address (IP address or name)
  *   @param foreignPort foreign port
  */
  TCPSocket(short socketfamily, const std::string &foreignAddress, unsigned short foreignPort);

  /**
  *   Establish a socket connection with the given foreign
  *   address and port
  *   @exception SocketException thrown if unable to establish connection
  */
  void connect() throw(SocketException);


private:
  // Access for TCPServerSocket::accept() connection creation
  friend class TCPServerSocket;
  TCPSocket(short socketFamily, int type, int protocol, os_socket_t fd);
  //TCPSocket(short socketFamily, int type, os_socket_t newConnectionFileDescriptor);

  short m_sSocketFamily;
  std::string m_sForeignAddress;
  unsigned short m_usForeignPort;
};

/**
*   TCP socket class for servers
*/
class Lib_API TCPServerSocket : public Socket
{
public:
  typedef SharedPtr<TCPServerSocket> Ptr;

  /**
  *   Construct a TCP socket for use with a server, accepting connections
  *   on the specified port on any interface
  *   @param localPort local port of server socket, a value of zero will
  *                   give a system-assigned unused port
  *   @param queueLen maximum queue length for outstanding
  *                   connection requests (default 5)
  *   Use createSocket to finally create the socket
  */
  TCPServerSocket(short socketfamily, unsigned short localPort, int queueLen = 5);

  /**
  *   Construct a TCP socket for use with a server, accepting connections
  *   on the specified port on the interface specified by the given address
  *   @param localAddress local interface (address) of server socket
  *   @param localPort local port of server socket
  *   @param queueLen maximum queue length for outstanding
  *                   connection requests (default 5)
  *   Use createSocket to finally create the socket
  */
  TCPServerSocket(short socketfamily, const std::string &localAddress, unsigned short localPort, int queueLen = 5);

  bool createSocket() throw(SocketException);

  /**
  *   Blocks until a new connection is established on this socket or error
  *   @return new connection socket
  *   @exception SocketException thrown if attempt to accept a new connection fails
  */
  //TCPSocket *accept() throw( SocketException );
  TCPSocket::Ptr accept() throw(SocketException);

private:
  void setListen( int queueLen ) throw( SocketException );

  std::string m_sLocalAddress;
  unsigned short m_usLocalPort;
  int m_nQueueLen;
};

/**
*   UDP socket class
*/
class Lib_API UDPSocket : public CommunicatingSocket
{
public:
  /**
  *   Construct a UDP socket
  *   Use createSocket to finally create the socket
  */
  UDPSocket(short socketfamily);

  /**
  *   Construct a UDP socket with the given local port
  *   @param localPort local port
  *   Use createSocket to finally create the socket
  */
  UDPSocket(short socketfamily, unsigned short localPort);

  /**
  *   Construct a UDP socket with the given local port and address
  *   @param localAddress local address
  *   @param localPort local port
  *   Use createSocket to finally create the socket
  */
  UDPSocket(short socketfamily, const std::string &localAddress, unsigned short localPort);

  void createSocket() throw(SocketException);

  /**
  *   Unset foreign address and port
  *   @return true if disassociation is successful
  *   @exception SocketException thrown if unable to disconnect UDP socket
  */
  void disconnect() throw( SocketException );

  /**
  *   Send the given buffer as a UDP datagram to the
  *   specified address/port
  *   @param buffer buffer to be written
  *   @param bufferLen number of bytes to write
  *   @param foreignAddress address (IP address or name) to send to
  *   @param foreignPort port number to send to
  *   @return true if send is successful
  *   @exception SocketException thrown if unable to send datagram
  */
  void sendTo( const void *buffer, int bufferLen, const std::string &foreignAddress, unsigned short foreignPort ) throw( SocketException );

  /**
  *   Read read up to bufferLen bytes data from this socket.  The given buffer
  *   is where the data will be placed
  *   @param buffer buffer to receive data
  *   @param bufferLen maximum number of bytes to receive
  *   @param sourceAddress address of datagram source
  *   @param sourcePort port of data source
  *   @return number of bytes received and -1 for error
  *   @exception SocketException thrown if unable to receive datagram
  */
  int recvFrom( void *buffer, int bufferLen, std::string &sourceAddress, unsigned short &sourcePort ) throw( SocketException );

  /**
  *   Set the multicast TTL
  *   @param multicastTTL multicast TTL
  *   @exception SocketException thrown if unable to set TTL
  */
  void setMulticastTTL( unsigned char multicastTTL ) throw( SocketException );

  /**
  *   Join the specified multicast group
  *   @param multicastGroup multicast group address to join
  *   @exception SocketException thrown if unable to join group
  */
  void joinGroup(const std::string &multicastGroup) throw( SocketException );

  /**
  *   Leave the specified multicast group
  *   @param multicastGroup multicast group address to leave
  *   @exception SocketException thrown if unable to leave group
  */
  void leaveGroup(const std::string &multicastGroup) throw( SocketException );

private:
  void setBroadcast();

  std::string m_sLocalAddress;
  unsigned short m_usLocalPort;
};

#endif // SOCKET_H__
