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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#define close               closesocket
#endif // #ifndef WIN32
#include "TK_sock.h"

#define UNUSED_VAR(var)                                (var)

#ifdef WIN32
//int errno;
// char *strerror(int errnum) {
//    UNUSED_VAR(errnum);
//    return "(no info)";
// }
// void perror(const char *string) {
// }
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

namespace TK_Tools {

Socket::Socket() : m_socket(-1), m_bblock(true)
{
	
}

bool Socket::create(int type /*= SOCK_STREAM*/, int protocol /*= 0*/, int domain /*= PF_INET*/)
{
	m_socket = ::socket(domain, type, protocol);
	if (m_socket == -1)
	{
		//errputs("Error: ");
		//errputs(strerror(errno));
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

bool Socket::getlocalport(unsigned short &port) const
{
    bool suc = false;
    struct sockaddr localaddr;
    
    port = 0;

    suc = getlocalsock(&localaddr, NULL);
    if (!suc) {
        return false;
    }
    sockaddr_in *my_addr = reinterpret_cast<sockaddr_in *>(&localaddr);
	port = ntohs(my_addr->sin_port);
    suc = true;

    return suc;
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

bool Socket::GetLocalSocketAddr(std::string &sAddr, unsigned short &nPort)
{
    bool bRet = true;
    struct sockaddr_in oSockInet;
    int nSockInetLen = sizeof(sockaddr_in);

    if (bRet) {
        bRet = getlocalsock((sockaddr *)&oSockInet, &nSockInetLen);
    }

    if (bRet) {
        nPort = ::ntohs(oSockInet.sin_port);
        sAddr = ::inet_ntoa(oSockInet.sin_addr);
    }

    return bRet;
}

bool Socket::GetPeerSocketAddr(std::string &sAddr, unsigned short &nPort)
{
    bool bRet = true;
    struct sockaddr_in oSockInet;
    int nSockInetLen = sizeof(sockaddr_in);

    if (bRet) {
        bRet = getpeersock((sockaddr *)&oSockInet, &nSockInetLen);
    }

    if (bRet) {
        nPort = ::ntohs(oSockInet.sin_port);
        sAddr = ::inet_ntoa(oSockInet.sin_addr);
    }

    return bRet;
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
		//perror("1");
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
		*port = ntohs(from.sin_port);
	if (addr != NULL)
		strcpy(addr, inet_ntoa(from.sin_addr));
	
	return true;
}

} // namespace TK_Tools {
