#include "NT_afx.hpp"
#include "NT_EventHandler.hpp"
#include "NT_EventHandlerImpl.hpp"

using namespace NT;

unsigned int NT_EventHandlerImpl::m_nRequestSerialNo = 0;

NT_EventHandlerImpl::NT_EventHandlerImpl(size_t nIndex)
    : NT_EventHandler(nIndex)
    , m_rEngineThread(*new SMF_BaseThread("EngineThread"))
    , m_rEngineEventQueue(*new SMF_BaseEventQueue("EngineEventQueue"))
    , m_rEngine(SMF_GET_ENGINE(MainEngine, nIndex))
    , m_pIOHandler(NULL)
    , m_oOutputThread("OutputThread")
    , m_pLastInputPackage((APP_Package *)m_arrLastInputPackageBuf)
    , m_pToBeOutputPackage((APP_Package *)m_arrToBeOutputPackageBuf)
    , m_nIOCallbackData(0)
{
    memset(m_arrLastInputPackageBuf, 0, sizeof(m_arrLastInputPackageBuf));
    memset(m_arrToBeOutputPackageBuf, 0, sizeof(m_arrToBeOutputPackageBuf));

    m_rEngineThread.SetCallHandler(this);
    m_rEngineThread.Start();

    m_oOutputThread.SetCallHandler(this);
    m_oOutputThread.Start();

    Reset();
}

NT_EventHandlerImpl::~NT_EventHandlerImpl()
{
    delete &m_rEngineThread;
    delete &m_rEngineEventQueue;
}

class NT_EventHandleFactory
{
public:
    static NT_EventHandlerImpl *NewObject(size_t nIndex) {
        return new NT_EventHandlerImpl(nIndex);
    }
    static void DeleteObject(NT_EventHandlerImpl *pObject) {
        delete pObject;
    }
};

NT_EventHandlerImpl &NT_EventHandlerImpl::GetInstance(size_t nIndex)
{
    static SMF_AutoReleaseObjectPool<NT_EventHandlerImpl, NT_INSTANCE_COUNT, NT_EventHandleFactory> arop;
    return arop.At(nIndex);
}

SMF_ErrorCode NT_EventHandlerImpl::Reset()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = NT_EventHandler::Reset();
    }

    if (nErrorCode == SMF_ERR_OK) {
        bType2sSessionPhase1Finished = false;
        bType2cSessionPhase1Finished = false;
    }

    return nErrorCode;
}

int NT_EventHandlerImpl::OnRemoteInput(const std::vector<char> &arrContent)
{
    m_rEngineThread.AsyncCall(CALL_ID_ON_REMOTE_INPUT, (void *)&arrContent[0], arrContent.size());

    return arrContent.size();
}

int NT_EventHandlerImpl::EngineThread_OnRemoteInput(const std::vector<char> &arrContent)
{
    int nRet = 0;
    APP_Package *pPackage = NULL;

    if (nRet == 0) {
        if (arrContent.size() < sizeof(APP_PackageHeader)) {
            nRet = -1;
        } else {
            pPackage = (APP_Package *)&arrContent[0];
        }
    }

    if (nRet == 0) {
        if (pPackage->header.nMagic != APP_PACKAGE_MAGIC) {
            nRet = -1;
        } else if (pPackage->header.nPackageSize > APP_PACKAGE_SIZE_MAX || pPackage->header.nPackageSize != arrContent.size()) {
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        memcpy(&m_arrLastInputPackageBuf, (void *)&arrContent[0], arrContent.size());

        switch (pPackage->header.nCommand) {
        case APP_CMD_ECHO_REQUEST:
            {
                m_rEngine.PostEvent(SMF_D_EVENT_ID(Type1sSessionBegin));
            }
            break;
        case APP_CMD_ECHO_RESPONSE:
            {
                m_rEngine.PostEvent(SMF_D_EVENT_ID(ReceivingOK));
            }
            break;
        default:
            break;
        }
    }

    if (nRet != 0) {
        m_rEngine.PostEvent(SMF_D_EVENT_ID(ReceivingFailed));
    }

    return nRet;
}

int NT_EventHandlerImpl::OutputThread_RemoteOutput(const std::vector<char> &arrContent)
{
    int nOutputted = -1;

    if (m_pIOHandler != NULL) {
        nOutputted = m_pIOHandler->RemoteOutput(arrContent, m_nIOCallbackData);
    }
    m_rEngine.StopTimer(TIMER_ID_SENDING_TIMEOUT);
    if (nOutputted == arrContent.size()) {
        m_rEngine.PostEvent(SMF_D_EVENT_ID(SendingOK));
    } else {
        m_rEngine.PostEvent(SMF_D_EVENT_ID(SendingFailed));
    }

    return nOutputted;
}

int NT_EventHandlerImpl::OutputThread_LocalOutput(const std::vector<char> &arrContent)
{
    int nOutputted = -1;

    if (m_pIOHandler != NULL) {
        nOutputted = m_pIOHandler->LocalOutput(arrContent, m_nIOCallbackData);
    }

    return nOutputted;
}

int NT_EventHandlerImpl::OnLocalInput(const std::vector<char> &arrContent)
{
    bool bUnknownLocalInput = true;
    std::string sContent(&arrContent[0], &arrContent[0] + arrContent.size());

    sContent = TK_Tools::TrimLeft(sContent);
    if (TK_Tools::CompareCaseRightLen(sContent, "!echo ") == 0) {
        sContent = sContent.substr(std::string("!echo ").length());
        sContent = TK_Tools::TrimLeft(sContent);
        memset(&m_arrToBeOutputPackageBuf, 0, sizeof(m_arrToBeOutputPackageBuf));
        m_pToBeOutputPackage->header.nMagic = APP_PACKAGE_MAGIC;
        m_pToBeOutputPackage->header.nPackageSize = sizeof(APP_PackageHeader) + sizeof(APP_Package::APP_PackageBody::BodyEchoReq) + sContent.length() - 1;
        m_pToBeOutputPackage->header.nCheckSum = 0;
        m_pToBeOutputPackage->header.nCommand = APP_CMD_ECHO_REQUEST;
        m_pToBeOutputPackage->header.nStatus = 0;
        m_pToBeOutputPackage->header.nRequestSerialNo = m_nRequestSerialNo++;
        m_pToBeOutputPackage->body.stBodyEchoReq.nContentLen = sContent.length();
        memcpy(m_pToBeOutputPackage->body.stBodyEchoReq.arrContent, sContent.c_str(), sContent.length());

        bUnknownLocalInput = false;
    }

    if (!bUnknownLocalInput) {
        m_rEngineThread.AsyncCall(CALL_ID_ON_LOCAL_INPUT, (void *)m_pToBeOutputPackage, m_pToBeOutputPackage->header.nPackageSize);
    } else {
        fprintf(stderr, "*** Error: unkonwn input command!\n");
    }

    return arrContent.size();
}

int NT_EventHandlerImpl::EngineThread_OnLocalInput(const std::vector<char> &arrContent)
{
    int nRet = 0;
    APP_Package *pPackage = NULL;

    if (nRet == 0) {
        if (arrContent.size() < sizeof(APP_PackageHeader)) {
            nRet = -1;
        } else {
            pPackage = (APP_Package *)&arrContent[0];
        }
    }

    if (nRet == 0) {
        if (pPackage->header.nMagic != APP_PACKAGE_MAGIC) {
            nRet = -1;
        } else if (pPackage->header.nPackageSize > APP_PACKAGE_SIZE_MAX || pPackage->header.nPackageSize != arrContent.size()) {
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        switch (pPackage->header.nCommand) {
        case APP_CMD_ECHO_REQUEST:
            {
                m_rEngine.PostEvent(SMF_D_EVENT_ID(Type1cSessionBegin));
                RemoteOutput(arrContent);
            }
            break;
        default:
            break;
        }
    }

    if (nRet != 0) {
        m_rEngine.PostEvent(SMF_D_EVENT_ID(SendingFailed));
    }

    return nRet;
}

int NT_EventHandlerImpl::RemoteOutput(const std::vector<char> &arrContent)
{
    m_oOutputThread.AsyncCall(CALL_ID_REMOTE_OUTPUT, (void *)&arrContent[0], arrContent.size());
    m_rEngine.StartTimer(TIMER_ID_SENDING_TIMEOUT, SENDING_TIMEOUT_MSEC, false, SMF_D_EVENT_ID(SendingTimeout));

    return 0;
}

int NT_EventHandlerImpl::LocalOutput(const std::vector<char> &arrContent)
{
    m_oOutputThread.AsyncCall(CALL_ID_LOCAL_OUTPUT, (void *)&arrContent[0], arrContent.size());

    return 0;
}

SMF_ErrorCode NT_EventHandlerImpl::ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, void *pArgs, unsigned int nArgsSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (&rThread == &m_oOutputThread) {
        switch (nCallId) {
        case CALL_ID_REMOTE_OUTPUT:
            {
                std::vector<char> arrContent((char *)pArgs, (char *)pArgs + nArgsSize);
                OutputThread_RemoteOutput(arrContent);
            }
            break;
        case CALL_ID_LOCAL_OUTPUT:
            {
                std::vector<char> arrContent((char *)pArgs, (char *)pArgs + nArgsSize);
                OutputThread_LocalOutput(arrContent);
            }
            break;
        default:
            break;
        }
    } else if (&rThread == &m_rEngineThread) {
        switch (nCallId) {
        case CALL_ID_ON_REMOTE_INPUT:
            {
                std::vector<char> arrContent((char *)pArgs, (char *)pArgs + nArgsSize);
                EngineThread_OnRemoteInput(arrContent);
            }
            break;
        case CALL_ID_ON_LOCAL_INPUT:
            {
                std::vector<char> arrContent((char *)pArgs, (char *)pArgs + nArgsSize);
                EngineThread_OnLocalInput(arrContent);
            }
            break;
        default:
            break;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry_Type0cSession_End(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry_Type0sSession_End(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry_Type1cSession_End(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_rEngine.StopTimer(TIMER_ID_SENDING_TIMEOUT);
        m_rEngine.StopTimer(TIMER_ID_RECEIVING_TIMEOUT);
        nErrorCode = m_rEngine.PostEvent(SMF_D_EVENT_ID(Type1cSessionEnd));
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry_Type1sSession_End(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        m_rEngine.StopTimer(TIMER_ID_SENDING_TIMEOUT);
        m_rEngine.StopTimer(TIMER_ID_RECEIVING_TIMEOUT);
        nErrorCode = m_rEngine.PostEvent(SMF_D_EVENT_ID(Type1sSessionEnd));
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry_Type2cSession_End(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnEntry_Type2sSession_End(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnExit(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType0cSessionBegin(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType0cSessionBeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType0cSessionEnd(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType0sSessionBegin(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType0sSessionBeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType0sSessionEnd(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1cSessionBegin(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1cSessionBeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_rEngine.StartTimer(TIMER_ID_RECEIVING_TIMEOUT, RECEIVING_TIMEOUT_MSEC, false, SMF_D_EVENT_ID(ReceivingTimeout));
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1cSessionBeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1cSessionEnd(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1cSessionReceivingOK(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        LocalOutput(std::vector<char>((char *)m_pLastInputPackage->body.stBodyEchoRes.arrContent, 
            (char *)m_pLastInputPackage->body.stBodyEchoRes.arrContent + m_pLastInputPackage->body.stBodyEchoRes.nContentLen));
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1sSessionBegin(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1sSessionBeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    
    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_rEngine.PostEvent(SMF_D_EVENT_ID(ReceivingOK));
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1sSessionBeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        memcpy(m_arrToBeOutputPackageBuf, m_arrLastInputPackageBuf, sizeof(m_arrToBeOutputPackageBuf));
        switch (m_pLastInputPackage->header.nCommand) {
        case APP_CMD_ECHO_REQUEST:
            {
                LocalOutput(std::vector<char>((char *)m_pLastInputPackage->body.stBodyEchoReq.arrContent, 
                    (char *)m_pLastInputPackage->body.stBodyEchoReq.arrContent + m_pLastInputPackage->body.stBodyEchoReq.nContentLen));
                m_pToBeOutputPackage->header.nCommand = APP_CMD_ECHO_RESPONSE;
                RemoteOutput(std::vector<char>((char *)m_pToBeOutputPackage, (char *)m_pToBeOutputPackage + m_pToBeOutputPackage->header.nPackageSize));
            }
            break;
        default:
            break;
        }
    }

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType1sSessionEnd(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionBegin(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionEnd(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionPhase0BeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionPhase0BeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionPhase1BeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionPhase1BeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionPhase2BeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2cSessionPhase2BeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionBegin(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionEnd(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionPhase0BeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionPhase0BeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionPhase1BeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionPhase1BeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionPhase2BeginReceiving(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode NT_EventHandlerImpl::OnType2sSessionPhase2BeginSending(SMF_OpArg &rOpArg)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}
