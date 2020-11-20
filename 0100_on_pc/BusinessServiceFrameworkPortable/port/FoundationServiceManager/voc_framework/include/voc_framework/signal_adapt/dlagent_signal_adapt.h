///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  file dlagent_signal_adapt.h

// @project     GLY_TCAM
// @subsystem   FSM
// @author      Shen Jiaojiao
// @Init date   27-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef DLAGENT_SIGNAL_ADAPT_H
#define DLAGENT_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

class DLAgentSignalAdapt: public SignalAdaptBase
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack svt RequestBody.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;

public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return
    // @author     Shen Jiaojiao, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    DLAgentSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj) : SignalAdaptBase(vdsObj){};
    DLAgentSignalAdapt(void* vdsObj) : SignalAdaptBase(vdsObj) {};

    ////////////////////////////////////////////////////////////
    // @brief :pack DLAgentAssignmentNotification_t into ASN1C structure.
    // @param[in]  msg, DLAgentAssignmentNotification_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Shen Jiaojiao, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetAssignmentNotificationPayload(const fsm::FotaAssignmentNotification_Data &msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack AvailableAssignmentRequest_Data into ASN1C structure.
    // @param[in]  msg, AvailableAssignmentRequest_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     Tian Yong, 1-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetAvailableAssignmentRequestPayload(const fsm::AvailableAssignmentRequest_Data &msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack ExceptionReportRequest_Data into ASN1C structure.
    // @param[in]  msg, ExceptionReport_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     Tan Chang, 29-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetExceptionReportResultPayload(const fsm::EcuExceptionReport_Data& msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack Set InstallationSummary Payload into ASN1C structure.
    // @param[in]  msg, InstallationSummary_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     Li Jianhui, 19-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetInstallationSummaryPayload(const fsm::InstallationSummary_Data &msg);

private:
    ////////////////////////////////////////////////////////////
    // @brief :set value.
    // @param[in]  appData, Soruce data structure .
    // @param[in]  vdsData, The target data structure
    // @return     True if successfully set, false otherwise
    // @author     Tian Yong, 1-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetTimeStamp(const fsm::TimeStamp_Data &appData, TimeStamp_Model &vdsData);
    bool SetServiceParameters(const std::vector<fsm::ServiceParameter_Data> &appData, std::vector<ServiceParameter_Model> &vdsData);
    bool SetFotaAssignmentNotification(const fsm::FotaAssignmentNotification_Data &appData, FotaAssignmentNotification_Model &vdsData);
    bool SetLanguageSetting(const fsm::LanguageSetting_Data &appData, LanguageSetting_Model &vdsData);
    bool SetAvailableAssignmentRequest(const fsm::AvailableAssignmentRequest_Data &appData, AvailableAssignmentRequest_Model &vdsData);
    bool SetInstallationSummary(const fsm::InstallationSummary_Data& appData, InstallationSummary_Model &vdsData);

    bool GetFOTA(fsm::FOTA_Data &appData, const FOTA_Model &vdsData);
    bool GetEcuInventory(fsm::EcuInventory_Data &appData, const EcuInventory_Model &vdsData);
    bool GetClientConfSyncRequest(fsm::ClientConfSyncRequest_Data &appData, const ClientConfSyncRequest_Model &vdsData);
    bool GetInventoryInstruction(fsm::InventoryInstruction_Data &appData, const InventoryInstruction_Model &vdsData);
    bool GetAvailableAssignmentRequest(fsm::AvailableAssignmentRequest_Data &appData, const AvailableAssignmentRequest_Model &vdsData);
    bool GetAvailableAssignment(fsm::AvailableAssignment_Data &appData, const AvailableAssignment_Model &vdsData);
    bool GetInstruction(fsm::Instruction_Data &appData, const Instruction_Model &vdsData);
    bool GetInstallationInstruction(fsm::InstallationInstruction_Data &appData, const InstallationInstruction_Model &vdsData);
    bool GetInstallationSummary(fsm::InstallationSummary_Data &appData, const InstallationSummary_Model &vdsData);
    bool GetEcuExceptionReport(fsm::EcuExceptionReport_Data &appData, const EcuExceptionReport_Model &vdsData);
    bool GetClientHmiVehicleSettings(fsm::ClientHmiVehicleSettings_Data &appData, const ClientHmiVehicleSettings_Model &vdsData);
    bool GetAssignmentData(fsm::AssignmentData_Data &appData, const AssignmentData_Model &vdsData);
    bool GetFotaAssignmentNotification(fsm::FotaAssignmentNotification_Data &appData, const FotaAssignmentNotification_Model &vdsData);
    bool GetOtaAssignmentFileInfo(fsm::OtaAssignmentFileInfo_Data &appData, const OtaAssignmentFileInfo_Model &vdsData);
    bool GetExceptionMessage(fsm::ExceptionMessage_Data &appData, const ExceptionMessage_Model &vdsData);
    bool GetLanguageSetting(fsm::LanguageSetting_Data &appData, const LanguageSetting_Model &vdsData);
    bool GetTimeStamp(fsm::TimeStamp_Data &appData, const TimeStamp_Model &vdsData);
};

#endif

