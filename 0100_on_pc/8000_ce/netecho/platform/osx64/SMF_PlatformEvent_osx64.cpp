#include "Tools.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformAfx_osx64.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformEventImpl
//

//
// (!!!)NOTE: WaitForMultipleObjects() is not supported in this implementation.
//
class SMF_PlatformEventImpl : private SMF_NonCopyable
{
public:
    SMF_PlatformEventImpl(SMF_PlatformEvent *pApi, bool bManualReset, bool bInitSignaled, const char *pName = NULL);
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
    void ReleaseOtherResources();

private:
    SMF_PlatformEvent *m_pApi;
    sem_t *m_pSem;
    sem_t m_oSemWithoutName;
    bool m_bManualReset;
    bool m_bInitSignaled;
    const char *m_pName;
    std::string m_sName;
};

//
// *** NOTE:
// 1) On Mac OS X, unnamed POSIX IPC object is unsupported.
//
SMF_PlatformEventImpl::SMF_PlatformEventImpl(SMF_PlatformEvent *pApi, bool bManualReset, bool bInitSignaled, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_pSem(NULL)
    , m_oSemWithoutName()
    , m_bManualReset(bManualReset)
    , m_bInitSignaled(bInitSignaled)
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
{
    if (pName == NULL) {
        char pTemp[] = "Event_XXXXXXXX";
        m_sName = ::mktemp(pTemp);
        m_pName = m_sName.c_str();
    }
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
            m_pSem = ::sem_open(sSemName.c_str(), O_CREAT, 0666, (m_bInitSignaled ? 1 : 0));
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
                if (!TK_Tools::ObjectFileExists(sSemName)) {
                    ReleaseOtherResources();
                }
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::SetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSem == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::sem_post(m_pSem);
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
    int nRet = 0;
    int nTimeoutTemp;

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
        if (nTimeout == SMF_INFINITE) {
            do {
                nRet = ::sem_wait(pEvent->m_pImpl->m_pSem);
                if (nRet == -1 && errno == EINTR) {
                    continue;
                }
            } while (false);
        } else {
            nTimeoutTemp = static_cast<int>(nTimeout);
            while (nTimeoutTemp > 0) {
                do {
                    nRet = ::sem_trywait(pEvent->m_pImpl->m_pSem);
                    if (nRet == -1 && errno == EINTR) {
                        continue;
                    }
                } while (false);
                if (nRet == -1) {
                    if (errno != EAGAIN) {
                        break;
                    }
                } else {
                    break;
                }
                nTimeoutTemp -= SMF_PLATFORM_TRY_WAIT_INTERVAL;
                TK_Tools::Sleep(SMF_PLATFORM_TRY_WAIT_INTERVAL);
            }
            if (nTimeoutTemp <= 0) {
                nRet = -1;
                errno = ETIMEDOUT;
            }
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

//
// *** NOTE:
// 1) On Mac OS X, the length of the POSIX IPC object name must less than or equal to 30.
//
std::string SMF_PlatformEventImpl::GetSemName() const
{
    std::string sResult;
    std::string sFilePath;

    sFilePath = TK_Tools::FormatStr("%s/SMF_PlatformEventImpl_sem_%s", SMF_TEMP_DIR_ROOT, m_sName.c_str());
    TK_Tools::GetFileINode(sFilePath, sResult);
    //sResult = std::string("/") + sResult;

    return sResult;
}

void SMF_PlatformEventImpl::ReleaseOtherResources()
{
    std::string sFilePath;

    sFilePath = TK_Tools::FormatStr("%s/SMF_PlatformEventImpl_sem_%s", SMF_TEMP_DIR_ROOT, m_sName.c_str());
    TK_Tools::DeleteFile(sFilePath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformEvent
//

SMF_PlatformEvent::SMF_PlatformEvent(bool bManualReset, bool bInitSignaled, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformEventImpl(this, bManualReset, bInitSignaled, pName);
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
