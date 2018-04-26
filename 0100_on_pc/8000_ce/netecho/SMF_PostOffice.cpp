#if defined(SMF_PO_IMPL_BY_SOCK)
#include "TK_sock.h"
#else
#include "SMF_BaseMailBox.h"
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
#include "SMF_PostOffice.h"

#if defined(SMF_PO_IMPL_BY_SOCK)
#define SMF_BPO_LOCAL_ADDRESS                                       "127.127.127.127"
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)

SMF_BasePostOffice::SMF_BasePostOffice()
    : SMF_NonCopyable()
    , SMF_PostOfficeDefinition()
    , SMF_ThreadHandler()
    , m_oLock("BasePostOffice_Lock")
    , m_oSubscriberMap()
    , m_oPublisherMap()
    , m_oSharedMem(sizeof(PubSubInfo), "BasePostOffice_SharedMem")
    , m_pSharedMem(NULL)
    , m_pPubSubInfo(NULL)
    , m_pPubInfo(NULL)
    , m_pSubInfo(NULL)
#if defined(SMF_PO_IMPL_BY_SOCK)
    , m_pSenderSocket(NULL)
#else
    , m_pSenderMailBox(NULL)
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oSharedMem.Map();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pSharedMem = reinterpret_cast<unsigned char *>(m_oSharedMem.GetSharedMemAddr());
        if (m_pSharedMem != NULL) {
            if (!m_oSharedMem.AlreadyExists()) {
                memset(m_pSharedMem, 0, sizeof(PubSubInfo));
            }
        }
        if (m_pSharedMem != NULL) {
            m_pPubSubInfo = reinterpret_cast<PubSubInfo *>(m_pSharedMem);
            m_pPubInfo = &m_pPubSubInfo->stPubInfo;
            m_pSubInfo = &m_pPubSubInfo->stSubInfo;
        }
        Unlock();
    }

//#ifdef WIN32
//    {
//        WSADATA wsaData;
//        WSAStartup(MAKEWORD(2, 2), &wsaData);
//    }
//#endif // #ifdef WIN32

#if defined(SMF_PO_IMPL_BY_SOCK)
    m_pSenderSocket = new TK_Tools::Socket();
    if (m_pSenderSocket != NULL)
    {
        m_pSenderSocket->create(SOCK_DGRAM);
        m_pSenderSocket->reuse_addr();
        m_pSenderSocket->bind(0, SMF_BPO_LOCAL_ADDRESS);
    }
#else
    m_pSenderMailBox = new SMF_BaseMailBox();
    if (m_pSenderMailBox != NULL) {
        // do nothing
    }
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
}

SMF_BasePostOffice::~SMF_BasePostOffice()
{
#if defined(SMF_PO_IMPL_BY_SOCK)
    if (m_pSenderSocket != NULL) {
        delete m_pSenderSocket;
        m_pSenderSocket = NULL;
    }
#else
    if (m_pSenderMailBox != NULL) {
        delete m_pSenderMailBox;
        m_pSenderMailBox = NULL;
    }
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)

    {
        SubscriberMap::iterator it;
        for (it = m_oSubscriberMap.begin(); it != m_oSubscriberMap.end(); ++it) {
            SubscriberInfo &oSubscriberInfo = (*it).second;
            if (oSubscriberInfo.pThread != NULL) {
                delete oSubscriberInfo.pThread;
                oSubscriberInfo.pThread = NULL;
            }
        }
    }

    m_oSharedMem.Unmap();
}

SMF_BasePostOffice &SMF_BasePostOffice::GetInstance()
{
    static SMF_BasePostOffice oBasePostOffice;

    return oBasePostOffice;
}

SMF_ErrorCode SMF_BasePostOffice::RegisterMails(PublisherId nPublisherId, MailId arrMailId[], unsigned int nMailIdCount)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        for (i = SUB_ID_BASE; i < SUB_ID_BASE + SUB_COUNT_MAX; i++) {
            nErrorCode = RegisterMails(nPublisherId, i, arrMailId, nMailIdCount);
            if (nErrorCode != SMF_ERR_OK) {
                break;
            }
        }
    }

    return SMF_ERR_OK;
}

SMF_ErrorCode SMF_BasePostOffice::RegisterMails(PublisherId nPublisherId, SubscriberId nSubscriberId, MailId arrMailId[], unsigned int nMailIdCount)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (nSubscriberId >= SUB_ID_BASE + SUB_COUNT_MAX) {
            nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
        } else if (nSubscriberId < SUB_ID_BASE) {
            nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nPublisherId >= PUB_ID_BASE + PUB_COUNT_MAX) {
            nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
        } else if (nPublisherId < PUB_ID_BASE) {
            nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < nMailIdCount; i++) {
            MailId nMailId = arrMailId[i];
            if (nMailId >= MAIL_ID_BASE + MAIL_COUNT_MAX) {
                nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
            } else if (nMailId < MAIL_ID_BASE) {
                nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
            }
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPubInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        SMF_UINT32 *pMasks = m_pPubInfo->arrPubInfoItem[nPublisherId].stPublishedMailInfo[nSubscriberId].stMasks.arrMasks;
        unsigned int nMaskSize = SMF_ARR_SIZE(m_pPubInfo->arrPubInfoItem[nPublisherId].stPublishedMailInfo[nSubscriberId].stMasks.arrMasks);
        for (i = 0; i < nMailIdCount; i++) {
            MailId nMailId = arrMailId[i];
            SetFlag(nMailId, pMasks, nMaskSize);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::SubscribeMails(SubscriberId nSubscriberId, PublisherId nPublisherId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        SMF_UINT32 *pMasks = m_pSubInfo->arrSubInfoItem[nSubscriberId].stSubscibedMailInfo[nPublisherId].stMasks.arrMasks;
        unsigned int nMaskSize = SMF_ARR_SIZE(m_pSubInfo->arrSubInfoItem[nSubscriberId].stSubscibedMailInfo[nPublisherId].stMasks.arrMasks);
        memset(pMasks, 0xFF, nMaskSize * 4);
        Unlock();
    }

    return SMF_ERR_OK;
}

SMF_ErrorCode SMF_BasePostOffice::SubscribeMails(SubscriberId nSubscriberId, PublisherId nPublisherId, MailId arrMailId[], unsigned int nMailIdCount)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (nSubscriberId >= SUB_ID_BASE + SUB_COUNT_MAX) {
            nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
        } else if (nSubscriberId < SUB_ID_BASE) {
            nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nPublisherId >= PUB_ID_BASE + PUB_COUNT_MAX) {
            nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
        } else if (nPublisherId < PUB_ID_BASE) {
            nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < nMailIdCount; i++) {
            MailId nMailId = arrMailId[i];
            if (nMailId >= MAIL_ID_BASE + MAIL_COUNT_MAX) {
                nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
            } else if (nMailId < MAIL_ID_BASE) {
                nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
            }
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSubInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        SMF_UINT32 *pMasks = m_pSubInfo->arrSubInfoItem[nSubscriberId].stSubscibedMailInfo[nPublisherId].stMasks.arrMasks;
        unsigned int nMaskSize = SMF_ARR_SIZE(m_pSubInfo->arrSubInfoItem[nSubscriberId].stSubscibedMailInfo[nPublisherId].stMasks.arrMasks);
        for (i = 0; i < nMailIdCount; i++) {
            MailId nMailId = arrMailId[i];
            SetFlag(nMailId, pMasks, nMaskSize);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::PostMail(PublisherId nPublisherId, MailId nMailId, MailData &rMailData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (nPublisherId >= PUB_ID_BASE + PUB_COUNT_MAX) {
            nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
        } else if (nPublisherId < PUB_ID_BASE) {
            nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nMailId >= MAIL_ID_BASE + MAIL_COUNT_MAX) {
            nErrorCode = SMF_ERR_INDEX_TOO_GREAT;
        } else if (nMailId < MAIL_ID_BASE) {
            nErrorCode = SMF_ERR_INDEX_TOO_LITTLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (rMailData.pData == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (rMailData.nDataSize > MAIL_LEN_MAX - sizeof(MailHeader)) {
            nErrorCode = SMF_ERR_DATA_BUF_TOO_LONG;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pPubInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSubInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        for (i = SUB_ID_BASE; i < SUB_ID_BASE + SUB_COUNT_MAX; i++) {
            SMF_UINT32 *pMasks_Pub = m_pPubInfo->arrPubInfoItem[nPublisherId].stPublishedMailInfo[i].stMasks.arrMasks;
            unsigned int nMaskSize_Pub = SMF_ARR_SIZE(m_pPubInfo->arrPubInfoItem[nPublisherId].stPublishedMailInfo[i].stMasks.arrMasks);
            if (GetFlag(nMailId, pMasks_Pub, nMaskSize_Pub)) {
                SMF_UINT32 *pMasks_Sub = m_pSubInfo->arrSubInfoItem[i].stSubscibedMailInfo[nPublisherId].stMasks.arrMasks;
                unsigned int nMaskSize_Sub = SMF_ARR_SIZE(m_pSubInfo->arrSubInfoItem[i].stSubscibedMailInfo[nPublisherId].stMasks.arrMasks);
                if (GetFlag(nMailId, pMasks_Sub, nMaskSize_Sub)) {
                    Unlock();
                    nErrorCode = PostMail_ProceOneMail(nPublisherId, i, nMailId, rMailData);
                    Lock();
                    if (nErrorCode != SMF_ERR_OK) {
                        break;
                    }
                }
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::PostMail_ProceOneMail(PublisherId nPublisherId, SubscriberId nSubscriberId, MailId nMailId, MailData &rMailData)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned char pBuf[MAIL_LEN_MAX];
    MailHeader oMailHeader;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        unsigned short *pListenPorts = m_pSubInfo->arrSubInfoItem[nSubscriberId].arrListenPorts;
        if (pListenPorts != NULL) {
            for (i = 0; i < LISTEN_PORT_COUNT_PER_SUB_MAX; i++) {
                if (pListenPorts[i] != 0) {
#if defined(SMF_PO_IMPL_BY_SOCK)
                    if (m_pSenderSocket != NULL) {
#else
                    if (m_pSenderMailBox != NULL) {
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
                        unsigned short nListenPort = pListenPorts[i];
                        Unlock();
                        memset(&oMailHeader, 0, sizeof(MailHeader));
                        oMailHeader.nPackageSize = rMailData.nDataSize + sizeof(MailHeader);
                        oMailHeader.nPublisherId = nPublisherId;
                        oMailHeader.nSubscriberId = nSubscriberId;
                        oMailHeader.nMailId = nMailId;
                        oMailHeader.nDataSize = rMailData.nDataSize;
                        memcpy(pBuf, &oMailHeader, sizeof(MailHeader));
                        unsigned int nDataSize = MAIL_LEN_MAX - sizeof(MailHeader);
                        nDataSize = (nDataSize < rMailData.nDataSize) ? nDataSize : rMailData.nDataSize;
                        memcpy(pBuf + sizeof(MailHeader), rMailData.pData, nDataSize);
#if defined(SMF_PO_IMPL_BY_SOCK)
                        m_pSenderSocket->sendto(pBuf, oMailHeader.nPackageSize, nListenPort, SMF_BPO_LOCAL_ADDRESS);
#else
                        nErrorCode = m_pSenderMailBox->Bind(nListenPort);
                        if (nErrorCode != SMF_ERR_OK) {
                            break;
                        }
                        nErrorCode = m_pSenderMailBox->PutMail(pBuf, oMailHeader.nPackageSize);
                        if (nErrorCode != SMF_ERR_OK) {
                            break;
                        }
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
                        Lock();
                    }
                }
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::SetSubscriber(SubscriberId nSubscriberId, SMF_SubscriberHandler *pSubscriber)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_BaseThread::ThreadData oThreadData;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        SubscriberMap::iterator it = m_oSubscriberMap.find(nSubscriberId);
        if (pSubscriber != NULL) {
            if (it != m_oSubscriberMap.end()) {
                SubscriberInfo &oSubscriberInfo = (*it).second;
                oSubscriberInfo.pSubscriber = pSubscriber;
            } else {
                Unlock();
                SMF_BaseThread *pSubscriberThread = new SMF_BaseThread("SubscriberThread");
                if (pSubscriberThread != NULL) {
                    oThreadData.nUInt = nSubscriberId;
                    pSubscriberThread->SetThreadData(0, oThreadData);
                    pSubscriberThread->SetThreadHandler(this);
                    pSubscriberThread->StartThread();
                }
                Lock();
                m_oSubscriberMap.insert(SubscriberMap::value_type(nSubscriberId, SubscriberInfo(pSubscriberThread, pSubscriber)));
            }
        } else {
            if (it != m_oSubscriberMap.end()) {
                SubscriberInfo &oSubscriberInfo = (*it).second;
                SMF_BaseThread *pSubscriberThread = oSubscriberInfo.pThread;
                m_oSubscriberMap.erase(it);
                Unlock();
                if (pSubscriberThread != NULL) {
                    pSubscriberThread->StopThread();
                    delete pSubscriberThread;
                }
                Lock();
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::GetPublisher(PublisherId nPublisherId, SMF_PublisherHandler **ppPublisher)
{
    class SMF_Publisher : public SMF_PublisherHandler
    {
    public:
        SMF_Publisher(SMF_BasePostOffice *pPostOffice, PublisherId nPublisherId)
            : m_pPostOffice(pPostOffice), m_nPublisherId(nPublisherId) {}

        virtual SMF_ErrorCode PostMail(MailId nMailId, MailData &rMailData) {
            SMF_ErrorCode nErrorCode = SMF_ERR_OK;
            if (m_pPostOffice != NULL) {
                nErrorCode = m_pPostOffice->PostMail(m_nPublisherId, nMailId, rMailData);
            }
            return nErrorCode;
        }

    private:
        SMF_BasePostOffice *m_pPostOffice;
        PublisherId m_nPublisherId;
    };

    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PublisherHandler *pPublisher = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        PublisherMap::iterator it = m_oPublisherMap.find(nPublisherId);
        if (it != m_oPublisherMap.end()) {
            PublisherInfo &rPublisherInfo = (*it).second;
            pPublisher = rPublisherInfo.pPublisher;
        } else {
            pPublisher = new SMF_Publisher(this, nPublisherId);
            if (pPublisher != NULL) {
                m_oPublisherMap.insert(PublisherMap::value_type(nPublisherId, PublisherInfo(pPublisher)));
            }
        }
        Unlock();

        if (ppPublisher != NULL) {
            *ppPublisher = pPublisher;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::RemovePublisher(PublisherId nPublisherId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        PublisherMap::iterator it = m_oPublisherMap.find(nPublisherId);
        if (it != m_oPublisherMap.end()) {
            PublisherInfo &rPublisherInfo = (*it).second;
            if (rPublisherInfo.pPublisher != NULL) {
                delete rPublisherInfo.pPublisher;
            }
            m_oPublisherMap.erase(it);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BasePostOffice::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SubscriberId nSubscriberId = rThread.GetThreadData(0).nUInt;
#if defined(SMF_PO_IMPL_BY_SOCK)
    TK_Tools::Socket sock;
#else
    SMF_BaseMailBox *pMailBox = NULL;
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
    unsigned char pBuf[MAIL_LEN_MAX];
    unsigned int nSize;
    SubscriberInfo *pSubscriberInfo;
    unsigned short nListenPort = 0;
    unsigned int i;

    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);

    if (nErrorCode == SMF_ERR_OK) {
        pMailBox = new SMF_BaseMailBox;
        if (pMailBox == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

#if defined(SMF_PO_IMPL_BY_SOCK)
    if (nErrorCode == SMF_ERR_OK) {
	    if (!sock.create(SOCK_DGRAM))
	    {
		    nErrorCode = SMF_ERR_FAILED_TO_CREATE_SOCKET;
	    }
    }

    if (nErrorCode == SMF_ERR_OK) {
	    sock.reuse_addr();
        if (!sock.bind(0, SMF_BPO_LOCAL_ADDRESS))
	    {
		    nErrorCode = SMF_ERR_FAILED_TO_BIND_ADDR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (!sock.getlocalport(nListenPort) || nListenPort == 0) {
            nErrorCode = SMF_ERR_FAILED_TO_BIND_ADDR;
        }
    }
#else
    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = pMailBox->Bind(SMF_BaseMailBox::MB_ID_INVALID);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nListenPort = pMailBox->GetMailBoxId();
    }
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        unsigned short *pListenPorts = m_pSubInfo->arrSubInfoItem[nSubscriberId].arrListenPorts;
        if (pListenPorts != NULL) {
            for (i = 0; i < LISTEN_PORT_COUNT_PER_SUB_MAX; i++) {
                if (pListenPorts[i] == 0) {
                    pListenPorts[i] = nListenPort;
                    break;
                }
            }
            if (i == LISTEN_PORT_COUNT_PER_SUB_MAX) {
                nErrorCode = SMF_ERR_TOO_LISTEN_ON_A_SUBSCRIBER;
            }
        }
        Unlock();
    }

    if (nErrorCode == SMF_ERR_OK) {
        rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);
	    for (;;)
	    {
            if (rThread.GetThreadState() != SMF_BaseThread::TS_ACTIVATED)
            {
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
                break;
            }

            nSize = MAIL_LEN_MAX;
#if defined(SMF_PO_IMPL_BY_SOCK)
            if (!sock.recvfrom(pBuf, nSize)) {
                SMF_LOG_ERROR("*** ERROR: [%s()] sock.recvfrom() is failed\n", __FUNCTION__);
			    continue;
		    }
#else
            nErrorCode = pMailBox->GetMail(pBuf, nSize, SMF_THREAD_AWAKEN_INTERVAL_MILLISECONDS);
            if (nErrorCode == SMF_ERR_TIMEOUT) {
                continue;
            }
            if (nErrorCode != SMF_ERR_OK) {
                SMF_LOG_ERROR("*** ERROR: [%s()] oMailBox.GetMail() is failed, exits thread ...\n", __FUNCTION__);
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
			    break;
		    }
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)

            if (nSize < sizeof(MailHeader)) {
                SMF_LOG_ERROR("*** ERROR: [%s()] nSize < sizeof(MailHeader)\n", __FUNCTION__);
                continue;
            }

            MailHeader oMailHeader;
            memcpy(&oMailHeader, pBuf, sizeof(MailHeader));
            if (oMailHeader.nPackageSize != nSize) {
                SMF_LOG_ERROR("*** ERROR: [%s()] oMailHeader.nPackageSize != nSize\n", __FUNCTION__);
                continue;
            }

            pSubscriberInfo = NULL;

            Lock();
            SubscriberMap::iterator it = m_oSubscriberMap.find(nSubscriberId);
            if (it != m_oSubscriberMap.end()) {
                pSubscriberInfo = &(*it).second;
            }
            Unlock();

            MailData oMailData(pBuf + sizeof(MailHeader), oMailHeader.nDataSize);
            if (pSubscriberInfo != NULL && pSubscriberInfo->pSubscriber != NULL) {
                pSubscriberInfo->pSubscriber->OnReceiveMail(
                    oMailHeader.nSubscriberId,
                    oMailHeader.nPublisherId,
                    oMailHeader.nMailId,
                    oMailData
               );
            }
        }

        rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVED);
    }

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYING);

    {
        Lock();
        unsigned short *pListenPorts = m_pSubInfo->arrSubInfoItem[nSubscriberId].arrListenPorts;
        if (pListenPorts != NULL) {
            for (i = 0; i < LISTEN_PORT_COUNT_PER_SUB_MAX; i++) {
                if (pListenPorts[i] == nListenPort) {
                    pListenPorts[i] = 0;
                    break;
                }
            }
        }
        Unlock();
    }

    if (pMailBox == NULL) {
        delete pMailBox;
        pMailBox = NULL;
    }

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYED);

    return nErrorCode;
}

void SMF_BasePostOffice::Lock()
{
    m_oLock.Lock();
}

void SMF_BasePostOffice::Unlock()
{
    m_oLock.Unlock();
}

void SMF_BasePostOffice::SetFlag(unsigned int nFlagShift, SMF_UINT32 pMaskArray[], unsigned int nMaskArrayCount)
{
    unsigned int nIndex = nFlagShift / sizeof(SMF_UINT32);
    unsigned int nShift = nFlagShift % sizeof(SMF_UINT32);

    if (pMaskArray != NULL && nIndex < nMaskArrayCount) {
        pMaskArray[nIndex] |= (0x00000001 << nShift);
    }
}

void SMF_BasePostOffice::ResetFlag(unsigned int nFlagShift, SMF_UINT32 pMaskArray[], unsigned int nMaskArrayCount)
{
    unsigned int nIndex = nFlagShift / sizeof(SMF_UINT32);
    unsigned int nShift = nFlagShift % sizeof(SMF_UINT32);

    if (pMaskArray != NULL && nIndex < nMaskArrayCount) {
        pMaskArray[nIndex] &= ~(0x00000001 << nShift);
    }
}

unsigned int SMF_BasePostOffice::GetFlag(unsigned int nFlagShift, SMF_UINT32 pMaskArray[], unsigned int nMaskArrayCount)
{
    unsigned int nResult = 0;

    unsigned int nIndex = nFlagShift / sizeof(SMF_UINT32);
    unsigned int nShift = nFlagShift % sizeof(SMF_UINT32);

    if (pMaskArray != NULL && nIndex < nMaskArrayCount) {
        nResult = (pMaskArray[nIndex] >> nShift) & 0x00000001;
    }

    return nResult;
}
