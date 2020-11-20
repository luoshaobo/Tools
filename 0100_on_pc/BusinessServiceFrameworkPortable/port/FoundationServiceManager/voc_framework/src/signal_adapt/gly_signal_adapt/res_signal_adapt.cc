///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file res_signal_adapt.cc

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
#include "voc_framework/signal_adapt/res_signal_adapt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

ResSignalAdapt::ResSignalAdapt(void* vdsObj): SignalAdaptBase(vdsObj)
{

}

bool ResSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s\n", __FUNCTION__);
    
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new fsm::RMT_EngineRequest_t();
    fsm::RMT_EngineRequest_t *_RMT_EngineData = dynamic_cast<fsm::RMT_EngineRequest_t*>(appData);
    if(_RMT_EngineData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s _RMT_EngineData == nullptr.\n", __FUNCTION__);
        return false;
    }

    _RMT_EngineData->serviceId = vdsData->body.serviceId;
    _RMT_EngineData->serviceCommand = vdsData->body.serviceData.serviceCommand;
    if(_RMT_EngineData->serviceCommand != app_terminateService)
    {
        _RMT_EngineData->interval = (vdsData->body.serviceData.serviceTrigger.duration) * 10;//unit is 10s
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s, serviceId: %d\n", __FUNCTION__, vdsData->body.serviceId);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s, serviceCommand: %d\n", __FUNCTION__, vdsData->body.serviceData.serviceCommand);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s, duration: %d\n", __FUNCTION__, vdsData->body.serviceData.serviceTrigger.duration);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s completed.\n", __FUNCTION__);

    return true;
}

bool ResSignalAdapt::SetStartResultPayload(const fsm::RMT_EngineStartResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_res;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus = (Vds_EngineStatus)result.engine_status;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speed = result.speed;

    vdsData.body.serviceData.vehicleStatus.lockStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusDriver_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusDriver = result.doorOpenStatusDriver;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusDriver_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusDriver = result.doorLockStatusDriver;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusPassenger_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusPassenger = result.doorOpenStatusPassenger;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusPassenger_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusPassenger = result.doorLockStatusPassenger;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusDriverRear_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusDriverRear = result.doorOpenStatusDriverRear;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusDriverRear_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusDriverRear = result.doorLockStatusDriverRear;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusPassengerRear_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorOpenStatusPassengerRear = result.doorOpenStatusPassengerRear;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusPassengerRear_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.doorLockStatusPassengerRear = result.doorLockStatusPassengerRear;
    vdsData.body.serviceData.vehicleStatus.lockStatus.trunkLockStatus_presence = true;
    if(1 == result.trunkLockStatus){
        vdsData.body.serviceData.vehicleStatus.lockStatus.trunkLockStatus = 0;
    } else if (2 == result.trunkLockStatus){
        vdsData.body.serviceData.vehicleStatus.lockStatus.trunkLockStatus = 1;
    }
    vdsData.body.serviceData.vehicleStatus.lockStatus.trunkOpenStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.trunkOpenStatus = result.trunkOpenStatus;
    vdsData.body.serviceData.vehicleStatus.lockStatus.engineHoodOpenStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.engineHoodOpenStatus = result.engineHoodOpenStatus;
    vdsData.body.serviceData.vehicleStatus.lockStatus.centralLockingStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.centralLockingStatus = result.centralLockingStatus;
    vdsData.body.serviceData.vehicleStatus.lockStatus.electricParkBrakeStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus.electricParkBrakeStatus = result.electricParkBrakeStatus;

    vdsData.body.serviceData.vehicleStatus.maintenanceStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilLevelStatus_presence =true;
    vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilLevelStatus = result.engineOilLevelStatus;

    vdsData.body.serviceData.vehicleStatus.runningStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevel_presence = true;
    vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevel = result.fuelLevel;
    vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevelStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevelStatus = result.fuelLevelStatus;
    vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevel_presence = true;
    vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevel = result.fuelLevel;

    bool res = PackService(vdsData, result);
    if(!res)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return res;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s completed.\n", __FUNCTION__);
    return res;
}

bool ResSignalAdapt::SetStopResultPayload(const fsm::RMT_EngineStopResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_res;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus = (Vds_EngineStatus)result.engine_status;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.usageMode_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.usageMode = result.usageMode;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speed = result.speed;

    bool res = PackService(vdsData, result);
    if(!res)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return res;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s completed.\n", __FUNCTION__);
    return res;
}

bool ResSignalAdapt::SetErrorResultPayload(const fsm::RMT_EngineErrorResult_t& result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_res;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = result.operationSucceeded;

    vdsData.body.serviceData.serviceResult.error_presence = true;
    vdsData.body.serviceData.serviceResult.error.code = (Vds_ErrorCode)result.errorCode;

    if(-1 !=result.vehicleErrorCode ){
        vdsData.body.serviceData.serviceResult.error.vehicleErrorCode_presence = true;
        vdsData.body.serviceData.serviceResult.error.vehicleErrorCode = result.vehicleErrorCode;
    }

    bool res = PackService(vdsData, result);
    if(!res)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return res;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s completed.\n", __FUNCTION__);
    return res;
}

bool ResSignalAdapt::SetNotifyPayload(const fsm::RMT_EngineNotify_t& notify)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s\n", __FUNCTION__);
    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_res;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus = (Vds_EngineStatus)notify.engine_status;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speed = notify.speed;

    bool res = PackService(vdsData, notify);
    if(!res)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return res;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "ResSignalAdapt::%s completed.\n", __FUNCTION__);
    return res;
}