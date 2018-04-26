#include "APP_TcpClient.h"
#include "APP_EventHandlerFactory.h"

TcpClient::TcpClient(const std::string &sServerAddr, unsigned short nServerPort)
    : m_oSocket()
    , m_sServerAddr(sServerAddr)
    , m_nServerPort(nServerPort)
    , m_oClientThread("TcpClientThread")
    , m_pClientHandler(NULL)
    , m_pEh(NULL)
{
    m_oClientThread.SetThreadHandler(this);
    m_pClientHandler = this;
}

TcpClient::~TcpClient()
{
    Stop();
}

bool TcpClient::Start()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oClientThread.Start() == SMF_ERR_OK);
    }

    return bRet;
}

bool TcpClient::Stop()
{
    bool bRet = true;

    if (bRet) {
        bRet = (m_oClientThread.Stop() == SMF_ERR_OK);
    }

    return bRet;
}

void TcpClient::SetClientHandler(TcpClientHandler *pTcpClientHandler)
{
    m_oClientThread.Lock();
    m_pClientHandler = pTcpClientHandler;
    m_oClientThread.Unlock();
}

TcpClientHandler *TcpClient::GetClientHandler()
{
    TcpClientHandler *pTcpClientHandler = this;

    m_oClientThread.Lock();
    if (m_pClientHandler != NULL) {
        pTcpClientHandler = m_pClientHandler;
    }
    m_oClientThread.Unlock();

    return pTcpClientHandler;
}

SMF_ErrorCode TcpClient::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bSuc;
    char arrBuf[APP_PACKAGE_SIZE_MAX];

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
        TK_MSG("Conneting to the server [TCP:%s:%u] ...\n", m_sServerAddr.c_str(), m_nServerPort);
        bSuc = m_oSocket.connect(m_sServerAddr.c_str(), m_nServerPort);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED;
            TK_ERR("*** Failed to connect to the server!\n");
            GetClientHandler()->OnConnectionToServerFailed(*this);
        } else {
            std::string sLocalAddr;
            unsigned short nLocalPort;
            std::string sPeerAddr;
            unsigned short nPeerPort;

            if (m_oSocket.GetLocalSocketAddr(sLocalAddr, nLocalPort) && m_oSocket.GetPeerSocketAddr(sPeerAddr, nPeerPort)) {
                TK_MSG("Connection is established. [TCP:%s:%u <--> TCP:%s:%u]\n", sLocalAddr.c_str(), nLocalPort, sPeerAddr.c_str(), nPeerPort);
            }
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_pEh = APP_EventHandlerFactory::GetInstance().Allocate();
        if (m_pEh == NULL) {
            fprintf(stderr, "*** ERROR: Failed to allocate an event handler for the new connection!\n");
            nErrorCode = SMF_ERR_FAILED;
            m_oSocket.shutdown_fd();
            m_oSocket.close_fd();
        } else {
            m_pEh->SetIOHandler(this);
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
                nErrorCode = SMF_ERR_FAILED;
                break;
            } else if (nRet == 0) { // timeout
                continue;
            }

            bSuc = m_oSocket.recv(arrBuf, nSize);
            if (!bSuc) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            }
            if (nSize == 0) {
                nErrorCode = SMF_ERR_FAILED;
                break;
            }

            m_pEh->OnRemoteInput(std::vector<char>(arrBuf, arrBuf + nSize));
        }

        if (nErrorCode != SMF_ERR_OK) {
            m_oSocket.shutdown_fd();
            m_oSocket.close_fd();
            TK_MSG("Connection is closed by the server.\n");
            GetClientHandler()->OnConnectionClosedByServer(*this);
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

int TcpClient::SendContentToPeer(const std::vector<char> &arrContent)
{
    int nRet = arrContent.size();

    if (m_pEh != NULL) {
        m_pEh->OnLocalInput(arrContent);
    }

    return nRet;
}

int TcpClient::OnConnectionToServerFailed(TcpClient &rTcpClient)
{
    int nRet = 0;

    exit(-1);

    return nRet;
}

int TcpClient::OnConnectionClosedByServer(TcpClient &rTcpClient)
{
    int nRet = 0;

    exit(0);

    return nRet;
}

int TcpClient::OnTcpClientReceivedContent(TcpClient &rTcpClient, const std::vector<char> &arrContent)
{
    return arrContent.size();
}

int TcpClient::LocalOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData)
{
    std::string sContent(&arrContent[0], &arrContent[0] + arrContent.size());
    printf(sContent.c_str());

    return arrContent.size();
}

int TcpClient::RemoteOutput(const std::vector<char> &arrContent, unsigned int nIOCallbackData)
{
    int nRet = arrContent.size();
    size_t nSize = arrContent.size();
    bool bSuc;

    bSuc = m_oSocket.send(&arrContent[0], nSize);
    if (!bSuc) {
        nRet = -1;
    }

    return nRet;
}