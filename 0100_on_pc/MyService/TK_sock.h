/* Copyright (R) 2002. Hexin Software Technology Co., Ltd. All rights reserved. 
 * 
 * �ļ���:       sock.h
 * ��Ӧ�ļ�:     sock.cpp
 *
 * �ϴΰ汾�ţ�  
 * �����Ա��    
 * ������ڣ�    
 *
 * ���ΰ汾�ţ�  0.1.0
 * �����Ա��    ���ٲ�
 * ������ڣ�    2003.1.15
 *
 * ��Ҫ���ݣ�ʵ����Socket���ṩ����socket�ĳ��÷�����
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
	// ��������
	Socket();								// ����һ���յ�Socket����
	bool create(int type = SOCK_STREAM, int protocol = 0, int domain = PF_INET);
											// Ϊ�յĶ��󴴽�socket������
	~Socket();								// �ر����Ӻ��׽���
	void destroy();							// �ر����Ӻ��׽���
	
	bool open_fd(int type = SOCK_STREAM, int protocol = 0, int domain = PF_INET);	
											// �ȹر�ԭ�е����Ӻ��׽��֣�Ȼ�󴴽��µ�socket
	void close_fd();						// �ر��׽���
	void attach_fd(int fd);					// �ȹر�ԭ�е����Ӻ��׽��֣�Ȼ�����ӵ�ָ����socket������
	void detach_fd();						// ����Socket���������������Ϊ��Ч
	void shutdown_fd(int how = SHUT_RDWR);	// �ر�����
	
	// ����
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
	
	// ����
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
	// ��ֹ�Ĳ���
	Socket(const Socket& rhs);
	Socket& operator =(const Socket& rhs);
	
protected:
	// ���ݳ�Ա
	int m_socket;
	bool m_bblock;
};

} // namespace TK_Tools {

#endif // #ifndef _SOCK_H_JKL23895U23N524NM235R90124H1U238XHY901P4J3HY845_
