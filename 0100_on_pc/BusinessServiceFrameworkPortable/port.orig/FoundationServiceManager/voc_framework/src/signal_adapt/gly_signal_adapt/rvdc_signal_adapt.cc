///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rvdc_signal_adapt.cc

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Nie Yujin
// @Init date	26-Feb-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/rvdc_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);
namespace fsm
{
/**
 *    TypeAssignment : RVDC_Data
 *    Class constructor
 */
RVDC_Data::RVDC_Data()
   :serviceId(250)
   ,serviceCommand(0)
   ,appId(app_geely)
   ,maOrderId()
   ,timestamp()
   ,vin()
   ,maBusinessType(app_MaBusinessType_rvdcSetting)
   ,rvdcSetting_presence(false)
   ,rvdcSetting()
   ,assignmentNotification_presence(false)
   ,assignmentNotification()
   ,maSynchronizationReq_presence(false)
   ,maSynchronizationReq()
   ,maSynchronization_presence(false)
   ,maSynchronization()
   ,exceptionHandling_presence(false)
   ,exceptionHandling()
   ,measurementDataPackage_presence(false)
   ,measurementDataPackage()
{
}
RVDC_Data:: ~RVDC_Data()
{
}

RvdcSetting_Data::RvdcSetting_Data()
    :maSetting_presence(false)
    ,maSetting()
    ,vehicleSetting_presence(false)
    ,vehicleSetting()
    ,languageSetting_presence(false)
    ,languageSetting()
    ,forcedSetting_presence(false)
    ,forcedSetting()
    ,serviceParameters_presence(false)
    ,serviceParameters()
{
}
RvdcSetting_Data::~RvdcSetting_Data()
{
}

RvdcAssignmentNotification_Data::RvdcAssignmentNotification_Data()
    :maCarId_presence(false)
    ,maCarId(0)
    ,maVersion_presence(false)
    ,maVersion(0)
    ,newStatus()
    ,timestamp()
    ,reason_presence(false)
    ,reason()
{
}
RvdcAssignmentNotification_Data::~RvdcAssignmentNotification_Data()
{
}
void RvdcAssignmentNotification_Data::Assign(const OpRVDCAssignmentNotification_Notification_Data& data)
{
    maCarId_presence = true;
    maCarId = data.maid;
    maVersion_presence = true;
    maVersion = data.maversion;
    newStatus = data.newstatus;
    timestamp = data.timestamp.timestamp1;
    reason_presence = data.reason_presence;
    reason = data.reason;
}

MaSynchronizationReq_Data::MaSynchronizationReq_Data()
    :storageQuota(0)
    ,maFormatVersion(0)
    ,installedMa_presence(false)
    ,installedMa()
    ,serviceParameters_presence(false)
    ,serviceParameters()
{
}
MaSynchronizationReq_Data::~MaSynchronizationReq_Data()
{
}

MaSynchronization_Data::MaSynchronization_Data()
    :removeMas_presence(false)
    ,removeMas()
    ,newMas_presence(false)
    ,newMas()
    ,serviceParameters_presence(false)
    ,serviceParameters()
{
}
MaSynchronization_Data::~MaSynchronization_Data()
{
}
void MaSynchronization_Data::AssignTo(OpRVDCMeasurementSynchronization_Response_Data& data)
{
    data.removemas_presence = removeMas_presence;
    data.newmas_presence = newMas_presence;
    if (removeMas_presence)
    {
        for (auto pRemoveMa = removeMas.begin(); pRemoveMa != removeMas.end();) 
        {
            MA_Data removedMa;
            removedMa.maid = pRemoveMa->maId;
            removedMa.maversion = pRemoveMa->maVersion;
            data.removemas.push_back(removedMa);
            ++pRemoveMa;
        }
    }
    if (newMas_presence)
    {
        for (auto pNewMa = newMas.begin(); pNewMa != newMas.end();) 
        {
            NewMAs_Data newMa;
            newMa.maformatversion = pNewMa->maFormatVersion;
            newMa.vin = pNewMa->vin;
            newMa.maid = pNewMa->maId;
            newMa.maversion = pNewMa->maVersion;
            newMa.maexecutionpriority = pNewMa->maExecutionPriority;
            newMa.telematicsparameters.timetolive = pNewMa->telematicsParameters.timeToLive;
            newMa.telematicsparameters.uploadpriority = pNewMa->telematicsParameters.uploadPriority;
            newMa.usagemodesallowed = pNewMa->usageModesAllowed;
            newMa.startcondition.timestamp1 = pNewMa->startCondition;
            newMa.stopcondition.period_presence = pNewMa->stopCondition.period_presence;
            newMa.stopcondition.period = pNewMa->stopCondition.period;
            newMa.stopcondition.amount_presence = pNewMa->stopCondition.amount_presence;
            newMa.stopcondition.amount = pNewMa->stopCondition.amount;
            newMa.stopcondition.time_presence = pNewMa->stopCondition.time_presence;
            newMa.stopcondition.time = pNewMa->stopCondition.time;
            newMa.retentiondate.timestamp1 = pNewMa->retentiondate;
            newMa.authorizationstatus.datacollection = pNewMa->authorizationStatus.dataCollection;
            newMa.authorizationstatus.remotediagnostics = pNewMa->authorizationStatus.remoteDiagnostics;
            newMa.executiontrigger.uds_choice = pNewMa->executionTrigger.uds_presence;
            if (pNewMa->executionTrigger.uds_presence)
            {
                newMa.executiontrigger.uds.Ipcb_operator = static_cast<Ipcb_Operator>(pNewMa->executionTrigger.uds.App_operator);
                newMa.executiontrigger.uds.delayfornexttriggercheck = pNewMa->executionTrigger.uds.delayForNextTriggerCheck;
                newMa.executiontrigger.uds.delayaftercollection = pNewMa->executionTrigger.uds.delayAfterCollection;
                for (auto pTriggerData = pNewMa->executionTrigger.uds.diagnosticCommands.begin(); pTriggerData != pNewMa->executionTrigger.uds.diagnosticCommands.end();) 
                {
                    TriggerDiagnosticCommands_Data triggerData;
                    triggerData.ecuaddress.addr = pTriggerData->ecuAddress;
                    triggerData.serviceid.sid = pTriggerData->serviceId;
                    triggerData.input.input = pTriggerData->input;
                    triggerData.comparetype = Ipcb_equalto;
                    if (pTriggerData->compareType_presence)
                    {
                        triggerData.comparetype = static_cast<Ipcb_CompareType>(pTriggerData->compareType);
                    }
                    if (pTriggerData->compareValue_presence)
                    {
                        triggerData.comparevalue = pTriggerData->compareValue;
                    }
                    triggerData.delayuntilnexttriggercommand = pTriggerData->delayUntilNextTriggerCommand;
                    newMa.executiontrigger.uds.triggerdiagnosticcommands.push_back(triggerData);
                    ++pTriggerData;
                }
            }
            newMa.executiontrigger.push_choice = true;
            newMa.executiontrigger.push = pNewMa->executionTrigger.push;
            newMa.executiontrigger.time_choice = pNewMa->executionTrigger.time_presence;
            if (pNewMa->executionTrigger.time_presence)
            {
                newMa.executiontrigger.time.trigger = static_cast<Ipcb_Trigger>(pNewMa->executionTrigger.time.trigger);
                newMa.executiontrigger.time.timeinterval = pNewMa->executionTrigger.time.interval;
            }
            newMa.executiontrigger.drivingcycle_choice = pNewMa->executionTrigger.drivingCycle_presence;
            if (pNewMa->executionTrigger.drivingCycle_presence)
            {
                newMa.executiontrigger.drivingcycle.trigger = static_cast<Ipcb_Trigger>(pNewMa->executionTrigger.drivingCycle.trigger);
                newMa.executiontrigger.drivingcycle.drivingcycleinterval = pNewMa->executionTrigger.drivingCycle.interval;
            }
            
            for (auto pEcusAndPartNumber = pNewMa->ecusAndPartNumbers.begin(); pEcusAndPartNumber != pNewMa->ecusAndPartNumbers.end();) 
            {
                ECUsAndPartNumbers_Data ecusAndPartNumber;
                ecusAndPartNumber.ecuaddress.addr = pEcusAndPartNumber->ecuAddress;
                ecusAndPartNumber.diagnosticpartnumber = pEcusAndPartNumber->diagnosticPartNumber;
                newMa.ecusandpartnumbers.push_back(ecusAndPartNumber);
                ++pEcusAndPartNumber;
            }

            for (auto pDiagCmd = pNewMa->diagnosticCommand.begin(); pDiagCmd != pNewMa->diagnosticCommand.end();) 
            {
                DiagnosticCommands_Data diagCmd;
                diagCmd.ecuaddress.addr = pDiagCmd->ecuAddress;
                diagCmd.serviceid.sid = pDiagCmd->serviceId;
                diagCmd.input.input = pDiagCmd->input;
                diagCmd.delayuntilnexttriggercommand = pDiagCmd->delayUntilNextTriggerCommand;
                newMa.diagnosticcommands.push_back(diagCmd);
                ++pDiagCmd;
            }
            newMa.dataintegritycheck = pNewMa->dataIntegrityCheck;
            data.newmas.push_back(newMa);
            ++pNewMa;
        }
    }
}
RvdcExceptionHandling_Data::RvdcExceptionHandling_Data()
    :exceptionName()
    ,description()
    ,publisher()
    ,severity_presence(false)
    ,severity()
    ,impact_presence(false)
    ,impact()
    ,action_presence(false)
    ,action()
{
}
RvdcExceptionHandling_Data::~RvdcExceptionHandling_Data()
{
}

MeasurementDataPackage_Data::MeasurementDataPackage_Data()
    :telematicsParameters()
    ,serviceParameters_presence(false)
    ,serviceParameters()
    ,mdp()
{
}
MeasurementDataPackage_Data::~MeasurementDataPackage_Data()
{
}
void MeasurementDataPackage_Data::Assign(const OpRVDCMeasurementDataPackage_Request_Data &data)
{
    telematicsParameters.timeToLive = data.telematicsparameters.timetolive;
    telematicsParameters.uploadPriority = data.telematicsparameters.uploadpriority;
    serviceParameters_presence = false;
    mdp.mdpFormatVersion = data.payload.mdpformatversion;
    mdp.vin = data.payload.vin;
    mdp.ma.maId = data.payload.maid;
    mdp.ma.maVersion = data.payload.maversion;
    mdp.sequenceNumber = data.payload.sequencenumber;
    mdp.timestampReadOutStart = data.payload.timestampreadoutstart.timestamp1;
    mdp.udsTriggerId_presence = true;
    mdp.udsTriggerId = data.payload.udstriggerid;
    mdp.diagnosticResponse = data.payload.diagnosticresponse;
}

DiagnosticCommand_Data::DiagnosticCommand_Data()
    :ecuAddress()
    ,serviceId()
    ,input()
    ,compareType_presence(false)
    ,compareType(app_equalto)
    ,compareValue_presence(false)
    ,compareValue()
    ,delayUntilNextTriggerCommand(1)
{
}
DiagnosticCommand_Data::~DiagnosticCommand_Data()
{
}

bool RvdcSignalAdapt::GetServiceParameters(std::vector<ServiceParameter_Data> &appData, const std::vector<ServiceParameter_Model> &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    for (auto pServiceParameter = vdsData.begin(); pServiceParameter != vdsData.end();) 
    {
        ServiceParameter_Data serviceParameter;
        serviceParameter.key = pServiceParameter->key;
        serviceParameter.intVal_presence = pServiceParameter->intVal_presence;
        if (pServiceParameter->intVal_presence)
        {
            serviceParameter.intVal = pServiceParameter->intVal;
        }
        serviceParameter.stringVal_presence = pServiceParameter->stringVal_presence;
        if (pServiceParameter->stringVal_presence)
        {
            serviceParameter.stringVal = pServiceParameter->stringVal;
        }
        serviceParameter.boolVal_presence = pServiceParameter->boolVal_presence;
        if (pServiceParameter->boolVal_presence)
        {
            serviceParameter.boolVal = pServiceParameter->boolVal;
        }
        serviceParameter.timestampVal_presence = pServiceParameter->timestampVal_presence;
        if (pServiceParameter->timestampVal_presence)
        {
            GetTimeStamp(serviceParameter.timestampVal, pServiceParameter->timestampVal);
        }
        serviceParameter.dataVal_presence = pServiceParameter->dataVal_presence;
        if (pServiceParameter->dataVal_presence)
        {
            serviceParameter.dataVal = pServiceParameter->dataVal;
        }
        appData.push_back(serviceParameter);
        ++pServiceParameter;

        
    }

    return true;
}

bool RvdcSignalAdapt::GetTimeStamp(TimeStamp_Data &appData, const TimeStamp_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.seconds = vdsData.seconds;
    appData.milliseconds_presence = vdsData.milliseconds_presence;
    if (vdsData.milliseconds_presence)
    {
        appData.milliseconds = vdsData.milliseconds;
    }

    return true;
}

bool RvdcSignalAdapt::GetAuthorizationStatus(AuthorizationStatus_Data &appData, const AuthorizationStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.dataCollection_presence = vdsData.dataCollection_presence;
    if (vdsData.dataCollection_presence)
    {
        appData.dataCollection = vdsData.dataCollection;
    }
    appData.remoteDiagnostics_presence = vdsData.remoteDiagnostics_presence;
    if (vdsData.remoteDiagnostics_presence)
    {
        appData.remoteDiagnostics = vdsData.remoteDiagnostics;
    }

    return true;
}

bool RvdcSignalAdapt::GetMaSetting(MaSetting_Data &appData, const MaSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.collectFunction = vdsData.collectFunction;
    GetAuthorizationStatus(appData.authorizationStatus, vdsData.authorizationStatus);

    return true;
}

bool RvdcSignalAdapt::GetRvdcVehicleSetting(RvdcVehicleSetting_Data &appData, const RvdcVehicleSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.rvdcFunction = vdsData.rvdcFunction;
    appData.collectOffSetTime = vdsData.collectOffSetTime;

    return true;
}

bool RvdcSignalAdapt::GetLanguageSetting(LanguageSetting_Data &appData, const LanguageSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.language = (App_Language)vdsData.language;

    return true;
}

bool RvdcSignalAdapt::GetRvdcForcedSetting(RvdcForcedSetting_Data &appData, const RvdcForcedSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.visibility = vdsData.visibility;
    appData.suppress = vdsData.suppress;
    appData.lock = vdsData.lock;

    return true;
}

bool RvdcSignalAdapt::GetTelematicsParameters(TelematicsParameters_Data &appData, const TelematicsParameters_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.timeToLive = vdsData.timeToLive;
    appData.uploadPriority = vdsData.uploadPriority;

    return true;
}

bool RvdcSignalAdapt::GetStopCondition(StopCondition_Data &appData, const StopCondition_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.period_presence = vdsData.period_presence;
    if (vdsData.period_presence)
    {
        appData.period = vdsData.period;
    }
    appData.amount_presence = vdsData.amount_presence;
    if (vdsData.amount_presence)
    {
        appData.amount = vdsData.amount;
    }
    appData.time_presence = vdsData.time_presence;
    if (vdsData.time_presence)
    {
        appData.time = vdsData.time;
    }

    return true;
}

bool RvdcSignalAdapt::GetUDS(UDS_Data &appData, const UDS_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.App_operator = (App_Operator)vdsData.Vds_operator;
    appData.delayForNextTriggerCheck = vdsData.delayForNextTriggerCheck;
    appData.delayAfterCollection = vdsData.delayAfterCollection;
    for (auto pDiagnosticCommand = vdsData.diagnosticCommands.begin(); pDiagnosticCommand != vdsData.diagnosticCommands.end();) 
    {
        DiagnosticCommand_Data diagnosticCommand;
        diagnosticCommand.ecuAddress = pDiagnosticCommand->ecuAddress;
        diagnosticCommand.serviceId = pDiagnosticCommand->serviceId;
        diagnosticCommand.input = pDiagnosticCommand->input;
        diagnosticCommand.compareType_presence = pDiagnosticCommand->compareType_presence; 
        if (pDiagnosticCommand->compareType_presence)
        {
            diagnosticCommand.compareType = (App_CompareType)pDiagnosticCommand->compareType;
        }
        diagnosticCommand.compareValue_presence = pDiagnosticCommand->compareValue_presence;
        if (pDiagnosticCommand->compareValue_presence)
        {
            diagnosticCommand.compareValue = pDiagnosticCommand->compareValue;
        }
        diagnosticCommand.delayUntilNextTriggerCommand = pDiagnosticCommand->delayUntilNextTriggerCommand;
        appData.diagnosticCommands.push_back(diagnosticCommand);
        ++pDiagnosticCommand;
    }
    

    return true;
}

bool RvdcSignalAdapt::GetTrigger(Trigger_Data &appData, const Trigger_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.trigger = (App_TriggerType)vdsData.trigger;
    appData.interval = vdsData.interval;

    return true;
}

bool RvdcSignalAdapt::GetExecutionTrigger(ExecutionTrigger_Data &appData, const ExecutionTrigger_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.push = vdsData.push;
    appData.uds_presence = vdsData.uds_presence;
    if (vdsData.uds_presence)
    {
        GetUDS(appData.uds, vdsData.uds);
    }
    appData.time_presence = vdsData.time_presence;
    if (vdsData.time_presence)
    {
        GetTrigger(appData.time, vdsData.time);
    }
    appData.drivingCycle_presence = vdsData.drivingCycle_presence;
    if (vdsData.drivingCycle_presence)
    {
        GetTrigger(appData.drivingCycle, vdsData.drivingCycle);
    }

    return true;
}

bool RvdcSignalAdapt::GetMa(Ma_Data &appData, const Ma_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.maId = vdsData.maId;
    appData.maVersion = vdsData.maVersion;

    return true;
}

bool RvdcSignalAdapt::GetMdp(Mdp_Data &appData, const Mdp_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.mdpFormatVersion = vdsData.mdpFormatVersion;
    appData.vin = vdsData.vin;
    GetMa(appData.ma, vdsData.ma);
    appData.sequenceNumber = vdsData.sequenceNumber;
    appData.timestampReadOutStart = vdsData.timestampReadOutStart;
    appData.udsTriggerId_presence = vdsData.udsTriggerId_presence;
    if (vdsData.udsTriggerId_presence)
    {
        appData.udsTriggerId = vdsData.udsTriggerId;
    }
    appData.diagnosticResponse = vdsData.diagnosticResponse;

    return true;
}
bool RvdcSignalAdapt::GetRvdcSetting(RvdcSetting_Data &appData, const RvdcSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.maSetting_presence = vdsData.maSetting_presence;
    if (vdsData.maSetting_presence)
    {
        GetMaSetting(appData.maSetting, vdsData.maSetting);
    }
    appData.vehicleSetting_presence = vdsData.vehicleSetting_presence;
    if (vdsData.vehicleSetting_presence)
    {
        GetRvdcVehicleSetting(appData.vehicleSetting, vdsData.vehicleSetting);
    }
    appData.languageSetting_presence = vdsData.languageSetting_presence;
    if (vdsData.languageSetting_presence)
    {
        GetLanguageSetting(appData.languageSetting, vdsData.languageSetting);
    }
    appData.forcedSetting_presence = vdsData.forcedSetting_presence;
    if (vdsData.forcedSetting_presence)
    {
        GetRvdcForcedSetting(appData.forcedSetting, vdsData.forcedSetting);
    }
    appData.serviceParameters_presence = vdsData.serviceParameters_presence;
    if (vdsData.serviceParameters_presence)
    {
        GetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);
    }

    return true;
}

bool RvdcSignalAdapt::GetRvdcAssignmentNotification(RvdcAssignmentNotification_Data &appData, const RvdcAssignmentNotification_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);	
    appData.maCarId_presence = vdsData.maCarId_presence;
    if (vdsData.maCarId_presence)
    {
        appData.maCarId = vdsData.maCarId;
    }
    appData.maVersion_presence = vdsData.maVersion_presence;
    if (vdsData.maVersion_presence)
    {
        appData.maVersion = vdsData.maVersion;
    }
    appData.newStatus = vdsData.newStatus;
    appData.timestamp = vdsData.timestamp;
    appData.reason_presence = vdsData.reason_presence;
    if (vdsData.reason_presence)
    {
        appData.reason = vdsData.reason;
    }

    return true;
}

bool RvdcSignalAdapt::GetMaSynchronizationReq(MaSynchronizationReq_Data &appData, const MaSynchronizationReq_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.storageQuota = vdsData.storageQuota;
    appData.maFormatVersion = vdsData.maFormatVersion;
    appData.installedMa_presence = vdsData.installedMa_presence;
    appData.installedMa_presence = vdsData.installedMa_presence;
    if (vdsData.installedMa_presence)
    {
        for (auto pInstalledMa = vdsData.installedMa.begin(); pInstalledMa != vdsData.installedMa.end();) 
        {
            Ma_Data installedMa;
            installedMa.maId = pInstalledMa->maId;
            installedMa.maVersion = pInstalledMa->maVersion;
            appData.installedMa.push_back(installedMa);
            ++pInstalledMa;
        }
        
    }
    appData.serviceParameters_presence = vdsData.serviceParameters_presence;
    if (vdsData.serviceParameters_presence)
    {
        GetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);
    }

    return true;
}

bool RvdcSignalAdapt::GetMaSynchronization(MaSynchronization_Data &appData, const MaSynchronization_Model &vdsData)
{
	DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.removeMas_presence = vdsData.removeMas_presence;
    if (vdsData.removeMas_presence)
    {
        for (auto pRemoveMa = vdsData.removeMas.begin(); pRemoveMa != vdsData.removeMas.end();) 
        {
            Ma_Data removeMa;
            removeMa.maId = pRemoveMa->maId;
            removeMa.maVersion = pRemoveMa->maVersion;
            appData.removeMas.push_back(removeMa);
            ++pRemoveMa;
        }
        
    }
    appData.newMas_presence = vdsData.newMas_presence;
    if (vdsData.newMas_presence)
    {
        for (auto pNewMa = vdsData.newMas.begin(); pNewMa != vdsData.newMas.end();) 
        {
            MeasurementAssignment_Data newMa;
            newMa.maId = pNewMa->maId;
            newMa.maVersion = pNewMa->maVersion;
            newMa.vin = pNewMa->vin;
            newMa.maFormatVersion = pNewMa->maFormatVersion;
            newMa.maExecutionPriority = pNewMa->maExecutionPriority;
            GetTelematicsParameters(newMa.telematicsParameters, pNewMa->telematicsParameters);
            newMa.usageModesAllowed = pNewMa->usageModesAllowed;
            newMa.startCondition = pNewMa->startCondition;
            GetStopCondition(newMa.stopCondition, pNewMa->stopCondition);
            newMa.retentiondate = pNewMa->retentiondate;
            GetAuthorizationStatus(newMa.authorizationStatus, pNewMa->authorizationStatus);
            GetExecutionTrigger(newMa.executionTrigger, pNewMa->executionTrigger);
            for (auto pEcusAndPartNumber = pNewMa->ecusAndPartNumbers.begin(); pEcusAndPartNumber != pNewMa->ecusAndPartNumbers.end();) 
            {
                EcusAndPartNumber_Data ecusAndPartNumber;
                ecusAndPartNumber.ecuAddress = pEcusAndPartNumber->ecuAddress;
                ecusAndPartNumber.diagnosticPartNumber = pEcusAndPartNumber->diagnosticPartNumber;
                newMa.ecusAndPartNumbers.push_back(ecusAndPartNumber);
                ++pEcusAndPartNumber;
            }
            
            for (auto pDiagnosticCommand = pNewMa->diagnosticCommand.begin(); pDiagnosticCommand != pNewMa->diagnosticCommand.end();) 
            {
                DiagnosticCommand_Data diagnosticCommand;
                diagnosticCommand.ecuAddress = pDiagnosticCommand->ecuAddress;
                diagnosticCommand.serviceId = pDiagnosticCommand->serviceId;
                diagnosticCommand.input = pDiagnosticCommand->input;
                diagnosticCommand.compareType_presence = pDiagnosticCommand->compareType_presence;
                if (pDiagnosticCommand->compareType_presence)
                {
                    diagnosticCommand.compareType = (App_CompareType)pDiagnosticCommand->compareType;
                }
                diagnosticCommand.compareValue_presence = pDiagnosticCommand->compareValue_presence;
                if (pDiagnosticCommand->compareValue_presence)
                {
                    diagnosticCommand.compareValue = pDiagnosticCommand->compareValue;
                }
                diagnosticCommand.delayUntilNextTriggerCommand = pDiagnosticCommand->delayUntilNextTriggerCommand;
                newMa.diagnosticCommand.push_back(diagnosticCommand);
                ++pDiagnosticCommand;
            }
            
            newMa.dataIntegrityCheck = pNewMa->dataIntegrityCheck;
            newMa.serviceParameters_presence = pNewMa->serviceParameters_presence;
            if (pNewMa->serviceParameters_presence)
            {
                GetServiceParameters(newMa.serviceParameters, pNewMa->serviceParameters);                
            }
            appData.newMas.push_back(newMa);
            ++pNewMa;
        }
        
    }
    appData.serviceParameters_presence = vdsData.serviceParameters_presence;
    if (vdsData.serviceParameters_presence)
    {
        GetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);
    }

    return true;
}

bool RvdcSignalAdapt::GetRvdcExceptionHandling(RvdcExceptionHandling_Data &appData, const RvdcExceptionHandling_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    appData.exceptionName = vdsData.exceptionName;
    appData.description = vdsData.description;
    appData.publisher = vdsData.publisher;
    appData.severity_presence = vdsData.severity_presence;
    if (vdsData.severity_presence)
    {
        appData.severity = vdsData.severity;
    }
    appData.impact_presence = vdsData.impact_presence;
    if (vdsData.impact_presence)
    {
        appData.impact = vdsData.impact;
    }
    appData.action_presence = vdsData.action_presence;
    if (vdsData.action_presence)
    {
        appData.action = vdsData.action;
    }

    return true;
}

bool RvdcSignalAdapt::GetMeasurementDataPackage(MeasurementDataPackage_Data &appData, const MeasurementDataPackage_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    GetTelematicsParameters(appData.telematicsParameters, vdsData.telematicsParameters);
    appData.serviceParameters_presence = vdsData.serviceParameters_presence;
    if (vdsData.serviceParameters_presence)
    {
        GetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);
    }
    GetMdp(appData.mdp, vdsData.mdp);

    return true;;
}

bool RvdcSignalAdapt::GetRVDC(RVDC_Data &appData, const RVDC_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);	
    appData.appId = (App_AppId)vdsData.appId;
    appData.maOrderId = vdsData.maOrderId;
    appData.timestamp = vdsData.timestamp;
    appData.vin = vdsData.vin;
    appData.maBusinessType = (App_MaBusinessType)vdsData.maBusinessType;

    appData.rvdcSetting_presence = vdsData.rvdcSetting_presence;
    if (vdsData.rvdcSetting_presence)
    {
        GetRvdcSetting(appData.rvdcSetting, vdsData.rvdcSetting);
    }
    appData.assignmentNotification_presence = vdsData.assignmentNotification_presence;
    if (vdsData.assignmentNotification_presence)
    {
        GetRvdcAssignmentNotification(appData.assignmentNotification, vdsData.assignmentNotification);
    }
    appData.maSynchronizationReq_presence = vdsData.maSynchronizationReq_presence;
    if (vdsData.maSynchronizationReq_presence)
    {
        GetMaSynchronizationReq(appData.maSynchronizationReq, vdsData.maSynchronizationReq);
    }
    appData.maSynchronization_presence = vdsData.maSynchronization_presence;
    if (vdsData.maSynchronization_presence)
    {
        GetMaSynchronization(appData.maSynchronization, vdsData.maSynchronization);
    }
    appData.exceptionHandling_presence = vdsData.exceptionHandling_presence;
    if (vdsData.exceptionHandling_presence)
    {
        GetRvdcExceptionHandling(appData.exceptionHandling, vdsData.exceptionHandling);
    }
    appData.measurementDataPackage_presence = vdsData.measurementDataPackage_presence;
    if (vdsData.measurementDataPackage_presence)
    {
        GetMeasurementDataPackage(appData.measurementDataPackage, vdsData.measurementDataPackage);
    }

    return true;
}

bool RvdcSignalAdapt::SetServiceParameters(const std::vector<ServiceParameter_Data> &appData, std::vector<ServiceParameter_Model> &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
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
bool RvdcSignalAdapt::SetAuthorizationStatus(const AuthorizationStatus_Data &appData, AuthorizationStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.dataCollection_presence = appData.dataCollection_presence;
    if (appData.dataCollection_presence)
    {
        vdsData.dataCollection = appData.dataCollection;
    }
    vdsData.remoteDiagnostics_presence = appData.remoteDiagnostics_presence;
    if (appData.remoteDiagnostics_presence)
    {
        vdsData.remoteDiagnostics = appData.remoteDiagnostics;
    }

    
    return result;
}

bool RvdcSignalAdapt::SetMaSetting(const MaSetting_Data &appData, MaSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.collectFunction = appData.collectFunction;
    SetAuthorizationStatus(appData.authorizationStatus, vdsData.authorizationStatus);

    
    return result;
}

bool RvdcSignalAdapt::SetRvdcVehicleSetting(const RvdcVehicleSetting_Data &appData, RvdcVehicleSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.rvdcFunction = appData.rvdcFunction;
    vdsData.collectOffSetTime = appData.collectOffSetTime;

    
    return result;
}

bool RvdcSignalAdapt::SetLanguageSetting(const LanguageSetting_Data &appData, LanguageSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.language = (Vds_Language)appData.language;

    
    return result;
}

bool RvdcSignalAdapt::SetRvdcForcedSetting(const RvdcForcedSetting_Data &appData, RvdcForcedSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.visibility = appData.visibility;
    vdsData.suppress = appData.suppress;
    vdsData.lock = appData.lock;

    
    return result;
}

bool RvdcSignalAdapt::SetRvdcSetting(const RvdcSetting_Data &appData, RvdcSetting_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.maSetting_presence = appData.maSetting_presence;
    if (appData.maSetting_presence)
    {
        SetMaSetting(appData.maSetting, vdsData.maSetting);
    }
    vdsData.vehicleSetting_presence = appData.vehicleSetting_presence;
    if (appData.vehicleSetting_presence)
    {
        SetRvdcVehicleSetting(appData.vehicleSetting, vdsData.vehicleSetting);
    }
    vdsData.languageSetting_presence = appData.languageSetting_presence;
    if (appData.languageSetting_presence)
    {
        SetLanguageSetting(appData.languageSetting, vdsData.languageSetting);
    }
    vdsData.forcedSetting_presence = appData.forcedSetting_presence;
    if (appData.forcedSetting_presence)
    {
        SetRvdcForcedSetting(appData.forcedSetting, vdsData.forcedSetting);
    }
    vdsData.serviceParameters_presence = appData.serviceParameters_presence;
    if (appData.serviceParameters_presence)
    {
        SetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);        
    }

    
    return result;
}

bool RvdcSignalAdapt::SetTimeStamp(const TimeStamp_Data &appData, TimeStamp_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.seconds = appData.seconds;
    if (appData.milliseconds_presence)
    {
        vdsData.milliseconds_presence = true;
        vdsData.milliseconds = appData.milliseconds;
    }

    
    return result;
}

bool RvdcSignalAdapt::SetTelematicsParameters(const TelematicsParameters_Data &appData, TelematicsParameters_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.timeToLive = appData.timeToLive;
    vdsData.uploadPriority = appData.uploadPriority;

    
    return result;
}

bool RvdcSignalAdapt::SetMa(const Ma_Data &appData, Ma_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.maId = appData.maId;
    vdsData.maVersion = appData.maVersion;

    
    return result;
}

bool RvdcSignalAdapt::SetMdp(const Mdp_Data &appData, Mdp_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.mdpFormatVersion = appData.mdpFormatVersion;
    vdsData.vin = appData.vin;
    SetMa(appData.ma, vdsData.ma);
    vdsData.sequenceNumber = appData.sequenceNumber;
    vdsData.timestampReadOutStart = appData.timestampReadOutStart;
    vdsData.udsTriggerId_presence = appData.udsTriggerId_presence;
    if (appData.udsTriggerId_presence)
    {
        vdsData.udsTriggerId = appData.udsTriggerId;
    }
    vdsData.diagnosticResponse = appData.diagnosticResponse;

    
    return result;
}

bool RvdcSignalAdapt::SetRvdcAssignmentNotification(const RvdcAssignmentNotification_Data &appData, RvdcAssignmentNotification_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.maCarId_presence = appData.maCarId_presence;
    if (appData.maCarId_presence)
    {
        vdsData.maCarId = appData.maCarId;
    }
    vdsData.maVersion_presence = appData.maVersion_presence;
    if (appData.maVersion_presence)
    {
        vdsData.maVersion = appData.maVersion;
    }
    vdsData.newStatus = appData.newStatus;
    vdsData.timestamp = appData.timestamp;
    vdsData.reason_presence = appData.reason_presence;
    if (appData.reason_presence)
    {
        vdsData.reason = appData.reason;
    }
    
    return result;
}

bool RvdcSignalAdapt::SetMaSynchronizationReq(const MaSynchronizationReq_Data &appData, MaSynchronizationReq_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.storageQuota = appData.storageQuota;
    vdsData.maFormatVersion = appData.maFormatVersion;
    vdsData.installedMa_presence = appData.installedMa_presence;
    if (appData.installedMa_presence)
    {
        for (auto p_installedMa = appData.installedMa.begin(); p_installedMa != appData.installedMa.end();) 
        {
            Ma_Model installedMa;
            installedMa.maId = p_installedMa->maId;
            installedMa.maVersion = p_installedMa->maVersion;
            vdsData.installedMa.push_back(installedMa);

        
            ++p_installedMa;
        }
        
    }
    vdsData.serviceParameters_presence = appData.serviceParameters_presence;
    if (appData.serviceParameters_presence)
    {
        SetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);
    }

    
    return result;
}

bool RvdcSignalAdapt::SetMeasurementDataPackage(const MeasurementDataPackage_Data &appData, MeasurementDataPackage_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    SetTelematicsParameters(appData.telematicsParameters, vdsData.telematicsParameters);
    vdsData.serviceParameters_presence = appData.serviceParameters_presence;
    if (appData.serviceParameters_presence)
    {
        SetServiceParameters(appData.serviceParameters, vdsData.serviceParameters);        
    }
    SetMdp(appData.mdp, vdsData.mdp);

    
    return result;
}

bool RvdcSignalAdapt::SetRVDC(const RVDC_Data &appData, RVDC_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.appId = (Vds_AppId)appData.appId;
    vdsData.maOrderId = appData.maOrderId;
    vdsData.timestamp = appData.timestamp;
    vdsData.vin = appData.vin;
    vdsData.maBusinessType = (Vds_MaBusinessType)appData.maBusinessType;
    vdsData.rvdcSetting_presence = appData.rvdcSetting_presence;
    if (appData.rvdcSetting_presence)
    {
        SetRvdcSetting(appData.rvdcSetting, vdsData.rvdcSetting);
    }
    vdsData.assignmentNotification_presence = appData.assignmentNotification_presence;
    if (appData.assignmentNotification_presence)
    {
        SetRvdcAssignmentNotification(appData.assignmentNotification, vdsData.assignmentNotification);
    }
    vdsData.maSynchronizationReq_presence =  appData.maSynchronizationReq_presence;
    if (appData.maSynchronizationReq_presence)
    {
        SetMaSynchronizationReq(appData.maSynchronizationReq, vdsData.maSynchronizationReq);
    }
    vdsData.maSynchronization_presence = false; //appData.maSynchronization_presence
    if (appData.maSynchronization_presence)
    {
//        vdsData.maSynchronization_presence = true;
//        SetMaSynchronization(appData.maSynchronization, vdsData.maSynchronization);
    }
    vdsData.exceptionHandling_presence = false; //appData.exceptionHandling_presence;
    if (appData.exceptionHandling_presence)
    {
//        vdsData.exceptionHandling_presence = true;
//        SetRvdcExceptionHandling(appData.exceptionHandling, vdsData.exceptionHandling);
    }
    vdsData.measurementDataPackage_presence = appData.measurementDataPackage_presence;
    if (appData.measurementDataPackage_presence)
    {
        SetMeasurementDataPackage(appData.measurementDataPackage, vdsData.measurementDataPackage);
    }
    
    return result;
}

bool RvdcSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s\n", __FUNCTION__);
    
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new RVDC_Data();
    RVDC_Data *rvdcData = dynamic_cast<RVDC_Data*>(appData);
    if(rvdcData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s rvdcData == nullptr.\n", __FUNCTION__);
        return false;
    }

    rvdcData->serviceId = vdsData->body.serviceId;
    rvdcData->serviceCommand = vdsData->body.serviceData.serviceCommand;
    GetRVDC(*rvdcData, vdsData->body.serviceData.rvdc);
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s completed.\n", __FUNCTION__);
    
    return true;
}

bool RvdcSignalAdapt::SetHeadPayload(VDServiceRequest_Model &vdsData, const RequestHeader_Data &appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);

    vdsData.header.timeStamp.seconds = appData.timeStampSeconds;
    vdsData.header.timeStamp.milliseconds_presence = appData.timeStampMilliseconds_presence;
    vdsData.header.timeStamp.milliseconds= appData.timeStampMilliseconds;
    vdsData.header.creatorId_presence = appData.creatorId_presence;
    vdsData.header.creatorId =static_cast<Vds_CreatorId>(appData.creatorId);
    vdsData.header.messageTTL_presence = appData.messageTTL_presence;
    vdsData.header.messageTTL = appData.messageTTL;
    vdsData.header.requestType_presence = true;
    vdsData.header.requestType = Vds_serviceRequest;
    vdsData.header.ackRequired_presence = appData.ackRequired_presence;
    vdsData.header.ackRequired = appData.ackRequired;
    vdsData.header.requestid = appData.requestid;

    if(!appData.isInitiator)
    {
        vdsData.header.eventId_presence = appData.eventId_presence;
        vdsData.header.eventId.seconds = appData.eventIdSeconds;
        vdsData.header.eventId.milliseconds_presence = appData.eventIdMilliseconds_presence;
        vdsData.header.eventId.milliseconds= appData.eventIdMilliseconds;
    }
    else
    {
        vdsData.header.eventId_presence = true;
        vdsData.header.eventId.seconds = vdsData.header.timeStamp.seconds ;
        vdsData.header.eventId.milliseconds_presence = true;
        vdsData.header.eventId.milliseconds= vdsData.header.timeStamp.milliseconds;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s completed.\n", __FUNCTION__);
    
    return true;
}

bool RvdcSignalAdapt::SetRvdcPayload(const RVDC_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s.payload.header.req = %d", __FUNCTION__, payload.header.requestid);
    
    VDServiceRequest_Model vdsData;
    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_rvc;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.rvdc_presence = true;
    SetRVDC(payload, vdsData.body.serviceData.rvdc);
    
    bool result = PackService(vdsData, payload);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s PackService failed.", __FUNCTION__);
        return result;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvdcSignalAdapt::%s completed.", __FUNCTION__);
    return result;
}

}

