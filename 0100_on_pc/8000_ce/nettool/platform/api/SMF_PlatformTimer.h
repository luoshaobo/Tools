#ifndef SMF_PLATFORM_TIMER_H__78358920852923589233489345238578345834785348583485238
#define SMF_PLATFORM_TIMER_H__78358920852923589233489345238578345834785348583485238

#include "SMF_afx.h"

class SMF_PlatformTimerImpl;
class SMF_PlatformThread;
class SMF_PlatformTimer;

interface SMF_PlatformTimerHandler : public SMF_BaseInterface
{
    typedef unsigned int TimerId;

    virtual SMF_ErrorCode PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId) = 0;
};

class SMF_PlatformTimer : private SMF_NonCopyable, public SMF_PlatformTimerHandler
{
public:
    enum {
        PLATFORM_TIMER_ID_BASE = 0,
        PLATFORM_TIMER_ID_INVALID = SMF_INFINITE
    };

public:
    SMF_PlatformTimer(const char *pName = NULL);
    virtual ~SMF_PlatformTimer();

public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode StartTimer(TimerId nTimerId, unsigned int nInterval, bool bRepeat = false);
    SMF_ErrorCode StopTimer(TimerId nTimerId);

public:
    SMF_ErrorCode SetTimerHandler(SMF_PlatformTimerHandler *pTimerHandler);

public:
    SMF_PlatformThread *GetConnectedThread();

protected:
    virtual SMF_ErrorCode PlatformTimerProc(SMF_PlatformTimer &rPlatformTimer, TimerId nTimerId);

public:
    SMF_PlatformTimerImpl *m_pImpl;
};

#endif // #ifndef SMF_PLATFORM_TIMER_H__78358920852923589233489345238578345834785348583485238
