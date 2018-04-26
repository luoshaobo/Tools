#ifndef NT_EVENTHANDLERIMPL_HPP
#define NT_EVENTHANDLERIMPL_HPP

#include "SMF_afx.h"
#include "SMF_BaseThread.h"
#include "SMF_BaseEventQueue.h"
#include "SMF_PlatformLock.h"
#include "APP_afx.h"
#include "NT_EventHandler.hpp"

class NT_EventHandlerImpl 
    : public NT_EventHandler
    , public SMF_ThreadCallHandler
    , public SMF_ThreadTimerHandler
{
public:
    static const unsigned int SENDING_TIMEOUT_MSEC = (1000 * 30);
    static const unsigned int RECEIVING_TIMEOUT_MSEC = (1000 * 30);

public:
    enum {
        TIMER_ID_SENDING_TIMEOUT = TIMER_ID_USER_BASE,
        TIMER_ID_RECEIVING_TIMEOUT,
    };
    enum {
        CALL_ID_ON_REMOTE_INPUT = CALL_ID_USER_BASE,
        CALL_ID_ON_LOCAL_INPUT,
        CALL_ID_REMOTE_OUTPUT,
        CALL_ID_LOCAL_OUTPUT,
    };

public:
    NT_EventHandlerImpl(size_t nIndex);

public:
    virtual ~NT_EventHandlerImpl();
    static NT_EventHandlerImpl &GetInstance(size_t nIndex);

public:
    virtual SMF_ErrorCode Reset();

public:
    SMF_BaseThread &GetEngineThread() { return m_rEngineThread; }
    SMF_BaseEventQueue &GetEngineEventQueue() { return m_rEngineEventQueue; }
    SMF_BaseEngine &GetEngine() { return m_rEngine; }

public:
    void SetIOHandler(APP_IOHandler *pIOHandler) { m_pIOHandler = pIOHandler; }
    void SetIOCallbackData(unsigned int nIOCallbackData) { m_nIOCallbackData = nIOCallbackData; }

public:
    virtual int OnRemoteInput(const std::vector<char> &arrContent);
    virtual int OnLocalInput(const std::vector<char> &arrContent);

private:
    int RemoteOutput(const std::vector<char> &arrContent);
    int LocalOutput(const std::vector<char> &arrContent);

private:
    virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize);
    virtual SMF_ErrorCode ThreadTimerProc(SMF_BaseThread &rThread, TimerId nTimerId) { return SMF_ERR_OK; }

private:
    int EngineThread_OnRemoteInput(const std::vector<char> &arrContent);
    int EngineThread_OnLocalInput(const std::vector<char> &arrContent);
    int OutputThread_RemoteOutput(const std::vector<char> &arrContent);
    int OutputThread_LocalOutput(const std::vector<char> &arrContent);

public:
    //
    // implementation interfaces
    //
    virtual SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnEntry_Type0cSession_End(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnEntry_Type0sSession_End(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnEntry_Type1cSession_End(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnEntry_Type1sSession_End(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnEntry_Type2cSession_End(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnEntry_Type2sSession_End(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnExit(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType0cSessionBegin(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType0cSessionBeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType0cSessionEnd(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType0sSessionBegin(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType0sSessionBeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType0sSessionEnd(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1cSessionBegin(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1cSessionBeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1cSessionBeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1cSessionEnd(SMF_OpArg &rOpArg);
	virtual SMF_ErrorCode OnType1cSessionReceivingOK(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1sSessionBegin(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1sSessionBeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1sSessionBeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType1sSessionEnd(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionBegin(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionEnd(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionPhase0BeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionPhase0BeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionPhase1BeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionPhase1BeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionPhase2BeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2cSessionPhase2BeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionBegin(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionEnd(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionPhase0BeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionPhase0BeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionPhase1BeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionPhase1BeginSending(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionPhase2BeginReceiving(SMF_OpArg &rOpArg);
    virtual SMF_ErrorCode OnType2sSessionPhase2BeginSending(SMF_OpArg &rOpArg);

public:
    //
    // conditions
    //
    bool bType2sSessionPhase1Finished;
    bool bType2cSessionPhase1Finished;

private:
    //
    // implementations
    //

private:
    //
    // data
    //
    static unsigned int m_nRequestSerialNo;
    SMF_BaseThread &m_rEngineThread;
    SMF_BaseEventQueue &m_rEngineEventQueue;
    SMF_BaseEngine &m_rEngine;
    APP_IOHandler *m_pIOHandler;
    SMF_BaseThread m_oOutputThread;
    APP_Package *m_pLastInputPackage;
    int m_arrLastInputPackageBuf[(APP_PACKAGE_SIZE_MAX + sizeof(int) - 1) / sizeof(int)];
    APP_Package *m_pToBeOutputPackage;
    int m_arrToBeOutputPackageBuf[(APP_PACKAGE_SIZE_MAX + sizeof(int) - 1) / sizeof(int)];
    unsigned int m_nIOCallbackData;
};

#endif // #define NT_EVENTHANDLERIMPL_HPP
