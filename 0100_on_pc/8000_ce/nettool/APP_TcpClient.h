#ifndef APP_TCP_CLIENT_H__738910278589127423476124776127847612748237617843873
#define APP_TCP_CLIENT_H__738910278589127423476124776127847612748237617843873

#include "TK_sock.h"
#include "stdafx.h"
#include "SMF_afx.h"
#include "APP_afx.h"
#include "SMF_BaseThread.h"
#include "NT_EventHandlerImpl.hpp"

class TcpClient;

interface TcpClientHandler : public SMF_BaseInterface
{
    virtual int OnTcpClientReceivedContent(TcpClient &rTcpClient, const std::vector<char> &arrContent) = 0;
    virtual int OnConnectionToServerFailed(TcpClient &rTcpClient) = 0;
    virtual int OnConnectionClosedByServer(TcpClient &rTcpClient) = 0;
};

class TcpClient 
    : public SMF_ThreadHandler
    , public TcpClientHandler
    , public APP_IOHandler
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
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);
    virtual int OnTcpClientReceivedContent(TcpClient &rTcpClient, const std::vector<char> &arrContent);
    virtual int OnConnectionToServerFailed(TcpClient &rTcpClient);
    virtual int OnConnectionClosedByServer(TcpClient &rTcpClient);
    virtual int LocalOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);
    virtual int RemoteOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);

private:
    TK_Tools::Socket m_oSocket;
    std::string m_sServerAddr;
    unsigned short m_nServerPort;
    SMF_BaseThread m_oClientThread;
    TcpClientHandler *m_pClientHandler;
    NT_EventHandlerImpl *m_pEh;
};

#endif // #ifndef APP_TCP_CLIENT_H__738910278589127423476124776127847612748237617843873
