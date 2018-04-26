#include "TK_Tools.h"
#include "SMF_PlatformSharedMem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformSharedMemImpl
//
class SMF_PlatformSharedMemImpl : private SMF_NonCopyable
{
public:
    SMF_PlatformSharedMemImpl(SMF_PlatformSharedMem *pApi, unsigned int nSharedMemSize, const char *pName = NULL);
    virtual ~SMF_PlatformSharedMemImpl();

public:
    SMF_ErrorCode Map();
    SMF_ErrorCode Unmap();

public:
    bool AlreadyExists();
    void *GetSharedMemAddr();

private:
    SMF_ErrorCode CreateSharedMem();
    SMF_ErrorCode DestroySharedMem();

private:
    std::string GetSharedMemName() const;

private:
    SMF_PlatformSharedMem *m_pApi;
    HANDLE m_hSharedMem;
    bool m_bAlreadyExists;
    void *m_pSharedMemAddr;
    unsigned int m_nSharedMemSize;
    const char *m_pName;
    std::string m_sName;
};

SMF_PlatformSharedMemImpl::SMF_PlatformSharedMemImpl(SMF_PlatformSharedMem *pApi, unsigned int nSharedMemSize, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_hSharedMem(NULL)
    , m_bAlreadyExists(false)
    , m_pSharedMemAddr(NULL)
    , m_nSharedMemSize(nSharedMemSize)
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
{
    CreateSharedMem();
}

SMF_PlatformSharedMemImpl::~SMF_PlatformSharedMemImpl()
{
    Unmap();
    DestroySharedMem();
}

SMF_ErrorCode SMF_PlatformSharedMemImpl::CreateSharedMem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    std::string sSharedMemName = GetSharedMemName();

    if (nErrorCode == SMF_ERR_OK) {
        m_hSharedMem = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_nSharedMemSize, 
            (m_pName == NULL ? NULL : TK_Tools::str2wstr(sSharedMemName).c_str()));
        if (m_hSharedMem != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
            m_bAlreadyExists = true;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hSharedMem == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformSharedMemImpl::DestroySharedMem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hSharedMem != NULL) {
            ::UnmapViewOfFile(m_hSharedMem);
            m_hSharedMem = NULL;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformSharedMemImpl::Map()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hSharedMem == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMemAddr == NULL) {
            m_pSharedMemAddr = ::MapViewOfFile(m_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, m_nSharedMemSize);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMemAddr == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformSharedMemImpl::Unmap()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hSharedMem == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMemAddr != NULL) {
            ::UnmapViewOfFile(m_pSharedMemAddr);
            m_pSharedMemAddr = NULL;
        }
    }

    return nErrorCode;
}

bool SMF_PlatformSharedMemImpl::AlreadyExists()
{
    return m_bAlreadyExists;
}

void *SMF_PlatformSharedMemImpl::GetSharedMemAddr()
{
    return m_pSharedMemAddr;
}

std::string SMF_PlatformSharedMemImpl::GetSharedMemName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformSharedMemImpl_FileMapping_%s", m_sName.c_str());
    
    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformSharedMem
//
SMF_PlatformSharedMem::SMF_PlatformSharedMem(unsigned int nSharedMemSize, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformSharedMemImpl(this, nSharedMemSize, pName);
}

SMF_PlatformSharedMem::~SMF_PlatformSharedMem()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformSharedMem::Map()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->Map();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformSharedMem::Unmap()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->Unmap();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

bool SMF_PlatformSharedMem::AlreadyExists()
{
    bool bAlreadyExits = false;

    if (m_pImpl != NULL) {
        bAlreadyExits = m_pImpl->AlreadyExists();
    }

    return bAlreadyExits;
}

void *SMF_PlatformSharedMem::GetSharedMemAddr()
{
    void *pSharedMemAddr = NULL;

    if (m_pImpl != NULL) {
        pSharedMemAddr = m_pImpl->GetSharedMemAddr();
    }

    return pSharedMemAddr;
}
