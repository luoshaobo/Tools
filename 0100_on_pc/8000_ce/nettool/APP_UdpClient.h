#ifndef APP_UDP_CLIENT_H__778491290478272174289478234234823472384823423823884
#define APP_UDP_CLIENT_H__778491290478272174289478234234823472384823423823884

#include "TK_sock.h"
#include "stdafx.h"
#include "SMF_afx.h"
#include "APP_afx.h"
#include "SMF_BaseThread.h"
#include "NT_EventHandlerImpl.hpp"

class UdpClient;

interface UdpClientHandler : public SMF_BaseInterface
{
    virtual int OnUdpClientReceivedContent(UdpClient &rUdpClient, const std::vector<char> &arrContent) = 0;
};

class UdpClient 
    : public SMF_ThreadHandler
    , public UdpClientHandler
    , public APP_IOHandler
{
public:
    UdpClient(const std::string &sServerAddr, unsigned short nServerPort);
    virtual ~UdpClient();

public:
    bool Start();
    bool Stop();

    int SendContentToPeer(const std::vector<char> &arrContent);

public:
    void SetClientHandler(UdpClientHandler *pUdpClientHandler);
    UdpClientHandler *GetClientHandler();

private:
    SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);

private:
    virtual int OnUdpClientReceivedContent(UdpClient &rUdpClient, const std::vector<char> &arrContent);
    virtual int LocalOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);
    virtual int RemoteOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);

private:
    TK_Tools::Socket m_oSocket;
    std::string m_sServerAddr;
    unsigned short m_nServerPort;
    SMF_BaseThread m_oClientThread;
    UdpClientHandler *m_pClientHandler;
    NT_EventHandlerImpl *m_pEh;
};

#endif // #ifndef APP_UDP_CLIENT_H__778491290478272174289478234234823472384823423823884
