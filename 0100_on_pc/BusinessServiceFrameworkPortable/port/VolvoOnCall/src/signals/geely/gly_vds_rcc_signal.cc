///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    @file gly_vds_rcc_signal.cc
//    geely rcc signal.

// @project        GLY_TCAM
// @subsystem    Application
// @author        uia93888
// @Init date    20-Mar-2019
///////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rcc_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/rcc_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

std::shared_ptr<fsm::Signal> GlyVdsRccSignal::CreateGlyVdsRccSignal(fsm::VdServiceTransactionId& transaction_id, void* vdsService)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s.\n", __FUNCTION__);
    GlyVdsRccSignal* signal = new GlyVdsRccSignal(transaction_id, vdsService);

    return std::shared_ptr<GlyVdsRccSignal>(signal);
}

GlyVdsRccSignal::GlyVdsRccSignal(fsm::VdServiceTransactionId& transaction_id, void* vdsService):
                    fsm::VdmSignal(transaction_id, fsm::kVDServiceRcc)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s.\n", __FUNCTION__);
    bool result = UnpackPayload(vdsService);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s.get res request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "GlyVdsRccSignal::%s.get res request payload fail.\n", __FUNCTION__);
    }
}

GlyVdsRccSignal::~GlyVdsRccSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s.\n", __FUNCTION__);
}

int GlyVdsRccSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s\n", __FUNCTION__);
    if(NULL == vdServiceRequest){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, NULL == vdServiceRequest\n", __FUNCTION__);
        return -1;
    }
    fsm::RccSignalAdapt _rcc_adaptor((void*)vdServiceRequest);
    if(nullptr == m_pStrtStopResult.get()){
        bool ret = _rcc_adaptor.SetStrtStopResult(*m_pStrtStopResult);
        return (ret == true? 0 : -1);
    } else if(nullptr == m_pErrResult.get()){
        bool ret = _rcc_adaptor.SetErrorResult(*m_pErrResult);
        return (ret == true? 0 : -1);
    }
    return -1;
}

bool GlyVdsRccSignal::SetResultPayload(volvo_on_call::Parking_ClimateResult result)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s.\n", __FUNCTION__);
    if (nullptr == m_pRequest.get()){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, nullptr == m_pRequest.get().\n", __FUNCTION__);
        return false;
    }
    RequestHeader_Data _heade = m_pRequest->header;
    if (result.operationSuccessed){
        m_pStrtStopResult.reset(new fsm::Parking_CliamteStrtStopResult_t());
        if (nullptr == m_pStrtStopResult.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, nullptr == m_pStrtStopResult.get().\n", __FUNCTION__);
            return false;
        }
        m_pStrtStopResult->operationSuccessed = result.operationSuccessed;
        m_pStrtStopResult->preClimateActive = result.preClimateActive;
        m_pStrtStopResult->header = _heade;
    } else {
        m_pErrResult.reset(new fsm::Parking_ErrorResult_t());
        if (nullptr == m_pErrResult.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, nullptr == m_pErrResult.get().\n", __FUNCTION__);
            return false;
        }
        m_pErrResult->operationSucceeded = result.operationSuccessed;
        m_pErrResult->errorCode = result.errorCode;
        m_pErrResult->vehicleErrorCode = result.vehicleErrorCode;
        m_pErrResult->message = result.message;
        m_pErrResult->header = _heade;
    }
    return true;
}

std::shared_ptr<Parking_ClimateRequest> GlyVdsRccSignal::GetRequestPayload()
{
    return m_pLocalRequest;
}

bool GlyVdsRccSignal::UnpackPayload(void* vdsServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s.\n", __FUNCTION__);
    fsm::RccSignalAdapt _rcc_adaptor(vdsServiceRequest);
    std::shared_ptr<AppDataBase> appData = _rcc_adaptor.UnpackService();
    m_pRequest = std::dynamic_pointer_cast<fsm::Parking_ClimateRequest_t>(appData);

    if(nullptr == m_pRequest.get())
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, nullptr == m_pRequest.\n", __FUNCTION__);
        return false;
    }

    m_pLocalRequest.reset(new Parking_ClimateRequest());
    if (nullptr == m_pLocalRequest){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, nullptr == m_pLocalRequest.\n", __FUNCTION__);
        return false;
    }

    m_pLocalRequest->serviceId = m_pRequest->serviceId;
    m_pLocalRequest->serviceCommand = (ParkingCliamte_ServiceCmd_E)m_pRequest->serviceCommand;
    m_pLocalRequest->timerId = m_pRequest->timerId;
     if ( 0 == (m_pRequest->timers).size() ){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, 0 == timers.size().\n", __FUNCTION__);
        return false;
    }
    for (fsm::Parking_ClimateTimerInfo& iter: m_pRequest->timers){
        m_pLocalRequest->timerActivation = iter.timerActivation;
        m_pLocalRequest->duration = iter.duration;
        m_pLocalRequest->dayofWeek = iter.dayofWeek;
        m_pLocalRequest->startTimeofDay = iter.startTimeofDay;
    }
    m_pLocalRequest->rec_temp = m_pRequest->rec_temp;
    m_pLocalRequest->rec_Level = m_pRequest->rec_Level;
    if (0 != (m_pRequest->rec_heat).size()){
        int _num = 0;
        for (int& iter: m_pRequest->rec_heat){
            if(0 == _num){
                m_pLocalRequest->rec_heat_1 = iter;
                _num++;
            } else {
                m_pLocalRequest->rec_heat_2 = iter;
            }
        }
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->serviceId:%d. \n", __FUNCTION__, m_pLocalRequest->serviceId);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->serviceCommand:%d. \n", __FUNCTION__, m_pLocalRequest->serviceCommand);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->timerId:%d. \n", __FUNCTION__, m_pLocalRequest->timerId);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->timerActivation:%d. \n", __FUNCTION__, m_pLocalRequest->timerActivation);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->duration:%d. \n", __FUNCTION__, m_pLocalRequest->duration);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->dayofWeek:%s. \n", __FUNCTION__, (m_pLocalRequest->dayofWeek).c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->startTimeofDay:%s. \n", __FUNCTION__, (m_pLocalRequest->startTimeofDay).c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->rec_temp:%d. \n", __FUNCTION__, m_pLocalRequest->rec_temp);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->rec_Level:%d. \n", __FUNCTION__, m_pLocalRequest->rec_Level);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->rec_heat_1:%d. \n", __FUNCTION__, m_pLocalRequest->rec_heat_1);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRccSignal::%s, UnpackPayload request->rec_heat_2:%d. \n", __FUNCTION__, m_pLocalRequest->rec_heat_2);
    return true;
}

}