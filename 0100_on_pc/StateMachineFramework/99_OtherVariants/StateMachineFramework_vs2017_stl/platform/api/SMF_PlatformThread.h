#ifndef SMF_PLATFORM_THREAD_H__378284923578924723785214823762392423284764238473478237
#define SMF_PLATFORM_THREAD_H__378284923578924723785214823762392423284764238473478237

#include <thread>
#include "SMF_afx.h"

class SMF_PlatformThreadImpl;
class SMF_PlatformThread;
class SMF_PlatformMsgQ;
class SMF_PlatformTimer;

interface SMF_PlatformThreadHandler : public SMF_BaseInterface
{
    virtual SMF_ErrorCode PlatformThreadProc(SMF_PlatformThread &rPlatformThread) = 0;
};

class SMF_PlatformThread : private SMF_NonCopyable, public SMF_PlatformThreadHandler
{
public:
#if defined(CYGWIN32)
    typedef void *ThreadId;
#elif defined(OSX64)
    typedef void *ThreadId;
#elif defined(LINUX32)
    typedef unsigned long ThreadId;
#else
    //typedef unsigned long ThreadId;
    typedef std::thread::id ThreadId;
#endif

public:
    SMF_PlatformThread(const char *pName = NULL);
    virtual ~SMF_PlatformThread();

public:
    SMF_ErrorCode Start();
    SMF_ErrorCode Stop();

public:
    SMF_ErrorCode SetThreadHandler(SMF_PlatformThreadHandler *pThreadHandler);
    SMF_ErrorCode SetConnectedMsgQ(SMF_PlatformMsgQ *pMsgQ);
    SMF_ErrorCode SetConnectedTimer(SMF_PlatformTimer *pTimer);

public:
    SMF_PlatformMsgQ *GetConnectedMsgQ();
    SMF_PlatformTimer *GetConnectedTimer();

public:
    static ThreadId GetCurrentThreadId();
    static bool IsThreadEqual(ThreadId nThreadId1, ThreadId nThreadId2);
    ThreadId GetThreadId();

protected:
    virtual SMF_ErrorCode PlatformThreadProc(SMF_PlatformThread &rPlatformThread);

public:
    SMF_PlatformThreadImpl *m_pImpl;
};

#endif // #ifndef SMF_PLATFORM_THREAD_H__378284923578924723785214823762392423284764238473478237
