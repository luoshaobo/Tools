/****************************** Module Header ******************************\
* Module Name:  Service.cpp
* Project:      CppWin7TriggerStartService
* Copyright (c) Microsoft Corporation.
* 
* The file implements the skeleton of the Windows service. These functions 
* can be reused by all Windows services:
* 
*   SvcMain            Entry point for the service
*   SvcCtrlHandler     Service control handler
*   SvcReportStatus    Report service status to SCM
*   SvcReportEvent     Report an event to Windows event log
* 
* You can customize the service body by rewriting the SvcInit and SvcStop 
* functions. They are called when the service is being started and when the 
* service is to be stopped respectively. For example, you can create a named 
* pipe and monitor the named pipe in SvcInit, and close the named pipe in 
* SvcStop. In this sample, we just report the function call information to 
* the Application log in the two functions to keep the sample simple.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/en-us/openness/licenses.aspx#MPL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "stdafx.h"
#include <windows.h>
#include <WtsApi32.h>
#include <UserEnv.h>
#include <process.h>
#include <stdio.h>
#include "Service.h"


SERVICE_STATUS          g_ssSvcStatus;         // Current service status
SERVICE_STATUS_HANDLE   g_sshSvcStatusHandle;  // Current service status handle
HANDLE                  g_hSvcStopEvent;


void WINAPI SvcMain(DWORD dwArgc, LPWSTR* lpszArgv);
void WINAPI SvcCtrlHandler(DWORD dwCtrl);
void SvcInit(DWORD dwArgc, LPWSTR* lpszArgv);
void SvcStop();
void SvcReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
void SvcReportEvent(LPWSTR lpszFunction, DWORD dwErr = 0);


//
//  FUNCTION: RunService
//
//  PURPOSE: Run the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void RunService()
{
	// You can add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY dispatchTable[] = 
	{
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)SvcMain }, 
		{ NULL, NULL }
	};

	// This call returns when the service has stopped.
	// The process should simply terminate when the call returns.
	if (!StartServiceCtrlDispatcher(dispatchTable))
	{
		SvcReportEvent(L"StartServiceCtrlDispatcher", GetLastError());
	}
}

DWORD LaunchAppIntoSession0(LPTSTR lpCommand)
{
	////////////////////////////////////////////system show dlg////////////////////

    MS_LOG_FUNC();

	HDESK hdeskCurrent = NULL;
	HDESK hdesk = NULL;
	HWINSTA hwinstaCurrent = NULL;
	HWINSTA hwinsta = NULL;

    MS_LOG_FUNC();
	hwinstaCurrent = GetProcessWindowStation();
	if (hwinstaCurrent == NULL)
	{
        MS_LOG_FUNC();
		return FALSE;
	}

    MS_LOG_FUNC();
	hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
	if (hdeskCurrent == NULL){
        MS_LOG_FUNC();
		return FALSE;
	}
	//打开winsta0
	//打开winsta0
    MS_LOG_FUNC();
	hwinsta = OpenWindowStation(L"Winsta0", FALSE,WINSTA_ALL_ACCESS);
	// 		WINSTA_ACCESSCLIPBOARD|
	// 		WINSTA_ACCESSGLOBALATOMS |
	// 		WINSTA_ENUMDESKTOPS |
	// 		WINSTA_CREATEDESKTOP |
	// 		WINSTA_CREATEDESKTOP |
	// 		WINSTA_ENUMERATE |
	// 		WINSTA_EXITWINDOWS |
	// 		WINSTA_READATTRIBUTES |
	// 		WINSTA_READSCREEN |
	// 		WINSTA_WRITEATTRIBUTES);
	if (hwinsta == NULL){
        MS_LOG_FUNC();
		return FALSE;
	}

    MS_LOG_FUNC();
	if (!SetProcessWindowStation(hwinsta))
	{
        MS_LOG_FUNC();
		return FALSE;
	}
	//打开desktop
    MS_LOG_FUNC();
	hdesk = OpenDesktop(L"default", 0, FALSE,
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE|
		DESKTOP_HOOKCONTROL|
		DESKTOP_JOURNALPLAYBACK |
		DESKTOP_JOURNALRECORD |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		DESKTOP_WRITEOBJECTS);
	if (hdesk == NULL){
        MS_LOG_FUNC();
		return FALSE;
	}
    MS_LOG_FUNC();
	SetThreadDesktop(hdesk);
	////////////////////////////////////////////end of system show dlg////////////////////
	/*
	CShowDialog propSheet;
	//m_pMainWnd = &propSheet;
	INT_PTR nResponse = propSheet.DoModal();

	if (nResponse == IDOK)
	{
	// TODO: 在此放置处理何时用
	//  “确定”来关闭对话框的代码
	DebugTrace2LogFile(_T("DoModal IDOK"));

	}
	else if (nResponse == IDCANCEL)
	{
	// TODO: 在此放置处理何时用
	//  “取消”来关闭对话框的代码
	DebugTrace2LogFile(_T("DoModal IDCANCEL"));
	}*/
    MS_LOG_FUNC();
	STARTUPINFO si = { sizeof(si) };  
	SECURITY_ATTRIBUTES saProcess, saThread;  
	PROCESS_INFORMATION piProcessB;  

    MS_LOG_FUNC();
	// Prepare to spawn Process B from Process A.  
	// The handle identifying the new process  
	// object should be inheritable.  
	saProcess.nLength = sizeof(saProcess);  
	saProcess.lpSecurityDescriptor = NULL;  
	saProcess.bInheritHandle = TRUE;  

    MS_LOG_FUNC();
	// The handle identifying the new thread  
	// object should NOT be inheritable.  
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;  

    MS_LOG_FUNC();
	CreateProcess(NULL, lpCommand, &saProcess, &saThread,  
		FALSE, 0, NULL, NULL, &si, &piProcessB);  

    MS_LOG_FUNC();
    if (!SetProcessWindowStation(hwinstaCurrent)) {
        MS_LOG_FUNC();
		return FALSE;
    }

    MS_LOG_FUNC();
    if (!SetThreadDesktop(hdeskCurrent)) {
        MS_LOG_FUNC();
		return FALSE;
    }

    MS_LOG_FUNC();
    if (!CloseWindowStation(hwinsta)) {
        MS_LOG_FUNC();
		return FALSE;
    }

    MS_LOG_FUNC();
    if (!CloseDesktop(hdesk)) {
        MS_LOG_FUNC();
		return FALSE;
    }

	return TRUE;
}

DWORD LaunchAppIntoDifferentSession( LPTSTR lpCommand )
{
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	DWORD dwSessionId;
	HANDLE hUserToken = NULL;
	HANDLE hUserTokenDup = NULL;
	HANDLE hPToken = NULL;
	HANDLE hProcess = NULL;
	DWORD dwCreationFlags;

    MS_LOG_FUNC();
	HMODULE hInstKernel32    = NULL;
	typedef DWORD (WINAPI *WTSGetActiveConsoleSessionIdPROC)(); 
	WTSGetActiveConsoleSessionIdPROC WTSGetActiveConsoleSessionId = NULL;

    MS_LOG_FUNC();
	hInstKernel32 = LoadLibrary(L"Kernel32.dll");

    MS_LOG_FUNC();
	if (!hInstKernel32)  
	{ 
        MS_LOG_FUNC();
		return FALSE; 
	}

    MS_LOG_FUNC();
	OutputDebugString(L"LaunchAppIntoDifferentSession 1\n");
	WTSGetActiveConsoleSessionId = (WTSGetActiveConsoleSessionIdPROC)GetProcAddress(hInstKernel32,"WTSGetActiveConsoleSessionId");

    MS_LOG_FUNC();
	// Log the client on to the local computer.
	dwSessionId = WTSGetActiveConsoleSessionId();

    MS_LOG_FUNC();
	do
	{
        MS_LOG_FUNC();
		WTSQueryUserToken( dwSessionId,&hUserToken );
		dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
		ZeroMemory( &si, sizeof( STARTUPINFO ) );
		si.cb= sizeof( STARTUPINFO );
		si.lpDesktop = L"winsta0\\default";
		ZeroMemory( &pi, sizeof(pi) );
		TOKEN_PRIVILEGES tp;
		LUID luid;

        MS_LOG_FUNC();
		if( !::OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
			| TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
			| TOKEN_READ | TOKEN_WRITE, &hPToken ) )
		{
            MS_LOG_FUNC();
			dwRet = GetLastError();
			break;
		}
		else;

        MS_LOG_FUNC();
		if ( !LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &luid ) )
		{
            MS_LOG_FUNC();
			dwRet = GetLastError();
			break;
		}
		else;

        MS_LOG_FUNC();
		tp.PrivilegeCount =1;
		tp.Privileges[0].Luid =luid;
		tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;

        MS_LOG_FUNC();
		if( !DuplicateTokenEx( hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup ) )
		{
            MS_LOG_FUNC();
			dwRet = GetLastError();
			break;
		}
		else;

        MS_LOG_FUNC();
		//Adjust Token privilege
		if( !SetTokenInformation( hUserTokenDup,TokenSessionId,(void*)&dwSessionId,sizeof(DWORD) ) )
		{
            MS_LOG_FUNC();
			dwRet = GetLastError();
			break;
		}
		else;

        MS_LOG_FUNC();
		if( !AdjustTokenPrivileges( hUserTokenDup, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL ) )
		{
            MS_LOG_FUNC();
			dwRet = GetLastError();
			break;
		}
		else;

        MS_LOG_FUNC();
		LPVOID pEnv =NULL;
		if( CreateEnvironmentBlock( &pEnv, hUserTokenDup, TRUE ) )
		{
            MS_LOG_FUNC();
			dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
		}
		else pEnv=NULL;
		
        MS_LOG_FUNC();
		// Launch the process in the client's logon session.
		if( CreateProcessAsUser(    hUserTokenDup,    // client's access token
			NULL,        // file to execute
			lpCommand,        // command line
			NULL,            // pointer to process SECURITY_ATTRIBUTES
			NULL,            // pointer to thread SECURITY_ATTRIBUTES
			FALSE,            // handles are not inheritable
			dwCreationFlags,// creation flags
			pEnv,          // pointer to new environment block
			NULL,          // name of current directory
			&si,            // pointer to STARTUPINFO structure
			&pi            // receives information about new process
			) )
		{
            MS_LOG_FUNC();
		}
		else
		{   
            MS_LOG_FUNC();
			dwRet = GetLastError();
			break;
		}
	}
	while( 0 );

	//Perform All the Close Handles task
    MS_LOG_FUNC();
	if( NULL != hUserToken )
	{
        MS_LOG_FUNC();
		CloseHandle( hUserToken );
	}
	else;

    MS_LOG_FUNC();
	if( NULL != hUserTokenDup)
	{
        MS_LOG_FUNC();
		CloseHandle( hUserTokenDup );
	}
	else;

    MS_LOG_FUNC();
	if( NULL != hPToken )
	{
		CloseHandle( hPToken );
	}
	else;

    MS_LOG_FUNC();
	return dwRet;
}

void StartProcess(const std::wstring &sCmdLine)
{
    tchar_t arrCmdLine[1024] = { 0 };
    
    MS_LOG_FUNC();
    if (sCmdLine.length() == 0) {
        MS_LOG_FUNC();
        return;
    } else if (sCmdLine.length() >= TK_ARR_LEN(arrCmdLine)) {
        MS_LOG_FUNC();
        return;
    }

    MS_LOG_FUNC();
    wcsncpy(arrCmdLine, sCmdLine.c_str(), sCmdLine.length());

    MS_LOG_FUNC();
    OSVERSIONINFO OsVerInfoEx = { 0 };
	OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVerInfoEx); 

    MS_LOG_FUNC();
    if ( OsVerInfoEx.dwMajorVersion < 6 )
	{
        MS_LOG_FUNC();
		LaunchAppIntoSession0(arrCmdLine);
	}
	else
	{
        MS_LOG_FUNC();
		LaunchAppIntoDifferentSession(arrCmdLine);
	}

    MS_LOG_FUNC();
}

void SubThreadProc(void *pArg)
{
    StartProcess(L"cmd.ext");
}

void StartSubThread()
{
    _beginthread(&SubThreadProc, 0, NULL);
}

//
//  FUNCTION: SvcMain
//
//  PURPOSE: Entry point for the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls the 
//    user defined SvcInit() routine to perform majority of the work.
//
void WINAPI SvcMain(DWORD dwArgc, LPWSTR* lpszArgv)
{
	SvcReportEvent(L"Enter SvcMain");

	// Register the handler function for the service
	g_sshSvcStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, SvcCtrlHandler);
	if (!g_sshSvcStatusHandle)
	{
		SvcReportEvent(L"RegisterServiceCtrlHandler", GetLastError());
		return; 
	} 

	// These SERVICE_STATUS members remain as set here
	g_ssSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
	g_ssSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM
	SvcReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.
	SvcInit(dwArgc, lpszArgv);
}


//
//  FUNCTION: SvcCtrlHandler
//
//  PURPOSE: Called by SCM whenever a control code is sent to the service 
//           using the ControlService function.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code.
	switch(dwCtrl) 
	{  
	case SERVICE_CONTROL_STOP: 
		// Stop the service

		// SERVICE_STOP_PENDING should be reported before setting the Stop 
		// Event - g_hSvcStopEvent - in SvcStop(). This avoids a race 
		// condition which may result in a 1053 - The Service did not 
		// respond... error.
		SvcReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		SvcStop();

		SvcReportStatus(g_ssSvcStatus.dwCurrentState, NO_ERROR, 0);

		return;

	case SERVICE_CONTROL_INTERROGATE: 
		break; 

	default:
		break;
	} 

}

//
//  FUNCTION: SvcInit
//
//  PURPOSE: Actual code of the service that does the work.
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void SvcInit(DWORD dwArgc, LPWSTR* lpszArgv)
{
	SvcReportEvent(L"Enter SvcInit");

	// Service initialization.

	// Declare and set any required variables. Be sure to periodically call 
	// ReportSvcStatus() with SERVICE_START_PENDING. If initialization fails, 
	// call ReportSvcStatus with SERVICE_STOPPED.

	// Create a manual-reset event that is not signaled at first. The control 
	// handler function, SvcCtrlHandler, signals this event when it receives 
	// the stop control code.
	g_hSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hSvcStopEvent == NULL)
	{
		SvcReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.
	SvcReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

    //StartSubThread();

   // Perform work until service stops.  Stop when Control handler
   // calls SvcStop() to signal g_hSvcStopEvent.
   do
   {
      // Perform a piece of work ...
      Sleep(1000);  // simulate work

   } while (WaitForSingleObject(g_hSvcStopEvent, 0) == WAIT_TIMEOUT);

   SvcReportStatus(SERVICE_STOPPED, NO_ERROR, 0);

}


//
//  FUNCTION: SvcStop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a SvcStop procedure is going to take longer than 3 seconds to 
//    execute, it should spawn a thread to execute the stop code, and return. 
//    Otherwise, the ServiceControlManager will believe that the service has
//    stopped responding.
//
void SvcStop()
{
	SvcReportEvent(L"Enter SvcStop");

	// Signal the service to stop.
	if (g_hSvcStopEvent)
	{
		SetEvent(g_hSvcStopEvent);
	}
}


//
//  FUNCTION: SvcReportStatus
//
//  PURPOSE: Sets the current service status and reports it to the SCM.
//
//  PARAMETERS:
//    dwCurrentState - the state of the service (see SERVICE_STATUS)
//    dwWin32ExitCode - error code to report
//    dwWaitHint - Estimated time for pending operation, in milliseconds
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void SvcReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, 
					      DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	g_ssSvcStatus.dwCurrentState = dwCurrentState;
	g_ssSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ssSvcStatus.dwWaitHint = dwWaitHint;

	g_ssSvcStatus.dwControlsAccepted = 
		(dwCurrentState == SERVICE_START_PENDING) ? 
		0 : SERVICE_ACCEPT_STOP;

	g_ssSvcStatus.dwCheckPoint = 
		((dwCurrentState == SERVICE_RUNNING) || 
		(dwCurrentState == SERVICE_STOPPED)) ? 0 : dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(g_sshSvcStatusHandle, &g_ssSvcStatus);
}


//
//  FUNCTION: SvcReportEvent
//
//  PURPOSE: Allows any thread to log an error message
//
//  PARAMETERS:
//    lpszFunction - name of function that failed
//    dwErr - error code returned from the function
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void SvcReportEvent(LPWSTR lpszFunction, DWORD dwErr) 
{
// 	HANDLE hEventSource;
// 	LPCWSTR lpszStrings[2];
// 	wchar_t szBuffer[80];
// 
// 	hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
// 	if (NULL != hEventSource)
// 	{
// 		WORD wType;
// 		if (dwErr == 0)
// 		{
// 			swprintf_s(szBuffer, ARRAYSIZE(szBuffer), lpszFunction);
// 			wType = EVENTLOG_INFORMATION_TYPE;
// 		}
// 		else
// 		{
// 			swprintf_s(szBuffer, ARRAYSIZE(szBuffer), L"%s failed w/err 0x%08lx", 
// 				lpszFunction, dwErr);
// 			wType = EVENTLOG_ERROR_TYPE;
// 		}
// 
// 		lpszStrings[0] = SERVICE_NAME;
// 		lpszStrings[1] = szBuffer;
// 
// 		ReportEvent(hEventSource,  // Event log handle
// 			wType,                 // Event type
// 			0,                     // Event category
// 			0,                     // Event identifier
// 			NULL,                  // No security identifier
// 			2,                     // Size of lpszStrings array
// 			0,                     // No binary data
// 			lpszStrings,           // Array of strings
// 			NULL);                 // No binary data
// 
// 		DeregisterEventSource(hEventSource);
// 	}
}