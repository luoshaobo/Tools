#include <mutex>
#include <condition_variable>
#include <chrono>
#include "TK_Tools.h"
#include "SMF_PlatformEvent.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformEventImpl
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
    std::string GetEventName() const;

private:
    SMF_PlatformEvent *m_pApi;
    std::string m_sName;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

SMF_PlatformEventImpl::SMF_PlatformEventImpl(SMF_PlatformEvent *pApi, bool bManualReset, bool bInitSignaled, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_sName(pName != NULL ? pName : "")
    , m_mutex()
    , m_cv()
{
    std::string sEventName = GetEventName();
}

SMF_PlatformEventImpl::~SMF_PlatformEventImpl()
{

}

SMF_ErrorCode SMF_PlatformEventImpl::SetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.notify_all();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::ResetEvent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_NOT_SUPPORTED;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::WaitForSingleEvent(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent *pEvent, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        std::unique_lock<std::mutex> lock(pEvent->m_pImpl->m_mutex);
        if (nTimeout == SMF_INFINITE) {
            pEvent->m_pImpl->m_cv.wait(lock);
            nWaitResult = SMF_PlatformEvent::WAIT_RESULT_OBJECT_0;
        } else {
            std::cv_status status = pEvent->m_pImpl->m_cv.wait_for(lock, std::chrono::milliseconds(nTimeout));
            if (status == std::cv_status::timeout) {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_TIMEOUT;
            }
            else {
                nWaitResult = SMF_PlatformEvent::WAIT_RESULT_OBJECT_0;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformEventImpl::WaitForMultipleEvents(SMF_PlatformEvent::WaitResult &nWaitResult, SMF_PlatformEvent **ppEvents, unsigned int nEventCount, bool bWaitAll /*= false*/, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_NOT_SUPPORTED;

    return nErrorCode;
}

std::string SMF_PlatformEventImpl::GetEventName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformEventImpl_Event_%s", SMF_GetDomainStr(false).c_str(), m_sName.c_str());
    
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
