#ifndef SMF_BASE_MAIL_BOX_H__623743908874328582349094365469845683474378874
#define SMF_BASE_MAIL_BOX_H__623743908874328582349094365469845683474378874

#include "SMF_afx.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformSharedMem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseMailBox relevant interfaces
//
interface SMF_BaseMailBoxDefinition : public SMF_BaseInterface
{
    enum {
        MB_BUF_SIZE = 1024 * 10,
    };
    enum {
        MB_COUNT_MAX = 10,
    };
    enum {
        MB_ID_INVALID = 0,
        MB_ID_BASE = 1,
    };
    typedef unsigned int MailBoxId;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_MailBoxFactory
//
class SMF_MailBoxFactory
    : private SMF_NonCopyable
    , public SMF_BaseMailBoxDefinition
{
public:
    struct SharedMailBoxSetInfo {
        struct MailBoxInfo {
            MailBoxId nMailBoxId;
            int nRefCount;
            unsigned char pMailBuf[MB_BUF_SIZE];
            unsigned int nPosWrite;
            unsigned int nPosRead;
        } arrMailBoxInfo[MB_COUNT_MAX];
    };

private:
    SMF_MailBoxFactory();

public:
    virtual ~SMF_MailBoxFactory();

    static SMF_MailBoxFactory &GetInstance() {
        static SMF_MailBoxFactory oBaseMailBoxMananger;
        return oBaseMailBoxMananger;
    }

    SMF_ErrorCode AllocMailBox(MailBoxId &nMailBoxId, SharedMailBoxSetInfo::MailBoxInfo **ppMailBoxInfo);
    SMF_ErrorCode FreeMailBox(MailBoxId nMailBoxId);

    void Lock();
    void Unlock();

private:
    MailBoxId AllocMailId_unsafe(MailBoxId nRequestedMailBoxId);

private:
    SMF_PlatformLock m_oLock;
    SMF_PlatformSharedMem m_oSharedMem;
    unsigned char *m_pSharedMem;
    SharedMailBoxSetInfo *m_pSharedMailBoxSetInfo;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseMailBox
//
class SMF_BaseMailBox
    : private SMF_NonCopyable
    , public SMF_BaseMailBoxDefinition
{
public:
    enum MailBoxFeature{
        MB_FEATURE_DEFAULT                                 = 0x00000000,
        MB_FEATURE_NO_WAIT_ON_POST_MAIL                    = 0x00000001,
    };

private:
    struct MailHeader {
        MailHeader() : nMailSize(0), nCheckSum(0) {}
        MailHeader(unsigned short a_nMailSize, unsigned short a_nCheckSum)
            : nMailSize(a_nMailSize), nCheckSum(a_nCheckSum) {}

        unsigned short nMailSize;
        unsigned short nCheckSum;
    };

private:
    typedef SMF_MailBoxFactory::SharedMailBoxSetInfo::MailBoxInfo MailBoxInfo;

public:
    SMF_BaseMailBox();
    virtual ~SMF_BaseMailBox();

    SMF_ErrorCode Bind(MailBoxId nMailBoxId);

    SMF_ErrorCode GetMail(unsigned char *pBuf, unsigned int &nBufSize, unsigned int nTimeout = SMF_INFINITE);
    SMF_ErrorCode PutMail(const unsigned char *pBuf, unsigned int nBufSize);

public:
    MailBoxId GetMailBoxId() const { return m_nMailBoxId; }
    void SetFeature(MailBoxFeature nMailBoxFeature) { m_nMailBoxFeature = nMailBoxFeature; }

private:
    SMF_ErrorCode AllocMailBox();
    SMF_ErrorCode FreeMailBox();

    bool IsMailBoxFull_unsafe(unsigned int nNewMailDataSize);
    bool IsMailBoxEmpty_unsafe();
    void GetMailFromQ_unsafe(unsigned char *pBuf, unsigned int &nBufSize);
    void PutMailToQ_unsafe(const unsigned char *pBuf, unsigned int nBufSize);
    void CopyFromQ_unsafe(unsigned char *pBuf, unsigned int &nQBufPos, unsigned int nSize);
    void CopyToQ_unsafe(const unsigned char *pBuf, unsigned int &nQBufPos, unsigned int nSize);

    void Lock();
    void Unlock();

private:
    SMF_MailBoxFactory &m_rMailBoxFactory;
    MailBoxFeature m_nMailBoxFeature;
    MailBoxId m_nMailBoxId;
    MailBoxInfo *m_pMailBoxInfo;
    SMF_PlatformEvent *m_pQNotFullEvent;
    std::string m_sQNotFullEventName;
    SMF_PlatformEvent *m_pQNotEmptyEvent;
    std::string m_sQNotEmptyEventName;
    SMF_PlatformLock *m_pLock;
    std::string m_sLockName;
};

#endif // #ifndef SMF_BASE_MAIL_BOX_H__623743908874328582349094365469845683474378874
