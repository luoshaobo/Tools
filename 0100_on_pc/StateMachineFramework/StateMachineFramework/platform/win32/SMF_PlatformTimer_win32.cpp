#include "TK_Tools.h"
#include "SMF_PlatformTimer.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimerImpl
//
class SMF_PlatformTimerImpl : private SMF_NonCopyable, public SMF_PlatformTimerHandler
{
private:
    typedef std::map<SMF_PlatformTimer::TimerId, bool> TimerRepeatMap;

public:
    SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName = NULL, bool bSharedInProcesses = false);
    virtual ~SMF_PlatformTimerImpl();
    
public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat = false);
    SMF_ErrorCode StopTimer(TimerId nTimerId);

public:
    SMF_ErrorCode SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler);

public:
    SMF_PlatformThread *GetConnectedThread();
    SMF_PlatformTimer::TimerId GetTimerId();

public:
    virtual SMF_ErrorCode PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId);

private:
    SMF_ErrorCode CreateInternalWindow();
    SMF_ErrorCode DestroyInternalWindow();

private:
    static LRESULT CALLBACK WinWndProcStatic(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
    LRESULT WinWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
    static void CALLBACK WinTimerProcStatic(HWND hWnd, UINT nMsg, UINT nEvent, DWORD nTime);
    void WinTimerProc(HWND hWnd, UINT nMsg, UINT nTimerId, DWORD nTime);

private:
    std::string GetWindowClassName() const;
    std::string GetWindowName() const;
    SMF_PlatformTimerHandler *GetTimerHandler();

    void SetTimerRepeat(SMF_PlatformTimer::TimerId nTimerId, bool bRepeat);
    bool GetTimerRepeat(SMF_PlatformTimer::TimerId nTimerId);
    void RemoveTimerRepeat(SMF_PlatformTimer::TimerId nTimerId);

private:
    void Lock();
    void Unlock();

private:
    static unsigned int m_nPlatformTimerSerialNo;
    SMF_PlatformTimer *m_pApi;
    unsigned int m_nSerialNo;
    std::string m_sName;
    const char *m_pName;
    bool m_bSharedInProcesses;
    HWND m_hWnd;
    ATOM m_atomWndCls;
    SMF_PlatformTimerHandler *m_pTimerHandler;
    SMF_PlatformLock m_oLock;
    TimerRepeatMap m_mapTimeRepeat;
    SMF_PlatformThread *m_pConnectedThread;
};

unsigned int SMF_PlatformTimerImpl::m_nPlatformTimerSerialNo = 0;

SMF_PlatformTimerImpl::SMF_PlatformTimerImpl(SMF_PlatformTimer *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_nSerialNo(m_nPlatformTimerSerialNo++)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bSharedInProcesses(bSharedInProcesses)
    , m_hWnd(NULL)
    , m_atomWndCls(0)
    , m_pTimerHandler(NULL)
    , m_oLock()
    , m_mapTimeRepeat()
    , m_pConnectedThread(NULL)
{
    CreateInternalWindow();
}

SMF_PlatformTimerImpl::~SMF_PlatformTimerImpl()
{
    DestroyInternalWindow();
}

SMF_ErrorCode SMF_PlatformTimerImpl::CreateInternalWindow()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    std::wstring wsWindowClassName = TK_Tools::str2wstr(GetWindowClassName());
    std::wstring wsWindowName = TK_Tools::str2wstr(GetWindowName());

    if (nErrorCode == SMF_ERR_OK) {
        WNDCLASS wc;

	    ::memset(&wc, 0, sizeof(WNDCLASS));
	    wc.style = CS_NOCLOSE;
	    wc.lpfnWndProc = &WinWndProcStatic;
        wc.lpszClassName = wsWindowClassName.c_str();

	    m_atomWndCls = ::RegisterClass(&wc);
	    if (m_atomWndCls == 0) {
		    nErrorCode = SMF_ERR_FAILED_TO_CREATE_PLATFORM_MSGQ;
	    }
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_hWnd = ::CreateWindow(
            wsWindowClassName.c_str(),
            wsWindowName.c_str(),
			WS_OVERLAPPED, 
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, ::GetModuleHandle(NULL), NULL
        );
        if (m_hWnd == NULL) {
		    nErrorCode = SMF_ERR_FAILED_TO_CREATE_PLATFORM_MSGQ;
	    }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::SetWindowLong(m_hWnd, GWL_USERDATA, reinterpret_cast<LONG>(this));
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::DestroyInternalWindow()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    std::wstring wsWindowClassName = TK_Tools::str2wstr(GetWindowClassName());
    std::wstring wsWindowName = TK_Tools::str2wstr(GetWindowName());

    if (m_hWnd != NULL) {
        ::DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }
    if (m_atomWndCls != 0) {
        ::UnregisterClass(wsWindowClassName.c_str(), ::GetModuleHandle(NULL));
        m_atomWndCls = 0;
    }

    return nErrorCode;
}

LRESULT CALLBACK SMF_PlatformTimerImpl::WinWndProcStatic(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    SMF_PlatformTimerImpl *pThis = reinterpret_cast<SMF_PlatformTimerImpl *>(::GetWindowLong(hWnd, GWL_USERDATA));
    
    if (pThis != NULL) {
        lResult = pThis->WinWndProc(hWnd, nMsg, wParam, lParam);
    } else {
        lResult = ::DefWindowProc(hWnd, nMsg, wParam, lParam);
    }
    
    return lResult;
}

LRESULT SMF_PlatformTimerImpl::WinWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;

    lResult = ::DefWindowProc(hWnd, nMsg, wParam, lParam);
    
    return lResult;
}

void CALLBACK SMF_PlatformTimerImpl::WinTimerProcStatic(HWND hWnd, UINT nMsg, UINT nTimerId, DWORD nTime)
{
    SMF_PlatformTimerImpl *pThis = reinterpret_cast<SMF_PlatformTimerImpl *>(::GetWindowLong(hWnd, GWL_USERDATA));

    if (pThis != NULL) {
        pThis->WinTimerProc(hWnd, nMsg, nTimerId, nTime);
    } else {
        ::KillTimer(hWnd, nTimerId);
    }
}

void SMF_PlatformTimerImpl::WinTimerProc(HWND hWnd, UINT nMsg, UINT nTimerId, DWORD nTime)
{
    SMF_PlatformTimerHandler *pTimerHandler = GetTimerHandler();
    
    if (pTimerHandler != NULL) {
        if (!GetTimerRepeat(nTimerId)) {
            ::KillTimer(hWnd, nTimerId);
            RemoveTimerRepeat(nTimerId);
        }
        pTimerHandler->PlatformTimerProc(*m_pApi, nTimerId);
    } else {
        ::KillTimer(hWnd, nTimerId);
        RemoveTimerRepeat(nTimerId);
    }
}

SMF_ErrorCode SMF_PlatformTimerImpl::PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat /*= false*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hWnd == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::SetTimer(m_hWnd, nTimerId, nInterval, &WinTimerProcStatic);
        SetTimerRepeat(nTimerId, bRepeat);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hWnd == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::KillTimer(m_hWnd, nTimerId);
        RemoveTimerRepeat(nTimerId);
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimerImpl::SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_pTimerHandler = pTimerHandler;
    }

    return nErrorCode;
}

SMF_PlatformTimerHandler *SMF_PlatformTimerImpl::GetTimerHandler()
{
    SMF_PlatformTimerHandler *pTimerHandler = m_pApi;

    if (m_pTimerHandler != NULL) {
        pTimerHandler = m_pTimerHandler;
    }

    return pTimerHandler;
}

SMF_ErrorCode SMF_PlatformTimerImpl::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedTimer(NULL);
            Lock();
            m_pConnectedThread = NULL;
            Unlock();
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pConnectedThread = pThread;
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedTimer(this->m_pApi);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformTimerImpl::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    Lock();
    pPlatformThread = m_pConnectedThread;
    Unlock();

    return pPlatformThread;
}

void SMF_PlatformTimerImpl::Lock()
{
    m_oLock.Lock();
}

void SMF_PlatformTimerImpl::Unlock()
{
    m_oLock.Unlock();
}

std::string SMF_PlatformTimerImpl::GetWindowClassName() const 
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformTimerImpl_WindowClassName_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());
    
    return sResult;
}

std::string SMF_PlatformTimerImpl::GetWindowName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformTimerImpl_WindowName_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());
    
    return sResult;
}

void SMF_PlatformTimerImpl::SetTimerRepeat(SMF_PlatformTimer::TimerId nTimerId, bool bRepeat)
{
    m_oLock.Lock();
    TimerRepeatMap::iterator it = m_mapTimeRepeat.find(nTimerId);
    if (it != m_mapTimeRepeat.end()) {
        (*it).second = bRepeat;
    } else {
        m_mapTimeRepeat.insert(TimerRepeatMap::value_type(nTimerId, bRepeat));
    }
    m_oLock.Unlock();
}

bool SMF_PlatformTimerImpl::GetTimerRepeat(SMF_PlatformTimer::TimerId nTimerId)
{
    bool bRepeat = false;

    m_oLock.Lock();
    TimerRepeatMap::iterator it = m_mapTimeRepeat.find(nTimerId);
    if (it != m_mapTimeRepeat.end()) {
        bRepeat = (*it).second;
    }
    m_oLock.Unlock();

    return bRepeat;
}

void SMF_PlatformTimerImpl::RemoveTimerRepeat(SMF_PlatformTimer::TimerId nTimerId)
{
    m_oLock.Lock();
    TimerRepeatMap::iterator it = m_mapTimeRepeat.find(nTimerId);
    if (it != m_mapTimeRepeat.end()) {
        m_mapTimeRepeat.erase(it);
    }
    m_oLock.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformTimer
//
SMF_PlatformTimer::SMF_PlatformTimer(const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformTimerImpl(this, pName, bSharedInProcesses);
}

SMF_PlatformTimer::~SMF_PlatformTimer()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformTimer::StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat /*= false*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->StartTimer(nTimerId, nInterval, bRepeat);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::StopTimer(TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->StopTimer(nTimerId);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetTimerHandler(pTimerHandler);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformTimerProc(rPlatformTimer, nTimerId);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformTimer::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ConnectToThread(pThread);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformTimer::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    if (m_pImpl != NULL) {
        pPlatformThread = m_pImpl->GetConnectedThread();
    }

    return pPlatformThread;
}
