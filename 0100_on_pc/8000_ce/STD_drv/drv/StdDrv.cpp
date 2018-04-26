#include <windows.h>
#include <ceddk.h>
#include "TK_Tools.h"

#define LOG_PREFIX                      "[STD_DRV] "

#define LOG_INFO(...)                   fprintf(stdout, __VA_ARGS__)
#define LOG_ERR(...)                    fprintf(stdout, __VA_ARGS__)

#define TSTR2STR(tstr)                  TK_Tools::tstr2str(tstr)

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

DWORD STD_Init(LPCTSTR pContext, LPCVOID lpvBusContext)
{
    DWORD hDeviceContext = 0x1234;
    
    LOG_INFO(LOG_PREFIX "%s(): pContext=%s\n", __FUNCTION__, TSTR2STR(pContext).c_str());
    LOG_INFO(LOG_PREFIX "%s(): hRetDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
    
    return hDeviceContext;
}

DWORD STD_Deinit(DWORD hDeviceContext)
{
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
    
    return 0;
}

DWORD STD_Open(DWORD hDeviceContext, DWORD nAccessCode, DWORD nShareMode)
{
    DWORD hOpenContext = 0x5678;
    
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, nAccessCode=0x%08X, nShareMode=0x%08X, hRetOpenContext=0x%08X\n", 
        __FUNCTION__, hDeviceContext, nAccessCode, nShareMode, hOpenContext);
    
    return hOpenContext;
}

BOOL STD_Close(DWORD hOpenContext)
{
    LOG_INFO(LOG_PREFIX "%s(): hOpenContext=0x%08X\n", __FUNCTION__, hOpenContext);
    
    return TRUE;
}

BOOL STD_IOControl(DWORD hOpenContext, DWORD nCode, PBYTE pBufIn, DWORD nLenIn, PBYTE pBufOut, DWORD nLenOut, PDWORD pnActuralLenOut)
{
    LOG_INFO(LOG_PREFIX "%s(): hOpenContext=0x%08X, nCode=0x%08X, pBufIn=0x%08X, nLenIn=%lu, pBufOut=0x%08X, nLenOut=%lu, pnActuralLenOut=0x%08X\n", 
        __FUNCTION__, hOpenContext, nCode, pBufIn, nLenIn, pBufOut, nLenOut, pnActuralLenOut);

    if (pBufIn != NULL && pBufOut != NULL && nLenIn >= 4 && nLenOut >= 4) {
        DWORD dwInData;
        DWORD dwOutData;

        memcpy(&dwInData, pBufIn, 4);
        dwOutData = dwInData + nCode;
        memcpy(pBufOut, &dwOutData, 4);
    }

    if (pnActuralLenOut != NULL) {
        *pnActuralLenOut = 4;
    }
    
    return TRUE;
}

void STD_PowerUp(DWORD hDeviceContext)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
}

void STD_PowerDown(DWORD hDeviceContext)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X\n", __FUNCTION__, hDeviceContext);
}

#define BUF_SIZE                1024

static char arrBuf[BUF_SIZE];
static unsigned int nBufContentLen = 0;

DWORD STD_Read(DWORD hDeviceContext, PUCHAR pBuf, ULONG nCount)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, pBuf=0x%08X\n, nCount=%lu", __FUNCTION__, hDeviceContext, pBuf, nCount);
    
    ULONG nCopyCount = nCount;

    if (nCopyCount > nBufContentLen) {
        nCopyCount = nBufContentLen;
    }

    memcpy(pBuf, arrBuf, nCopyCount);

    return nCopyCount;
}

DWORD STD_Write(DWORD hDeviceContext, PUCHAR pBuf, ULONG nCount)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, pBuf=0x%08X\n, nCount=%lu", __FUNCTION__, hDeviceContext, pBuf, nCount);
    
    ULONG nCopyCount = nCount;

    if (nCopyCount > BUF_SIZE) {
        nCopyCount = BUF_SIZE;
    }
    nBufContentLen = nCopyCount;

    memcpy(arrBuf, pBuf, nCopyCount);

    return nCopyCount;
}

DWORD STD_Seek(DWORD hDeviceContext, long nAmount, WORD nType)
{  
    LOG_INFO(LOG_PREFIX "%s(): hDeviceContext=0x%08X, nAmount=%ld\n, nType=%u", __FUNCTION__, hDeviceContext, nAmount, nType);
    
    return 0;
}
