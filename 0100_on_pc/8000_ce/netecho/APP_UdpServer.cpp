#include "APP_UdpServer.h"

UdpServer::UdpServer(unsigned short nServerPort)
    : m_oSocket()
    , m_nServerPort(nServerPort)
    , m_oServerThread("UdpServerThread")
    , m_pServerHandler(NULL)
    , m_bPeerSockAvailable(false)
    //, m_arrPeerSockaddrBuf()
    , m_pPeerSockAddr(NULL)
    , m_nPeerSockAddrLen(0)
{
    m_oServerThread.SetThreadHandler(this);
    m_pServerHandler = this;

    memset(m_arrPeerSockaddrBuf, 0, sizeof(m_arrPeerSockaddrBuf));
    m_pPeerSockAddr = (struct sockaddr *)m_arrPeerSockaddrBuf;
    m_nPeerSockAddrLen = sizeof(m_arrPeerSockaddrBuf);
}

UdpServer::~UdpServer()
{
    Stop();
}

bool UdpServer::Start()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oServerThread.StartThread() == SMF_ERR_OK);
    }

    return bRet;
}

bool UdpServer::Stop()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oServerThread.StopThread() == SMF_ERR_OK);
    }

    return bRet;
}

void UdpServer::SetServerHandler(UdpServerHandler *pUdpServerHandler)
{
    m_oServerThread.Lock();
    m_pServerHandler = pUdpServerHandler;
    m_oServerThread.Unlock();
}

UdpServerHandler *UdpServer::GetServerHandler()
{
    UdpServerHandler *pUdpServerHandler = this;

    m_oServerThread.Lock();
    if (m_pServerHandler != NULL) {
        pUdpServerHandler = m_pServerHandler;
    }
    m_oServerThread.Unlock();

    return pUdpServerHandler;
}

SMF_ErrorCode UdpServer::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bSuc;
    char arrBuf[4096];
    UdpServerHandler *pUdpServerHandler;

    // NOTE: add some code here to create some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);
    
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);
    // NOTE: add some code here to activate some resources.
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);

    if (nErrorCode == SMF_ERR_OK) {
        bSuc = m_oSocket.create(SOCK_DGRAM);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        TK_MSG("Wait on [UDP:%s:%u] ...\n", "0.0.0.0", m_nServerPort);
        m_oSocket.reuse_addr();
        bSuc = m_oSocket.bind(m_nServerPort);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            int nRet;
            int fdMe = -1;
            int nfds;
            fd_set fdsetRead;
            struct timeval timeout;

            size_t nSize = sizeof(arrBuf);
            unsigned int sockaddrbuf[100] = { 0 };
            struct sockaddr *from = (struct sockaddr *)sockaddrbuf;
            socklen_t addrlen = sizeof(sockaddrbuf);

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
                m_oServerThread.Lock();
                m_bPeerSockAvailable = false;
                m_oServerThread.Unlock();
                Sleep(50);
                continue;
            } else if (nRet == 0) { // timeout
                continue;
            }

            bSuc = m_oSocket.recvfrom(arrBuf, nSize, from, &addrlen);
            if (!bSuc) {
                TK_ERR("*** ERROR: [%s()] m_oSocket.recvfrom() failed!\n", __FUNCTION__);
                m_oServerThread.Lock();
                m_bPeerSockAvailable = false;
                m_oServerThread.Unlock();
                continue;
            }
            if (nSize == 0) {
                TK_ERR("*** ERROR: [%s()] m_oSocket.recvfrom() failed!\n", __FUNCTION__);
                m_oServerThread.Lock();
                m_bPeerSockAvailable = false;
                m_oServerThread.Unlock();
                continue;
            }

            m_oServerThread.Lock();
            memcpy(m_arrPeerSockaddrBuf, sockaddrbuf, sizeof(m_arrPeerSockaddrBuf));
            m_nPeerSockAddrLen = addrlen;
            m_bPeerSockAvailable = true;
            m_oServerThread.Unlock();

            pUdpServerHandler = GetServerHandler();
            if (pUdpServerHandler != NULL) {
                pUdpServerHandler->OnUdpServerReceivedContent(*this, std::vector<char>((char *)arrBuf, (char *)arrBuf + nSize));
            }

            bSuc = m_oSocket.sendto(arrBuf, nSize, from, addrlen);
            if (!bSuc) {
                TK_ERR("*** ERROR: [%s()] m_oSocket.sendto() failed!\n", __FUNCTION__);
                m_oServerThread.Lock();
                m_bPeerSockAvailable = false;
                m_oServerThread.Unlock();
                continue;
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

int UdpServer::SendContentToPeer(const std::vector<char> &arrContent)
{
    int nRet = arrContent.size();
    size_t nSize = arrContent.size();
    bool bSuc;

    m_oServerThread.Lock();
    if (m_bPeerSockAvailable) {
        bSuc = m_oSocket.sendto(&arrContent[0], nSize, m_pPeerSockAddr, m_nPeerSockAddrLen);
        if (!bSuc) {
            nRet = -1;
        }
    } else {
        nRet = -1;
    }
    m_oServerThread.Unlock();

    return nRet;
}

int UdpServer::OnUdpServerReceivedContent(UdpServer &rUdpServer, const std::vector<char> &arrContent)
{
    std::string sConent(&arrContent[0], arrContent.size());

    printf(sConent.c_str());

    return arrContent.size();
}
