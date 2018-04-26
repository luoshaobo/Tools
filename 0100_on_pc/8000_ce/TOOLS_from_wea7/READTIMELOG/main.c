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

    Retrieves the current system wake source and allows the wake sources to be enabled/disabled.

-- */

#include <windows.h>
#include <autocomp.h>
#include <autohal.h>
#include <autotrace.h>

AUTO_TIME_LOG_HEADER g_timeLogHeader;

DWORD GetSpentTime( PAUTO_TIME_LOG_HEADER pTimeLogHeader, PAUTO_TIME_LOG_DATA pTimeLog, DWORD currentIndex )
{
    DWORD spentTime     = -1;
    DWORD searchStart   = 0;
    int   index         = 0;

    if( currentIndex==0 )
    {
        if( pTimeLogHeader->status & AUTO_TIME_LOG_STATUS_ROLLED_OVER )
            searchStart = pTimeLogHeader->count-1;
        else
            goto EXIT;
    }
    else
    {
        searchStart = currentIndex-1;
    }

    for(index=searchStart; index>=0; index-- )
    {
        if( strncmp( &(pTimeLog[currentIndex].info[1]),
                     &(pTimeLog[index].info[1]),
                     AUTO_TIME_LOG_INFO_SIZE-1 )==0 )
        {
            spentTime = pTimeLog[currentIndex].timestamp - pTimeLog[index].timestamp;
            goto EXIT;
        }
    }

    if( pTimeLogHeader->status & AUTO_TIME_LOG_STATUS_ROLLED_OVER )
    {
        for(index=pTimeLogHeader->count-1; index>(int)currentIndex; index-- )
        {
            if( strncmp( &(pTimeLog[currentIndex].info[1]),
                         &(pTimeLog[index].info[1]),
                         AUTO_TIME_LOG_INFO_SIZE-1 )==0 )
            {
                spentTime = pTimeLog[currentIndex].timestamp - pTimeLog[index].timestamp;
                goto EXIT;
            }
        }
    }

EXIT:
    return spentTime;
}

void PrintTimeLog( PAUTO_TIME_LOG_HEADER pTimeLogHeader, PAUTO_TIME_LOG_DATA pTimeLog, DWORD index )
{
    static BOOL fFirstLog = TRUE;
    DWORD offset        = 0;
    DWORD relativeTick  = 0;
    DWORD ch    = 0;
    WCHAR szInfo[AUTO_TIME_LOG_INFO_SIZE+1];

    for( ch=0; ch<AUTO_TIME_LOG_INFO_SIZE; ch++ )
        szInfo[ch] = pTimeLog[index].info[ch];
    szInfo[ch] = 0;

    relativeTick = pTimeLog[index].timestamp - pTimeLogHeader->iplOffset;

    if( fFirstLog && \
        ((pTimeLogHeader->status & AUTO_TIME_LOG_STATUS_ROLLED_OVER)==AUTO_TIME_LOG_STATUS_ROLLED_OVER))
    {
        // It doesn't have any log to calculate offset.
        fFirstLog = FALSE;
        AUTO_ERROR( (L"[%-3d] %10d: %-16s [ n/a ]\r\n", \
            index, relativeTick, szInfo ) );

        return;
    }

    if( index == 0 )
    {
        if( pTimeLogHeader->status & AUTO_TIME_LOG_STATUS_ROLLED_OVER )
            offset = pTimeLog[index].timestamp - pTimeLog[pTimeLogHeader->count-1].timestamp;
        else
            offset = pTimeLog[index].timestamp;
    }
    else
    {
        offset = pTimeLog[index].timestamp - pTimeLog[index-1].timestamp;
    }

    if( szInfo[0]==L'-' )
    {
        DWORD spentTime = GetSpentTime(pTimeLogHeader, pTimeLog, index);

        if( spentTime!=-1 )
        {
            AUTO_ERROR( (L"[%-3d] %10d: %-16s [%5d][Took %5d]\r\n", \
                index, relativeTick, szInfo, offset, spentTime) );
            return;
        }
    }

    if( index == 0 && (pTimeLogHeader->status & AUTO_TIME_LOG_STATUS_ROLLED_OVER)==0 )
        AUTO_ERROR( (L"[%-3d] %10d: %-16s [%5d]\r\n", \
            index, relativeTick, szInfo, relativeTick) );
    else
        AUTO_ERROR( (L"[%-3d] %10d: %-16s [%5d]\r\n", \
            index, relativeTick, szInfo, offset) );
    return;
}

void PrintTimeLogs( PAUTO_TIME_LOG_HEADER pTimeLogHeader )
{
    DWORD index = 0;
    PAUTO_TIME_LOG_DATA pTimeLog = malloc(pTimeLogHeader->count * sizeof (AUTO_TIME_LOG_DATA));

    if ( !pTimeLog )
    {
        AUTO_ERROR( (L"Log data allocation failed") );
        return;
    }

    if ( !ReadTimeLogData( pTimeLog, sizeof(AUTO_TIME_LOG_DATA)*pTimeLogHeader->count, NULL ) )
    {
        AUTO_ERROR( (L"IOCTL_HAL_READ_TIME_LOG_DATA: Failed") );
        return;
    }
    
    AUTO_ERROR( (L"<ReadTimeLog>\r\n") );
    AUTO_ERROR( (L"###############################\r\n") );
    AUTO_ERROR( (L"Signature: 0x%08x\r\n", pTimeLogHeader->signature) );
    AUTO_ERROR( (L"Status   : 0x%08x\r\n", pTimeLogHeader->status) );
    AUTO_ERROR( (L"Index    : %d\r\n", pTimeLogHeader->index) );
    AUTO_ERROR( (L"Count    : %d\r\n", pTimeLogHeader->count) );
    AUTO_ERROR( (L"Hw Offset: %d\r\n", pTimeLogHeader->hwOffset) );
    AUTO_ERROR( (L"First IPL: %d\r\n", pTimeLogHeader->iplFirstTimeStamp) );
    AUTO_ERROR( (L"IPLOffset: %d\r\n", pTimeLogHeader->iplOffset) );
    AUTO_ERROR( (L"###############################\r\n") );
    AUTO_ERROR( (L"%5s %10s %16s %7s\r\n", L"Index", L"MSecond", L"Information  ", L"Offset") );
    AUTO_ERROR( (L"##############################################################\r\n") );

    if( pTimeLogHeader->status & AUTO_TIME_LOG_STATUS_ROLLED_OVER )
    {
        for(index=pTimeLogHeader->index; index<pTimeLogHeader->count; index++ )
            PrintTimeLog( pTimeLogHeader, pTimeLog, index );
    }

    for(index=0; index<pTimeLogHeader->index; index ++ )
        PrintTimeLog( pTimeLogHeader, pTimeLog, index );

    AUTO_ERROR( (L"##############################################################\r\n") );
    AUTO_ERROR( (L"</ReadTimeLog>\r\n") );

    free(pTimeLog);
    pTimeLog = NULL;
}


int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    // Signal the IdleBootThread to terminate
    HANDLE hEv = CreateEvent( NULL, TRUE, FALSE, _T("MA/IdleBootThread") );
    if ( hEv ) {
        SetEvent( hEv );
        CloseHandle( hEv );
    } else {
        AUTO_ERROR( (L"ReadTimeLog: CreateEvent ERR: %d\r\n", GetLastError() ) );
    }

    if( ReadTimeLogHeader( &g_timeLogHeader, sizeof(AUTO_TIME_LOG_HEADER), NULL ) )
    {
        PrintTimeLogs( &g_timeLogHeader );
    }
    else
    {
        AUTO_ERROR( (L"IOCTL_HAL_READ_TIME_LOG_HEADER: Failed") );
    }

    return 0;
}

