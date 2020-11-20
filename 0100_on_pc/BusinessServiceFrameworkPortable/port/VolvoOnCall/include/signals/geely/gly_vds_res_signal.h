///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    @file gly_vds_res_signal.h
//    geely res signal.

// @project        GLY_TCAM
// @subsystem    Application
// @author        uia93888
// @Init date    3-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_RES_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_RES_SIGNAL_H_

//#include "VDServiceRequest.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"


//#include "asn.h"
#include "voc_framework/signal_adapt/app_data.h"


#include <memory>

class SignalAdaptBase;

namespace volvo_on_call
{
    static const int RMT_ENGINE_START_DEFAULT_TIME = 17; //uint: minutes
    static const int RMT_ENGINE_START_MIX_TIME = 59;     //uint: minutes
    static const int RMT_ENGINE_START_MIN_TIME = 0;      //uint: minutes
    static const int RMT_ENGINE_SERVICE_ID = 17;
    static const int RMT_VEHICLE_ERROR_CODE_DEFAULT = -1;
    /**
     * \brief the commands to operate ERS.
     */
    enum ServiceCmd
    {
        kStartService     = 0,   // start service
        kRequestData      = 1,   // the request data
        kResponseData     = 2,   // the response data
        kExecutionAction  = 4,   // the execution action
        kTerminateService = 20   // terminate service
    };

    //create request structure, include ErsCmd, ErsRuntime, ImobVehRemReqCmd...
    /*typedef enum{
        ErsCmd_ErsCmdNotSet = 0,
        ErsCmd_ErsCmdOn,
        ErsCmd_ErsCmdOff
    } ErsCmd;*/
    typedef long ErsRunTime;
    typedef enum {
        ImobVehRemReqCmd_ImobRemReqIdle = 0,
        ImobVehRemReqCmd_NoImobnRemReq,
        ImobVehRemReqCmd_ImobnRemReq,
        ImobVehRemReqCmd_SpdLimRemReq,
        ImobVehRemReqCmd_ImobnRemChkReq,
        ImobVehRemReqCmd_ImobnRemStsReq
    } ImobVehRemReqCmd;
    /**
     * \brief ErsRequest_t
     * This is a structure, and send it to CEM through CAN network.
     */
    typedef struct {
        vc::ErsCmd ersCmd;
        ErsRunTime ersRunTime;  // range: (0 ~ 59min)
        ImobVehRemReqCmd imobVehRemReqCmd;
        //include authentication default value.
    }ErsRequest_t;

    typedef enum{
        ErsResponseType_Start = 0,
        ErsResponseType_Stop,
        ErsResponseType_Notify,
        ErsResponseType_Error
    } ErsResponseType_e;

    /**
     * \brief ErsResponse_t
     */
    typedef struct{
        ErsResponseType_e type_;
        bool operationSucceeded;
        long errorCode;
        long vehicleErrorCode  = RMT_VEHICLE_ERROR_CODE_DEFAULT;   //the error code from vehicle/TEM, OEM specific
        std::string message;      //OPTIONAL -- the addtional error description(0 ... 255)

        vc::ResRMTStatus rmt_status;  //vehicle states of doors, windows, locks etc.
        vc::CarUsageModeState usgeMode_state;  //usage mode state
        vc::ErsStrtApplSts engine_state;   //engine current state
        int speed;
    }ErsResponse_t;

class GlyVdsResSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     uia93888, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    GlyVdsResSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);


    ////////////////////////////////////////////////////////////
    // @brief :create res signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     uia93888, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    static std::shared_ptr<fsm::Signal> CreateGlyVdsResSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);
    
    ~GlyVdsResSignal ();
    
    std::string ToString() {return "GlyVdsResSignal"; }

    ////////////////////////////////////////////////////////////
    // @brief :GetSignal
    // @return     fsm::Signal *
    // @author     uia93888, 12-Oct-2018
    ////////////////////////////////////////////////////////////
    fsm::Signal *GetSignal()
    {
        return this;
    }

    ////////////////////////////////////////////////////////////
    // @brief :PackGeelyAsn
    // @return     fsm::Signal *
    // @author     uia93888, 5-Mar-2018
    ////////////////////////////////////////////////////////////
    virtual int PackGeelyAsn(void *vdServiceRequest);

    ////////////////////////////////////////////////////////////
    // @brief :pack ResServiceResult_t into ASN1C structure.
    // @param[in]  response, ResServiceResult_t structure .
    //
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 04-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(ErsResponse_t response);

    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SetHeadPayload();

    ////////////////////////////////////////////////////////////
    // @brief :get packed ASN1C structure.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     uia93888, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void *GetPackPayload()
    {
        return (void *)NULL;
    }

    ////////////////////////////////////////////////////////////
    // @brief :get unpacked ASN1C structure.
    // @return     unpacked structure
    // @author     uia93888, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    std::shared_ptr<ErsRequest_t> GetRequestPayload();

    ////////////////////////////////////////////////////////////
    // @brief :get operation command.
    // @return     unpacked structure
    // @author     uia93888, 23-Oct-2018
    ////////////////////////////////////////////////////////////
    vc::ErsCmd GetOperationCommand();

 private:
    ////////////////////////////////////////////////////////////
    // @brief :unpack ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool UnpackPayload(void* vdsServiceRequest);

 private:

    std::shared_ptr<fsm::RMT_EngineStartResult_t>  m_pRMT_EngineStrtRes;
    std::shared_ptr<fsm::RMT_EngineStopResult_t>  m_pRMT_EngineStopRes;
    std::shared_ptr<fsm::RMT_EngineErrorResult_t>  m_pRMT_EngineErrRes;
    std::shared_ptr<fsm::RMT_EngineNotify_t>  m_pRMT_EngineNotify;
    std::shared_ptr<fsm::RMT_EngineRequest_t>  m_pRMT_EngineReq;    /** m_pVdServiceRequest is the response parameters from TSP service */
    std::shared_ptr<ErsRequest_t> m_pErsRequest;          /** m_pErsRequest is the request, and used to send to CAN-network*/
};

class AppEngineStatRequestSignal: public fsm::Signal
{
public:
    AppEngineStatRequestSignal():
            fsm::Signal(fsm::InternalSignalTransactionId(),
            VocInternalSignalTypes::kAppStartEngineRequestSignal)
    {
    }
    std::string ToString()
    {
        return "AppEngineStatRequestSignal";
    }

    ////////////////////////////////////////////////////////////
    // @brief: SetValue.
    //
    // @param[in]     int
    // @param[in]     ServiceCmd
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    void SetValue(ErsRunTime run_time, vc::ErsCmd cmd)
    {
        m_pErsRequest.reset();
        m_pErsRequest = std::make_shared<ErsRequest_t>();
        m_pErsRequest->ersRunTime = run_time;
        m_pErsRequest->ersCmd = cmd;
    }

    ////////////////////////////////////////////////////////////
    // @brief :GetRequestPayload, get request
    // @return     unpacked structure
    // @author     uia93888, Jan-28-2019
    ////////////////////////////////////////////////////////////
    std::shared_ptr<ErsRequest_t> GetRequestPayload()
    {
        return m_pErsRequest;
    }

private:
    std::shared_ptr<ErsRequest_t> m_pErsRequest;
};

class AppEngineStatResponseSignal: public fsm::Signal
{
public:
    AppEngineStatResponseSignal():
            fsm::Signal(fsm::InternalSignalTransactionId(),
            VocInternalSignalTypes::kAppStartEngineResponseSignal),
            m_operate_status(false),
            m_ErsStartApplSts(vc::ErsStrtApplSts_ErsStsOff),
            m_EngStlWdSts(vc::EngSt1_RunngRemStrtd),
            m_errorCode(-1),
            m_vehicleErrorCode(-1)
    {
    }
    std::string ToString()
    {
        return "AppEngineStatResponseSignal";
    }

    ////////////////////////////////////////////////////////////
    // @brief :SetOperateStatus.
    //
    // @param[in]     bool
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    void SetOperateStatus(bool operate_status) { m_operate_status =       operate_status; }

    ////////////////////////////////////////////////////////////
    // @brief :SetErsStartApplSts.
    //
    // @param[in]  vc::ErsStrtApplSt
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    void SetErsStartApplSts(vc::ErsStrtApplSts ersStrtApplSts) { m_ErsStartApplSts = ersStrtApplSts; }

    ////////////////////////////////////////////////////////////
    // @brief :SetEngStlWdSts.
    // 
    // @param[in] vc::EngStlWdSts
    // @author    uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    void SetEngStlWdSts(vc::EngStlWdSts engStlWdSts) { m_EngStlWdSts = engStlWdSts; }

    void SetErrorCode(long error_code){ m_errorCode = error_code; }

    void SetVehicleErrorCode(long vehicle_error_code){ m_vehicleErrorCode = vehicle_error_code; }

    ////////////////////////////////////////////////////////////
    // @brief :GetOperateStatus.
    //
    // @return     bool
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    bool GetOperateStatus() { return m_operate_status; }

    ////////////////////////////////////////////////////////////
    // @brief :GetErsStrtApplSts.
    //
    // @return     vc::ErsStrtApplSts
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    vc::ErsStrtApplSts GetErsStrtApplSts() { return m_ErsStartApplSts; }

    ////////////////////////////////////////////////////////////
    // @brief :GetEngStlWdSts.
    //
    // @return     vc::EngStlWdSts
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    vc::EngStlWdSts GetEngStlWdSts() { return m_EngStlWdSts; }

    ////////////////////////////////////////////////////////////
    // @brief :GetEngStlWdSts.
    //
    // @return     vc::EngStlWdSts
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    long GetErrorCode() { return m_errorCode; }

    ////////////////////////////////////////////////////////////
    // @brief :GetEngStlWdSts.
    //
    // @return     vc::EngStlWdSts
    // @author     uia93888, Jan 25, 2019
    ////////////////////////////////////////////////////////////
    long GetVehicleErrorCode() { return m_vehicleErrorCode; }

private:
    bool m_operate_status;
    vc::ErsStrtApplSts m_ErsStartApplSts;
    vc::EngStlWdSts m_EngStlWdSts;
    long m_errorCode;
    long m_vehicleErrorCode;
};


} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

// \}    end of addtogroup
