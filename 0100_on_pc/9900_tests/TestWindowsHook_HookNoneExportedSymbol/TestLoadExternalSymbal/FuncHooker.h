#ifndef FUNC_HOOKER_H__37842388952399235u82357832894532578
#define FUNC_HOOKER_H__37842388952399235u82357832894532578

#include <windows.h>
#include <string>
#include <vector>
#include <map>

class FuncHookerHelper;

class FuncHooker
{
    friend class FuncHookerHelper;

private:
    class DestProcMemMgr    // NOTE: only for allocation memory with PAGE_EXECUTE_READWRITE
    {
    private:
        struct MemBlockInfo {
            unsigned char *pMemBlockAddr;
            unsigned int nMemBlockSize;
            unsigned int nAllocatedSize;
        };

    private:
        DestProcMemMgr(HANDLE hProcess);
        ~DestProcMemMgr();

    public:
        static DestProcMemMgr *GetInstance(HANDLE hProcess);

        void *Alloc(unsigned int nSize);
        BOOL Free(void *pMemAddr);

    private:
        static std::map<HANDLE, DestProcMemMgr *> m_mapInstances;
        HANDLE m_hProcess;
        std::vector<MemBlockInfo *> m_vMemBlockInfo;
    };

public:
    struct ThreadInfo {
        ThreadInfo(HANDLE hThread, DWORD nThreadID) {
            this->hThread = hThread;
            this->nThreadID = nThreadID;
        }
        
        HANDLE hThread;
        DWORD nThreadID;
    };

private:
#if defined(WIN32_X86) // for Win32 X86
    static const int JUMP_INSTRUCTION_SIZE = 5;
#elif defined(WINCE_ARM32) // for WinCE ARM32
    static const int JUMP_INSTRUCTION_SIZE = 12;
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
    static const int MEM_BUF_SIZE_FOR_DEST = 1024;

public:
    static BOOL GetAllThreads(HANDLE hProcess, std::vector<ThreadInfo> &vecThreadInfo);
    static BOOL SuspendAllThreads(const std::vector<ThreadInfo> &vecThreadInfo);
    static BOOL ResumeAllThreads(const std::vector<ThreadInfo> &vecThreadInfo);
    static BOOL GetProcessByName(const std::tstring &sProcessName, HANDLE &hProcess, DWORD &nProcessID);
    static BOOL LoadLibraryInProcess(const std::tstring &sLibraryPathname, HANDLE hProcess, unsigned int nHookDestFuncAddr = 0);

public:
    FuncHooker(HANDLE hDestProcess, void *pfTarget, void *pfDetour, void **ppfTrampoline, bool bInDebugMode = true);
    ~FuncHooker(void);

    BOOL Hook(const std::vector<ThreadInfo> &vecThreadInfo);
    BOOL Unhook(const std::vector<ThreadInfo> &vecThreadInfo);

private:
    static void FixFuncAddress(HANDLE hProcess, void **ppfFunc);
    static BOOL FlushICache(HANDLE hProcess);

    BOOL FixThreadContextsOnHook(const std::vector<ThreadInfo> &vecThreadInfo);
    BOOL FixThreadContextsOnUnhook(const std::vector<ThreadInfo> &vecThreadInfo);
    BOOL CalcTrampolineSize();
    BOOL CreateJumpInstruction(unsigned int nDestAddress, void *pBuf, unsigned int nBufSize);
    HANDLE GetDestProcess();

private:
    DestProcMemMgr *m_pDestProcMemMgr;
    bool m_bInDebugMode;
    HANDLE m_hDestProcess;
    void *m_pfTarget;
    void *m_pfDetour;
    void *m_pfTrampoline;
    void **m_ppfTrampoline;

    unsigned int m_nTrampolineSize;
    void *m_pMemBufForDest;
};

class FuncHookerHelper
{
private:
    struct DualHookerInfo {
        void *pfTarget;
        void *pfDetour;
        void *pfTrampoline;
        void *pfTrampolineProxy;
    };

public:
    FuncHookerHelper(HANDLE hDestProcess, bool bAutoUnhookOnDestroy = true);
    ~FuncHookerHelper();

    BOOL HookTransactionBegin();
    BOOL AddHooker(void *pfTarget, void *pfDetour, void **ppfTrampoline);
    BOOL AddDualHooker(void *pfTarget, void *pfDetour, void *pfTrampolineProxy);
    BOOL HookTransactionCommit(bool bResumeThreads = true);
    BOOL ResumeThreads();
    BOOL HookTransactionEnd();

private:
    HANDLE m_hDestProcess;
    bool m_bHooked;
    bool m_bAutoUnhookOnDestroy;
    std::vector<FuncHooker *> m_vFuncHookers;
    std::vector<FuncHooker::ThreadInfo> m_vThreadInfo;
    std::vector<DualHookerInfo> m_vDualHookerInfo;
};

#endif // #ifndef FUNC_HOOKER_H__37842388952399235u82357832894532578
