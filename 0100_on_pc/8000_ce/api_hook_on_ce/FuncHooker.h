#ifndef FUNC_HOOKER_H__37842388952399235u82357832894532578
#define FUNC_HOOKER_H__37842388952399235u82357832894532578

#include <windows.h>
#include <string>
#include <vector>

class FuncHookerHelper;

class FuncHooker
{
    friend class FuncHookerHelper;

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

public:
    static BOOL GetAllThreads(std::vector<ThreadInfo> &vecThreadInfo);
    static BOOL SuspendAllThreads(const std::vector<ThreadInfo> &vecThreadInfo);
    static BOOL ResumeAllThreads(const std::vector<ThreadInfo> &vecThreadInfo);

public:
    FuncHooker(void *pfTarget, void *pfDetour, void **ppfTrampoline);
    ~FuncHooker(void);

    BOOL Hook(const std::vector<ThreadInfo> &vecThreadInfo);
    BOOL Unhook(const std::vector<ThreadInfo> &vecThreadInfo);

private:
    BOOL FixThreadContextsOnHook(const std::vector<ThreadInfo> &vecThreadInfo);
    BOOL FixThreadContextsOnUnhook(const std::vector<ThreadInfo> &vecThreadInfo);
    BOOL FlushICache();

private:
    void FixFuncAddress(void **ppfFunc);
    BOOL CalcTrampolineSize();
    BOOL CreateJumpInstruction(unsigned int nDestAddress, void *pBuf, unsigned int nBufSize);

private:
    void *m_pfTarget;
    void *m_pfDetour;
    void *m_pfTrampoline;
    void **m_ppfTrampoline;

    unsigned int m_nTrampolineSize;
};

class FuncHookerHelper
{
public:
    FuncHookerHelper(bool bAutoUnhookOnDestroy = true);
    ~FuncHookerHelper();

    BOOL HookTransactionBegin();
    BOOL AddHooker(void *pfTarget, void *pfDetour, void **ppfTrampoline);
    BOOL HookTransactionCommit();
    BOOL HookTransactionEnd();

private:
    bool m_bHooked;
    bool m_bAutoUnhookOnDestroy;
    std::vector<FuncHooker *> m_vFuncHookers;
    std::vector<FuncHooker::ThreadInfo> m_vThreadInfo;
};

#endif // #ifndef FUNC_HOOKER_H__37842388952399235u82357832894532578
