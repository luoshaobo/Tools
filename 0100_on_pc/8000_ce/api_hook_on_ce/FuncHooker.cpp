#include "StdAfx.h"
#include <Tlhelp32.h>
#include "FuncHooker.h"

#if 0
#define LOG_MSG                 printf
#define LOG_LINE()              printf("%s() [line:%d]\n", __FUNCTION__, __LINE__)
#else
#define LOG_MSG(...)
#define LOG_LINE()
#endif

FuncHooker::FuncHooker(void *pfTarget, void *pfDetour, void **ppfTrampoline)
    : m_pfTarget(pfTarget)
    , m_pfDetour(pfDetour)
    , m_ppfTrampoline(ppfTrampoline)
    , m_pfTrampoline(NULL)
    , m_nTrampolineSize(0)
{
    FixFuncAddress(&m_pfTarget);
    FixFuncAddress(&m_pfDetour);
}

FuncHooker::~FuncHooker(void)
{
    if (m_nTrampolineSize != 0 && m_pfTrampoline != NULL) {
        //Unhook();
    }
}

BOOL FuncHooker::Hook(const std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    int nJumpDistance1, nJumpDistance2;
    DWORD nOldPermission, nNewPermissiion;

    LOG_LINE();
    if (m_pfTarget == NULL || m_pfDetour == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    LOG_LINE();
    if (m_nTrampolineSize != 0 && m_pfTrampoline != NULL) {
        goto SUC;
    }
    
    LOG_LINE();
    bSuc = CalcTrampolineSize();
    if (!bSuc) {
        goto FAILED;
    }

    LOG_LINE();
    m_pfTrampoline = ::VirtualAllocEx(::GetCurrentProcess(), NULL, m_nTrampolineSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    bSuc = ::WriteProcessMemory(::GetCurrentProcess(), m_pfTrampoline, m_pfTarget, m_nTrampolineSize - JUMP_INSTRUCTION_SIZE, NULL);
    if (!bSuc) {
        goto FAILED;
    }

    //{
    //    nOldPermissions = ::SetProcPermissions(0xFFFFFFFF);
    //    DWORD nXXX = ::SetProcPermissions(0xFFFFFFFF);
    //    nXXX = nXXX;
    //    bSuc = ::SetKMode(TRUE);
    //    bSuc = ::SetKMode(TRUE);
    //    if (!bSuc){
    //        volatile DWORD nError = ::GetLastError();
    //        nError = nError;
    //        goto FAILED;
    //    }

    //    MEMORY_BASIC_INFORMATION mbi = {0};
    //    VirtualQuery(m_pfTarget, &mbi, sizeof(mbi));

    //    bSuc = VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &nOldPermission);
    //    if (!bSuc){
    //        volatile DWORD nError = ::GetLastError();
    //        nError = nError;
    //        goto FAILED;
    //    }
    //}

#if 0
    LOG_LINE();
    nJumpDistance1 = (DWORD)m_pfDetour - (DWORD)m_pfTarget - JUMP_INSTRUCTION_SIZE;
    bSuc = ::VirtualProtect(m_pfTarget, JUMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &nOldPermission);
    if (!bSuc){
        volatile DWORD nError = ::GetLastError();
        nError = nError;
        goto FAILED;
    }
#else
    LOG_LINE();
    nJumpDistance1 = (DWORD)m_pfDetour - (DWORD)m_pfTarget - JUMP_INSTRUCTION_SIZE;
    bSuc = ::VirtualProtect(m_pfTarget, JUMP_INSTRUCTION_SIZE, PAGE_READWRITE, &nOldPermission);
    if (!bSuc){
        volatile DWORD nError = ::GetLastError();
        nError = nError;
        goto FAILED;
    }
#endif // #if 0

#if defined(WIN32_X86) // for Win32 X86
    {
        *(unsigned char *)m_pfTarget = (unsigned char)0xE9;
        *(int *)((unsigned int)m_pfTarget + 1) = nJumpDistance1;
        ::VirtualProtect(m_pfTarget, JUMP_INSTRUCTION_SIZE, nOldPermission, &nNewPermissiion);
    }
#elif defined(WINCE_ARM32) // for WinCE ARM32
    nJumpDistance1 = 0;
    nJumpDistance2 = 0;
    nOldPermission = 0;
    nNewPermissiion = 0;
    bSuc = CreateJumpInstruction((unsigned int)m_pfDetour, (void *)m_pfTarget, JUMP_INSTRUCTION_SIZE);
    if (!bSuc) {
        goto FAILED;
    }
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

#if defined(WIN32_X86) // for Win32 X86
    nJumpDistance2 = (DWORD)m_pfTarget - (DWORD)m_pfTrampoline - JUMP_INSTRUCTION_SIZE;
    *(unsigned char *)((unsigned int)m_pfTrampoline + m_nTrampolineSize - JUMP_INSTRUCTION_SIZE) = (unsigned char)0xE9;
    *(int *)((unsigned int)m_pfTrampoline + m_nTrampolineSize - JUMP_INSTRUCTION_SIZE + 1) = nJumpDistance2;
#elif defined(WINCE_ARM32) // for WinCE ARM32
    bSuc = CreateJumpInstruction((unsigned int)m_pfTarget, (void *)((unsigned int)m_pfTrampoline + m_nTrampolineSize - JUMP_INSTRUCTION_SIZE), JUMP_INSTRUCTION_SIZE);
    if (!bSuc) {
        goto FAILED;
    }
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

    LOG_LINE();
    if (m_ppfTrampoline != NULL) {
        *m_ppfTrampoline = m_pfTrampoline;
    }

    LOG_LINE();
    bSuc = FlushICache();
    if (!bSuc) {
        goto FAILED;
    }

    LOG_LINE();
SUC:
    bSuc = TRUE;
    return bSuc;
FAILED:
    if (m_nTrampolineSize != 0  && m_pfTrampoline != NULL) {
        ::VirtualFreeEx(::GetCurrentProcess(), m_pfTrampoline, m_nTrampolineSize, MEM_RELEASE);
        m_nTrampolineSize = 0;
        m_pfTrampoline = NULL;
    }
    return bSuc;
}

#if defined(WINCE_ARM32) // for WinCE ARM32
BOOL FuncHooker::CreateJumpInstruction(unsigned int nDestAddress, void *pBuf, unsigned int nBufSize)
{
    if (pBuf == NULL || nBufSize < JUMP_INSTRUCTION_SIZE) {
        return FALSE;
    }

    if ((nDestAddress & 0x03) != 0) {
        return FALSE;
    }

    unsigned int *nJumpInst = ((unsigned int *)pBuf) + 0;                    // ldr r15, [r15, 0]
    *nJumpInst = 0
        | ((0x0E & 0x0F) << 27)                     // cond
        | ((0x01 & 0x03) << 25)
        | ((0x00 & 0x01) << 24)
        | ((0x01 & 0x01) << 23)                     // U
        | ((0x00 & 0x01) << 22)                     // B
        | ((0x00 & 0x01) << 21)
        | ((0x01 & 0x01) << 20)                     // L
        | ((0x0F & 0x0F) << 15)                     // Rn
        | ((0x0F & 0x0F) << 11)                     // Rd
        | ((0x0000 & 0x0FFF) << 0)                  // Offset_12
    ;

    unsigned int *nNoopInst = ((unsigned int *)pBuf) + 1;                    // mov r0, r0
    *nNoopInst = 0xe1a00000;

    unsigned int *nDestAddressInst = ((unsigned int *)pBuf) + 2;             // dest addr (not an instruction!)
    *nDestAddressInst = nDestAddress;

    return TRUE;
}
#endif // #if defined(WINCE_ARM32)

BOOL FuncHooker::Unhook(const std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    DWORD nOldPermission, nNewPermissiion;

    if (m_pfTarget == NULL || m_pfDetour == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    if (m_nTrampolineSize == 0 || m_pfTrampoline == NULL) {
        goto SUC;
    }

    bSuc = ::VirtualProtect(m_pfTarget, JUMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &nOldPermission);
    if (!bSuc) {
        goto FAILED;
    }
    ::WriteProcessMemory(::GetCurrentProcess(), m_pfTarget, m_pfTrampoline, JUMP_INSTRUCTION_SIZE, NULL);
    ::VirtualFreeEx(::GetCurrentProcess(), m_pfTrampoline, m_nTrampolineSize, MEM_RELEASE);
    ::VirtualProtect(m_pfTarget, JUMP_INSTRUCTION_SIZE, nOldPermission, &nNewPermissiion);

    m_nTrampolineSize = 0;
    m_pfTrampoline = NULL;

    LOG_LINE();
    bSuc = Unhook(vecThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    bSuc = FlushICache();
    if (!bSuc) {
        goto FAILED;
    }

SUC:
    bSuc = TRUE;
FAILED:
    return bSuc;
}

#if defined(WIN32_X86) // for Win32 X86
void FuncHooker::FixFuncAddress(void **ppfFunc)
{
    if (ppfFunc == NULL || *ppfFunc == NULL) {
        return;
    }

    //
    // NOTE: 
    // In the code as below:
    //     void (*pFunc) = &func;
    // the value of `pFunc' is not the address of `func', but the address of a long jump instruction.
    //
    unsigned char c = *(unsigned char *)*ppfFunc;
    int n = *(int *)((unsigned char *)*ppfFunc + 1);
    if (c == 0xE9) {                        // (0xE9 0x00000000)    jmp address
        unsigned char *nNewAddr = (unsigned char *)*ppfFunc + 5 + n;
        *ppfFunc = (void *)nNewAddr;
    }
}
#elif defined(WINCE_ARM32) // for WinCE ARM32
void FuncHooker::FixFuncAddress(void **ppfFunc)
{

}
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

#if defined(WIN32_X86) // for Win32 X86
BOOL FuncHooker::CalcTrampolineSize()
{
    BOOL bSuc = FALSE;
    unsigned int nCounter;

    LOG_LINE();

    if (m_pfTarget == NULL) {
        LOG_LINE();
        bSuc = FALSE;
        goto FAILED;
    }

    LOG_LINE();
    nCounter = 0;
    while (nCounter < 5) {
        LOG_LINE();
        void *pAddress = (void *)((unsigned int)m_pfTarget + nCounter);
        unsigned char c = *(unsigned char *)pAddress;
        unsigned short s = *(unsigned short *)pAddress;

        if (s == 0xff8b) 
            nCounter += 2;                  // (0x8BFF)             mov edi, edi
        else if (s == 0xEC8B) 
            nCounter += 2;                  // (0x8BEC)             mov ebp, esp
        else if (s == 0x25ff) 
            nCounter += 6;                  // (0xFF25)             jmp dword ptr [DS:]
        else if (c == 0xE9) 
            nCounter += 5;                  // (0xE9 0x00000000)    jmp address
        else if (c == 0x55) 
            nCounter += 1;                  // (0x55)               push ebp
        else if (c == 0xE8) 
            nCounter += 5;                  // (0xE8 0x00000000)    call address
        else if (c == 0x68) 
            nCounter += 5;                  // (0x68 0x00000000)    push address
        else if (c == 0x6a) 
            nCounter += 2;                  // (0x6A 0x00)          push onebyte 
        else if (c == 0xa1) 
            nCounter += 5;                  // (0xA1)               mov eax, 0xadress

        // {{{ added by luoshaobo
        else if (s == 0xEC8B) 
            nCounter += 2;
        else if (s == 0xEC81) 
            nCounter += 6;
        // added by luoshaobo }}}
    }
    LOG_LINE();
    nCounter += JUMP_INSTRUCTION_SIZE;
    m_nTrampolineSize = nCounter;

    bSuc = TRUE;
FAILED:
    return bSuc;
}
#elif defined(WINCE_ARM32) // for WinCE ARM32
BOOL FuncHooker::CalcTrampolineSize()
{
    BOOL bSuc = FALSE;
    unsigned int nCounter;

    LOG_LINE();

    if (m_pfTarget == NULL) {
        LOG_LINE();
        bSuc = FALSE;
        goto FAILED;
    }

    LOG_LINE();
    nCounter = JUMP_INSTRUCTION_SIZE;
    nCounter += JUMP_INSTRUCTION_SIZE;
    m_nTrampolineSize = nCounter;

    bSuc = TRUE;
FAILED:
    return bSuc;
}
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

BOOL FuncHooker::GetAllThreads(std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    BOOL bEnumEnd = FALSE;
    HANDLE hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    THREADENTRY32 te;
    DWORD nCurrentProcessID;
    HANDLE hThread;

    vecThreadInfo.clear();

    nCurrentProcessID = ::GetCurrentProcessId();

    LOG_LINE();
    hToolhelp32Snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hToolhelp32Snapshot == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    LOG_LINE();
    te.dwSize = sizeof(THREADENTRY32);
    bSuc = ::Thread32First(hToolhelp32Snapshot, &te);
    if (!bSuc) {
        if (::GetLastError() != ERROR_NO_MORE_FILES) {
            bSuc = FALSE;
            goto FAILED;
        } else {
            bEnumEnd = TRUE;
        }
    }
    while (true) {
        //LOG_LINE();
        if (bEnumEnd) {
            LOG_LINE();
            break;
        }
        
        if (te.th32OwnerProcessID == nCurrentProcessID) {
            LOG_LINE();
#if defined(WIN32_X86) // for Win32 X86
            hThread = ::OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, TRUE, te.th32ThreadID);
#elif defined(WINCE_ARM32) // for WinCE ARM32
            hThread = ::OpenThread(0, FALSE, te.th32ThreadID);
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
            if (hThread == NULL) {
                LOG_LINE();
                DWORD nError = ::GetLastError();
                bSuc = FALSE;
                goto FAILED;
            }

            LOG_LINE();
            vecThreadInfo.push_back(ThreadInfo(hThread, te.th32ThreadID));
        }
        
        //LOG_LINE();
        te.dwSize = sizeof(THREADENTRY32);
        bSuc = ::Thread32Next(hToolhelp32Snapshot, &te);
        if (!bSuc) {
            LOG_LINE();
            if (::GetLastError() != ERROR_NO_MORE_FILES) {
                LOG_LINE();
                bSuc = FALSE;
                goto FAILED;
            } else {
                LOG_LINE();
                bEnumEnd = TRUE;
            }
        }
    };

    LOG_LINE();
    bSuc = TRUE;
FAILED:
    if (hToolhelp32Snapshot != INVALID_HANDLE_VALUE) {
        ::CloseHandle(hToolhelp32Snapshot);
        hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    }
    return bSuc;
}

BOOL FuncHooker::SuspendAllThreads(const std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    DWORD nResult;
    DWORD nCurrentThreadID;

    nCurrentThreadID = ::GetCurrentThreadId();

    for (std::vector<ThreadInfo>::const_iterator it = vecThreadInfo.begin(); it != vecThreadInfo.end(); ++it) {
        if (it->nThreadID == nCurrentThreadID) {
            continue;
        }

        nResult = ::SuspendThread(it->hThread);
        if (nResult == (DWORD)-1) {
            bSuc = FALSE;
            goto FAILED;
        }
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL FuncHooker::ResumeAllThreads(const std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    DWORD nResult;
    DWORD nCurrentThreadID;

    nCurrentThreadID = ::GetCurrentThreadId();

    for (std::vector<ThreadInfo>::const_iterator it = vecThreadInfo.begin(); it != vecThreadInfo.end(); ++it) {
        if (it->nThreadID == nCurrentThreadID) {
            continue;
        }

        nResult = ::ResumeThread(it->hThread);
        if (nResult == (DWORD)-1) {
            bSuc = FALSE;
            goto FAILED;
        }
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL FuncHooker::FixThreadContextsOnHook(const std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    DWORD nCurrentThreadID;
    CONTEXT context;

    nCurrentThreadID = ::GetCurrentThreadId();

    for (std::vector<ThreadInfo>::const_iterator it = vecThreadInfo.begin(); it != vecThreadInfo.end(); ++it) {
        if (it->nThreadID == nCurrentThreadID) {
            continue;
        }

        context.ContextFlags = CONTEXT_CONTROL;
        bSuc = ::GetThreadContext(it->hThread, &context);
        if (!bSuc) {
            bSuc = FALSE;
            goto FAILED;
        }
#if defined(WIN32_X86) // for Win32 X86
        if ((DWORD)m_pfTarget <= context.Eip && context.Eip < (DWORD)m_pfTarget + JUMP_INSTRUCTION_SIZE) {
            context.Eip = (DWORD)m_pfTrampoline + context.Eip - (DWORD)m_pfTarget;
        }
#elif defined(WINCE_ARM32) // for WinCE ARM32
        if ((DWORD)m_pfTarget <= context.Pc && context.Pc < (DWORD)m_pfTarget + JUMP_INSTRUCTION_SIZE) {
            context.Pc = (DWORD)m_pfTrampoline + context.Pc - (DWORD)m_pfTarget;
        }
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
        bSuc = ::SetThreadContext(it->hThread, &context);
        if (!bSuc) {
            bSuc = FALSE;
            goto FAILED;
        }
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL FuncHooker::FixThreadContextsOnUnhook(const std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    DWORD nCurrentThreadID;
    CONTEXT context;

    nCurrentThreadID = ::GetCurrentThreadId();

    for (std::vector<ThreadInfo>::const_iterator it = vecThreadInfo.begin(); it != vecThreadInfo.end(); ++it) {
        if (it->nThreadID == nCurrentThreadID) {
            continue;
        }

        context.ContextFlags = CONTEXT_CONTROL;
        bSuc = ::GetThreadContext(it->hThread, &context);
        if (!bSuc) {
            bSuc = FALSE;
            goto FAILED;
        }
#if defined(WIN32_X86) // for Win32 X86
        if ((DWORD)m_pfTrampoline <= context.Eip && context.Eip < (DWORD)m_pfTrampoline + JUMP_INSTRUCTION_SIZE) {
            context.Eip = (DWORD)m_pfTarget + context.Eip - (DWORD)m_pfTrampoline;
        }
#elif defined(WINCE_ARM32) // for WinCE ARM32
        if ((DWORD)m_pfTrampoline <= context.Pc && context.Pc < (DWORD)m_pfTrampoline + JUMP_INSTRUCTION_SIZE) {
            context.Pc = (DWORD)m_pfTarget + context.Pc - (DWORD)m_pfTrampoline;
        }
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
        bSuc = ::SetThreadContext(it->hThread, &context);
        if (!bSuc) {
            bSuc = FALSE;
            goto FAILED;
        }
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL FuncHooker::FlushICache()
{
    BOOL bSuc = FALSE;

    bSuc = ::FlushInstructionCache(::GetCurrentProcess(), NULL, 0);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////
FuncHookerHelper::FuncHookerHelper(bool bAutoUnhookOnDestroy /*= true*/)
    : m_bHooked(false)
    , m_bAutoUnhookOnDestroy(bAutoUnhookOnDestroy)
{

}

FuncHookerHelper::~FuncHookerHelper()
{
    if (m_bAutoUnhookOnDestroy) {
        HookTransactionEnd();
    }
}

BOOL FuncHookerHelper::HookTransactionBegin()
{
    HookTransactionEnd();
    return TRUE;
}

BOOL FuncHookerHelper::AddHooker(void *pfTarget, void *pfDetour, void **ppfTrampoline)
{
    FuncHooker *pFuncHooker = new FuncHooker(pfTarget, pfDetour, ppfTrampoline);
    m_vFuncHookers.push_back(pFuncHooker);

    return TRUE;
}

BOOL FuncHookerHelper::HookTransactionCommit()
{
    BOOL bSuc = FALSE;

    if (m_bHooked) {
        return TRUE;
    }

    bSuc = FuncHooker::GetAllThreads(m_vThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    bSuc = FuncHooker::SuspendAllThreads(m_vThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    for (std::vector<FuncHooker *>::const_iterator it = m_vFuncHookers.begin(); it != m_vFuncHookers.end(); ++it) {
        FuncHooker *pFuncHooker = (*it);
        if (pFuncHooker != NULL) {
            bSuc = pFuncHooker->Hook(m_vThreadInfo);
            if (!bSuc) {
                goto FAILED;
            }
        }
    }
    m_bHooked = true;

    bSuc = FuncHooker::ResumeAllThreads(m_vThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL FuncHookerHelper::HookTransactionEnd()
{
    BOOL bSuc = FALSE;

    if (m_bHooked) {
        bSuc = FuncHooker::GetAllThreads(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }

        bSuc = FuncHooker::SuspendAllThreads(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }
    }

    for (std::vector<FuncHooker *>::const_iterator it = m_vFuncHookers.begin(); it != m_vFuncHookers.end(); ++it) {
        FuncHooker *pFuncHooker = (*it);
        if (pFuncHooker != NULL) {
            if (m_bHooked) {
                bSuc = pFuncHooker->Unhook(m_vThreadInfo);
                if (!bSuc) {
                    goto FAILED;
                }
            }
            delete pFuncHooker;
        }
    }

    if (m_bHooked) {
        bSuc = FuncHooker::ResumeAllThreads(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }
    }
    
    m_bHooked = false;
    m_vFuncHookers.clear();
    m_vThreadInfo.clear();

    bSuc = TRUE;
FAILED:
    return bSuc;
}
