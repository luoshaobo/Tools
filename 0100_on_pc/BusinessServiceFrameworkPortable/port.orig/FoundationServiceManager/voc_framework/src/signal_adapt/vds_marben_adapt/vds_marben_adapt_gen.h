///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vds_marben_adapt_gen.h
//     Generated from VDS 1.3.12 and MarbenCode

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VDS_MARBEN_ADAPT_GEN_H
#define VDS_MARBEN_ADAPT_GEN_H

#include <string>
#include <vector>
#include <memory>
#include "vds_data_gen.h"
#include "vds_marben_adapt_base.h"

class VdsMarbenAdaptGen: virtual protected VdsMarbenAdaptBase
{
public:
    VdsMarbenAdaptGen(){};
    
    VdsMarbenAdaptGen(asn_wrapper::VDServiceRequest* vdsObj) : VdsMarbenAdaptBase(vdsObj){};

private:
    asntype* getMarbenEncodeObj(const std::string &path);
    
    asntype* getMarbenDecodeObj(const std::string &path);

public:
    //GEN CODE////////////////////////////////////////////////////////////

    bool VDServiceRequest_Encode(const VDServiceRequest_Model &pData, asn_wrapper::VDServiceRequest* inputObj = nullptr);
    bool VDServiceRequest_Decode(const std::string &path, std::shared_ptr<VDServiceRequest_Model> &pData, asn_wrapper::VDServiceRequest* inputObj = nullptr);

    bool RequestHeader_Encode(const RequestHeader_Model &pData, asn_wrapper::RequestHeader* inputObj = nullptr);
    bool RequestHeader_Decode(const std::string &path, std::shared_ptr<RequestHeader_Model> &pData, asn_wrapper::RequestHeader* inputObj = nullptr);

    bool TimeStamp_Encode(const TimeStamp_Model &pData, asn_wrapper::TimeStamp* inputObj = nullptr);
    bool TimeStamp_Decode(const std::string &path, std::shared_ptr<TimeStamp_Model> &pData, asn_wrapper::TimeStamp* inputObj = nullptr);

    bool RequestBody_Encode(const RequestBody_Model &pData, asn_wrapper::RequestBody* inputObj = nullptr);
    bool RequestBody_Decode(const std::string &path, std::shared_ptr<RequestBody_Model> &pData, asn_wrapper::RequestBody* inputObj = nullptr);

    bool RVDC_Encode(const RVDC_Model &pData, asn_wrapper::RVDC* inputObj = nullptr);
    bool RVDC_Decode(const std::string &path, std::shared_ptr<RVDC_Model> &pData, asn_wrapper::RVDC* inputObj = nullptr);

    bool RvdcSetting_Encode(const RvdcSetting_Model &pData, asn_wrapper::RvdcSetting* inputObj = nullptr);
    bool RvdcSetting_Decode(const std::string &path, std::shared_ptr<RvdcSetting_Model> &pData, asn_wrapper::RvdcSetting* inputObj = nullptr);

    bool RvdcForcedSetting_Encode(const RvdcForcedSetting_Model &pData, asn_wrapper::RvdcForcedSetting* inputObj = nullptr);
    bool RvdcForcedSetting_Decode(const std::string &path, std::shared_ptr<RvdcForcedSetting_Model> &pData, asn_wrapper::RvdcForcedSetting* inputObj = nullptr);

    bool RvdcVehicleSetting_Encode(const RvdcVehicleSetting_Model &pData, asn_wrapper::RvdcVehicleSetting* inputObj = nullptr);
    bool RvdcVehicleSetting_Decode(const std::string &path, std::shared_ptr<RvdcVehicleSetting_Model> &pData, asn_wrapper::RvdcVehicleSetting* inputObj = nullptr);

    bool MaSetting_Encode(const MaSetting_Model &pData, asn_wrapper::MaSetting* inputObj = nullptr);
    bool MaSetting_Decode(const std::string &path, std::shared_ptr<MaSetting_Model> &pData, asn_wrapper::MaSetting* inputObj = nullptr);

    bool AuthorizationStatus_Encode(const AuthorizationStatus_Model &pData, asn_wrapper::AuthorizationStatus* inputObj = nullptr);
    bool AuthorizationStatus_Decode(const std::string &path, std::shared_ptr<AuthorizationStatus_Model> &pData, asn_wrapper::AuthorizationStatus* inputObj = nullptr);

    bool RvdcAssignmentNotification_Encode(const RvdcAssignmentNotification_Model &pData, asn_wrapper::RvdcAssignmentNotification* inputObj = nullptr);
    bool RvdcAssignmentNotification_Decode(const std::string &path, std::shared_ptr<RvdcAssignmentNotification_Model> &pData, asn_wrapper::RvdcAssignmentNotification* inputObj = nullptr);

    bool MaSynchronizationReq_Encode(const MaSynchronizationReq_Model &pData, asn_wrapper::MaSynchronizationReq* inputObj = nullptr);
    bool MaSynchronizationReq_Decode(const std::string &path, std::shared_ptr<MaSynchronizationReq_Model> &pData, asn_wrapper::MaSynchronizationReq* inputObj = nullptr);

    bool MaSynchronization_Encode(const MaSynchronization_Model &pData, asn_wrapper::MaSynchronization* inputObj = nullptr);
    bool MaSynchronization_Decode(const std::string &path, std::shared_ptr<MaSynchronization_Model> &pData, asn_wrapper::MaSynchronization* inputObj = nullptr);

    bool Ma_Encode(const Ma_Model &pData, asn_wrapper::Ma* inputObj = nullptr);
    bool Ma_Decode(const std::string &path, std::shared_ptr<Ma_Model> &pData, asn_wrapper::Ma* inputObj = nullptr);

    bool MeasurementAssignment_Encode(const MeasurementAssignment_Model &pData, asn_wrapper::MeasurementAssignment* inputObj = nullptr);
    bool MeasurementAssignment_Decode(const std::string &path, std::shared_ptr<MeasurementAssignment_Model> &pData, asn_wrapper::MeasurementAssignment* inputObj = nullptr);

    bool EcusAndPartNumber_Encode(const EcusAndPartNumber_Model &pData, asn_wrapper::EcusAndPartNumber* inputObj = nullptr);
    bool EcusAndPartNumber_Decode(const std::string &path, std::shared_ptr<EcusAndPartNumber_Model> &pData, asn_wrapper::EcusAndPartNumber* inputObj = nullptr);

    bool ExecutionTrigger_Encode(const ExecutionTrigger_Model &pData, asn_wrapper::ExecutionTrigger* inputObj = nullptr);
    bool ExecutionTrigger_Decode(const std::string &path, std::shared_ptr<ExecutionTrigger_Model> &pData, asn_wrapper::ExecutionTrigger* inputObj = nullptr);

    bool Trigger_Encode(const Trigger_Model &pData, asn_wrapper::Trigger* inputObj = nullptr);
    bool Trigger_Decode(const std::string &path, std::shared_ptr<Trigger_Model> &pData, asn_wrapper::Trigger* inputObj = nullptr);

    bool UDS_Encode(const UDS_Model &pData, asn_wrapper::UDS* inputObj = nullptr);
    bool UDS_Decode(const std::string &path, std::shared_ptr<UDS_Model> &pData, asn_wrapper::UDS* inputObj = nullptr);

    bool DiagnosticCommand_Encode(const DiagnosticCommand_Model &pData, asn_wrapper::DiagnosticCommand* inputObj = nullptr);
    bool DiagnosticCommand_Decode(const std::string &path, std::shared_ptr<DiagnosticCommand_Model> &pData, asn_wrapper::DiagnosticCommand* inputObj = nullptr);

    bool TelematicsParameters_Encode(const TelematicsParameters_Model &pData, asn_wrapper::TelematicsParameters* inputObj = nullptr);
    bool TelematicsParameters_Decode(const std::string &path, std::shared_ptr<TelematicsParameters_Model> &pData, asn_wrapper::TelematicsParameters* inputObj = nullptr);

    bool StopCondition_Encode(const StopCondition_Model &pData, asn_wrapper::StopCondition* inputObj = nullptr);
    bool StopCondition_Decode(const std::string &path, std::shared_ptr<StopCondition_Model> &pData, asn_wrapper::StopCondition* inputObj = nullptr);

    bool MeasurementDataPackage_Encode(const MeasurementDataPackage_Model &pData, asn_wrapper::MeasurementDataPackage* inputObj = nullptr);
    bool MeasurementDataPackage_Decode(const std::string &path, std::shared_ptr<MeasurementDataPackage_Model> &pData, asn_wrapper::MeasurementDataPackage* inputObj = nullptr);

    bool Mdp_Encode(const Mdp_Model &pData, asn_wrapper::Mdp* inputObj = nullptr);
    bool Mdp_Decode(const std::string &path, std::shared_ptr<Mdp_Model> &pData, asn_wrapper::Mdp* inputObj = nullptr);

    bool RvdcExceptionHandling_Encode(const RvdcExceptionHandling_Model &pData, asn_wrapper::RvdcExceptionHandling* inputObj = nullptr);
    bool RvdcExceptionHandling_Decode(const std::string &path, std::shared_ptr<RvdcExceptionHandling_Model> &pData, asn_wrapper::RvdcExceptionHandling* inputObj = nullptr);

    bool FOTA_Encode(const FOTA_Model &pData, asn_wrapper::FOTA* inputObj = nullptr);
    bool FOTA_Decode(const std::string &path, std::shared_ptr<FOTA_Model> &pData, asn_wrapper::FOTA* inputObj = nullptr);

    bool ClientConfSyncRequest_Encode(const ClientConfSyncRequest_Model &pData, asn_wrapper::ClientConfSyncRequest* inputObj = nullptr);
    bool ClientConfSyncRequest_Decode(const std::string &path, std::shared_ptr<ClientConfSyncRequest_Model> &pData, asn_wrapper::ClientConfSyncRequest* inputObj = nullptr);

    bool InventoryInstruction_Encode(const InventoryInstruction_Model &pData, asn_wrapper::InventoryInstruction* inputObj = nullptr);
    bool InventoryInstruction_Decode(const std::string &path, std::shared_ptr<InventoryInstruction_Model> &pData, asn_wrapper::InventoryInstruction* inputObj = nullptr);

    bool EcuInventory_Encode(const EcuInventory_Model &pData, asn_wrapper::EcuInventory* inputObj = nullptr);
    bool EcuInventory_Decode(const std::string &path, std::shared_ptr<EcuInventory_Model> &pData, asn_wrapper::EcuInventory* inputObj = nullptr);

    bool ValidationKey_Encode(const ValidationKey_Model &pData, asn_wrapper::ValidationKey* inputObj = nullptr);
    bool ValidationKey_Decode(const std::string &path, std::shared_ptr<ValidationKey_Model> &pData, asn_wrapper::ValidationKey* inputObj = nullptr);

    bool AvailableAssignmentRequest_Encode(const AvailableAssignmentRequest_Model &pData, asn_wrapper::AvailableAssignmentRequest* inputObj = nullptr);
    bool AvailableAssignmentRequest_Decode(const std::string &path, std::shared_ptr<AvailableAssignmentRequest_Model> &pData, asn_wrapper::AvailableAssignmentRequest* inputObj = nullptr);

    bool AvailableAssignment_Encode(const AvailableAssignment_Model &pData, asn_wrapper::AvailableAssignment* inputObj = nullptr);
    bool AvailableAssignment_Decode(const std::string &path, std::shared_ptr<AvailableAssignment_Model> &pData, asn_wrapper::AvailableAssignment* inputObj = nullptr);

    bool InstallationInstruction_Encode(const InstallationInstruction_Model &pData, asn_wrapper::InstallationInstruction* inputObj = nullptr);
    bool InstallationInstruction_Decode(const std::string &path, std::shared_ptr<InstallationInstruction_Model> &pData, asn_wrapper::InstallationInstruction* inputObj = nullptr);

    bool Instruction_Encode(const Instruction_Model &pData, asn_wrapper::Instruction* inputObj = nullptr);
    bool Instruction_Decode(const std::string &path, std::shared_ptr<Instruction_Model> &pData, asn_wrapper::Instruction* inputObj = nullptr);

    bool EcuInstruction_Encode(const EcuInstruction_Model &pData, asn_wrapper::EcuInstruction* inputObj = nullptr);
    bool EcuInstruction_Decode(const std::string &path, std::shared_ptr<EcuInstruction_Model> &pData, asn_wrapper::EcuInstruction* inputObj = nullptr);

    bool SoftwarePartInstallationInstruction_Encode(const SoftwarePartInstallationInstruction_Model &pData, asn_wrapper::SoftwarePartInstallationInstruction* inputObj = nullptr);
    bool SoftwarePartInstallationInstruction_Decode(const std::string &path, std::shared_ptr<SoftwarePartInstallationInstruction_Model> &pData, asn_wrapper::SoftwarePartInstallationInstruction* inputObj = nullptr);

    bool AssignmentValidation_Encode(const AssignmentValidation_Model &pData, asn_wrapper::AssignmentValidation* inputObj = nullptr);
    bool AssignmentValidation_Decode(const std::string &path, std::shared_ptr<AssignmentValidation_Model> &pData, asn_wrapper::AssignmentValidation* inputObj = nullptr);

    bool FotaAssignmentNotification_Encode(const FotaAssignmentNotification_Model &pData, asn_wrapper::FotaAssignmentNotification* inputObj = nullptr);
    bool FotaAssignmentNotification_Decode(const std::string &path, std::shared_ptr<FotaAssignmentNotification_Model> &pData, asn_wrapper::FotaAssignmentNotification* inputObj = nullptr);

    bool InstallationSummary_Encode(const InstallationSummary_Model &pData, asn_wrapper::InstallationSummary* inputObj = nullptr);
    bool InstallationSummary_Decode(const std::string &path, std::shared_ptr<InstallationSummary_Model> &pData, asn_wrapper::InstallationSummary* inputObj = nullptr);

    bool EcuSummary_Encode(const EcuSummary_Model &pData, asn_wrapper::EcuSummary* inputObj = nullptr);
    bool EcuSummary_Decode(const std::string &path, std::shared_ptr<EcuSummary_Model> &pData, asn_wrapper::EcuSummary* inputObj = nullptr);

    bool SoftwarePartSummary_Encode(const SoftwarePartSummary_Model &pData, asn_wrapper::SoftwarePartSummary* inputObj = nullptr);
    bool SoftwarePartSummary_Decode(const std::string &path, std::shared_ptr<SoftwarePartSummary_Model> &pData, asn_wrapper::SoftwarePartSummary* inputObj = nullptr);

    bool EcuExceptionReport_Encode(const EcuExceptionReport_Model &pData, asn_wrapper::EcuExceptionReport* inputObj = nullptr);
    bool EcuExceptionReport_Decode(const std::string &path, std::shared_ptr<EcuExceptionReport_Model> &pData, asn_wrapper::EcuExceptionReport* inputObj = nullptr);

    bool ExceptionReport_Encode(const ExceptionReport_Model &pData, asn_wrapper::ExceptionReport* inputObj = nullptr);
    bool ExceptionReport_Decode(const std::string &path, std::shared_ptr<ExceptionReport_Model> &pData, asn_wrapper::ExceptionReport* inputObj = nullptr);

    bool ExceptionMessage_Encode(const ExceptionMessage_Model &pData, asn_wrapper::ExceptionMessage* inputObj = nullptr);
    bool ExceptionMessage_Decode(const std::string &path, std::shared_ptr<ExceptionMessage_Model> &pData, asn_wrapper::ExceptionMessage* inputObj = nullptr);

    bool ClientHmiVehicleSettings_Encode(const ClientHmiVehicleSettings_Model &pData, asn_wrapper::ClientHmiVehicleSettings* inputObj = nullptr);
    bool ClientHmiVehicleSettings_Decode(const std::string &path, std::shared_ptr<ClientHmiVehicleSettings_Model> &pData, asn_wrapper::ClientHmiVehicleSettings* inputObj = nullptr);

    bool AssignmentData_Encode(const AssignmentData_Model &pData, asn_wrapper::AssignmentData* inputObj = nullptr);
    bool AssignmentData_Decode(const std::string &path, std::shared_ptr<AssignmentData_Model> &pData, asn_wrapper::AssignmentData* inputObj = nullptr);

    bool OtaAssignmentFileInfo_Encode(const OtaAssignmentFileInfo_Model &pData, asn_wrapper::OtaAssignmentFileInfo* inputObj = nullptr);
    bool OtaAssignmentFileInfo_Decode(const std::string &path, std::shared_ptr<OtaAssignmentFileInfo_Model> &pData, asn_wrapper::OtaAssignmentFileInfo* inputObj = nullptr);

    bool AssignmentFileInfo_Encode(const AssignmentFileInfo_Model &pData, asn_wrapper::AssignmentFileInfo* inputObj = nullptr);
    bool AssignmentFileInfo_Decode(const std::string &path, std::shared_ptr<AssignmentFileInfo_Model> &pData, asn_wrapper::AssignmentFileInfo* inputObj = nullptr);

    bool LanguageSetting_Encode(const LanguageSetting_Model &pData, asn_wrapper::LanguageSetting* inputObj = nullptr);
    bool LanguageSetting_Decode(const std::string &path, std::shared_ptr<LanguageSetting_Model> &pData, asn_wrapper::LanguageSetting* inputObj = nullptr);

    bool NEVServiceData_Encode(const NEVServiceData_Model &pData, asn_wrapper::NEVServiceData* inputObj = nullptr);
    bool NEVServiceData_Decode(const std::string &path, std::shared_ptr<NEVServiceData_Model> &pData, asn_wrapper::NEVServiceData* inputObj = nullptr);

    bool NEVDateTime_Encode(const NEVDateTime_Model &pData, asn_wrapper::NEVDateTime* inputObj = nullptr);
    bool NEVDateTime_Decode(const std::string &path, std::shared_ptr<NEVDateTime_Model> &pData, asn_wrapper::NEVDateTime* inputObj = nullptr);

    bool NEVDRPositionData_Encode(const NEVDRPositionData_Model &pData, asn_wrapper::NEVDRPositionData* inputObj = nullptr);
    bool NEVDRPositionData_Decode(const std::string &path, std::shared_ptr<NEVDRPositionData_Model> &pData, asn_wrapper::NEVDRPositionData* inputObj = nullptr);

    bool NEVCoordinatesLongLat_Encode(const NEVCoordinatesLongLat_Model &pData, asn_wrapper::NEVCoordinatesLongLat* inputObj = nullptr);
    bool NEVCoordinatesLongLat_Decode(const std::string &path, std::shared_ptr<NEVCoordinatesLongLat_Model> &pData, asn_wrapper::NEVCoordinatesLongLat* inputObj = nullptr);

    bool ECUData_Encode(const ECUData_Model &pData, asn_wrapper::ECUData* inputObj = nullptr);
    bool ECUData_Decode(const std::string &path, std::shared_ptr<ECUData_Model> &pData, asn_wrapper::ECUData* inputObj = nullptr);

    bool DIDData_Encode(const DIDData_Model &pData, asn_wrapper::DIDData* inputObj = nullptr);
    bool DIDData_Decode(const std::string &path, std::shared_ptr<DIDData_Model> &pData, asn_wrapper::DIDData* inputObj = nullptr);

    bool DTCData_Encode(const DTCData_Model &pData, asn_wrapper::DTCData* inputObj = nullptr);
    bool DTCData_Decode(const std::string &path, std::shared_ptr<DTCData_Model> &pData, asn_wrapper::DTCData* inputObj = nullptr);

    bool ServiceData_Encode(const ServiceData_Model &pData, asn_wrapper::ServiceData* inputObj = nullptr);
    bool ServiceData_Decode(const std::string &path, std::shared_ptr<ServiceData_Model> &pData, asn_wrapper::ServiceData* inputObj = nullptr);

    bool ServiceParameter_Encode(const ServiceParameter_Model &pData, asn_wrapper::ServiceParameter* inputObj = nullptr);
    bool ServiceParameter_Decode(const std::string &path, std::shared_ptr<ServiceParameter_Model> &pData, asn_wrapper::ServiceParameter* inputObj = nullptr);

    bool ServiceTrigger_Encode(const ServiceTrigger_Model &pData, asn_wrapper::ServiceTrigger* inputObj = nullptr);
    bool ServiceTrigger_Decode(const std::string &path, std::shared_ptr<ServiceTrigger_Model> &pData, asn_wrapper::ServiceTrigger* inputObj = nullptr);

    bool ServiceResult_Encode(const ServiceResult_Model &pData, asn_wrapper::ServiceResult* inputObj = nullptr);
    bool ServiceResult_Decode(const std::string &path, std::shared_ptr<ServiceResult_Model> &pData, asn_wrapper::ServiceResult* inputObj = nullptr);

    bool ServiceStatus_Encode(const ServiceStatus_Model &pData, asn_wrapper::ServiceStatus* inputObj = nullptr);
    bool ServiceStatus_Decode(const std::string &path, std::shared_ptr<ServiceStatus_Model> &pData, asn_wrapper::ServiceStatus* inputObj = nullptr);

    bool OTA_Encode(const OTA_Model &pData, asn_wrapper::OTA* inputObj = nullptr);
    bool OTA_Decode(const std::string &path, std::shared_ptr<OTA_Model> &pData, asn_wrapper::OTA* inputObj = nullptr);

    bool Error_Encode(const Error_Model &pData, asn_wrapper::Error* inputObj = nullptr);
    bool Error_Decode(const std::string &path, std::shared_ptr<Error_Model> &pData, asn_wrapper::Error* inputObj = nullptr);

    bool Position_Encode(const Position_Model &pData, asn_wrapper::Position* inputObj = nullptr);
    bool Position_Decode(const std::string &path, std::shared_ptr<Position_Model> &pData, asn_wrapper::Position* inputObj = nullptr);

    bool Configuration_Encode(const Configuration_Model &pData, asn_wrapper::Configuration* inputObj = nullptr);
    bool Configuration_Decode(const std::string &path, std::shared_ptr<Configuration_Model> &pData, asn_wrapper::Configuration* inputObj = nullptr);

    bool ConfigurationItem_Encode(const ConfigurationItem_Model &pData, asn_wrapper::ConfigurationItem* inputObj = nullptr);
    bool ConfigurationItem_Decode(const std::string &path, std::shared_ptr<ConfigurationItem_Model> &pData, asn_wrapper::ConfigurationItem* inputObj = nullptr);

    bool Trip_Encode(const Trip_Model &pData, asn_wrapper::Trip* inputObj = nullptr);
    bool Trip_Decode(const std::string &path, std::shared_ptr<Trip_Model> &pData, asn_wrapper::Trip* inputObj = nullptr);

    bool TripFragment_Encode(const TripFragment_Model &pData, asn_wrapper::TripFragment* inputObj = nullptr);
    bool TripFragment_Decode(const std::string &path, std::shared_ptr<TripFragment_Model> &pData, asn_wrapper::TripFragment* inputObj = nullptr);

    bool TrackPoint_Encode(const TrackPoint_Model &pData, asn_wrapper::TrackPoint* inputObj = nullptr);
    bool TrackPoint_Decode(const std::string &path, std::shared_ptr<TrackPoint_Model> &pData, asn_wrapper::TrackPoint* inputObj = nullptr);

    bool PointOfInterest_Encode(const PointOfInterest_Model &pData, asn_wrapper::PointOfInterest* inputObj = nullptr);
    bool PointOfInterest_Decode(const std::string &path, std::shared_ptr<PointOfInterest_Model> &pData, asn_wrapper::PointOfInterest* inputObj = nullptr);

    bool ContactInfo_Encode(const ContactInfo_Model &pData, asn_wrapper::ContactInfo* inputObj = nullptr);
    bool ContactInfo_Decode(const std::string &path, std::shared_ptr<ContactInfo_Model> &pData, asn_wrapper::ContactInfo* inputObj = nullptr);

    bool PostAddress_Encode(const PostAddress_Model &pData, asn_wrapper::PostAddress* inputObj = nullptr);
    bool PostAddress_Decode(const std::string &path, std::shared_ptr<PostAddress_Model> &pData, asn_wrapper::PostAddress* inputObj = nullptr);

    bool Notification_Encode(const Notification_Model &pData, asn_wrapper::Notification* inputObj = nullptr);
    bool Notification_Decode(const std::string &path, std::shared_ptr<Notification_Model> &pData, asn_wrapper::Notification* inputObj = nullptr);

    bool EventRule_Encode(const EventRule_Model &pData, asn_wrapper::EventRule* inputObj = nullptr);
    bool EventRule_Decode(const std::string &path, std::shared_ptr<EventRule_Model> &pData, asn_wrapper::EventRule* inputObj = nullptr);

    bool RuleCondition_Encode(const RuleCondition_Model &pData, asn_wrapper::RuleCondition* inputObj = nullptr);
    bool RuleCondition_Decode(const std::string &path, std::shared_ptr<RuleCondition_Model> &pData, asn_wrapper::RuleCondition* inputObj = nullptr);

    bool EventAction_Encode(const EventAction_Model &pData, asn_wrapper::EventAction* inputObj = nullptr);
    bool EventAction_Decode(const std::string &path, std::shared_ptr<EventAction_Model> &pData, asn_wrapper::EventAction* inputObj = nullptr);

    bool TimerData_Encode(const TimerData_Model &pData, asn_wrapper::TimerData* inputObj = nullptr);
    bool TimerData_Decode(const std::string &path, std::shared_ptr<TimerData_Model> &pData, asn_wrapper::TimerData* inputObj = nullptr);

    bool TimerInfo_Encode(const TimerInfo_Model &pData, asn_wrapper::TimerInfo* inputObj = nullptr);
    bool TimerInfo_Decode(const std::string &path, std::shared_ptr<TimerInfo_Model> &pData, asn_wrapper::TimerInfo* inputObj = nullptr);

    bool VehicleStatus_Encode(const VehicleStatus_Model &pData, asn_wrapper::VehicleStatus* inputObj = nullptr);
    bool VehicleStatus_Decode(const std::string &path, std::shared_ptr<VehicleStatus_Model> &pData, asn_wrapper::VehicleStatus* inputObj = nullptr);

    bool BasicVehicleStatus_Encode(const BasicVehicleStatus_Model &pData, asn_wrapper::BasicVehicleStatus* inputObj = nullptr);
    bool BasicVehicleStatus_Decode(const std::string &path, std::shared_ptr<BasicVehicleStatus_Model> &pData, asn_wrapper::BasicVehicleStatus* inputObj = nullptr);

    bool LockStatus_Encode(const LockStatus_Model &pData, asn_wrapper::LockStatus* inputObj = nullptr);
    bool LockStatus_Decode(const std::string &path, std::shared_ptr<LockStatus_Model> &pData, asn_wrapper::LockStatus* inputObj = nullptr);

    bool MaintenanceStatus_Encode(const MaintenanceStatus_Model &pData, asn_wrapper::MaintenanceStatus* inputObj = nullptr);
    bool MaintenanceStatus_Decode(const std::string &path, std::shared_ptr<MaintenanceStatus_Model> &pData, asn_wrapper::MaintenanceStatus* inputObj = nullptr);

    bool RunningStatus_Encode(const RunningStatus_Model &pData, asn_wrapper::RunningStatus* inputObj = nullptr);
    bool RunningStatus_Decode(const std::string &path, std::shared_ptr<RunningStatus_Model> &pData, asn_wrapper::RunningStatus* inputObj = nullptr);

    bool ClimateStatus_Encode(const ClimateStatus_Model &pData, asn_wrapper::ClimateStatus* inputObj = nullptr);
    bool ClimateStatus_Decode(const std::string &path, std::shared_ptr<ClimateStatus_Model> &pData, asn_wrapper::ClimateStatus* inputObj = nullptr);

    bool PollutionStatus_Encode(const PollutionStatus_Model &pData, asn_wrapper::PollutionStatus* inputObj = nullptr);
    bool PollutionStatus_Decode(const std::string &path, std::shared_ptr<PollutionStatus_Model> &pData, asn_wrapper::PollutionStatus* inputObj = nullptr);

    bool ElectricStatus_Encode(const ElectricStatus_Model &pData, asn_wrapper::ElectricStatus* inputObj = nullptr);
    bool ElectricStatus_Decode(const std::string &path, std::shared_ptr<ElectricStatus_Model> &pData, asn_wrapper::ElectricStatus* inputObj = nullptr);

    bool TEMStatus_Encode(const TEMStatus_Model &pData, asn_wrapper::TEMStatus* inputObj = nullptr);
    bool TEMStatus_Decode(const std::string &path, std::shared_ptr<TEMStatus_Model> &pData, asn_wrapper::TEMStatus* inputObj = nullptr);

    bool CarConfig_Encode(const CarConfig_Model &pData, asn_wrapper::CarConfig* inputObj = nullptr);
    bool CarConfig_Decode(const std::string &path, std::shared_ptr<CarConfig_Model> &pData, asn_wrapper::CarConfig* inputObj = nullptr);

    bool AdditionalStatus_Encode(const AdditionalStatus_Model &pData, asn_wrapper::AdditionalStatus* inputObj = nullptr);
    bool AdditionalStatus_Decode(const std::string &path, std::shared_ptr<AdditionalStatus_Model> &pData, asn_wrapper::AdditionalStatus* inputObj = nullptr);

    bool OEMMSD_Encode(const OEMMSD_Model &pData, asn_wrapper::OEMMSD* inputObj = nullptr);
    bool OEMMSD_Decode(const std::string &path, std::shared_ptr<OEMMSD_Model> &pData, asn_wrapper::OEMMSD* inputObj = nullptr);

    bool ConfigurationAndIdentity_Encode(const ConfigurationAndIdentity_Model &pData, asn_wrapper::ConfigurationAndIdentity* inputObj = nullptr);
    bool ConfigurationAndIdentity_Decode(const std::string &path, std::shared_ptr<ConfigurationAndIdentity_Model> &pData, asn_wrapper::ConfigurationAndIdentity* inputObj = nullptr);

    bool GenericVehicleSignal_Encode(const GenericVehicleSignal_Model &pData, asn_wrapper::GenericVehicleSignal* inputObj = nullptr);
    bool GenericVehicleSignal_Decode(const std::string &path, std::shared_ptr<GenericVehicleSignal_Model> &pData, asn_wrapper::GenericVehicleSignal* inputObj = nullptr);

    bool NetworkAccessStatus_Encode(const NetworkAccessStatus_Model &pData, asn_wrapper::NetworkAccessStatus* inputObj = nullptr);
    bool NetworkAccessStatus_Decode(const std::string &path, std::shared_ptr<NetworkAccessStatus_Model> &pData, asn_wrapper::NetworkAccessStatus* inputObj = nullptr);

    bool ConnectivityStatus_Encode(const ConnectivityStatus_Model &pData, asn_wrapper::ConnectivityStatus* inputObj = nullptr);
    bool ConnectivityStatus_Decode(const std::string &path, std::shared_ptr<ConnectivityStatus_Model> &pData, asn_wrapper::ConnectivityStatus* inputObj = nullptr);

    bool TimePosition_Encode(const TimePosition_Model &pData, asn_wrapper::TimePosition* inputObj = nullptr);
    bool TimePosition_Decode(const std::string &path, std::shared_ptr<TimePosition_Model> &pData, asn_wrapper::TimePosition* inputObj = nullptr);

    bool ConnectivityInfo_Encode(const ConnectivityInfo_Model &pData, asn_wrapper::ConnectivityInfo* inputObj = nullptr);
    bool ConnectivityInfo_Decode(const std::string &path, std::shared_ptr<ConnectivityInfo_Model> &pData, asn_wrapper::ConnectivityInfo* inputObj = nullptr);

    bool SIMInfo_Encode(const SIMInfo_Model &pData, asn_wrapper::SIMInfo* inputObj = nullptr);
    bool SIMInfo_Decode(const std::string &path, std::shared_ptr<SIMInfo_Model> &pData, asn_wrapper::SIMInfo* inputObj = nullptr);

    bool BatteryStatus_Encode(const BatteryStatus_Model &pData, asn_wrapper::BatteryStatus* inputObj = nullptr);
    bool BatteryStatus_Decode(const std::string &path, std::shared_ptr<BatteryStatus_Model> &pData, asn_wrapper::BatteryStatus* inputObj = nullptr);

    bool ECUDiagnostic_Encode(const ECUDiagnostic_Model &pData, asn_wrapper::ECUDiagnostic* inputObj = nullptr);
    bool ECUDiagnostic_Decode(const std::string &path, std::shared_ptr<ECUDiagnostic_Model> &pData, asn_wrapper::ECUDiagnostic* inputObj = nullptr);

    bool DiagnosticTroubleCode_Encode(const DiagnosticTroubleCode_Model &pData, asn_wrapper::DiagnosticTroubleCode* inputObj = nullptr);
    bool DiagnosticTroubleCode_Decode(const std::string &path, std::shared_ptr<DiagnosticTroubleCode_Model> &pData, asn_wrapper::DiagnosticTroubleCode* inputObj = nullptr);

    bool TheftNotification_Encode(const TheftNotification_Model &pData, asn_wrapper::TheftNotification* inputObj = nullptr);
    bool TheftNotification_Decode(const std::string &path, std::shared_ptr<TheftNotification_Model> &pData, asn_wrapper::TheftNotification* inputObj = nullptr);

    bool FragStrs_Encode(const FragStrs_Model &pData, asn_wrapper::FragStrs* inputObj = nullptr);
    bool FragStrs_Decode(const std::string &path, std::shared_ptr<FragStrs_Model> &pData, asn_wrapper::FragStrs* inputObj = nullptr);

    bool FragItem_Encode(const FragItem_Model &pData, asn_wrapper::FragItem* inputObj = nullptr);
    bool FragItem_Decode(const std::string &path, std::shared_ptr<FragItem_Model> &pData, asn_wrapper::FragItem* inputObj = nullptr);

    bool TowStatus_Encode(const TowStatus_Model &pData, asn_wrapper::TowStatus* inputObj = nullptr);
    bool TowStatus_Decode(const std::string &path, std::shared_ptr<TowStatus_Model> &pData, asn_wrapper::TowStatus* inputObj = nullptr);


    //GEN CODE////////////////////////////////////////////////////////////
};

#endif

