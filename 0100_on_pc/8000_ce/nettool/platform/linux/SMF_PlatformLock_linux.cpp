#include "TK_Tools.h"
#include "SMF_PlatformLock.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformLockImpl
//

//
// (!!!)NOTE: recursive lock is not supported which will cause dead lock in this implementation.
//
class SMF_PlatformLockImpl : private SMF_NonCopyable
{
public:
    SMF_PlatformLockImpl(SMF_PlatformLock *pApi, const char *pName = NULL);
    virtual ~SMF_PlatformLockImpl();

public:
    SMF_ErrorCode Lock(unsigned int nTimeout = SMF_INFINITE);
    SMF_ErrorCode TryLock(bool &bLocked);
    SMF_ErrorCode Unlock();

private:
    SMF_ErrorCode CreateSem();
    SMF_ErrorCode DestroySem();

private:
    std::string GetSemName() const;

private:
    SMF_PlatformLock *m_pApi;
    sem_t *m_pSem;
    sem_t m_oSemWithoutName;
    const char *m_pName;
    std::string m_sName;
};

SMF_PlatformLockImpl::SMF_PlatformLockImpl(SMF_PlatformLock *pApi, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_pSem(NULL)
    , m_oSemWithoutName()
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
{
    CreateSem();
}

SMF_PlatformLockImpl::~SMF_PlatformLockImpl()
{
    DestroySem();
}

SMF_ErrorCode SMF_PlatformLockImpl::CreateSem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    bool bSuc;
    bool bRetained = false;

    if (m_pName == NULL) {
        if (nErrorCode == SMF_ERR_OK) {
            m_pSem = &m_oSemWithoutName;
        }

        if (nErrorCode == SMF_ERR_OK) {
            nRet = ::sem_init(m_pSem, 0, 1);
            if (nRet != 0) {
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_SEM;
                m_pSem = NULL;
            }
        }
    } else {
        std::string sSemName = GetSemName();

        if (nErrorCode == SMF_ERR_OK) {
            if (!TK_Tools::ObjectFileExists(sSemName)) {
                ::sem_unlink(sSemName.c_str());
            }

            bSuc = TK_Tools::RetainObjectFile(sSemName);
            if (!bSuc) {
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_SEM;
            } else {
                bRetained = true;
            }
        }

        if (nErrorCode == SMF_ERR_OK) {
            m_pSem = ::sem_open(sSemName.c_str(), O_CREAT | O_RDWR, 0666, 1);
            if (m_pSem == SEM_FAILED) {
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_SEM;
                m_pSem = NULL;
            }
        }

        if (nErrorCode != SMF_ERR_OK) {
            if (bRetained) {
                TK_Tools::ReleaseObjectFile(sSemName);
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::DestroySem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    bool bSuc;

    if (m_pName == NULL) {
        if (nErrorCode == SMF_ERR_OK) {
            if (m_pSem != NULL) {
                nRet = ::sem_destroy(m_pSem);
                if (nRet != 0) {
                    nErrorCode = SMF_ERR_FAILED_TO_DESTROY_SEM;
                }
                m_pSem = NULL;
            }
        }
    } else {
        std::string sSemName = GetSemName();

        if (nErrorCode == SMF_ERR_OK) {
            if (m_pSem != NULL) {
                nRet = ::sem_close(m_pSem);
                if (nRet != 0) {
                    nErrorCode = SMF_ERR_FAILED_TO_DESTROY_SEM;
                }
                m_pSem = NULL;

                bSuc = TK_Tools::ReleaseObjectFile(sSemName);
                if (!bSuc) {
                    nErrorCode = SMF_ERR_FAILED_TO_DESTROY_SEM;
                }
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::Lock(unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSem == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        struct timespec ts;
        TK_Tools::GetAbsoluteTimeSpec(nTimeout, ts);

        if (nTimeout == SMF_INFINITE) {
            do {
                nRet = ::sem_wait(m_pSem);
                if (nRet == -1 && errno == EINTR) {
                    continue;
                }
            } while (false);
        } else {
            do {
                nRet = ::sem_timedwait(m_pSem, &ts);
                if (nRet == -1 && errno == EINTR) {
                    continue;
                }
            } while (false);
        }
        switch (nRet) {
        case 0:
            {
                // do nothing
            }
            break;
        case -1:
        default:
            {
                if (errno == ETIMEDOUT) {
                    nErrorCode = SMF_ERR_TIMEOUT;
                } else {
                    nErrorCode = SMF_ERR_FAILED;
                }
            }
            break;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::TryLock(bool &bLocked)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    bLocked = false;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSem == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nRet = ::sem_trywait(m_pSem);
        switch (nRet) {
        case 0:
            {
                bLocked = true;
            }
            break;
        case -1:
        default:
            {
                if (errno == EAGAIN) {
                    nErrorCode = SMF_ERR_EAGAIN;
                } else {
                    nErrorCode = SMF_ERR_FAILED;
                }
            }
            break;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformLockImpl::Unlock()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSem == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nRet = ::sem_post(m_pSem);
        if (nRet != 0) {
            nErrorCode = SMF_ERR_FAILED;
        }
    }

    return nErrorCode;
}

std::string SMF_PlatformLockImpl::GetSemName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("/SMF_PlatformLockImpl_sem_%s", m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformLock
//
SMF_PlatformLock::SMF_PlatformLock(const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformLockImpl(this, pName);
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
