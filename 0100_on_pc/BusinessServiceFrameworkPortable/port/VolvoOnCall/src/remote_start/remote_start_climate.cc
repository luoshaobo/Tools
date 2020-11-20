// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_climate.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   26-Jan-2018
///////////////////////////////////////////////////////////////////

#include "remote_start/remote_start_climate.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call{
    RMT_Climate::RMT_Climate():
        RMTStartBasic()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate:: %s.", __FUNCTION__);
    }

    RMT_Climate::~RMT_Climate()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate:: %s. destroy", __FUNCTION__);
    }

    bool RMT_Climate::Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate:: %s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<RMT_ClimatizationRequest_t> request = m_pGlyVdsRceSignal->GetRequestPayload();
#if 0
        std::shared_ptr<fsm::OperateRMTClimateSignal> _res_signal = std::static_pointer_cast<fsm::OperateRMTClimateSignal>(signal);
        vc::ReturnValue vc_return_value = _res_signal->GetVcStatus();
        if (vc::RET_OK != vc_return_value){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, operate remote climate failed", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        if ((vc::OnOff1_On == _res_signal->GetData()->remStrtClimaActv) &&
            (vc::ClimaWarn_NoWarn == _res_signal->GetData()->prkgClimaWarn))
        {
            RMT_ClimatizationResponse_t res;
            if (CMD_Start == m_cmd){
                SetState(kWaitingStop);
                m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request.m_duration_time), false);
                res.m_operateSuccess = true;
                res.m_active = _res_signal->GetData()->remStrtClimaActv;
            } else if ((CMD_Stop == m_cmd) || (CMD_TimeoutStop == m_cmd) ){
                SetState(kDone);
                res.m_operateSuccess = true;
            }
            SendResponseToTSP(res);
            ret = true;
        } else {
            HandleFailed(kExecutionFailure);
        }
#elif 1
        RMT_ClimatizationResponse_t res;
        res.m_serviceType = RMT_Climatization;
        if (CMD_Start == m_cmd){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. start success, time: %d", __FUNCTION__, request->m_duration_time);
            SetState(kWaitingStop);
            m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request->m_duration_time), false);
            res.m_operateSuccess = true;
            res.m_resType = ResType_StrtStop;
            res.m_active = vc::OnOff1_On;
        } else if (CMD_Stop == m_cmd){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. tsp stop success", __FUNCTION__);
            SetState(kDone);
            res.m_resType = ResType_StrtStop;
            res.m_operateSuccess = true;
        } else if (CMD_TimeoutStop == m_cmd){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. CMD_TimeoutStop success", __FUNCTION__);
            SetState(kDone);
            res.m_resType = ResType_AutoStop;
            res.m_operateSuccess = true;
        }
        SendResponseToTSP(res);
        ret = true;
#endif
        return ret;
    }

    bool RMT_Climate::SendResponseToTSP(RMT_ClimatizationResponse_t response)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate:: %s.", __FUNCTION__);
        RemoveTimeout(m_TSPTimeoutId);
        response.m_serviceType = RMT_Climatization;
        if (!m_pGlyVdsRceSignal->SetResultPayload(response)){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate:: %s, set response message failed", __FUNCTION__);
            return false;
        }
        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsRceSignal))
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_Climate::%s, success", __FUNCTION__);
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_Climate::%s, failed", __FUNCTION__);
        }
        return true;
    }

    bool RMT_Climate::SendRequest(std::shared_ptr<RMT_ClimatizationRequest_t> request)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate:: %s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        SetState(kHandleRequest);
        vc::Req_OperateRMTClimate req;
        switch(request->m_operateCmd)
        {
        case CMD_NoReq:{
                m_cmd = CMD_NoReq;
                req.telmClimaReq = vc::OnOffNoReq_NoReq;
                ret = true;
            }
            return ret;
        case CMD_Stop:{
                m_cmd = CMD_Stop;
                req.telmClimaReq = vc::OnOffNoReq_Off;
                //req.telmClimaTSetTempRange = 0;
                //req.hmiCmptmtTSpSpcl = vc::HmiCmptmtSpSpcl_Norm;
                ret = true;
            }
            break;
        case CMD_Start:{
                m_cmd = CMD_Start;
                req.telmClimaReq = vc::OnOffNoReq_On;
                req.telmClimaTSetTempRange = request->m_telemClimaTemperature;
                if (TCAM_APP_RMOTE_CLIMATE_TEMP_LEVEL_LO == request->m_telemClimaTemperature){
                    req.hmiCmptmtTSpSpcl = 1;
                } else if (TCAM_APP_RMOTE_CLIMATE_TEMP_LEVEL_HI == request->m_telemClimaTemperature){
                    req.hmiCmptmtTSpSpcl = 2;
                } else {
                    req.hmiCmptmtTSpSpcl = 0;
                }
                ret = true;
            }
            break;
        case CMD_TimeoutStop:{
                m_cmd = CMD_TimeoutStop;
                req.telmClimaReq = vc::OnOffNoReq_Off;
                //req.telmClimaTSetTempRange = 0;
                //req.hmiCmptmtTSpSpcl = vc::HmiCmptmtSpSpcl_Norm;
                ret = true;
            }
            break;
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_Climate:: %s, UNKNOW Operate CMD type", __FUNCTION__);
            return ret;
        }

        if ( vc::RET_OK == m_igen.Request_RemoteClimateOperate(&req, vehicle_comm_request_id->GetId()))
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate::%s, SUCCESS, state: %d", __FUNCTION__, GetState());
            ret = true;
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Climate::%s, FAILED", __FUNCTION__);
            HandleFailed(kExecutionFailure);
        }
        //TODO
        return ret;
    }
}