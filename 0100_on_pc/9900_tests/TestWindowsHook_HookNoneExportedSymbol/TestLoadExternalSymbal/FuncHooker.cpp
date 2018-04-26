#include "StdAfx.h"
#include <Windows.h>
#include <Tlhelp32.h>
#include "HookerWithAsmFunc.h"
#include "FuncHooker.h"

#if 0
#define LOG_MSG                 _tprintf
#define LOG_LINE()              _tprintf(TEXT("%s() [line:%d]\n"), __FUNCTION__, __LINE__)
#else
#define LOG_MSG(...)
#define LOG_LINE()
#endif

#if defined(WIN32_X86) // for Win32 X86
#define CloseToolhelp32Snapshot CloseHandle
#elif defined(WINCE_ARM32) // for WinCE ARM32
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

// NOTE: the strcut CONTEXT is in winnt.h.

////////////////////////////////////////////////////////////////////////////////////////

std::map<HANDLE, FuncHooker::DestProcMemMgr *> FuncHooker::DestProcMemMgr::m_mapInstances;

FuncHooker::DestProcMemMgr *FuncHooker::DestProcMemMgr::GetInstance(HANDLE hProcess)
{
    std::map<HANDLE, DestProcMemMgr *>::iterator it = m_mapInstances.find(hProcess);
    if (it != m_mapInstances.end()) {
        return it->second;
    } else {
        DestProcMemMgr *pDestProcMemMgr = new DestProcMemMgr(hProcess);
        if (pDestProcMemMgr == NULL) {
            return NULL;
        } else {
            m_mapInstances.insert(std::pair<HANDLE, DestProcMemMgr *>(hProcess, pDestProcMemMgr));
            return pDestProcMemMgr;
        }
    }
}

FuncHooker::DestProcMemMgr::DestProcMemMgr(HANDLE hProcess)
    : m_hProcess(hProcess)
{

}

FuncHooker::DestProcMemMgr::~DestProcMemMgr()
{

}

void *FuncHooker::DestProcMemMgr::Alloc(unsigned int nSize)
{
    unsigned char *pRet = NULL;
    MemBlockInfo *pMemBlockInfo = NULL;

    if (nSize == 0) {
        pRet = NULL;
        goto FAILED;
    }

    if (m_vMemBlockInfo.size() > 0) {
        pMemBlockInfo = m_vMemBlockInfo[m_vMemBlockInfo.size() - 1];
        if (pMemBlockInfo->nAllocatedSize + nSize <= pMemBlockInfo->nMemBlockSize) {
            // do nothing
        } else {
            pMemBlockInfo = NULL;
        }
    } else {
        pMemBlockInfo = NULL;
    }

    if (pMemBlockInfo == NULL) {
        pMemBlockInfo = new MemBlockInfo();
        if (pMemBlockInfo == NULL) {
            pRet = NULL;
            goto FAILED;
        }
        pMemBlockInfo->nMemBlockSize = 1024 * 4;
        pMemBlockInfo->pMemBlockAddr = (unsigned char *)::VirtualAllocEx(m_hProcess, NULL, pMemBlockInfo->nMemBlockSize, 
            MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (pMemBlockInfo->pMemBlockAddr == NULL) {
            pRet = NULL;
            goto FAILED;
        }
        pMemBlockInfo->nAllocatedSize = 0;
        m_vMemBlockInfo.push_back(pMemBlockInfo);
    }

    pRet = pMemBlockInfo->pMemBlockAddr + pMemBlockInfo->nAllocatedSize;
    pMemBlockInfo->nAllocatedSize += nSize;
    if ((pMemBlockInfo->nAllocatedSize % sizeof(int)) != 0) {
        pMemBlockInfo->nAllocatedSize = (pMemBlockInfo->nAllocatedSize + sizeof(int) - 1) / sizeof(int) * sizeof(int);
    }

FAILED:
    return pRet;
}

BOOL FuncHooker::DestProcMemMgr::Free(void *pMemAddr)
{
    UNUSED_VAR(pMemAddr);

    // NOTE: do nothing
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

FuncHooker::FuncHooker(HANDLE hDestProcess, void *pfTarget, void *pfDetour, void **ppfTrampoline, bool bInDebugMode)
    : m_pDestProcMemMgr(NULL)
    , m_hDestProcess(hDestProcess)
    , m_pfTarget(pfTarget)
    , m_pfDetour(pfDetour)
    , m_ppfTrampoline(ppfTrampoline)
    , m_pfTrampoline(NULL)
    , m_nTrampolineSize(0)
    , m_pMemBufForDest(NULL)
    , m_bInDebugMode(bInDebugMode)
{
    m_pDestProcMemMgr = DestProcMemMgr::GetInstance(hDestProcess);
    if (m_pDestProcMemMgr == NULL) {
        throw std::tstring(TEXT("m_pDestProcMemMgr == NULL"));
    }

    m_pMemBufForDest = new unsigned int[MEM_BUF_SIZE_FOR_DEST / 4 + 1];

    FixFuncAddress(hDestProcess, &m_pfTarget);
    FixFuncAddress(hDestProcess, &m_pfDetour);
}

FuncHooker::~FuncHooker(void)
{
    if (m_nTrampolineSize != 0 && m_pfTrampoline != NULL) {
        //Unhook();
    }

    delete [] m_pMemBufForDest;
}

HANDLE FuncHooker::GetDestProcess()
{
    if (m_hDestProcess != NULL) {
        return m_hDestProcess;
    } else {
        return ::GetCurrentProcess();
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
    m_pfTrampoline = m_pDestProcMemMgr->Alloc(m_nTrampolineSize);
    if (m_pfTrampoline == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::ReadProcessMemory(GetDestProcess(), m_pfTarget, m_pMemBufForDest, m_nTrampolineSize - JUMP_INSTRUCTION_SIZE, NULL);
    if (!bSuc) {
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(GetDestProcess(), m_pfTrampoline, m_pMemBufForDest, m_nTrampolineSize - JUMP_INSTRUCTION_SIZE, NULL);
    if (!bSuc) {
        goto FAILED;
    }

    LOG_LINE();
    nJumpDistance1 = (DWORD)m_pfDetour - (DWORD)m_pfTarget - JUMP_INSTRUCTION_SIZE;
    if (!m_bInDebugMode) {
        bSuc = ::VirtualProtectEx(GetDestProcess(), m_pfTarget, JUMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &nOldPermission);
        if (!bSuc){
            volatile DWORD nError = ::GetLastError();
            nError = nError;
            goto FAILED;
        }
    }

#if defined(WIN32_X86) // for Win32 X86
    {
        *(unsigned char *)m_pMemBufForDest = (unsigned char)0xE9;
        *(int *)((unsigned int)m_pMemBufForDest + 1) = nJumpDistance1;
        bSuc = ::WriteProcessMemory(GetDestProcess(), m_pfTarget, m_pMemBufForDest, JUMP_INSTRUCTION_SIZE, NULL);
        if (!bSuc) {
            goto FAILED;
        }
    }
#elif defined(WINCE_ARM32) // for WinCE ARM32
    {
        nJumpDistance1 = 0;
        nJumpDistance2 = 0;
        bSuc = CreateJumpInstruction((unsigned int)m_pfDetour, (void *)m_pMemBufForDest, JUMP_INSTRUCTION_SIZE);
        if (!bSuc) {
            goto FAILED;
        }
        bSuc = ::WriteProcessMemory(GetDestProcess(), m_pfTarget, m_pMemBufForDest, JUMP_INSTRUCTION_SIZE, NULL);
        if (!bSuc) {
            goto FAILED;
        }
    }
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

    if (!m_bInDebugMode) {
        ::VirtualProtectEx(GetDestProcess(), m_pfTarget, JUMP_INSTRUCTION_SIZE, nOldPermission, &nNewPermissiion);
    }

#if defined(WIN32_X86) // for Win32 X86
    {
        nJumpDistance2 = (DWORD)m_pfTarget - (DWORD)m_pfTrampoline - JUMP_INSTRUCTION_SIZE;
        *(unsigned char *)((unsigned int)m_pMemBufForDest ) = (unsigned char)0xE9;
        *(int *)((unsigned int)m_pMemBufForDest + 1) = nJumpDistance2;
        bSuc = ::WriteProcessMemory(GetDestProcess(), (void *)((unsigned int)m_pfTrampoline + m_nTrampolineSize - JUMP_INSTRUCTION_SIZE), m_pMemBufForDest, JUMP_INSTRUCTION_SIZE, NULL);
        if (!bSuc) {
            goto FAILED;
        }
    }
#elif defined(WINCE_ARM32) // for WinCE ARM32
    {
        bSuc = CreateJumpInstruction((unsigned int)m_pfTarget + JUMP_INSTRUCTION_SIZE, (void *)(m_pMemBufForDest), JUMP_INSTRUCTION_SIZE);
        if (!bSuc) {
            goto FAILED;
        }
        bSuc = ::WriteProcessMemory(GetDestProcess(), (void *)((unsigned int)m_pfTrampoline + m_nTrampolineSize - JUMP_INSTRUCTION_SIZE), m_pMemBufForDest, JUMP_INSTRUCTION_SIZE, NULL);
        if (!bSuc) {
            goto FAILED;
        }
    }
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

    LOG_LINE();
    if (m_ppfTrampoline != NULL) {
        *m_ppfTrampoline = m_pfTrampoline;
    }

    bSuc = FixThreadContextsOnHook(vecThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    LOG_LINE();
    bSuc = FlushICache(GetDestProcess());
    if (!bSuc) {
        goto FAILED;
    }

    LOG_LINE();
SUC:
    bSuc = TRUE;
    return bSuc;
FAILED:
    if (m_nTrampolineSize != 0  && m_pfTrampoline != NULL) {
        ::VirtualFreeEx(GetDestProcess(), m_pfTrampoline, m_nTrampolineSize, MEM_RELEASE);
        m_nTrampolineSize = 0;
        m_pfTrampoline = NULL;
    }
    return bSuc;
}

#if defined(WINCE_ARM32) // for WinCE ARM32
BOOL FuncHooker::CreateJumpInstruction(unsigned int nDestAddress, void *pBuf, unsigned int nBufSize)
{
    int nIndex = 0;

    if (pBuf == NULL || nBufSize < JUMP_INSTRUCTION_SIZE) {
        return FALSE;
    }

    if ((nDestAddress & 0x03) != 0) {
        return FALSE;
    }

    unsigned int *pInstructions = (unsigned int *)pBuf;
    pInstructions[nIndex++] = 0xe59ff000;            // ldr pc, [pc, #0]
    pInstructions[nIndex++] = 0xe1a00000;            // mov r0, r0
    pInstructions[nIndex++] = nDestAddress;          // dcd #n

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

    if (!m_bInDebugMode) {
        bSuc = ::VirtualProtectEx(GetDestProcess(), m_pfTarget, JUMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &nOldPermission);
        if (!bSuc) {
            goto FAILED;
        }
    }
    bSuc = ::ReadProcessMemory(GetDestProcess(), m_pfTrampoline, m_pMemBufForDest, JUMP_INSTRUCTION_SIZE, NULL);
    if (!bSuc) {
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(GetDestProcess(), m_pfTarget, m_pMemBufForDest, JUMP_INSTRUCTION_SIZE, NULL);
    if (!bSuc) {
        goto FAILED;
    }
    bSuc = m_pDestProcMemMgr->Free(m_pfTrampoline);
    if (!bSuc) {
        goto FAILED;
    }
    if (!m_bInDebugMode) {
        bSuc = ::VirtualProtectEx(GetDestProcess(), m_pfTarget, JUMP_INSTRUCTION_SIZE, nOldPermission, &nNewPermissiion);
        if (!bSuc) {
            goto FAILED;
        }
    }

    m_nTrampolineSize = 0;
    m_pfTrampoline = NULL;

    bSuc = FixThreadContextsOnUnhook(vecThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    bSuc = FlushICache(GetDestProcess());
    if (!bSuc) {
        goto FAILED;
    }

SUC:
    bSuc = TRUE;
FAILED:
    return bSuc;
}

#if defined(WIN32_X86) // for Win32 X86
void FuncHooker::FixFuncAddress(HANDLE hProcess, void **ppfFunc)
{
    BOOL bSuc;
    unsigned int aInstBuf[20];

    if (ppfFunc == NULL || *ppfFunc == NULL) {
        return;
    }

    bSuc = ::ReadProcessMemory(hProcess, *ppfFunc, aInstBuf, JUMP_INSTRUCTION_SIZE, NULL);
    if (!bSuc) {
        goto FAILED;
    }

    //
    // NOTE: 
    // In the code as below:
    //     void (*pFunc) = &func;
    // the value of `pFunc' is not the address of `func', but the address of a long jump instruction.
    //
    unsigned char c = *(unsigned char *)aInstBuf;
    int n = *(int *)((unsigned char *)aInstBuf + 1);
    if (c == 0xE9) {                        // (0xE9 0x00000000)    jmp address
        unsigned char *nNewAddr = (unsigned char *)aInstBuf + 5 + n;
        *ppfFunc = (void *)nNewAddr;
    }

FAILED:
    ;
}
#elif defined(WINCE_ARM32) // for WinCE ARM32
void FuncHooker::FixFuncAddress(HANDLE hProcess, void **ppfFunc)
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
        bSuc = ::ReadProcessMemory(GetDestProcess(), pAddress, m_pMemBufForDest, 2, NULL);
        if (!bSuc) {
            goto FAILED;
        }
        unsigned char c = *(unsigned char *)m_pMemBufForDest;
        unsigned short s = *(unsigned short *)m_pMemBufForDest;

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

BOOL FuncHooker::GetProcessByName(const std::tstring &sProcessName, HANDLE &hProcess, DWORD &nProcessID)
{
    BOOL bSuc = FALSE;
    BOOL bEnumEnd = FALSE;
    HANDLE hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    LOG_LINE();
    hToolhelp32Snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hToolhelp32Snapshot == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    LOG_LINE();
    pe.dwSize = sizeof(PROCESSENTRY32);
    bSuc = ::Process32First(hToolhelp32Snapshot, &pe);
    if (!bSuc) {
        if (::GetLastError() != ERROR_NO_MORE_FILES) {
            bSuc = FALSE;
            goto FAILED;
        } else {
            bEnumEnd = TRUE;
        }
    }
    while (true) {
        if (bEnumEnd) {
            LOG_LINE();
            break;
        }
        
        if (std::tstring(pe.szExeFile) == sProcessName) {
            hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (hProcess == NULL) {
                bSuc = FALSE;
                goto FAILED;
            }
            nProcessID = pe.th32ProcessID;
            goto SUC;
        }
        
        pe.dwSize = sizeof(PROCESSENTRY32);
        bSuc = ::Process32Next(hToolhelp32Snapshot, &pe);
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
    }

    bSuc = FALSE;
    goto FAILED;

SUC:
    LOG_LINE();
    bSuc = TRUE;
FAILED:
    if (hToolhelp32Snapshot != INVALID_HANDLE_VALUE) {
        ::CloseToolhelp32Snapshot(hToolhelp32Snapshot);
        hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    }
    return bSuc;
}

#if 1
BOOL FuncHooker::LoadLibraryInProcess(const std::tstring &sLibraryPathname, HANDLE hProcess, unsigned int nHookDestFuncAddr)
{
    BOOL bSuc = FALSE;
    void *pLoadLibraryProc;
    void *pSetEventProc;
    HookerWithAsmFunc *pHookerWithAsmFunc = NULL;
    HookerWithAsmFunc::HookExtInfo oHookExtInfo;
    HookerWithAsmFunc::HookExtInfo *pHookExtInfo = NULL;
    void *pHookFuncAddr = NULL;
    FuncHookerHelper aFuncHookerHelper(hProcess, false);
    std::vector<unsigned char> vDetourFuncContent;
    HANDLE hEvent = NULL;
    DWORD nRet;
    HANDLE hEventInDestProcess = NULL;

    if (hProcess == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    if (nHookDestFuncAddr == 0) {
        bSuc = FALSE;
        goto FAILED;
    }

    /*
     * prepare pHookExtInfo
     */
    memset(&oHookExtInfo, 0, sizeof(HookerWithAsmFunc::HookExtInfo));
    oHookExtInfo.nLoadLibraryRet = 0;
    memcpy(oHookExtInfo.pLoadLibraryArg_LibPath, (void *)sLibraryPathname.c_str(), sLibraryPathname.length() * sizeof(TCHAR));

    hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hEvent == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::DuplicateHandle(::GetCurrentProcess(), hEvent, hProcess, &hEventInDestProcess, 0, FALSE, DUPLICATE_SAME_ACCESS);
    if (hEvent == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    oHookExtInfo.hEvent = hEventInDestProcess;

#if defined(WIN32_X86) // for Win32 X86
    pLoadLibraryProc = (int (WINAPI *)(void *))::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryW");
#elif defined(WINCE_ARM32) // for WinCE ARM32
    pLoadLibraryProc = (int (WINAPI *)(void *))::GetProcAddress(::GetModuleHandle(TEXT("coredll.dll")), TEXT("LoadLibraryW"));
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
    if (pLoadLibraryProc == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    oHookExtInfo.pLoadLibrary = (int (WINAPI *)(void *))pLoadLibraryProc;

#if defined(WIN32_X86) // for Win32 X86
    pSetEventProc = (BOOL (WINAPI *)(HANDLE))::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "SetEvent");
#elif defined(WINCE_ARM32) // for WinCE ARM32
    pSetEventProc = (BOOL (WINAPI *)(HANDLE))::GetProcAddress(::GetModuleHandle(TEXT("coredll.dll")), TEXT("EventModify"));
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
    if (pSetEventProc == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    oHookExtInfo.pSetEvent = (BOOL (WINAPI *)(HANDLE))pSetEventProc;

    pHookExtInfo = (HookerWithAsmFunc::HookExtInfo *)::VirtualAllocEx(hProcess, NULL, sizeof(HookerWithAsmFunc::HookExtInfo), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (pHookExtInfo == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(hProcess, pHookExtInfo, &oHookExtInfo, sizeof(HookerWithAsmFunc::HookExtInfo), NULL);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    /*
     * prepare pHookFuncAddr
     */
    pHookerWithAsmFunc = HookerWithAsmFunc::CreateInstance();
    if (pHookerWithAsmFunc == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = pHookerWithAsmFunc->SetHookExtInfo(pHookExtInfo);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = pHookerWithAsmFunc->GetDetourFuncContent(vDetourFuncContent);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    /*
     * hook
     */
    pHookFuncAddr = ::VirtualAllocEx(hProcess, NULL, vDetourFuncContent.size(), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pHookFuncAddr == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(hProcess, pHookFuncAddr, (void *)&vDetourFuncContent[0], vDetourFuncContent.size(), NULL);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    
    bSuc = aFuncHookerHelper.HookTransactionBegin();
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = aFuncHookerHelper.AddHooker((void *)nHookDestFuncAddr, pHookFuncAddr, (void **)&oHookExtInfo.pOrigHookFunc);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = aFuncHookerHelper.HookTransactionCommit(false);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(hProcess, pHookExtInfo, &oHookExtInfo, sizeof(HookerWithAsmFunc::HookExtInfo), NULL);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = aFuncHookerHelper.ResumeThreads();
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    /*
     * unhook
     */
    nRet = ::WaitForSingleObject(hEvent, INFINITE);
    if (nRet != WAIT_OBJECT_0) {
        bSuc = FALSE;
        goto FAILED;
    }
    //bSuc = aFuncHookerHelper.HookTransactionEnd();    // NOTE: unhook will lead to unknow error!
    //if (!bSuc) {
    //    bSuc = FALSE;
    //    goto FAILED;
    //}

    bSuc = TRUE;

FAILED:
    if (hEvent != NULL) {
        ::CloseHandle(hEvent);
        hEvent = 0;
    }
    if (pHookerWithAsmFunc != NULL) {
        delete pHookerWithAsmFunc;
        pHookerWithAsmFunc = NULL;
    }
    if (pHookFuncAddr != NULL) {
        //::VirtualFreeEx(hProcess, pHookFuncAddr, LOAD_LIBRARY_IN_PROCESS_PROC_SIZE, MEM_RELEASE);
        pHookFuncAddr = NULL;
    }
    if (pHookExtInfo != NULL) {
        //::VirtualFreeEx(hProcess, pHookExtInfo, sizeof(HookExtInfo), MEM_RELEASE);
        pHookExtInfo = NULL;
    }
    return bSuc;
}
#else
#if defined(WIN32_X86) // for Win32 X86
BOOL FuncHooker::LoadLibraryInProcess(const std::tstring &sLibraryPathname, HANDLE hProcess, unsigned int)
{
    BOOL bSuc = FALSE;
    HANDLE hThread;
    LPTHREAD_START_ROUTINE pThreadProc;
    VOID *pThreadProcArg = NULL;
    size_t nSize;

    if (hProcess == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    pThreadProc = (LPTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryW");
    if (pThreadProc == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    nSize = (sLibraryPathname.length() + 1) * sizeof(TCHAR);
    pThreadProcArg = ::VirtualAllocEx(hProcess, NULL, nSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (pThreadProcArg == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(hProcess, pThreadProcArg, sLibraryPathname.c_str(), nSize, NULL);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    hThread = ::CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pThreadProcArg, 0, NULL);
    if (hThread == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    //::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);

FAILED:
    if (pThreadProcArg != NULL) {
        //::VirtualFreeEx(hProcess, pThreadProcArg, nSize, MEM_RELEASE);
        pThreadProcArg = NULL;
    }
    return bSuc;
}
#elif defined(WINCE_ARM32) // for WinCE ARM32
#if 0
typedef struct _CALLBACKINFO {
    HANDLE  hProc;      /* destination process */
    FARPROC pfn;        /* function to call in dest. process */
    PVOID   pvArg0;     /* arg0 data */
    DWORD   dwErrRtn;   /* error return value on excpetion */
} CALLBACKINFO;
typedef CALLBACKINFO *PCALLBACKINFO;
DWORD PerformCallBack4(
    PCALLBACKINFO   pcbi,
    LPVOID          p2,
    LPVOID          p3,
    LPVOID          p4
);
#endif // #if 0
BOOL FuncHooker::LoadLibraryInProcess(const std::tstring &sLibraryPathname, HANDLE hProcess, unsigned int)
{
    BOOL bSuc = FALSE;
    CALLBACKINFO cbi = { 0 };
    size_t nSize;

    if (hProcess == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    memset(&cbi, 0, sizeof(CALLBACKINFO));
    cbi.hProc = hProcess;
    cbi.pfn = ::GetProcAddress(::GetModuleHandle(TEXT("coredll.dll")), TEXT("LoadLibraryW"));
    if (cbi.pfn == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    nSize = (sLibraryPathname.length() + 1) * sizeof(TCHAR);
    cbi.pvArg0 = ::VirtualAllocEx(hProcess, NULL, nSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (cbi.pvArg0 == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }
    bSuc = ::WriteProcessMemory(hProcess, cbi.pvArg0, (LPVOID)sLibraryPathname.c_str(), nSize, NULL);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }
    DWORD nRet = ::PerformCallBack4(&cbi, NULL, NULL, NULL);
    if (nRet == 0) {
        //bSuc = FALSE;
        //goto FAILED;
    }

    bSuc = TRUE;
FAILED:
    if (cbi.pvArg0 != NULL) {
        //::VirtualFreeEx(hProcess, cbi.pvArg0, nSize, MEM_RELEASE);
        cbi.pvArg0 = NULL;
    }
    return bSuc;
}
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
#endif // #if 1

BOOL FuncHooker::GetAllThreads(HANDLE hProcess, std::vector<ThreadInfo> &vecThreadInfo)
{
    BOOL bSuc = FALSE;
    BOOL bEnumEnd = FALSE;
    HANDLE hToolhelp32Snapshot = INVALID_HANDLE_VALUE;
    THREADENTRY32 te;
    DWORD nCurrentProcessID;
    HANDLE hThread;

    vecThreadInfo.clear();

    if (hProcess == NULL) {
        nCurrentProcessID = ::GetCurrentProcessId();
    } else {
        nCurrentProcessID = ::GetProcessId(hProcess);
    }

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
#if 0                                                   // NOTE: the thread may terminate, so can't be opened.
                DWORD nError = ::GetLastError();
                bSuc = FALSE;
                goto FAILED;
#else
                goto CONTINUE;
#endif
            }

            LOG_LINE();
            vecThreadInfo.push_back(ThreadInfo(hThread, te.th32ThreadID));
        }
        
CONTINUE:
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
    }

    LOG_LINE();
    bSuc = TRUE;
FAILED:
    if (hToolhelp32Snapshot != INVALID_HANDLE_VALUE) {
        ::CloseToolhelp32Snapshot(hToolhelp32Snapshot);
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

BOOL FuncHooker::FlushICache(HANDLE hProcess)
{
    BOOL bSuc = FALSE;

    bSuc = ::FlushInstructionCache(hProcess, NULL, 0);
    if (!bSuc) {
        bSuc = FALSE;
        goto FAILED;
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////

FuncHookerHelper::FuncHookerHelper(HANDLE hDestProcess, bool bAutoUnhookOnDestroy /*= true*/)
    : m_hDestProcess(hDestProcess)
    , m_bHooked(false)
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
    FuncHooker *pFuncHooker = new FuncHooker(m_hDestProcess, pfTarget, pfDetour, ppfTrampoline);
    m_vFuncHookers.push_back(pFuncHooker);

    return TRUE;
}

BOOL FuncHookerHelper::AddDualHooker(void *pfTarget, void *pfDetour, void *pfTrampolineProxy)
{
    DualHookerInfo oDualHookerInfo;

    memset(&oDualHookerInfo, 0, sizeof(DualHookerInfo));
    oDualHookerInfo.pfTarget = pfTarget;
    oDualHookerInfo.pfDetour = pfDetour;
    oDualHookerInfo.pfTrampoline = NULL;
    oDualHookerInfo.pfTrampolineProxy = pfTrampolineProxy;
    m_vDualHookerInfo.push_back(oDualHookerInfo);

    return TRUE;
}

BOOL FuncHookerHelper::HookTransactionCommit(bool bResumeThreads)
{
    BOOL bSuc = FALSE;
    FuncHooker *pFuncHooker;

    if (m_bHooked) {
        return TRUE;
    }

    bSuc = FuncHooker::GetAllThreads(m_hDestProcess, m_vThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    bSuc = FuncHooker::SuspendAllThreads(m_vThreadInfo);
    if (!bSuc) {
        goto FAILED;
    }

    for (std::vector<FuncHooker *>::const_iterator it = m_vFuncHookers.begin(); it != m_vFuncHookers.end(); ++it) {
        pFuncHooker = (*it);
        if (pFuncHooker != NULL) {
            bSuc = pFuncHooker->Hook(m_vThreadInfo);
            if (!bSuc) {
                goto FAILED;
            }
        }
    }

    for (std::vector<DualHookerInfo>::iterator it = m_vDualHookerInfo.begin(); it != m_vDualHookerInfo.end(); ++it) {
        DualHookerInfo &rDualHookerInfo = *it;
        pFuncHooker = new FuncHooker(m_hDestProcess, rDualHookerInfo.pfTarget, rDualHookerInfo.pfDetour, &rDualHookerInfo.pfTrampoline);
        if (pFuncHooker == NULL) {
            bSuc = FALSE;
            goto FAILED;
        }
        m_vFuncHookers.push_back(pFuncHooker);
        bSuc = pFuncHooker->Hook(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }

        pFuncHooker = new FuncHooker(m_hDestProcess, rDualHookerInfo.pfTrampolineProxy, rDualHookerInfo.pfTrampoline, NULL);
        if (pFuncHooker == NULL) {
            bSuc = FALSE;
            goto FAILED;
        }
        m_vFuncHookers.push_back(pFuncHooker);
        bSuc = pFuncHooker->Hook(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }
    }

    m_bHooked = true;

    if (bResumeThreads) {
        bSuc = FuncHooker::ResumeAllThreads(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL FuncHookerHelper::ResumeThreads()
{
    BOOL bSuc = FALSE;

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
        bSuc = FuncHooker::GetAllThreads(m_hDestProcess, m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }

        bSuc = FuncHooker::SuspendAllThreads(m_vThreadInfo);
        if (!bSuc) {
            goto FAILED;
        }
    }

    for (std::vector<FuncHooker *>::reverse_iterator it = m_vFuncHookers.rbegin(); it != m_vFuncHookers.rend(); ++it) {
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
    m_vDualHookerInfo.clear();

    bSuc = TRUE;
FAILED:
    return bSuc;
}
