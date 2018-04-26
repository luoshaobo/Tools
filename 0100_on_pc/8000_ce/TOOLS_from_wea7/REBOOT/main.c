//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
/* ++
 
    Reboot utility that supports all the reboot flavors.

-- */

#include <windows.h>
#include <autocomp.h>
#include <autohal.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef RETAILMSG
#undef RETAILMSG
#endif
#define RETAILMSG(level,format)             wprintf format

#ifdef Debug
#undef Debug
#endif
#define Debug(format,...)                   wprintf(format, __VA_ARGS__)

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    ULONG rebootType = 0;
    ULONG updateFlag = 0;

    ULONG reliabilitySvcParams[3];

    RETAILMSG(1, (TEXT("=================== Reboot Utility =================== \r\n")));

    if(lpCmdLine && lstrcmpi(lpCmdLine, L"warm")==0)
    {
        //
        // Warm reboot
        //
        RETAILMSG(1, (TEXT("Warm reboot...\r\n")));

		// flush the important registry first for warmboot
		RegFlushKey(HKEY_LOCAL_MACHINE);
		RegFlushKey(HKEY_CURRENT_USER);

        rebootType = HAL_REBOOT_WARM;
        if(!KernelIoControl(IOCTL_HAL_REBOOT, &rebootType, sizeof(ULONG), 0, 0, NULL))
        {
            RETAILMSG(1, (TEXT("%s: KernelIoControl(IOCTL_HAL_REBOOT, HAL_REBOOT_WARM) failed!\r\n")));
            goto EXIT;
        }
    }


    if(lpCmdLine && lstrcmpi(lpCmdLine, L"CPM")==0)
    {
        //
        // Cold reboot
        //
        RETAILMSG(1, (TEXT("CPM reboot...\r\n")));
        rebootType = HAL_REBOOT_CPM|1;
        if(!KernelIoControl(IOCTL_HAL_REBOOT, &rebootType, sizeof(ULONG), 0, 0, NULL))
        {
            RETAILMSG(1, (TEXT("%s: KernelIoControl(IOCTL_HAL_REBOOT, HAL_REBOOT_CPM) failed!\r\n")));
            goto EXIT;
        }
    }

    if(lpCmdLine && lstrcmpi(lpCmdLine, L"cold")==0)
    {
        //
        // Cold reboot
        //
        RETAILMSG(1, (TEXT("Cold reboot...\r\n")));
        rebootType = HAL_REBOOT_COLD;
        if(!KernelIoControl(IOCTL_HAL_REBOOT, &rebootType, sizeof(ULONG), 0, 0, NULL))
        {
            RETAILMSG(1, (TEXT("%s: KernelIoControl(IOCTL_HAL_REBOOT, HAL_REBOOT_COLD) failed!\r\n")));
            goto EXIT;
        }
    }

    if(lpCmdLine && lstrcmpi(lpCmdLine, L"delayed")==0)
    {
        //
        // Delayed reboot
        //
        RETAILMSG(1, (TEXT("Delayed reboot.  Reboot will take place upon next suspend event...\r\n")));
        
        reliabilitySvcParams[0] = REBOOT_PARAM_SIGNATURE | 0x1;
        reliabilitySvcParams[1] = ERROR_SUCCESS;
        reliabilitySvcParams[2] = 0;
        
        if(!KernelIoControl(IOCTL_REBOOT_HANDLER, &reliabilitySvcParams, sizeof(reliabilitySvcParams), 0, 0, NULL))
        {
			//
			// Returning false simply means it won't reboot immediately
			//
            //RETAILMSG(1, (TEXT("KernelIoControl(IOCTL_REBOOT_HANDLER) failed!\r\n")));
            //goto EXIT;
        }
    }

    if(lpCmdLine && lstrcmpi(lpCmdLine, L"powersave")==0)
    {
        RETAILMSG(1, (TEXT("Power save shutdown...\r\n")));
        rebootType = HAL_REBOOT_SHUTDOWN;
        if(!KernelIoControl(IOCTL_HAL_REBOOT, &rebootType, sizeof(ULONG), 0, 0, NULL))
        {
            RETAILMSG(1, (TEXT("%s: KernelIoControl(IOCTL_HAL_REBOOT, HAL_REBOOT_SHUTDOWN) failed!\r\n")));
            goto EXIT;
        }
    }

    if(lpCmdLine && lstrcmpi(lpCmdLine, L"uldr")==0)
    {
        RETAILMSG(1, (TEXT("Power save shutdown...\r\n")));
        rebootType = HAL_REBOOT_ULDR;
        if(!KernelIoControl(IOCTL_HAL_REBOOT, &rebootType, sizeof(ULONG), 0, 0, NULL))
        {
            RETAILMSG(1, (TEXT("%s: KernelIoControl(IOCTL_HAL_REBOOT, HAL_REBOOT_ULDR) failed!\r\n")));
            goto EXIT;
        }
    }    
  
    if((lpCmdLine && lstrcmpi(lpCmdLine, L"/?")==0) ||
       (lpCmdLine && lstrcmpi(lpCmdLine, L"-?")==0) ||
       (lpCmdLine && lstrcmpi(lpCmdLine, L"?")==0))
    {
        //
        // Help
        //
        RETAILMSG(1, (TEXT("Usage: reboot {warm | cold | CPM | delayed | uldr | powersave}\r\n")));
    }
    
    RETAILMSG(1, (TEXT("=============================================================== \r\n")));

EXIT:
    return 0;
}
