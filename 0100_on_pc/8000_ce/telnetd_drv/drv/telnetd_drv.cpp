#include "StdAfx.h"
#include <windows.h>
#include <ceddk.h>
#include "TK_Tools.h"
#include "TK_DebugServer.h"
#include "TK_RegKey.h"
#include "Launcher.h"
#include "Service.h"

#define LOG_PREFIX                      "[TLD_DRV] "

#define TLD_DEV_HANDLE_BASE             0x1234
#define TLD_FILE_HANDLE_BASE            0x5678

#define LOG_INFO(...)                   fprintf(stdout, __VA_ARGS__)
#define LOG_ERR(...)                    fprintf(stderr, __VA_ARGS__)

#define TSTR2STR(tstr)                  TK_Tools::tstr2str(tstr)

//////////////////////////////////////////////////////////////////////////////////////////
// TldDevice
//

class TldDevice
{
private:
    TldDevice();
    
public:
    ~TldDevice();
    static TldDevice &GetInstance();

public:
    bool OnInit(const std::wstring &sActiveRegKeyPath);


public:
    int GetDevIndex() { return m_nDevIndex; }

private:
    int m_nDevIndex;
};

TldDevice::TldDevice()
    : m_nDevIndex(-1)
{

}

TldDevice::~TldDevice()
{

}

TldDevice &TldDevice::GetInstance()
{
    static TldDevice oTldDevice;

    return oTldDevice;
}

bool TldDevice::OnInit(const std::wstring &sActiveRegKeyPath)
{
    bool bRet = true;
    TK_Tools::RegKey::ErrCode nErrCode;
    TK_Tools::RegKey oRegKeyHKLM(HKEY_LOCAL_MACHINE);
    TK_Tools::RegKey::KeyValue oValue;
    unsigned int i;

    if (bRet) {
        nErrCode = TK_Tools::RegKey::GetValue(sActiveRegKeyPath, L"Name", oValue, &oRegKeyHKLM);
        if (nErrCode != TK_Tools::RegKey::EC_OK) {
            bRet = false;
        }
    }

    if (bRet) {
        std::wstring &sSz = oValue.value.vSz;
        for (i = 0; i < 10; i++) {
            std::wstring sDevFile = TK_Tools::FormatWStr(L"TLD%u:", i);
            if (sSz == sDevFile) {
                m_nDevIndex = i;
                break;
            }
        }
        if (i == 10) {
            bRet = false;
        }
    }

    if (bRet) {
        TK_Tools::DebugServer::GetInstance((TK_Tools::DebugServerInstanceId)(TK_Tools::DSIID_0 + m_nDevIndex));
    }

    return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TldDevice
//

BOOL APIENTRY DllMain(HANDLE hModule, DWORD nReasonForCall, LPVOID lpReserved)
{
    switch (nReasonForCall) {
        case DLL_PROCESS_ATTACH:
            {
                LOG_INFO(LOG_PREFIX "%s(): DLL_PROCESS_ATTACH\n", __FUNCTION__);
            }
            break;
        case DLL_PROCESS_DETACH:
            {
                LOG_INFO(LOG_PREFIX "%s(): DLL_PROCESS_DETACH\n", __FUNCTION__);
            }
            break;
        case DLL_THREAD_ATTACH:
            {
                LOG_INFO(LOG_PREFIX "%s: DLL_THREAD_ATTACH\n", __FUNCTION__);
            }
            break;
        case DLL_THREAD_DETACH:
            {
                LOG_INFO(LOG_PREFIX "%s: DLL_THREAD_DETACH\n", __FUNCTION__);
            }
            break;
        default:
            break;
    }
    
    return TRUE;
}

DWORD TLD_Init(LPCTSTR pContext, LPCVOID lpvBusContext)
{
    DWORD hDeviceContext = 0;
    bool bRet;
    unsigned int nDevIndex;
    TldDevice &rTldDevice = TldDevice::GetInstance();

    bRet = rTldDevice.OnInit(pContext);
    if (bRet) {
        nDevIndex = rTldDevice.GetDevIndex();
        ServiceFactory::GetInstance().SetLastServiceId(nDevIndex);
        hDeviceContext = TLD_DEV_HANDLE_BASE + nDevIndex;
    }

    LOG_INFO(LOG_PREFIX "%s(): pContext=%s\n", __FUNCTION__, TSTR2STR(pContext).c_str());
    LOG_INFO(LOG_PREFIX "%s(): hRetDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
    
    return hDeviceContext;
}

DWORD TLD_Deinit(DWORD hDeviceContext)
{
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);

    ServiceFactory::GetInstance().DestroyAllServices();
    
    return 0;
}

DWORD TLD_Open(DWORD hDeviceContext, DWORD nAccessCode, DWORD nShareMode)
{
    DWORD hOpenContext = TLD_FILE_HANDLE_BASE + ServiceFactory::GetInstance().GetLastServiceId();
    
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, nAccessCode=0x%08X, nShareMode=0x%08X, hRetOpenContext=0x%08X\n", 
        __FUNCTION__, hDeviceContext, nAccessCode, nShareMode, hOpenContext);
    
    return hOpenContext;
}

BOOL TLD_Close(DWORD hOpenContext)
{
    LOG_INFO(LOG_PREFIX "%s(): hOpenContext=0x%08X\n", __FUNCTION__, hOpenContext);
    
    return TRUE;
}

BOOL TLD_IOControl(DWORD hOpenContext, DWORD nCode, PBYTE pBufIn, DWORD nLenIn, PBYTE pBufOut, DWORD nLenOut, PDWORD pnActuralLenOut)
{
    LOG_INFO(LOG_PREFIX "%s(): hOpenContext=0x%08X, nCode=0x%08X, pBufIn=0x%08X, nLenIn=%lu, pBufOut=0x%08X, nLenOut=%lu, pnActuralLenOut=0x%08X\n", 
        __FUNCTION__, hOpenContext, nCode, pBufIn, nLenIn, pBufOut, nLenOut, pnActuralLenOut);
    
    return TRUE;
}

void TLD_PowerUp(DWORD hDeviceContext)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
}

void TLD_PowerDown(DWORD hDeviceContext)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
}

DWORD TLD_Read(DWORD hDeviceContext, PUCHAR pBuf, ULONG nCount)
{  
    DWORD nRet = 0;
    
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, pBuf=0x%08X\n, nCount=%lu", __FUNCTION__, hDeviceContext, pBuf, nCount);

    unsigned int nServiceId = hDeviceContext - TLD_FILE_HANDLE_BASE;
    Service *pService = ServiceFactory::GetInstance().GetService(nServiceId);

    if (pService != NULL) {
        nRet = pService->Read(pBuf, nCount);
    }

    return nRet;
}

DWORD TLD_Write(DWORD hDeviceContext, PUCHAR pBuf, ULONG nCount)
{  
    DWORD nRet = 0;
    
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, pBuf=0x%08X\n, nCount=%lu", __FUNCTION__, hDeviceContext, pBuf, nCount);

    unsigned int nServiceId = hDeviceContext - TLD_FILE_HANDLE_BASE;
    Service *pService = ServiceFactory::GetInstance().GetService(nServiceId);

    if (pService != NULL) {
        nRet = pService->Write(pBuf, nCount);
    }

    return nRet;
}

DWORD TLD_Seek(DWORD hDeviceContext, long nAmount, WORD nType)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, nAmount=%ld\n, nType=%u", __FUNCTION__, hDeviceContext, nAmount, nType);
    
    return 0;
}
