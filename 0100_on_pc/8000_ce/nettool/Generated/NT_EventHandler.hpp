#ifndef NT_EVENTHANDLER_HPP
#define NT_EVENTHANDLER_HPP

#include "SMF_afx.h"
#include "SMF_BaseEngine.h"
#include "SMF_BaseState.h"
#include "SMF_BaseEventHandler.h"

class NT_EventHandlerImpl;

class NT_EventHandler : public SMF_BaseEventHandler
{
protected:
    NT_EventHandler(size_t nIndex);

public:
    virtual ~NT_EventHandler();
    static NT_EventHandlerImpl &GetInstance(size_t nIndex);

public:
    virtual SMF_ErrorCode Reset() { return SMF_BaseEventHandler::Reset(); }

public:
    //
    // Event handlers
    //
    virtual SMF_ErrorCode OnEntry(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry_Type0cSession_End(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry_Type0sSession_End(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry_Type1cSession_End(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry_Type1sSession_End(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry_Type2cSession_End(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnEntry_Type2sSession_End(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnExit(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType0cSessionBegin(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType0cSessionBeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType0cSessionEnd(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType0sSessionBegin(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType0sSessionBeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType0sSessionEnd(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1cSessionBegin(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1cSessionBeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1cSessionBeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1cSessionEnd(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1cSessionReceivingOK(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1sSessionBegin(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1sSessionBeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1sSessionBeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType1sSessionEnd(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionBegin(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionEnd(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionPhase0BeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionPhase0BeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionPhase1BeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionPhase1BeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionPhase2BeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2cSessionPhase2BeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionBegin(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionEnd(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionPhase0BeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionPhase0BeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionPhase1BeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionPhase1BeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionPhase2BeginReceiving(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
    virtual SMF_ErrorCode OnType2sSessionPhase2BeginSending(SMF_OpArg &rOpArg) { return SMF_ERR_OK; }
};

#endif // #define NT_EVENTHANDLER_HPP
