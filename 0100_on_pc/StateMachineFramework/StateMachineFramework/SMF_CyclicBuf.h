#ifndef SMF_CYCLIC_BUF_H__738950928746890285902104328589235094358943899
#define SMF_CYCLIC_BUF_H__738950928746890285902104328589235094358943899

#include "SMF_afx.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformEvent.h"

class SMF_CyclicBuf : private SMF_NonCopyable
{
private:
    struct BufControlBlock {
        BufControlBlock() : nBufSize(0), nPosToWrite(0), nPosToRead(0), nSizeCanWrite(0), nSizeCanRead(0) {}

        unsigned int nBufSize;
        unsigned int nPosToWrite;
        unsigned int nPosToRead;
        unsigned int nSizeCanWrite;
        unsigned int nSizeCanRead;
    };
    static const int BUF_CONTROL_BLOCK_SIZE = SMF_ALIGNED_SIZE(BufControlBlock);

public:
    SMF_CyclicBuf(unsigned char *pRawBufAddr, unsigned int nRawBufSize, const std::string &sBufName, bool bSharedInProcesses = false, bool bBuildControlBlock = true);
    virtual ~SMF_CyclicBuf();

    SMF_ErrorCode PutData(unsigned char *pData, unsigned int nDataSize, bool bWait = true);
    SMF_ErrorCode GetData(unsigned char *pData, unsigned int nDataSize, bool bWait = true);

    SMF_ErrorCode UnPutData(unsigned int nDataSize);
    SMF_ErrorCode UnGetData(unsigned int nDataSize);

    unsigned int GetSizeCanWrite();
    unsigned int GetSizeCanRead();

private:
    SMF_ErrorCode InitBufControlBlock();

    SMF_ErrorCode PutData_unsafe(unsigned char *pData, unsigned int nDataSize);
    SMF_ErrorCode GetData_unsafe(unsigned char *pData, unsigned int nDataSize);

private:
    void Lock();
    void Unlock();

private:
    std::string GetLockName() const;
    std::string GetBufNotEmptyEventName() const;
    std::string GetBufNotFullEventName() const;

private:
    BufControlBlock *m_pBufControlBlock;
    unsigned char *m_pBufAddr;
    unsigned char *m_pRawBufAddr;
    unsigned int m_nRawBufSize;
    const std::string m_sBufName;
    bool m_bSharedInProcesses;
    bool m_bBuildControlBlock;
    SMF_PlatformLock m_oLock;
    SMF_PlatformEvent m_oBufNotEmptyEvent;
    SMF_PlatformEvent m_oBufNotFullEvent;
};

#endif // #ifndef SMF_CYCLIC_BUF_H__738950928746890285902104328589235094358943899
