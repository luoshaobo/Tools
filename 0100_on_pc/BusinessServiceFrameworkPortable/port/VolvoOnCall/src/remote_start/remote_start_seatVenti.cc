///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_seatVenti.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   26-Jan-2018
///////////////////////////////////////////////////////////////////

#include "remote_start/remote_start_seatVenti.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{
RMT_SeatVenti::RMT_SeatVenti():
	RMTStartBasic()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s, construct.", __FUNCTION__);
}

RMT_SeatVenti::~RMT_SeatVenti()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s, destruct.", __FUNCTION__);
}

bool RMT_SeatVenti::Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s.", __FUNCTION__);
    std::shared_ptr<fsm::OperateRMTSeatHeatSignal> _res_signal = std::static_pointer_cast<fsm::OperateRMTSeatHeatSignal>(signal);
    vc::ReturnValue vc_return_value = _res_signal->GetVcStatus();
    if (vc::RET_OK != vc_return_value){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti::%s, operate remote climate failed", __FUNCTION__);
        HandleFailed(kExecutionFailure);
        return false;
    }
    std::shared_ptr<RMT_ClimatizationRequest_t> request = m_pGlyVdsRceSignal->GetRequestPayload();
    RMT_ClimatizationResponse_t res;
    res.m_serviceType = RMT_SEATVENTI;
    if (CMD_Start == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti::%s. start success, time: %d", __FUNCTION__, request->m_duration_time);
        SetState(kWaitingStop);
        m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request->m_duration_time), false);
        res.m_operateSuccess = true;
        res.m_resType = ResType_StrtStop;
    } else if (CMD_Stop == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti::%s. tsp stop success", __FUNCTION__);
        SetState(kDone);
        res.m_resType = ResType_StrtStop;
        res.m_operateSuccess = true;
    } else if (CMD_TimeoutStop == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti::%s. CMD_TimeoutStop success", __FUNCTION__);
        SetState(kDone);
        res.m_resType = ResType_AutoStop;
        res.m_operateSuccess = true;
    }
    SendResponseToTSP(res);
    return true;
}

bool RMT_SeatVenti::SendResponseToTSP(RMT_ClimatizationResponse_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s.", __FUNCTION__);
    RemoveTimeout(m_TSPTimeoutId);
    response.m_serviceType = RMT_Climatization;
    if (!m_pGlyVdsRceSignal->SetResultPayload(response)){
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s, set response message failed", __FUNCTION__);
        return false;
    }
    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsRceSignal))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_SeatVenti::%s, success", __FUNCTION__);
    } else {
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatVenti::%s, failed", __FUNCTION__);
    }
    return true;
}

bool RMT_SeatVenti::SendRequest(std::shared_ptr<RMT_ClimatizationRequest_t> request)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s.", __FUNCTION__);
    bool ret = false;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
        std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);
    vc::Req_OperateRMTSeatVenti req;
    req.telmSeatDrvVenti   = vc::TelmSeatCliamtLvl_Closed;
    req.telmSeatPassVenti  = vc::TelmSeatCliamtLvl_Closed;
    req.telmSeatSecLeVenti = vc::TelmSeatCliamtLvl_Closed;
    req.telmSeatSecRiVenti = vc::TelmSeatCliamtLvl_Closed;
    switch(request->m_operateCmd)
    {
    /**
     * @brief  1    1    1    1    1   1    1    0
     *       frLe frRi MiLe MiMi MiRi BaLe BaRi StWe
     *         1    1    1    0    1
     **/
        case CMD_NoReq:{
            m_cmd = CMD_NoReq;
            ret = true;
            break;
        }
        case CMD_Stop:{
            m_cmd = CMD_Stop;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s, request->m_RceVenti:%d", __FUNCTION__, request->m_RceVenti);
            SetSeat(req, request->m_RceVenti, request->m_level);
            ret = true;
            break;
        }
        case CMD_Start:{
            m_cmd = CMD_Start;
            SetSeat(req, request->m_RceVenti, request->m_level);
            ret = true;
            break;
        }
        case CMD_TimeoutStop:{
            m_cmd = CMD_TimeoutStop;
            req.telmSeatDrvVenti   = vc::TelmSeatCliamtLvl_Closed;
            req.telmSeatPassVenti  = vc::TelmSeatCliamtLvl_Closed;
            req.telmSeatSecLeVenti = vc::TelmSeatCliamtLvl_Closed;
            req.telmSeatSecRiVenti = vc::TelmSeatCliamtLvl_Closed;
            ret = true;
            break;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatVenti:: %s, UNKNOW Operate CMD type", __FUNCTION__);
            return ret;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatVenti:: %s, telmSeatDrvVenti:%d", __FUNCTION__, req.telmSeatDrvVenti);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatVenti:: %s, telmSeatPassVenti:%d", __FUNCTION__, req.telmSeatPassVenti);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatVenti:: %s, telmSeatSecLeVenti:%d", __FUNCTION__, req.telmSeatSecLeVenti);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatVenti:: %s, telmSeatSecRiVenti:%d", __FUNCTION__, req.telmSeatSecRiVenti);
    if ( vc::RET_OK == m_igen.Request_RMTSeatVentilation(&req, vehicle_comm_request_id->GetId()))
    {
        SetState(kHandleRequest);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti::%s, SUCCESS, state: %d", __FUNCTION__, GetState());
        ret = true;
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti::%s, FAILED", __FUNCTION__);
        HandleFailed(kExecutionFailure);
    }
    return true;
}

bool RMT_SeatVenti::SetSeat(vc::Req_OperateRMTSeatVenti& req, int32_t seatVenti, int level)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatVenti:: %s. seatVenti:%d, level:%d", __FUNCTION__, seatVenti, level);
    for(int i = 0; i < 8; i++){
        if(7 == i){ //front left seat
            if (1 == (seatVenti % 10)){
                req.telmSeatDrvVenti   = (vc::TelmSeatCliamtLvls)level;
            }
        } else if (6 == i){//front right seat
            if (1 == (seatVenti % 10)){
                req.telmSeatPassVenti   = (vc::TelmSeatCliamtLvls)level;
            }
        } else if (5 == i){//Middle left seat
            if (1 == (seatVenti % 10)){
                req.telmSeatSecLeVenti   = (vc::TelmSeatCliamtLvls)level;
            }
        } else if (3 == i){//Middle right seat
            if (1 == (seatVenti % 10)){
                req.telmSeatSecRiVenti   = (vc::TelmSeatCliamtLvls)level;
            }
        }
        seatVenti = seatVenti / 10;
        if (0 == seatVenti)
            break;
    }
    return true;
}

}