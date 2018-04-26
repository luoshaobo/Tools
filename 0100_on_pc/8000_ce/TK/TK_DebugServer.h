#ifndef TK_DEBUG_SERVER_H__72383785235782342342347235734867846874786
#define TK_DEBUG_SERVER_H__72383785235782342342347235734867846874786

#include "TK_Tools.h"
#include "TK_sock.h"

namespace TK_Tools {

class DebugServer
{
public:
	typedef bool ProcessCmdLineFunc_t(const char *pCmdLine);
	struct PeerInfo {
		unsigned long addrBuf[50];
		socklen_t addrBufSize;
	};

public:
    ~DebugServer();
    
    static DebugServer& GetInstance(DebugServerInstanceId id = DSIID_DEF);

	bool SendTextToPeer(const PeerInfo &peerInfo, const char *pFormat, ...);
    bool SendTextToPeer1(const char *pFormat, ...);
    bool SendTextToPeer2(const char *pFormat, ...);
   
private:
    static DWORD WINAPI ServiceThreadProcWin(void *arg);
    
    void StartServer();
    DWORD ServiceThreadProc();
    
    bool ParseSockAddrInfo(const PeerInfo& peerInfo, std::string &strAddr, unsigned short &nPort);

private:
	DebugServer(unsigned short nServerPort, ProcessCmdLineFunc_t *pProcessCmdLineFunc);

private:
	unsigned short m_nServerPort;
	ProcessCmdLineFunc_t *m_pProcessCmdLineFunc;

	bool m_bConsole1Enabled;
	bool m_bConsole2Enabled;
	
    PeerInfo m_peer1Info;
    PeerInfo m_peer2Info;
};

} // namespace TK_Tools {

#endif // #ifndef TK_DEBUG_SERVER_H__72383785235782342342347235734867846874786
