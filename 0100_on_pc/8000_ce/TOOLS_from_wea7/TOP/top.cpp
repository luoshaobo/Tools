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


// Top
// Displays process/thread CPU usage,
// with biggest hogs at the top of the list
//

#include <windows.h>
#include <tlhelp32.h>
#include <pkfuncs.h>
#include <string.h>
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

#define INT64_CAST( x ) *( (_int64*)&x ) // for casting from a FILETIME

// globals for command line switches
BOOL g_fActive, g_fNoOrder, g_fNoThreads;
DWORD g_dwIterations, g_dwInterval;
LPWSTR g_pwszExeName;

typedef struct
{
    __int64 iTotalTime;
    __int64 iTickCount;
    __int64 iUserTime;
    __int64 iKernelTime;
}
TimeTrio;

// these are actually %*10, so we can get 1 decimal place on printout
typedef struct
{
    int iTotalPct;
    int iUserPct;
    int iKernelPct;
}
CpuPctTrio;

class ThreadListItem
{
public:
    ThreadListItem() : m_pNext( NULL ) {}

    ThreadListItem( THREADENTRY32& teThread )
            : m_pNext( NULL ), m_teThread( teThread ), m_fAlive( FALSE )
        { memset( &m_ttSnapTwo, 0, sizeof( TimeTrio ) ); }

    void Print();
    void CalcUsage();

public:
    THREADENTRY32 m_teThread;
    TimeTrio m_ttSnapOne, m_ttSnapTwo;
    CpuPctTrio m_cptInterval;
    ThreadListItem* m_pNext;
    BOOL m_fAlive;
};

void ThreadListItem::Print()
{
    if ( g_fActive && ( m_cptInterval.iTotalPct < 1 ) )
        return;

    // Figure out how long this thread has been running
    // FILETIME is in units of 100ns
    __int64 iSeconds, iMinutes, iHours, iDays;
    
    iSeconds = m_ttSnapTwo.iTotalTime / 10000000;
    iMinutes = iSeconds / 60;
    iSeconds %= 60;
    iHours = iMinutes / 60;
    iMinutes %= 60;
    iDays = iHours / 24;
    iHours %= 24;
    
    RETAILMSG( 1, ( L"\t\tTID 0x%08x CPU %2d.%d%% (%2d.%d%%U, %2d.%d%%K) ET %d:%d:%d:%02d\n",
                    m_teThread.th32ThreadID,
                    m_cptInterval.iTotalPct / 10, m_cptInterval.iTotalPct % 10,
                    m_cptInterval.iUserPct / 10, m_cptInterval.iUserPct % 10,
                    m_cptInterval.iKernelPct / 10, m_cptInterval.iKernelPct % 10,
                    (int)iDays, (int)iHours, (int)iMinutes, (int)iSeconds ) );
}

void ThreadListItem::CalcUsage()
{
    __int64 iIntervalTotal_1000 = (m_ttSnapTwo.iTickCount - m_ttSnapOne.iTickCount) / 1000;
    __int64 iIntervalUser = m_ttSnapTwo.iUserTime - m_ttSnapOne.iUserTime;
    __int64 iIntervalKernel = m_ttSnapTwo.iKernelTime - m_ttSnapOne.iKernelTime;

    if ( iIntervalTotal_1000 )
    {
        m_cptInterval.iTotalPct =
        (int)( (iIntervalUser + iIntervalKernel) / iIntervalTotal_1000 );
        m_cptInterval.iUserPct = (int)( iIntervalUser / iIntervalTotal_1000 );
        m_cptInterval.iKernelPct = (int)( iIntervalKernel / iIntervalTotal_1000 );
    }
    else // no div-by-zeros, please
    {
        m_cptInterval.iTotalPct = m_cptInterval.iUserPct =
        m_cptInterval.iKernelPct = 0;
    }
}

class ProcessListItem
{
public:
    ProcessListItem() : m_pNext( NULL ), m_pThreadList( new ThreadListItem() ) {}

    // constructor puts a dummy head on the list
    ProcessListItem( PROCESSENTRY32& peProcess )
            : m_pNext( NULL ), m_pThreadList( new ThreadListItem() ),
              m_peProcess( peProcess ), m_fAlive( FALSE )
        {}

    ~ProcessListItem();
    
    ThreadListItem* FindThread( DWORD dwTID );
    void AddThread( ThreadListItem* pThread );
    void OrderThreads();
    int CalcUsage();

    void Print();

public:
    PROCESSENTRY32 m_peProcess;
    CpuPctTrio m_cptInterval;
    ThreadListItem* m_pThreadList;
    ProcessListItem* m_pNext;
    BOOL m_fAlive;
};

ProcessListItem::~ProcessListItem()
{
    while ( m_pThreadList )
    {
        ThreadListItem* pKill = m_pThreadList;
        m_pThreadList = pKill->m_pNext;
        delete pKill;
    }
}

// Find the ThreadListItem associated with the Thread ID
ThreadListItem* ProcessListItem::FindThread( DWORD dwTID )
{
    if( m_pThreadList )
    {
        for ( ThreadListItem* pItem = m_pThreadList->m_pNext; pItem;
              pItem = pItem->m_pNext )
        {
            if ( pItem->m_teThread.th32ThreadID == dwTID )
                return pItem;
        }
    }
    return NULL;
}

// Add a thread to the tail of the list
// Putting it on the head would be easier, but using the
// order returned by the OS is consistent w/other tools
void ProcessListItem::AddThread( ThreadListItem* pThread )
{
    for ( ThreadListItem* pItem = m_pThreadList;
          pItem && pItem->m_pNext; pItem = pItem->m_pNext )
        ;

    pItem->m_pNext = pThread;
}

void ProcessListItem::OrderThreads()
{
    // we'll do a bubble sort, because this list shouldn't churn much
    BOOL fSwap;
    do
    {
        fSwap = FALSE;
        for ( ThreadListItem* pItem = m_pThreadList;
              pItem && pItem->m_pNext && pItem->m_pNext->m_pNext;
              pItem = pItem->m_pNext )
        {
            if ( pItem->m_pNext->m_pNext->m_cptInterval.iTotalPct >
                 pItem->m_pNext->m_cptInterval.iTotalPct )
            {
                fSwap = TRUE;
                ThreadListItem* pTemp = pItem->m_pNext;
                pItem->m_pNext = pTemp->m_pNext;
                pTemp->m_pNext = pTemp->m_pNext->m_pNext;
                pItem->m_pNext->m_pNext = pTemp;
            }
        }
    }
    while ( fSwap );
}

void ProcessListItem::Print()
{   
    if ( g_fActive && ( m_cptInterval.iTotalPct < 10 ) )
        return;
    
    RETAILMSG( 1, ( L"'%s' \tPID 0x%08x CPU %2d.%d%% (%2d.%d%%U, %2d.%d%%K)\n",
                    m_peProcess.szExeFile, m_peProcess.th32ProcessID,
                    m_cptInterval.iTotalPct / 10, m_cptInterval.iTotalPct % 10,
                    m_cptInterval.iUserPct / 10, m_cptInterval.iUserPct % 10,
                    m_cptInterval.iKernelPct / 10, m_cptInterval.iKernelPct % 10 ) );

    // print our child threads, if so ordered
    if ( !g_fNoThreads )
    {
        RETAILMSG( 1, ( L"\t\t----\n" ) );
        if( m_pThreadList )
        {
            for ( ThreadListItem* pItem = m_pThreadList->m_pNext;
                  pItem; pItem = pItem->m_pNext )
                pItem->Print();
        }
        RETAILMSG( 1, ( L"\n" ) );
    }
}

int ProcessListItem::CalcUsage()
{
    // Calc usages for all our child threads
    for ( ThreadListItem* pItem = m_pThreadList;
          pItem && pItem->m_pNext; pItem = pItem->m_pNext )
    {
        // see if this thread is still alive, if not, remove it
        if ( !pItem->m_pNext->m_fAlive )
        {
            ThreadListItem* pKill = pItem->m_pNext;
            pItem->m_pNext = pItem->m_pNext->m_pNext;
            delete pKill;
        }
        else
        {
            pItem->m_pNext->m_fAlive = FALSE;
            pItem->m_pNext->CalcUsage();
        }
    }

    __int64 iIntervalTotal = 0;
    __int64 iIntervalUser = 0;
    __int64 iIntervalKernel = 0;
    int iThreadCount = 0;
    
    // Sum all the intervals of all this process's threads
    for ( pItem = m_pThreadList->m_pNext; pItem; pItem = pItem->m_pNext )
    {
        iThreadCount++;
        
        __int64 iIntervalTemp =
        pItem->m_ttSnapTwo.iTickCount - pItem->m_ttSnapOne.iTickCount;

        // interval times for all child threads should be exactly the
        // same.  Sometimes they aren't, and it'll create an incorrectly
        // high process CPU usage if we use a low interval time
        if ( iIntervalTemp > iIntervalTotal )
            iIntervalTotal = iIntervalTemp;
        
        iIntervalUser +=
        pItem->m_ttSnapTwo.iUserTime - pItem->m_ttSnapOne.iUserTime;

        iIntervalKernel +=
        pItem->m_ttSnapTwo.iKernelTime - pItem->m_ttSnapOne.iKernelTime;
    }

    // If we have no threads, we shouldn't be here
    // this ProcessListItem should get cleaned up next pass
    if ( !iThreadCount || (iIntervalTotal == 0) )
    {
        m_fAlive = FALSE;
        memset( &m_cptInterval, 0, sizeof( CpuPctTrio ) );
        return 0;
    }

    m_cptInterval.iTotalPct =
    (int)( (iIntervalUser + iIntervalKernel) / (iIntervalTotal / 1000) );
    m_cptInterval.iUserPct = (int)( iIntervalUser / (iIntervalTotal / 1000) );
    m_cptInterval.iKernelPct = (int)( iIntervalKernel / (iIntervalTotal / 1000) );

    return m_cptInterval.iTotalPct;
}

// find the ProcessListItem associated with the given PID
ProcessListItem* FindProcess( ProcessListItem* pProcessList, DWORD dwPID )
{
    for ( ProcessListItem* pItem = pProcessList->m_pNext;
          pItem; pItem = pItem->m_pNext )
    {
        if ( pItem->m_peProcess.th32ProcessID == dwPID )
            return pItem;
    }
    return NULL;
}

// Add a process to the tail of a process list
void AddProcess( ProcessListItem* pProcessList,
                 ProcessListItem* pProcess )
{
    for ( ProcessListItem* pItem = pProcessList;
          pItem->m_pNext; pItem = pItem->m_pNext )
        ;

    pItem->m_pNext = pProcess;
}

// walk through the process list and get each process
// to calculate its usage
int CalcProcessUsage( ProcessListItem* pProcessList )
{
    int iTotalProcessUsage = 0;
    
    for ( ProcessListItem* pItem = pProcessList;
          pItem && pItem->m_pNext; pItem = pItem->m_pNext )
    {
        // see if this process is still alive, if not, remove it
        if ( !pItem->m_pNext->m_fAlive )
        {
            ProcessListItem* pKill = pItem->m_pNext;
            pItem->m_pNext = pItem->m_pNext->m_pNext;
            delete pKill;
        }
        else
        {
            pItem->m_fAlive = FALSE;
            iTotalProcessUsage += pItem->m_pNext->CalcUsage();
        }
    }

    return iTotalProcessUsage;
}

// Order the processes by decreasing total CPU usage
void OrderProcesses( ProcessListItem* pProcessList )
{
    for ( ProcessListItem* pItem = pProcessList->m_pNext;
          pItem; pItem = pItem->m_pNext )
        pItem->OrderThreads();
    
    // we'll do a bubble sort, because this list shouldn't churn much
    BOOL fSwap;
    do
    {
        fSwap = FALSE;
        for ( pItem = pProcessList;
              pItem->m_pNext && pItem->m_pNext->m_pNext;
              pItem = pItem->m_pNext )
        {
            if ( pItem->m_pNext->m_pNext->m_cptInterval.iTotalPct >
                 pItem->m_pNext->m_cptInterval.iTotalPct )
            {
                fSwap = TRUE;
                ProcessListItem* pTemp = pItem->m_pNext;
                pItem->m_pNext = pTemp->m_pNext;
                pTemp->m_pNext = pTemp->m_pNext->m_pNext;
                pItem->m_pNext->m_pNext = pTemp;
            }
        }
    }
    while ( fSwap );
}

// The money 
void PrintProcesses( ProcessListItem* pProcessList )
{
    for ( ProcessListItem* pProcess = pProcessList->m_pNext;
          pProcess; pProcess = pProcess->m_pNext )
        pProcess->Print();
}

void CreateSnapshot( ProcessListItem* pProcessList )
{
    HANDLE hSnapshot =
    CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0 );

    // BEWARE!!! GetSystemTime does not do milliseconds!
    // SystemTime is now only used for thread run time calculations.  CPU
    // usage is calculated with GetTickCount
    __int64 iSystemTime;
    {
        FILETIME ft;
        SYSTEMTIME st;
        GetSystemTime( &st );
        if( !SystemTimeToFileTime( &st, &ft ) )
        {
            return;
        }   
        iSystemTime = INT64_CAST( ft );
    }
    __int64 iTickCount = GetTickCount() * 10000;  // we want it in 100ns units
    
    PROCESSENTRY32 peProcess;
    BOOL fGotProcess;
    peProcess.dwSize = sizeof( PROCESSENTRY32 );

    for ( fGotProcess = Process32First( hSnapshot, &peProcess );
          fGotProcess;
          fGotProcess = Process32Next( hSnapshot, &peProcess ) )
    {
        peProcess.dwSize = sizeof( PROCESSENTRY32 );

        // if a process name was specified, only look for that process
        if ( g_pwszExeName &&
             ( wcscmp( g_pwszExeName, peProcess.szExeFile ) != 0 ) )
            continue;
            
        ProcessListItem* pProcessItem =
        FindProcess( pProcessList, peProcess.th32ProcessID );
        
        if ( ! pProcessItem )
            AddProcess( pProcessList,
                        pProcessItem = new ProcessListItem( peProcess ) );

        if( !pProcessItem )
        {
            break;
        }

        pProcessItem->m_fAlive = TRUE;

        THREADENTRY32 teThread;
        BOOL fGotThread;
        teThread.dwSize = sizeof( THREADENTRY32 );
                
        for ( fGotThread = Thread32First( hSnapshot, &teThread );
              fGotThread;
              fGotThread = Thread32Next( hSnapshot, &teThread ) )
        {
            teThread.dwSize = sizeof( THREADENTRY32 );
            if ( teThread.th32OwnerProcessID == peProcess.th32ProcessID )
            {
                // Find this thread item, or create a new one
                ThreadListItem* pThreadItem =
                pProcessItem->FindThread( teThread.th32ThreadID );

                if ( ! pThreadItem )
                    pProcessItem->AddThread(
                        pThreadItem = new ThreadListItem( teThread ) );

                if( !pThreadItem )
                {
                    break;
                }

                // Move the time measurements back one slot
                memcpy( &pThreadItem->m_ttSnapOne, &pThreadItem->m_ttSnapTwo,
                        sizeof( TimeTrio ) );

                FILETIME ftCreate, ftExit, ftKernel, ftUser;

                if ( GetThreadTimes( (HANDLE)teThread.th32ThreadID,
                                      &ftCreate, &ftExit, &ftKernel, &ftUser ) )
                {
                    __int64 iCreateTime = INT64_CAST( ftCreate );
                    pThreadItem->m_ttSnapTwo.iKernelTime = INT64_CAST( ftKernel );
                    pThreadItem->m_ttSnapTwo.iUserTime = INT64_CAST( ftUser );
                    pThreadItem->m_ttSnapTwo.iTotalTime = iSystemTime - iCreateTime;
                    pThreadItem->m_ttSnapTwo.iTickCount = iTickCount;
                    
                    pThreadItem->m_fAlive = TRUE;
                }
                // else // it won't fail -- trust me
            }
        }
    }
    
    CloseToolhelp32Snapshot( hSnapshot );
}

BOOL ParseCommandLine( LPCWSTR pwszCmdLine )
{
    BOOL fError = FALSE;
    
    // default -- 1 second snapshot, ordered, all threads
    g_fActive = FALSE;
    g_fNoOrder = FALSE;
    g_fNoThreads = FALSE;
    g_dwIterations = 1;
    g_dwInterval = 0;
    g_pwszExeName = NULL;

    if ( wcsstr( pwszCmdLine, L"-?" ) || wcsstr( pwszCmdLine, L"--usage" ) )
    {
        RETAILMSG( 1, ( L"usage: top [options]\noptions:\n" ) );

        LPWSTR apwszOptions[] =
        {
            L"-a --active          hide processes with < 1% CPU and threads with < .1%",
            L"   --noorder         do not order by CPU usage",
            L"   --nothreads       do not show threads in processes",
            L"-i --interval <n>    interval between snapshots, in ms",
            L"-l --loop <n>        number of iterations",
            L"-c                   continuous updates",
            L"-e --exe <name.exe>  show only process with this name",
            L"-? --usage           usage (this message)"
        };

        for ( int i = 0; i < sizeof(apwszOptions)/sizeof(apwszOptions[0]); i++ )
            RETAILMSG( 1, ( L"\t%s\n", apwszOptions[ i ] ) );

        return FALSE;
    }

    // We can't let wcstok stomp all over the string that was passed in
    LPWSTR pwszCmdLineCopy = wcsdup( pwszCmdLine );
    
    LPWSTR pwszToken = wcstok( pwszCmdLineCopy, L" " );
    LPWSTR pwszNextToken = wcstok( NULL, L" " );

    // typing this out is a hassle
#define _CMPTOKEN( x ) (wcscmp( pwszToken, (x) ) == 0)

    while ( pwszToken )
    {
        BOOL fAteTwoTokens = FALSE;
        
        if ( _CMPTOKEN( L"-a" ) || _CMPTOKEN( L"--active" ) )
        {
            g_fActive = TRUE;
        }
        else if ( _CMPTOKEN( L"--noorder" ) )
        {
            g_fNoOrder = TRUE;
        }
        else if ( _CMPTOKEN( L"--nothreads" ) )
        {
            g_fNoThreads = TRUE;
        }
        else if ( _CMPTOKEN( L"-i" ) || _CMPTOKEN( L"--interval" ) )
        {
            if ( !pwszNextToken )
            {
                RETAILMSG( 1, ( L"Top: missing parameter to --interval\n" ) );
                fError = TRUE;
                break;
            }
            g_dwInterval = _wtol( pwszNextToken );
            fAteTwoTokens = TRUE;

            if ( g_dwInterval == 0 )
            {
                RETAILMSG( 1, ( L"Top: try a value >0 for --interval\n" ) );
                fError = TRUE;
                break;
            }
        }
        else if ( _CMPTOKEN( L"-l" ) || _CMPTOKEN( L"--loop" ) )
        {
            if ( !pwszNextToken )
            {
                RETAILMSG( 1, ( L"Top: missing parameter to --loop\n" ) );
                fError = TRUE;
                break;
            }
            g_dwIterations = _wtol( pwszNextToken );
            fAteTwoTokens = TRUE;

            if ( g_dwIterations == 0 )
            {
                RETAILMSG( 1, ( L"Top: try a value >0 for --loop\n" ) );
                fError = TRUE;
                break;
            }
        }
        else if ( _CMPTOKEN( L"-c" ) )
        {
            g_dwIterations = -1; // continous looping
            if ( !g_dwInterval )
                g_dwInterval = 3000; // 3000ms works well
        }
        else if ( _CMPTOKEN( L"--exe" ) ||  _CMPTOKEN( L"-e" ) )
        {
            if ( !pwszNextToken )
            {
                RETAILMSG( 1, ( L"Top: missing parameter to --exe\n" ) );
                fError = TRUE;
                break;
            }

            if ( !wcsstr( pwszNextToken, L".exe" ) &&
                 !wcsstr( pwszNextToken, L".EXE" ) )
            {
                RETAILMSG( 1, ( L"Top: your exe name doesn't end in .exe! (it should)\n" ) );
                fError = TRUE;
                break;
            }            

            g_pwszExeName = _wcsdup( pwszNextToken );
            fAteTwoTokens = TRUE;
        }
        else // unknown option
        {
            RETAILMSG( 1, ( L"Top: unknown parameter '%s'\n", pwszToken ) );
            fError = TRUE;
            break;
        }

        pwszToken = fAteTwoTokens ? wcstok( NULL, L" " ) : pwszNextToken;
        pwszNextToken = wcstok( NULL, L" " );
    }
#undef _CMPTOKEN

    if ( !g_dwInterval )
        g_dwInterval = 1000; // default to 1s interval
    
    free( pwszCmdLineCopy );
    
    return !fError;
}

ProcessListItem* g_pProcessList = NULL;

WINAPI WinMain( HINSTANCE, HINSTANCE, LPTSTR lpCmdLine, int )
{
    if ( !ParseCommandLine( lpCmdLine ) )
    {
        if ( g_pwszExeName )
            free( g_pwszExeName );
        return 1;
    }
    
    // put on a dummy list head
    g_pProcessList = new ProcessListItem();

    if( !g_pProcessList )
    {
        return 1;
    }

    // Loop around! Loop around!
    while ( (g_dwIterations == -1) || (g_dwIterations--) )
    {
        RETAILMSG( 1, ( L"\n\nTop -- %dms interval -----------------\n",
                        g_dwInterval ) );

        
        DWORD
        dwIdleSnap1 = GetIdleTime(),
        dwTimeSnap1 = GetTickCount();
        
        CreateSnapshot( g_pProcessList );

        Sleep( g_dwInterval );
        
        CreateSnapshot( g_pProcessList );

        DWORD
        dwIdleTotal = GetIdleTime() - dwIdleSnap1,
        dwTimeTotal = GetTickCount() - dwTimeSnap1;

        __int64 iCpuUsage =
        (__int64)(dwTimeTotal - dwIdleTotal) * 1000 / dwTimeTotal;

        int iTotalProcessUsage = CalcProcessUsage( g_pProcessList );
        
        RETAILMSG( 1, ( L"Total CPU usage: (GetIdleTime) %d.%d%% (Accumulated GetThreadTimes) %d.%d%%\n",
                        (int)(iCpuUsage / 10), (int)(iCpuUsage % 10),
                        iTotalProcessUsage / 10, iTotalProcessUsage % 10) );
        
        if ( !g_fNoOrder )
            OrderProcesses( g_pProcessList );
        PrintProcesses( g_pProcessList );
    }

    // take out the trash
    if ( g_pwszExeName )
        free( g_pwszExeName );
    
    while ( g_pProcessList )
    {
        ProcessListItem* pKill = g_pProcessList;
        g_pProcessList = g_pProcessList->m_pNext;
        delete pKill;
    }

    return 0;
}







