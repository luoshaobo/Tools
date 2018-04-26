#include <windows.h>
#include "TK_DebugServer.h"
#include "TK_sock.h"
#include "TK_Tools.h"

#define PORT_BASE				1059

extern bool NAVSIM_ProcessCmdLine(const char *pCmdLine);
extern bool TLMSIM_ProcessCmdLine(const char *pCmdLine);
extern bool AVSIM_ProcessCmdLine(const char *pCmdLine);
extern bool SYSCONFSIM_ProcessCmdLine(const char *pCmdLine);
extern bool SNSSIM_ProcessCmdLine(const char *pCmdLine);

bool TKDS_ProcessCmdLineDef(const char *pCmdLine) { return false; }

#pragma comment(linker, "/alternatename:?TLMSIM_ProcessCmdLine@@YA_NPBD@Z=?TKDS_ProcessCmdLineDef@@YA_NPBD@Z")
#pragma comment(linker, "/alternatename:?AVSIM_ProcessCmdLine@@YA_NPBD@Z=?TKDS_ProcessCmdLineDef@@YA_NPBD@Z")
#pragma comment(linker, "/alternatename:?SYSCONFSIM_ProcessCmdLine@@YA_NPBD@Z=?TKDS_ProcessCmdLineDef@@YA_NPBD@Z")
#pragma comment(linker, "/alternatename:?SNSSIM_ProcessCmdLine@@YA_NPBD@Z=?TKDS_ProcessCmdLineDef@@YA_NPBD@Z")

namespace TK_Tools {

static DebugServer& g_rDefDebugServer = DebugServer::GetInstance();

DebugServer& DebugServer::GetInstance(DebugServerInstanceId id /*= DSIID_DEF*/)
{
    static DebugServer dsNavi(PORT_BASE + DSIID_NAVI, &NAVSIM_ProcessCmdLine);
    static DebugServer dsTlm(PORT_BASE + DSIID_TLM, &TLMSIM_ProcessCmdLine);
    static DebugServer dsVr(PORT_BASE + DSIID_AV, &AVSIM_ProcessCmdLine);
    static DebugServer dsSysConf(PORT_BASE + DSIID_SYSCONF, &SYSCONFSIM_ProcessCmdLine);
    static DebugServer dsSns(PORT_BASE + DSIID_SNS, &SNSSIM_ProcessCmdLine);

    switch (id) {
    case DSIID_NAVI:
    default:
        {
            return dsNavi;
        }
        break;

    case DSIID_TLM:
        {   
            return dsTlm;
        }
        break;

    case DSIID_AV:
        {   
            return dsVr;
        }
        break;
    case DSIID_SYSCONF:
        {   
            return dsSysConf;
        }
        break;
    case DSIID_SNS:
        {   
            return dsSns;
        }
        break;
    }
}

bool SendTextToPeer1(const char *pFormat, ...)
{
    if (pFormat == NULL) {
        return true;
    }
    
#define FORMAT_BUF_SIZE                         4096
    int nSize = 0;
    char *buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        return false;
    }
    AutoFreeArrayPtr<char> afaBuff(buff);
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    return DebugServer::GetInstance(DSIID_DEF).SendTextToPeer1(buff);
}

bool SendTextToPeer2(const char *pFormat, ...)
{
    if (pFormat == NULL) {
        return true;
    }
    
#define FORMAT_BUF_SIZE                         4096
    int nSize = 0;
    char *buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        return false;
    }
    AutoFreeArrayPtr<char> afaBuff(buff);
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    return DebugServer::GetInstance(DSIID_DEF).SendTextToPeer2(buff);
}

DebugServer::DebugServer(unsigned short nServerPort, ProcessCmdLineFunc_t *pProcessCmdLineFunc) :
    m_nServerPort(nServerPort), m_pProcessCmdLineFunc(pProcessCmdLineFunc)
    , m_bConsole1Enabled(true), m_bConsole2Enabled(true)
{
    memset(&m_peer1Info, 0, sizeof(m_peer1Info));
    memset(&m_peer2Info, 0, sizeof(m_peer2Info));
	
    StartServer();
}

DebugServer::~DebugServer()
{
}

bool DebugServer::SendTextToPeer(const PeerInfo &peerInfo, const char *pFormat, ...)
{
    if (peerInfo.addrBufSize == 0)
    {
        return false;
    }

    if (pFormat == NULL) {
        return true;
    }

#define FORMAT_BUF_SIZE                         4096
    int nSize = 0;
    char *buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        return false;
    }
    AutoFreeArrayPtr<char> afaBuff(buff);
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    Socket sock;
    sock.create(SOCK_DGRAM);
    size_t len = strlen(buff);
    bool ret = false;
    
    if (peerInfo.addrBufSize != 0) {
        ret = sock.sendto(buff, len, (struct sockaddr*)&peerInfo.addrBuf, peerInfo.addrBufSize);
    }
    return ret;
}

bool DebugServer::SendTextToPeer1(const char *pFormat, ...)
{
    if (m_peer1Info.addrBufSize == 0)
    {
        return false;
    }

    if (pFormat == NULL) {
        return true;
    }

#define FORMAT_BUF_SIZE                         4096
    int nSize = 0;
    char *buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        return false;
    }
    AutoFreeArrayPtr<char> afaBuff(buff);
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    Socket sock;
    sock.create(SOCK_DGRAM);
    size_t len = strlen(buff);
    bool ret = false;
    
    if (m_peer1Info.addrBufSize != 0) {
        ret = sock.sendto(buff, len, (struct sockaddr*)&m_peer1Info.addrBuf, m_peer1Info.addrBufSize);
    }
    return ret;
}

bool DebugServer::SendTextToPeer2(const char *pFormat, ...)
{
    if (m_peer2Info.addrBufSize == 0)
    {
        return false;
    }

    if (pFormat == NULL) {
        return true;
    }

#define FORMAT_BUF_SIZE                         4096
    int nSize = 0;
    char *buff = new char[FORMAT_BUF_SIZE];
    if (buff == NULL) {
        return false;
    }
    AutoFreeArrayPtr<char> afaBuff(buff);
    memset(buff, 0, FORMAT_BUF_SIZE);
    va_list args;
    va_start(args, pFormat);
    nSize = _vsnprintf(buff, FORMAT_BUF_SIZE - 1, pFormat, args);
    va_end(args);
#undef FORMAT_BUF_SIZE

    Socket sock;
    sock.create(SOCK_DGRAM);
    size_t len = strlen(buff);
    bool ret = false;

    if (m_peer2Info.addrBufSize != 0) {
        ret = sock.sendto(buff, len, (struct sockaddr*)&m_peer2Info.addrBuf, m_peer2Info.addrBufSize);
    }
    
    return ret;
}

void DebugServer::StartServer()
{
    DWORD dwThreadId;
    HANDLE hThread = CreateThread(NULL, 0, &ServiceThreadProcWin, this, 0, &dwThreadId);
    CloseHandle(hThread);
}

bool DebugServer::ParseSockAddrInfo(const PeerInfo& peerInfo, std::string &strAddr, unsigned short &nPort)
{
    strAddr = "(unknown_addr)";
    nPort = 0;

    if (peerInfo.addrBufSize == 0) {
        return false;
    }

    const sockaddr_in *sin = (const sockaddr_in *)&peerInfo.addrBuf;
    strAddr = FormatStr("%u.%u.%u.%u", 
        sin->sin_addr.S_un.S_un_b.s_b1,
        sin->sin_addr.S_un.S_un_b.s_b2,
        sin->sin_addr.S_un.S_un_b.s_b3,
        sin->sin_addr.S_un.S_un_b.s_b4
    );
    nPort = ntohs(sin->sin_port);

    return true;
}

DWORD DebugServer::ServiceThreadProc()
{
#define MAX_BUF_SIZE		1024
#define MAX_STR_LEN			255

	char buf[MAX_BUF_SIZE];
	PeerInfo peerInfo;

#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        int n = WSAGetLastError();
        TK_UNUSED_VAR(n);
        printf("Error at WSAStartup()\n");
        return static_cast<DWORD>(-1);
    }
#endif // #ifdef WIN32
	
	Socket sock;
	if (!sock.create(SOCK_DGRAM))
	{
		printf("Fatal Error: sock.create() failed!\n");
		return static_cast<DWORD>(-1);
	}
	sock.reuse_addr();
	if (!sock.bind(m_nServerPort))
	{
		printf("Fatal Error: sock.bind() failed!\n");
		return static_cast<DWORD>(-1);
	}	

	for (;;)
	{
		size_t size = sizeof(buf);
		
        socklen_t peerSockaddrBufSizeTemp = sizeof(peerInfo.addrBuf);
		if (!sock.recvfrom(buf, size, (struct sockaddr*)&peerInfo.addrBuf, &peerSockaddrBufSizeTemp))
		{
			printf("Error: sock.recvfrom() error!\n");
			continue;
		}
        peerInfo.addrBufSize = peerSockaddrBufSizeTemp;

        while (true) {
		    if (size > 1 && (buf[size - 1] == '\r' || buf[size - 1] == '\n')) {
                size--;
            } else {
                break;
            }
        }

		buf[size] = '\0';
		printf("Server Recv: %s\n", buf);

        if (CompareNoCase(buf, "open console 1") == 0) {
            memcpy(&m_peer1Info, &peerInfo, sizeof(peerInfo));
            m_bConsole1Enabled = true;
            SendTextToPeer(peerInfo, "This console is set to console 1.\n");
        } else if (CompareNoCase(buf, "open console 2") == 0) {
            memcpy(&m_peer2Info, &peerInfo, sizeof(peerInfo));
            m_bConsole2Enabled = true;
            SendTextToPeer(peerInfo, "This console is set to console 2.\n");
        } else if (CompareNoCase(buf, "close console 1") == 0) {
            SendTextToPeer(peerInfo, "Console 1 is closed.\n");
            m_peer1Info.addrBufSize = 0;
            m_bConsole1Enabled = false;
        } else if (CompareNoCase(buf, "close console 2") == 0) {
            SendTextToPeer(peerInfo, "Console 2 is closed.\n");
            m_peer2Info.addrBufSize = 0;
            m_bConsole2Enabled = false;
        } else if (CompareNoCase(buf, "query console") == 0) {
            std::string strAddr;
            unsigned short nPort;
            bool bSuc;

            bSuc = ParseSockAddrInfo(m_peer1Info, strAddr, nPort);
            if (!bSuc) {
                SendTextToPeer(peerInfo, "Console 1: (unknown).\n");
            } else {
                SendTextToPeer(peerInfo, "Console 1: connected to %s:%u.\n", strAddr.c_str(), nPort);
            }
        
            bSuc = ParseSockAddrInfo(m_peer2Info, strAddr, nPort);
            if (!bSuc) {
                SendTextToPeer(peerInfo, "Console 2: (unknown).\n");
            } else {
                SendTextToPeer(peerInfo, "Console 2: connected to %s:%u.\n", strAddr.c_str(), nPort);
            }
        } else {
            if (m_peer1Info.addrBufSize == 0 && m_bConsole1Enabled) {
                memcpy(&m_peer1Info, &peerInfo, sizeof(peerInfo));
                SendTextToPeer(peerInfo, "This console is set to console 1 automatically.\n");
            } else if (m_peer2Info.addrBufSize == 0 && m_bConsole2Enabled) {
                memcpy(&m_peer2Info, &peerInfo, sizeof(peerInfo));
                SendTextToPeer(peerInfo, "This console is set to console 2 automatically.\n");
            }

            if (CompareNoCase(buf, "~!@#$magic%^&*(") == 0) {
                // do nothing
            } else if (CompareNoCaseRightLen(buf, "echo ") == 0) {
                SendTextToPeer(peerInfo, "%s\n", std::string(buf).substr(strlen("echo ")).c_str());
            } else {
                if (m_pProcessCmdLineFunc != NULL) {
                    if (m_pProcessCmdLineFunc(buf)) {
                        // do nothing
                    } else {
                        // do nothing
                    }
                } else {
                    // do nothing
                }
            }
        }
    }
    
    return 0;
}

DWORD WINAPI DebugServer::ServiceThreadProcWin(void *arg)
{
    DebugServer *pThis = (DebugServer *)arg;
    if (pThis != NULL) {
        return pThis->ServiceThreadProc();
    } else {
        return (DWORD)-1;
    }
}

} // namespace TK_Tools {
