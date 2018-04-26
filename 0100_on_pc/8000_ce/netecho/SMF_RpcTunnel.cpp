#include "TK_Tools.h"
#include "SMF_RpcTunnel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_RpcTunnelFactory
//
SMF_RpcTunnelFactory::SMF_RpcTunnelFactory()
    : SMF_NonCopyable()
    , SMF_RpcTunnelDefinition()
    , m_oLock("RpcTunnelFactory_Mutex")
    , m_oSharedMem(sizeof(SharedRpcTunnelSetInfo), "RpcTunnelFactory_SharesMem")
    , m_pSharedMem(NULL)
    , m_pSharedRpcTunnelSetInfo(NULL)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        nErrorCode = m_oSharedMem.Map();
    }

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        m_pSharedMem = reinterpret_cast<unsigned char *>(m_oSharedMem.GetSharedMemAddr());
        if (m_pSharedMem != NULL) {
            if (!m_oSharedMem.AlreadyExists()) {
                memset(m_pSharedMem, 0, sizeof(SharedRpcTunnelSetInfo));
            }
        }
        if (m_pSharedMem != NULL) {
            m_pSharedRpcTunnelSetInfo = reinterpret_cast<SharedRpcTunnelSetInfo *>(m_pSharedMem);
        }
        Unlock();
    }
}

SMF_RpcTunnelFactory::~SMF_RpcTunnelFactory()
{
    m_oSharedMem.Unmap();
}

SMF_ErrorCode SMF_RpcTunnelFactory::AllocRpcTunnel(RpcTunnelId &nRpcTunnelId, SharedRpcTunnelSetInfo::RpcTunnelInfo **ppRpcTunnelInfo)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    RpcTunnelId nRpcTunnelIdMax = RPCT_ID_INVALID;
    unsigned int i;
    unsigned int nFirstInvalidIndex = RPCT_COUNT_MAX;

    if (nErrorCode == SMF_ERR_OK) {
        if (ppRpcTunnelInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedRpcTunnelSetInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    Lock();
    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < RPCT_COUNT_MAX; i++) {
            SharedRpcTunnelSetInfo::RpcTunnelInfo &oRpcTunnelInfo = m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i];
            if (oRpcTunnelInfo.nRpcTunnelId > nRpcTunnelIdMax) {
                nRpcTunnelIdMax = oRpcTunnelInfo.nRpcTunnelId;
            }
            if (oRpcTunnelInfo.nRpcTunnelId == RPCT_ID_INVALID && nFirstInvalidIndex == RPCT_COUNT_MAX) {
                nFirstInvalidIndex = i;
            }
            if (nRpcTunnelId != RPCT_ID_INVALID && nRpcTunnelId == oRpcTunnelInfo.nRpcTunnelId) {
                break;
            }
        }

        if (i < RPCT_COUNT_MAX) {
            m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i].nRefCount++;
            *ppRpcTunnelInfo = &m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i];
        } else {
            if (nFirstInvalidIndex < RPCT_COUNT_MAX) {
                nRpcTunnelId = AllocTunnelId_unsafe(nRpcTunnelIdMax + 1);
                SharedRpcTunnelSetInfo::RpcTunnelInfo &oRpcTunnelInfo = m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[nFirstInvalidIndex];
                oRpcTunnelInfo.nRpcTunnelId = nRpcTunnelId;
                oRpcTunnelInfo.nRefCount = 1;
                oRpcTunnelInfo.nDataPos = 0;
                *ppRpcTunnelInfo = &oRpcTunnelInfo;
            } else {
                nErrorCode = SMF_ERR_FAILED_TO_ALLOC_RPC_TUNNEL;
            }
        }
    }
    Unlock();

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnelFactory::FreeRpcTunnel(RpcTunnelId nRpcTunnelId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    unsigned int i;

    if (nErrorCode == SMF_ERR_OK) {
        if (nRpcTunnelId == RPCT_ID_INVALID) {
            nErrorCode = SMF_ERR_INVALID_RPC_TUNNEL_ID;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pSharedRpcTunnelSetInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    Lock();
    if (nErrorCode == SMF_ERR_OK) {
        for (i = 0; i < RPCT_COUNT_MAX; i++) {
            SharedRpcTunnelSetInfo::RpcTunnelInfo &oRpcTunnelInfo = m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i];
            if (oRpcTunnelInfo.nRpcTunnelId == nRpcTunnelId) {
                break;
            }
        }

        if (i < RPCT_COUNT_MAX) {
            m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i].nRefCount--;
            if (m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i].nRefCount <= 0) {
                m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i].nRpcTunnelId = RPCT_ID_INVALID;
            }
        } else {
            nErrorCode = SMF_ERR_RPC_TUNNEL_ALREADY_FREED;
        }
    }
    Unlock();

    return nErrorCode;
}

void SMF_RpcTunnelFactory::Lock()
{
    m_oLock.Lock();
}

void SMF_RpcTunnelFactory::Unlock()
{
    m_oLock.Unlock();
}

SMF_RpcTunnelFactory::RpcTunnelId SMF_RpcTunnelFactory::AllocTunnelId_unsafe(RpcTunnelId nRequestedRpcTunnelId)
{
    RpcTunnelId nRetRpcTunnelId = RPCT_ID_INVALID;
    std::set<RpcTunnelId> setTunnelIds;
    unsigned int i;

    for (i = 0; i < RPCT_COUNT_MAX; i++) {
        RpcTunnelId nRpcTunnelId = m_pSharedRpcTunnelSetInfo->arrRpcTunnelInfo[i].nRpcTunnelId;
        if (nRpcTunnelId != RPCT_ID_INVALID) {
            setTunnelIds.insert(nRpcTunnelId);
        }
    }

    if (nRequestedRpcTunnelId != RPCT_ID_INVALID) {
        std::set<RpcTunnelId>::iterator it = setTunnelIds.find(nRequestedRpcTunnelId);
        if (it == setTunnelIds.end()) {
            nRetRpcTunnelId = nRequestedRpcTunnelId;
        } else {
            nRequestedRpcTunnelId = RPCT_ID_INVALID;
        }
    }

    if (nRequestedRpcTunnelId == RPCT_ID_INVALID) {
        for (nRequestedRpcTunnelId = RPCT_ID_BASE;; nRetRpcTunnelId++) {
            std::set<RpcTunnelId>::iterator it = setTunnelIds.find(nRequestedRpcTunnelId);
            if (it == setTunnelIds.end()) {
                nRetRpcTunnelId = nRequestedRpcTunnelId;
                break;
            }
        }
    }

    return nRetRpcTunnelId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_RpcTunnel
//
SMF_RpcTunnel::SMF_RpcTunnel()
    : SMF_NonCopyable()
    , SMF_RpcTunnelDefinition()
    , m_rRpcTunnelFactory(SMF_RpcTunnelFactory::GetInstance())
    , m_nRpcTunnelId(RPCT_ID_INVALID)
    , m_pRpcTunnelInfo(NULL)
    , m_pNoCallNorResultEvent(NULL)
    , m_sNoCallNorResultEventName()
    , m_pCallPutInEvent(NULL)
    , m_sCallPutInEventName()
    , m_pCallProcEndEvent(NULL)
    , m_sCallProcEndEventName()
    , m_pResultPutInEvent(NULL)
    , m_sResultPutInEventName()
    , m_pLock(NULL)
    , m_sLockName()
{

}

SMF_RpcTunnel::~SMF_RpcTunnel()
{
    if (m_pLock != NULL) {
        delete m_pLock;
        m_pLock = NULL;
    }
    if (m_pResultPutInEvent != NULL) {
        delete m_pResultPutInEvent;
        m_pResultPutInEvent = NULL;
    }
    if (m_pCallProcEndEvent != NULL) {
        delete m_pCallProcEndEvent;
        m_pCallProcEndEvent = NULL;
    }
    if (m_pCallPutInEvent != NULL) {
        delete m_pCallPutInEvent;
        m_pCallPutInEvent = NULL;
    }
    if (m_pNoCallNorResultEvent != NULL) {
        delete m_pNoCallNorResultEvent;
        m_pNoCallNorResultEvent = NULL;
    }

    FreeRpcTunnel();
}

SMF_RpcTunnel::State SMF_RpcTunnel::GetState()
{
    State nState;

    if (m_pRpcTunnelInfo == NULL) {
        nState = ST_INVALID;
    } else {
        nState = static_cast<State>(m_pRpcTunnelInfo->nState);
    }

    return nState;
}

void SMF_RpcTunnel::SetState(State nState)
{
    if (m_pRpcTunnelInfo != NULL) {
         m_pRpcTunnelInfo->nState = nState;
    }
}

SMF_ErrorCode SMF_RpcTunnel::Bind(RpcTunnelId nRpcTunnelId)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pLock != NULL) {
            delete m_pLock;
            m_pLock = NULL;
        }
        if (m_pResultPutInEvent != NULL) {
            delete m_pResultPutInEvent;
            m_pResultPutInEvent = NULL;
        }
        if (m_pCallProcEndEvent != NULL) {
            delete m_pCallProcEndEvent;
            m_pCallProcEndEvent = NULL;
        }
        if (m_pCallPutInEvent != NULL) {
            delete m_pCallPutInEvent;
            m_pCallPutInEvent = NULL;
        }
        if (m_pNoCallNorResultEvent != NULL) {
            delete m_pNoCallNorResultEvent;
            m_pNoCallNorResultEvent = NULL;
        }

        //m_sLockName.clear();
        //m_sResultPutInEventName.clear();
        //m_sCallProcEndEventName.clear();
        //m_sNoCallNorResultEventName.clear();
        //m_sCallPutInEventName.clear();

        m_sLockName = "";
        m_sResultPutInEventName = "";
        m_sCallProcEndEventName = "";
        m_sNoCallNorResultEventName = "";
        m_sCallPutInEventName = "";

        FreeRpcTunnel();
        m_nRpcTunnelId = nRpcTunnelId;
        nErrorCode = AllocRpcTunnel();
    }

    if (nErrorCode == SMF_ERR_OK) {
        TK_Tools::FormatStr(m_sLockName, "RpcTunnel_%u_Mutex", m_nRpcTunnelId);
        m_pLock = new SMF_PlatformLock(m_sLockName.c_str());

        TK_Tools::FormatStr(m_sNoCallNorResultEventName, "RpcTunnel_%u_NoCallNorResultEvent", m_nRpcTunnelId);
        m_pNoCallNorResultEvent = new SMF_PlatformEvent(FALSE, FALSE, m_sNoCallNorResultEventName.c_str());

        TK_Tools::FormatStr(m_sCallPutInEventName, "RpcTunnel_%u_CallPutInEvent", m_nRpcTunnelId);
        m_pCallPutInEvent = new SMF_PlatformEvent(FALSE, FALSE, m_sCallPutInEventName.c_str());

        TK_Tools::FormatStr(m_sResultPutInEventName, "RpcTunnel_%u_ResultPutInEvent", m_nRpcTunnelId);
        m_pResultPutInEvent = new SMF_PlatformEvent(FALSE, FALSE, m_sResultPutInEventName.c_str());

        TK_Tools::FormatStr(m_sCallProcEndEventName, "RpcTunnel_%u_CallProcEndEvent", m_nRpcTunnelId);
        m_pCallProcEndEvent = new SMF_PlatformEvent(FALSE, FALSE, m_sCallProcEndEventName.c_str());
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::GetCall(unsigned char *pBuf, unsigned int &nBufSize, unsigned int nTimeout /*= SMF_INFINITE*/)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bOk;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize == 0) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRpcTunnelInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            bOk = GetState() == ST_CALL_PUT_IN;
            if (!bOk) {
                Unlock();
                nErrorCode = SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, m_pCallPutInEvent, nTimeout);
                if (nErrorCode != SMF_ERR_OK) {
                    break;
                } else if (nWaitResult == SMF_PlatformEvent::WAIT_RESULT_TIMEOUT) {
                    nErrorCode = SMF_ERR_TIMEOUT;
                    break;
                }
            } else {
                if (nBufSize < m_pRpcTunnelInfo->nDataPos - sizeof(CallHeader)) {
                    nErrorCode = SMF_ERR_OUT_OF_BUFFER_CAPCITY;
                    Unlock();
                } else {
                    GetFromTunnel_unsafe(pBuf, nBufSize);
                    SetState(ST_CALL_PROC_BEGIN);
                    Unlock();
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::CallProcEnd()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bOk;

    if (nErrorCode == SMF_ERR_OK) {
        Lock();
        bOk = GetState() == ST_CALL_PROC_BEGIN;
        if (!bOk) {
            Unlock();
            nErrorCode = SMF_ERR_RPC_CALL_NOT_BEGIN;
        } else {
            SetState(ST_CALL_PROC_END);
            Unlock();
            if (m_pCallProcEndEvent != NULL) {
                m_pCallProcEndEvent->SetEvent();
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::PutCallResult(const unsigned char *pBuf, unsigned int nBufSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bOk;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize == 0) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRpcTunnelInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize + sizeof(CallHeader) > RPCT_BUF_SIZE) {
            nErrorCode = SMF_ERR_OUT_OF_BUFFER_CAPCITY;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            bOk = GetState() == ST_CALL_PROC_END;
            if (!bOk) {
                Unlock();
                SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, m_pCallProcEndEvent);
            } else {
                PutToTunnel_unsafe(pBuf, nBufSize);
                SetState(ST_RESULT_PUT_IN);
                Unlock();
                if (m_pResultPutInEvent != NULL) {
                    m_pResultPutInEvent->SetEvent();
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::PutCall(const unsigned char *pBuf, unsigned int nBufSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bOk;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize == 0) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize + sizeof(CallHeader) > RPCT_BUF_SIZE) {
            nErrorCode = SMF_ERR_TOO_LARGE_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRpcTunnelInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            bOk = GetState() == ST_NO_CALL_NOR_RESULT;
            if (!bOk) {
                Unlock();
                SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, m_pNoCallNorResultEvent);
            } else {
                PutToTunnel_unsafe(pBuf, nBufSize);
                SetState(ST_CALL_PUT_IN);
                Unlock();
                if (m_pCallPutInEvent != NULL) {
                    m_pCallPutInEvent->SetEvent();
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::GetCallResult(unsigned char *pBuf, unsigned int &nBufSize)
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;
    bool bOk;
    SMF_PlatformEvent::WaitResult nWaitResult;

    if (nErrorCode == SMF_ERR_OK) {
        if (pBuf == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (nBufSize == 0) {
            nErrorCode = SMF_ERR_EMPTY_BUFFER;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRpcTunnelInfo == NULL) {
            nErrorCode = SMF_ERR_NULL_PTR;
        }
    }

    if (nErrorCode == SMF_ERR_OK) {
        while (true) {
            Lock();
            bOk = GetState() == ST_RESULT_PUT_IN;
            if (!bOk) {
                Unlock();
                SMF_PlatformEvent::WaitForSingleEvent(nWaitResult, m_pResultPutInEvent);
            } else {
                if (nBufSize < m_pRpcTunnelInfo->nDataPos - sizeof(CallHeader)) {
                    nErrorCode = SMF_ERR_OUT_OF_BUFFER_CAPCITY;
                    Unlock();
                } else {
                    GetFromTunnel_unsafe(pBuf, nBufSize);
                    ResetTunnel_unsafe();
                    SetState(ST_NO_CALL_NOR_RESULT);
                    Unlock();
                    if (m_pNoCallNorResultEvent != NULL) {
                        m_pNoCallNorResultEvent->SetEvent();
                    }
                }
                break;
            }
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::AllocRpcTunnel()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRpcTunnelInfo == NULL) {
            nErrorCode = m_rRpcTunnelFactory.AllocRpcTunnel(m_nRpcTunnelId, &m_pRpcTunnelInfo);
        }
    }

    return nErrorCode;
}

SMF_ErrorCode SMF_RpcTunnel::FreeRpcTunnel()
{
    SMF_ErrorCode nErrorCode = SMF_ERR_OK;

    if (nErrorCode == SMF_ERR_OK) {
        if (m_pRpcTunnelInfo != NULL) {
            nErrorCode = m_rRpcTunnelFactory.FreeRpcTunnel(m_nRpcTunnelId);
            m_pRpcTunnelInfo = NULL;
        }
    }

    return nErrorCode;
}

void SMF_RpcTunnel::GetFromTunnel_unsafe(unsigned char *pBuf, unsigned int &nBufSize)
{
    CallHeader oCallHeader;

    memcpy(reinterpret_cast<void *>(&oCallHeader), reinterpret_cast<const void *>(m_pRpcTunnelInfo->pTunnelBuf + 0), sizeof(CallHeader));
    memcpy(reinterpret_cast<void *>(pBuf), reinterpret_cast<const void *>(m_pRpcTunnelInfo->pTunnelBuf + sizeof(CallHeader)), oCallHeader.nCallSize);
    nBufSize = oCallHeader.nCallSize;
}

void SMF_RpcTunnel::PutToTunnel_unsafe(const unsigned char *pBuf, unsigned int nBufSize)
{
    CallHeader oCallHeader(nBufSize, 0);

    memcpy(reinterpret_cast<void *>(m_pRpcTunnelInfo->pTunnelBuf + 0), reinterpret_cast<const void *>(&oCallHeader), sizeof(CallHeader));
    memcpy(reinterpret_cast<void *>(m_pRpcTunnelInfo->pTunnelBuf + sizeof(CallHeader)), reinterpret_cast<const void *>(pBuf), nBufSize);

    m_pRpcTunnelInfo->nDataPos = sizeof(CallHeader) + nBufSize;
}

void SMF_RpcTunnel::ResetTunnel_unsafe()
{
    m_pRpcTunnelInfo->nDataPos = 0;
}

void SMF_RpcTunnel::Lock()
{
    if (m_pLock != NULL) {
        m_pLock->Lock();
    }
}

void SMF_RpcTunnel::Unlock()
{
    if (m_pLock != NULL) {
        m_pLock->Unlock();
    }
}
