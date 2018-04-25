#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sock.h"
#include <process.h>

using namespace std;

#define MAX_BUF_SIZE		1024
#define MAX_STR_LEN			255

const unsigned short nReceiveServerPorts[] = {
    9501,
    9502,
    9503,
    9504,
    9505,
    9506,
    9507,
    9508,
};
unsigned int nReceiveServerPortIndex = 0;

bool bCmdSent = false;

void ReceiveProc(void *)
{
    Socket sock;
    char buf[MAX_BUF_SIZE];
    
    if (!sock.create(SOCK_DGRAM))
	{
        printf("*** Fatal Error: %s(): sock.create() failed!\n", __FUNCTION__);
		return;
	}
	sock.reuse_addr();
	if (!sock.bind(nReceiveServerPorts[nReceiveServerPortIndex]))
	{
		printf("*** Fatal Error: %s(): sock.bind(%u) failed!\n", __FUNCTION__, nReceiveServerPorts[nReceiveServerPortIndex]);
		return;
	}

    for (;;)
	{
		size_t size = sizeof(buf);
		if (!sock.recvfrom(buf, size))
		{
			printf("*** Error: %s(): sock.recvfrom() error!\n", __FUNCTION__);
			continue;
		}
		if (size > 1 && (buf[size - 1] == '\r' || buf[size - 1] == '\n')) {
            size--;
		}

		buf[size] = '\0';
        if (bCmdSent) {
            printf("\n");
            bCmdSent = false;
        }
		printf("%s\n", buf);
    }
}

int main(int argc, char* argv[])
{
#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("*** Error at WSAStartup()\n");
        return -1;
    }
#endif // #ifdef WIN32

    if (argc >= 2) {
        nReceiveServerPortIndex = atoi(argv[1]);
    }

    unsigned int nCount = sizeof(nReceiveServerPorts) / sizeof(nReceiveServerPorts[0]);
    if (nReceiveServerPortIndex >= nCount) {
        printf("*** ERROR: port index is to big!\n");
        exit(-1);
    }
    _beginthread(&ReceiveProc, 0, NULL);

    for (;;) {
        printf("user input> ");
        bCmdSent = true;

	    char buf[1024];
	    char *p = fgets(buf, sizeof(buf) - 1, stdin);
        if (p == NULL)
        {
            printf("*** Error at fgets()\n");
            continue;
        }
        size_t len = strlen(buf);

        while (true) {
            if (strlen(buf) >= 1 && (
                    buf[strlen(buf) - 1] == ' '
                    || buf[strlen(buf) - 1] == '\t'
                    || buf[strlen(buf) - 1] == '\r'
                    || buf[strlen(buf) - 1] == '\n'
                )
            ) {
                buf[strlen(buf) - 1] = '\0';
            } else {
                break;
            }
        }

        if (strlen(buf) == 0) {
            continue;
        }
    	
	    Socket sock;
	    sock.create(SOCK_DGRAM);
        sock.reuse_addr();
	    if (!sock.bind(nReceiveServerPorts[nReceiveServerPortIndex]))
	    {
		    printf("*** Fatal Error: %s(): sock.bind(%u) failed!\n", __FUNCTION__, nReceiveServerPorts[nReceiveServerPortIndex]);
		    exit(-1);
	    }
	    //sock.sendto(buf, len, 1059, "127.0.0.1");
	    sock.sendto(buf, len, 1059, "192.168.1.3");
    }

	return 0;
}
