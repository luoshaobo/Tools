// StateMachineFramework.cpp : Defines the entry point for the console application.
//

#include "TK_sock.h"
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

void usage(int argc, char **argv)
{
    printf("Usage:\n");
    printf("  %s [<nCount>]\n", argv[0]);
    printf("\n");
}

int InitSock()
{
#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("*** Error at WSAStartup()\n");
        return -1;
    }
#endif // #ifdef WIN32
    return 0;
}

int main(int argc, char **argv)
{
    UINT nCount = 1;
    bool bIsSocket = 1;
    UINT i;
    
    InitSock();
    
    if (argc <= 1) {
        usage(argc, argv);
        return 0;
    }
    
    if (argc >= 2) {
        nCount = TK_Tools::StrToUL(argv[1]);
    }
    
    // if (argc >= 3) {
        // if (std::string(argv[2]) == "file") {
            // bIsSocket = false;
        // }
    // }
    
    if (nCount == 0) {
        nCount = 1;
    }
    
    printf("%s(): %lu sockets/files will be created!\n", __FUNCTION__, nCount);

    for (i = 0; i < nCount; i++) {
        if (bIsSocket) {
            TK_Tools::Socket *p = new TK_Tools::Socket();
            if (p != NULL) {
                if (!p->create()) {
                    printf("%s(): [%lu] Failed to p->create()!\n", __FUNCTION__, i);
                    break;
                }
            } else {
                printf("%s(): [%lu] Failed to new TK_Tools::Socket()!\n", __FUNCTION__, i);
            }
        } else {
            FILE *p = fopen("\\Windows\\baseline.txt", "rb");
            if (p == NULL) {
                printf("%s(): [%lu] Failed to fopen()!\n", __FUNCTION__, i);
            }
        }
    }

    printf("Enter any key to exit this process...");
    getchar();
    
    return 0;
}
