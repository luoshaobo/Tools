#ifndef APP_UDP_SERVER_H__782374912940235728409235782349390230423478234723478
#define APP_UDP_SERVER_H__782374912940235728409235782349390230423478234723478

#include "TK_sock.h"
#include "stdafx.h"
#include "SMF_afx.h"
#include "APP_afx.h"
#include "SMF_BaseThread.h"
#include "SMF_PlatformLock.h"
#include "NT_EventHandlerImpl.hpp"

class UdpServer;

interface UdpServerHandler : public SMF_BaseInterface
{
    virtual int OnUdpServerReceivedContent(UdpServer &rUdpServer, const std::vector<char> &arrContent) = 0;
};

class UdpServer 
    : public SMF_ThreadHandler
    , public UdpServerHandler
    , public APP_IOHandler
{
public:
    UdpServer(unsigned short nServerPort);
    virtual ~UdpServer();

public:
    bool Start();
    bool Stop();

    int SendContentToPeer(const std::vector<char> &arrContent);

public:
    void SetServerHandler(UdpServerHandler *pUdpServerHandler);
    UdpServerHandler *GetServerHandler();

private:
    SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);

private:
    virtual int OnUdpServerReceivedContent(UdpServer &rUdpServer, const std::vector<char> &arrContent);
    virtual int LocalOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);
    virtual int RemoteOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);

private:
    TK_Tools::Socket m_oSocket;
    unsigned short m_nServerPort;
    SMF_BaseThread m_oServerThread;
    UdpServerHandler *m_pServerHandler;

    bool m_bPeerSockAvailable;
    unsigned int m_arrPeerSockaddrBuf[100];
    struct sockaddr *m_pPeerSockAddr;
    socklen_t m_nPeerSockAddrLen;
    NT_EventHandlerImpl *m_pEh;
};

#endif // #ifndef APP_UDP_SERVER_H__782374912940235728409235782349390230423478234723478
