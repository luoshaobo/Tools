#include "TK_Tools.h"
#include "SMF_PlatformMsgQ.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformMsgQImpl
//
class SMF_PlatformMsgQImpl : private SMF_NonCopyable, public SMF_PlatformMsgQHandler
{
public:
    SMF_PlatformMsgQImpl(SMF_PlatformMsgQ *m_pApi, const char *pName = NULL, bool bSharedInProcesses = false, bool bRemoteAgent = false);
    virtual ~SMF_PlatformMsgQImpl();

public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode PostMsg(const SMF_PlatformMsgQ::Msg &rMsg);
    SMF_ErrorCode SendMsg(const SMF_PlatformMsgQ::Msg &rMsg);

    SMF_ErrorCode EnterMsgLoop();
    SMF_ErrorCode ExitMsgLoop();

    SMF_ErrorCode PostQuitMsg();

public:
    SMF_ErrorCode SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler);

public:
    SMF_PlatformThread *GetConnectedThread();

public:
    virtual SMF_ErrorCode PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, Msg &rMsg);

private:
    SMF_ErrorCode CreateInternalWindow();
    SMF_ErrorCode DestroyInternalWindow();

private:
    static LRESULT CALLBACK WinWndProcStatic(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
    LRESULT WinWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
    SMF_PlatformMsgQHandler *GetMsgQHandler();

private:
    HWND GetInternalWnd();
    HWND CloseInternalWnd();

private:
    void Lock();
    void Unlock();

private:
    SMF_ErrorCode NotSupportedInRemoteAgent();

private:
    std::string GetWindowClassName() const;
    std::string GetWindowName() const;
    std::string GetLockName() const;

private:
    static unsigned int m_nPlatformMsgQSerialNo;
    SMF_PlatformMsgQ *m_pApi;
    unsigned int m_nSerialNo;
    std::string m_sName;
    const char *m_pName;
    bool m_bSharedInProcesses;
    bool m_bRemoteAgent;
    HWND m_hWnd;
    ATOM m_atomWndCls;
    SMF_PlatformMsgQHandler *m_pMsgQHandler;
    SMF_PlatformLock m_oLock;
    SMF_PlatformThread *m_pConnectedThread;
};

unsigned int SMF_PlatformMsgQImpl::m_nPlatformMsgQSerialNo = 0;

SMF_PlatformMsgQImpl::SMF_PlatformMsgQImpl(SMF_PlatformMsgQ *pApi, const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/, bool bRemoteAgent /*= false*/)
    : SMF_NonCopyable()
    , m_pApi(pApi)
    , m_nSerialNo(m_nPlatformMsgQSerialNo++)
    , m_sName(pName != NULL ? pName : "")
    , m_pName(m_sName.length() == 0 ? NULL : m_sName.c_str())
    , m_bSharedInProcesses(bSharedInProcesses)
    , m_bRemoteAgent(bRemoteAgent)
    , m_hWnd(NULL)
    , m_atomWndCls(0)
    , m_pMsgQHandler(NULL)
    , m_oLock(GetLockName().c_str())
    , m_pConnectedThread(NULL)
{
    if (!m_bRemoteAgent) {
        CreateInternalWindow();
    }
}

SMF_PlatformMsgQImpl::~SMF_PlatformMsgQImpl()
{
    if (!m_bRemoteAgent) {
        DestroyInternalWindow();
    }
}

HWND SMF_PlatformMsgQImpl::GetInternalWnd()
{
    if (m_bRemoteAgent) {
        Lock();
        m_hWnd = NULL;
        std::wstring wsWindowClassName = TK_Tools::str2wstr(GetWindowClassName());
        std::wstring wsWindowName = TK_Tools::str2wstr(GetWindowName());
        m_hWnd = ::FindWindow(wsWindowClassName.c_str(), wsWindowName.c_str());
        Unlock();
    }

    return m_hWnd;
}

HWND SMF_PlatformMsgQImpl::CloseInternalWnd()
{
    if (m_bRemoteAgent) {
        Lock();
        m_hWnd = NULL;
        Unlock();
    }

    return m_hWnd;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::CreateInternalWindow()
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

SMF_ErrorCode SMF_PlatformMsgQImpl::DestroyInternalWindow()
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

LRESULT CALLBACK SMF_PlatformMsgQImpl::WinWndProcStatic(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    SMF_PlatformMsgQImpl *pThis = reinterpret_cast<SMF_PlatformMsgQImpl *>(::GetWindowLong(hWnd, GWL_USERDATA));
    
    if (pThis != NULL) {
        lResult = pThis->WinWndProc(hWnd, nMsg, wParam, lParam);
    } else {
        lResult = ::DefWindowProc(hWnd, nMsg, wParam, lParam);
    }
    
    return lResult;
}

LRESULT SMF_PlatformMsgQImpl::WinWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;

    if (hWnd != NULL && hWnd == m_hWnd) {
        if (nMsg >= WM_USER) {
            SMF_PlatformMsgQ::Msg oMsg(nMsg - WM_USER, wParam, lParam);
            SMF_PlatformMsgQHandler *pMsgQHandler = GetMsgQHandler();
            if (pMsgQHandler != NULL) {
                lResult = pMsgQHandler->PlatformMsgQProc(*m_pApi, oMsg);
            }
        } else {
            lResult = ::DefWindowProc(hWnd, nMsg, wParam, lParam);
        }
    } else {
        lResult = ::DefWindowProc(hWnd, nMsg, wParam, lParam);
    }
    
    return lResult;
}

//
// NOTE:
// 1) This function should be thread-safe in single process when m_bRemoteAgent != true.
// 2) This function should be thread-safe among processes when m_bRemoteAgent == true.
//
SMF_ErrorCode SMF_PlatformMsgQImpl::PostMsg(const SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    BOOL bSuc;
    HWND hWnd;

    if (nErrorCode == SMF_ERR_OK) {
        hWnd = GetInternalWnd();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (hWnd == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        bSuc = ::PostMessage(hWnd, rMsg.nMsgId + WM_USER, rMsg.wParam, rMsg.lParam);
        if (!bSuc) {
            SMF_nPlatformErrorCode = ::GetLastError();
            nErrorCode = SMF_ERR_PLATFORM_API_CALL_FAILED;
        }
    }

    CloseInternalWnd();

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PostQuitMsg()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    SMF_PlatformThread::ThreadId nThreadId;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_hWnd == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pConnectedThread == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nThreadId = m_pConnectedThread->GetThreadId();
        ::PostThreadMessage(nThreadId, WM_QUIT, 0, 0);
    }

    return nErrorCode;
}

//
// NOTE:
// 1) This function should be thread-safe in single process when m_bRemoteAgent != true.
// 2) This function should be thread-safe among processes when m_bRemoteAgent == true.
//
SMF_ErrorCode SMF_PlatformMsgQImpl::SendMsg(const SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    HWND hWnd;

    if (nErrorCode == SMF_ERR_OK) {
        hWnd = GetInternalWnd();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (hWnd == NULL) {
            nErrorCode = SMF_ERR_INVALID_HANDLE;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        ::SendMessage(hWnd, rMsg.nMsgId + WM_USER, rMsg.wParam, rMsg.lParam);
    }

    CloseInternalWnd();

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::EnterMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    MSG msg;
    SMF_PlatformMsgQHandler *pMsgQHandler;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            pMsgQHandler = GetMsgQHandler();
            if (pMsgQHandler != NULL) {
                nErrorCode = pMsgQHandler->PlatformMsgQPreGetMsg(*m_pApi);
            }
            if (::GetMessage(&msg, NULL, 0, 0)) {
                ::DispatchMessage(&msg);
                if (pMsgQHandler != NULL) {
                    nErrorCode = pMsgQHandler->PlatformMsgQPostGetMsg(*m_pApi);
                }
            } else {
                if (pMsgQHandler != NULL) {
                    nErrorCode = pMsgQHandler->PlatformMsgQPostGetMsg(*m_pApi);
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::ExitMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = PostQuitMsg();
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        m_pMsgQHandler = pMsgQHandler;
    }

    return nErrorCode;
}

SMF_PlatformMsgQHandler *SMF_PlatformMsgQImpl::GetMsgQHandler()
{
    SMF_PlatformMsgQHandler *pMsgQHandler = m_pApi;

    if (pMsgQHandler != NULL) {
        pMsgQHandler = m_pMsgQHandler;
    }

    return pMsgQHandler;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, SMF_PlatformMsgQ::Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQImpl::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NotSupportedInRemoteAgent();
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedMsgQ(NULL);
            Lock();
            m_pConnectedThread = NULL;
            Unlock();
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pConnectedThread = pThread;
        if (m_pConnectedThread != NULL) {
            nErrorCode = m_pConnectedThread->SetConnectedMsgQ(this->m_pApi);
        }
        Unlock();
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformMsgQImpl::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    Lock();
    pPlatformThread = m_pConnectedThread;
    Unlock();

    return pPlatformThread;
}

void SMF_PlatformMsgQImpl::Lock()
{
    m_oLock.Lock();
}

void SMF_PlatformMsgQImpl::Unlock()
{
    m_oLock.Unlock();
}

SMF_ErrorCode SMF_PlatformMsgQImpl::NotSupportedInRemoteAgent()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_bRemoteAgent) {
            nErrorCode = SMF_ERR_NOT_SUPPORTED;
        }
    }

    return nErrorCode;
}

std::string SMF_PlatformMsgQImpl::GetWindowClassName() const 
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformMsgQImpl_WindowClassName_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());
    
    return sResult;
}

std::string SMF_PlatformMsgQImpl::GetWindowName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("%s_SMF_PlatformMsgQImpl_WindowName_%s", SMF_GetDomainStr(m_bSharedInProcesses).c_str(), m_sName.c_str());
    
    return sResult;
}

std::string SMF_PlatformMsgQImpl::GetLockName() const
{
    std::string sResult;

    sResult = TK_Tools::FormatStr("SMF_PlatformMsgQImpl_Lock_%s", m_sName.c_str());

    return sResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_PlatformMsgQ
//
SMF_PlatformMsgQ::SMF_PlatformMsgQ(const char *pName /*= NULL*/, bool bSharedInProcesses /*= false*/, bool bRemoteAgent /*= false*/)
    : SMF_NonCopyable()
    , m_pImpl(NULL)
{
    m_pImpl = new SMF_PlatformMsgQImpl(this, pName, bSharedInProcesses, bRemoteAgent);
}

SMF_PlatformMsgQ::~SMF_PlatformMsgQ()
{
    if (m_pImpl != NULL) {
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

SMF_ErrorCode SMF_PlatformMsgQ::PostMsg(const Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PostMsg(rMsg);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::SendMsg(const Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SendMsg(rMsg);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::EnterMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->EnterMsgLoop();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::ExitMsgLoop()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ExitMsgLoop();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformMsgQPreGetMsg(rPlatformMsgQ);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformMsgQPostGetMsg(rPlatformMsgQ);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, Msg &rMsg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PlatformMsgQProc(rPlatformMsgQ, rMsg);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->SetMsgQHandler(pMsgQHandler);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::PostQuitMsg()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->PostQuitMsg();
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_PlatformMsgQ::ConnectToThread(SMF_PlatformThread *pThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (m_pImpl != NULL) {
        nErrorCode = m_pImpl->ConnectToThread(pThread);
    } else {
        nErrorCode = SMF_ERR_NULL_PTR;
    }

    return nErrorCode;
}

SMF_PlatformThread *SMF_PlatformMsgQ::GetConnectedThread()
{
    SMF_PlatformThread *pPlatformThread = NULL;

    if (m_pImpl != NULL) {
        pPlatformThread = m_pImpl->GetConnectedThread();
    }

    return pPlatformThread;
}
