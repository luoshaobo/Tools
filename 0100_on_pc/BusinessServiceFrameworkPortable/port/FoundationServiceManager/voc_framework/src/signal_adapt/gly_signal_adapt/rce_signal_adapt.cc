///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file rce_signal_adapt.cc

// @project     GLY_TCAM
// @subsystem   FSM
// @author      uis93888
// @Init date   05-Mar-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/rce_signal_adapt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm)
using namespace fsm;

RceSignalAdapt::RceSignalAdapt(void* vdsObj): SignalAdaptBase(vdsObj)
{

}

bool RceSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s\n", __FUNCTION__);

    if (nullptr == vdsData){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN, "RceSignalAdapt::%s, nullptr == vdsdata\n", __FUNCTION__);
        return false;
    }
    appData = new fsm::RMT_CliamtizationRequest_t();
    fsm::RMT_CliamtizationRequest_t* _req = dynamic_cast<fsm::RMT_CliamtizationRequest_t*>(appData);
    if (nullptr == _req){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN, "RceSignalAdapt::%s, nullptr == _req\n", __FUNCTION__);
        return false;
    }
    _req->serviceId = vdsData->body.serviceId;
    _req->serviceCommand = vdsData->body.serviceData.serviceCommand;
    if(_req->serviceCommand != app_terminateService)
    {
        _req->interval = (vdsData->body.serviceData.serviceTrigger.duration) * 10;// _req->interval unit: min
    }
    
    //std::vector<fsm::ServiceParameter_Data> _serviceParams = vdsData->body.serviceData.serviceParameters;
    //if (0 == _serviceParams.size()){
    if (0 == (vdsData->body.serviceData.serviceParameters).size()){
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s, _serviceParams doesn't have elements\n", __FUNCTION__);
        return true;
    }
    
    //std::vector<fsm::ServiceParameter_Data>::iterator iter = (vdsData->body.serviceData.serviceParameters).begin();
    //for(; iter < (vdsData->body.serviceData.serviceParameters).end(); iter++){
    for(ServiceParameter_Model &iter: vdsData->body.serviceData.serviceParameters){
        if (REC_TEMP == iter.key){
            _req->rec_temp = iter.intVal;
        } else if (REC_LEVEL == iter.key){
            _req->rec_Level = iter.intVal;
        } else if (REC_HEAT == iter.key){
            _req->rec_heat = iter.intVal;
        } else if (REC_VENTI == iter.key){
            _req->rec_venti = iter.intVal;
        }
    }
    return true;
}

bool RceSignalAdapt::SetStrtStopResultPayload(const fsm::RMT_ClimateSrtStopResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_rce;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus = (Vds_EngineStatus)result.engine_status;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speed = result.speed;

    vdsData.body.serviceData.vehicleStatus.electricStatus_presence = true;
    // @brief 0x0: Undefined, 0x1:failed, 0x2:Successful, 0x3:Reserved
    vdsData.body.serviceData.vehicleStatus.electricStatus.chargeHvSts_presence = true;
    vdsData.body.serviceData.vehicleStatus.electricStatus.chargeHvSts = result.chargeHvSts;

    if (PackService(vdsData, result)){ // success
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s PackService success.\n", __FUNCTION__);
    } else { // failed
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool RceSignalAdapt::SetAutoStopResultPayload(const fsm::RMT_CliamteAutoStopResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_rce;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.climateStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.climateStatus.preClimateActive_presence = true;
    vdsData.body.serviceData.vehicleStatus.climateStatus.preClimateActive = result.preClimateActive;

    if (PackService(vdsData, result)){ // success
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s PackService success.\n", __FUNCTION__);
    } else { // failed
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool RceSignalAdapt::SetErrorResultPayload(const fsm::RMT_RemoteStartCommErrorResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_rce;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.serviceResult.error_presence = true;
    vdsData.body.serviceData.serviceResult.error.code = (Vds_ErrorCode)result.errorCode;

    if(-1 !=result.vehicleErrorCode ){
        vdsData.body.serviceData.serviceResult.error.vehicleErrorCode_presence = true;
        vdsData.body.serviceData.serviceResult.error.vehicleErrorCode = result.vehicleErrorCode;
    }

    if (PackService(vdsData, result)){ // success
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RceSignalAdapt::%s PackService success.\n", __FUNCTION__);
    } else { // failed
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return false;
    }
    return true;
}