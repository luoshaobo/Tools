#include <Windows.h>
#include "TK_Tools.h"

#define LOG_PREFIX                      "[STD_APP] "

#define LOG_INFO(...)                   fprintf(stdout, __VA_ARGS__)
#define LOG_ERR(...)                    fprintf(stdout, __VA_ARGS__)

#define TSTR2STR(tstr)                  TK_Tools::tstr2str(tstr)

#define IN_BUF_SIZE         100
#define OUT_BUF_SIZE        100

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
    int nRet = 0;
    BOOL bSuc;
    HANDLE hStd = INVALID_HANDLE_VALUE;
    char arrBufIn[IN_BUF_SIZE];
    char arrBufOut[OUT_BUF_SIZE] = "1234567890";
    DWORD nByteOut;
    DWORD nByteOutRet;
    DWORD nByteIn;
    DWORD nByteInRet;
    
    if (nRet == 0) {
        hStd = CreateFile(TEXT("STD1:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hStd == INVALID_HANDLE_VALUE) {
            LOG_INFO(LOG_PREFIX "%s(): CreateFile() is failed!\n", __FUNCTION__);
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        nByteOutRet = 0;
        bSuc = WriteFile(hStd, arrBufOut, strlen(arrBufOut) + 1, &nByteOutRet, 0);
        if (!bSuc) {
            LOG_INFO(LOG_PREFIX "%s(): WriteFile() is failed!\n", __FUNCTION__);
            nRet = -1;
        } else {
            LOG_INFO(LOG_PREFIX "%s(): WriteFile(): %lu bytes are written!\n", __FUNCTION__, nByteOutRet);
        }
    }
    
    if (nRet == 0) {
        bSuc = ReadFile(hStd, arrBufIn, IN_BUF_SIZE, &nByteInRet, 0);
        if (!bSuc) {
            LOG_INFO(LOG_PREFIX "%s(): ReadFile() is failed!\n", __FUNCTION__);
            nRet = -1;
        } else {
            LOG_INFO(LOG_PREFIX "%s(): ReadFile(): %lu bytes are read!\n", __FUNCTION__, nByteInRet);
            if (nByteInRet < IN_BUF_SIZE) {
                arrBufIn[nByteInRet] = '\0';
            } else {
                arrBufIn[IN_BUF_SIZE - 1] = '\0';
            }
            LOG_INFO(LOG_PREFIX "%s(): ReadFile(): content=%s\n", __FUNCTION__, arrBufIn);
        }
    }

    if (nRet == 0) {
        DWORD nInBuf = 5;
        DWORD nOutBuf = 0;
        DWORD nBytesRet;

        bSuc = DeviceIoControl(hStd, 1000, &nInBuf, 4, &nOutBuf, 4, &nBytesRet, NULL);
        if (bSuc) {
            LOG_INFO(LOG_PREFIX "%s(): IOControl(): dwIoControlCode=%lu, nInBuf=%lu, nOutBuf=%lu, nBytesRet=%lu\n", 
                __FUNCTION__, 1000, nInBuf, nOutBuf, nBytesRet);
        } else {
            LOG_INFO(LOG_PREFIX "%s(): IOControl() is failed!\n", __FUNCTION__);
        }
    }
    
    if (hStd != INVALID_HANDLE_VALUE) {
        CloseHandle(hStd);
    }
    
    return nRet;
}
