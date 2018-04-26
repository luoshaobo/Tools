// StateMachineFramework.cpp : Defines the entry point for the console application.
//

#include "TK_sock.h"
#include "stdafx.h"
#include <string>
#include "getopt.h"
#include "APP_TcpServer.h"
#include "APP_UdpServer.h"
#include "APP_TcpClient.h"
#include "APP_UdpClient.h"
#include "APP_afx.h"

using namespace TK_Tools;

char **argv_new = NULL;

static struct option longopts[] =
{
    { "server", no_argument, NULL, 's' },
    { "udp", no_argument, NULL, 'u' },
    { "server-port", required_argument, NULL, 'p' },
    { "help", no_argument, NULL, 'H' },
    { NULL, 0, NULL, 0 }
};

static char opts[] = "sup:H";

void usage(int argc, char* argv[])
{
    printf("usage:\n");
    printf("  %s [OPTION] [<server_addr>]\n", argv[0]);
    printf("    -s, --server                    Server mode. [Default: client mode]\n");
    printf("    -u, --udp                       UDP. [Default: TCP]\n");
    printf("    -p<n>, --server-port=<n>        Server listening port. [Default: 23]\n");
    printf("    -H, --help                      Help information.\n");
    printf("    <server_addr>                   Server address. Needed on client mode. [Default: 127.0.0.1]\n");
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

int main(int argc, char* argv[])
{
    int nRet = -1;
    int g;
    bool bIsServer = false;
    bool bIsTcp = true;
    unsigned nPort = 23;

    InitSock();

    while ((g = getopt_long (argc, argv, opts, longopts, NULL)) != EOF)
    switch (g)
    {
        case 's':
            {
                bIsServer = true;
            }
            break;
        case 'u':
            {
                bIsTcp = false;
            }
            break;
        case 'p':
            {
                nPort = (unsigned short)StrToUL(optarg);
            }
            break;
        case 'H':
            {
                usage(argc, argv);
                return 0;
            }
            break;
        default :
            {
                fprintf (stderr, "Try `%s --help' for more information.\n", argv[0]);
                return 1;
            }
            break;
    }

    if ((argv[optind] == NULL) || (argv[optind + 1] == NULL)) {
        // 
    } else {
        fprintf (stderr, "Try `%s --help' for more information.\n", argv[0]);
        return 1;
    }

    InitStateMachines();

    argv_new = argv + optind;

    if (bIsServer) {
        TcpServer oTcpServer(nPort);
        UdpServer oUdpServer(nPort);

        if (bIsTcp) {    
            oTcpServer.Start();
        } else {
            oUdpServer.Start();
        }

        printf("NOTE: You can enter \"!quit\" followed by a CR to exit the server.\n");
        while (true) {
            char arrLine[1024];
            char *p;
            p = fgets(arrLine, sizeof(arrLine), stdin);
            if (p != NULL) {
                std::string sLine(p);
                sLine = TrimAll(sLine);
                if (sLine == "\x03" || sLine == "!quit") {
                    break;
                }
                
                {
                    sLine += "\r\n";
                    if (bIsTcp) {    
                        oTcpServer.SendContentToPeer(std::vector<char>(sLine.c_str(), sLine.c_str() + sLine.length()));
                    } else {
                        oUdpServer.SendContentToPeer(std::vector<char>(sLine.c_str(), sLine.c_str() + sLine.length()));
                    }
                }
            }
        }

        if (bIsTcp) {    
            oTcpServer.Stop();
        } else {
            oUdpServer.Stop();
        }
    } else {    // client
        std::string sServerAddr = "127.0.0.1";
        if (argv_new[0] != NULL) {
            sServerAddr = argv_new[0];
        }

        TcpClient oTcpClient(sServerAddr, nPort);
        UdpClient oUdpClient(sServerAddr, nPort);

        if (bIsTcp) {    
            oTcpClient.Start();
        } else {
            oUdpClient.Start();
        }

        printf("NOTE: You can enter \"!quit\" followed by a CR to exit the client.\n");
        while (true) {
            char arrLine[1024];
            char *p;
            p = fgets(arrLine, sizeof(arrLine), stdin);
            if (p != NULL) {
                std::string sLine(p);
                sLine = TrimAll(sLine);
                if (sLine == "\x03" || sLine == "!quit") {
                    break;
                }
                
                {
                    sLine += "\r\n";
                    if (bIsTcp) {    
                        oTcpClient.SendContentToPeer(std::vector<char>(sLine.c_str(), sLine.c_str() + sLine.length()));
                    } else {
                        oUdpClient.SendContentToPeer(std::vector<char>(sLine.c_str(), sLine.c_str() + sLine.length()));
                    }
                }
            }
        }

        if (bIsTcp) {    
            oTcpClient.Stop();
        } else {
            oUdpClient.Stop();
        }
    }

	return nRet;
}
