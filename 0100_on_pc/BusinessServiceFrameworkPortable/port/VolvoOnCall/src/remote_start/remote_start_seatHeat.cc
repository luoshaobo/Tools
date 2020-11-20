///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_seatHeat.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   26-Jan-2018
///////////////////////////////////////////////////////////////////

#include "remote_start/remote_start_seatHeat.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call{

RMT_SeatHeat::RMT_SeatHeat():
    RMTStartBasic()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s, construct.", __FUNCTION__);
}

RMT_SeatHeat::~RMT_SeatHeat()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s, destroy.", __FUNCTION__);
}

bool RMT_SeatHeat::Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s.", __FUNCTION__);
#if 0
    std::shared_ptr<fsm::OperateRMTClimateSignal> _res_signal = std::static_pointer_cast<fsm::OperateRMTClimateSignal>(signal);
    vc::ReturnValue vc_return_value = _res_signal->GetVcStatus();
    if (vc::RET_OK != vc_return_value){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat::%s, operate remote climate failed", __FUNCTION__);
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
    std::shared_ptr<RMT_ClimatizationRequest_t> request = m_pGlyVdsRceSignal->GetRequestPayload();
    RMT_ClimatizationResponse_t res;
    res.m_serviceType = RMT_SEATHEAT;
    if (CMD_Start == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat::%s. start success, time: %d", __FUNCTION__, request->m_duration_time);
        SetState(kWaitingStop);
        m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request->m_duration_time), false);
        res.m_operateSuccess = true;
        res.m_resType = ResType_StrtStop;
    } else if (CMD_Stop == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat::%s. tsp stop success", __FUNCTION__);
        SetState(kDone);
        res.m_resType = ResType_StrtStop;
        res.m_operateSuccess = true;
    } else if (CMD_TimeoutStop == m_cmd){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat::%s. CMD_TimeoutStop success", __FUNCTION__);
        SetState(kDone);
        res.m_resType = ResType_AutoStop;
        res.m_operateSuccess = true;
    }
    SendResponseToTSP(res);
#endif
    return true;
}

bool RMT_SeatHeat::SendResponseToTSP(RMT_ClimatizationResponse_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s.", __FUNCTION__);
    RemoveTimeout(m_TSPTimeoutId);
    response.m_serviceType = RMT_Climatization;
    if (!m_pGlyVdsRceSignal->SetResultPayload(response)){
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s, set response message failed", __FUNCTION__);
        return false;
    }
    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsRceSignal))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_SeatHeat::%s, success", __FUNCTION__);
    } else {
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatHeat::%s, failed", __FUNCTION__);
    }
    return true;
}

bool RMT_SeatHeat::SendRequest(std::shared_ptr<RMT_ClimatizationRequest_t> request)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s.", __FUNCTION__);
    bool ret = false;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
        std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);
    SetState(kHandleRequest);
    vc::Req_OperateRMTSeatHeat req;
    req.telmSeatDrvHeat   = vc::TelmSeatCliamtLvl_Closed;
    req.telmSeatPassHeat  = vc::TelmSeatCliamtLvl_Closed;
    req.telmSeatSecLeHeat = vc::TelmSeatCliamtLvl_Closed;
    req.telmSeatSecRiHeat = vc::TelmSeatCliamtLvl_Closed;
    switch(request->m_operateCmd)
    {
    /**
     * @brief  1    1    1    1    1   1    1    0
     *       frLe frRi MiLe MiMi MiRi BaLe BaRi StWe
     *         1    1    1    0    1
     **/
        case CMD_NoReq:{
            m_cmd = CMD_NoReq;
            //req.telmClimaReq = vc::OnOffNoReq_NoReq;
            ret = true;
            break;
        }
        case CMD_Stop:{
            m_cmd = CMD_Stop;
            SetSeat(req, request->m_RceHeat, request->m_level);
            ret = true;
            break;
        }
        case CMD_Start:{
            m_cmd = CMD_Start;
            SetSeat(req, request->m_RceHeat, request->m_level);
            ret = true;
            break;
        }
        case CMD_TimeoutStop:{
            m_cmd = CMD_TimeoutStop;
            req.telmSeatDrvHeat   = vc::TelmSeatCliamtLvl_Closed;
            req.telmSeatPassHeat  = vc::TelmSeatCliamtLvl_Closed;
            req.telmSeatSecLeHeat = vc::TelmSeatCliamtLvl_Closed;
            req.telmSeatSecRiHeat = vc::TelmSeatCliamtLvl_Closed;
            ret = true;
            break;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatHeat:: %s, UNKNOW Operate CMD type", __FUNCTION__);
            return ret;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatHeat:: %s, telmSeatDrvHeat:%d", __FUNCTION__, req.telmSeatDrvHeat);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatHeat:: %s, telmSeatPassHeat:%d", __FUNCTION__, req.telmSeatPassHeat);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatHeat:: %s, telmSeatSecLeHeat:%d", __FUNCTION__, req.telmSeatSecLeHeat);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_SeatHeat:: %s, telmSeatSecRiHeat:%d", __FUNCTION__, req.telmSeatSecRiHeat);
    if ( vc::RET_OK == m_igen.Request_RMTSeatHeat(&req, vehicle_comm_request_id->GetId()))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat::%s, SUCCESS, state: %d", __FUNCTION__, GetState());
        ret = true;
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat::%s, FAILED", __FUNCTION__);
        HandleFailed(kExecutionFailure);
    }
    return true;
}

bool RMT_SeatHeat::SetSeat(vc::Req_OperateRMTSeatHeat& req, int32_t seatHeat, int level)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_SeatHeat:: %s. seatHeat: %d, level:%d", __FUNCTION__, seatHeat, level);
    for(int i = 0; i < 8; i++){
        if(7 == i){ //front left seat
            if (1 == (seatHeat % 10)){
                req.telmSeatDrvHeat   = (vc::TelmSeatCliamtLvls)level;
            }
        } else if (6 == i){//front right seat
            if (1 == (seatHeat % 10)){
                req.telmSeatPassHeat   = (vc::TelmSeatCliamtLvls)level;
            }
        } else if (5 == i){//Middle left seat
            if (1 == (seatHeat % 10)){
                req.telmSeatSecLeHeat   = (vc::TelmSeatCliamtLvls)level;
            }
        } else if (3 == i){//Middle right seat
            if (1 == (seatHeat % 10)){
                req.telmSeatSecRiHeat   = (vc::TelmSeatCliamtLvls)level;
            }
        }
        seatHeat = seatHeat / 10;
        if (0 == seatHeat)
            break;
    }
    return true;
}

}