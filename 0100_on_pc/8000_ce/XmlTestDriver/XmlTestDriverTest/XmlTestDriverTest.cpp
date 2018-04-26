// TestXML001.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define XTD_DEVICE_TYPE             45889

#define XTD_IOCTL_PARSERXML         CTL_CODE(XTD_DEVICE_TYPE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = -1;
    BOOL bSuc;
    HANDLE hDrv = NULL;
    
    hDrv = CreateFile( 
        L"XTD1:", 
        GENERIC_ALL, 
        0, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    ); 
    if (INVALID_HANDLE_VALUE == hDrv)  
    { 
        printf("***ERROR: Failed to open Driver...\n");
        nRetCode = -1;
        goto ERR; 
    }

    bSuc = DeviceIoControl(hDrv, XTD_IOCTL_PARSERXML, NULL, 0, NULL, 0, NULL, NULL);
    if (!bSuc) {
        printf("***ERROR: Failed to execute DeviceIoControl...\n");
        goto ERR;
    }

    bSuc = DeviceIoControl(hDrv, XTD_IOCTL_PARSERXML, NULL, 0, NULL, 0, NULL, NULL);
    if (!bSuc) {
        printf("***ERROR: Failed to execute DeviceIoControl...\n");
        goto ERR;
    }
    

    nRetCode = 0;

ERR:
    if (hDrv != NULL) {
        CloseHandle(hDrv);
        hDrv = NULL;
    }

	return nRetCode;
}
