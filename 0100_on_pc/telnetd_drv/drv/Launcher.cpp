#include "StdAfx.h"
#include <Pkfuncs.h>
#include "Launcher.h"

#define MyCreateProcess(app, args) CreateProcess(app, args, NULL,NULL,FALSE,0,NULL,NULL,NULL,NULL)
#define MyCreateThread(fn, arg)    CreateThread(NULL, 0, fn, (LPVOID)arg, 0, NULL)

Launcher::Launcher(unsigned int nServiceId)
    : m_nServiceId(nServiceId)
    , m_sExePath()
    , m_sArgs()
{

}

Launcher::~Launcher()
{

}

bool Launcher::LaunchExe(const std::string &sExePath, const std::string &sArgs)
{
    bool bRet = true;
    HANDLE hThread;
    m_sExePath = sExePath;
    m_sArgs = sArgs;

    hThread = MyCreateThread(&LauncherThreadProc_static, this);
    if (hThread != NULL) {
        ::WaitForSingleObject(hThread, INFINITE);
        ::CloseHandle(hThread);
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

DWORD Launcher::LauncherThreadProc_static(LPVOID lpParameter)
{
    Launcher *pThis = (Launcher *)lpParameter;

    return pThis->LauncherThreadProc();
}

DWORD Launcher::LauncherThreadProc()
{
	BOOL bRet = FALSE;
	WCHAR szDevName[20];
	BOOL bResetStdioPaths = FALSE;
    DWORD dwLastError = 0;

    _sntprintf(szDevName, sizeof(szDevName) / sizeof(szDevName[0]), TEXT("TLD%u:"), m_nServiceId);

	// save current std paths
	DWORD dwLen;
	TCHAR szStdin[MAX_PATH], szStdout[MAX_PATH], szStderr[MAX_PATH];
	szStdin[0] = szStdout[0] = szStderr[0] = 0;
	dwLen = MAX_PATH;
	bRet = GetStdioPathW(0, szStdin, &dwLen);
    dwLastError = ::GetLastError();
	dwLen = MAX_PATH;
	bRet = GetStdioPathW(1, szStdout, &dwLen);
    dwLastError = ::GetLastError();
	dwLen = MAX_PATH;
	bRet = GetStdioPathW(2, szStderr, &dwLen);
    dwLastError = ::GetLastError();

	// set the new ones
	bRet = SetStdioPathW(0, szDevName);
    dwLastError = ::GetLastError();
	bRet = SetStdioPathW(1, szDevName);
    dwLastError = ::GetLastError();
	bRet = SetStdioPathW(2, szDevName);
    dwLastError = ::GetLastError();
	bResetStdioPaths = TRUE;

	bRet = MyCreateProcess(L"CMD.EXE", NULL);
    dwLastError = ::GetLastError();

	// restore std paths
	if (bResetStdioPaths) {
		bRet = SetStdioPathW(0, szStdin);
		bRet = SetStdioPathW(1, szStdout);
		bRet = SetStdioPathW(2, szStderr);
	}
    
	return bRet;
}

