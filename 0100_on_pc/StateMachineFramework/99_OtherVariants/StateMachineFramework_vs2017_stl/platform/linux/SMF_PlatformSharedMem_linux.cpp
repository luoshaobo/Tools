#include "TK_Tools.h"
#include "SMF_PlatformSharedMem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformSharedMemImpl
//

class SMF_PlatformSharedMemImpl : private SMF_NonCopyable
{
public:
    SMF_PlatformSharedMemImpl(SMF_PlatformSharedMem *pApi, unsigned int nSharedMemSize, const char *pName = NULL, bool bSharedInProcesses = false);
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
    int m_oSharedMem;
    bool m_bAlreadyExists;
    void *m_pSharedMemAddr;
    unsigned int m_oSharedMemSize;
    std::string m_sName;
    const char *m_pName;
    bool m_bSharedInProcesses;
};

SMF_PlatformSharedMemImpl::SMF_PlatformSharedMemImpl(SMF_PlatformSharedMem *pApi, unsigned int nSharedMemSize, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_oSharedMem(-1)
    , m_bAlreadyExists(false)
    , m_pSharedMemAddr(NULL)
    , m_oSharedMemSize(nSharedMemSize)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bSharedInProcesses(bSharedInProcesses)
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
    int nRet;
    bool bSuc;
    std::string sSharedMemName = GetSharedMemName();
    bool bRetained = false;

    if (nErrorCode == SMF_ERR_OK) {
        if (!TK_Tools::ObjectFileExists(sSharedMemName)) {
            ::shm_unlink(sSharedMemName.c_str());
        }

        bSuc = TK_Tools::RetainObjectFile(sSharedMemName);
        if (!bSuc) {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_SHM;
        } else {
            bRetained = true;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_oSharedMem = ::shm_open(sSharedMemName.c_str(), O_CREAT | O_EXCL | O_RDWR, 0666);
        if (m_oSharedMem >= 0) {
            nRet = ::ftruncate(m_oSharedMem, m_oSharedMemSize);
            if (nRet != 0) {
                ::shm_unlink(sSharedMemName.c_str());
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_SHM;
            }
        } else if (m_oSharedMem < 0 && errno == EEXIST) {
            m_oSharedMem = ::shm_open(sSharedMemName.c_str(), O_RDWR, 0666);
            if (m_oSharedMem >= 0) {
                m_bAlreadyExists = true;
            } else {
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_SHM;
            }
        } else {
            nErrorCode = SMF_ERR_FAILED_TO_CREATE_SHM;
        }
    }

    if (nErrorCode != SMF_ERR_OK) {
        if (bRetained) {
            TK_Tools::ReleaseObjectFile(sSharedMemName);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformSharedMemImpl::DestroySharedMem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    std::string sSharedMemName = GetSharedMemName();
    bool bSuc;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_oSharedMem >= 0) {
            m_oSharedMem = -1;

            bSuc = TK_Tools::ReleaseObjectFile(sSharedMemName);
            if (!bSuc) {
                nErrorCode = SMF_ERR_FAILED_TO_DESTROY_SHM;
            }
            if (!TK_Tools::ObjectFileExists(sSharedMemName)) {
                ::shm_unlink(sSharedMemName.c_str());
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformSharedMemImpl::Map()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_oSharedMem < 0) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMemAddr == NULL) {
            m_pSharedMemAddr = ::mmap(NULL, m_oSharedMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_oSharedMem, 0);
            if (m_pSharedMemAddr == MAP_FAILED) {
                m_pSharedMemAddr = NULL;
            }
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
        if (m_oSharedMem < 0) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedMemAddr != NULL) {
            ::munmap(m_pSharedMemAddr, m_oSharedMemSize);
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

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformSharedMemImpl_shm_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformSharedMem
//
SMF_PlatformSharedMem::SMF_PlatformSharedMem(unsigned int nSharedMemSize, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformSharedMemImpl(this, nSharedMemSize, pName, bSharedInProcesses);
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