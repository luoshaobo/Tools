#include "socket.h"
#include "timestamp.h"

#ifdef OS_WINDOWS
#include <winsock.h>         // socket(), connect(), send(), and recv()
typedef int socklen_t;
typedef char raw_type;       // Type used for raw data on this platform
#else
#include <sys/types.h>       // data types
//#include <sys/socket.h>      // socket(), connect(), send(), and recv()
#include <cstring> //memset
#include <fcntl.h> //O_NONBLOCK etc.
#include <sys/ioctl.h> //FIONREAD

#include <netdb.h>           // gethostbyname()
#include <arpa/inet.h>       // inet_addr()
#include <unistd.h>          // close()
#include <netinet/in.h>      // sockaddr_in
typedef void raw_type;       // Type used for raw data on this platform
#endif
#include <errno.h>           // errno
#include "comalog.h"

#include <iostream>
#include <iomanip>           // hex string building
#include <algorithm>

#ifdef OS_WINDOWS
static bool g_network_initialised = false;
#endif

SocketException::SocketException(const std::string &message, bool useErrno) throw() : m_sExceptionMessage(message)
{
  if (useErrno == true)
  {
    char errnoBuf[50];
    //_snprintf_s(errnoBuf, 19, " errno: %s", errno);
    snprintf(errnoBuf, 49, " errno: %d", errno);
    m_sExceptionMessage.append(errnoBuf);
  }
}

SocketException::~SocketException() throw()
{
}

const char *SocketException::what() const throw()
{
  return (m_sExceptionMessage.c_str());
}

// Function to fill in address structure given an address and port
static void fillAddress(short socketFamily, const std::string &address, unsigned short port, sockaddr_in &sockaddr)
{
  //init sockaddr
  memset(&sockaddr, 0, sizeof(sockaddr));  // Zero out address structure
  sockaddr.sin_family = socketFamily;           // Internet address


  //The hostent structure is used by functions to store information about a given host, such as host name, IPv4 address, and so forth.
  //An application should never attempt to modify this structure or to free any of its components.Furthermore,
  //only one copy of the hostent structure is allocated per thread, and an application should therefore copy any information
  //that it needs before issuing any other Sockets API calls.
  hostent *host = 0;

  //resolve host
  if ((host = ::gethostbyname(address.c_str())) == NULL)
  {
    throw SocketException("Failed to resolve name with gethostbyname()");
  }

  sockaddr.sin_addr.s_addr = *((unsigned long *)host->h_addr_list[0]);

  //assign port in network byte order
  sockaddr.sin_port = htons(port);
}


Socket::Socket(short socketFamily, int type, int protocol) : m_socket(new SocketResource(OS_INVALID_SOCKET, type, socketFamily, protocol)),
                                                                        m_bReuseAddress(false)
{

}

//TODO: Leak? Where will 'new SocketResource' be cleaned up? (ref.counter.release() contains commented 'delete'...)
Socket::Socket(short socketFamily, int type, int protocol, os_socket_t fd) : m_socket(new SocketResource(fd, type, socketFamily, protocol)),
                                                                              m_bReuseAddress(false)
{
}

/*Socket::Socket(short socketFamily, int socketFileDescriptor) : m_socketFileDescriptor(socketFileDescriptor)
                                                              ,m_socketFamily(socketFamily)
{
}*/

Socket::Socket(SocketResource &socket) :m_socket(&socket), m_bReuseAddress(false)
{
}

Socket::~Socket()
{
}

Socket::Socket(const Socket &rhs) : m_socket(rhs.m_socket), m_bReuseAddress(rhs.m_bReuseAddress)
{
}

Socket& Socket::operator=(const Socket &rhs)
{
  if (&rhs != this)
  {
 /*   if (*m_socket != 0)
    {
      m_socket->release();
    }

    m_socket = rhs.m_socket;

    if (m_socket)
    {
      m_socket->duplicate();
    }*/
    m_socket = rhs.m_socket;

    m_bReuseAddress = rhs.m_bReuseAddress;
  }
  return *this;
}

bool Socket::createSocket() throw(SocketException)
{
  bool bRet = false;

  if (m_socket.get() != 0)
  {
    if (m_socket->getSockFD() != OS_INVALID_SOCKET)
    {
      //we already have a valid socket fd
      bRet = true;
    }
    else
    {
#ifdef OS_WINDOWS
      //in windows the network must be initialised before usage!
      if (g_network_initialised == false)
      {
        WORD wVersionRequested;
        WSADATA wsaData;

        //Prepare WinSock 2.0
        wVersionRequested = MAKEWORD(2, 0);
        if (WSAStartup(wVersionRequested, &wsaData) != 0)
        {
          throw SocketException("Unable to load/init WinSock 2.0 DLL");
        }
        g_network_initialised = true;
      }
#endif

      //create a socket and check filedescriptor
      //https://www.freebsd.org/doc/en_US.ISO8859-1/books/developers-handbook/sockets-essential-functions.html
      os_socket_t socketFileDescriptor = OS_INVALID_SOCKET;
      if ((socketFileDescriptor = ::socket(PF_INET, m_socket->getSockType(), m_socket->getProtocol())) < 0)
      {
        throw SocketException("Socket creation failed (socket())", true);
      }

      //tell resource the filedescriptor
      m_socket->setSockFD(socketFileDescriptor);

      if (m_bReuseAddress == true)
      {
        int reuse = 1;
        if (setsockopt(socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse)) < 0)
        {
          throw SocketException("Socket setting SO_REUSEADDR failed", true);
        }

#ifdef SO_REUSEPORT
        if (setsockopt(socketFileDescriptor, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(&reuse), sizeof(reuse)) < 0)
        {
          throw SocketException("Socket setting SO_REUSEPORT failed", true);
        }
#endif
      }

      bRet = true;
    }
  }

  return (bRet);
}

os_socket_t Socket::getSockFD() const
{
  os_socket_t ret = OS_INVALID_SOCKET;

  if (m_socket.get() != 0)
  {
    ret = m_socket->getSockFD();
  }

  return (ret);
}

void Socket::setReuseAddress(bool b)
{
  m_bReuseAddress = b;
}

std::string Socket::getLocalAddress() const throw(SocketException)
{
  std::string ret;
  sockaddr_in sockaddress;
  unsigned int sockaddresslen = sizeof(sockaddress);

  if (::getsockname(m_socket->getSockFD(), reinterpret_cast<sockaddr *>(&sockaddress), reinterpret_cast<socklen_t *>(&sockaddresslen)) < 0)
  {
    throw SocketException("getLocalAddress failed by getsockname()", true);
  }

  //convert to string
  ret = ::inet_ntoa(sockaddress.sin_addr);

  return (ret);
}

unsigned short Socket::getLocalPort() const throw(SocketException)
{
  unsigned short usRet = 0;
  sockaddr_in sockaddress;
  unsigned int sockaddresslen = sizeof(sockaddress);

  //getsockname() returns the current address to which the socket sockfd
  //is bound, in the buffer pointed to by addr.The addrlen argument
  //should be initialized to indicate the amount of space(in bytes)
  //pointed to by addr.On return it contains the actual size of the
  //socket address.
  if (::getsockname(m_socket->getSockFD(), reinterpret_cast<sockaddr *>(&sockaddress), reinterpret_cast<socklen_t *>(&sockaddresslen)) < 0)
  {
    throw SocketException("getLocalPort failed by getsockname()", true);
  }

  usRet = ::ntohs(sockaddress.sin_port);
  return usRet;
}

void Socket::setLocalPort(unsigned short localPort) throw(SocketException)
{
  // Bind the socket to its port
  sockaddr_in localAddress;
  memset(&localAddress, 0, sizeof(localAddress));
  localAddress.sin_family = m_socket->getSockFamily(); // AF_INET;
  localAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  localAddress.sin_port = htons(localPort);

  if (::bind(m_socket->getSockFD(), reinterpret_cast<sockaddr *>(&localAddress), sizeof(sockaddr_in)) < 0)
  {
    char message[200];
    snprintf(message, 199, "setLocalPort(%d) failed by bind()", localPort);

    throw SocketException(message, true);
  }
}

void Socket::setLocalAddressAndPort(const std::string &localAddress, unsigned short localPort) throw(SocketException)
{
  // Get the address of the requested host
  sockaddr_in localAddr;
  fillAddress(m_socket->getSockFamily(), localAddress, localPort, localAddr);

  if (::bind(m_socket->getSockFD(), (sockaddr *)&localAddr, sizeof(sockaddr_in)) < 0)
  {
    char message[200];
    snprintf(message, 199, "setLocalAddressAndPort(%s,%d) failed by bind()",localAddress.c_str(), localPort);

    throw SocketException(message, true);
  }
}

void Socket::cleanUp() throw(SocketException)
{
#ifdef OS_WINDOWS
  if (g_network_initialised == true && WSACleanup() != 0)
  {
    throw SocketException("WSACleanup() failed");
  }
#endif
}

unsigned short Socket::resolveService(const std::string &service, const std::string &protocol)
{
  unsigned short usRet = 0;
  //struct servent {
  //  char  *s_name;       /* official service name */
  //  char **s_aliases;    /* alias list */
  //  int    s_port;       /* port number */
  //  char  *s_proto;      /* protocol to use */
  //}
  //service information structure
  struct servent *serv = 0;

  if ((serv = ::getservbyname(service.c_str(), protocol.c_str())) == NULL)
  {
    //convert the service to port number
    usRet = atoi(service.c_str());
  }
  else
  {
    //found port
    usRet = ::ntohs(serv->s_port);
  }

  return (usRet);
}

// CommunicatingSocket Code
CommunicatingSocket::CommunicatingSocket(short socketfamily, int type, int protocol) : Socket(socketfamily, type, protocol)
{
}

CommunicatingSocket::CommunicatingSocket(short socketfamily, int type, int protocol, os_socket_t fd) : Socket(socketfamily, type, protocol, fd)
{
}

/*CommunicatingSocket::CommunicatingSocket(short socketFamily, int newConnectionFileDescriptor) : Socket(socketFamily, newConnectionFileDescriptor)
{
}*/

void CommunicatingSocket::close()
{
  COMALOG(COMALOG_DEBUG, "CommunicatingSocket::close\n");
  if (m_socket->getSockFD() != OS_INVALID_SOCKET)
  {
    COMALOG(COMALOG_DEBUG, "CommunicatingSocket::close(%d)\n", m_socket->getSockFD());
    os_closesocket(m_socket->getSockFD());
    m_socket->setSockFD(OS_INVALID_SOCKET);
  }
}


void CommunicatingSocket::connect(const std::string &foreignAddress, unsigned short foreignPort) throw(SocketException)
{
  // Get the address of the requested host
  sockaddr_in destAddr;
  fillAddress(m_socket->getSockFamily(), foreignAddress, foreignPort, destAddr);

  // Try to connect to the given port
  if (::connect(m_socket->getSockFD(), reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr)) < 0)
  {
    char message[200];
    snprintf(message, 199, "CommunicatingSocket::connect(%s,%u) failed by connect()", foreignAddress.c_str(), foreignPort);

    throw SocketException(message, true);
  }
}

int CommunicatingSocket::send(const void *buffer, int bufferLen) throw(SocketException)
{
  int sendBytes = ::send(m_socket->getSockFD(), (raw_type *)(buffer), bufferLen, 0);
  if (sendBytes < 0)
  {
    throw SocketException("CommunicatingSocket::send failed by send()", true);
  }
  return (sendBytes);
}

unsigned long CommunicatingSocket::recvBytesAvailable() const
{
  unsigned long ret = 0;

  os_ioctl(m_socket->getSockFD(), FIONREAD, &ret);

  return (ret);
}

int CommunicatingSocket::recv(void *buffer, int bufferLen) throw(SocketException)
{
  int nRet = -1;

  if ((nRet = ::recv(m_socket->getSockFD(), (raw_type *)buffer, bufferLen, 0)) <= 0)
  {
    char message[200];
    snprintf(message, 199, "CommunicatingSocket::recv failed by recv() at socket %s:%u", getLocalAddress().c_str(), getLocalPort());
    throw SocketException(message , true);
  }

  return nRet;
}

std::string CommunicatingSocket::getForeignAddress() const throw(SocketException)
{
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (::getpeername(m_socket->getSockFD(), reinterpret_cast<sockaddr *>(&addr), reinterpret_cast<socklen_t *>(&addr_len)) < 0)
  {
    throw SocketException("CommunicatingSocket::getForeignAddress failed by getpeername()", true);
  }

  return ::inet_ntoa(addr.sin_addr);
}

unsigned short CommunicatingSocket::getForeignPort() const throw(SocketException)
{
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (::getpeername(m_socket->getSockFD(), (sockaddr *)&addr, (socklen_t *)&addr_len) < 0)
  {
    throw SocketException("CommunicatingSocket::getForeignPort failed by getpeername()", true);
  }

  return ::ntohs(addr.sin_port);
}

bool CommunicatingSocket::setBlockingMode(bool blocking)
{
  bool ret = true;

#ifdef OS_WINDOWS
  u_long flags = blocking ? 0 : 1;
  ret = NO_ERROR == ioctlsocket(m_socket->getSockFD(), FIONBIO, &flags);
#else
  const int flags = fcntl(m_socket->getSockFD(), F_GETFL, 0);
  if ((flags & O_NONBLOCK) && !blocking)
  {
    COMALOG(COMALOG_DEBUG, "set_blocking_mode(): socket was already in non-blocking mode\n");
    return ret;
  }
  if (!(flags & O_NONBLOCK) && blocking)
  {
    COMALOG(COMALOG_DEBUG, "set_blocking_mode(): socket was already in blocking mode");
    return ret;
  }
  ret = 0 == fcntl(m_socket->getSockFD(), F_SETFL, blocking ? flags ^ O_NONBLOCK : flags | O_NONBLOCK);
#endif

  return ret;
}

int CommunicatingSocket::select(SocketList& readList, SocketList& writeList, SocketList& exceptList, const TimeElapse& timeout) throw(SocketException)
{

  fd_set fdRead;
  fd_set fdWrite;
  fd_set fdExcept;
  int nfd = 0;

  FD_ZERO(&fdRead);
  for (SocketList::const_iterator it = readList.begin(); it != readList.end(); ++it)
  {
    int fd = (*it)->getSockFD();
    if (fd != OS_INVALID_SOCKET)
    {
      //highest file descriptor number, need it for the select function
      if (int(fd) > nfd)
      {
        nfd = int(fd);
      }
      FD_SET(fd, &fdRead);
    }
  }

  FD_ZERO(&fdWrite);
  for (SocketList::const_iterator it = writeList.begin(); it != writeList.end(); ++it)
  {
    int fd = (*it)->getSockFD();
    if (fd != OS_INVALID_SOCKET)
    {
      //highest file descriptor number, need it for the select function
      if (int(fd) > nfd)
      {
        nfd = int(fd);
      }
      FD_SET(fd, &fdWrite);
    }
  }

  FD_ZERO(&fdExcept);
  for (SocketList::const_iterator it = exceptList.begin(); it != exceptList.end(); ++it)
  {
    int fd = (*it)->getSockFD();
    if (fd != OS_INVALID_SOCKET)
    {
      //highest file descriptor number, need it for the select function
      if (int(fd) > nfd)
      {
        nfd = int(fd);
      }
      FD_SET(fd, &fdExcept);
    }
  }

  //if there are no sockets return immediately
  if (nfd == 0) return 0;

  TimeElapse remainingTime(timeout);
  int rc = 0;
  do
  {
    struct timeval tv;
    tv.tv_sec = (long)remainingTime.getTotalSeconds();
    tv.tv_usec = (long)remainingTime.getFraction();
    Timestamp start;

    rc = ::select(nfd + 1, &fdRead, &fdWrite, &fdExcept, &tv);

    if (rc < 0 && Socket::lastError() == OS_EINTR)
    {
      Timestamp end;
      TimeElapse waited = end - start;
      if (waited < remainingTime)
      {
        remainingTime -= waited;
      }
      else
      {
        remainingTime = 0;
      }
    }
  } while (rc < 0 && Socket::lastError() == OS_EINTR);

  if (rc < 0)
  {
    throw SocketException("CommunicatingSocket::select() got error", true);
  }

  //prepare list for output-parameters
  SocketList readyReadList;
  for (SocketList::const_iterator it = readList.begin(); it != readList.end(); ++it)
  {
    int fd = (*it)->getSockFD();
    if (fd != OS_INVALID_SOCKET)
    {
      if (FD_ISSET(fd, &fdRead))
      {
        readyReadList.push_back(*it);
      }
    }
  }
  std::swap(readList, readyReadList);
  SocketList readyWriteList;
  for (SocketList::const_iterator it = writeList.begin(); it != writeList.end(); ++it)
  {
    int fd = (*it)->getSockFD();
    if (fd != OS_INVALID_SOCKET)
    {
      if (FD_ISSET(fd, &fdWrite))
      {
        readyWriteList.push_back(*it);
      }
    }
  }
  std::swap(writeList, readyWriteList);
  SocketList readyExceptList;
  for (SocketList::const_iterator it = exceptList.begin(); it != exceptList.end(); ++it)
  {
    int fd = (*it)->getSockFD();
    if (fd != OS_INVALID_SOCKET)
    {
      if (FD_ISSET(fd, &fdExcept))
      {
        readyExceptList.push_back(*it);
      }
    }
  }
  std::swap(exceptList, readyExceptList);
  return rc;
}

MessagingSocket::MessagingSocket(short socketfamily, int type, int protocol)
  : CommunicatingSocket(socketfamily, type, protocol)
{
}
MessagingSocket::MessagingSocket(short socketfamily, int type, int protocol, os_socket_t fd)
  : CommunicatingSocket(socketfamily, type, protocol, fd)
{
}

void MessagingSocket::printAsHex(const char* prefix, char* buf, unsigned int length) {
  const unsigned int max_print_length = 100;
  std::cout << prefix;
  for (unsigned int i = 0; i < std::min(length, max_print_length); i++) {
    std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)buf[i] << " ";
  }
  if (length > max_print_length) {
    std::cout << "...(snip)";
  }
  std::cout << std::endl;
}

void MessagingSocket::cleanupDynamicBuffers()
{
  COMALOG(COMALOG_DEBUG, "Cleaning up dynamically allocated buffers.\n");
  delete[] m_messagebuf_dynamic;
  m_messagebuf_dynamic = nullptr;
}
void MessagingSocket::resetSession()
{
  COMALOG(COMALOG_DEBUG, "Resetting message receiving session.\n");
  if (m_messagebuf_dynamic != nullptr)
  {
    delete[] m_messagebuf_dynamic;
    m_messagebuf_dynamic = nullptr;
  }
  m_collected_header_length = 0;
  m_collected_message_length = 0;
  m_expected_message_length = 0;
  m_messagebuf_export.clear();
  m_rawbuf.reset();
  m_state = CLEAN;
}

void MessagingSocket::printState(const char* prefix)
{
  COMALOG(COMALOG_DEBUG, "[ Prefix: %s, RingBuffer-size: %d, State: %d, coll.header: %d, coll.payload: %d, exp.payload: %d ]\n",
    prefix, m_rawbuf.get_size(), m_state, m_collected_header_length, m_collected_message_length, m_expected_message_length);
}


int MessagingSocket::receiveMessages(std::vector<std::string>& messages) throw (SocketException)
{
  COMALOG(COMALOG_DEBUG, "MessagingSocket::receiveMessages on port:%u\n", getLocalPort() );
  std::size_t bytes_read=0;
  char recvbuf[RECVBUF_SIZE];

  // if the method is called after a triggered `select`, always check for empty recv which signals a closed socket
  bool check_socket_close = true;

  // loop over all receivable data in RECVBUFSIZE chunks
  do
  {
    bytes_read = 0;

      // This check makes sure that data is read if data is really available. In case of a previously closed socket,
      // a `select` before this method call would trigger anyway, so we want to do a single recv anyway in order to
      // try whether we get a SocketException (which in this framework covers socket failure AND clean socket close...)
    if (recvBytesAvailable() > 0 || check_socket_close)
    {
      check_socket_close = false;
      //bytes_read = recv(recvbuf, RECVBUF_SIZE); // might throw SocketException

      int read = recv(recvbuf, RECVBUF_SIZE-1); // might throw SocketException;

      //do not assign a negative value to a size_t type or a to big value
      if (read >= 0 && read < (int)RECVBUF_SIZE)
      {
        bytes_read = read;
      }
      else
      {
        bytes_read = 0;
      }

      COMALOG(COMALOG_DEBUG, "Read from socket: %ld bytes, origread: %ld\n", bytes_read, read);
      printAsHex("Read from socket: ", recvbuf, bytes_read);
    }

    // parse tempbuf for header/message and track results and states until the
    // first partial item occurs or the tempbuf is fully processed
    if (bytes_read > 0)
    {
      // Emergency procedure: kill the current corrupted parsing session
      if (m_rawbuf.get_capacity_left() < bytes_read)
      {
        COMALOG(COMALOG_FATAL, "Exceeding rawbuf capacity! Improve the code! (m_rawbuf_length:%d, bytes_read:%d, sizeof(m_rawbuf):%d)\n",
          m_rawbuf.get_size(), bytes_read, m_rawbuf.get_capacity());
        resetSession();
        return 0;
      }

      // append recvbuf to already gathered information, check out of bounds first [KLOCWORKS]
      if (bytes_read < RECVBUF_SIZE)
      {
        m_rawbuf.write(recvbuf, bytes_read);
      }

      // depending on the current state, try to read a header or payload from collected raw data
      // and  LOOP  until there's nothing left to fully parse in the raw buffer
      while (m_rawbuf.get_size() > 0)
      {
        printState("Start of loop");

        std::size_t needed_header_length = 0;
        std::size_t needed_message_length = 0;
        std::size_t read_from_rawbuf = 0;

        switch (m_state)
        {
        case CLEAN:
          m_collected_header_length = 0;
          m_collected_message_length = 0;
          m_expected_message_length = 0;

          //check if at least 4 bytes are in rawbuffer which enables to read the expected message length the message has in total
          if (m_rawbuf.get_size() >= sizeof(m_expected_message_length))
          {
            // header should be completely parsable
            read_from_rawbuf = m_rawbuf.read(m_headerbuf, sizeof(m_expected_message_length));
            m_collected_header_length = read_from_rawbuf; // just for completeness, could be done without it

            // get header and apply correct endianness
            memcpy(&m_expected_message_length, m_headerbuf, m_collected_header_length);
            m_expected_message_length = ntohl(m_expected_message_length);
            m_state = FULL_HEADER;
          }
          else
          {
            // at most a partial header is available
            read_from_rawbuf = m_rawbuf.read(m_headerbuf, m_rawbuf.get_size());
            m_collected_header_length = read_from_rawbuf;
            m_state = PARTIAL_HEADER;
          }
          break;
        case PARTIAL_HEADER:
          needed_header_length = sizeof(m_expected_message_length) - m_collected_header_length;
          if (m_rawbuf.get_size() >= needed_header_length)
          {
            // header should be completable
            read_from_rawbuf = m_rawbuf.read(&m_headerbuf[m_collected_header_length], needed_header_length);
            m_collected_header_length += read_from_rawbuf;

            // get header and apply correct endianness
            memcpy(&m_expected_message_length, m_headerbuf, m_collected_header_length);
            m_expected_message_length = ntohl(m_expected_message_length);
            m_state = FULL_HEADER;
          }
          else
          {
            // only slight progress with collecting the header
            read_from_rawbuf = m_rawbuf.read(&m_headerbuf[m_collected_header_length], m_rawbuf.get_size());
            m_collected_header_length += read_from_rawbuf;

            m_state = PARTIAL_HEADER; // no state progress
          }

          break;
        case FULL_HEADER:
          m_collected_header_length = 0;

          if (m_rawbuf.get_size() >= m_expected_message_length)
          {
            // full message available in ringbuffer
            if (m_expected_message_length <= sizeof(m_messagebuf))
            {
              read_from_rawbuf = m_rawbuf.read(m_messagebuf, m_expected_message_length);
              m_collected_message_length = read_from_rawbuf; // just for completeness, not really needed for the msg any more
            }
            else
            {
              COMALOG(COMALOG_ERROR, "Large message of %d bytes, really a header?\n", m_expected_message_length);
            }

            m_messagebuf_export.clear();
            m_messagebuf_export.append(m_messagebuf, m_collected_message_length);
            messages.push_back(m_messagebuf_export);
            m_messagebuf_export.clear();
            m_state = CLEAN;
          }
          else
          {
            // process partial message, read all available data
            // CAUTION! At this point, payload collection continues depending on the expected size.
            // Special messages might not fit into the ringbuffer and need a lot of dynamically
            // allocated memory. Clean up afterwards!
            if (m_expected_message_length <= m_rawbuf.get_capacity())
            {
              std::size_t readSize = m_rawbuf.get_size();
              if (readSize > sizeof(m_messagebuf))
              {
                readSize = sizeof(m_messagebuf);
                COMALOG(COMALOG_ERROR, "%s m_rawbuf size:%d > m_messagebuf size:%d \n", __FUNCTION__, m_rawbuf.get_size(), readSize);
              }

              read_from_rawbuf = m_rawbuf.read(m_messagebuf, readSize);
              m_collected_message_length = read_from_rawbuf;

              m_state = PARTIAL_PAYLOAD;
            }
            else
            {
              COMALOG(COMALOG_WARN, "Large message expected! Allocating special buffer with %d bytes.\n", m_expected_message_length);
              m_messagebuf_dynamic = new char[m_expected_message_length]; // Caution! Clean up afterwards!
              read_from_rawbuf = m_rawbuf.read(m_messagebuf_dynamic, m_rawbuf.get_size());
              m_collected_message_length = read_from_rawbuf;
              m_state = PARTIAL_LARGE_PAYLOAD;
            }
          }
          break;
        case PARTIAL_PAYLOAD:
          needed_message_length = m_expected_message_length - m_collected_message_length;
          if (m_rawbuf.get_size() >= needed_message_length)
          {
            // message/payload can be completed
            read_from_rawbuf = m_rawbuf.read(&m_messagebuf[m_collected_message_length], needed_message_length);
            m_collected_message_length += read_from_rawbuf;

            m_messagebuf_export.clear();
            m_messagebuf_export.append(m_messagebuf, m_collected_message_length);
            messages.push_back(m_messagebuf_export);
            m_messagebuf_export.clear();
            m_state = CLEAN;
          }
          else
          {
            // partial progress with message
            read_from_rawbuf = m_rawbuf.read(&m_messagebuf[m_collected_message_length], m_rawbuf.get_size());
            m_collected_message_length += read_from_rawbuf;

            m_state = PARTIAL_PAYLOAD;
          }
          break;
        case PARTIAL_LARGE_PAYLOAD:
          needed_message_length = m_expected_message_length - m_collected_message_length;
          if (m_rawbuf.get_size() >= needed_message_length)
          {
            // message/payload can be completed
            read_from_rawbuf = m_rawbuf.read(&m_messagebuf_dynamic[m_collected_message_length], needed_message_length);
            m_collected_message_length += read_from_rawbuf;
            m_messagebuf_export.clear();
            m_messagebuf_export.append(m_messagebuf_dynamic, m_collected_message_length);
            messages.push_back(m_messagebuf_export); //TODO: Mem. problem with large messages? This will COPY/DUPL. the message, right?
            m_messagebuf_export.clear();
            // Cleanup!
            cleanupDynamicBuffers();
            m_state = CLEAN;
          }
          else
          {
            // partial progress with message
            read_from_rawbuf = m_rawbuf.read(&m_messagebuf_dynamic[m_collected_message_length], m_rawbuf.get_size());
            m_collected_message_length += read_from_rawbuf;
            m_state = PARTIAL_LARGE_PAYLOAD;
          }

          break;
        default:
          COMALOG(COMALOG_ERROR, "Unsupported state %d!\n", m_state);
          resetSession();
          break;
        }

        printState("End of loop  ");
      }
    }
  } while (bytes_read > 0);

  return messages.size();
}

int MessagingSocket::sendMessage(const std::string& message)
{
  if (!message.empty())
  {
    if (message.size() > MAX_MESSAGE_SIZE)
    {
      COMALOG(COMALOG_ERROR, "Message too large (%ld) for sending! (Max. message size: %ld)\n", message.size(), MAX_MESSAGE_SIZE);
      return 0;
    }
    // first send message length/size
    uint32_t endianized_length = htonl(message.size());
    unsigned int sent_header_bytes = send(&endianized_length, sizeof(endianized_length));
    // immediately send message payload afterwards
    unsigned int sent_payload_bytes = 0;
    unsigned int msg_size = message.size();
    unsigned int send_calls = 0;
    // the loop is never running indefinitely because send() always returns >0 or throws an exc.
    const char* message_ptr = message.c_str();
    while (sent_payload_bytes < msg_size)
    {

      //TODO: ask elwe
      sent_payload_bytes += send(message_ptr + sent_payload_bytes, msg_size - sent_payload_bytes);
      send_calls++;
    }
    if (send_calls > 1)
    {
      COMALOG(COMALOG_WARN, "sendMessage (message length: %d) needed %d send() calls. Reasonable?\n", msg_size, send_calls);
    }
    return sent_header_bytes + sent_payload_bytes;
  }
  else
  {
    return 0;
  }
}

TCPSocket::TCPSocket(short socketfamily) : MessagingSocket(socketfamily, SOCK_STREAM, IPPROTO_TCP), m_sSocketFamily(socketfamily),
m_sForeignAddress(),
m_usForeignPort()
{
}

TCPSocket::TCPSocket(short socketfamily, const std::string &foreignAddress, unsigned short foreignPort) : MessagingSocket(socketfamily, SOCK_STREAM, IPPROTO_TCP),
m_sSocketFamily(socketfamily),
m_sForeignAddress(foreignAddress),
m_usForeignPort(foreignPort)
{
  //connect(foreignAddress, foreignPort);
}

/*TCPSocket::TCPSocket(short socketFamily, int type, os_socket_t newConnectionFileDescriptor) : CommunicatingSocket(socketFamily, type, newConnectionFileDescriptor)
{
}*/

TCPSocket::TCPSocket(short socketFamily, int type, int protocol, os_socket_t fd) : MessagingSocket(socketFamily, type, protocol, fd),
m_sSocketFamily(socketFamily),
m_sForeignAddress(),
m_usForeignPort()
{
}

void TCPSocket::connect() throw(SocketException)
{
  CommunicatingSocket::connect(m_sForeignAddress, m_usForeignPort);
}


// TCPServerSocket Code
TCPServerSocket::TCPServerSocket(short socketfamily, unsigned short localPort, int queueLen) : Socket(socketfamily, SOCK_STREAM, IPPROTO_TCP),
  m_sLocalAddress(),
  m_usLocalPort(localPort),
  m_nQueueLen(queueLen)
{
}

TCPServerSocket::TCPServerSocket(short socketfamily, const std::string &localAddress, unsigned short localPort, int queueLen) : Socket(socketfamily, SOCK_STREAM, IPPROTO_TCP),
  m_sLocalAddress(localAddress),
  m_usLocalPort(localPort),
  m_nQueueLen(queueLen)
{
}

bool TCPServerSocket::createSocket() throw(SocketException)
{
  bool bRet = Socket::createSocket();

  if (!m_sLocalAddress.empty())
  {
    setLocalAddressAndPort(m_sLocalAddress, m_usLocalPort);
  }
  else
  {
    setLocalPort(m_usLocalPort);
  }

  setListen(m_nQueueLen);

  return (bRet);
}

/*TCPSocket *TCPServerSocket::accept() throw(SocketException)
{
  int newConnectionFileDescriptor = OS_INVALID_SOCKET;

  if ((newConnectionFileDescriptor = ::accept(m_socket->getSockFD(), NULL, 0)) < 0)
  {
    throw SocketException("TCPServerSocket::accept failed by accept()", true);
  }

  return (new TCPSocket(m_socket->getSockFamily(), m_socket->getSockType(), newConnectionFileDescriptor));
}*/
TCPSocket::Ptr TCPServerSocket::accept() throw(SocketException)
{
  int newConnectionFileDescriptor = OS_INVALID_SOCKET;

  if ((newConnectionFileDescriptor = ::accept(m_socket->getSockFD(), NULL, 0)) < 0)
  {
    throw SocketException("TCPServerSocket::accept failed by accept()", true);
  }

  return (TCPSocket::Ptr(new TCPSocket(m_socket->getSockFamily(), m_socket->getSockType(), m_socket->getProtocol(), newConnectionFileDescriptor)));
}

void TCPServerSocket::setListen(int queueLen) throw(SocketException)
{
  if (::listen(m_socket->getSockFD(), queueLen) < 0)
  {
    throw SocketException("TCPServerSocket::setListen failed by listen()", true);
  }
}

// UDPSocket Code

UDPSocket::UDPSocket(short socketfamily) : CommunicatingSocket(socketfamily, SOCK_DGRAM, IPPROTO_UDP),
m_sLocalAddress(),
m_usLocalPort(0)
{
}

UDPSocket::UDPSocket(short socketfamily, unsigned short localPort) : CommunicatingSocket(socketfamily, SOCK_DGRAM, IPPROTO_UDP),
m_sLocalAddress(),
m_usLocalPort(localPort)
{
}

UDPSocket::UDPSocket(short socketfamily, const std::string &localAddress, unsigned short localPort) : CommunicatingSocket(socketfamily, SOCK_DGRAM, IPPROTO_UDP),
m_sLocalAddress(localAddress),
m_usLocalPort(localPort)
{
}

void UDPSocket::setBroadcast()
{
  //SO_BROADCAST
  //Permits sending of broadcast messages, if this is supported by the protocol.This option takes an int value.This is a Boolean option.
  int allowBroadcast = 1;
  ::setsockopt(m_socket->getSockFD(), SOL_SOCKET, SO_BROADCAST, reinterpret_cast<raw_type *>(&allowBroadcast), sizeof(allowBroadcast));
}


void UDPSocket::createSocket() throw(SocketException)
{
  if (!m_sLocalAddress.empty())
  {
    setLocalAddressAndPort(m_sLocalAddress, m_usLocalPort);
  }
  else
  {
    if (m_usLocalPort != 0)
    {
      setLocalPort(m_usLocalPort);
    }
  }

  setBroadcast();

}

void UDPSocket::disconnect() throw(SocketException)
{
  sockaddr_in nullAddress;
  memset(&nullAddress, 0, sizeof(nullAddress));
  nullAddress.sin_family = AF_UNSPEC;

  // Try to disconnect
  if (::connect(m_socket->getSockFD(), reinterpret_cast<sockaddr *>(&nullAddress), sizeof(nullAddress)) < 0)
  {
#ifdef OS_WINDOWS
    if (errno != WSAEAFNOSUPPORT)
    {
#else
    if (errno != EAFNOSUPPORT)
    {
#endif
      throw SocketException("UDPSocket::disconnect failed by connect()", true);
    }
  }
}

void UDPSocket::sendTo(const void *buffer, int bufferLen, const std::string &foreignAddress, unsigned short foreignPort) throw(SocketException)
{
  sockaddr_in destAddr;
  fillAddress(m_socket->getSockFamily(), foreignAddress, foreignPort, destAddr);

  // Write out the whole buffer as a single message.
  if (::sendto(m_socket->getSockFD(), (raw_type *)(buffer), bufferLen, 0, reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr)) != bufferLen)
  {
    throw SocketException("UDPSocket::sendTo failed by sendto()", true);
  }
}

int UDPSocket::recvFrom(void *buffer, int bufferLen, std::string &sourceAddress, unsigned short &sourcePort) throw(SocketException)
{
  int nRet = -1;

  sockaddr_in clientAddress;
  socklen_t addrLen = sizeof(clientAddress);

  nRet = ::recvfrom(m_socket->getSockFD(),
    reinterpret_cast<raw_type *>(buffer),
    bufferLen,
    0,
    reinterpret_cast<sockaddr *>(&clientAddress),
    reinterpret_cast<socklen_t *>(&addrLen));

  if (nRet < 0)
  {
    throw SocketException("UDPSocket::recvFrom failed by recvfrom()", true);
  }

  sourceAddress = ::inet_ntoa(clientAddress.sin_addr);
  sourcePort = ::ntohs(clientAddress.sin_port);

  return nRet;
}

void UDPSocket::setMulticastTTL(unsigned char multicastTTL) throw(SocketException)
{
  //The IP multicast routing protocol uses the Time To Live(TTL) field of IP datagrams to decide how "far" from a sending
  //host a given multicast packet should be forwarded.The default TTL for multicast datagrams is 1, which will result
  //in multicast packets going only to other hosts on the local network.A setsockopt(2) call may be used to change the TTL :

  // As the values of the TTL field increase, routers will expand the number of hops they will forward a multicast packet.
  //To provide meaningful scope control, multicast routers enforce the following "thresholds" on forwarding based on the TTL field:
  //0   restricted to the same host
  //1   restricted to the same subnet
  //32  restricted to the same site
  //64  restricted to the same region
  //128 restricted to the same continent
  //255 unrestricted

  //Choose a value of 1 for the entire subnet to see the traffic.
  //Even larger values will enable the packets to pass through routers.
  //However, these routers must be set up with multicast routing tables in order to successfully relay multicast traffic.

  if (::setsockopt(m_socket->getSockFD(), IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<raw_type *>(&multicastTTL), sizeof(multicastTTL)) < 0)
  {
    throw SocketException("UDPSocket::setMulticastTTL failed by setsockopt()", true);
  }
}

void UDPSocket::joinGroup(const std::string &multicastGroup) throw(SocketException)
{
  //Multicast Groups and Addresses
  //Every IP multicast group has a group address.IP multicast provides only open groups :
  //That is, it is not necessary to be a member of a group in order to send datagrams to the group.
  //Multicast address are like IP addresses used for single hosts,
  //and is written in the same way : A.B.C.D.Multicast addresses will never clash with host addresses because
  //a portion of the IP address space is specifically reserved for multicast.
  //This reserved range consists of addresses from 224.0.0.0 to 239.255.255.255.
  //However, the multicast addresses from 224.0.0.0 to 224.0.0.255 are reserved for multicast routing information;
  //Application programs should use multicast addresses outside this range.
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

  if (::setsockopt(m_socket->getSockFD(), IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<raw_type *>(&multicastRequest), sizeof(multicastRequest)) < 0)
  {
    throw SocketException("UDPSocket::joinGroup failed by setsockopt()", true);
  }
}

void UDPSocket::leaveGroup(const std::string &multicastGroup) throw(SocketException)
{
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

  if (::setsockopt(m_socket->getSockFD(), IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<raw_type *>(&multicastRequest), sizeof(multicastRequest)) < 0)
  {
    throw SocketException("UDPSocket::leaveGroup failed by setsockopt()", true);
  }
}
