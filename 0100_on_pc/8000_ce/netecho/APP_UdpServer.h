#ifndef APP_UDP_SERVER_H__782374912940235728409235782349390230423478234723478
#define APP_UDP_SERVER_H__782374912940235728409235782349390230423478234723478

#include "TK_sock.h"
#include "stdafx.h"
#include "SMF_afx.h"
#include "SMF_BaseThread.h"

class UdpServer;

interface UdpServerHandler : public SMF_BaseInterface
{
    virtual int OnUdpServerReceivedContent(UdpServer &rUdpServer, const std::vector<char> &arrContent) = 0;
};

class UdpServer : public SMF_ThreadHandler, public UdpServerHandler
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
    int OnUdpServerReceivedContent(UdpServer &rUdpServer, const std::vector<char> &arrContent);

private:
    TK_Tools::Socket m_oSocket;
    unsigned short m_nServerPort;
    SMF_BaseThread m_oServerThread;
    UdpServerHandler *m_pServerHandler;

    bool m_bPeerSockAvailable;
    unsigned int m_arrPeerSockaddrBuf[100];
    struct sockaddr *m_pPeerSockAddr;
    socklen_t m_nPeerSockAddrLen;
};

#endif // #ifndef APP_UDP_SERVER_H__782374912940235728409235782349390230423478234723478
