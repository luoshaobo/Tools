#ifndef SMF_POST_OFFICE_H__820425348959235834953476893458934593782347828
#define SMF_POST_OFFICE_H__820425348959235834953476893458934593782347828

#include "SMF_afx.h"
#include "SMF_BaseThread.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformSharedMem.h"

//#define SMF_PO_IMPL_BY_SOCK

#if defined(SMF_PO_IMPL_BY_SOCK)
namespace TK_Tools {
    class Socket;
}
#else
class SMF_BaseMailBox;
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BasePostOffice relevant interfaces
//

interface SMF_PostOfficeDefinition : public SMF_BaseInterface
{
    enum {
        PUB_COUNT_MAX = 10,
    };
    enum {
        SUB_COUNT_MAX = PUB_COUNT_MAX,
    };
    enum {
        MAIL_COUNT_MAX = 256,
    };
    enum {
        MAIL_LEN_MAX = 256,
    };
    enum {
        LISTEN_PORT_COUNT_PER_SUB_MAX = 10,
    };
    enum {
        PUB_ID_BASE = 0,
        PUB_ID_ALL = SMF_INFINITE
    };
    enum {
        SUB_ID_BASE = 0,
        SUB_ID_ALL = SMF_INFINITE
    };
    enum {
        MAIL_ID_BASE = 0,
        MAIL_ID_ALL = SMF_INFINITE
    };

    typedef unsigned int PublisherId;
    typedef unsigned int SubscriberId;
    typedef unsigned int MailId;
    struct MailData {
        MailData(void *a_pData, unsigned int a_nDataSize) : pData(a_pData), nDataSize(a_nDataSize) {}

        void *pData;
        unsigned int nDataSize;
    };
};

interface SMF_PublisherHandler : public SMF_PostOfficeDefinition
{
    virtual SMF_ErrorCode PostMail(MailId nMailId, MailData &rMailData) = 0;
};

interface SMF_SubscriberHandler : public SMF_PostOfficeDefinition
{
    virtual SMF_ErrorCode OnReceiveMail(SubscriberId nSubscriberId, PublisherId nPublisherId, MailId nMailId, MailData &rMailData) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BasePostOffice
//
class SMF_BasePostOffice
    : private SMF_NonCopyable
    , public SMF_PostOfficeDefinition
    , public SMF_ThreadHandler
{
private:
    // NOTE: For this data structure is in the shared memory, it must be plain old data.
    struct PubSubInfo {
        struct PubInfo {
            struct PubInfoItem {
                struct PublishedMailInfo {
                    struct Masks {
                        SMF_UINT32 arrMasks[(MAIL_ID_BASE + MAIL_COUNT_MAX + (sizeof(SMF_UINT32)*8)) / (sizeof(SMF_UINT32)*8)];
                    } stMasks;
                } stPublishedMailInfo[SUB_ID_BASE + SUB_COUNT_MAX];
            } arrPubInfoItem[PUB_ID_BASE + PUB_COUNT_MAX];
        } stPubInfo;
        struct SubInfo {
            struct SubInfoItem {
                struct SubscibedMailInfo {
                    struct Masks {
                        SMF_UINT32 arrMasks[(MAIL_ID_BASE + MAIL_COUNT_MAX + (sizeof(SMF_UINT32)*8)) / (sizeof(SMF_UINT32)*8)];
                    } stMasks;
                } stSubscibedMailInfo[PUB_ID_BASE + PUB_COUNT_MAX];
                SMF_UINT16 arrListenPorts[LISTEN_PORT_COUNT_PER_SUB_MAX];
            } arrSubInfoItem[SUB_ID_BASE + SUB_COUNT_MAX];
        } stSubInfo;
    };

    struct MailHeader {
        unsigned int nPackageSize;
        PublisherId nPublisherId;
        SubscriberId nSubscriberId;
        MailId nMailId;
        unsigned int nDataSize;
    };

    struct SubscriberInfo {
        SubscriberInfo() : pThread(NULL), pSubscriber(NULL) {}
        SubscriberInfo(SMF_BaseThread *a_pThread, SMF_SubscriberHandler *a_pSubscriber)
            : pThread(a_pThread), pSubscriber(a_pSubscriber) {}

        SMF_BaseThread *pThread;
        SMF_SubscriberHandler *pSubscriber;
    };
    typedef std::map<SubscriberId, SubscriberInfo> SubscriberMap;

    struct PublisherInfo {
        PublisherInfo() : pPublisher(NULL) {}
        PublisherInfo(SMF_PublisherHandler *a_pPublisher)
            : pPublisher(a_pPublisher) {}

        SMF_PublisherHandler *pPublisher;
    };
    typedef std::map<PublisherId, PublisherInfo> PublisherMap;

protected:
    SMF_BasePostOffice();

public:
    virtual ~SMF_BasePostOffice();

    static SMF_BasePostOffice &GetInstance();

public:
    SMF_ErrorCode RegisterMails(PublisherId nPublisherId, MailId arrMailId[], unsigned int nMailIdCount);
    SMF_ErrorCode RegisterMails(PublisherId nPublisherId, SubscriberId nSubscriberId, MailId arrMailId[], unsigned int nMailIdCount);
    SMF_ErrorCode SubscribeMails(SubscriberId nSubscriberId, PublisherId nPublisherId);
    SMF_ErrorCode SubscribeMails(SubscriberId nSubscriberId, PublisherId nPublisherId, MailId arrMailId[], unsigned int nMailIdCount);

    SMF_ErrorCode PostMail(PublisherId nPublisherId, MailId nMailId, MailData &rMailData);

public:
    SMF_ErrorCode SetSubscriber(SubscriberId nSubscriberId, SMF_SubscriberHandler *ppSubscriber);
    SMF_ErrorCode GetPublisher(PublisherId nPublisherId, SMF_PublisherHandler **pPublisher);
    SMF_ErrorCode RemovePublisher(PublisherId nPublisherId);

private:
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);
    SMF_ErrorCode PostMail_ProceOneMail(PublisherId nPublisherId, SubscriberId nSubscriberId, MailId nMailId, MailData &rMailData);

private:
    void Lock();
    void Unlock();

    void SetFlag(unsigned int nFlagShift, SMF_UINT32 pMaskArray[], unsigned int nMaskArrayCount);
    void ResetFlag(unsigned int nFlagShift, SMF_UINT32 pMaskArray[], unsigned int nMaskArrayCount);
    unsigned int GetFlag(unsigned int nFlagShift, SMF_UINT32 pMaskArray[], unsigned int nMaskArrayCount);

private:
    SMF_PlatformLock m_oLock;
    SubscriberMap m_oSubscriberMap;
    PublisherMap m_oPublisherMap;
    SMF_PlatformSharedMem m_oSharedMem;
    unsigned char *m_pSharedMem;
    PubSubInfo *m_pPubSubInfo;
    PubSubInfo::PubInfo *m_pPubInfo;
    PubSubInfo::SubInfo *m_pSubInfo;
#if defined(SMF_PO_IMPL_BY_SOCK)
    TK_Tools::Socket *m_pSenderSocket;
#else
    SMF_BaseMailBox *m_pSenderMailBox;
#endif // #if defined(SMF_PO_IMPL_BY_SOCK)
};

#endif // #ifndef SMF_POST_OFFICE_H__820425348959235834953476893458934593782347828
