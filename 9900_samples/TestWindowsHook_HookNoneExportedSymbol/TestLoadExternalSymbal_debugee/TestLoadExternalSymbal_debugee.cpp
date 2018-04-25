// TestLoadExternalSymbal_debugee.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////

extern int CreateTestWindow(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow);

static DWORD WINAPI TestWindowThread(LPVOID lpParameter)
{
    HINSTANCE hInstance;
    hInstance = (HINSTANCE)GetModuleHandle(NULL);

    return CreateTestWindow(hInstance, NULL, NULL, SW_HIDE);
}

////////////////////////////////////////////////////////////////////////////////////////

class FuncToBeHookedForDllInjectionClass
{
public:
    int FuncToBeHookedForDllInjection(int arg1, int arg2, int arg3);
};

int FuncToBeHookedForDllInjectionClass::FuncToBeHookedForDllInjection(int arg1, int arg2, int arg3)
{
    static unsigned int i = 0;

    printf("[%08u] FuncToBeHookedForDllInjection(%d, %d, %d)", i++, arg1, arg2, arg3);
    return arg1 + arg2 + arg3;
}

////////////////////////////////////////////////////////////////////////////////////////

extern int GGG(int n);
extern int TTT_PubF(char *p);
int _tmain(int argc, _TCHAR* argv[])
{
    int nTotal;
    HANDLE hThread;
    
    hThread = CreateThread(NULL, 0, &TestWindowThread, NULL, 0, NULL);
    if (hThread == NULL) {
        return -1;
    }
    CloseHandle(hThread);

    for (unsigned int i = 0; ;i++) {
        nTotal = FuncToBeHookedForDllInjectionClass().FuncToBeHookedForDllInjection(11, 22, 33);
        printf("=%d\n", nTotal);
        GGG(20);
        TTT_PubF("abc");

        Sleep(3000);
    }
    
	return 0;
}
