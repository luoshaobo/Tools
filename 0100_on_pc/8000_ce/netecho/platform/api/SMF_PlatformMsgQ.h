#ifndef SMF_PLATFORM_MSGQ_H__738402358932957842858940603052088934589345934894896489
#define SMF_PLATFORM_MSGQ_H__738402358932957842858940603052088934589345934894896489

#include "SMF_afx.h"

class SMF_PlatformMsgQImpl;
class SMF_PlatformThread;
class SMF_PlatformMsgQ;
class SMF_PlatformEvent;

interface SMF_PlatformMsgQHandler : public SMF_BaseInterface
{
    typedef unsigned int MsgId;
#ifdef WIN32
    typedef WPARAM WParam;
#else
    typedef unsigned int WParam;
#endif // #ifdef WIN32
#ifdef WIN32
    typedef LPARAM LParam;
#else
    typedef void *LParam;
#endif // #ifdef WIN32

    enum {
        PLATFORM_MSG_ID_QUIT = 1,
        PLATFORM_MSG_ID_TIMER,
        PLATFORM_MSG_ID_USER_BASE = 256,
    };

    struct Msg {
        Msg() : nMsgId(0), wParam(0), lParam(0), bSystemMsg(false) {}
        Msg(MsgId a_nMsgId, WParam a_wParam, LParam a_lParam, bool a_bSystemMsg = false)
            : nMsgId(a_nMsgId), wParam(a_wParam), lParam(a_lParam), bSystemMsg(a_bSystemMsg) {}
        Msg(const Msg &rOther) : nMsgId(rOther.nMsgId), wParam(rOther.wParam), lParam(rOther.lParam), bSystemMsg(rOther.bSystemMsg) {}

        MsgId nMsgId;
        WParam wParam;
        LParam lParam;
        bool bSystemMsg;
    };

    virtual SMF_ErrorCode PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ) = 0;
    virtual SMF_ErrorCode PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ) = 0;
    virtual SMF_ErrorCode PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, Msg &rMsg) = 0;
};

class SMF_PlatformMsgQ : private SMF_NonCopyable, public SMF_PlatformMsgQHandler
{
public:
    SMF_PlatformMsgQ(const char *pName = NULL);
    virtual ~SMF_PlatformMsgQ();

public:
    SMF_ErrorCode ConnectToThread(SMF_PlatformThread *pThread);

    SMF_ErrorCode PostMsg(const Msg &rMsg);
    SMF_ErrorCode SendMsg(const Msg &rMsg);
    SMF_ErrorCode SendMsg(const Msg &rMsg, SMF_PlatformEvent *pWaitedEvent);

    SMF_ErrorCode EnterMsgLoop();
    SMF_ErrorCode ExitMsgLoop();

    SMF_ErrorCode PostQuitMsg();

public:
    SMF_ErrorCode SetMsgQHandler(SMF_PlatformMsgQHandler *pMsgQHandler);

public:
    SMF_PlatformThread *GetConnectedThread();

protected:
    virtual SMF_ErrorCode PlatformMsgQPreGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQPostGetMsg(SMF_PlatformMsgQ &rPlatformMsgQ);
    virtual SMF_ErrorCode PlatformMsgQProc(SMF_PlatformMsgQ &rPlatformMsgQ, Msg &rMsg);

public:
    SMF_PlatformMsgQImpl *m_pImpl;
};

#endif // #ifndef SMF_PLATFORM_MSGQ_H__738402358932957842858940603052088934589345934894896489
