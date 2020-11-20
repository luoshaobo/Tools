///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_steerwhlHeat.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   26-Jan-2018
///////////////////////////////////////////////////////////////////

#include "remote_start/remote_start_steerwhlHeat.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call{

RMT_SteerWhlHeat::RMT_SteerWhlHeat():
    RMTStartBasic()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat:: %s, construct.", __FUNCTION__);
}

RMT_SteerWhlHeat::~RMT_SteerWhlHeat()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat:: %s, construct.", __FUNCTION__);
}

bool RMT_SteerWhlHeat::Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat:: %s.", __FUNCTION__);
    std::shared_ptr<fsm::OperateRMTSteerWhlHeatSignal> _res_signal = std::static_pointer_cast<fsm::OperateRMTSteerWhlHeatSignal>(signal);
    vc::ReturnValue vc_return_value = _res_signal->GetVcStatus();
    if (vc::RET_OK != vc_return_value){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat::%s, operate remote steer wheel heat failed", __FUNCTION__);
        HandleFailed(kExecutionFailure);
        return false;
    }
    std::shared_ptr<RMT_ClimatizationRequest_t> request = m_pGlyVdsRceSignal->GetRequestPayload();
    RMT_ClimatizationResponse_t res;
    res.m_serviceType = RMT_SEATVENTI;
    if (CMD_Start == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat::%s. start success, time: %d", __FUNCTION__, request->m_duration_time);
        SetState(kWaitingStop);
        m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request->m_duration_time), false);
        res.m_operateSuccess = true;
        res.m_resType = ResType_StrtStop;
    } else if (CMD_Stop == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat::%s. tsp stop success", __FUNCTION__);
        SetState(kDone);
        res.m_resType = ResType_StrtStop;
        res.m_operateSuccess = true;
    } else if (CMD_TimeoutStop == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat::%s. CMD_TimeoutStop success", __FUNCTION__);
        SetState(kDone);
        res.m_resType = ResType_AutoStop;
        res.m_operateSuccess = true;
    }
    SendResponseToTSP(res);
    return true;
}

bool RMT_SteerWhlHeat::SendResponseToTSP(RMT_ClimatizationResponse_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat:: %s.", __FUNCTION__);
    RemoveTimeout(m_TSPTimeoutId);
    response.m_serviceType = RMT_Climatization;
    if (!m_pGlyVdsRceSignal->SetResultPayload(response)){
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat:: %s, set response message failed", __FUNCTION__);
        return false;
    }
    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsRceSignal))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_SteerWhlHeat::%s, success", __FUNCTION__);
    } else {
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SteerWhlHeat::%s, failed", __FUNCTION__);
    }
    return true;
}

bool RMT_SteerWhlHeat::SendRequest(std::shared_ptr<RMT_ClimatizationRequest_t> request)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat:: %s.", __FUNCTION__);
    bool ret = false;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
        std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);
    vc::Req_OperateRMTSteerWhlHeat_S req;
    req.SteerWhlHeatgDurgClimaEnadFromTelm = 0;
    switch(request->m_operateCmd)
    {
    /**
     * @brief  0: off, 1: On
     **/
        case CMD_NoReq:{
            m_cmd = CMD_NoReq;
            ret = true;
            break;
        }
        case CMD_Stop:{
            m_cmd = CMD_Stop;
            req.SteerWhlHeatgDurgClimaEnadFromTelm = 0;
            ret = true;
            break;
        }
        case CMD_Start:{
            m_cmd = CMD_Start;
            req.SteerWhlHeatgDurgClimaEnadFromTelm = 1;
            ret = true;
            break;
        }
        case CMD_TimeoutStop:{
            m_cmd = CMD_TimeoutStop;
            req.SteerWhlHeatgDurgClimaEnadFromTelm = 0;
            ret = true;
            break;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SteerWhlHeat:: %s, UNKNOW Operate CMD type", __FUNCTION__);
            return ret;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SteerWhlHeat:: %s, SteerWhlHeatgDurgClimaEnadFromTelm:%d", __FUNCTION__, req.SteerWhlHeatgDurgClimaEnadFromTelm);
    if ( vc::RET_OK == m_igen.Request_RMTSteerWhlHeat(&req, vehicle_comm_request_id->GetId()))
    {
        SetState(kHandleRequest);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat::%s, SUCCESS, state: %d", __FUNCTION__, GetState());
        ret = true;
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SteerWhlHeat::%s, FAILED", __FUNCTION__);
        HandleFailed(kExecutionFailure);
    }
    return true;
}


}