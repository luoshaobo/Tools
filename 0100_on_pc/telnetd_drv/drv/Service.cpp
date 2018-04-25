#include "Service.h"
#include "TK_DebugServer.h"
#include "Launcher.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// exported functions
//
bool DS_ProcessCmdLine(TK_Tools::DebugServerInstanceId id, const char *pCmdLine)
{
    return ServiceFactory::ProcessCmdLine_static(id, pCmdLine);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ServiceFactory
//
ServiceFactory::ServiceFactory()
    : m_pServices()
    , m_hLockMutex(NULL)
    , m_nLastServiceId(0)
{
    memset(m_pServices, 0, sizeof(m_pServices));
    m_hLockMutex = ::CreateMutex(NULL, FALSE, NULL);
}

ServiceFactory::~ServiceFactory()
{
    if (m_hLockMutex != NULL) {
        ::CloseHandle(m_hLockMutex);
        m_hLockMutex = NULL;
    }
}

ServiceFactory &ServiceFactory::GetInstance()
{
    static ServiceFactory oServiceFactory;
    
    return oServiceFactory;
}

Service *ServiceFactory::GetService(unsigned int nServiceId)
{
    Service *pService = NULL;

    if (nServiceId < SERVICE_ID_COUNT) {
        Lock();
        pService = m_pServices[nServiceId];
        Unlock();
        
        if (pService == NULL) {
            pService = new Service(nServiceId);
            
            Lock();
            m_pServices[nServiceId] = pService;
            Unlock();
        }
        
    }

    return pService;
}

void ServiceFactory::DestroyService(unsigned int nServiceId)
{
    Service *pService;
    
    if (nServiceId < SERVICE_ID_COUNT) {
        Lock();
        pService = m_pServices[nServiceId];
        Unlock();
        
        if (pService != NULL) {
            delete pService;

            Lock();
            m_pServices[nServiceId] = NULL;
            Unlock();
        }
    }
}

void ServiceFactory::DestroyAllServices()
{
    unsigned int i;
    
    for (i = 0; i < SERVICE_ID_COUNT; i++) {
        DestroyService(i);
    }
}

bool ServiceFactory::ProcessCmdLine_static(TK_Tools::DebugServerInstanceId id, const char *pCmdLine)
{
    ServiceFactory &rServiceFactory = GetInstance();

    return rServiceFactory.ProcessCmdLine(id, pCmdLine);
}

bool ServiceFactory::ProcessCmdLine(TK_Tools::DebugServerInstanceId id, const char *pCmdLine)
{
    bool bRet = false;
    unsigned int nServiceId = id;
    Service *pService;
    
    if (nServiceId < SERVICE_ID_COUNT) {
        if (std::string(pCmdLine) == "login") {
            GetService(nServiceId);
        }
        
        Lock();
        pService = m_pServices[nServiceId];
        Unlock();
        
        if (pService != NULL) {
            bRet = pService->ProcessCmdLine(pCmdLine);
        }
    }

    return bRet;
}

void ServiceFactory::Lock()
{
    if (m_hLockMutex != NULL) {
        ::WaitForSingleObject(m_hLockMutex, INFINITE);
    }
}

void ServiceFactory::Unlock()
{
    if (m_hLockMutex != NULL) {
        ::ReleaseMutex(m_hLockMutex);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Service
//
Service::Service(unsigned int nServiceId)
    : m_nServiceId(nServiceId)
    , m_rDebugServer(TK_Tools::DebugServer::GetInstance((TK_Tools::DebugServerInstanceId)(nServiceId + TK_Tools::DSIID_DEF)))
    , m_pLauncher(NULL)
    , m_arrCmdLines()
    , m_bExeExited(false)
    , m_hLockMutex(NULL)
    , m_hReadBufNonemptyEvent(NULL)
{
    m_hLockMutex = ::CreateMutex(NULL, FALSE, NULL);
    m_hReadBufNonemptyEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

Service::~Service()
{
    ProcessCmdLine("exit");
    while (!m_bExeExited) {
        Sleep(10);
    }

    if (m_hReadBufNonemptyEvent != NULL) {
        ::CloseHandle(m_hReadBufNonemptyEvent);
        m_hReadBufNonemptyEvent = NULL;
    }

    if (m_hLockMutex != NULL) {
        ::CloseHandle(m_hLockMutex);
        m_hLockMutex = NULL;
    }
}

bool Service::ProcessCmdLine(const char *pCmdLine)
{
    if (pCmdLine == NULL) {
        return false;
    }

    if (std::string(pCmdLine) == "login") {
        Lock();
        m_arrCmdLines.clear();
        Unlock();

        if (m_pLauncher == NULL) {
            m_pLauncher = new Launcher(m_nServiceId);
            if (m_pLauncher != NULL) {
                ServiceFactory::GetInstance().SetLastServiceId(m_nServiceId);
                m_pLauncher->LaunchExe("", "");
            }
        }
        return true;
    } else if (std::string(pCmdLine) == "exit") {
        Lock();
        m_arrCmdLines.clear();
        Unlock();
        
        delete m_pLauncher;
        m_pLauncher = NULL;
    }

    Lock();
    m_arrCmdLines.push_back(std::string(pCmdLine) + "\r\n");
    Unlock();

    SetReadBufNonempty();

    return true;
}

DWORD Service::Read(PUCHAR pBuf, ULONG nCount)
{
    bool bCmdLineEmpty;
    DWORD nRetVal;
    std::string sCmdLine;
    
    while (true) {
        Lock();
        bCmdLineEmpty = m_arrCmdLines.empty();
        Unlock();
        
        if (bCmdLineEmpty) {
            nRetVal = WaitReadBufNonempty(INFINITE);
            if (nRetVal == WAIT_FAILED) {
                m_bExeExited = true;
                return 0; // end of file
            }
        } else {
            break;
        }
    }

    Lock();
    sCmdLine = m_arrCmdLines.front();
    m_arrCmdLines.pop_front();
    Unlock();
    
    ULONG nCopyCount = nCount;

    if (nCopyCount > sCmdLine.length()) {
        nCopyCount = sCmdLine.length();
    }

    memcpy(pBuf, sCmdLine.c_str(), nCopyCount);

    if (sCmdLine == "exit\r\n") {
        m_bExeExited = true;
    }

    return nCopyCount;

}

DWORD Service::Write(PUCHAR pBuf, ULONG nCount)
{
    ULONG nCopyCount = nCount;

    std::string sOutput((const char *)pBuf, nCount);
    m_rDebugServer.SendTextToPeer1(sOutput.c_str());

    return nCopyCount;
}

void Service::Lock()
{
    if (m_hLockMutex != NULL) {
        ::WaitForSingleObject(m_hLockMutex, INFINITE);
    }
}

void Service::Unlock()
{
    if (m_hLockMutex != NULL) {
        ::ReleaseMutex(m_hLockMutex);
    }
}

DWORD Service::WaitReadBufNonempty(DWORD dwMilliseconds)
{
    DWORD nRet = WAIT_FAILED;
    
    if (m_hReadBufNonemptyEvent != NULL) {
        nRet = ::WaitForSingleObject(m_hReadBufNonemptyEvent, dwMilliseconds);
    }

    return nRet;
}

void Service::SetReadBufNonempty()
{
    if (m_hReadBufNonemptyEvent != NULL) {
        ::SetEvent(m_hReadBufNonemptyEvent);
    }
}

