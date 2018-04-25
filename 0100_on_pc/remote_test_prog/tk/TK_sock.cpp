/* Copyright (R) 2002. Hexin Software Technology Co., Ltd. All rights reserved. 
 * 
 * 文件名:       sock.cpp
 * 对应文件:     sock.h
 *
 * 上次版本号：  
 * 完成人员：    
 * 完成日期：    
 *
 * 本次版本号：  0.1.0
 * 完成人员：    罗少波
 * 完成日期：    2003.1.15
 *
 * 主要内容：实现类Socket，提供操作socket的常用方法。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef WIN32
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <stropts.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#define close               closesocket
#endif // #ifndef WIN32
#include "TK_sock.h"

#define UNUSED_VAR(var)                                (var)

#define L() printf("### %s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__);
#define LE() printf("*** %s :%d: %s\n", __FILE__, __LINE__, __FUNCTION__);

namespace TK_Tools {

#ifdef WIN32
int errno;
 char *strerror(int errnum) {
    UNUSED_VAR(errnum);
    return "(no info)";
 }
 void perror(const char *string) {
 }
#endif // #ifdef WINCE

#define errputs puts

#ifndef MAX_BUF_SIZE
#define MAX_BUF_SIZE		1024
#endif

#ifndef MAX_STR_LEN
#define MAX_STR_LEN			254
#endif

#define P(x) cout << (x) << "<br>"<< endl
#define F(str, file) \
	{ \
		char cmd[MAX_BUF_SIZE]; \
		sprintf(cmd, "echo \'%s\' >> %s", str, file); \
		system(cmd); \
		
#define ASSERT_NOT(x) \
	if (x) \
	{ \
		perror("ERROR"); \
		exit(1); \
	}
	
//extern int errno;

Socket::Socket() : m_socket(-1), m_bblock(true)
{
	
}

bool Socket::create(int type /*= SOCK_STREAM*/, int protocol /*= 0*/, int domain /*= PF_INET*/)
{
	m_socket = ::socket(domain, type, protocol);
	if (m_socket == -1)
	{
		errputs("Error: ");
		errputs(strerror(errno));
		return false;
	}
	return true;
}

Socket::~Socket()
{	
	destroy();
}

void Socket::destroy()
{
	if (m_socket != -1)
	{
		::shutdown(m_socket, SHUT_RDWR);
		::close(m_socket);
		m_socket = -1;
		m_bblock = true;
	}
}

bool Socket::open_fd(int type /*= SOCK_STREAM*/, int protocol /*= 0*/, int domain /*= PF_INET*/)
{
	if (m_socket != -1)
	{
		::shutdown(m_socket, SHUT_RDWR);
		::close(m_socket);
		m_socket = -1;
		m_bblock = true;
	}
	m_socket = ::socket(domain, type, protocol);
	return m_socket == -1 ? false : true;
}

void Socket::close_fd()
{
	if (m_socket != -1)
	{
		::close(m_socket);
		m_socket = -1;
		m_bblock = true;
	}
}

void Socket::attach_fd(int fd)
{
	if (m_socket != -1)
	{
		::shutdown(m_socket, SHUT_RDWR);
		::close(m_socket);
		m_socket = -1;
		m_bblock = true;
	}
	m_socket = fd;
}

void Socket::detach_fd()
{
	m_socket = -1;
	m_bblock = true;
}

void Socket::shutdown_fd(int how /*= SHUT_RDWR*/)
{
	if (m_socket != -1)
	{
		::shutdown(m_socket, how);
	}
}

bool Socket::get_fd(int& fd) const
{
	if (m_socket == -1)
		return false;
	fd = m_socket;
	return true;
}

bool Socket::getlocalsock(struct sockaddr* localaddr, int* addrlen /*= NULL*/) const
{
	if (m_socket == -1)
		return false;

	int len = sizeof(struct sockaddr);
	if (addrlen == NULL)
		addrlen = &len;
	int result = ::getsockname(m_socket, localaddr, (socklen_t *)addrlen);
	
	return result == -1 ? false : true;
}

bool Socket::getpeersock(struct sockaddr* peeraddr, int* addrlen /*= NULL*/) const
{
	if (m_socket == -1)
		return false;
		
	int len = sizeof(struct sockaddr);
	if (addrlen == NULL)
		addrlen = &len;
	int result = ::getpeername(m_socket, peeraddr, (socklen_t *)addrlen);
	
	return result == -1 ? false : true;
}

void Socket::reuse_addr(bool reuse /*= true*/)
{
	char open = reuse ? 1 : 0;
	socklen_t optlen = sizeof(open);
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &open, optlen);
}

bool Socket::bind(struct sockaddr* addr, int addrlen /*= sizeof(struct sockaddr)*/)
{
	if (m_socket == -1)
		return false;
		
	int result = ::bind(m_socket, addr, addrlen);
	
	return result == -1 ? false : true;
}

bool Socket::bind(unsigned short port /*= 0*/, const char* addr /*= NULL*/, short family /*= AF_INET*/)
{
	if (m_socket == -1)
	{
		perror("1");
		return false;
	}
	
	sockaddr_in my_addr = { sizeof(struct sockaddr_in) };
	my_addr.sin_family = family;
	my_addr.sin_port = htons(port);
	if (addr == NULL)
		my_addr.sin_addr.s_addr = INADDR_ANY;
	else
		my_addr.sin_addr.s_addr = inet_addr(addr);
	
	int addrlen = sizeof(struct sockaddr_in);
	int result = ::bind(m_socket, (struct sockaddr*)&my_addr, addrlen);
	return result == -1 ? false : true;
}

bool Socket::listen(int backlog /*= MAX_REQUEST*/)
{
	if (m_socket == -1)
		return false;
		
	int result = ::listen(m_socket, backlog);
	
	return result == -1 ? false : true;
}

bool Socket::accept(int& connected_fd, struct sockaddr* addr /*= NULL*/, int* addrlen /*= NULL*/)
{
	if (m_socket == -1)
		return false;
	
	connected_fd = ::accept(m_socket, addr, (socklen_t *)addrlen);
	
	return connected_fd == -1 ? false : true;
}

bool Socket::accept(Socket& sock, struct sockaddr* addr /*= NULL*/, int* addrlen /*= NULL*/)
{
	if (sock.m_socket != -1)
		sock.detach_fd();
		
	if (m_socket == -1)
		return false;
		
	sock.m_socket = ::accept(m_socket, addr, (socklen_t *)addrlen);
	
	return sock.m_socket == -1 ? false : true;
}

bool Socket::connect(struct sockaddr* addr, int addrlen /*= sizeof(struct sockaddr)*/)
{
	if (m_socket == -1)
		return false;
	
	int result = ::connect(m_socket, addr, addrlen);
	
	return result == -1 ? false : true;
}

bool Socket::connect(const char* addr, unsigned short port, short family /*= AF_INET*/)
{
	if (m_socket == -1)
		return false;
		
	if (addr == NULL)
		return false;
		
	sockaddr_in my_addr;
	my_addr.sin_family = family;
	my_addr.sin_port = htons(port);
	struct in_addr sin_addr;
#ifndef WIN32
	if (!inet_aton(addr, &sin_addr))
		return false;
#else
    sin_addr.S_un.S_addr = inet_addr(addr);
#endif // #ifndef WIN32
	my_addr.sin_addr = sin_addr;	
	
	int addrlen = sizeof(struct sockaddr);
	
	int result = ::connect(m_socket, (struct sockaddr*)&my_addr, addrlen);
	
	return result == -1 ? false : true;
}

void Socket::set_block(bool bblock /*= true*/)
{
	m_bblock = bblock;
	
	if (m_socket == -1)
		return;
		
#ifndef WIN32
	int flag;
	flag = ::fcntl(m_socket, F_GETFL, 0);
	if (bblock)
		flag &= ~O_NONBLOCK;
	else
		flag |= O_NONBLOCK;
	::fcntl(m_socket, F_SETFL, flag);
#else
    u_long iMode = bblock ? 0 : 1;
    ioctlsocket(m_socket, FIONBIO, &iMode);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// private methodes

bool Socket::_send_aux(bool is_const, const void* buf, size_t& size, int flags)
{
	if (m_socket == -1)
		return false;
		
	ssize_t new_size = ::send(m_socket, (const char *)buf, size, flags);
	if (!is_const)
		size = new_size;
	
	return new_size == -1 ? false : true;
}

bool Socket::_recv_aux(bool is_const, void* buf, size_t& size, int flags)
{
	if (m_socket == -1)
		return false;
	
	ssize_t new_size = ::recv(m_socket, (char *)buf, size, flags);
	if (!is_const)
		size = new_size;
	
	return new_size == -1 ? false : true;
}

bool Socket::_sendto_aux(bool is_const, const void* buf, size_t& size, struct sockaddr* to, socklen_t addrlen, int flags)
{
	if (m_socket == -1)
		return false;
		
	ssize_t new_size = ::sendto(m_socket, (const char *)buf, size, flags, to, addrlen);
	if (!is_const)
	{
		size = new_size;
	}
	
	return new_size == -1 ? false : true;
}

bool Socket::_sendto_aux(bool is_const, const void* buf, size_t& size, unsigned short port, const char* addr, int flags)
{
	if (m_socket == -1)
		return false;
		
	sockaddr_in my_addr = { sizeof(struct sockaddr_in) };
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	if (addr == NULL)
		my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	else
		my_addr.sin_addr.s_addr = inet_addr(addr);
	
	socklen_t addrlen = sizeof(sockaddr_in);
		
	return _sendto_aux(is_const, buf, size, (struct sockaddr*)&my_addr, addrlen, flags);
}

bool Socket::_recvfrom_aux(bool is_const, void* buf, size_t& size, struct sockaddr* from, socklen_t* addrlen, int flags)
{
	if (m_socket == -1)
		return false;
		
	ssize_t new_size = ::recvfrom(m_socket, (char *)buf, size, flags, from, (socklen_t *)addrlen);
	if (!is_const)
	{
		size = new_size;
	}
	
	return new_size == -1 ? false : true;
}

bool Socket::_recvfrom_aux(bool is_const, void* buf, size_t& size, unsigned short* port, char* addr, int flags)
{
	struct sockaddr_in from;
	socklen_t addrlen;
	if (!_recvfrom_aux(is_const, buf, size, (struct sockaddr*)&from, &addrlen, flags))
		return false;
		
	if (port != NULL)
		*port = from.sin_port;
	if (addr != NULL)
		strcpy(addr, inet_ntoa(from.sin_addr));
	
	return true;
}

#ifndef WIN32
static bool make_addr__unix(const char *addr_path, struct sockaddr_un &unix_addr, socklen_t &unix_addr_len)
{
    bool bRet = true;

    memset(&unix_addr, 0, sizeof(sockaddr_un));
    unix_addr_len = 0;

    if (bRet) {
        if (addr_path == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        unix_addr.sun_family = AF_UNIX;
        strncpy(unix_addr.sun_path, addr_path, sizeof(unix_addr.sun_path));
        unix_addr.sun_path[sizeof(unix_addr.sun_path) - 1] = '\0';
        unix_addr_len = (offsetof(struct sockaddr_un, sun_path) + strlen(unix_addr.sun_path));
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::bind__unix(const char *addr_path)
{
    bool bRet = true;
    int nRet;
    struct sockaddr_un unix_addr;
    socklen_t unix_addr_len;

    if (bRet) {
        if (addr_path == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        bRet = make_addr__unix(addr_path, unix_addr, unix_addr_len);
    }

    if (bRet) {
        nRet = ::bind(m_socket, (const sockaddr*)&unix_addr, unix_addr_len);
        if (nRet < 0) {
            bRet = false;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::connect__unix(const char *addr_path)
{
    bool bRet = true;
    int nRet;
    struct sockaddr_un unix_addr;
    socklen_t unix_addr_len;

    if (bRet) {
        if (addr_path == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        bRet = make_addr__unix(addr_path, unix_addr, unix_addr_len);
    }

    if (bRet) {
        nRet = ::connect(m_socket, (const sockaddr*)&unix_addr, unix_addr_len);
        if (nRet < 0) {
            bRet = false;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::sendto__unix(const void* buf, size_t& size, const char *addr_path)
{
    bool bRet = true;
    int nRet;
    struct sockaddr_un unix_addr;
    socklen_t unix_addr_len;

    if (bRet) {
        if (buf == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        if (size == 0) {
            bRet = false;
        }
    }

    if (bRet) {
        if (addr_path != NULL) {
            bRet = make_addr__unix(addr_path, unix_addr, unix_addr_len);
        }
    }

    if (bRet) {
        if (addr_path != NULL) {
            nRet = ::sendto(m_socket, buf, size, 0, (const sockaddr*)&unix_addr, unix_addr_len);
        } else {
            nRet = ::sendto(m_socket, buf, size, 0, NULL, 0);
        }
        if (nRet < 0) {
            bRet = false;
        } else {
            size = nRet;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::sendto__unix(const void* buf, size_t& size, void *receiver_addr, socklen_t *receiver_addr_len)
{
    bool bRet = true;
    int nRet;

    if (bRet) {
        if (buf == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        if (size == 0) {
            bRet = false;
        }
    }

    if (bRet) {
        if (receiver_addr != NULL && receiver_addr_len != NULL) {
            nRet = ::sendto(m_socket, buf, size, 0, (const sockaddr*)&receiver_addr, *receiver_addr_len);
        } else {
            nRet = ::sendto(m_socket, buf, size, 0, NULL, 0);
        }
        if (nRet < 0) {
            bRet = false;
        } else {
            size = nRet;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::recvfrom__unix(void* buf, size_t& size, void *sender_addr /*= NULL*/, socklen_t *sender_addr_len /*= NULL*/)
{
    bool bRet = true;
    int nRet;

    if (bRet) {
        if (buf == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        if (size == 0) {
            bRet = false;
        }
    }

    if (bRet) {
        if (sender_addr != NULL && sender_addr_len != NULL) {
            nRet = ::recvfrom(m_socket, buf, size, 0, (sockaddr*)&sender_addr, sender_addr_len);
        } else {
            nRet = ::recvfrom(m_socket, buf, size, 0, NULL, 0);
        }
        if (nRet < 0) {
            bRet = false;
        } else {
            size = nRet;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::sendmsg_fd__unix(int fd_to_send, const char *addr_path)
{
    bool bRet = true;
    int nRet;
    char data[1] = { 0 };
    struct iovec iov[1];
    struct sockaddr_un unix_addr;
    socklen_t unix_addr_len;
    struct msghdr msghdr_send;
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct cmsghdr *pcmsghdr;

    if (bRet) {
        if (addr_path != NULL) {
            bRet = make_addr__unix(addr_path, unix_addr, unix_addr_len);
        }
    }

    if (bRet) {
        memset(&msghdr_send, 0, sizeof(struct msghdr));
        memset(&control_un, 0, sizeof(control_un));

        msghdr_send.msg_control = control_un.control;
        msghdr_send.msg_controllen = sizeof(control_un.control);

        pcmsghdr = CMSG_FIRSTHDR(&msghdr_send);
        pcmsghdr->cmsg_len = CMSG_LEN(sizeof(int));
        pcmsghdr->cmsg_level = SOL_SOCKET;
        pcmsghdr->cmsg_type = SCM_RIGHTS;
        *((int *)CMSG_DATA(pcmsghdr)) = fd_to_send;

        if (addr_path != NULL) {
            msghdr_send.msg_name = &unix_addr;
            msghdr_send.msg_namelen = unix_addr_len;
        }

        iov[0].iov_base = (void *)data;
        iov[0].iov_len = 1;
        msghdr_send.msg_iov = iov;
        msghdr_send.msg_iovlen = 1;

        nRet = ::sendmsg(m_socket, &msghdr_send, 0);
        if (nRet < 0) {
            bRet = false;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::sendmsg_fd__unix(int fd_to_send, void *receiver_addr, socklen_t *receiver_addr_len)
{
    bool bRet = true;
    int nRet;
    char data[1] = { 0 };
    struct iovec iov[1];
    struct msghdr msghdr_send;
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct cmsghdr *pcmsghdr;

    if (bRet) {
        memset(&msghdr_send, 0, sizeof(struct msghdr));
        memset(&control_un, 0, sizeof(control_un));

        msghdr_send.msg_control = control_un.control;
        msghdr_send.msg_controllen = sizeof(control_un.control);

        pcmsghdr = CMSG_FIRSTHDR(&msghdr_send);
        pcmsghdr->cmsg_len = CMSG_LEN(sizeof(int));
        pcmsghdr->cmsg_level = SOL_SOCKET;
        pcmsghdr->cmsg_type = SCM_RIGHTS;
        *((int *)CMSG_DATA(pcmsghdr)) = fd_to_send;

        msghdr_send.msg_name = receiver_addr;
        msghdr_send.msg_namelen = receiver_addr_len != NULL ? *receiver_addr_len : 0;

        iov[0].iov_base = (void *)data;
        iov[0].iov_len = 1;
        msghdr_send.msg_iov = iov;
        msghdr_send.msg_iovlen = 1;

        nRet = ::sendmsg(m_socket, &msghdr_send, 0);
        if (nRet < 0) {
            bRet = false;
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool Socket::recvmsg_fd__unix(int &fd_received, void *sender_addr /*= NULL*/, socklen_t *sender_addr_len /*= NULL*/)
{
    bool bRet = true;
    int nRet;
    char data[1] = { 0 };
    struct iovec iov[1];
    struct msghdr msghdr_recv;
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct cmsghdr *pcmsghdr;

    if (bRet) {
        memset(&msghdr_recv, 0, sizeof(struct msghdr));
        memset(&control_un, 0, sizeof(control_un));

        msghdr_recv.msg_control = control_un.control;
        msghdr_recv.msg_controllen = sizeof(control_un.control);

        pcmsghdr = CMSG_FIRSTHDR(&msghdr_recv);
        pcmsghdr->cmsg_len = CMSG_LEN(sizeof(int));
        pcmsghdr->cmsg_level = SOL_SOCKET;
        pcmsghdr->cmsg_type = SCM_RIGHTS;
        *((int *)CMSG_DATA(pcmsghdr)) = fd_received;

        msghdr_recv.msg_name = sender_addr;
        msghdr_recv.msg_namelen = sender_addr_len != NULL ? *sender_addr_len : 0;

        iov[0].iov_base = (void *)data;
        iov[0].iov_len = 1;
        msghdr_recv.msg_iov = iov;
        msghdr_recv.msg_iovlen = 1;

        nRet = ::recvmsg(m_socket, &msghdr_recv, 0);
        if (nRet < 0) {
            bRet = false;
        } else {
            fd_received = *((int *)CMSG_DATA(pcmsghdr));
            if (sender_addr_len != NULL) {
                *sender_addr_len = msghdr_recv.msg_namelen;
            }
        }
    }

    return bRet;
}
#endif // #ifndef WIN32

} // namespace TK_Tools {
