#include <mutex>
#include <chrono>
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
    std::string m_sName;
    std::timed_mutex m_mutex;
};

SMF_PlatformLockImpl::SMF_PlatformLockImpl(SMF_PlatformLock *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_sName(pName != NULL ? pName : "")
    , m_mutex()
{
    std::string sMutexName = GetMutexName();
}

SMF_PlatformLockImpl::~SMF_PlatformLockImpl()
{

}

SMF_ErrorCode SMF_PlatformLockImpl::Lock(unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (nTimeout == SMF_INFINITE) {
            m_mutex.lock();
        } else {
            m_mutex.try_lock_for(std::chrono::milliseconds(nTimeout));
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::TryLock(bool &bLocked)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        bLocked = m_mutex.try_lock();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::Unlock()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_mutex.unlock();
    }

    return nErrorCode;
}

std::string SMF_PlatformLockImpl::GetMutexName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformLockImpl_Mutex_%s", SMF_GetDomainStr(false).c_str(), m_sName.c_str());
    
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
