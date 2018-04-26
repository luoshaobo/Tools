#ifndef APP_TCP_SERVER_H__784589230489378523894347892359278348953474238578478
#define APP_TCP_SERVER_H__784589230489378523894347892359278348953474238578478

#include "TK_sock.h"
#include "stdafx.h"
#include "SMF_afx.h"
#include "APP_afx.h"
#include "SMF_BaseThread.h"
#include "SMF_PlatformLock.h"
#include "NT_EventHandlerImpl.hpp"

class TcpServer;

interface TcpServerHandler : public SMF_BaseInterface
{
    virtual int OnTcpServerReceivedContent(TcpServer &rTcpServer, const std::vector<char> &arrContent) = 0;
    virtual int OnConnectionClosedByServer(TcpServer &rTcpServer) = 0;
};

class TcpServer 
    : public SMF_ThreadHandler
    , public TcpServerHandler
    , public APP_IOHandler
{
public:
    TcpServer(unsigned short nServerPort);
    virtual ~TcpServer();

public:
    bool Start();
    bool Stop();

    int SendContentToPeer(const std::vector<char> &arrContent);

public:
    void SetServerHandler(TcpServerHandler *pTcpServerHandler);
    TcpServerHandler *GetServerHandler();

private:
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);
    virtual int OnTcpServerReceivedContent(TcpServer &rTcpServer, const std::vector<char> &arrContent);
    virtual int OnConnectionClosedByServer(TcpServer &rTcpServer);
    virtual int LocalOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);
    virtual int RemoteOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData);

private:
    SMF_ErrorCode ThreadProc_Server(SMF_BaseThread &rThread);
    SMF_ErrorCode ThreadProc_Sub(SMF_BaseThread &rThread);

private:
    SMF_BaseThread *NewThreadFromPool();
    bool IsThreadInPool(SMF_BaseThread *pThread);
    bool DeleteThreadFromPool(SMF_BaseThread *pThread);
    bool DeleteAllThreadsFromPool();

    void InsertToEhPool(int nFd, NT_EventHandlerImpl *pEh);
    void RemoveFromEhPool(int nFd);
    NT_EventHandlerImpl *FindFromEhPool(int nFd);

private:
    TK_Tools::Socket m_oSocket;
    int m_nLastSubSockFd;
    unsigned short m_nServerPort;
    SMF_BaseThread m_oServerThread;
    std::vector<SMF_BaseThread *> m_oSubThreadsPool;
    TcpServerHandler *m_pServerHandler;
    std::map<int, NT_EventHandlerImpl *> m_mapFdToEh;
    SMF_PlatformLock m_oFdToEhLock;
};

#endif // #ifndef APP_TCP_SERVER_H__784589230489378523894347892359278348953474238578478
