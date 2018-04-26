/* Copyright (R) 2002. Hexin Software Technology Co., Ltd. All rights reserved. 
 * 
 * 文件名:       sock.h
 * 对应文件:     sock.cpp
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

#ifndef _SOCK_H_JKL23895U23N524NM235R90124H1U238XHY901P4J3HY845_
#define _SOCK_H_JKL23895U23N524NM235R90124H1U238XHY901P4J3HY845_

#ifndef WIN32
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#define socklen_t                       int
#define ssize_t                         int
#define SHUT_RDWR                       2
#endif // #ifndef WIN32

#define MAX_REQUEST 255

namespace TK_Tools {

class Socket
{
public:
	// 构造与解除
	Socket();								// 创建一个空的Socket对象
	bool create(int type = SOCK_STREAM, int protocol = 0, int domain = PF_INET);
											// 为空的对象创建socket描述符
	~Socket();								// 关闭连接和套接字
	void destroy();							// 关闭连接和套接字
	
	bool open_fd(int type = SOCK_STREAM, int protocol = 0, int domain = PF_INET);	
											// 先关闭原有的连接和套接字，然后创建新的socket
	void close_fd();						// 关闭套接字
	void attach_fd(int fd);					// 先关闭原有的连接和套接字，然后连接到指定的socket描述符
	void detach_fd();						// 仅将Socket对象的描述符设置为无效
	void shutdown_fd(int how = SHUT_RDWR);	// 关闭连接
	
	// 属性
	bool get_fd(int& fd) const;
	bool getlocalsock(struct sockaddr* localaddr, int* addrlen = NULL) const;
	bool getpeersock(struct sockaddr* peeraddr, int* addrlen = NULL) const;
	
	static struct hostent* gethost(const char* name) { return ::gethostbyname(name); }
	static struct hostent* gethost(const char* addr, int len, int family = AF_INET) 
	{ return ::gethostbyaddr(addr, len, family); }
	
	static struct servent* getserv(const char* name, const char* protocol = NULL) 
	{ return ::getservbyname(name, protocol); }
	static struct servent* getserv(int port, const char* protocol = NULL) 
	{ return ::getservbyport(port, protocol); }
	
	// 操作
	void reuse_addr(bool reuse = true);
	
	bool bind(struct sockaddr* addr, int addrlen = sizeof(struct sockaddr));
	bool bind(unsigned short port = 0, const char* addr = NULL, short family = AF_INET);
	
	bool listen(int backlog = MAX_REQUEST);
	
	bool accept(int& connected_fd, struct sockaddr* addr = NULL, int* addrlen = NULL);
	bool accept(Socket& sock, struct sockaddr* addr = NULL, int* addrlen = NULL);
	
	bool connect(struct sockaddr* addr, int addrlen = sizeof(struct sockaddr));
	bool connect(const char* addr, unsigned short port, short family = AF_INET);
	
	bool send(const void* buf, size_t& size, int flags = 0) { return _send_aux(false, buf, size, flags); }
	bool recv(void* buf, size_t& size, int flags = 0) { return _recv_aux(false, buf, size, flags); }
	bool write(const void* buf, size_t& size) { return _send_aux(false, buf, size, 0); }
	bool read(void* buf, size_t& size) { return _recv_aux(false, buf, size, 0); }
	
	bool sendto(const void* buf, size_t& size, struct sockaddr* to, socklen_t addrlen = sizeof(struct sockaddr), int flags = 0)
	{ return _sendto_aux(false, buf, size, to, addrlen, flags); }
	bool sendto(const void* buf, size_t& size, unsigned short port, const char* addr, int flags = 0)
	{ return _sendto_aux(false, buf, size, port, addr, flags); }
	bool recvfrom(void* buf, size_t& size, struct sockaddr* from = NULL, socklen_t* addrlen = NULL, int flags = 0)
	{ return _recvfrom_aux(false, buf, size, from, addrlen, flags); }
	bool recvfrom(void* buf, size_t& size, unsigned short* port, char* addr, int flags = 0)
	{ return _recvfrom_aux(false, buf, size, port, addr, flags); }
	
	void set_block(bool bblock = true);
	bool get_block() { return m_bblock; }
	
private:
	bool _send_aux(bool is_const, const void* buf, size_t& size, int flags);
	bool _recv_aux(bool is_const, void* buf, size_t& size, int flags);
	bool _sendto_aux(bool is_const, const void* buf, size_t& size, struct sockaddr* to, socklen_t addrlen, int flags);
	bool _sendto_aux(bool is_const, const void* buf, size_t& size, unsigned short port, const char* addr, int flags);
	bool _recvfrom_aux(bool is_const, void* buf, size_t& size, struct sockaddr* from, socklen_t* addrlen, int flags);
	bool _recvfrom_aux(bool is_const, void* buf, size_t& size, unsigned short* port, char* addr, int flags);
	
private:
	// 禁止的操作
	Socket(const Socket& rhs);
	Socket& operator =(const Socket& rhs);
	
protected:
	// 数据成员
	int m_socket;
	bool m_bblock;
};

} // namespace TK_Tools {

#endif // #ifndef _SOCK_H_JKL23895U23N524NM235R90124H1U238XHY901P4J3HY845_
