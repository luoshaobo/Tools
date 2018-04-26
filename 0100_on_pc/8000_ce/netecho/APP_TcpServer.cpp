#include "APP_TcpServer.h"
#include <algorithm>

TcpServer::TcpServer(unsigned short nServerPort)
    : m_oSocket()
    , m_nLastSubSockFd(-1)
    , m_nServerPort(nServerPort)
    , m_oServerThread("TcpServerThread")
    , m_oSubThreadsPool()
    , m_pServerHandler(NULL)
{
    m_oServerThread.SetThreadHandler(this);
    m_pServerHandler = this;
}

TcpServer::~TcpServer()
{
    Stop();
}

bool TcpServer::Start()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oServerThread.StartThread() == SMF_ERR_OK);
    }

    return bRet;
}

bool TcpServer::Stop()
{
    bool bRet = true;

    if (bRet) {
        DeleteAllThreadsFromPool();
        bRet = (m_oServerThread.StopThread() == SMF_ERR_OK);
    }

    return bRet;
}

void TcpServer::SetServerHandler(TcpServerHandler *pTcpServerHandler)
{
    m_oServerThread.Lock();
    m_pServerHandler = pTcpServerHandler;
    m_oServerThread.Unlock();
}

TcpServerHandler *TcpServer::GetServerHandler()
{
    TcpServerHandler *pTcpServerHandler = this;

    m_oServerThread.Lock();
    if (m_pServerHandler != NULL) {
        pTcpServerHandler = m_pServerHandler;
    }
    m_oServerThread.Unlock();

    return pTcpServerHandler;
}

SMF_ErrorCode TcpServer::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    
    if (&rThread == &m_oServerThread) {
        nErrorCode = ThreadProc_Server(rThread);
    } else {
        nErrorCode = ThreadProc_Sub(rThread);
    }

    return nErrorCode;
}

SMF_ErrorCode TcpServer::ThreadProc_Server(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bSuc;
    char arrBuf[4096];
    TK_Tools::Socket oSocketSub;

    // NOTE: add some code here to create some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);
    
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);
    // NOTE: add some code here to activate some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);

    if (nErrorCode == SMF_ERR_OK) {
        bSuc = m_oSocket.create(SOCK_STREAM);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_oSocket.reuse_addr();
        bSuc = m_oSocket.bind(m_nServerPort);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        TK_MSG("Listen on [TCP:%s:%u] ...\n", "0.0.0.0", m_nServerPort);
        bSuc = m_oSocket.listen();
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            int fdMe = -1;
            int nfds;
            fd_set fdsetRead;
            struct timeval timeout;
            int nRet;
            size_t nSize = sizeof(arrBuf);

            if (rThread.GetThreadState() != SMF_BaseThread::TS_ACTIVATED) {
                break;
            }

            m_oSocket.get_fd(fdMe);
        
            nfds = fdMe  + 1;
            FD_ZERO(&fdsetRead);
            FD_SET(fdMe, &fdsetRead);
            timeout.tv_sec = 0;
            timeout.tv_usec = 100 * 1000;

            nRet = ::select(nfds, &fdsetRead, NULL, NULL, &timeout);
            if (nRet == SOCKET_ERROR) {
                TK_ERR("*** ERROR: [%s()] ::select() failed!\n", __FUNCTION__);
                continue;
            } else if (nRet == 0) { // timeout
                continue;
            }
            
            bSuc = m_oSocket.accept(oSocketSub);
            if (!bSuc) {
                TK_ERR("*** ERROR: [%s()] m_oSocket.accept() failed!\n", __FUNCTION__);
                continue;
            }
            
            {
                std::string sLocalAddr;
                unsigned short nLocalPort;
                std::string sPeerAddr;
                unsigned short nPeerPort;

                if (oSocketSub.GetLocalSocketAddr(sLocalAddr, nLocalPort) && oSocketSub.GetPeerSocketAddr(sPeerAddr, nPeerPort)) {
                    TK_MSG("Connection is established. [TCP:%s:%u <--> TCP:%s:%u]\n", sLocalAddr.c_str(), nLocalPort, sPeerAddr.c_str(), nPeerPort);
                }
            }

            {
                SMF_BaseThread *pSubThread = NewThreadFromPool();
                if (pSubThread != NULL) {
                    int fd = -1;
                    SMF_BaseThread::ThreadData oThreadData;
                    oSocketSub.get_fd(fd);
                    oSocketSub.detach_fd();
                    oThreadData.nInt = fd;
                    pSubThread->SetThreadData(0, oThreadData);
                    pSubThread->SetThreadHandler(this);
                    pSubThread->StartThread();
                }
            }
        }
    }

    rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);

    // NOTE: add some code here to deactivate any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVED);

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYING);
    // NOTE: add some code here to destroy any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYED);

    return nErrorCode;
}

SMF_ErrorCode TcpServer::ThreadProc_Sub(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bSuc;
    char arrBuf[4096];
    TcpServerHandler *pTcpServerHandler;
    TK_Tools::Socket oSocketSub;
    std::string sLocalAddr;
    unsigned short nLocalPort;
    std::string sPeerAddr;
    unsigned short nPeerPort;

    oSocketSub.attach_fd(rThread.GetThreadData(0).nInt);

    // NOTE: add some code here to create some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);
    
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);
    // NOTE: add some code here to activate some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);

    if (nErrorCode == SMF_ERR_OK) {
        if (!oSocketSub.GetLocalSocketAddr(sLocalAddr, nLocalPort) || !oSocketSub.GetPeerSocketAddr(sPeerAddr, nPeerPort)) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            int fdSub = -1;
            int nfds;
            fd_set fdsetRead;
            struct timeval timeout;
            int nRet;
            size_t nSize = sizeof(arrBuf);

            if (rThread.GetThreadState() != SMF_BaseThread::TS_ACTIVATED) {
                break;
            }

            oSocketSub.get_fd(fdSub);
        
            nfds = fdSub  + 1;
            FD_ZERO(&fdsetRead);
            FD_SET(fdSub, &fdsetRead);
            timeout.tv_sec = 0;
            timeout.tv_usec = 100 * 1000;

            nRet = ::select(nfds, &fdsetRead, NULL, NULL, &timeout);
            if (nRet == SOCKET_ERROR) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            } else if (nRet == 0) { // timeout
                continue;
            }

            bSuc = oSocketSub.recv(arrBuf, nSize);
            if (!bSuc) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            }
            if (nSize == 0) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            }
            oSocketSub.get_fd(m_nLastSubSockFd);

            pTcpServerHandler = GetServerHandler();
            if (pTcpServerHandler != NULL) {
                pTcpServerHandler->OnTcpServerReceivedContent(*this, std::vector<char>((char *)arrBuf, (char *)arrBuf + nSize));
            }

            bSuc = oSocketSub.send(arrBuf, nSize);
            if (!bSuc) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            }
        }

        if (nErrorCode != SMF_ERR_OK) {
            oSocketSub.shutdown_fd();
            oSocketSub.close_fd();
            TK_MSG("Connection closed by the client. [TCP:%s:%u <--> TCP:%s:%u]\n", sLocalAddr.c_str(), nLocalPort, sPeerAddr.c_str(), nPeerPort);
            GetServerHandler()->OnConnectionClosedByServer(*this);
        }
    }

    rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);

    // NOTE: add some code here to deactivate any resources.
    rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVED);

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYING);
    // NOTE: add some code here to destroy any resources.

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYED);

    oSocketSub.detach_fd();
    DeleteThreadFromPool(&rThread);

    return nErrorCode;
}

int TcpServer::SendContentToPeer(const std::vector<char> &arrContent)
{
    int nRet = arrContent.size();
    size_t nSize = arrContent.size();
    bool bSuc;
    TK_Tools::Socket oSocketSub;

    oSocketSub.attach_fd(m_nLastSubSockFd);
    bSuc = oSocketSub.send(&arrContent[0], nSize);
    if (!bSuc) {
        nRet = -1;
    }
    oSocketSub.detach_fd();

    return nRet;
}

int TcpServer::OnTcpServerReceivedContent(TcpServer &rTcpServer, const std::vector<char> &arrContent)
{
    if (arrContent.size() > 0) {
        std::string sConent(&arrContent[0], arrContent.size());
        printf(sConent.c_str());
    }

    return arrContent.size();
}

int TcpServer::OnConnectionClosedByServer(TcpServer &rTcpServer)
{
    int nRet = 0;

    return nRet;
}

SMF_BaseThread *TcpServer::NewThreadFromPool()
{
    SMF_BaseThread *pThread;

    pThread = new SMF_BaseThread("TcpServer_Sub");
    if (pThread != NULL) {
        m_oServerThread.Lock();
        m_oSubThreadsPool.push_back(pThread);
        m_oServerThread.Unlock();
    }

    return pThread;
}

bool TcpServer::IsThreadInPool(SMF_BaseThread *pThread)
{
    bool bRet = false;
    std::vector<SMF_BaseThread *>::iterator it;

    m_oServerThread.Lock();
    it = std::find(m_oSubThreadsPool.begin(), m_oSubThreadsPool.end(), pThread);
    bRet = (it != m_oSubThreadsPool.end());
    m_oServerThread.Unlock();

    return bRet;
}

bool TcpServer::DeleteThreadFromPool(SMF_BaseThread *pThread)
{
    bool bRet = false;
    std::vector<SMF_BaseThread *>::iterator it;

    m_oServerThread.Lock();
    it = std::find(m_oSubThreadsPool.begin(), m_oSubThreadsPool.end(), pThread);
    if (it != m_oSubThreadsPool.end()) {
        bRet = true;
        m_oSubThreadsPool.erase(it);
    }
    m_oServerThread.Unlock();

    if (bRet && pThread != NULL) {
        delete pThread;
    }

    return bRet;
}

bool TcpServer::DeleteAllThreadsFromPool()
{
    bool bRet = false;
    std::vector<SMF_BaseThread *> oSubThreadsPoolTemp;
    std::vector<SMF_BaseThread *>::iterator it;

    m_oServerThread.Lock();
    oSubThreadsPoolTemp = m_oSubThreadsPool;
    m_oSubThreadsPool.clear();
    m_oServerThread.Unlock();

    for (it = oSubThreadsPoolTemp.begin(); it != oSubThreadsPoolTemp.end(); ++it) {
        SMF_BaseThread *pThread = (*it);
        if (pThread != NULL) {
            delete pThread;
        }
    }

    return bRet;
}
