#include "TK_Tools.h"
#include "SMF_BaseMailBox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_MailBoxFactory
//
SMF_MailBoxFactory::SMF_MailBoxFactory()
    : SMF_NonCopyable()
    , SMF_BaseMailBoxDefinition()
    , m_oLock("MailBoxFactory_Mutex")
    , m_oSharedMem(sizeof(SharedMailBoxSetInfo), "MailBoxFactory_SharedMem")
    , m_pSharedMem(NULL)
    , m_pSharedMailBoxSetInfo(NULL)
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
                memset(m_pSharedMem, 0, sizeof(SharedMailBoxSetInfo));
            }
        }
        if (m_pSharedMem != NULL) {
            m_pSharedMailBoxSetInfo = reinterpret_cast<SharedMailBoxSetInfo *>(m_pSharedMem);
        }
        Unlock();
    }
}

SMF_MailBoxFactory::~SMF_MailBoxFactory()
{
    m_oSharedMem.Unmap();
}

SMF_ErrorCode SMF_MailBoxFactory::AllocMailBox(MailBoxId &nMailBoxId, SharedMailBoxSetInfo::MailBoxInfo **ppMailBoxInfo)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    MailBoxId nMailBoxIdMax = MB_ID_INVALID;
    unsigned int i;
    unsigned int nFirstInvalidIndex = MB_COUNT_MAX;

    if (nErrorCode == SMF_ERR_OK) {
        if (ppMailBoxInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMailBoxSetInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    Lock();
    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < MB_COUNT_MAX; i++) {
            SharedMailBoxSetInfo::MailBoxInfo &oMailBoxInfo = m_pSharedMailBoxSetInfo->arrMailBoxInfo[i];
            if (oMailBoxInfo.nMailBoxId > nMailBoxIdMax) {
                nMailBoxIdMax = oMailBoxInfo.nMailBoxId;
            }
            if (oMailBoxInfo.nMailBoxId == MB_ID_INVALID && nFirstInvalidIndex == MB_COUNT_MAX) {
                nFirstInvalidIndex = i;
            }
            if (nMailBoxId != MB_ID_INVALID && nMailBoxId == oMailBoxInfo.nMailBoxId) {
                break;
            }
        }

        if (i < MB_COUNT_MAX) {
            m_pSharedMailBoxSetInfo->arrMailBoxInfo[i].nRefCount++;
            *ppMailBoxInfo = &m_pSharedMailBoxSetInfo->arrMailBoxInfo[i];
        } else {
            if (nFirstInvalidIndex < MB_COUNT_MAX) {
                nMailBoxId = AllocMailId_unsafe(nMailBoxIdMax + 1);
                SharedMailBoxSetInfo::MailBoxInfo &oMailBoxInfo = m_pSharedMailBoxSetInfo->arrMailBoxInfo[nFirstInvalidIndex];
                oMailBoxInfo.nMailBoxId = nMailBoxId;
                oMailBoxInfo.nRefCount = 1;
                oMailBoxInfo.nPosWrite = 0;
                oMailBoxInfo.nPosRead = 0;
                *ppMailBoxInfo = &oMailBoxInfo;
            } else {
                nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MAIL_BOX;
            }
        }
    }
    Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_MailBoxFactory::FreeMailBox(MailBoxId nMailBoxId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (nMailBoxId == MB_ID_INVALID) {
            nErrorCode = SMF_ERR_INVALID_MAIL_BOX_ID;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMailBoxSetInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    Lock();
    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < MB_COUNT_MAX; i++) {
            SharedMailBoxSetInfo::MailBoxInfo &oMailBoxInfo = m_pSharedMailBoxSetInfo->arrMailBoxInfo[i];
            if (oMailBoxInfo.nMailBoxId == nMailBoxId) {
                break;
            }
        }

        if (i < MB_COUNT_MAX) {
            m_pSharedMailBoxSetInfo->arrMailBoxInfo[i].nRefCount--;
            if (m_pSharedMailBoxSetInfo->arrMailBoxInfo[i].nRefCount <= 0) {
                m_pSharedMailBoxSetInfo->arrMailBoxInfo[i].nMailBoxId = MB_ID_INVALID;
            }
        } else {
            nErrorCode = SMF_ERR_MAIL_BOX_ALREADY_FREED;
        }
    }
    Unlock();

    return nErrorCode;
}

void SMF_MailBoxFactory::Lock()
{
    m_oLock.Lock();
}

void SMF_MailBoxFactory::Unlock()
{
    m_oLock.Unlock();
}

SMF_MailBoxFactory::MailBoxId SMF_MailBoxFactory::AllocMailId_unsafe(MailBoxId nRequestedMailBoxId)
{
    MailBoxId nRetMailBoxId = MB_ID_INVALID;
    std::set<MailBoxId> setMailIds;
    unsigned int i;

    for (i = 0; i < MB_COUNT_MAX; i++) {
        MailBoxId nMailBoxId = m_pSharedMailBoxSetInfo->arrMailBoxInfo[i].nMailBoxId;
        if (nMailBoxId != MB_ID_INVALID) {
            setMailIds.insert(nMailBoxId);
        }
    }

    if (nRequestedMailBoxId != MB_ID_INVALID) {
        std::set<MailBoxId>::iterator it = setMailIds.find(nRequestedMailBoxId);
        if (it == setMailIds.end()) {
            nRetMailBoxId = nRequestedMailBoxId;
        } else {
            nRequestedMailBoxId = MB_ID_INVALID;
        }
    }

    if (nRequestedMailBoxId == MB_ID_INVALID) {
        for (nRequestedMailBoxId = MB_ID_BASE;; nRetMailBoxId++) {
            std::set<MailBoxId>::iterator it = setMailIds.find(nRequestedMailBoxId);
            if (it == setMailIds.end()) {
                nRetMailBoxId = nRequestedMailBoxId;
                break;
            }
        }
    }

    return nRetMailBoxId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseMailBox
//
SMF_BaseMailBox::SMF_BaseMailBox()
    : SMF_NonCopyable()
    , SMF_BaseMailBoxDefinition()
    , m_rMailBoxFactory(SMF_MailBoxFactory::GetInstance())
    , m_nMailBoxFeature(MB_FEATURE_DEFAULT)
    , m_nMailBoxId(MB_ID_INVALID)
    , m_pMailBoxInfo(NULL)
    , m_pQNotFullEvent(NULL)
    , m_sQNotFullEventName()
    , m_pQNotEmptyEvent(NULL)
    , m_sQNotEmptyEventName()
    , m_pLock(NULL)
    , m_sLockName()
{

}

SMF_BaseMailBox::~SMF_BaseMailBox()
{
    if (m_pLock != NULL) {
        delete m_pLock;
        m_pLock = NULL;
    }
    if (m_pQNotEmptyEvent != NULL) {
        delete m_pQNotEmptyEvent;
        m_pQNotEmptyEvent = NULL;
    }
    if (m_pQNotFullEvent != NULL) {
        delete m_pQNotFullEvent;
        m_pQNotFullEvent = NULL;
    }
    FreeMailBox();
}

SMF_ErrorCode SMF_BaseMailBox::Bind(MailBoxId nMailBoxId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pLock != NULL) {
            delete m_pLock;
            m_pLock = NULL;
        }
        if (m_pQNotEmptyEvent != NULL) {
            delete m_pQNotEmptyEvent;
            m_pQNotEmptyEvent = NULL;
        }
        if (m_pQNotFullEvent != NULL) {
            delete m_pQNotFullEvent;
            m_pQNotFullEvent = NULL;
        }

        //m_sQNotFullEventName.clear();
        //m_sQNotEmptyEventName.clear();
        //m_sLockName.clear();
        m_sQNotFullEventName = "";
        m_sQNotEmptyEventName = "";
        m_sLockName = "";
        FreeMailBox();
        m_nMailBoxId = nMailBoxId;
        nErrorCode = AllocMailBox();
    }

    if (nErrorCode == SMF_ERR_OK) {
        TK_Tools::FormatStr(m_sLockName, "BaseMailBox_%u_Mutex", m_nMailBoxId);
        m_pLock = new SMF_PlatformLock(m_sLockName.c_str());

        TK_Tools::FormatStr(m_sQNotFullEventName, "BaseMailBox_%u_QNotFullEvent", m_nMailBoxId);
        m_pQNotFullEvent = new SMF_PlatformEvent(FALSE, FALSE, m_sQNotFullEventName.c_str());

        TK_Tools::FormatStr(m_sQNotEmptyEventName, "BaseMailBox_%u_QNotEmptyEvent", m_nMailBoxId);
        m_pQNotEmptyEvent = new SMF_PlatformEvent(FALSE, FALSE, m_sQNotEmptyEventName.c_str());
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseMailBox::GetMail(unsigned char *pBuf, unsigned int &nBufSize, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bEmpty;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize == 0) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pMailBoxInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            bEmpty = IsMailBoxEmpty_unsafe();
            if (bEmpty) {
                Unlock();
                nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, m_pQNotEmptyEvent, nTimeout);
                if (nErrorCode != SMF_ERR_OK) {
                    break;
                } else if (nWaitResult == SMF_PlatformEvent::WAIT_RESULT_TIMEOUT) {
                    nErrorCode = SMF_ERR_TIMEOUT;
                    break;
                }
            } else {
                GetMailFromQ_unsafe(pBuf, nBufSize);
                Unlock();
                if (nBufSize == 0) {
                    nErrorCode = SMF_ERR_EMPTY_BUFFER;
                } else {
                    if (m_pQNotFullEvent != NULL) {
                        m_pQNotFullEvent->SetEvent();
                    }
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseMailBox::PutMail(const unsigned char *pBuf, unsigned int nBufSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bFull;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize == 0) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pMailBoxInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            bFull = IsMailBoxFull_unsafe(nBufSize);
            if (bFull) {
                Unlock();
                if (m_nMailBoxFeature & MB_FEATURE_NO_WAIT_ON_POST_MAIL) {
                    nErrorCode = SMF_ERR_MAIL_BOX_IS_FULL;
                    break;
                } else {
                    SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, m_pQNotFullEvent);
                }
            } else {
                PutMailToQ_unsafe(pBuf, nBufSize);
                Unlock();
                if (m_pQNotEmptyEvent != NULL) {
                    m_pQNotEmptyEvent->SetEvent();
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseMailBox::AllocMailBox()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pMailBoxInfo == NULL) {
            nErrorCode = m_rMailBoxFactory.AllocMailBox(m_nMailBoxId, &m_pMailBoxInfo);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseMailBox::FreeMailBox()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pMailBoxInfo != NULL) {
            nErrorCode = m_rMailBoxFactory.FreeMailBox(m_nMailBoxId);
            m_pMailBoxInfo = NULL;
        }
    }

    return nErrorCode;
}

bool SMF_BaseMailBox::IsMailBoxFull_unsafe(unsigned int nNewMailDataSize)
{
    bool bFull = true;

    if (sizeof(MailHeader) + nNewMailDataSize >=  MB_BUF_SIZE) {
        bFull = true;
    } else {
        int nUsed = m_pMailBoxInfo->nPosWrite - m_pMailBoxInfo->nPosRead;
        if (nUsed < 0) {
            nUsed += MB_BUF_SIZE;
        }
        if (nUsed + sizeof(MailHeader) + nNewMailDataSize >= MB_BUF_SIZE) {
            bFull = true;
        } else {
            bFull = false;
        }
    }

    return bFull;
}

bool SMF_BaseMailBox::IsMailBoxEmpty_unsafe()
{
    bool bEmpty = true;

    if (m_pMailBoxInfo->nPosWrite == m_pMailBoxInfo->nPosRead) {
        bEmpty = true;
    } else {
        bEmpty = false;
    }

    return bEmpty;
}

void SMF_BaseMailBox::GetMailFromQ_unsafe(unsigned char *pBuf, unsigned int &nBufSize)
{
    MailHeader oMailHeader;
    unsigned char *pBufTmp = NULL;

    CopyFromQ_unsafe(reinterpret_cast<unsigned char *>(&oMailHeader), m_pMailBoxInfo->nPosRead, sizeof(MailHeader));
    if (nBufSize >= oMailHeader.nMailSize) {
        CopyFromQ_unsafe(pBuf, m_pMailBoxInfo->nPosRead, oMailHeader.nMailSize);
        nBufSize = oMailHeader.nMailSize;
    } else {
        pBufTmp = new unsigned char [oMailHeader.nMailSize];
        if (pBufTmp != NULL) {
            CopyFromQ_unsafe(pBufTmp, m_pMailBoxInfo->nPosRead, oMailHeader.nMailSize);
            memcpy(pBuf, pBufTmp, nBufSize);
            delete pBufTmp;
            pBufTmp = NULL;
        } else {
            nBufSize = 0;
        }
    }
}

void SMF_BaseMailBox::PutMailToQ_unsafe(const unsigned char *pBuf, unsigned int nBufSize)
{
    MailHeader oMailHeader(nBufSize, 0);

    CopyToQ_unsafe(reinterpret_cast<unsigned char *>(&oMailHeader), m_pMailBoxInfo->nPosWrite, sizeof(MailHeader));
    CopyToQ_unsafe(pBuf, m_pMailBoxInfo->nPosWrite, nBufSize);
}

void SMF_BaseMailBox::CopyFromQ_unsafe(unsigned char *pBuf, unsigned int &nQBufPos, unsigned int nSize)
{
    unsigned int i;

    for (i = 0; i < nSize; i++) {
        *pBuf = m_pMailBoxInfo->pMailBuf[nQBufPos];
        pBuf++;
        nQBufPos++;
        if (nQBufPos >= MB_BUF_SIZE) {
            nQBufPos = 0;
        }
    }
}

void SMF_BaseMailBox::CopyToQ_unsafe(const unsigned char *pBuf, unsigned int &nQBufPos, unsigned int nSize)
{
    unsigned int i;

    for (i = 0; i < nSize; i++) {
        m_pMailBoxInfo->pMailBuf[nQBufPos] = *pBuf;
        pBuf++;
        nQBufPos++;
        if (nQBufPos >= MB_BUF_SIZE) {
            nQBufPos = 0;
        }
    }
}

void SMF_BaseMailBox::Lock()
{
    if (m_pLock != NULL) {
        m_pLock->Lock();
    }
}

void SMF_BaseMailBox::Unlock()
{
    if (m_pLock != NULL) {
        m_pLock->Unlock();
    }
}
