///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    @file rcc_signal_adapt.cc
//    geely rcc signal.

// @project        GLY_TCAM
// @subsystem    fsm
// @author        uia93888
// @Init date    20-Mar-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/rce_signal_adapt.h"
#include "voc_framework/signal_adapt/rcc_signal_adapt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm)

namespace fsm{

RccSignalAdapt::RccSignalAdapt(void* vdsObj): SignalAdaptBase(vdsObj)
{
}

bool RccSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s\n", __FUNCTION__);
    if (nullptr == vdsData){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN, "RccSignalAdapt::%s, nullptr == vdsdata\n", __FUNCTION__);
        return false;
    }
    appData = new fsm::Parking_ClimateRequest_t();
    fsm::Parking_ClimateRequest_t* _req = dynamic_cast<fsm::Parking_ClimateRequest_t*>(appData);
    if (nullptr == _req){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN, "RccSignalAdapt::%s, nullptr == _req\n", __FUNCTION__);
        return false;
    }
    _req->serviceId = vdsData->body.serviceId;
    _req->serviceCommand = vdsData->body.serviceData.serviceCommand;
    if(_req->serviceCommand == app_terminateService)
    {//stop parking climate
        _req->timerId = vdsData->body.serviceData.timerData.timerId;
        return true;
    }
    if (0 == (vdsData->body.serviceData.serviceParameters).size()){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s, _serviceParams doesn't have elements\n", __FUNCTION__);
        return true;
    }
    for(ServiceParameter_Model &iter: vdsData->body.serviceData.serviceParameters){
        if (REC_TEMP == iter.key){
            _req->rec_temp = iter.intVal;
        } else if (REC_LEVEL == iter.key){
            _req->rec_Level = iter.intVal;
        } else if (REC_HEAT == iter.key){
            (_req->rec_heat).push_back(iter.intVal);
        }
    }
    if (0 == (vdsData->body.serviceData.timerData.timers).size()){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s, _serviceParams doesn't have elements\n", __FUNCTION__);
        return false;
    }
    for(TimerInfo_Model& iter: vdsData->body.serviceData.timerData.timers){
        Parking_ClimateTimerInfo _timerinfo;
        _timerinfo.timerActivation = iter.timerActivation;
        _timerinfo.duration = iter.duration;
        _timerinfo.dayofWeek = iter.dayofWeek;
        _timerinfo.startTimeofDay = iter.startTimeofDay;
        _req->timers.push_back(_timerinfo);
    }
    return true;
}

bool RccSignalAdapt::SetStrtStopResult(const fsm::Parking_CliamteStrtStopResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_rcc;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSuccessed;

    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.climateStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.climateStatus.preClimateActive_presence = true;
    vdsData.body.serviceData.vehicleStatus.climateStatus.preClimateActive = result.preClimateActive;

    if (PackService(vdsData, result)){ // success
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s PackService success.\n", __FUNCTION__);
    } else { // failed
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool RccSignalAdapt::SetErrorResult(const fsm::Parking_ErrorResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_rcc;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.serviceResult.error_presence = true;
    vdsData.body.serviceData.serviceResult.error.code = (Vds_ErrorCode)result.errorCode;

    if(-1 != result.vehicleErrorCode ){
        vdsData.body.serviceData.serviceResult.error.vehicleErrorCode_presence = true;
        vdsData.body.serviceData.serviceResult.error.vehicleErrorCode = result.vehicleErrorCode;
    }

    if (PackService(vdsData, result)){ // success
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s PackService success.\n", __FUNCTION__);
    } else { // failed
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RccSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return false;
    }
    return true;
}

}