#include "TK_Tools.h"
#include "SMF_CyclicBuf.h"

SMF_CyclicBuf::SMF_CyclicBuf(unsigned char *pRawBufAddr, unsigned int nRawBufSize, const std::string &sBufName,  bool bSharedInProcesses /*= false*/, bool bBuildControlBlock /*= true*/)
    : SMF_NonCopyable()
    , m_pBufControlBlock(NULL)
    , m_pBufAddr(m_pBufAddr)
    , m_pRawBufAddr(pRawBufAddr)
    , m_nRawBufSize(nRawBufSize)
    , m_sBufName(sBufName)
    , m_bSharedInProcesses(bSharedInProcesses)
    , m_bBuildControlBlock(!bSharedInProcesses ? true : bBuildControlBlock)
    , m_oLock(GetLockName().c_str(), bSharedInProcesses)
    , m_oBufNotEmptyEvent(false, false, GetBufNotEmptyEventName().c_str(), bSharedInProcesses)
    , m_oBufNotFullEvent(false, false, GetBufNotFullEventName().c_str(), bSharedInProcesses)
{
    InitBufControlBlock();
}

SMF_CyclicBuf::~SMF_CyclicBuf()
{

}

SMF_ErrorCode SMF_CyclicBuf::PutData(unsigned char *pData, unsigned int nDataSize, bool bWait /*= true*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pData == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBufControlBlock == NULL) {
            nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_INIT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            if (nDataSize > m_pBufControlBlock->nSizeCanWrite) {
                if (!bWait) {
                    nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_ENOUGH_SPACE_TO_WRITE;
                    Unlock();
                    break;
                } else {
                    Unlock();
                    nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, &m_oBufNotFullEvent);
                    if (nErrorCode != SMF_ERR_OK) {
                        break;
                    } else {
                        if (nWaitResult == SMF_PlatformEvent::WAIT_RESULT_OBJECT_0) {
                            continue;
                        } else {
                            nErrorCode = SMF_ERR_WAIT_FAILED;
                            break;
                        }
                    }
                }
            } else {
                nErrorCode = PutData_unsafe(pData, nDataSize);
                Unlock();
                break;
            }
        }
    }
    
    if (nErrorCode == SMF_ERR_OK) {
        m_oBufNotEmptyEvent.SetEvent();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_CyclicBuf::GetData(unsigned char *pData, unsigned int nDataSize, bool bWait /*= true*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pData == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBufControlBlock == NULL) {
            nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_INIT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            if (nDataSize > m_pBufControlBlock->nSizeCanRead) {
                if (!bWait) {
                    nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_ENOUGH_DATA_TO_READ;
                    Unlock();
                    break;
                } else {
                    Unlock();
                    nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, &m_oBufNotEmptyEvent);
                    if (nErrorCode != SMF_ERR_OK) {
                        break;
                    } else {
                        if (nWaitResult == SMF_PlatformEvent::WAIT_RESULT_OBJECT_0) {
                            continue;
                        } else {
                            nErrorCode = SMF_ERR_WAIT_FAILED;
                            break;
                        }
                    }
                }
            } else {
                nErrorCode = GetData_unsafe(pData, nDataSize);
                Unlock();
                break;
            }
        }
    }
    
    if (nErrorCode == SMF_ERR_OK) {
        m_oBufNotFullEvent.SetEvent();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_CyclicBuf::PutData_unsafe(unsigned char *pData, unsigned int nDataSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;
    
    for (i = 0; i < nDataSize; i++) {
        m_pBufAddr[m_pBufControlBlock->nPosToWrite] = pData[i];
        m_pBufControlBlock->nPosToWrite++;
        if (m_pBufControlBlock->nPosToWrite >= m_pBufControlBlock->nBufSize) {
            m_pBufControlBlock->nPosToWrite = 0;
        }
        m_pBufControlBlock->nSizeCanRead++;
        m_pBufControlBlock->nSizeCanWrite--;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_CyclicBuf::GetData_unsafe(unsigned char *pData, unsigned int nDataSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;
    
    for (i = 0; i < nDataSize; i++) {
        pData[i] = m_pBufAddr[m_pBufControlBlock->nPosToRead];
        m_pBufControlBlock->nPosToRead++;
        if (m_pBufControlBlock->nPosToRead >= m_pBufControlBlock->nBufSize) {
            m_pBufControlBlock->nPosToRead = 0;
        }
        m_pBufControlBlock->nSizeCanWrite++;
        m_pBufControlBlock->nSizeCanRead--;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_CyclicBuf::UnPutData(unsigned int nDataSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBufControlBlock == NULL) {
            nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_INIT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        if (nDataSize > m_pBufControlBlock->nSizeCanRead) {
            nErrorCode = SMF_ERR_FAILED;
        } else {
            for (i = 0; i < nDataSize; i++) {
                if (m_pBufControlBlock->nPosToWrite == 0) {
                    m_pBufControlBlock->nPosToWrite = m_pBufControlBlock->nBufSize;
                } else {
                    m_pBufControlBlock->nPosToWrite--;
                }
                m_pBufControlBlock->nSizeCanRead--;
                m_pBufControlBlock->nSizeCanWrite++;
            }
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_CyclicBuf::UnGetData(unsigned int nDataSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBufControlBlock == NULL) {
            nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_INIT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        if (nDataSize > m_pBufControlBlock->nSizeCanWrite) {
            nErrorCode = SMF_ERR_FAILED;
        } else {
            for (i = 0; i < nDataSize; i++) {
                if (m_pBufControlBlock->nPosToRead == 0) {
                    m_pBufControlBlock->nPosToRead = m_pBufControlBlock->nBufSize;
                } else {
                    m_pBufControlBlock->nPosToRead--;
                }
                m_pBufControlBlock->nSizeCanWrite--;
                m_pBufControlBlock->nSizeCanRead++;
            }
        }
        Unlock();
    }

    return nErrorCode;
}

unsigned int SMF_CyclicBuf::GetSizeCanWrite()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int nSize = 0;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBufControlBlock == NULL) {
            nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_INIT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        nSize = m_pBufControlBlock->nSizeCanWrite;
        Unlock();
    }

    return nSize;
}

unsigned int SMF_CyclicBuf::GetSizeCanRead()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int nSize = 0;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pBufControlBlock == NULL) {
            nErrorCode = SMF_ERR_CYCLIC_BUF_NOT_INIT;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        nSize = m_pBufControlBlock->nSizeCanRead;
        Unlock();
    }

    return nSize;
}

SMF_ErrorCode SMF_CyclicBuf::InitBufControlBlock()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned char *pBufTmp = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRawBufAddr == NULL) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pBufTmp = reinterpret_cast<unsigned char *>(SMF_ALIGNED(reinterpret_cast<unsigned long>(m_pRawBufAddr)));
        if (pBufTmp + BUF_CONTROL_BLOCK_SIZE > m_pRawBufAddr + m_nRawBufSize) {
            nErrorCode = SMF_ERR_TOO_SMALL_BUFFER;
        }
    }
    
    if (nErrorCode == SMF_ERR_OK) {
        m_pBufControlBlock = reinterpret_cast<BufControlBlock *>(pBufTmp);
        m_pBufAddr = pBufTmp + BUF_CONTROL_BLOCK_SIZE;
        if (m_bBuildControlBlock) {
            Lock();
            m_pBufControlBlock->nBufSize = m_pRawBufAddr + m_nRawBufSize - m_pBufAddr;
            m_pBufControlBlock->nPosToWrite = 0;
            m_pBufControlBlock->nSizeCanWrite = m_pBufControlBlock->nBufSize;
            m_pBufControlBlock->nPosToRead = 0;
            m_pBufControlBlock->nSizeCanRead = 0;
            Unlock();
        }
    }

    return nErrorCode;
}

void SMF_CyclicBuf::Lock()
{
    m_oLock.Lock();
}

void SMF_CyclicBuf::Unlock()
{
    m_oLock.Unlock();
}

std::string SMF_CyclicBuf::GetLockName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_Lock", m_sBufName.c_str());

    return sRet;
}

std::string SMF_CyclicBuf::GetBufNotEmptyEventName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_BufNotEmptyEvent", m_sBufName.c_str());

    return sRet;
}

std::string SMF_CyclicBuf::GetBufNotFullEventName() const
{
    std::string sRet;

    TK_Tools::FormatStr(sRet, "%s_BufNotFullEvent", m_sBufName.c_str());

    return sRet;
}
