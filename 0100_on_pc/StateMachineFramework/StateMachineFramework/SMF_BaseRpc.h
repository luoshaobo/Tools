#ifndef SMF_BASE_RPC_H__783458208923523589209489235782858234782385734578
#define SMF_BASE_RPC_H__783458208923523589209489235782858234782385734578

#include "SMF_afx.h"
#include "SMF_RpcTunnel.h"
#include "SMF_BaseThread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseRPC relevant interfaces
//
interface SMF_BaseRpcDefinition : public SMF_BaseInterface
{
    enum {
        RPC_HOST_ID_INVAILD = 0,
        RPC_HOST_ID_BASE,
    };

    typedef unsigned int RpcHostId;
    typedef unsigned int CallId;

    struct RpcCallArgs {
        RpcCallArgs(unsigned char *a_pInBuf, unsigned int a_nInBufSize, unsigned char *a_pOutBuf, unsigned int a_nOutBufSize)
            : pInBuf(a_pInBuf), nInBufSize(a_nInBufSize), pOutBuf(a_pOutBuf), nOutBufSize(a_nOutBufSize), nOutBufRetSize(0), nRet(0) {}

        unsigned char *pInBuf;
        unsigned int nInBufSize;
        unsigned char *pOutBuf;         /* out */
        unsigned int nOutBufSize;
        unsigned int nOutBufRetSize;    /* out */
        int nRet;                       /* out */
    };
};

interface SMF_BaseRpcHostHandler : public SMF_BaseRpcDefinition
{
    virtual SMF_ErrorCode OnRpcCall(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs) = 0;
};

interface SMF_BaseRpcClientHandler : public SMF_BaseRpcDefinition
{
    virtual SMF_ErrorCode CallRpc(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SMF_BaseRpc
//
class SMF_BaseRpc
    : private SMF_NonCopyable
    , public SMF_BaseRpcHostHandler
    , public SMF_BaseRpcClientHandler
    , public SMF_ThreadHandler
{
    //
    // constructors and destructors 
    //
public:
    SMF_BaseRpc();
    virtual ~SMF_BaseRpc();

    //
    // operations
    //
public:
    SMF_ErrorCode BindRpcHostId(RpcHostId nRpcHostId);
    SMF_ErrorCode StartRpcHost();

    //
    // setters
    //
public:
    SMF_ErrorCode SetRpcHostHandler(SMF_BaseRpcHostHandler *pRpcHostHandler);

    //
    // getters
    //
public:
    SMF_BaseRpcClientHandler &GetRpcClientHandler() { return *this; }

    //
    // implementation interfaces
    //
protected:
    virtual SMF_ErrorCode OnRpcCall(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs);
    virtual SMF_ErrorCode CallRpc(RpcHostId nRpcHostId, CallId nCallId, RpcCallArgs &rRpcCallArgs);

private:
    virtual SMF_ErrorCode ThreadProc(SMF_BaseThread &rThread);

    //
    // data
    //
private:
    RpcHostId m_nRpcHostId;
    SMF_BaseRpcHostHandler *m_pRpcHostHandler;
    SMF_BaseThread m_oRpcHostThread;
    SMF_RpcTunnel m_oRpcTunnel;
};

#endif // #ifndef SMF_BASE_RPC_H__783458208923523589209489235782858234782385734578
