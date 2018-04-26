// TestSockClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_BUF_SIZE		1024
//#define P(x) cout << (x) << endl
#define P(x)
#define ASSERT_NOT(x) \
	if (x) \
	{ \
		printf("Error: %d\n", WSAGetLastError()); \
		exit(1); \
	}
#define ASSERT(x) \
	if (!(x)) \
	{ \
		printf("Error: %d\n", WSAGetLastError()); \
		exit(1); \
	}

int main(int argc, char* argv[])
{
	P("WSAStartup...");
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int ret = WSAStartup(wVersion, &wsaData);
	ASSERT(ret == 0);

	struct sockaddr_in saPeer = { sizeof(struct sockaddr_in) };
	saPeer.sin_family = AF_INET;
	saPeer.sin_addr.s_addr = inet_addr("192.168.2.116");
	saPeer.sin_port = htons(1777);

	for (int i = 0; i < 10000; i++)
	{
		P("socket...");
		SOCKET sMain = socket(PF_INET, SOCK_STREAM, 0);
		ASSERT_NOT(sMain == INVALID_SOCKET);

		P("connect...");
		ret = connect(sMain, (struct sockaddr*)&saPeer, sizeof(struct sockaddr));
		ASSERT_NOT(ret == SOCKET_ERROR);

		P("send...");
		char buf[MAX_BUF_SIZE];
		_snprintf(buf, sizeof(buf), "request %d", i);
		ret = send(sMain, buf, strlen(buf), 0);
		ASSERT_NOT(ret == SOCKET_ERROR);

		P("recv...");
		ret = recv(sMain, buf, sizeof(buf), 0);
		ASSERT_NOT(ret == SOCKET_ERROR);
		buf[ret] = '\0';
		printf("%s\n", buf);

		closesocket(sMain);
	}

	WSACleanup();
	return 0;
}


