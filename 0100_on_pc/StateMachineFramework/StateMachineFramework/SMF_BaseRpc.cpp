#include "SMF_BaseRpc.h"

#define SMF_ROUND_SIZE_TO_INT_BOUNDARY(n)                       (((n) + sizeof(int) - 1) / sizeof(int) * sizeof(int))

namespace {

struct RpcCallInInfo {
    SMF_BaseRpc::RpcHostId nRpcHostId;
    SMF_BaseRpc::CallId nCallId;
    unsigned int nInBufSize;
    unsigned int nOutBufSize;
    unsigned char arrInBuf[1];
};

struct RpcCallOutInfo {
    SMF_BaseRpc::RpcHostId nRpcHostId;
    SMF_BaseRpc::CallId nCallId;
    unsigned int nOutBufRetSize;
    int nRet;
    unsigned char arrOutBuf[1];
};

} //namespace {

SMF_BaseRpc::SMF_BaseRpc()
    : SMF_NonCopyable()
    , SMF_BaseRpcHostHandler()
    , SMF_BaseRpcClientHandler()
    , SMF_ThreadHandler()
    , m_nRpcHostId(RPC_HOST_ID_INVAILD)
    , m_pRpcHostHandler(NULL)
    , m_oRpcHostThread("RpcThread")
    , m_oRpcTunnel()
{
    m_oRpcHostThread.SetThreadHandler(this);
}

SMF_BaseRpc::~SMF_BaseRpc()
{
    m_oRpcHostThread.Stop();
}

SMF_ErrorCode SMF_BaseRpc::BindRpcHostId(RpcHostId nRpcHostId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = m_oRpcTunnel.Bind(nRpcHostId);

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseRpc::StartRpcHost()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    nErrorCode = m_oRpcHostThread.Start();

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseRpc::SetRpcHostHandler(SMF_BaseRpcHostHandler *pRpcHostHandler)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    m_pRpcHostHandler = pRpcHostHandler;

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseRpc::OnRpcCall(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseRpc::CallRpc(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    RpcCallInInfo *pRpcCallInInfo = NULL;
    unsigned int nRpcCallInInfoSize = SMF_ROUND_SIZE_TO_INT_BOUNDARY(sizeof(RpcCallInInfo) + rRpcCallArgs.nInBufSize);
    RpcCallOutInfo *pRpcCallOutInfo = NULL;
    unsigned int nRpcCallOutInfoSize = SMF_ROUND_SIZE_TO_INT_BOUNDARY(sizeof(RpcCallOutInfo) + rRpcCallArgs.nOutBufSize);

    if (nErrorCode == SMF_ERR_OK) {
        if (m_nRpcHostId != nRpcHostId) {
            nErrorCode = BindRpcHostId(nRpcHostId);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_nRpcHostId != nRpcHostId) {
            m_nRpcHostId = nRpcHostId;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pRpcCallInInfo = reinterpret_cast<RpcCallInInfo *>(new unsigned int[nRpcCallInInfoSize / sizeof(int)]);
        if (pRpcCallInInfo == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pRpcCallOutInfo = reinterpret_cast<RpcCallOutInfo *>(new unsigned int[nRpcCallOutInfoSize / sizeof(int)]);
        if (pRpcCallOutInfo == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        memset(pRpcCallInInfo, 0, nRpcCallInInfoSize);
        pRpcCallInInfo->nRpcHostId = nRpcHostId;
        pRpcCallInInfo->nCallId = nCallId;
        pRpcCallInInfo->nInBufSize = rRpcCallArgs.nInBufSize;
        pRpcCallInInfo->nOutBufSize = rRpcCallArgs.nOutBufSize;
        if (rRpcCallArgs.pInBuf != NULL && pRpcCallInInfo->nInBufSize > 0) {
            memcpy(&pRpcCallInInfo->arrInBuf, rRpcCallArgs.pInBuf, pRpcCallInInfo->nInBufSize);
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oRpcTunnel.PutCall(reinterpret_cast<const unsigned char *>(pRpcCallInInfo), nRpcCallInInfoSize);
    }

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oRpcTunnel.GetCallResult(reinterpret_cast<unsigned char *>(pRpcCallOutInfo), nRpcCallOutInfoSize);
    }

    if (nErrorCode == SMF_ERR_OK) {
        rRpcCallArgs.nRet = pRpcCallOutInfo->nRet;
        rRpcCallArgs.nOutBufRetSize = pRpcCallOutInfo->nOutBufRetSize;
        if (rRpcCallArgs.pOutBuf != NULL && rRpcCallArgs.nOutBufRetSize > 0) {
            memcpy(rRpcCallArgs.pOutBuf, &pRpcCallOutInfo->arrOutBuf, rRpcCallArgs.nOutBufRetSize);
        }
    }

    if (pRpcCallOutInfo != NULL) {
        delete [] (reinterpret_cast<int *>(pRpcCallOutInfo));
        pRpcCallOutInfo = NULL;
    }

    if (pRpcCallInInfo != NULL) {
        delete [] (reinterpret_cast<int *>(pRpcCallInInfo));
        pRpcCallInInfo = NULL;
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_BaseRpc::ThreadProc(SMF_BaseThread &rThread)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    RpcCallInInfo *pRpcCallInInfo = NULL;
    unsigned int nRpcCallInInfoSize = SMF_ROUND_SIZE_TO_INT_BOUNDARY(SMF_RpcTunnel::RPCT_BUF_SIZE * 2);
    RpcCallOutInfo *pRpcCallOutInfo = NULL;
    unsigned int nRpcCallOutInfoSize = SMF_ROUND_SIZE_TO_INT_BOUNDARY(SMF_RpcTunnel::RPCT_BUF_SIZE);

    rThread.SetThreadState(SMF_BaseThread::TS_CREATED);
    rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATING);

    if (nErrorCode == SMF_ERR_OK) {
        pRpcCallInInfo = reinterpret_cast<RpcCallInInfo *>(new unsigned int[nRpcCallInInfoSize / sizeof(int)]);
        if (pRpcCallInInfo == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        pRpcCallOutInfo = reinterpret_cast<RpcCallOutInfo *>(new unsigned int[nRpcCallOutInfoSize / sizeof(int)]);
        if (pRpcCallOutInfo == NULL) {
            nErrorCode = SMF_ERR_FAILED_TO_ALLOC_MEM;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        rThread.SetThreadState(SMF_BaseThread::TS_ACTIVATED);
        while (true) {
            if (rThread.GetThreadState() != SMF_BaseThread::TS_ACTIVATED)
            {
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
                break;
            }

            nErrorCode = m_oRpcTunnel.GetCall(reinterpret_cast<unsigned char *>(pRpcCallInInfo), nRpcCallInInfoSize, SMF_THREAD_AWAKEN_INTERVAL_MILLISECONDS);
            if (nErrorCode == SMF_ERR_TIMEOUT) {
                continue;
            }
            if (nErrorCode != SMF_ERR_OK) {
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
                break;
            }

            RpcCallArgs oRpcCallArgs(
                pRpcCallInInfo->arrInBuf,
                pRpcCallInInfo->nInBufSize,
                pRpcCallOutInfo->arrOutBuf,
                pRpcCallInInfo->nOutBufSize
            );
            SMF_BaseRpcHostHandler *pRpcHostHandler = m_pRpcHostHandler != NULL ? m_pRpcHostHandler : this;
            nErrorCode = pRpcHostHandler->OnRpcCall(pRpcCallInInfo->nRpcHostId, pRpcCallInInfo->nCallId, oRpcCallArgs);
            if (nErrorCode != SMF_ERR_OK) {
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
                break;
            }

            nErrorCode = m_oRpcTunnel.CallProcEnd();
            if (nErrorCode != SMF_ERR_OK) {
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
                break;
            }

            pRpcCallOutInfo->nRpcHostId = pRpcCallInInfo->nRpcHostId;
            pRpcCallOutInfo->nCallId = pRpcCallInInfo->nCallId;
            pRpcCallOutInfo->nOutBufRetSize = oRpcCallArgs.nOutBufRetSize;
            pRpcCallOutInfo->nRet = oRpcCallArgs.nRet;
            unsigned int nRpcCallOutInfoRealSize = SMF_ROUND_SIZE_TO_INT_BOUNDARY(sizeof(RpcCallOutInfo) + pRpcCallOutInfo->nOutBufRetSize);
            nErrorCode = m_oRpcTunnel.PutCallResult(reinterpret_cast<const unsigned char *>(pRpcCallOutInfo), nRpcCallOutInfoRealSize);
            if (nErrorCode != SMF_ERR_OK) {
                rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVING);
                break;
            }
        }
        rThread.SetThreadState(SMF_BaseThread::TS_DEACTIVED);
    }

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYING);

    if (pRpcCallOutInfo != NULL) {
        delete [] (reinterpret_cast<int *>(pRpcCallOutInfo));
        pRpcCallOutInfo = NULL;
    }

    if (pRpcCallInInfo != NULL) {
        delete [] (reinterpret_cast<int *>(pRpcCallInInfo));
        pRpcCallInInfo = NULL;
    }

    rThread.SetThreadState(SMF_BaseThread::TS_DESTRORYED);

    return nErrorCode;
}
