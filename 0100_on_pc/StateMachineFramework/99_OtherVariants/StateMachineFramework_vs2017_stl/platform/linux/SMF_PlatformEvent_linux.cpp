#include "TK_Tools.h"
#include "SMF_PlatformEvent.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformEventImpl
//

//
// (!!!)NOTE: WaitForMultipleObjects() is not supported in this implementation.
//
class SMF_PlatformEventImpl : private SMF_NonCopyable
{
public:
    SMF_PlatformEventImpl(SMF_PlatformEvent *pApi, bool bManualReset, bool bInitSignaled, const char *pName = NULL, bool bSharedInProcesses = false);
    virtual ~SMF_PlatformEventImpl();

public:
    SMF_ErrorCode SetEvent();
    SMF_ErrorCode ResetEvent();
    static SMF_ErrorCode WaitForSingleEvent(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent *pEvent, unsigned int nTimeout = SMF_INFINITE);
    static SMF_ErrorCode WaitForMultipleEvents(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent **ppEvents, unsigned int nEventCount, bool bWaitAll = false, unsigned int nTimeout = SMF_INFINITE);

private:
    SMF_ErrorCode CreateSem();
    SMF_ErrorCode DestroySem();

private:
    std::string GetSemName() const;

private:
    SMF_PlatformEvent *m_pApi;
    sem_t *m_pSem;
    sem_t m_oSemWithoutName;
    bool m_bManualReset;
    bool m_bInitSignaled;
    std::string m_sName;
    const char *m_pName;
    bool m_bSharedInProcesses;
};

SMF_PlatformEventImpl::SMF_PlatformEventImpl(SMF_PlatformEvent *pApi, bool bManualReset, bool bInitSignaled, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_pSem(NULL)
    , m_oSemWithoutName()
    , m_bManualReset(bManualReset)
    , m_bInitSignaled(bInitSignaled)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bSharedInProcesses(bSharedInProcesses)
{
    CreateSem();
}

SMF_PlatformEventImpl::~SMF_PlatformEventImpl()
{
    DestroySem();
}

SMF_ErrorCode SMF_PlatformEventImpl::CreateSem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    bool bSuc;
    bool bRetained = false;
    std::string sSemName;

    if (m_pName == NULL) {
        if (nErrorCode == SMF_ERR_OK) {
            m_pSem = &m_oSemWithoutName;
        }

        if (nErrorCode == SMF_ERR_OK) {
            nRet = ::sem_init(m_pSem, 0, (m_bInitSignaled ? 1 : 0));
            if (nRet != 0) {
                nErrorCode = SMF_ERR_FAILED_TO_CREATE_SEM;
                m_pSem = NULL;
            }
        }
    } else {
        sSemName = GetSemName();

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
            m_pSem = ::sem_open(sSemName.c_str(), O_CREAT | O_RDWR, 0666, (m_bInitSignaled ? 1 : 0));
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

SMF_ErrorCode SMF_PlatformEventImpl::DestroySem()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;
    bool bSuc;
    std::string sSemName;

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
        sSemName = GetSemName();

        if (nErrorCode == SMF_ERR_OK) {
            if (m_pSem != NULL) {
#if 0
                //
                // NOTE:
                // 1) All open named semaphores are automatically closed on process termination, or upon execve(2).
                // 2) A same named semaphore has the same address in a single process.
                //
                nRet = ::sem_close(m_pSem);
                if (nRet != 0) {
                    nErrorCode = SMF_ERR_FAILED_TO_DESTROY_SEM;
                }
#endif // #if 0
                m_pSem = NULL;

                bSuc = TK_Tools::ReleaseObjectFile(sSemName);
                if (!bSuc) {
                    nErrorCode = SMF_ERR_FAILED_TO_DESTROY_SEM;
                }
                if (!TK_Tools::ObjectFileExists(sSemName)) {
                    ::sem_unlink(sSemName.c_str());
                }
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::SetEvent()
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
            SMF_nPlatformErrorCode = errno;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::ResetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSem == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            nRet = ::sem_trywait(m_pSem);
            if (nRet == 0) {
                continue;
            } else {
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::WaitForSingleEvent(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent *pEvent, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    int nRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (pEvent == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        } else if (pEvent->m_pImpl == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        } else if (pEvent->m_pImpl->m_pSem == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        struct timespec ts;
        TK_Tools::GetAbsoluteTimeSpec(nTimeout, ts);

        if (nTimeout == SMF_INFINITE) {
            do {
                nRet = ::sem_wait(pEvent->m_pImpl->m_pSem);
                if (nRet == -1 && errno == EINTR) {
                    continue;
                }
            } while (false);
        } else {
            do {
                nRet = ::sem_timedwait(pEvent->m_pImpl->m_pSem, &ts);
                if (nRet == -1 && errno == EINTR) {
                    continue;
                }
            } while (false);
        }
        switch (nRet) {
        case 0:
            {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_OBJECT_0;
                if (pEvent->m_pImpl->m_bManualReset) {
                    pEvent->m_pImpl->SetEvent();
                }
            }
            break;
        case -1:
        default:
            {
                if (errno == ETIMEDOUT) {
                    nWaitResult = SMF_PlatformEvent::WAIT_RESULT_TIMEOUT;
                } else {
                    nWaitResult = SMF_PlatformEvent::WAIT_RESULT_FAILED;
                    nErrorCode = SMF_ERR_WAIT_FAILED;
                }
            }
            break;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::WaitForMultipleEvents(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent **ppEvents, unsigned int nEventCount, bool bWaitAll /*= false*/, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = SMF_ERR_NOT_SUPPORTED;

    return nErrorCode;
}

std::string SMF_PlatformEventImpl::GetSemName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformEventImpl_sem_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformEvent
//

SMF_PlatformEvent::SMF_PlatformEvent(bool bManualReset, bool bInitSignaled, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformEventImpl(this, bManualReset, bInitSignaled, pName, bSharedInProcesses);
}

SMF_PlatformEvent::~SMF_PlatformEvent()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformEvent::SetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetEvent();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEvent::ResetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ResetEvent();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEvent::WaitForSingleEvent(WaitResult &nWaitResult, SMF_PlatformEvent *pEvent, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = SMF_PlatformEventImpl::WaitForSingleEvent(nWaitResult, pEvent, nTimeout);

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEvent::WaitForMultipleEvents(WaitResult &nWaitResult, SMF_PlatformEvent **ppEvents, unsigned int nEventCount, bool bWaitAll /*= false*/, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = SMF_PlatformEventImpl::WaitForMultipleEvents(nWaitResult, ppEvents, nEventCount, bWaitAll, nTimeout);

    return nErrorCode;
}
