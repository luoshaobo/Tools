#ifndef SMF_RPC_TUNNEL_H__623743908874328582349094365469845683474378874
#define SMF_RPC_TUNNEL_H__623743908874328582349094365469845683474378874

#include "SMF_afx.h"
#include "SMF_PlatformLock.h"
#include "SMF_PlatformEvent.h"
#include "SMF_PlatformSharedMem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_RpcTunnel relevant interfaces
//
interface SMF_RpcTunnelDefinition : public SMF_BaseInterface
{
    enum {
        RPCT_BUF_SIZE = 1024 * 40,
    };
    enum {
        RPCT_COUNT_MAX = 50,
    };
    enum {
        RPCT_ID_INVALID = 0,
        RPCT_ID_BASE = 1,
    };
    typedef unsigned int RpcTunnelId;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_RpcTunnelFactory
//
class SMF_RpcTunnelFactory
    : private SMF_NonCopyable
    , public SMF_RpcTunnelDefinition
{
public:
    struct SharedRpcTunnelSetInfo {
        struct RpcTunnelInfo {
            unsigned int nState;
            RpcTunnelId nRpcTunnelId;
            int nRefCount;
            unsigned char pTunnelBuf[RPCT_BUF_SIZE];
            unsigned int nDataPos;
        } arrRpcTunnelInfo[RPCT_COUNT_MAX];
    };

private:
    SMF_RpcTunnelFactory();

public:
    virtual ~SMF_RpcTunnelFactory();

    static SMF_RpcTunnelFactory &GetInstance() {
        static SMF_RpcTunnelFactory oRpcTunnelMananger;
        return oRpcTunnelMananger;
    }

    SMF_ErrorCode AllocRpcTunnel(RpcTunnelId &nRpcTunnelId, SharedRpcTunnelSetInfo::RpcTunnelInfo **ppRpcTunnelInfo);
    SMF_ErrorCode FreeRpcTunnel(RpcTunnelId nRpcTunnelId);

    void Lock();
    void Unlock();

private:
    RpcTunnelId AllocTunnelId_unsafe(RpcTunnelId nRequestedRpcTunnelId);

private:
    SMF_PlatformLock m_oLock;
    SMF_PlatformSharedMem m_oSharedMem;
    unsigned char *m_pSharedMem;
    SharedRpcTunnelSetInfo *m_pSharedRpcTunnelSetInfo;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_RpcTunnel
//
class SMF_RpcTunnel
    : private SMF_NonCopyable
    , public SMF_RpcTunnelDefinition
{
private:
    enum State {
        ST_NO_CALL_NOR_RESULT,
        ST_CALL_PUT_IN,
        ST_CALL_PROC_BEGIN,
        ST_CALL_PROC_END,
        ST_RESULT_PUT_IN,
        ST_INVALID = -1,
    };

    struct CallHeader {
        CallHeader() : nCallSize(0), nCheckSum(0) {}
        CallHeader(unsigned short a_nCallSize, unsigned short a_nCheckSum)
            : nCallSize(a_nCallSize), nCheckSum(a_nCheckSum) {}

        unsigned short nCallSize;
        unsigned short nCheckSum;
    };

private:
    typedef SMF_RpcTunnelFactory::SharedRpcTunnelSetInfo::RpcTunnelInfo RpcTunnelInfo;

public:
    SMF_RpcTunnel();
    virtual ~SMF_RpcTunnel();

    SMF_ErrorCode Bind(RpcTunnelId nRpcTunnelId);

    SMF_ErrorCode GetCall(unsigned char *pBuf, unsigned int &nBufSize, unsigned int nTimeout = SMF_INFINITE);
    SMF_ErrorCode CallProcEnd();
    SMF_ErrorCode PutCallResult(const unsigned char *pBuf, unsigned int nBufSize);

    SMF_ErrorCode PutCall(const unsigned char *pBuf, unsigned int nBufSize);
    SMF_ErrorCode GetCallResult(unsigned char *pBuf, unsigned int &nBufSize);

public:
    RpcTunnelId GetRpcTunnelId() const { return m_nRpcTunnelId; }

private:
    SMF_ErrorCode AllocRpcTunnel();
    SMF_ErrorCode FreeRpcTunnel();

    void GetFromTunnel_unsafe(unsigned char *pBuf, unsigned int &nBufSize);
    void PutToTunnel_unsafe(const unsigned char *pBuf, unsigned int nBufSize);
    void ResetTunnel_unsafe();

    void Lock();
    void Unlock();

    State GetState();
    void SetState(State nState);

private:
    SMF_RpcTunnelFactory &m_rRpcTunnelFactory;
    RpcTunnelId m_nRpcTunnelId;
    RpcTunnelInfo *m_pRpcTunnelInfo;
    SMF_PlatformEvent *m_pNoCallNorResultEvent;
    std::string m_sNoCallNorResultEventName;
    SMF_PlatformEvent *m_pCallPutInEvent;
    std::string m_sCallPutInEventName;
    SMF_PlatformEvent *m_pCallProcEndEvent;
    std::string m_sCallProcEndEventName;
    SMF_PlatformEvent *m_pResultPutInEvent;
    std::string m_sResultPutInEventName;
    SMF_PlatformLock *m_pLock;
    std::string m_sLockName;
};

#endif // #ifndef SMF_RPC_TUNNEL_H__623743908874328582349094365469845683474378874
