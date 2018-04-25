// IPCSender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include "SMF_afx.h"
#include "SMF_PostOffice.h"
#include "SMF_BaseRpc.h"
#include "IpcDef.h"
#include "RpcDef.h"

SMF_BasePostOffice &GetPostOffice()
{
    static SMF_BasePostOffice &rPostOffice = SMF_BasePostOffice::GetInstance();

    return rPostOffice;
}

SMF_PublisherHandler *pPublisher = NULL;

void InitIPC()
{
    SMF_BasePostOffice::MailId nPublishedMailIds[] = {
        MAIL_ID_PUB2_MAIL001,
        MAIL_ID_PUB2_MAIL002,
        MAIL_ID_PUB2_MAIL003,
    };
    GetPostOffice().RegisterMails(PUB_ID_PUB2, SUB_ID_SUB1, nPublishedMailIds, SMF_ARR_SIZE(nPublishedMailIds));
    GetPostOffice().GetPublisher(PUB_ID_PUB2, &pPublisher);
}

void PostMail(SMF_BasePostOffice::MailId nMailId, void *pData, unsigned int nDataSize)
{
    if (pPublisher != NULL) {
        SMF_BasePostOffice::MailData oMailData(pData, nDataSize);
        pPublisher->PostMail(nMailId, oMailData);
    }
}

void help()
{
    TK_MSG("Commands:\n");
    TK_MSG("  help|h|?: print the help information.\n");
    TK_MSG("  exit: exit the state machine.\n");
    TK_MSG("  mail1: PostMail 1.\n");
    TK_MSG("  mail2: PostMail 2.\n");
    TK_MSG("  mail3: PostMail 3.\n");
    TK_MSG("  remote_onoff <nOnOff>: set to send to remote or local.\n");
    TK_MSG("  rtc11: remote/local thread call 11: async call 1.\n");
    TK_MSG("  rtc12: remote/local thread call 12: async call 2.\n");
    TK_MSG("  rtc21: remote/local thread call 21: sync call 1.\n");
    TK_MSG("  rtc22: remote/local thread call 22: sync call 2.\n");
    TK_MSG("  pre <nEventId>: post remote/local event <nEventId>.\n");
    TK_MSG("  sre <nEventId>: send remote/local event <nEventId>.\n");
    TK_MSG("  pre2 <nEventId>: post remote event <nEventId> to state machine.\n");
    TK_MSG("  sre2 <nEventId>: send remote event <nEventId> to state machine.\n");
    TK_MSG("\n");
}

int InitSock()
{
#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("*** Error at WSAStartup()\n");
        return -1;
    }
#endif // #ifdef WIN32

    return 0;
}

class RpcHost : public SMF_BaseRpc
{
private:
    virtual SMF_ErrorCode OnRpcCall(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;

        switch (nCallId)
        {
        case RPC_HOST_001_IOCTL_001:
            {
                {
                    std::string sInBuf(reinterpret_cast<char *>(rRpcCallArgs.pInBuf), rRpcCallArgs.nInBufSize);
                    SMF_LOG_MSG("RPC input: nInBufSize=%u, pInBuf=\"%s\"\n", rRpcCallArgs.nInBufSize, sInBuf.c_str());
                }
                if (rRpcCallArgs.pOutBuf != NULL && rRpcCallArgs.nOutBufSize >= 4) {
                    rRpcCallArgs.pOutBuf[0] = 'O';
                    rRpcCallArgs.pOutBuf[1] = 'K';
                    rRpcCallArgs.pOutBuf[2] = '\0';
                    rRpcCallArgs.nOutBufRetSize = 3;
                    rRpcCallArgs.nRet = 0;
                } else {
                    rRpcCallArgs.nOutBufRetSize = 0;
                    rRpcCallArgs.nRet = -1;
                }
            }
            break;
        case RPC_HOST_001_IOCTL_002:
            {
                rRpcCallArgs.nOutBufRetSize = rRpcCallArgs.nOutBufSize;
                rRpcCallArgs.nRet = 0;
            }
            break;
        default:
            break;
        }

        return nErrorCode;
    }
};

void InitRPC()
{
    static RpcHost oRpc;

    oRpc.BindRpcHostId(RPC_HOST_001);
    oRpc.StartRpcHost();
}

void TryDeleteTempObjectFiles()
{
#ifndef WIN32
    {
        std::string sCmdLine = TK_Tools::FormatStr("mkdir -p %s", SMF_TEMP_KEY_DIR_ROOT);
        system(sCmdLine.c_str());
    }

    {
        std::string sCmdLine = TK_Tools::FormatStr("mkdir -p %s", SMF_TEMP_DIR_ROOT);
        system(sCmdLine.c_str());
    }

#if 0
   if (!TK_Tools::ProcessExists("IPCSender")) {
       SMF_LOG_MSG("*** A process \"IPCSender\" exists already! Exiting...\n");
       exit(0);
   }
#endif // #if 0

    if (!TK_Tools::ProcessExists("StateMachineFramework")) {
        std::string sCmdLine;
        SMF_LOG_MSG("The process \"StateMachineFramework\" does not exist.\n");
        sCmdLine = TK_Tools::FormatStr("rm -f %s/*", SMF_TEMP_KEY_DIR_ROOT);
        system(sCmdLine.c_str());
        sCmdLine = TK_Tools::FormatStr("rm -f %s/*", SMF_TEMP_DIR_ROOT);
        system(sCmdLine.c_str());
    } else {
        SMF_LOG_MSG("The process \"StateMachineFramework\" exists.\n");
    }
#endif // #ifndef WIN32
}

interface RemoteCallAndEventDefinitons
{
    enum {
        CALL_ID_Call001 = SMF_ThreadCallHandler::CALL_ID_USER_BASE,
        CALL_ID_Call002,
        CALL_ID_Call003,
        CALL_ID_Call004,
    };

    struct CallArgs {
        union InputArgs {
            struct caCall001 {
                int nArg1;
            } stCall001;
            struct caCall002{
                int nArg1;
                char arrArg2[40];
            } stCall002;
            struct caCall003 {
                int nArg1;
                char arrArg2[40];
                long nArg3;
            } stCall003;
            struct caCall004 {
                int nArg1;
                char arrArg2[40];
                long nArg3;
                short nArg4;
            } stCall004;
        } unInputArgs;
        union OutputArgs {
            struct caCall001 {
                char arrResult[30];
            } stCall001;
            struct caCall002{
                char arrResult[30];
            } stCall002;
            struct caCall003 {
                char arrResult[30];
            } stCall003;
            struct caCall004 {
                char arrResult[30];
            } stCall004;
        } unOutputArgs;
    };
};

class LocalCallAndEventThread : public RemoteCallAndEventDefinitons, public SMF_BaseThread
{
public:
    LocalCallAndEventThread(const std::string &sThreadName) : SMF_BaseThread(sThreadName, true) {}

    virtual SMF_ErrorCode ThreadCallProc(SMF_BaseThread &rThread, CallId nCallId, ThreadCallArgs &rThreadCallArgs)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;

        switch (nCallId) {
        case CALL_ID_Call001:
            {
                printf("%s(): on CALL_ID_Call001\n", __FUNCTION__);

                rThreadCallArgs.nOutBufRetSize = 0;

                if (rThreadCallArgs.pInBuf != NULL && rThreadCallArgs.nInBufSize == sizeof(CallArgs::InputArgs::caCall001)) {
                    CallArgs::InputArgs::caCall001 *pInputArgsCall001 = reinterpret_cast<CallArgs::InputArgs::caCall001 *>(rThreadCallArgs.pInBuf);
                    printf("%s(): arg1=%d\n", __FUNCTION__, pInputArgsCall001->nArg1);
                }

                if (rThreadCallArgs.pOutBuf != NULL && rThreadCallArgs.nOutBufSize == sizeof(CallArgs::OutputArgs::caCall001)) {
                    CallArgs::OutputArgs::caCall001 *pOutputArgsCall001 = reinterpret_cast<CallArgs::OutputArgs::caCall001 *>(rThreadCallArgs.pOutBuf);
                    strncpy(pOutputArgsCall001->arrResult, "call001 returned data", sizeof(pOutputArgsCall001->arrResult));
                    rThreadCallArgs.nOutBufRetSize = rThreadCallArgs.nOutBufSize;

                    std::string sTmp(pOutputArgsCall001->arrResult, rThreadCallArgs.nOutBufRetSize);
                    printf("%s(): %s\n", __FUNCTION__, sTmp.c_str());
                }

                rThreadCallArgs.nRet = 0;
            }
            break;

        case CALL_ID_Call002:
            {
                printf("%s(): on CALL_ID_Call002\n", __FUNCTION__);

                rThreadCallArgs.nOutBufRetSize = 0;

                if (rThreadCallArgs.pInBuf != NULL && rThreadCallArgs.nInBufSize == sizeof(CallArgs::InputArgs::caCall002)) {
                    CallArgs::InputArgs::caCall002 *pInputArgsCall002 = reinterpret_cast<CallArgs::InputArgs::caCall002 *>(rThreadCallArgs.pInBuf);
                    printf("%s(): arg1=%d; arg2=%s\n", __FUNCTION__, pInputArgsCall002->nArg1, pInputArgsCall002->arrArg2);
                }

                if (rThreadCallArgs.pOutBuf != NULL && rThreadCallArgs.nOutBufSize == sizeof(CallArgs::OutputArgs::caCall002)) {
                    CallArgs::OutputArgs::caCall002 *pOutputArgsCall002 = reinterpret_cast<CallArgs::OutputArgs::caCall002 *>(rThreadCallArgs.pOutBuf);
                    strncpy(pOutputArgsCall002->arrResult, "call002 returned data", sizeof(pOutputArgsCall002->arrResult));
                    rThreadCallArgs.nOutBufRetSize = rThreadCallArgs.nOutBufSize;

                    std::string sTmp(pOutputArgsCall002->arrResult, rThreadCallArgs.nOutBufRetSize);
                    printf("%s(): %s\n", __FUNCTION__, sTmp.c_str());
                }

                rThreadCallArgs.nRet = 0;
            }
            break;

        case CALL_ID_Call003:
            {
                printf("%s(): on CALL_ID_Call003\n", __FUNCTION__);
            }
            break;

        case CALL_ID_Call004:
            {
                printf("%s(): on CALL_ID_Call004\n", __FUNCTION__);
            }
            break;

        default:
            {
                nErrorCode = SMF_ERR_NOT_PROCESSED;
            }
            break;
        }

        return nErrorCode;
    }

	virtual SMF_ErrorCode ThreadEventQueueProc(SMF_BaseThread &rThread, QueueId nQueueId, EventData &rEventData)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;
        
        printf("%s(): nQueueId=%u, nEngineId=%u, nSenderId=%u, nEventId=%u\n", __FUNCTION__,
            nQueueId, rEventData.nEngineId, rEventData.nSenderId, rEventData.nEventId);

        if (rEventData.pThreadEventArgs != NULL) {
            rEventData.pThreadEventArgs->nOutBufRetSize = 0;

            if (rEventData.pThreadEventArgs->pInBuf != NULL && rEventData.pThreadEventArgs->nInBufSize > 0) {
                printf("%s(): buf_in=%s\n", __FUNCTION__, std::string(reinterpret_cast<char *>(rEventData.pThreadEventArgs->pInBuf), rEventData.pThreadEventArgs->nInBufSize).c_str());
            }
            if (rEventData.pThreadEventArgs->pOutBuf != NULL && rEventData.pThreadEventArgs->nOutBufSize > 0) {
                std::string sTmp;
                TK_Tools::FormatStr(sTmp, "[remote_ret] for event_id %u", rEventData.nEventId);
                printf("%s(): %s\n", __FUNCTION__, sTmp.c_str());
                strncpy(reinterpret_cast<char *>(rEventData.pThreadEventArgs->pOutBuf), sTmp.c_str(), rEventData.pThreadEventArgs->nOutBufSize);
                rEventData.pThreadEventArgs->nOutBufRetSize = rEventData.pThreadEventArgs->nOutBufSize;
            }
            rEventData.pThreadEventArgs->nRet = 0;
        }

        return nErrorCode;
    }
};

class RemoteThreadAgentApp : public RemoteCallAndEventDefinitons
{
public:
    RemoteThreadAgentApp()
        : m_bSendToRemote(true)
        , m_oThreadCallThreadAgent("RemoteThreadCallThread")
        , m_oThreadEventThreadAgent("RemoteThreadEventThread")
        , m_oMainStateMachineThreadAgent("MainStateMachineThread")
        , m_oLocalCallAndEventThread("LocalCallAndEventThread")
        , m_LocalCallAndEventThreadAgent("LocalCallAndEventThread")
    {
        m_oLocalCallAndEventThread.Start();
    }

    void SetSendToRemote(bool bSendToRemote)
    {
        m_bSendToRemote = bSendToRemote;
    }

    SMF_ErrorCode RemoteThreadCall11()
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        CallArgs ca;
        memset(&ca, 0, sizeof(CallArgs));
        ca.unInputArgs.stCall001.nArg1 = 11;

        SMF_ThreadCallHandler::ThreadCallArgs oThreadCallArgs;
        oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca.unInputArgs.stCall001);
        oThreadCallArgs.nInBufSize = sizeof(ca.unInputArgs.stCall001);
        oThreadCallArgs.pOutBuf = reinterpret_cast<unsigned char *>(&ca.unOutputArgs.stCall001);
        oThreadCallArgs.nOutBufSize = sizeof(ca.unOutputArgs.stCall001);

        if (m_bSendToRemote) {
            nErrorCode = m_oThreadCallThreadAgent.AsyncCall(CALL_ID_Call001, &oThreadCallArgs);
        } else {
            nErrorCode = m_LocalCallAndEventThreadAgent.AsyncCall(CALL_ID_Call001, &oThreadCallArgs);
        }
        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): AsyncCall(%d) is successful.\n", __FUNCTION__, CALL_ID_Call001);
        } else {
            printf("%s(): AsyncCall(%d) is failed.\n", __FUNCTION__, CALL_ID_Call001);
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode RemoteThreadCall12()
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;
        std::string sResult;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        CallArgs ca;
        memset(&ca, 0, sizeof(CallArgs));
        ca.unInputArgs.stCall002.nArg1 = 11;

        strncpy(ca.unInputArgs.stCall002.arrArg2, "[remote] arg2 input data", 40);

        SMF_ThreadCallHandler::ThreadCallArgs oThreadCallArgs;
        oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca.unInputArgs.stCall002);
        oThreadCallArgs.nInBufSize = sizeof(ca.unInputArgs.stCall002);
        oThreadCallArgs.pOutBuf = reinterpret_cast<unsigned char *>(&ca.unOutputArgs.stCall002);
        oThreadCallArgs.nOutBufSize = sizeof(ca.unOutputArgs.stCall002);

        if (m_bSendToRemote) {
            nErrorCode = m_oThreadCallThreadAgent.AsyncCall(CALL_ID_Call002, &oThreadCallArgs);
        } else {
            nErrorCode = m_LocalCallAndEventThreadAgent.AsyncCall(CALL_ID_Call002, &oThreadCallArgs);
        }

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): AsyncCall(%d) is successful.\n", __FUNCTION__, CALL_ID_Call002);
        } else {
            printf("%s(): AsyncCall(%d) is failed.\n", __FUNCTION__, CALL_ID_Call002);
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode RemoteThreadCall21()
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        CallArgs ca;
        memset(&ca, 0, sizeof(CallArgs));
        ca.unInputArgs.stCall001.nArg1 = 11;

        SMF_ThreadCallHandler::ThreadCallArgs oThreadCallArgs;
        oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca.unInputArgs.stCall001);
        oThreadCallArgs.nInBufSize = sizeof(ca.unInputArgs.stCall001);
        oThreadCallArgs.pOutBuf = reinterpret_cast<unsigned char *>(&ca.unOutputArgs.stCall001);
        oThreadCallArgs.nOutBufSize = sizeof(ca.unOutputArgs.stCall001);

        if (m_bSendToRemote) {
            nErrorCode = m_oThreadCallThreadAgent.SyncCall(CALL_ID_Call001, &oThreadCallArgs);
        } else {
            nErrorCode = m_LocalCallAndEventThreadAgent.SyncCall(CALL_ID_Call001, &oThreadCallArgs);
        }

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): SyncCall(%d) is successful.\n", __FUNCTION__, CALL_ID_Call001);
        } else {
            printf("%s(): SyncCall(%d) is failed.\n", __FUNCTION__, CALL_ID_Call001);
        }

        if (nErrorCode == SMF_ERR_OK) {
            if (oThreadCallArgs.nRet == 0 && oThreadCallArgs.nOutBufRetSize == oThreadCallArgs.nOutBufSize) {
                // do nothing
            } else {
                printf("%s(): call is failed!\n", __FUNCTION__);
            }
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode RemoteThreadCall22()
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;
        std::string sResult;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        CallArgs ca;
        memset(&ca, 0, sizeof(CallArgs));
        ca.unInputArgs.stCall002.nArg1 = 11;

        strncpy(ca.unInputArgs.stCall002.arrArg2, "[remote] arg2 input data", 40);

        SMF_ThreadCallHandler::ThreadCallArgs oThreadCallArgs;
        oThreadCallArgs.pInBuf = reinterpret_cast<unsigned char *>(&ca.unInputArgs.stCall002);
        oThreadCallArgs.nInBufSize = sizeof(ca.unInputArgs.stCall002);
        oThreadCallArgs.pOutBuf = reinterpret_cast<unsigned char *>(&ca.unOutputArgs.stCall002);
        oThreadCallArgs.nOutBufSize = sizeof(ca.unOutputArgs.stCall002);

        if (m_bSendToRemote) {
            nErrorCode = m_oThreadCallThreadAgent.SyncCall(CALL_ID_Call002, &oThreadCallArgs);
        } else {
            nErrorCode = m_LocalCallAndEventThreadAgent.SyncCall(CALL_ID_Call002, &oThreadCallArgs);
        }

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): SyncCall(%d) is successful.\n", __FUNCTION__, CALL_ID_Call002);
        } else {
            printf("%s(): SyncCall(%d) is failed.\n", __FUNCTION__, CALL_ID_Call002);
        }

        if (nErrorCode == SMF_ERR_OK) {
            if (oThreadCallArgs.nRet == 0 && oThreadCallArgs.nOutBufRetSize == oThreadCallArgs.nOutBufSize) {
                sResult = std::string(ca.unOutputArgs.stCall002.arrResult, sizeof(ca.unOutputArgs.stCall002.arrResult));
                printf("%s(): %s\n", __FUNCTION__, sResult.c_str());
            } else {
                printf("%s(): call is failed!\n", __FUNCTION__);
            }
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode PostEvent(unsigned int nEventId)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        unsigned char arrInBuf[50] = "[remote_input] Hello, event hander!";
        unsigned char arrOutBuf[50] = { 0 };

        SMF_ThreadEventQueueHandler::ThreadEventArgs oThreadEventArgs(arrInBuf, sizeof(arrInBuf), arrOutBuf, sizeof(arrOutBuf));

        if (m_bSendToRemote) {
            nErrorCode = m_oThreadEventThreadAgent.PostEvent(nEventId, &oThreadEventArgs);
        } else {
            nErrorCode = m_LocalCallAndEventThreadAgent.PostEvent(nEventId, &oThreadEventArgs);
        }

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): PostEvent(%d) is successful.\n", __FUNCTION__, nEventId);
        } else {
            printf("%s(): PostEvent(%d) is failed.\n", __FUNCTION__, nEventId);
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode SendEvent(unsigned int nEventId)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;
        std::string sResult;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        unsigned char arrInBuf[50] = "[remote_input] Hello, event hander!";
        unsigned char arrOutBuf[50] = { 0 };

        SMF_ThreadEventQueueHandler::ThreadEventArgs oThreadEventArgs(arrInBuf, sizeof(arrInBuf), arrOutBuf, sizeof(arrOutBuf));

        if (m_bSendToRemote) {
            nErrorCode = m_oThreadEventThreadAgent.SendEvent(nEventId, &oThreadEventArgs);
        } else {
            nErrorCode = m_LocalCallAndEventThreadAgent.SendEvent(nEventId, &oThreadEventArgs);
        }

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): SendEvent(%d) is successful.\n", __FUNCTION__, nEventId);
        } else {
            printf("%s(): SendEvent(%d) is failed.\n", __FUNCTION__, nEventId);
        }

        if (nErrorCode == SMF_ERR_OK) {
            if (oThreadEventArgs.nRet == 0 && oThreadEventArgs.nOutBufRetSize == oThreadEventArgs.nOutBufSize) {
                sResult = std::string(reinterpret_cast<char *>(arrOutBuf), sizeof(arrOutBuf));
                printf("%s(): %s\n", __FUNCTION__, sResult.c_str());
            } else {
                printf("%s(): no data returned!\n", __FUNCTION__);
            }
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode PostEvent2(unsigned int nEventId)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        unsigned char arrInBuf[50] = "[remote_input] Hello, event hander!";
        unsigned char arrOutBuf[50] = { 0 };

        SMF_ThreadEventQueueHandler::ThreadEventArgs oThreadEventArgs(arrInBuf, sizeof(arrInBuf), arrOutBuf, sizeof(arrOutBuf));
        nErrorCode = m_oMainStateMachineThreadAgent.PostEvent(nEventId, &oThreadEventArgs);

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): PostEvent(%d) is successful.\n", __FUNCTION__, nEventId);
        } else {
            printf("%s(): PostEvent(%d) is failed.\n", __FUNCTION__, nEventId);
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

    SMF_ErrorCode SendEvent2(unsigned int nEventId)
    {
        SMF_ErrorCode nErrorCode = SMF_ERR_OK;
        std::string sResult;

        SMF_LOG_MSG("<<<=== %s()\n", __FUNCTION__);

        unsigned char arrInBuf[50] = "[remote_input] Hello, event hander!";
        unsigned char arrOutBuf[50] = { 0 };

        SMF_ThreadEventQueueHandler::ThreadEventArgs oThreadEventArgs(arrInBuf, sizeof(arrInBuf), arrOutBuf, sizeof(arrOutBuf));
        nErrorCode = m_oMainStateMachineThreadAgent.SendEvent(nEventId, &oThreadEventArgs);

        if (nErrorCode == SMF_ERR_OK) {
            printf("%s(): SendEvent(%d) is successful.\n", __FUNCTION__, nEventId);
        } else {
            printf("%s(): SendEvent(%d) is failed.\n", __FUNCTION__, nEventId);
        }

        if (nErrorCode == SMF_ERR_OK) {
            if (oThreadEventArgs.nRet == 0 && oThreadEventArgs.nOutBufRetSize == oThreadEventArgs.nOutBufSize) {
                sResult = std::string(reinterpret_cast<char *>(arrOutBuf), sizeof(arrOutBuf));
                printf("%s(): %s\n", __FUNCTION__, sResult.c_str());
            } else {
                printf("%s(): no data returned!\n", __FUNCTION__);
            }
        }

        SMF_LOG_MSG("===>>> %s()\n", __FUNCTION__);

        return nErrorCode;
    }

private:
    bool m_bSendToRemote;

    SMF_RemoteThreadAgent m_oThreadCallThreadAgent;
    SMF_RemoteThreadAgent m_oThreadEventThreadAgent;
    SMF_RemoteThreadAgent m_oMainStateMachineThreadAgent;

    LocalCallAndEventThread m_oLocalCallAndEventThread;
    SMF_RemoteThreadAgent m_LocalCallAndEventThreadAgent;
};

int main(int argc, char* argv[])
{
    int nRet = -1;
    char sBuf[1024];
    std::vector<std::string> vCmdLine;

    TryDeleteTempObjectFiles();

    RemoteThreadAgentApp oRemoteThreadAgentApp;

    InitSock();
    InitIPC();
    InitRPC();

    while (true) {
        char *pLine = fgets(sBuf, sizeof(sBuf), stdin);
        if (pLine == NULL) {
            continue;
        }

        vCmdLine = TK_Tools::SplitString(TK_Tools::TrimAll(sBuf));
        if (vCmdLine.size() <= 0) {
            continue;
        }

        if (vCmdLine[0] == "exit") {
            nRet = 0;
            break;
        } else if (vCmdLine[0] == "h" || vCmdLine[0] == "help" || vCmdLine[0] == "?") {
            help();
            continue;
        } else if (vCmdLine[0] == "mail1") {
            MAIL_ID_PUB2_MAIL001_data data;
            memset(&data, 0x11, sizeof(MAIL_ID_PUB2_MAIL001_data));
            PostMail(MAIL_ID_PUB2_MAIL001, &data, sizeof(MAIL_ID_PUB2_MAIL001_data));
            continue;
        } else if (vCmdLine[0] == "mail2") {
            MAIL_ID_PUB2_MAIL002_data data;
            memset(&data, 0x22, sizeof(MAIL_ID_PUB2_MAIL002_data));
            PostMail(MAIL_ID_PUB2_MAIL002, &data, sizeof(MAIL_ID_PUB2_MAIL002_data));
            continue;
        } else if (vCmdLine[0] == "mail3") {
            MAIL_ID_PUB2_MAIL003_data data;
            memset(&data, 0x33, sizeof(MAIL_ID_PUB2_MAIL003_data));
            PostMail(MAIL_ID_PUB2_MAIL003, &data, sizeof(MAIL_ID_PUB2_MAIL003_data));
            continue;
        } else if (vCmdLine[0] == "remote_onoff") {
            if (vCmdLine.size() >= 2) {
                unsigned int nOnOff = TK_Tools::StrToUL(vCmdLine[1]);
                oRemoteThreadAgentApp.SetSendToRemote(nOnOff == 1);
            }
            continue;
        } else if (vCmdLine[0] == "rtc11") {
            oRemoteThreadAgentApp.RemoteThreadCall11();
            continue;
        } else if (vCmdLine[0] == "rtc12") {
            oRemoteThreadAgentApp.RemoteThreadCall12();
            continue;
        } else if (vCmdLine[0] == "rtc21") {
            oRemoteThreadAgentApp.RemoteThreadCall21();
            continue;
        } else if (vCmdLine[0] == "rtc22") {
            oRemoteThreadAgentApp.RemoteThreadCall22();
            continue;
        } else if (vCmdLine[0] == "pre") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = TK_Tools::StrToUL(vCmdLine[1]);
                oRemoteThreadAgentApp.PostEvent(nEventId);
            }
            continue;
        } else if (vCmdLine[0] == "sre") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = TK_Tools::StrToUL(vCmdLine[1]);
                oRemoteThreadAgentApp.SendEvent(nEventId);
            }
            continue;
        } else if (vCmdLine[0] == "pre2") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = TK_Tools::StrToUL(vCmdLine[1]);
                oRemoteThreadAgentApp.PostEvent2(nEventId);
            }
            continue;
        } else if (vCmdLine[0] == "sre2") {
            if (vCmdLine.size() >= 2) {
                unsigned int nEventId = TK_Tools::StrToUL(vCmdLine[1]);
                oRemoteThreadAgentApp.SendEvent2(nEventId);
            }
            continue;
        }

        TK_ERR("*** Unknown command: %s\n", vCmdLine[0].c_str());
    }

	return nRet;
}
