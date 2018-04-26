#include "TsdTester.h"
#include <pm.h>
#include <ssw_IEmbRegistry.h>
//#include <app_IEmbRegistryIDs.h>
#include "entIHMITouchScreenDriver.h"

TsdTester::TsdTester()
{

}

TsdTester::~TsdTester()
{

}

TsdTester &TsdTester::GetInstance()
{
    static TsdTester oTsdTester;
    
    return oTsdTester;
}

int TsdTester::ReadErg(int argc, TCHAR* argv[])
{
    int nRet = 0;
    uint32_t nRegId = (UINT32_T)-1;
    uint32_t nDataSize = 0;
    uint32_t nDataSizeRet = 0;
    uint8_t *pDataBuf = NULL;
    
    if (argc >= 3) {
        nRegId = (uint32_t)StrToUL(wstr2str(argv[2]));
        
        if (ssw_ergUserGetDataSize(ERG_USER_SYSTEM, nRegId, &nDataSize) == 0) {
            pDataBuf = new uint8_t[nDataSize];
            if (pDataBuf != NULL) {
                if (ssw_ergUserReadData(ERG_USER_SYSTEM, nRegId, pDataBuf, nDataSize, &nDataSizeRet) == 0) {
                    LOG("%s(): [0x%08X] Read ERG value (hex):", __FUNCTION__, nRegId);
                    for (unsigned int i = 0; i < nDataSizeRet; i++) {
                        LOG(" %02X", pDataBuf[i]);
                    }
                    LOG("\n");
                } else {
                    LOG("*%s(): ssw_ergUserReadData() is failed!\n", __FUNCTION__);
                }
            } else {
                LOG("*%s(): ssw_ergUserGetDataSize() is failed!\n", __FUNCTION__);
            }
        } else {
            LOG("*%s(): ssw_ergUserGetDataSize() is failed!\n", __FUNCTION__);
        }
    }
    
    if (pDataBuf != NULL) {
        delete [] pDataBuf;
    }
    
    return nRet;
}

int TsdTester::WriteErg(int argc, TCHAR* argv[])
{
    int nRet = 0;
    uint32_t nRegId = (uint32_t)-1;
    uint32_t nDataSize = 0;
    uint8_t *pDataBuf = NULL;

    if (argc >= 4) {
        nRegId = (uint32_t)StrToUL(wstr2str(argv[2]));

        nDataSize = argc - 3;
        pDataBuf = new uint8_t[nDataSize];
        if (pDataBuf != NULL) {
            for (unsigned int i = 0; i < nDataSize; i++) {
                pDataBuf[i] = (uint8_t)StrToUL(wstr2str(argv[i + 3]));
            }

            if (ssw_ergUserWriteData(ERG_USER_SYSTEM, nRegId, pDataBuf, nDataSize) == 0) {
                LOG("%s(): [0x%08X] Write ERG value (hex):", __FUNCTION__, nRegId);
                for (unsigned int i = 0; i < nDataSize; i++) {
                    LOG(" %02X", pDataBuf[i]);
                }
                LOG("\n");
            } else {
                LOG("*%s(): ssw_ergUserWriteData() is failed!\n", __FUNCTION__);
            }
        } else {
            LOG("*%s(): new is failed!\n", __FUNCTION__);
        }
    }
    
    if (pDataBuf != NULL) {
        delete [] pDataBuf;
    }
    
    return nRet;
}

int TsdTester::SetDimming(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT8_T nDimming = 0;
    
    if (argc >= 3) {
        nDimming = (UINT8_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nDimming=%u\n", __FUNCTION__, nDimming);
        bSuc = entIHmiTouchScreenDriver_SetDimming(nDimming);
        if (!bSuc) {
            LOG("*%s(): entIHmiTouchScreenDriver_SetDimming() is failed!\n", __FUNCTION__);
        }
    }
    
    return 0;
}

int TsdTester::SetDarkMode(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT8_T nDarkMode = 0;
    
    if (argc >= 3) {
        nDarkMode = (UINT8_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nDarkMode=%u\n", __FUNCTION__, nDarkMode);
        bSuc = entIHmiTouchScreenDriver_SetDarkMode(nDarkMode);
        if (!bSuc) {
            LOG("*%s(): entIHmiTouchScreenDriver_SetDarkMode() is failed!\n", __FUNCTION__);
        }
    }
    
    return 0;
}

int TsdTester::SetScreenOrientation(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT8_T nScreenOrientation = 0;
    
    if (argc >= 3) {
        nScreenOrientation = (UINT8_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nScreenOrientation=%u\n", __FUNCTION__, nScreenOrientation);
        bSuc = entIHmiTouchScreenDriver_SetScreenOrientation(nScreenOrientation);
        if (!bSuc) {
            LOG("*%s(): entIHmiTouchScreenDriver_SetScreenOrientation() is failed!\n", __FUNCTION__);
        }
    }
    
    return 0;
}

int TsdTester::GetLastTouchPosition(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT8_T aToutchPos[COORDINATE_DATA_SIZE] = { 0 };
    
    LOG("%s()\n", __FUNCTION__);
    bSuc = entIHmiTouchScreenDriver_GetLastTouchPosition(aToutchPos);
    if (bSuc) {
        LOG("%s(): %02X %02X %02X %02X %02X %02X %02X\n", __FUNCTION__, 
            aToutchPos[0], 
            aToutchPos[1], 
            aToutchPos[2], 
            aToutchPos[3], 
            aToutchPos[4], 
            aToutchPos[5], 
            aToutchPos[6]
        );
    } else {
        LOG("*%s(): entIHmiTouchScreenDriver_GetLastTouchPosition() is failed!\n", __FUNCTION__);
    }
    
    return 0;
}

int TsdTester::GetDgtId(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT8_T aDgtId[DGT_ID_DATA_SIZE] = { 0 };
    
    LOG("%s()\n", __FUNCTION__);
    bSuc = entIHmiTouchScreenDriver_GetDgtId(aDgtId);
    if (bSuc) {
        LOG("%s(): %02X %02X %02X %02X %02X %02X %02X\n", __FUNCTION__, 
            aDgtId[0], 
            aDgtId[1], 
            aDgtId[2], 
            aDgtId[3], 
            aDgtId[4], 
            aDgtId[5], 
            aDgtId[6]
        );
    } else {
        LOG("*%s(): entIHmiTouchScreenDriver_GetDgtId() is failed!\n", __FUNCTION__);
    }
    
    return 0;
}

int TsdTester::SetDisable(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    
    bSuc = entIHmiTouchScreenDriver_SetDisable();
    if (bSuc) {
        LOG("%s()\n", __FUNCTION__);
    } else {
        LOG("*%s(): entIHmiTouchScreenDriver_SetDisable() is failed!\n", __FUNCTION__);
    }
    
    return 0;
}

int TsdTester::SetEnable(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    
    bSuc = entIHmiTouchScreenDriver_SetEnable();
    if (bSuc) {
        LOG("%s()\n", __FUNCTION__);
    } else {
        LOG("*%s(): entIHmiTouchScreenDriver_SetEnable() is failed!\n", __FUNCTION__);
    }
    
    return 0;
}

int TsdTester::SetDisableThenEnable(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT_T nIntervalMillicSeconds = 0;
    
    if (argc >= 3) {
        nIntervalMillicSeconds = (UINT_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nIntervalMillicSeconds=%u\n", __FUNCTION__, nIntervalMillicSeconds);
        
        bSuc = entIHmiTouchScreenDriver_SetDisable();
        if (bSuc) {
            LOG("%s(): entIHmiTouchScreenDriver_SetDisable\n", __FUNCTION__);
        } else {
            LOG("*%s(): entIHmiTouchScreenDriver_SetDisable() is failed!\n", __FUNCTION__);
        }
        
        Sleep(nIntervalMillicSeconds);
        
        bSuc = entIHmiTouchScreenDriver_SetEnable();
        if (bSuc) {
            LOG("%s(): entIHmiTouchScreenDriver_SetEnable\n", __FUNCTION__);
        } else {
            LOG("*%s(): entIHmiTouchScreenDriver_SetEnable() is failed!\n", __FUNCTION__);
        }
    }
    
    return 0;
}

int TsdTester::SetHmiReady(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    
    bSuc = entIHmiTouchScreenDriver_SetHmiReady();
    if (bSuc) {
        LOG("%s()\n", __FUNCTION__);
    } else {
        LOG("*%s(): entIHmiTouchScreenDriver_SetHmiReady() is failed!\n", __FUNCTION__);
    }
    
    return 0;
}

int TsdTester::SetHmiNotReady(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    
    bSuc = entIHmiTouchScreenDriver_SetHmiNotReady();
    if (bSuc) {
        LOG("%s()\n", __FUNCTION__);
    } else {
        LOG("*%s(): entIHmiTouchScreenDriver_SetHmiNotReady() is failed!\n", __FUNCTION__);
    }
    
    return 0;
}

int TsdTester::SetDebounceMode(int argc, TCHAR* argv[])
{
    int nRet = 0;
    bool_t bSuc;
    UINT8_T nDebounceMode = 0;
    
    if (argc >= 3) {
        nDebounceMode = (UINT8_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nDebounceMode=%u\n", __FUNCTION__, nDebounceMode);
        bSuc = entIHmiTouchScreenDriver_SetDebounceMode(nDebounceMode);
        if (!bSuc) {
            LOG("*%s(): entIHmiTouchScreenDriver_SetDebounceMode() is failed!\n", __FUNCTION__);
        }
    }
    
    return 0;
}

int TsdTester::SetPowerMode(int argc, TCHAR* argv[])
{
    int nRet = 0;
    UINT8_T nPowerMode = 0;
    HANDLE hIHMITsd = INVALID_HANDLE_VALUE;
    CEDEVICE_POWER_STATE nDevPowerState = D0;
    DWORD nOutputSize = sizeof(nDevPowerState);
    DWORD nBytesReturned = 0;
    
    hIHMITsd = CreateFile(TEXT("TSD1:"),
                        (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_WRITE),
                        0, 0, 0, 0);
    
    if (argc >= 3) {
        nPowerMode = (UINT8_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nPowerMode=%u\n", __FUNCTION__, nPowerMode);
        nDevPowerState = (CEDEVICE_POWER_STATE)nPowerMode;
        if (hIHMITsd != INVALID_HANDLE_VALUE) {
            if(!DeviceIoControl(	hIHMITsd, IOCTL_POWER_SET, 	        //Handle & IOCtl Code
                                    NULL, 0,	                        //Input Buffer
                                    &nDevPowerState,	nOutputSize,	//Output Buffer
                                    &nBytesReturned, 0))				//Bytes Returned =0,LpOverlapped =NULL
            {
                LOG("*%s(): DeviceIoControl(IOCTL_POWER_SET) is failed!\n", __FUNCTION__);
            } else {
                LOG("%s(): nPowerMode=%u\n", __FUNCTION__, nPowerMode);
            }
        }
    }
    
    if (hIHMITsd != INVALID_HANDLE_VALUE) {
        CloseHandle(hIHMITsd);
        hIHMITsd = INVALID_HANDLE_VALUE;
    }
    
    return 0;
}

int TsdTester::SetPowerOffThenOn(int argc, TCHAR* argv[])
{
    int nRet = 0;
    UINT_T nIntervalMillicSeconds = 0;
    UINT8_T nPowerMode = 0;
    HANDLE hIHMITsd = INVALID_HANDLE_VALUE;
    CEDEVICE_POWER_STATE nDevPowerState;
    DWORD nOutputSize;
    DWORD nBytesReturned;
    
    hIHMITsd = CreateFile(TEXT("TSD1:"),
                        (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_WRITE),
                        0, 0, 0, 0);

    
    if (argc >= 3) {
        nIntervalMillicSeconds = (UINT_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nIntervalMillicSeconds=%u\n", __FUNCTION__, nIntervalMillicSeconds);
        
        nPowerMode = 1;
        nDevPowerState = (CEDEVICE_POWER_STATE)nPowerMode;
        nOutputSize = sizeof(nDevPowerState);
        nBytesReturned = 0;
        if (hIHMITsd != INVALID_HANDLE_VALUE) {
            if(!DeviceIoControl(	hIHMITsd, IOCTL_POWER_SET, 	        //Handle & IOCtl Code
                                    NULL, 0,	                        //Input Buffer
                                    &nDevPowerState,	nOutputSize,	//Output Buffer
                                    &nBytesReturned, 0))				//Bytes Returned =0,LpOverlapped =NULL
            {
                LOG("*%s(): DeviceIoControl(IOCTL_POWER_SET) is failed!\n", __FUNCTION__);
            } else {
                LOG("%s(): nPowerMode=%u\n", __FUNCTION__, nPowerMode);
            }
        }
        
        Sleep(nIntervalMillicSeconds);
        
        nPowerMode = 0;
        nDevPowerState = (CEDEVICE_POWER_STATE)nPowerMode;
        nOutputSize = sizeof(nDevPowerState);
        nBytesReturned = 0;
        if (hIHMITsd != INVALID_HANDLE_VALUE) {
            if(!DeviceIoControl(	hIHMITsd, IOCTL_POWER_SET, 	        //Handle & IOCtl Code
                                    NULL, 0,	                        //Input Buffer
                                    &nDevPowerState,	nOutputSize,	//Output Buffer
                                    &nBytesReturned, 0))				//Bytes Returned =0,LpOverlapped =NULL
            {
                LOG("*%s(): DeviceIoControl(IOCTL_POWER_SET) is failed!\n", __FUNCTION__);
            } else {
                LOG("%s(): nPowerMode=%u\n", __FUNCTION__, nPowerMode);
            }
        }
    }
    
    return 0;
}

#if 0
// NOTE: add these lines to the tsdMain.cpp
#define TSD_IOCTL_SET_POWER_D0     CTL_CODE(FILE_DEVICE_UNKNOWN, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D1     CTL_CODE(FILE_DEVICE_UNKNOWN, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D2     CTL_CODE(FILE_DEVICE_UNKNOWN, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D3     CTL_CODE(FILE_DEVICE_UNKNOWN, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D4     CTL_CODE(FILE_DEVICE_UNKNOWN, 24, METHOD_BUFFERED, FILE_ANY_ACCESS)

       case TSD_IOCTL_SET_POWER_D0:
       {
            DGTPowerOn();
            dwErr = ERROR_SUCCESS;
       }
       break;
       case TSD_IOCTL_SET_POWER_D1:
       {
            DGTPowerOff();
            dwErr = ERROR_SUCCESS;
       }
       break;
#endif // #if 0

#define TSD_IOCTL_SET_POWER_D0     CTL_CODE(FILE_DEVICE_UNKNOWN, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D1     CTL_CODE(FILE_DEVICE_UNKNOWN, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D2     CTL_CODE(FILE_DEVICE_UNKNOWN, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D3     CTL_CODE(FILE_DEVICE_UNKNOWN, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define TSD_IOCTL_SET_POWER_D4     CTL_CODE(FILE_DEVICE_UNKNOWN, 24, METHOD_BUFFERED, FILE_ANY_ACCESS)

int TsdTester::SetPowerOffThenOn2(int argc, TCHAR* argv[])
{
    int nRet = 0;
    UINT_T nIntervalMillicSeconds = 0;
    HANDLE hIHMITsd = INVALID_HANDLE_VALUE;
    
    hIHMITsd = CreateFile(TEXT("TSD1:"),
                        (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_WRITE),
                        0, 0, 0, 0);

    
    if (argc >= 3) {
        nIntervalMillicSeconds = (UINT_T)StrToUL(wstr2str(argv[2]));
        LOG("%s(): nIntervalMillicSeconds=%u\n", __FUNCTION__, nIntervalMillicSeconds);
        
        if (hIHMITsd != INVALID_HANDLE_VALUE) {
            if(!DeviceIoControl(	hIHMITsd, TSD_IOCTL_SET_POWER_D1, 	        //Handle & IOCtl Code
                                    NULL, 0,	                                //Input Buffer
                                    NULL, 0,	                                //Output Buffer
                                    NULL, 0))				                    //Bytes Returned =0,LpOverlapped =NULL
            {
                LOG("*%s(): DeviceIoControl(TSD_IOCTL_SET_POWER_D1) is failed!\n", __FUNCTION__);
            } else {
                LOG("%s(): DeviceIoControl(TSD_IOCTL_SET_POWER_D1)\n", __FUNCTION__);
            }
        }
        
        Sleep(nIntervalMillicSeconds);
        
        if (hIHMITsd != INVALID_HANDLE_VALUE) {
            if(!DeviceIoControl(	hIHMITsd, TSD_IOCTL_SET_POWER_D0, 	        //Handle & IOCtl Code
                                    NULL, 0,	                                //Input Buffer
                                    NULL, 0,	                                //Output Buffer
                                    NULL, 0))				                    //Bytes Returned =0,LpOverlapped =NULL
            {
                LOG("*%s(): DeviceIoControl(TSD_IOCTL_SET_POWER_D0) is failed!\n", __FUNCTION__);
            } else {
                LOG("%s(): DeviceIoControl(TSD_IOCTL_SET_POWER_D0)\n", __FUNCTION__);
            }
        }
    }
    
    return 0;
}

