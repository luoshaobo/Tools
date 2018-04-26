#include "TK_Tools.h"
#include "SMF_PlatformEvent.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformEventImpl
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
    std::string GetEventName() const;

private:
    SMF_PlatformEvent *m_pApi;
    HANDLE m_hEvent;
    bool m_bManualReset;
    bool m_bInitSignaled;
    const char *m_pName;
    std::string m_sName;
};

SMF_PlatformEventImpl::SMF_PlatformEventImpl(SMF_PlatformEvent *pApi, bool bManualReset, bool bInitSignaled, const char *pName /*= NULL*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_hEvent(NULL)
    , m_bManualReset(bManualReset)
    , m_bInitSignaled(bInitSignaled)
    , m_pName(pName)
    , m_sName(pName != NULL ? pName : "")
{
    std::string sEventName = GetEventName();
    m_hEvent = ::CreateEvent(NULL, (m_bManualReset ? TRUE : FALSE), (bInitSignaled ? TRUE : FALSE), 
        (pName == NULL ? NULL : TK_Tools::str2wstr(sEventName).c_str()));
}

SMF_PlatformEventImpl::~SMF_PlatformEventImpl()
{
    if (m_hEvent != NULL) {
        ::CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }
}

SMF_ErrorCode SMF_PlatformEventImpl::SetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hEvent == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::SetEvent(m_hEvent);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::ResetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hEvent == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::ResetEvent(m_hEvent);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::WaitForSingleEvent(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent *pEvent, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    DWORD dwRet;

    if (nErrorCode == SMF_ERR_OK) {
        if (pEvent == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        } else if (pEvent->m_pImpl == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        } else if (pEvent->m_pImpl->m_hEvent == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        dwRet = ::WaitForSingleObject(pEvent->m_pImpl->m_hEvent, (nTimeout == SMF_INFINITE ? INFINITE : nTimeout));
        switch (dwRet) {
        case WAIT_TIMEOUT:
            {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_TIMEOUT;
            }
            break;
        case WAIT_OBJECT_0:
            {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_OBJECT_0;
            }
            break;
        case WAIT_FAILED:
        default:
            {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_FAILED;
                nErrorCode = SMF_ERR_WAIT_FAILED;
            }
            break;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::WaitForMultipleEvents(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent **ppEvents, unsigned int nEventCount, bool bWaitAll /*= false*/, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    DWORD dwRet;
    unsigned int i;
    SMF_PlatformEvent *pEvent;
    HANDLE *pEventHandles = NULL;

    if (nErrorCode == SMF_ERR_OK) {
        if (ppEvents == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < nEventCount; i++) {
            pEvent = ppEvents[i];
            if (pEvent == NULL) {
                nErrorCode = SMF_ERR_NULL_PTR;
                break;
            } else if (pEvent->m_pImpl == NULL) {
                nErrorCode = SMF_ERR_NULL_PTR;
                break;
            } else if (pEvent->m_pImpl->m_hEvent == NULL) {
                nErrorCode = SMF_ERR_INVALID_HANDLE;
                break;
            }
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pEventHandles = new HANDLE[nEventCount];
        if (pEventHandles == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < nEventCount; i++) {
            pEventHandles[i] = ppEvents[i]->m_pImpl->m_hEvent;
        }

        dwRet = ::WaitForMultipleObjects(nEventCount, pEventHandles, (bWaitAll ? TRUE : FALSE), (nTimeout == SMF_INFINITE ? INFINITE : nTimeout));
        switch (dwRet) {
        case WAIT_TIMEOUT:
            {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_TIMEOUT;
            }
            break;
        case WAIT_FAILED:
        default:
            {
                if (WAIT_OBJECT_0 <= dwRet && dwRet < WAIT_OBJECT_0 + nEventCount) {
                    nWaitResult = SMF_PlatformEvent::WAIT_RESULT_OBJECT_0 + (dwRet - WAIT_OBJECT_0);
                } else {
                    nWaitResult = SMF_PlatformEvent::WAIT_RESULT_FAILED;
                    nErrorCode = SMF_ERR_WAIT_FAILED;
                }
            }
            break;
        }
    }

    if (pEventHandles != NULL) {
        delete [] pEventHandles;
        pEventHandles = NULL;
    }

    return nErrorCode;
}

std::string SMF_PlatformEventImpl::GetEventName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformEventImpl_Event_%s", m_sName.c_str());
    
    return sResult;
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
