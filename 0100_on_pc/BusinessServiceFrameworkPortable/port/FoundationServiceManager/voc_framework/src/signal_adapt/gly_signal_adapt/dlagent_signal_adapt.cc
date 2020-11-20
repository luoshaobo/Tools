///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  file dlagent_signal_adapt.cc

// @project     GLY_TCAM
// @subsystem   FSM
// @author      Shen Jiaojiao
// @Init date   27-Feb-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/dlagent_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

bool DLAgentSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLAgentSignalAdapt::%s\n", __FUNCTION__);

    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLAgentSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = nullptr;
    fsm::FOTA_Data *pFOTAData = new fsm::FOTA_Data();
    if(pFOTAData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLAgentSignalAdapt::%s svtData == nullptr.\n", __FUNCTION__);
        return false;
    }

    if(vdsData->body.serviceId != Vds_fota)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLAgentSignalAdapt::%s serviceId(%d) is not FOTA.\n", __FUNCTION__, vdsData->body.serviceId);
        delete pFOTAData;
        return false;
    }

    if(vdsData->body.serviceData.serviceCommand != Vds_terminateService)
    {
        pFOTAData->serviceCommand = (App_ServiceCommand)vdsData->body.serviceData.serviceCommand;
        GetFOTA(*pFOTAData, vdsData->body.serviceData.fota);
    }

    appData = pFOTAData;
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLAgentSignalAdapt::%s completed.\n", __FUNCTION__);

    return true;
}

bool DLAgentSignalAdapt::SetAssignmentNotificationPayload(const fsm::FotaAssignmentNotification_Data &msg)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s", __FUNCTION__);

    VDServiceRequest_Model vdsData = {};
    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_fota;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.fota_presence = true;
    vdsData.body.serviceData.fota.appId = Vds_geely;
    vdsData.body.serviceData.fota.vin = "123456789"; // will be modify
    vdsData.body.serviceData.fota.dataType = Vds_DataType_assignmentNotification;
    vdsData.body.serviceData.fota.assignmentNotification_presence = true;
    bool result = SetFotaAssignmentNotification(msg, vdsData.body.serviceData.fota.assignmentNotification);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: installationOrderId_presence:%d",
        vdsData.body.serviceData.fota.assignmentNotification.installationOrderId_presence);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: installationOrderId:%s",
        vdsData.body.serviceData.fota.assignmentNotification.installationOrderId.c_str());
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: timestamp:%s",
        vdsData.body.serviceData.fota.assignmentNotification.timestamp.c_str());
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: newStatus:%s",
        vdsData.body.serviceData.fota.assignmentNotification.newStatus.c_str());
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "DLA: DLAgentSignalAdapt::%s Set FotaAssignmentNotification failed.", __FUNCTION__);
        return result;
    }
    result = PackService(vdsData, msg);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "DLA: DLAgentSignalAdapt::%s PackService failed.", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s completed.", __FUNCTION__);
    return result;
}

bool DLAgentSignalAdapt::SetInstallationSummary(const fsm::InstallationSummary_Data& appData, InstallationSummary_Model &vdsData)
{
   DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: DLAgentSignalAdapt::%s", __FUNCTION__);
   bool result = true;
   vdsData.installationOrderId = appData.installationOrderId;
   vdsData.timestamp = appData.timestamp;
   vdsData.repeatresets = appData.repeatresets;
   vdsData.totalInstallationTime = appData.totalInstallationTime;
   std::for_each(appData.ecuSummarys.begin(),appData.ecuSummarys.end(),[&vdsData](const fsm::EcuSummary_Data& appEcu){
     EcuSummary_Model ecu_model;
     ecu_model.ecuAddress = appEcu.ecuAddress;
     ecu_model.ecuRetries = appEcu.ecuRetries;
     ecu_model.ecuStatus = appEcu.ecuStatus;
     std::for_each(appEcu.softwarePartSummarys.begin(),appEcu.softwarePartSummarys.end(),[&ecu_model](const fsm::SoftwarePartSummary_Data& appSwps){
        SoftwarePartSummary_Model swps_model;
        swps_model.partIdentifier = appSwps.partIdentifier;
        swps_model.partRetries = appSwps.partRetries;
        swps_model.measuredInstallationTime = appSwps.measuredInstallationTime;
        swps_model.softwarePartStatus = appSwps.softwarePartStatus;
        ecu_model.softwarePartSummarys.push_back(swps_model);
     });
     vdsData.ecuSummarys.push_back(ecu_model);
   });

   return result;
}

bool DLAgentSignalAdapt::SetAvailableAssignmentRequestPayload(const fsm::AvailableAssignmentRequest_Data &msg)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s\n", __FUNCTION__);

    VDServiceRequest_Model vdsData = {};

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)Vds_fota;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.fota_presence = true;
    vdsData.body.serviceData.fota.appId = Vds_geely;
    vdsData.body.serviceData.fota.vin = "vin123456789";
    vdsData.body.serviceData.fota.dataType = Vds_DataType_availableAssignmentRequest;
    vdsData.body.serviceData.fota.availableAssignmentRequest_presence = true;
    if (!SetAvailableAssignmentRequest(msg, vdsData.body.serviceData.fota.availableAssignmentRequest))
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s failed.\n", __FUNCTION__);
        return false;
    }

    bool result = PackService(vdsData, msg);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

bool DLAgentSignalAdapt::SetExceptionReportResultPayload(const fsm::EcuExceptionReport_Data& msg)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s\n", __FUNCTION__);

    VDServiceRequest_Model vdsData = {};

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)Vds_fota;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.fota_presence = true;
    vdsData.body.serviceData.fota.appId = Vds_geely;
    vdsData.body.serviceData.fota.vin = "vin123456789";
    vdsData.body.serviceData.fota.dataType = Vds_DataType_ecuExceptionReport;
    vdsData.body.serviceData.fota.ecuExceptionReport_presence = true;
    vdsData.body.serviceData.fota.ecuExceptionReport.exceptionReportMsgRemaining = msg.exceptionReportMsgRemaining;
    for(auto it = msg.exceptionReports.begin(); it != msg.exceptionReports.end(); it++)
    {
        ExceptionReport_Model exceptionReport;

        exceptionReport.installationOrderId_presence = (*it).installationOrderId_presence;
        if((*it).installationOrderId_presence)
        {
            exceptionReport.installationOrderId = (*it).installationOrderId;
        }
        exceptionReport.timestamp = (*it).timestamp;
        exceptionReport.issuerId = (*it).issuerId;
        exceptionReport.clientConfigurationId_presence = (*it).clientConfigurationId_presence;
        if((*it).clientConfigurationId_presence)
        {
            exceptionReport.clientConfigurationId =(*it).clientConfigurationId;
        }
        exceptionReport.dataFileName_presence = (*it).dataFileName_presence;
        if((*it).dataFileName_presence)
        {
            exceptionReport.dataFileName =(*it).dataFileName;
        }
        exceptionReport.ecuAddress_presence = (*it).ecuAddress_presence;
        if((*it).ecuAddress_presence)
        {
            exceptionReport.ecuAddress =(*it).ecuAddress;
        }
        exceptionReport.validationKey_presence = (*it).validationKey_presence;
        if((*it).validationKey_presence)
        {
            exceptionReport.validationKey =(*it).validationKey;
        }
        exceptionReport.partIdentifier_presence = (*it).partIdentifier_presence;
        if((*it).partIdentifier_presence)
        {
            exceptionReport.partIdentifier =(*it).partIdentifier;
        }
        exceptionReport.dataBlockNumber_presence = (*it).dataBlockNumber_presence;
        if((*it).dataBlockNumber_presence)
        {
            exceptionReport.dataBlockNumber = (*it).dataBlockNumber;
        }
        exceptionReport.exceptionMessage.activity = (*it).exceptionMessage.activity;
        exceptionReport.exceptionMessage.action = (*it).exceptionMessage.action;
        exceptionReport.exceptionMessage.exception = (*it).exceptionMessage.exception;

        vdsData.body.serviceData.fota.ecuExceptionReport.exceptionReports.push_back(exceptionReport);
    }

    bool result = PackService(vdsData, msg);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

bool DLAgentSignalAdapt::SetTimeStamp(const fsm::TimeStamp_Data &appData, TimeStamp_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: DLAgentSignalAdapt::%s", __FUNCTION__);
    bool result = true;

    vdsData.seconds = appData.seconds;
    if (appData.milliseconds_presence)
    {
        vdsData.milliseconds_presence = true;
        vdsData.milliseconds = appData.milliseconds;
    } else {
        vdsData.milliseconds_presence = false;
    }


    return result;
}

bool DLAgentSignalAdapt::SetServiceParameters(const std::vector<fsm::ServiceParameter_Data> &appData, std::vector<ServiceParameter_Model> &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: DLAgentSignalAdapt::%s", __FUNCTION__);
    bool result = true;

    for (auto p_serviceParameter = appData.begin(); p_serviceParameter != appData.end();) 
    {
        ServiceParameter_Model serviceParameter;
        serviceParameter.key = p_serviceParameter->key;
        serviceParameter.intVal_presence = p_serviceParameter->intVal_presence;
        serviceParameter.intVal = 0;
        serviceParameter.stringVal = "";
        serviceParameter.boolVal = false;
        serviceParameter.dataVal = "";
        if (p_serviceParameter->intVal_presence)
        {
            serviceParameter.intVal = p_serviceParameter->intVal;
        }
        serviceParameter.stringVal_presence = p_serviceParameter->stringVal_presence;
        if (p_serviceParameter->stringVal_presence)
        {
            serviceParameter.stringVal = p_serviceParameter->stringVal;
        }
        serviceParameter.boolVal_presence = p_serviceParameter->boolVal_presence;
        if (p_serviceParameter->boolVal_presence)
        {
            serviceParameter.boolVal = p_serviceParameter->boolVal;
        }
        serviceParameter.timestampVal_presence = p_serviceParameter->timestampVal_presence;
        if (p_serviceParameter->timestampVal_presence)
        {
            SetTimeStamp(p_serviceParameter->timestampVal, serviceParameter.timestampVal);
        }
        serviceParameter.dataVal_presence = p_serviceParameter->dataVal_presence;
        if (p_serviceParameter->dataVal_presence)
        {
            serviceParameter.dataVal = p_serviceParameter->dataVal;
        }
        vdsData.push_back(serviceParameter);

        ++p_serviceParameter;
    }
    
    return result;
}

bool DLAgentSignalAdapt::SetFotaAssignmentNotification(const fsm::FotaAssignmentNotification_Data &appData, FotaAssignmentNotification_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "DLA: DLAgentSignalAdapt::%s", __FUNCTION__);
    bool result = true;

    if (appData.installationOrderId_presence)
    {
        vdsData.installationOrderId_presence = true;
        vdsData.installationOrderId = appData.installationOrderId;
    } else {
        vdsData.installationOrderId_presence = false;
    }

    vdsData.timestamp = appData.timestamp;
    vdsData.newStatus = appData.newStatus;

    if (appData.reason_presence)
    {
        vdsData.reason_presence = true;
        vdsData.reason = appData.reason;
    } else {
        vdsData.reason_presence = false;
    }

    if (appData.deltaTime_presence)
    {
        vdsData.deltaTime_presence = true;
        vdsData.deltaTime = appData.deltaTime;
    } else {
        vdsData.deltaTime_presence = false;
    }

    if (appData.serviceParams_presence)
    {
        vdsData.serviceParams_presence = true;
        SetServiceParameters(appData.serviceParams, vdsData.serviceParams);
    } else {
        vdsData.serviceParams_presence = false;
    }

    return result;
}

bool DLAgentSignalAdapt::SetInstallationSummaryPayload(const fsm::InstallationSummary_Data &msg)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s", __FUNCTION__);

    bool result = true;
    VDServiceRequest_Model vdsData = {};
    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = Vds_fota;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.fota_presence = true;
    vdsData.body.serviceData.fota.appId = Vds_geely;
    vdsData.body.serviceData.fota.vin = "123456789";// will be modify
    vdsData.body.serviceData.fota.dataType = Vds_DataType_installationSummary;
    vdsData.body.serviceData.fota.installationSummary_presence = true;

    result = SetInstallationSummary(msg,vdsData.body.serviceData.fota.installationSummary);

    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "DLA: DLAgentSignalAdapt::%s Set InstallationSummary failed.", __FUNCTION__);
        return result;
    }
    result = PackService(vdsData, msg);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "DLA: DLAgentSignalAdapt::%s PackService failed.", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "DLA: DLAgentSignalAdapt::%s completed.", __FUNCTION__);

    return result;
}

bool DLAgentSignalAdapt::SetLanguageSetting(const fsm::LanguageSetting_Data &appData, LanguageSetting_Model &vdsData)
{
    vdsData.language = (Vds_Language)appData.language;
    return true;
}

bool DLAgentSignalAdapt::SetAvailableAssignmentRequest(const fsm::AvailableAssignmentRequest_Data &appData, AvailableAssignmentRequest_Model &vdsData)
{
    SetLanguageSetting(appData.languageSetting, vdsData.languageSetting);
    return true;
}

bool DLAgentSignalAdapt::GetFOTA(fsm::FOTA_Data &appData, const FOTA_Model &vdsData)
{
    appData.appId = (App_AppId)vdsData.appId;
    appData.vin = vdsData.vin;
    appData.dataType = (App_DataType)vdsData.dataType;
    if (vdsData.clientConfSyncRequest_presence)
    {
        appData.clientConfSyncRequest_presence = true;
        GetClientConfSyncRequest(appData.clientConfSyncRequest, vdsData.clientConfSyncRequest);
    } else {
        appData.clientConfSyncRequest_presence = false;
    }

    if (vdsData.inventoryInstruction_presence)
    {
        appData.inventoryInstruction_presence = true;
        GetInventoryInstruction(appData.inventoryInstruction, vdsData.inventoryInstruction);
    } else {
        appData.inventoryInstruction_presence = false;
    }

    if (vdsData.availableAssignmentRequest_presence)
    {
        appData.availableAssignmentRequest_presence = true;
        GetAvailableAssignmentRequest(appData.availableAssignmentRequest, vdsData.availableAssignmentRequest);
    } else {
        appData.availableAssignmentRequest_presence = false;
    }

    if (vdsData.availableAssignment_presence)
    {
        appData.availableAssignment_presence = true;
        GetAvailableAssignment(appData.availableAssignment, vdsData.availableAssignment);
    } else {
        appData.availableAssignment_presence = false;
    }

    if (vdsData.installationInstruction_presence)
    {
        appData.installationInstruction_presence = true;
        GetInstallationInstruction(appData.installationInstruction, vdsData.installationInstruction);
    } else {
        appData.installationInstruction_presence = false;
    }

    if (vdsData.installationSummary_presence)
    {
        appData.installationSummary_presence = true;
        GetInstallationSummary(appData.installationSummary, vdsData.installationSummary);
    } else {
        appData.installationSummary_presence = false;
    }

    if (vdsData.ecuExceptionReport_presence)
    {
        appData.ecuExceptionReport_presence = true;
        GetEcuExceptionReport(appData.ecuExceptionReport, vdsData.ecuExceptionReport);
    } else {
        appData.ecuExceptionReport_presence = false;
    }

    if (vdsData.clientHmiVehicleSettings_presence)
    {
        appData.clientHmiVehicleSettings_presence = true;
        GetClientHmiVehicleSettings(appData.clientHmiVehicleSettings, vdsData.clientHmiVehicleSettings);
    } else {
        appData.clientHmiVehicleSettings_presence = false;
    }

    if (vdsData.assignmentData_presence)
    {
        appData.assignmentData_presence = true;
        GetAssignmentData(appData.assignmentData, vdsData.assignmentData);
    } else {
        appData.assignmentData_presence = false;
    }

    if (vdsData.assignmentNotification_presence)
    {
        appData.assignmentNotification_presence = true;
        GetFotaAssignmentNotification(appData.assignmentNotification, vdsData.assignmentNotification);
    } else {
        appData.assignmentNotification_presence = false;
    }

    if (vdsData.otaAssignmentFileInfo_presence)
    {
        appData.otaAssignmentFileInfo_presence = true;
        GetOtaAssignmentFileInfo(appData.otaAssignmentFileInfo, vdsData.otaAssignmentFileInfo);
    } else {
        appData.otaAssignmentFileInfo_presence = false;
    }

    return true;
}

bool DLAgentSignalAdapt::GetEcuInventory(fsm::EcuInventory_Data &appData, const EcuInventory_Model &vdsData)
{
    appData.ecuAddress = vdsData.ecuAddress;
    for (auto validationKeyIter = vdsData.validationKeys.begin(); validationKeyIter != vdsData.validationKeys.end(); validationKeyIter++)
    {
        fsm::ValidationKey_Data tmp_validationKey;
        tmp_validationKey.validationKey = (*validationKeyIter).validationKey;
        if ((*validationKeyIter).partIdentifiers_presence)
        {
            tmp_validationKey.partIdentifiers_presence = true;
            for (auto partIdentifierIter = (*validationKeyIter).partIdentifiers.begin(); partIdentifierIter != (*validationKeyIter).partIdentifiers.end(); partIdentifierIter++)
            {
                tmp_validationKey.partIdentifiers.push_back(*partIdentifierIter);
            }
        } else {
            tmp_validationKey.partIdentifiers_presence = false;
        }
        appData.validationKeys.push_back(tmp_validationKey);
    }

    if (vdsData.exceptionMessage_presence)
    {
        appData.exceptionMessage_presence = true;
        appData.exceptionMessage = vdsData.exceptionMessage;
    } else {
        appData.exceptionMessage_presence = false;
    }

    return true;
}

bool DLAgentSignalAdapt::GetClientConfSyncRequest(fsm::ClientConfSyncRequest_Data &appData, const ClientConfSyncRequest_Model &vdsData)
{
    appData.timestamp = vdsData.timestamp;
    appData.clientVersion = vdsData.clientVersion;
    if (vdsData.ecuInventory_presence)
    {
        appData.ecuInventory_presence = true;
        for (auto ecuInventoryIter = vdsData.ecuInventory.begin(); ecuInventoryIter != vdsData.ecuInventory.end(); ecuInventoryIter++)
        {
            fsm::EcuInventory_Data tmp_ecuInventory;
            GetEcuInventory(tmp_ecuInventory, *ecuInventoryIter);
            appData.ecuInventory.push_back(tmp_ecuInventory);
        }
    } else {
        appData.ecuInventory_presence = false;
    }
    return true;
}

bool DLAgentSignalAdapt::GetInventoryInstruction(fsm::InventoryInstruction_Data &appData, const InventoryInstruction_Model &vdsData)
{
    appData.inventoryInstructionVersion = vdsData.inventoryInstructionVersion;
    for (auto ecuInstructionIter = vdsData.ecuInstructions.begin(); ecuInstructionIter != vdsData.ecuInstructions.end(); ecuInstructionIter++)
    {
        fsm::EcuInventory_Data tmp_ecuInventory;
        GetEcuInventory(tmp_ecuInventory, *ecuInstructionIter);
        appData.ecuInstructions.push_back(tmp_ecuInventory);
    }
    return true;
}

bool DLAgentSignalAdapt::GetAvailableAssignmentRequest(fsm::AvailableAssignmentRequest_Data &appData, const AvailableAssignmentRequest_Model &vdsData)
{
    GetLanguageSetting(appData.languageSetting, vdsData.languageSetting);
    return true;
}

bool DLAgentSignalAdapt::GetAvailableAssignment(fsm::AvailableAssignment_Data &appData, const AvailableAssignment_Model &vdsData)
{
    appData.installationOrderId = vdsData.installationOrderId;
    appData.timestamp = vdsData.timestamp;
    appData.workshopInstallation = vdsData.workshopInstallation;
    appData.downloadsize = vdsData.downloadsize;
    appData.totalinstallationTime = vdsData.totalinstallationTime;
    appData.newstatus = vdsData.newstatus;
    if (vdsData.reason_presence)
    {
        appData.reason_presence = true;
        appData.reason = vdsData.reason;
    } else {
        appData.reason_presence = false;
    }
    appData.name = vdsData.name;
    appData.shortDescription = vdsData.shortDescription;
    if (vdsData.longDescription_presence)
    {
        appData.longDescription_presence = true;
        appData.longDescription = vdsData.longDescription;
    } else {
        appData.longDescription_presence = false;
    }

    return true;
}

bool DLAgentSignalAdapt::GetInstruction(fsm::Instruction_Data &appData, const Instruction_Model &vdsData)
{
    appData.ecuRemaining = vdsData.ecuRemaining;
    appData.installationInstructionSversion = vdsData.installationInstructionSversion;
    appData.requiredPreparationTime = vdsData.requiredPreparationTime;
    appData.expectedInstallationTime = vdsData.expectedInstallationTime;
    appData.area1112SecurityCode = vdsData.area1112SecurityCode;
    for (auto ecuInstructionIter = vdsData.ecuInstructions.begin(); ecuInstructionIter != vdsData.ecuInstructions.end(); ecuInstructionIter++)
    {
        fsm::EcuInstruction_Data tmp_ecuInstruction;
        tmp_ecuInstruction.ecuAddress = (*ecuInstructionIter).ecuAddress;
        tmp_ecuInstruction.queuedRequest = (*ecuInstructionIter).queuedRequest;
        tmp_ecuInstruction.securityKey = (*ecuInstructionIter).securityKey;
        for (auto softwarePartInstallationInstructionIter = (*ecuInstructionIter).softwarePartInstallationInstructions.begin();
                softwarePartInstallationInstructionIter != (*ecuInstructionIter).softwarePartInstallationInstructions.end();
                softwarePartInstallationInstructionIter++)
        {
            fsm::SoftwarePartInstallationInstruction_Data tmp_softwarePartInstallationInstruction;
            tmp_softwarePartInstallationInstruction.partidentifier = (*softwarePartInstallationInstructionIter).partidentifier;
            tmp_softwarePartInstallationInstruction.estimatedInstallationtime = (*softwarePartInstallationInstructionIter).estimatedInstallationtime;
            tmp_ecuInstruction.softwarePartInstallationInstructions.push_back(tmp_softwarePartInstallationInstruction);
        }
        appData.ecuInstructions.push_back(tmp_ecuInstruction);
    }

    for (auto assignmentValidationIter = vdsData.assignmentValidations.begin();
            assignmentValidationIter != vdsData.assignmentValidations.end();
            assignmentValidationIter++)
    {
        fsm::AssignmentValidation_Data tmp_assignmentValidation;
        tmp_assignmentValidation.validationType = (*assignmentValidationIter).validationType;
        tmp_assignmentValidation.ecuAddress = (*assignmentValidationIter).ecuAddress;
        tmp_assignmentValidation.validationkey = (*assignmentValidationIter).validationkey;

        for (auto partidentifierIter = (*assignmentValidationIter).partidentifiers.begin();
                partidentifierIter != (*assignmentValidationIter).partidentifiers.end();
                partidentifierIter++)
        {
            tmp_assignmentValidation.partidentifiers.push_back(*partidentifierIter);
        }

        appData.assignmentValidations.push_back(tmp_assignmentValidation);
    }
    return true;
}

bool DLAgentSignalAdapt::GetInstallationInstruction(fsm::InstallationInstruction_Data &appData, const InstallationInstruction_Model &vdsData)
{
    appData.installationOrderId = vdsData.installationOrderId;
    for (auto instructionIter = vdsData.instructions.begin();
            instructionIter != vdsData.instructions.end();
            instructionIter++)
    {
        fsm::Instruction_Data tmp_instruction;
        GetInstruction(tmp_instruction, *instructionIter);
        appData.instructions.push_back(tmp_instruction);
    }

    return true;
}

bool DLAgentSignalAdapt::GetInstallationSummary(fsm::InstallationSummary_Data &appData, const InstallationSummary_Model &vdsData)
{
    appData.installationOrderId = vdsData.installationOrderId;
    appData.timestamp = vdsData.timestamp;
    appData.repeatresets = vdsData.repeatresets;
    appData.totalInstallationTime = vdsData.totalInstallationTime;
    for (auto ecuSummaryIter = vdsData.ecuSummarys.begin(); ecuSummaryIter != vdsData.ecuSummarys.end(); ecuSummaryIter++)
    {
        fsm::EcuSummary_Data tmp_ecuSummary;
        tmp_ecuSummary.ecuAddress = (*ecuSummaryIter).ecuAddress;
        tmp_ecuSummary.ecuRetries = (*ecuSummaryIter).ecuRetries;
        tmp_ecuSummary.ecuStatus = (*ecuSummaryIter).ecuStatus;
        for (auto softwarePartSummaryIter = (*ecuSummaryIter).softwarePartSummarys.begin();
                softwarePartSummaryIter != (*ecuSummaryIter).softwarePartSummarys.end();
                softwarePartSummaryIter++)
        {
            fsm::SoftwarePartSummary_Data tmp_softwarePartSummary;
            tmp_softwarePartSummary.partIdentifier = (*softwarePartSummaryIter).partIdentifier;
            tmp_softwarePartSummary.partRetries = (*softwarePartSummaryIter).partRetries;
            tmp_softwarePartSummary.measuredInstallationTime = (*softwarePartSummaryIter).measuredInstallationTime;
            tmp_softwarePartSummary.softwarePartStatus = (*softwarePartSummaryIter).softwarePartStatus;
            tmp_ecuSummary.softwarePartSummarys.push_back(tmp_softwarePartSummary);
        }
        appData.ecuSummarys.push_back(tmp_ecuSummary);
    }
    return true;
}

bool DLAgentSignalAdapt::GetEcuExceptionReport(fsm::EcuExceptionReport_Data &appData, const EcuExceptionReport_Model &vdsData)
{
    appData.exceptionReportMsgRemaining = vdsData.exceptionReportMsgRemaining;
    for (auto exceptionReportIter = vdsData.exceptionReports.begin();
            exceptionReportIter != vdsData.exceptionReports.end();
            exceptionReportIter++)
    {
        fsm::ExceptionReport_Data tmp_exceptionReport;
        if ((*exceptionReportIter).installationOrderId_presence)
        {
            tmp_exceptionReport.installationOrderId_presence = true;
            tmp_exceptionReport.installationOrderId = (*exceptionReportIter).installationOrderId;
        } else {
            tmp_exceptionReport.installationOrderId_presence = false;
        }

        tmp_exceptionReport.timestamp = (*exceptionReportIter).timestamp;
        tmp_exceptionReport.issuerId = (*exceptionReportIter).issuerId;
        if ((*exceptionReportIter).clientConfigurationId_presence)
        {
            tmp_exceptionReport.clientConfigurationId_presence = true;
            tmp_exceptionReport.clientConfigurationId = (*exceptionReportIter).clientConfigurationId;
        } else {
            tmp_exceptionReport.clientConfigurationId_presence = false;
        }

        if ((*exceptionReportIter).dataFileName_presence)
        {
            tmp_exceptionReport.dataFileName_presence = true;
            tmp_exceptionReport.dataFileName = (*exceptionReportIter).dataFileName;
        } else {
            tmp_exceptionReport.dataFileName_presence = false;
        }

        if ((*exceptionReportIter).ecuAddress_presence)
        {
            tmp_exceptionReport.ecuAddress_presence = true;
            tmp_exceptionReport.ecuAddress = (*exceptionReportIter).ecuAddress;
        } else {
            tmp_exceptionReport.ecuAddress_presence = false;
        }

        if ((*exceptionReportIter).validationKey_presence)
        {
            tmp_exceptionReport.validationKey_presence = true;
            tmp_exceptionReport.validationKey = (*exceptionReportIter).validationKey;
        } else {
            tmp_exceptionReport.validationKey_presence = false;
        }

        if ((*exceptionReportIter).partIdentifier_presence)
        {
            tmp_exceptionReport.partIdentifier_presence = true;
            tmp_exceptionReport.partIdentifier = (*exceptionReportIter).partIdentifier;
        } else {
            tmp_exceptionReport.partIdentifier_presence = false;
        }

        if ((*exceptionReportIter).dataBlockNumber_presence)
        {
            tmp_exceptionReport.dataBlockNumber_presence = true;
            tmp_exceptionReport.dataBlockNumber = (*exceptionReportIter).dataBlockNumber;
        } else {
            tmp_exceptionReport.dataBlockNumber_presence = false;
        }

        GetExceptionMessage(tmp_exceptionReport.exceptionMessage, (*exceptionReportIter).exceptionMessage);
        appData.exceptionReports.push_back(tmp_exceptionReport);
    }
    return true;
}

bool DLAgentSignalAdapt::GetClientHmiVehicleSettings(fsm::ClientHmiVehicleSettings_Data &appData, const ClientHmiVehicleSettings_Model &vdsData)
{
    appData.otaSetting = vdsData.otaSetting;
    appData.autoSync = vdsData.autoSync;
    appData.autoDownload = vdsData.autoDownload;
    appData.autoInstallation = vdsData.autoInstallation;

    return true;
}

bool DLAgentSignalAdapt::GetAssignmentData(fsm::AssignmentData_Data &appData, const AssignmentData_Model &vdsData)
{
    appData.installationOrderId = vdsData.installationOrderId;
    appData.timestamp = vdsData.timestamp;
    for (auto urlIter = vdsData.urls.begin(); urlIter != vdsData.urls.end(); urlIter++)
    {
        appData.urls.push_back(*urlIter);
    }
    return true;
}

bool DLAgentSignalAdapt::GetFotaAssignmentNotification(fsm::FotaAssignmentNotification_Data &appData, const FotaAssignmentNotification_Model &vdsData)
{
    if (vdsData.installationOrderId_presence)
    {
        appData.installationOrderId_presence = true;
        appData.installationOrderId = vdsData.installationOrderId;
    } else {
        appData.installationOrderId_presence = false;
    }

    appData.timestamp = vdsData.timestamp;
    appData.newStatus = vdsData.newStatus;
    if (vdsData.reason_presence)
    {
        appData.reason_presence = true;
        appData.reason = vdsData.reason;
    } else {
        appData.reason_presence = false;
    }

    if (vdsData.deltaTime_presence)
    {
        appData.deltaTime_presence = true;
        appData.deltaTime = vdsData.deltaTime;
    } else {
        appData.deltaTime_presence = false;
    }
    if (vdsData.serviceParams_presence)
    {
        appData.serviceParams_presence = true;
        for (auto serviceParamIter = vdsData.serviceParams.begin();
                serviceParamIter != vdsData.serviceParams.end();
                serviceParamIter++)
        {
            fsm::ServiceParameter_Data tmp_serviceParam;
            tmp_serviceParam.key = (*serviceParamIter).key;
            if ((*serviceParamIter).intVal_presence)
            {
                tmp_serviceParam.intVal_presence = true;
                tmp_serviceParam.intVal = (*serviceParamIter).intVal;
            } else {
                tmp_serviceParam.intVal_presence = false;
            }

            if ((*serviceParamIter).stringVal_presence)
            {
                tmp_serviceParam.stringVal_presence = true;
                tmp_serviceParam.stringVal = (*serviceParamIter).stringVal;
            } else {
                tmp_serviceParam.stringVal_presence = false;
            }

            if ((*serviceParamIter).boolVal_presence)
            {
                tmp_serviceParam.boolVal_presence = true;
                tmp_serviceParam.boolVal = (*serviceParamIter).boolVal;
            } else {
                tmp_serviceParam.boolVal_presence = false;
            }

            if ((*serviceParamIter).timestampVal_presence)
            {
                tmp_serviceParam.timestampVal_presence = true;
                GetTimeStamp(tmp_serviceParam.timestampVal, (*serviceParamIter).timestampVal);
            } else {
                tmp_serviceParam.timestampVal_presence = false;
            }

            if ((*serviceParamIter).dataVal_presence)
            {
                tmp_serviceParam.dataVal_presence = true;
                tmp_serviceParam.dataVal = (*serviceParamIter).dataVal;
            } else {
                tmp_serviceParam.dataVal_presence = false;
            }

            appData.serviceParams.push_back(tmp_serviceParam);
        }
    } else {
        appData.serviceParams_presence = false;
    }

    return true;
}

bool DLAgentSignalAdapt::GetOtaAssignmentFileInfo(fsm::OtaAssignmentFileInfo_Data &appData, const OtaAssignmentFileInfo_Model &vdsData)
{
    appData.installationOrderId = vdsData.installationOrderId;
    for (auto assignFileInfoIter = vdsData.assignFileInfos.begin();
            assignFileInfoIter != vdsData.assignFileInfos.end();
            assignFileInfoIter++)
    {
        fsm::AssignmentFileInfo_Data tmp_assignFileInfo;
        tmp_assignFileInfo.partidentifier = (*assignFileInfoIter).partidentifier;
        tmp_assignFileInfo.softwarePartSignature = (*assignFileInfoIter).softwarePartSignature;
        tmp_assignFileInfo.fileCheckSum = (*assignFileInfoIter).fileCheckSum;
        appData.assignFileInfos.push_back(tmp_assignFileInfo);
    }

    return true;
}

bool DLAgentSignalAdapt::GetExceptionMessage(fsm::ExceptionMessage_Data &appData, const ExceptionMessage_Model &vdsData)
{
    appData.activity = vdsData.activity;
    appData.action = vdsData.action;
    appData.exception = vdsData.exception;

    return true;
}

bool DLAgentSignalAdapt::GetLanguageSetting(fsm::LanguageSetting_Data &appData, const LanguageSetting_Model &vdsData)
{
    appData.language = (App_Language)vdsData.language;
    return true;
}

bool DLAgentSignalAdapt::GetTimeStamp(fsm::TimeStamp_Data &appData, const TimeStamp_Model &vdsData)
{
    appData.seconds = vdsData.seconds;
    if (vdsData.milliseconds_presence)
    {
        appData.milliseconds_presence = true;
        appData.milliseconds = vdsData.milliseconds;
    } else {
        appData.milliseconds_presence = false;
    }

    return true;
}

