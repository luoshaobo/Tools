///////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file car_downloadagent_transaction.cpp
//  This file handles the OTA
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author
// @Init date   07-Feb-2019
///////////////////////////////////////////////////////////////////
#include <algorithm>
#include "download_agent/car_downloadagent_transaction.h"
#include "dlt/dlt.h"
#include "local_config_interface.hpp"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "download_agent/car_downloadagent_fileio.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

#define TIMESTAMP_MAX_LENGTH (128)
#define TIMESTAMP_MICROSECOND_LENGTH (3)
#define BUFFER_SIZE_128 (128)

CarDownloadAgentTransaction::CarDownloadAgentTransaction () : fsm::SmartTransaction(kStart),
    m_OTAService(fsm::IpCommandBrokerSignalSource::GetInstance().GetIpcbIOTAServiceObject()),
    m_configurationStatus(""),
    m_downloadConsent(false),
    m_installationOrderId(""),
    m_availablestorage(0),
    m_installationconsent(0)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    StateMap state_map = {{kStart,              {nullptr,               // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kHandleSignal, kStop}}}, // valid transitions
                          {kHandleSignal,       {nullptr,               // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kCanceled, kFailed, kComplete, kStop}}}, // valid transitions
                          {kCanceled,           {nullptr,               // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kHandleSignal, kStop}}},             // valid transitions
                          {kFailed,             {nullptr,               // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kHandleSignal, kStop}}},             // valid transitions
                          {kComplete,           {nullptr,               // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kHandleSignal, kStop}}},             // valid transitions
                          {kRejected,             {nullptr,             // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kHandleSignal, kStop}}},             // valid transitions
                          {kStop,               {nullptr,               // state function
                                                 SignalFunctionMap(),   // signal function map
                                                 {kHandleSignal, kStart}}}};           // valid transitions

    state_map[kStart].state_function = std::bind(&CarDownloadAgentTransaction::init, this);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAAssignmentNotification] =
        std::bind(&CarDownloadAgentTransaction::HandleOTAAssignmentNotification, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAAssignmentSyncReq] =
        std::bind(&CarDownloadAgentTransaction::HandleOTAAssignmentSyncRequest, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAHMILanguageSettingsNotification] =
        std::bind(&CarDownloadAgentTransaction::HandleOTAHMILanguageSettings, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kDownloadConsentNotification] =
        std::bind(&CarDownloadAgentTransaction::HandleDownloadConsent, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::kVDServiceFota] =
        std::bind(&CarDownloadAgentTransaction::HandleVdsFOTASignal, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAQueryAvailableStorageResp] =
        std::bind(&CarDownloadAgentTransaction::HandleOTAQueryAvailableStorageResp, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAInstallationSummaryNotification] =
        std::bind(&CarDownloadAgentTransaction::HanldeInstallationSummaryNotification, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kInstallationConsentNotification] =
        std::bind(&CarDownloadAgentTransaction::HandleInstallationConsent, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAAssignBOOTResp] =
        std::bind(&CarDownloadAgentTransaction::HandleOTAAssignBOOTResp, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAWriteAssignmentDataResp] =
        std::bind(&CarDownloadAgentTransaction::HandleWriteAssignmentDataResponse, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kHttpSignal] =
        std::bind(&CarDownloadAgentTransaction::HandleHttpGetCallbackMesage, this, std::placeholders::_1);
    state_map[kHandleSignal].signal_function_map[fsm::Signal::kOTAExceptionReportNotification] =
        std::bind(&CarDownloadAgentTransaction::HandleExceptionReportNotification, this, std::placeholders::_1);

    // will be modify start
    state_map[kCanceled].state_function = std::bind(&CarDownloadAgentTransaction::init, this);
    state_map[kFailed].state_function = std::bind(&CarDownloadAgentTransaction::init, this);
    state_map[kComplete].state_function = std::bind(&CarDownloadAgentTransaction::init, this);
    state_map[kRejected].state_function = std::bind(&CarDownloadAgentTransaction::init, this);
    // will be modify end

    SetStateMap(state_map);

    // register signal
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAAssignmentNotification);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAAssignmentSyncReq);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAHMILanguageSettingsNotification);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kDownloadConsentNotification);
    fsm::SmartTransaction::MapSignalType(fsm::kVDServiceFota);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAQueryAvailableStorageResp);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAInstallationSummaryNotification);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kInstallationConsentNotification);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAAssignBOOTResp);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAWriteAssignmentDataResp);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kHttpSignal);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kOTAExceptionReportNotification);

    init();
    clearDownloadStatus();

    // Load persist data
    LoadPersistData();
}

void CarDownloadAgentTransaction::init()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    m_language = (uint32_t)app_chineseSimpMan;
    SetState(kHandleSignal);
}

bool CarDownloadAgentTransaction::HandleOTAAssignmentNotification(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::AssignmentNotificationDataSignal> assignmentSignal =
                    std::static_pointer_cast<fsm::AssignmentNotificationDataSignal>(signal);
    const OpOTAAssignmentNotification_Notification_Data &data = assignmentSignal->GetPayload();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: installationorder [%s]", data.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: timestamp [%s]", data.timestamp.timestamp1.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: newstatus [%s]", data.newstatus.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: reason_presence [%d]", data.reason_presence);
#if 0 // The requirement has been changed and will be modified in the next requirement.
    bool result = false;
    if (data.newstatus == "LOCAL_STORAGE_ERROR")
    {
        // Send FotaAssignmentNotification to TSP
        fsm::FotaAssignmentNotification_Data assignmentNotification = {};
        assignmentNotification.installationOrderId_presence = true;
        assignmentNotification.installationOrderId = m_installationOrderId;
        assignmentNotification.timestamp = GetNowTimeStamp();
        // will be modify "_"
        assignmentNotification.newStatus = "DOWNLOAD-FAILED";
        if (data.reason_presence)
        {
            assignmentNotification.reason_presence = data.reason_presence;
            assignmentNotification.reason = data.reason;
        }

        result = SendAssignmentNotificationData(assignmentNotification);

        // Send OTATCAMAssignmentNotification to IHU
        OpOTATCAMAssignmentNotification_Notification_Data msg;
        msg.installationorder.uuid = m_installationOrderId;
        msg.timestamp.timestamp1 = assignmentNotification.timestamp;
        msg.newstatus = assignmentNotification.newStatus;
        msg.reason_presence = assignmentNotification.reason_presence;
        msg.reason = assignmentNotification.reason;

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: uuid:%s", msg.installationorder.uuid.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: timestamp1:%s", msg.timestamp.timestamp1.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: newstatus:%s", msg.newstatus.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: reason_presence:%d", msg.reason_presence);

        result = m_OTAService.sendOTAServiceData(OperationId::OTATCAMAssignmentNotification,
                                                 OperationType::NOTIFICATION,
                                                 0,
                                                 msg);
        if (result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send TCAMAssignment notification data success.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send TCAMAssignment notification data fail.");
        }

        SetState(kFailed);
    }

    return result;
#else
    return true;
#endif
}

bool CarDownloadAgentTransaction::HandleOTAUpdateInventoryInstructionsResponse(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    return true;
}

bool CarDownloadAgentTransaction::HandleOTAClientConfigurationSyncRequest(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    return true;
}

bool CarDownloadAgentTransaction::HandleOTAAssignmentSyncRequest(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    // send available assignmen request to TSP
    fsm::AvailableAssignmentRequest_Data msg;
    msg.languageSetting.language = (App_Language)m_language;
    msg.header.isInitiator = true;
    SendAvailableAssignmentRequestData(msg);

    return true;
}

bool CarDownloadAgentTransaction::HandleOTAHMILanguageSettings(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    std::shared_ptr<fsm::OTAHMILanguageSettingsSignal> settingSignal =
                    std::static_pointer_cast<fsm::OTAHMILanguageSettingsSignal>(signal);
    const OpOTAHMILanguageSettings_Notification_Data &data = settingSignal->GetPayload();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "language [%d]", data.languagesetting);
    m_language = (uint32_t)data.languagesetting;
    return true;
}

bool CarDownloadAgentTransaction::HandleDownloadConsent(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::DownloadConsentSignal> kSignal =
                    std::static_pointer_cast<fsm::DownloadConsentSignal>(signal);
    const OpDownloadConsent_Notification_Data &data = kSignal->GetPayload();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: uuid [%s]", data.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: downloadconsent [%d]", data.downloadconsent);

    m_installationOrderId = data.installationorder.uuid;
    m_downloadConsent = data.downloadconsent;

    vocpersistdatamgr::VocPersistDataMgr& vocpersist = vocpersistdatamgr::VocPersistDataMgr::GetInstance();
    bool result = vocpersist.SetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::DLA_DOWNLOAD_CONSENT, m_downloadConsent);
    if (!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: Set downloadconsent error. [%d]", m_downloadConsent);
    }

    // Send assignment notification
    fsm::FotaAssignmentNotification_Data assignmentNotification = {};
    assignmentNotification.installationOrderId_presence = true;
    assignmentNotification.installationOrderId = m_installationOrderId;
    assignmentNotification.timestamp = GetNowTimeStamp();
    // will be modify "_"
    if (m_downloadConsent)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "DOWNLOAD-CONSENT-GRANTED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "USER";
    }
    else
    {
        // will be modify "_"
        assignmentNotification.newStatus = "DOWNLOAD-CONSENT-REVOKED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "USER";
    }

    result = SendAssignmentNotificationData(assignmentNotification);

    if (!data.downloadconsent)
    {
        SetState(kRejected);
    }
    else
    {
        // Configuration Status OK ?
        if (m_configurationStatus != "OK")
        {
            result = ConfirmConfigurationStatus(m_configurationStatus);
        }
        else
        {
            // Send OTAQueryAvailableStorage
            result = SendOTAQueryAvailableStorage();
        }
    }

    return result;
}

bool CarDownloadAgentTransaction::HandleVdsFOTASignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s: begin", __FUNCTION__);
    std::shared_ptr<GlyVdsDLAgentSignal> kSignal =
                    std::static_pointer_cast<GlyVdsDLAgentSignal>(signal);
    std::shared_ptr<fsm::FOTA_Data> data = kSignal->getFOTAData();
    if (data->dataType == app_DataType_availableAssignment && data->availableAssignment_presence)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s: availableAssignment data received", __FUNCTION__);
        SendAvailableAssignmentData(data->availableAssignment, data->serviceCommand);
    }
    else if (data->dataType == app_DataType_assignmentData && data->assignmentData_presence)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s: assignmentData data received", __FUNCTION__);
        HanldeOTAAssignmentData(data->assignmentData);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: %s: handleVdsFOTASignal failed", __FUNCTION__);
    }
    return true;
}

bool CarDownloadAgentTransaction::HandleOTAQueryAvailableStorageResp(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::QueryAvailableStorageRespSignal> kSignal =
                    std::static_pointer_cast<fsm::QueryAvailableStorageRespSignal>(signal);
    const OpOTAQueryAvailableStorage_Response_Data &data = kSignal->GetPayload();
    m_availablestorage = data.availablestorage;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: availablestorage [%llu]", data.availablestorage);
    return true;
}

bool CarDownloadAgentTransaction::HanldeInstallationSummaryNotification(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarDownloadAgentTransaction::%s", __FUNCTION__);
    std::shared_ptr<fsm::InstallationSummaryNotificationSignal> settingSignal =
                    std::static_pointer_cast<fsm::InstallationSummaryNotificationSignal>(signal);

    const OpOTAInstallationSummary_Notification_Data &data = settingSignal->GetPayload();

    fsm::InstallationSummary_Data msg;
    msg.header.isInitiator = true;
    Convert(data,msg);

    SendInstallationSummaryNotificationData(msg);

    CarDownloadAgentFileIO writer;
    const char* outFile = "/data/OTAInstallationSummary.xml";
    if(writer.WriteData(data,outFile))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: write file ok: %s",outFile);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: write file fail: %s",outFile);
    }

    return true;
}

bool CarDownloadAgentTransaction::HandleExceptionReportNotification(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarDownloadAgentTransaction::%s", __FUNCTION__);
    std::shared_ptr<fsm::OTAExceptionReportNotificationSignal> exceptionReportNotificationSignal =
                    std::static_pointer_cast<fsm::OTAExceptionReportNotificationSignal>(signal);

    const OpOTAExceptionReports_Notification_Data &data = exceptionReportNotificationSignal->GetPayload();

    fsm::EcuExceptionReport_Data msg;
    msg.header.isInitiator = true;
    Convert(data,msg);
    SendExceptionReportNotificationData(msg);

    CarDownloadAgentFileIO writer;
    const char* outFile = "/data/OTAExceptionReport.xml";
    if(writer.WriteData(data,outFile))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: write file ok: %s",outFile);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: write file fail: %s",outFile);
    }

    return true;
}

bool CarDownloadAgentTransaction::HandleInstallationConsent(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::InstallationConsentSignal> kSignal =
                    std::static_pointer_cast<fsm::InstallationConsentSignal>(signal);
    const OpInstallationConsent_Notification_Data &data = kSignal->GetPayload();
    m_installationOrderId = data.installationorder.uuid;
    m_installationconsent = data.installationconsent;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: installationorder [%s]", data.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: installationconsent [%d]", data.installationconsent);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: deltatime_presence [%d]", data.deltatime_presence);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: deltatime [%d]", data.deltatime);

    // Send AssignmentNotification to TSP
    fsm::FotaAssignmentNotification_Data assignmentNotification = {};
    assignmentNotification.installationOrderId_presence = true;
    assignmentNotification.installationOrderId = m_installationOrderId;
    assignmentNotification.timestamp = GetNowTimeStamp();

    if (data.installationconsent == Ipcb_Installationconsent_granted)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-CONSENT-GRANTED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "USER";
    }
    else if (data.installationconsent == Ipcb_Installationconsent_revoked)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-CONSENT-REVOKED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "USER";
    }

    if (data.deltatime_presence)
    {
        assignmentNotification.deltaTime_presence = data.deltatime_presence;
        assignmentNotification.deltaTime = data.deltatime;
    }

    bool result = SendAssignmentNotificationData(assignmentNotification);

    // Send OTAAssignBOOT request
    OpOTAAssignBOOT_Request_Data msg = {};
    msg.installationorder.uuid = m_installationOrderId;
    if (data.installationconsent == Ipcb_Installationconsent_granted)
    {
        msg.requestcode = Ipcb_RequestCode_granted;
    }
    else if (data.installationconsent == Ipcb_Installationconsent_revoked)
    {
        msg.requestcode = Ipcb_RequestCode_revoked;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: uuid:%s", msg.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: requestcode:%d", msg.requestcode);

    result = m_OTAService.sendOTAServiceData(OperationId::OTAAssignBOOT,
                                             OperationType::REQUEST,
                                             0,
                                             msg);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send OTAAssignBOOT request data success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send OTAAssignBOOT request data fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::HandleOTAAssignBOOTResp(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::OTAAssignBOOTRespSignal> kSignal =
                    std::static_pointer_cast<fsm::OTAAssignBOOTRespSignal>(signal);
    const OpOTAAssignBOOT_Response_Data &data = kSignal->GetPayload();

    m_installationOrderId = data.installationorder.uuid;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: installationorder [%s]", data.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: responsecode [%d]", data.responsecode);

    // Send assignmentNotification to TSP
    fsm::FotaAssignmentNotification_Data assignmentNotification = {};
    assignmentNotification.installationOrderId_presence = true;
    assignmentNotification.installationOrderId = m_installationOrderId;
    assignmentNotification.timestamp = GetNowTimeStamp();

    if (data.responsecode == Ipcb_accepted)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-PENDING";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "ASSIGN-BOOT-ACCEPTED";
    }
    else if (data.responsecode == Ipcb_alreadyassigned)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-STARTED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "BOOT";
    }
    else if (data.responsecode == Ipcb_notfound)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-DEFERRED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "ASSIGN-BOOT-DENIED-NOT-FOUND";
    }
    else if (data.responsecode == Ipcb_serviceunavailable)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-DEFERRED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "ASSIGN-BOOT-DENIED-SERVICE-NA";
    }
    else if (data.responsecode == Ipcb_localstoragenotaccessible)
    {
        // will be modify "_"
        assignmentNotification.newStatus = "INSTALLATION-DEFERRED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "ASSIGN-BOOT-DENIED-LOCAL-STORAGE";
    }

    bool result = SendAssignmentNotificationData(assignmentNotification);

    // Send OTATCAMAssignmentNotification to IHU
    OpOTATCAMAssignmentNotification_Notification_Data msg = {};
    msg.installationorder.uuid = m_installationOrderId;
    msg.timestamp.timestamp1 = assignmentNotification.timestamp;
    msg.newstatus = assignmentNotification.newStatus;
    msg.reason_presence = assignmentNotification.reason_presence;
    msg.reason = assignmentNotification.reason;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: uuid:%s", msg.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: timestamp1:%s", msg.timestamp.timestamp1.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: newstatus:%s", msg.newstatus.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: reason_presence:%d", msg.reason_presence);

    result = m_OTAService.sendOTAServiceData(OperationId::OTATCAMAssignmentNotification,
                                             OperationType::NOTIFICATION,
                                             0,
                                             msg);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send TCAMAssignment notification data success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send TCAMAssignment notification data fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::Convert(const OpOTAInstallationSummary_Notification_Data& data,fsm::InstallationSummary_Data& msg)
{
   bool result = true;
   msg.installationOrderId = data.installationsummary.installationinorderid.uuid;
   msg.timestamp = data.installationsummary.timestamp.timestamp1;
   msg.repeatresets = data.installationsummary.repeatresets;
   msg.totalInstallationTime = data.installationsummary.totalinstallationtime;

   std::vector<Ecusummarydata_Data> ecusummarys = data.installationsummary.ecusummary;

   std::for_each(ecusummarys.begin(),ecusummarys.end(),[&msg](const Ecusummarydata_Data& dataEcuSummary){
     fsm::EcuSummary_Data msgEcuSummary;
     msgEcuSummary.ecuAddress = dataEcuSummary.ecu.addr;
     msgEcuSummary.ecuRetries = dataEcuSummary.ecuretries;
     msgEcuSummary.ecuStatus = dataEcuSummary.ecuinstallationstatus;

     const std::vector<SoftwarePartInstallationsummaryData_Data>& dataSoftWareSummarys = dataEcuSummary.softwarepartinstallationsummary;
     std::for_each(dataSoftWareSummarys.begin(),dataSoftWareSummarys.end(),[&msgEcuSummary]
        (const SoftwarePartInstallationsummaryData_Data& dataEcuSummary){

        fsm::SoftwarePartSummary_Data msgSoftwareSummary;
        msgSoftwareSummary.partIdentifier = dataEcuSummary.partidentifier.partid;
        msgSoftwareSummary.partRetries = dataEcuSummary.softwarepartretries;
        msgSoftwareSummary.measuredInstallationTime = dataEcuSummary.measuredinstallationtime;
        msgSoftwareSummary.softwarePartStatus = dataEcuSummary.softwarepartinstallationstatus;

        msgEcuSummary.softwarePartSummarys.push_back(msgSoftwareSummary);
     });

     msg.ecuSummarys.push_back(msgEcuSummary);
   });

   return result;
}

bool CarDownloadAgentTransaction::Convert(const OpOTAExceptionReports_Notification_Data& data,fsm::EcuExceptionReport_Data& msg)
{
   bool result = true;
   msg.exceptionReportMsgRemaining = data.exceptionreportmsgremaining;
   for(auto it = data.exceptionreports.begin(); it != data.exceptionreports.end(); ++it)
   {
       fsm::ExceptionReport_Data exportReport;

       exportReport.installationOrderId_presence = (*it).installationorderid.id_choice;
       if(exportReport.installationOrderId_presence)
       {
           exportReport.installationOrderId = (*it).installationorderid.id.uuid;
       }
       exportReport.timestamp = (*it).timestamp.timestamp1;
       exportReport.issuerId = (*it).issuerid;
       exportReport.clientConfigurationId_presence = (*it).clientconfigurationid.version_choice;
       if(exportReport.clientConfigurationId_presence)
       {
           exportReport.clientConfigurationId = (*it).clientconfigurationid.version.uuid;
       }
       exportReport.dataFileName_presence = (*it).datafilename.file_choice;
       if(exportReport.dataFileName_presence)
       {
           exportReport.dataFileName = (*it).datafilename.file.name;
       }
       exportReport.ecuAddress_presence = (*it).addr.addr_choice;
       if(exportReport.ecuAddress_presence)
       {
           exportReport.ecuAddress = (*it).addr.addr.addr;
       }
       exportReport.validationKey_presence = (*it).key.key_choice;
       if(exportReport.validationKey_presence)
       {
           exportReport.validationKey = (*it).key.key.key;
       }
       exportReport.partIdentifier_presence = (*it).softwarepartidentifier.id_choice;
       if(exportReport.partIdentifier_presence)
       {
           exportReport.partIdentifier = (*it).softwarepartidentifier.id.partid;
       }
       exportReport.dataBlockNumber_presence = (*it).datablock.blockNumber_choice;
       if(exportReport.dataBlockNumber_presence)
       {
           char *endpoint = nullptr;
           exportReport.dataBlockNumber = std::strtol((*it).datablock.blockNumber.c_str(), &endpoint, 10);
       }
       exportReport.exceptionMessage.activity = (*it).exceptionmessage.activity;
       exportReport.exceptionMessage.action = (*it).exceptionmessage.action;
       exportReport.exceptionMessage.exception = (*it).exceptionmessage.exception;

       msg.exceptionReports.push_back(exportReport);
   }

   return result;
}

bool CarDownloadAgentTransaction::SendInstallationSummaryNotificationData(const fsm::InstallationSummary_Data& msg)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);
    fsm::VdServiceTransactionId vdsTransId;
    void *vdmsg = nullptr;
    std::shared_ptr<GlyVdsDLAgentSignal> signal = std::dynamic_pointer_cast<GlyVdsDLAgentSignal> (GlyVdsDLAgentSignal::CreateGlyVdsDLAgentSignal(vdsTransId, vdmsg));
    bool result = signal->SetInstallationSummaryNotificationPayload(msg);
    if (!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Set InstallationSummary notification payload fail.");
        return result;
    }

    fsm::VocmoSignalSource vocmo_signal_source;
    result = vocmo_signal_source.GeelySendMessage(signal);
    if(!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: GeelySendMessage InstallationSummary fail.");
        return result;
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: GeelySendMessage InstallationSummary ok.");
    }

    return result;
}

bool CarDownloadAgentTransaction::HandleHttpGetCallbackMesage(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::HttpSignal> kSignal =
                    std::static_pointer_cast<fsm::HttpSignal>(signal);
    const HttpData &data = kSignal->GetPayload();

    if (m_downloadStatus.fileSize == 0){
        m_downloadStatus.fileSize = data.fileSize;
        m_downloadStatus.chunks = (data.fileSize + m_downloadStatus.chunkSize -1) / m_downloadStatus.chunkSize;
    }
    SendFileData(data);
    return true;
}

bool CarDownloadAgentTransaction::HandleWriteAssignmentDataResponse(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    std::shared_ptr<fsm::OTAWriteAssignmentDataRespSignal> kSignal =
                    std::static_pointer_cast<fsm::OTAWriteAssignmentDataRespSignal>(signal);
    const OpOTAWriteAssignmentData_Response_Data &data = kSignal->GetPayload();

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %d", data.chunkNumber);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %d", data.status);

    if (m_downloadStatus.chunkNumber >= m_downloadStatus.chunks)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s download file success(fileName:%s)",
                                               __FUNCTION__,
                                               m_downloadStatus.fileName.c_str());
        if (m_downloadStatus.curentUrl >= m_downloadStatus.urls.size())
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s download complete", __FUNCTION__);
            return true;
        }

        // download next file
        m_downloadStatus.chunkNumber = 0;
        m_downloadStatus.chunks = 0;
        m_downloadStatus.fileName.clear();
        m_downloadStatus.fileSize = 0;
        m_downloadStatus.curentUrl++;
    } else {
        // Get next chunk data
        m_downloadStatus.chunkNumber++;
    }

    // Get chunk data by http1.1
    char buffer[BUFFER_SIZE_128] = {0};
    uint64_t RangeMinNum = m_downloadStatus.chunkNumber * m_downloadStatus.chunkSize;
    uint64_t RangeMaxNum = RangeMinNum + m_downloadStatus.chunkSize -1;
    sprintf(buffer, "Range:bytes=%llu-%llu", RangeMinNum, RangeMaxNum);
    std::vector<std::string> headers;
    headers.push_back(buffer);
    m_downloadStatus.pHttpGet->get(m_downloadStatus.urls.at(m_downloadStatus.curentUrl), headers);

    return true;
}

bool CarDownloadAgentTransaction::HanldeOTAAssignmentData(fsm::AssignmentData_Data &data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    clearDownloadStatus();

    // register http callback
    m_downloadStatus.pHttpGet = std::make_shared<fsm::t_http_get>(std::bind(&CarDownloadAgentTransaction::httpCallBack, this, _1));
    if (m_downloadStatus.pHttpGet == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA:%s: make_shared<fsm::t_http_get> failed.", __FUNCTION__);
        return false;
    }

    int ret = m_downloadStatus.pHttpGet->init();
    if (0 != ret)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA:%s: http init failed(%d).", __FUNCTION__, ret);
        return false;
    }

    // sendStartDownload notification
    sendStartDownloadNotification(data.installationOrderId, data.timestamp);

    // start download files
    m_downloadStatus.urls = data.urls;
    if (m_downloadStatus.urls.size() > 0){
        char buffer[BUFFER_SIZE_128] = {0};
        uint64_t RangeMinNum = m_downloadStatus.chunkNumber * m_downloadStatus.chunkSize;
        uint64_t RangeMaxNum = RangeMinNum + m_downloadStatus.chunkSize -1;
        sprintf(buffer, "Range:bytes=%llu-%llu", RangeMinNum, RangeMaxNum);
        std::vector<std::string> headers;
        headers.push_back(buffer);
        std::string urlstr = m_downloadStatus.urls.at(m_downloadStatus.curentUrl);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA:%s: http-get url:%s.", __FUNCTION__, urlstr.c_str());
        m_downloadStatus.pHttpGet->get(urlstr, headers);
    }

    return true;
}

bool CarDownloadAgentTransaction::SendAvailableAssignmentData(fsm::AvailableAssignment_Data &data, App_ServiceCommand command)
{
    OpOTAAssignmentSync_Response_Data respData = {};
    respData.installationorder.uuid = data.installationOrderId;
    respData.timestamp.timestamp1 = data.timestamp;
    respData.workshop_installiation = data.workshopInstallation;
    respData.downloadsize = data.downloadsize;
    respData.totalinstallation_time = data.totalinstallationTime;
    respData.newstatus = data.newstatus;
    respData.title = data.name;
    respData.description = data.shortDescription;
    respData.reason_presence = data.reason_presence;
    if (respData.reason_presence)
        respData.reason = data.reason;

    if (command == app_responseData)
        m_OTAService.sendOTAServiceData(OperationId::OTAAssignmentSync, OperationType::RESPONSE, 0, respData);
    else
        m_OTAService.sendOTAServiceData(OperationId::OTAAssignmentSync, OperationType::NOTIFICATION, 0, respData);

    return true;
}

bool CarDownloadAgentTransaction::SendAssignmentNotificationData(fsm::FotaAssignmentNotification_Data &msg)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);
    fsm::VdServiceTransactionId vdsTransId;
    void *vdmsg = nullptr;
    std::shared_ptr<GlyVdsDLAgentSignal> signal = std::dynamic_pointer_cast<GlyVdsDLAgentSignal> (GlyVdsDLAgentSignal::CreateGlyVdsDLAgentSignal(vdsTransId, vdmsg));

    msg.header.isInitiator = true;
    bool result = signal->SetAssignmentNotificationPayload(msg);
    if (!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Set assignment notification payload fail.");
        return result;
    }

    fsm::VocmoSignalSource vocmo_signal_source;
    result = vocmo_signal_source.GeelySendMessage(signal);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send assignment notification data success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send assignment notification data fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::SendAvailableAssignmentRequestData(fsm::AvailableAssignmentRequest_Data &msg)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: SendAvailableAssignmentRequestData begin.");

    fsm::VdServiceTransactionId vdsTransId;
    std::shared_ptr<GlyVdsDLAgentSignal> signal = std::make_shared<GlyVdsDLAgentSignal>(vdsTransId, nullptr);
    if (signal == nullptr){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: %s std::make_shared<GlyVdsDLAgentSignal> failed.", __FUNCTION__);
        return false;
    }
    signal->SetAvailableAssignmentRequestPayload(msg);
    fsm::VocmoSignalSource vocmo_signal_source;

    bool result = vocmo_signal_source.GeelySendMessage(signal);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send AvailableAssignmentRequest success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send AvailableAssignmentRequest fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::SendExceptionReportNotificationData(fsm::EcuExceptionReport_Data &msg)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: SendExceptionReportNotificationData begin.");

    fsm::VdServiceTransactionId vdsTransId;
    std::shared_ptr<GlyVdsDLAgentSignal> signal = std::make_shared<GlyVdsDLAgentSignal>(vdsTransId, nullptr);
    if (signal == nullptr){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "DLA: %s std::make_shared<GlyVdsDLAgentSignal> failed.", __FUNCTION__);
        return false;
    }
    signal->SetExceptionReportNotificationPayload(msg);
    fsm::VocmoSignalSource vocmo_signal_source;

    bool result = vocmo_signal_source.GeelySendMessage(signal);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: SendExceptionReportNotificationData success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: SendExceptionReportNotificationData fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::SendFileData(const HttpData &data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s begin.", __FUNCTION__);
    OpOTAWriteAssignmentData_Request_Data writeData;
    writeData.transferType = 0;
    writeData.uuid = "VIN1234567890VIN12345678901234567890";
    writeData.fileName = m_downloadStatus.fileName;
    writeData.fileSize = m_downloadStatus.fileSize;
    writeData.chunkSize = data.payload.size();
    writeData.chunkNumber = m_downloadStatus.chunkNumber;
    writeData.chunkPayload.clear();
    for (uint32_t i=0; i<data.payload.size(); i++)
    {
        writeData.chunkPayload.push_back(data.payload.at(i));
    }
    m_OTAService.sendOTAServiceData(OperationId::OTAWriteAssignmentData, OperationType::REQUEST, 0, writeData);
    return true;
}

bool CarDownloadAgentTransaction::ConfirmConfigurationStatus(std::string sConfigurationStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    // Send Notification to TSP
    fsm::FotaAssignmentNotification_Data assignmentNotification = {};
    assignmentNotification.installationOrderId_presence = true;
    assignmentNotification.installationOrderId = m_installationOrderId;
    assignmentNotification.timestamp = GetNowTimeStamp();

    if (sConfigurationStatus == "NOK")
    {
        // will be modify "_"
        assignmentNotification.newStatus = "DOWNLOAD-ABORTED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "CONFIGURATION-STATUS-NOK";
        SetState(kCanceled);
    }
    else if (sConfigurationStatus == "UNAVAILABLE")
    {
        // will be modify "_"
        assignmentNotification.newStatus = "DOWNLOAD-FAILED";
        assignmentNotification.reason_presence = true;
        assignmentNotification.reason = "CONFIGURATION-UNAVAILABLE";
        SetState(kFailed);
    }

    bool result = SendAssignmentNotificationData(assignmentNotification);

    // Send Notification to IHU
    OpOTATCAMAssignmentNotification_Notification_Data msg = {};
    msg.installationorder.uuid = m_installationOrderId;
    msg.timestamp.timestamp1 = assignmentNotification.timestamp;
    msg.newstatus = assignmentNotification.newStatus;
    msg.reason_presence = assignmentNotification.reason_presence;
    msg.reason = assignmentNotification.reason;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: uuid:%s", msg.installationorder.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: timestamp1:%s", msg.timestamp.timestamp1.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: newstatus:%s", msg.newstatus.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: reason_presence:%d", msg.reason_presence);

    result = m_OTAService.sendOTAServiceData(OperationId::OTATCAMAssignmentNotification,
                                             OperationType::NOTIFICATION,
                                             0,
                                             msg);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send TCAMAssignment notification data success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send TCAMAssignment notification data fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::SendOTAQueryAvailableStorage()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    OpGeneric_Request_Data msg;

    bool result = m_OTAService.sendOTAServiceData(OperationId::OTAQueryAvailableStorage,
                                                  OperationType::REQUEST,
                                                  0,
                                                  msg);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Send OTAQueryAvailableStorage request success.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Send OTAQueryAvailableStorage request fail.");
    }

    return result;
}

bool CarDownloadAgentTransaction::LoadPersistData()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s", __FUNCTION__);

    vocpersistdatamgr::VocPersistDataMgr& vocpersist = vocpersistdatamgr::VocPersistDataMgr::GetInstance();

    std::string configurationStatus;
    bool result = vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::DLA_CONFIGURATION_STATUS,
                                     configurationStatus);
    if (result)
    {
        m_configurationStatus = configurationStatus;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: Get configuration status:%s.", m_configurationStatus.c_str());
    }

    uint32_t downloadConsent;
    result = vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::DLA_DOWNLOAD_CONSENT, downloadConsent);
    if (result)
    {
        m_downloadConsent = (downloadConsent == 1) ? true : false;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "DLA: Get download consent:%d.", m_downloadConsent);
    }

    if (!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "DLA: Get local config error. Use default config.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "DLA: Get local config success.");
    }

    return result;
}

std::string CarDownloadAgentTransaction::GetNowTimeStamp()
{
    Timestamp::TimeVal utcTime = GetUtcTime();
    long long int lCurrUtcTime = utcTime / 1000;

    Timestamp ts(lCurrUtcTime * 1000);

    return DateTimeToString(ts);
}

std::string CarDownloadAgentTransaction::DateTimeToString(Timestamp ts)
{
    char buff[TIMESTAMP_MAX_LENGTH];
    std::string microsec;

    std::time_t t = ts.epochTime();
    std::tm *tm = std::localtime(&t);

    if(tm != 0)
    {
        std::strftime(buff, TIMESTAMP_MAX_LENGTH, "%Y-%m-%d %H:%M:%S", tm);

        microsec = std::to_string(ts.epochMicroseconds() % 1000000);

        if (microsec.length() < TIMESTAMP_MICROSECOND_LENGTH) {
            microsec = std::string(TIMESTAMP_MICROSECOND_LENGTH - microsec.length(), '0') + microsec;
        }
        else if (microsec.length() > TIMESTAMP_MICROSECOND_LENGTH){
            microsec = microsec.substr(0, TIMESTAMP_MICROSECOND_LENGTH);
        }
    }

    return std::string(buff) + "." + microsec;
}

void CarDownloadAgentTransaction::httpCallBack(ResponseCommunicatorDataHttpsGet &callback)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s response body size [%d]\n", __FUNCTION__,callback.payload().size());
    HttpData data = {};
    data.payload = callback.payload();

    for (int i = 0; i < callback.header_size(); i++)
    {
        if (callback.header().Get(i).find("Content-Range:") == std::string::npos)
        {
            continue;
        }
        // get file size(bytes)
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s str: %s", __FUNCTION__, callback.header().Get(i).c_str());
        std::string fileSizeStr = callback.header().Get(i).substr(callback.header().Get(i).find('/')+1);
        data.fileSize = std::stoull(fileSizeStr);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s file size %ulld", __FUNCTION__, data.fileSize);
        break;
    }

    fsm::InternalSignalTransactionId transactionId;
    std::shared_ptr<fsm::HttpSignal> signal = fsm::HttpSignal::CreateHttpSignal(data, transactionId);
    this->EnqueueSignal(signal);

    return;
}

void CarDownloadAgentTransaction::sendStartDownloadNotification(std::string &uuid, std::string &timeStamp)
{
    // send message to IHU
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s send download started notification to IHU", __FUNCTION__);
    OpOTATCAMAssignmentNotification_Notification_Data ipcbNotification;
    ipcbNotification.installationorder.uuid = uuid;
    ipcbNotification.timestamp.timestamp1 = timeStamp;
    ipcbNotification.newstatus = "DOWNLOAD-STARTED";
    ipcbNotification.reason_presence = true;
    ipcbNotification.reason = "SYSTEM";
    m_OTAService.sendOTAServiceData(OperationId::OTATCAMAssignmentNotification, OperationType::NOTIFICATION, 0, ipcbNotification);

    // send messag to TSP
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "DLA: %s send download started notification to TSP", __FUNCTION__);
    fsm::FotaAssignmentNotification_Data vdsNotification;
    vdsNotification.installationOrderId_presence = true;
    vdsNotification.installationOrderId = uuid;
    vdsNotification.timestamp = timeStamp;
    vdsNotification.newStatus = "DOWNLOAD-STARTED";
    vdsNotification.reason_presence = true;
    vdsNotification.reason = "SYSTEM";
    SendAssignmentNotificationData(vdsNotification);

    return;
}

void CarDownloadAgentTransaction::clearDownloadStatus()
{
    m_downloadStatus.chunkNumber = 0;
    m_downloadStatus.chunks = 0;
    m_downloadStatus.chunkSize = 2048;  // default 2048 bytes
    m_downloadStatus.fileName.clear();
    m_downloadStatus.fileSize = 0;
    m_downloadStatus.curentUrl = 0;
    m_downloadStatus.urls.clear();
    m_downloadStatus.pHttpGet = nullptr;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */

