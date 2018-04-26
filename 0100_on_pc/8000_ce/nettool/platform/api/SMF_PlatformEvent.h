#ifndef SMF_PLATFORM_EVENT_H__758912789239578238534967823925378583457238578345995
#define SMF_PLATFORM_EVENT_H__758912789239578238534967823925378583457238578345995

#include "SMF_afx.h"

class SMF_PlatformEventImpl;

class SMF_PlatformEvent : private SMF_NonCopyable
{
public:
    typedef unsigned int WaitResult;
    enum {
        WAIT_RESULT_FAILED,
        WAIT_RESULT_TIMEOUT,
        WAIT_RESULT_OBJECT_0,
    };

public:
    SMF_PlatformEvent(bool bManualReset, bool bInitSignaled, const char *pName = NULL);
    virtual ~SMF_PlatformEvent();

public:
    SMF_ErrorCode SetEvent();
    SMF_ErrorCode ResetEvent();
    static SMF_ErrorCode WaitForSingleEvent(WaitResult &nWaitResult, SMF_PlatformEvent *pEvent, unsigned int nTimeout = SMF_INFINITE);

private:
    // NOTE: to be removed!
    static SMF_ErrorCode WaitForMultipleEvents(WaitResult &nWaitResult, SMF_PlatformEvent **ppEvents, unsigned int nEventCount, bool bWaitAll = false, unsigned int nTimeout = SMF_INFINITE);

public:
    SMF_PlatformEventImpl *m_pImpl;
};

#endif // #ifndef SMF_PLATFORM_EVENT_H__758912789239578238534967823925378583457238578345995
