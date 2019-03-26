#include "TK_Tools.h"
#include "SMF_PlatformLock.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformLockImpl
//
class SMF_PlatformLockImpl : private SMF_NonCopyable
{
public:
    SMF_PlatformLockImpl(SMF_PlatformLock *pApi, const char *pName = NULL, bool bSharedInProcesses = false);
    virtual ~SMF_PlatformLockImpl();

public:
    SMF_ErrorCode Lock(unsigned int nTimeout = SMF_INFINITE);
    SMF_ErrorCode TryLock(bool &bLocked);
    SMF_ErrorCode Unlock();

private:
    std::string GetMutexName() const;

private:
    SMF_PlatformLock *m_pApi;
    HANDLE m_hMutex;
    std::string m_sName;
    const char *m_pName;
    bool m_bSharedInProcesses;
};

SMF_PlatformLockImpl::SMF_PlatformLockImpl(SMF_PlatformLock *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_hMutex(NULL)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bSharedInProcesses(bSharedInProcesses)
{
    std::string sMutexName = GetMutexName();

    m_hMutex = ::CreateMutex(NULL, FALSE, (pName == NULL ? NULL : TK_Tools::str2wstr(sMutexName).c_str()));
}

SMF_PlatformLockImpl::~SMF_PlatformLockImpl()
{
    if (m_hMutex != NULL) {
        ::CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}

SMF_ErrorCode SMF_PlatformLockImpl::Lock(unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hMutex == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::WaitForSingleObject(m_hMutex, (nTimeout == SMF_INFINITE ? INFINITE : nTimeout));
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::TryLock(bool &bLocked)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    DWORD dwRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hMutex == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        dwRet = ::WaitForSingleObject(m_hMutex, 0);
        if (dwRet == WAIT_TIMEOUT) {
            bLocked = false;
        } else {
            bLocked = true;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::Unlock()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hMutex == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::ReleaseMutex(m_hMutex);
    }

    return nErrorCode;
}

std::string SMF_PlatformLockImpl::GetMutexName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformLockImpl_Mutex_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());
    
    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformLock
//
SMF_PlatformLock::SMF_PlatformLock(const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformLockImpl(this, pName, bSharedInProcesses);
}

SMF_PlatformLock::~SMF_PlatformLock()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformLock::Lock(unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->Lock(nTimeout);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLock::TryLock(bool &bLocked)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->TryLock(bLocked);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLock::Unlock()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->Unlock();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}
