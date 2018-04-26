#ifndef APP_TCP_CLIENT_H__738910278589127423476124776127847612748237617843873
#define APP_TCP_CLIENT_H__738910278589127423476124776127847612748237617843873

#include "TK_sock.h"
#include "stdafx.h"
#include "SMF_afx.h"
#include "SMF_BaseThread.h"

class TcpClient;

interface TcpClientHandler : public SMF_BaseInterface
{
    virtual int OnTcpClientReceivedContent(TcpClient &rTcpClient, const std::vector<char> &arrContent) = 0;
    virtual int OnConnectionToServerFailed(TcpClient &rTcpClient) = 0;
    virtual int OnConnectionClosedByServer(TcpClient &rTcpClient) = 0;
};

class TcpClient : public SMF_ThreadHandler, public TcpClientHandler
{
public:
    TcpClient(const std::string &sServerAddr, unsigned short nServerPort);
    virtual ~TcpClient();

public:
    bool Start();
    bool Stop();

    int SendContentToPeer(const std::vector<char> &arrContent);

public:
    void SetClientHandler(TcpClientHandler *pTcpClientHandler);
    TcpClientHandler *GetClientHandler();

private:
    SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);
    int OnTcpClientReceivedContent(TcpClient &rTcpClient, const std::vector<char> &arrContent);
    int OnConnectionToServerFailed(TcpClient &rTcpClient);
    int OnConnectionClosedByServer(TcpClient &rTcpClient);

private:
    TK_Tools::Socket m_oSocket;
    std::string m_sServerAddr;
    unsigned short m_nServerPort;
    SMF_BaseThread m_oClientThread;
    TcpClientHandler *m_pClientHandler;
};

#endif // #ifndef APP_TCP_CLIENT_H__738910278589127423476124776127847612748237617843873
