#include "APP_UdpClient.h"

UdpClient::UdpClient(const std::string &sServerAddr, unsigned short nServerPort)
    : m_oSocket()
    , m_sServerAddr(sServerAddr)
    , m_nServerPort(nServerPort)
    , m_oClientThread("UdpClientThread")
    , m_pClientHandler(NULL)
{
    m_oClientThread.SetThreadHandler(this);
    m_pClientHandler = this;
}

UdpClient::~UdpClient()
{
    Stop();
}

bool UdpClient::Start()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oClientThread.StartThread() == SMF_ERR_OK);
    }

    return bRet;
}

bool UdpClient::Stop()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oClientThread.StopThread() == SMF_ERR_OK);
    }

    return bRet;
}

void UdpClient::SetClientHandler(UdpClientHandler *pUdpClientHandler)
{
    m_oClientThread.Lock();
    m_pClientHandler = pUdpClientHandler;
    m_oClientThread.Unlock();
}

UdpClientHandler *UdpClient::GetClientHandler()
{
    UdpClientHandler *pUdpClientHandler = this;

    m_oClientThread.Lock();
    if (m_pClientHandler != NULL) {
        pUdpClientHandler = m_pClientHandler;
    }
    m_oClientThread.Unlock();

    return pUdpClientHandler;
}

SMF_ErrorCode UdpClient::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bSuc;
    char arrBuf[4096];
    UdpClientHandler *pUdpClientHandler;

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

    TK_MSG("The server is [%s:%u].\n", m_sServerAddr.c_str(), m_nServerPort);

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
                Sleep(50);
                continue;
            } else if (nRet == 0) { // timeout
                continue;
            }

            bSuc = m_oSocket.recvfrom(arrBuf, nSize, from, &addrlen);
            if (!bSuc) {
                continue;
            }
            if (nSize == 0) {
                continue;
            }

            pUdpClientHandler = GetClientHandler();
            if (pUdpClientHandler != NULL) {
                pUdpClientHandler->OnUdpClientReceivedContent(*this, std::vector<char>((char *)arrBuf, (char *)arrBuf + nSize));
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

int UdpClient::SendContentToPeer(const std::vector<char> &arrContent)
{
    int nRet = arrContent.size();
    size_t nSize = arrContent.size();
    bool bSuc;

    bSuc = m_oSocket.sendto(&arrContent[0], nSize, m_nServerPort, m_sServerAddr.c_str());
    if (!bSuc) {
        nRet = -1;
    }
    
    return nRet;
}

int UdpClient::OnUdpClientReceivedContent(UdpClient &rUdpClient, const std::vector<char> &arrContent)
{
    std::string sConent(&arrContent[0], arrContent.size());

    printf(sConent.c_str());

    return arrContent.size();
}
