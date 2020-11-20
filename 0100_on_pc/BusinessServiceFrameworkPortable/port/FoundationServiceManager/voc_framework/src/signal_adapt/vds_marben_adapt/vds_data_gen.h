///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file vds_data_gen.h
//     all asn enum and data struct define.
//     Generated from VDS 1.3.12

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VDS_DATA_GEN_H
#define VDS_DATA_GEN_H

#include <string>
#include <vector>
#include "vds_data_base.h"


typedef enum {
	Vds_ecu = 0,
	Vds_cvc = 1
}Vds_CreatorId;

typedef enum {
	Vds_serviceRequest = 0,
	Vds_ack = 1
}Vds_RequestType;

typedef enum {
	Vds_MaBusinessType_rvdcSetting = 0, //--  set RVDC Authorization Status
	Vds_MaBusinessType_assignmentNotification = 1, //--   Synchronization status between Server and end. 
	Vds_MaBusinessType_maSynchronizationReq = 2, //--  end to request MAs from RVDC server
	Vds_MaBusinessType_maSynchronization = 3, //-- Support vehicle request to cloud for response and cloud to vehicle for push
	Vds_MaBusinessType_exceptionHandling = 4,
	Vds_MaBusinessType_measurementDataPackage = 5
}Vds_MaBusinessType;

typedef enum {
	Vds_startofdcy = 0,
	Vds_endofdcy = 1
}Vds_TriggerType;

typedef enum {
	Vds_or = 0,
	Vds_and = 1
}Vds_Operator;

typedef enum {
	Vds_equalto = 1,
	Vds_greaterthan = 2,
	Vds_greaterthanorequal = 3,
	Vds_lessthan = 4,
	Vds_lessthanorequal = 5
}Vds_CompareType;

typedef enum {
	Vds_DataType_clientConfSyncRequest = 0,
	Vds_DataType_ecuExceptionReport = 1,
	Vds_DataType_inventoryInstruction = 2,
	Vds_DataType_availableAssignmentRequest = 3,
	Vds_DataType_availableAssignment = 4,
	Vds_DataType_installationInstruction = 5,
	Vds_DataType_assignmentNotification = 6,
	Vds_DataType_clientHmiVehicleSettings = 7,
	Vds_DataType_assignmentData = 8,
	Vds_DataType_otaAssignmentFileInfo = 9,
	Vds_DataType_installationSummary = 10
}Vds_DataType;

typedef enum {
	Vds_ecarx = 0, //-- using ecarx OTA Server
	Vds_geely = 1, //-- using geely It  OTA Server
	Vds_lynkCoEn = 2, //-- lynk&CO  international  OTA Server
	Vds_lynkCoCh = 3, //-- lynk&CO  chinese  OTA Server
	Vds_proton = 4, //-- proton  OTA Server
	Vds_lotus = 5 //-- lotus  OTA Server
}Vds_AppId;

typedef enum {
	Vds_arabic = 1,
	Vds_bulgarian = 2,
	Vds_chineseTradCan = 3,
	Vds_chineseSimpMan = 4,
	Vds_chineseTradMan = 5,
	Vds_czech = 6,
	Vds_danish = 7,
	Vds_dutch = 8,
	Vds_australianEnglish = 9,
	Vds_ukEnglish = 10,
	Vds_americanEnglish = 11,
	Vds_estonian = 12,
	Vds_finnish = 13,
	Vds_flemish = 14,
	Vds_canadianFrench = 15,
	Vds_french = 16,
	Vds_german = 17,
	Vds_greek = 18,
	Vds_hungarian = 19,
	Vds_italian = 20,
	Vds_japanese = 21,
	Vds_korean = 22, //-- (Han'Gul)
	Vds_latvian = 23,
	Vds_lithuanian = 24,
	Vds_norwegian = 25,
	Vds_polish = 26,
	Vds_brazilianPortuguese = 27,
	Vds_portuguese = 28,
	Vds_romanian = 29,
	Vds_russian = 30,
	Vds_slovak = 31,
	Vds_slovene = 32,
	Vds_spanish = 33,
	Vds_americanSpanish = 34,
	Vds_swedish = 35,
	Vds_thai = 36,
	Vds_turkish = 37
}Vds_Language;

typedef enum {
	Vds_periodicTrg = 1, //-- UM Driving (periodic MDP)
	Vds_umChangeTrg = 2, //-- UM Driving -> UM != Driving (1 MDP)
	Vds_chargeStartTrg = 3, //-- Charging start (1 MDP)
	Vds_chargeStopTrg = 4, //-- Charging Stop (1 MDP)
	Vds_disableRmsTrg = 5 //-- CC#14=Active -> cc#14=Inactive (1 MDP)
}Vds_NEVTriggerCondition;

typedef enum {
	Vds_notAvailable = 0, //--not available : no GNSS information available (e.g no GNSS antenna connection, GNSS receiver failure) 
	Vds_noFix = 1, //--No Fix: satellites received - no position available 
	Vds_fix2D = 2, //--2D fix : satellites received : 2 dimensional position fix 
	Vds_fix3D = 3, //--3D fix : satellites received : 3 dimensional position fix 
	Vds_startupMode = 4 //--Based on a position saved after last driving cycle (Cold Start Fix) 
}Vds_NEVGnssFixType;

typedef enum {
	Vds_noDr = 0, //-- No dead reckoning used 
	Vds_drNoMapMatch = 1, //-- Dead reckoning used but not matched to any map 
	Vds_drMapMatched = 2 //-- Position is matched to map
}Vds_NEVDeadReckoningType;

typedef enum {
	Vds_mtp = 1, //-- TEM Provision
	Vds_mtu = 2, //-- TEM Unprovision
	Vds_mcu = 4, //-- Configuration update
	Vds_mpm = 5, //-- TEM power mode mgmt.
	Vds_mta = 6, //-- TEM activation
	Vds_mul = 7, //-- Upload TEM log
	Vds_mrs = 8, //-- TEM factory reset
	Vds_rin = 11, //-- Remote  control inhibition
	Vds_rdl = 13, //-- Remote  door lock
	Vds_rdu = 14, //-- Remote  door unlock
	Vds_rtl = 15, //-- Remote  trunk lock
	Vds_rtu = 16, //-- Remote  trunk unlock
	Vds_res = 17, //-- Remote  engine start
	Vds_rvi = 18, //-- Remote  vehicle immobilization
	Vds_rcc = 19, //-- Remote  climate control (non-engine)
	Vds_rce = 20, //-- Remote  climate control
	Vds_rhl = 21, //-- Remote  honk & light
	Vds_rsh = 22, //-- Remote  seat heat
	Vds_rws = 23, //-- Remote  close/open window/sunroof
	Vds_rpc = 24, //-- Remote  PM 2.5 clean
	Vds_rpp = 25, //-- Remote  Pull PM 2.5
	Vds_ren = 26, //-- Remote  event notification
	Vds_rec = 27, //-- Remote EV charging, schedule
	Vds_cec = 31, //-- E Call
	Vds_cep = 32, //-- PSAP E Call
	Vds_cbc = 33, //-- B Call
	Vds_cic = 34, //-- I Call
	Vds_svt = 41, //-- SVT
	Vds_stn = 44, //-- Theft Notification
	Vds_rvs = 51, //-- Remote status & dashboard
	Vds_trs = 52, //-- Triggered Remote status
	Vds_jou = 61, //-- Journey log
	Vds_hec = 62, //-- Health check
	Vds_dia = 63, //-- Remote diagnostics
	Vds_ota = 67, //-- OTA upgrade
	Vds_osc = 68, //-- Send to car
	Vds_vun = 69, //-- Vehicle unlock notification
	Vds_dkb = 80, //-- Digital key booking
	Vds_bks = 89, //-- Blue tooth key sharing
	Vds_bdc = 90, //-- Big Data collect
	Vds_wcr = 71, //-- windows close reminder
	Vds_rsv = 72, //-- Remote seat ventilate
	Vds_mcp = 101, //-- Certificate provision
	Vds_msr = 102, //-- message sequence reset
	Vds_fota = 200, //-- FOTA upgrade
	Vds_rct = 201, //-- Remote Charge location and timer
	Vds_vdr = 202, //-- Vin Discrepancy report
	Vds_rdo = 203, //-- Remote open trunk/hood/Tank Flag/Charge Lid
	Vds_rdc = 204, //-- Remote close trunk/hood/Tank Flag/Charge Lid
	Vds_sra = 205, //-- Remote Service Reminder
	Vds_rcr = 206, //-- Remote Connectivity Report
	Vds_rfd = 207, //-- Remote fragrance dispensing
	Vds_rqt = 208, //-- Remote Query towing status
	Vds_rcs = 209, //-- Remote charge settings
	Vds_uvs = 210, //-- Remote update vehicle settings
	Vds_rvc = 250, //-- RVDC
	Vds_pvs = 300, //-- Part of vehicle status
	Vds_rms = 500 //-- Remote monitor service
}Vds_ServiceId;

typedef enum {
	Vds_startService = 0,
	Vds_requestData = 1,
	Vds_responseData = 2,
	Vds_executionAction = 4,
	Vds_terminateService = 20
}Vds_ServiceCommand;

typedef enum {
	Vds_incorrectState = 1,
	Vds_invalidData = 2,
	Vds_invalidFormat = 3,
	Vds_powderModeNotCorrect = 4,
	Vds_conditionsNotCorrect = 5,
	Vds_executionFailure = 6,
	Vds_permissionDenied = 7,
	Vds_timeout = 8,
	Vds_backupBatteryUsage = 9
}Vds_ErrorCode;

typedef enum {
	Vds_door_unlock = 0,
	Vds_event_trigger = 1,
	Vds_status_report = 2
}Vds_NotificationReason;

typedef enum {
	Vds_EngineStatus_engine_running = 0,
	Vds_EngineStatus_engine_off = 1,
	Vds_EngineStatus_unknown = 2
}Vds_EngineStatus;

typedef enum {
	Vds_KeyStatus_no_key_inserted = 0,
	Vds_KeyStatus_key_inserted = 1,
	Vds_KeyStatus_unknown = 2
}Vds_KeyStatus;

typedef enum {
	Vds_passengerVehicle = 0,
	Vds_bus = 1,
	Vds_cargo = 2,
	Vds_motorCycle = 3
}Vds_VehicleType;

typedef enum {
	Vds_unblocked = 0,
	Vds_blocked = 1
}Vds_EngineBlockedStatus;

typedef enum {
	Vds_no_srs_info = 0,
	Vds_srs_not_deployed = 1,
	Vds_srs_deployed = 2
}Vds_SRSStatus;

typedef enum {
	Vds_mainBattery = 0,
	Vds_backupBattery = 1
}Vds_PowerSource;

typedef enum {
	Vds_normal = 0,
	Vds_standby = 1,
	Vds_sleep_poll = 2,
	Vds_off = 3
}Vds_TEMPowerMode;

typedef enum {
	Vds_deactivated = 0,
	Vds_activatedAndUnprovioned = 1,
	Vds_activatedAndProvisioned = 2
}Vds_TEMState;

typedef enum {
	Vds_HealthStatus_ok = 0,
	Vds_HealthStatus_faulty = 1,
	Vds_HealthStatus_unknown = 2,
	Vds_HealthStatus_invalid = 3
}Vds_HealthStatus;

typedef enum {
	Vds_FuelType_petrol = 0,
	Vds_FuelType_diesel = 1,
	Vds_FuelType_petrolHybrid = 2,
	Vds_FuelType_dieselHybrid = 3,
	Vds_FuelType_ev = 4,
	Vds_FuelType_hydrogenFuelcellEV = 5,
	Vds_FuelType_compressedGas = 6,
	Vds_FuelType_liquefiedPropaneGas = 7,
	Vds_FuelType_invalid = 8
}Vds_FuelType;

typedef enum {
	Vds_testFailed = 0,
	Vds_testFailedThisOperationCycle = 1,
	Vds_pendingDTC = 2,
	Vds_confirmedDTC = 3,
	Vds_testNotCompletedSinceLastClear = 4,
	Vds_testFailedSinceLastClear = 5,
	Vds_testNotCompletedThisOperationCycle = 6,
	Vds_warningIndicatorRequested = 7
}Vds_DtcStatusBitsIso14229;

typedef enum {
	Vds_ServiceWarningTriggerReason_calendar = 0,
	Vds_ServiceWarningTriggerReason_distance = 1,
	Vds_ServiceWarningTriggerReason_engineHours = 2,
	Vds_ServiceWarningTriggerReason_engineRequest = 3,
	Vds_ServiceWarningTriggerReason_unknown = 7
}Vds_ServiceWarningTriggerReason;


class TimeStamp_Model: public VdsDataBase
{
public:
    TimeStamp_Model():
    seconds(),
    milliseconds_presence(),
    milliseconds()
    {
    }
    //-- UTC Timestamp Scope:INTEGER 0..2147483647;
    uint32_t seconds;
    //-- Millisecond within the second Scope:INTEGER 0..999;
    bool  milliseconds_presence;
    uint16_t milliseconds;

};

class Position_Model: public VdsDataBase
{
public:
    Position_Model():
    latitude_presence(),
    latitude(),
    longitude_presence(),
    longitude(),
    altitude_presence(),
    altitude(),
    posCanBeTrusted_presence(),
    posCanBeTrusted(),
    carLocatorStatUploadEn_presence(),
    carLocatorStatUploadEn(),
    marsCoordinates_presence(),
    marsCoordinates()
    {
    }
    //-- 32 bits Scope:INTEGER -2147483648..2147483647;
    bool  latitude_presence;
    uint64_t latitude;
    //-- 32 bits Scope:INTEGER -2147483648..2147483647;
    bool  longitude_presence;
    uint64_t longitude;
    //-- the altitude in meter Scope:INTEGER -100..8091;
    bool  altitude_presence;
    uint64_t altitude;
    //-- false means TEM use last position instead of GPS, default is true;
    bool  posCanBeTrusted_presence;
    bool posCanBeTrusted;
    //-- can TEM upload position, user can enable/disable through IHU, default is true;
    bool  carLocatorStatUploadEn_presence;
    bool carLocatorStatUploadEn;
    //-- true means GCJ-02 coordinaes, false means WGS-84, defult is false;
    bool  marsCoordinates_presence;
    bool marsCoordinates;

};

class ServiceParameter_Model: public VdsDataBase
{
public:
    ServiceParameter_Model():
    key(),
    intVal_presence(),
    intVal(),
    stringVal_presence(),
    stringVal(),
    boolVal_presence(),
    boolVal(),
    timestampVal_presence(),
    timestampVal(),
    dataVal_presence(),
    dataVal()
    {
    }
    //--Scope:PrintableString SIZE 0..250;
    std::string key;
    bool  intVal_presence;
    int32_t intVal;
    //--Scope:UTF8String SIZE 0..65535;
    bool  stringVal_presence;
    std::string stringVal;
    bool  boolVal_presence;
    bool boolVal;
    bool  timestampVal_presence;
    TimeStamp_Model timestampVal;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  dataVal_presence;
    std::string dataVal;

};

class DiagnosticCommand_Model: public VdsDataBase
{
public:
    DiagnosticCommand_Model():
    ecuAddress(),
    serviceId(),
    input(),
    compareType_presence(),
    compareType(),
    compareValue_presence(),
    compareValue(),
    delayUntilNextTriggerCommand()
    {
    }
    //-- This is the ECU target address for the request.  Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //-- This is the UDS service to be used Scope:PrintableString SIZE 1..64;
    std::string serviceId;
    //-- This is the input to be added after the UDS service, this can contain sub service ID,Identifiers and/or other input data to the ECU. Scope:PrintableString SIZE 1..65535;
    std::string input;
    bool  compareType_presence;
    Vds_CompareType compareType;
    //-- This is the value to compare the ECU response to. Scope:PrintableString SIZE 1..65535;
    bool  compareValue_presence;
    std::string compareValue;
    //-- This is the time to wait between two diagnostic trigger commands, this is specified din seconds. Scope:INTEGER 1..512;
    uint16_t delayUntilNextTriggerCommand;

};

class Trigger_Model: public VdsDataBase
{
public:
    Trigger_Model():
    trigger(),
    interval()
    {
    }
    //-- This contains if the collection of data shall be carried out on start or end of a DCY. startofdcy (0),endofdcy (1);
    Vds_TriggerType trigger;
    //--Scope:INTEGER 1..4294967295;
    uint32_t interval;

};

class AuthorizationStatus_Model: public VdsDataBase
{
public:
    AuthorizationStatus_Model():
    dataCollection_presence(),
    dataCollection(),
    remoteDiagnostics_presence(),
    remoteDiagnostics()
    {
    }
    //-- True if data collection is enabled;
    bool  dataCollection_presence;
    bool dataCollection;
    //--  True if full remote diagnostics is enabled for the vehicle;
    bool  remoteDiagnostics_presence;
    bool remoteDiagnostics;

};

class Ma_Model: public VdsDataBase
{
public:
    Ma_Model():
    maId(),
    maVersion()
    {
    }
    //--Scope:INTEGER 1..4294967295;
    uint32_t maId;
    //--Scope:INTEGER 1..4294967295;
    uint32_t maVersion;

};

class UDS_Model: public VdsDataBase
{
public:
    UDS_Model():
    Vds_operator(),
    delayForNextTriggerCheck(),
    delayAfterCollection(),
    diagnosticCommands()
    {
    }
    //-- The operator between the diagnostic trigger commands. 0 = Or 1 = And;
    Vds_Operator Vds_operator;
    //-- This contains the time between two trigger checks, it is specified in seconds. Scope:INTEGER 1..4294967295;
    uint32_t delayForNextTriggerCheck;
    //--Scope:INTEGER 1..4294967295;
    uint32_t delayAfterCollection;
    //-- This contains the actual trigger commands;
    std::vector<DiagnosticCommand_Model> diagnosticCommands;

};

class TelematicsParameters_Model: public VdsDataBase
{
public:
    TelematicsParameters_Model():
    timeToLive(),
    uploadPriority()
    {
    }
    //-- This ELEMENT describes how many seconds the maximum transmission time is before the MDP is discarded. Scope:INTEGER 1..4294967295;
    uint32_t timeToLive;
    //-- This contains the type of upload channels the telematics unit can use. Scope:INTEGER 1..255;
    uint8_t uploadPriority;

};

class ValidationKey_Model: public VdsDataBase
{
public:
    ValidationKey_Model():
    validationKey(),
    partIdentifiers_presence(),
    partIdentifiers()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string validationKey;
    bool  partIdentifiers_presence;
    std::vector<std::string> partIdentifiers;

};

class SoftwarePartInstallationInstruction_Model: public VdsDataBase
{
public:
    SoftwarePartInstallationInstruction_Model():
    partidentifier(),
    estimatedInstallationtime()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string partidentifier;
    //--Scope:INTEGER 1..4294967295;
    uint32_t estimatedInstallationtime;

};

class GenericVehicleSignal_Model: public VdsDataBase
{
public:
    GenericVehicleSignal_Model():
    key(),
    collectTime_presence(),
    collectTime(),
    value_presence(),
    value(),
    stringVal_presence(),
    stringVal(),
    boolVal_presence(),
    boolVal(),
    timestampVal_presence(),
    timestampVal(),
    dataVal_presence(),
    dataVal()
    {
    }
    //--Scope:PrintableString SIZE 1..100;
    std::string key;
    bool  collectTime_presence;
    TimeStamp_Model collectTime;
    bool  value_presence;
    int32_t value;
    //--Scope:UTF8String SIZE 0..65535;
    bool  stringVal_presence;
    std::string stringVal;
    bool  boolVal_presence;
    bool boolVal;
    bool  timestampVal_presence;
    TimeStamp_Model timestampVal;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  dataVal_presence;
    std::string dataVal;

};

class TimePosition_Model: public VdsDataBase
{
public:
    TimePosition_Model():
    time_presence(),
    time(),
    position_presence(),
    position()
    {
    }
    bool  time_presence;
    TimeStamp_Model time;
    bool  position_presence;
    Position_Model position;

};

class ConnectivityInfo_Model: public VdsDataBase
{
public:
    ConnectivityInfo_Model():
    connectTime_presence(),
    connectTime(),
    connectPosition_presence(),
    connectPosition(),
    connectType_presence(),
    connectType(),
    numRetriesConnect_presence(),
    numRetriesConnect(),
    retryReason_presence(),
    retryReason()
    {
    }
    bool  connectTime_presence;
    TimeStamp_Model connectTime;
    bool  connectPosition_presence;
    Position_Model connectPosition;
    //--Scope:PrintableString SIZE 0..50;
    bool  connectType_presence;
    std::string connectType;
    bool  numRetriesConnect_presence;
    int32_t numRetriesConnect;
    //--Scope:PrintableString SIZE 0..100;
    bool  retryReason_presence;
    std::string retryReason;

};

class DiagnosticTroubleCode_Model: public VdsDataBase
{
public:
    DiagnosticTroubleCode_Model():
    dtc(),
    status(),
    failureCount_presence(),
    failureCount(),
    lastFail_presence(),
    lastFail(),
    firtFail_presence(),
    firtFail(),
    firstFailureOdometer_presence(),
    firstFailureOdometer(),
    firstFailureBatteryVoltage_presence(),
    firstFailureBatteryVoltage(),
    firstFailureVehicleSpeed_presence(),
    firstFailureVehicleSpeed()
    {
    }
    //--ID of the trouble code. OEM specific, Scope:PrintableString SIZE 1..100;
    std::string dtc;
    //--status of the DTC. Follows ISO 14229;
    Vds_DtcStatusBitsIso14229 status;
    //--Failure counter. Scope:INTEGER 0..255;
    bool  failureCount_presence;
    uint8_t failureCount;
    //--Time of last failure;
    bool  lastFail_presence;
    TimeStamp_Model lastFail;
    //--Time of first failure;
    bool  firtFail_presence;
    TimeStamp_Model firtFail;
    //--the odometer at the first failure;
    bool  firstFailureOdometer_presence;
    int32_t firstFailureOdometer;
    //--the battery voltage at the first failure;
    bool  firstFailureBatteryVoltage_presence;
    int32_t firstFailureBatteryVoltage;
    //--the vehicle speed at the first failure;
    bool  firstFailureVehicleSpeed_presence;
    int32_t firstFailureVehicleSpeed;

};

class EcusAndPartNumber_Model: public VdsDataBase
{
public:
    EcusAndPartNumber_Model():
    ecuAddress(),
    diagnosticPartNumber()
    {
    }
    //-- This shall contain the ECU ID for the first ECU Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //-- This contains the diagnostic part number Scope:PrintableString SIZE 1..64;
    std::string diagnosticPartNumber;

};

class ExecutionTrigger_Model: public VdsDataBase
{
public:
    ExecutionTrigger_Model():
    push(),
    uds_presence(),
    uds(),
    time_presence(),
    time(),
    drivingCycle_presence(),
    drivingCycle()
    {
    }
    bool push;
    //-- Trigger is based on collection of UDS data and comparison, see requirement;
    bool  uds_presence;
    UDS_Model uds;
    //-- Trigger is based on time interval;
    bool  time_presence;
    Trigger_Model time;
    //--Trigger is based on driving cycle interval;
    bool  drivingCycle_presence;
    Trigger_Model drivingCycle;

};

class StopCondition_Model: public VdsDataBase
{
public:
    StopCondition_Model():
    period_presence(),
    period(),
    amount_presence(),
    amount(),
    time_presence(),
    time()
    {
    }
    //-- The amount of days the MA shall be performed after the first successfully collected MDP. Scope:INTEGER 1..65535;
    bool  period_presence;
    uint16_t period;
    //-- The amount of successful collections to be made. Scope:INTEGER 1..65535;
    bool  amount_presence;
    uint16_t amount;
    //-- The stop time for the MA. The timestamp is defined in the requirement Scope:PrintableString SIZE 1..32;
    bool  time_presence;
    std::string time;

};

class EcuInventory_Model: public VdsDataBase
{
public:
    EcuInventory_Model():
    ecuAddress(),
    validationKeys(),
    exceptionMessage_presence(),
    exceptionMessage()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    std::vector<ValidationKey_Model> validationKeys;
    //--Scope:UTF8String SIZE 1..65535;
    bool  exceptionMessage_presence;
    std::string exceptionMessage;

};

class EcuInstruction_Model: public VdsDataBase
{
public:
    EcuInstruction_Model():
    ecuAddress(),
    queuedRequest(),
    securityKey(),
    softwarePartInstallationInstructions()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //--Scope:INTEGER 1..65535;
    uint16_t queuedRequest;
    //--Scope:PrintableString SIZE 1..256;
    std::string securityKey;
    std::vector<SoftwarePartInstallationInstruction_Model> softwarePartInstallationInstructions;

};

class AssignmentValidation_Model: public VdsDataBase
{
public:
    AssignmentValidation_Model():
    validationType(),
    ecuAddress(),
    validationkey(),
    partidentifiers()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string validationType;
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //--Scope:PrintableString SIZE 1..256;
    std::string validationkey;
    std::vector<std::string> partidentifiers;

};

class SoftwarePartSummary_Model: public VdsDataBase
{
public:
    SoftwarePartSummary_Model():
    partIdentifier(),
    partRetries(),
    measuredInstallationTime(),
    softwarePartStatus()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string partIdentifier;
    //--Scope:INTEGER 1..65535;
    uint16_t partRetries;
    //--Scope:INTEGER 1..36000000;
    uint32_t measuredInstallationTime;
    //--Scope:PrintableString SIZE 1..256;
    std::string softwarePartStatus;

};

class ExceptionMessage_Model: public VdsDataBase
{
public:
    ExceptionMessage_Model():
    activity(),
    action(),
    exception()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string activity;
    //--Scope:PrintableString SIZE 1..256;
    std::string action;
    //--Scope:PrintableString SIZE 1..1024;
    std::string exception;

};

class LanguageSetting_Model: public VdsDataBase
{
public:
    LanguageSetting_Model():
    language()
    {
    }
    Vds_Language language;

};

class NEVDateTime_Model: public VdsDataBase
{
public:
    NEVDateTime_Model():
    year(),
    month(),
    day(),
    hour(),
    minute(),
    second()
    {
    }
    //--Scope:INTEGER 2000..2127;
    uint16_t year;
    //--Scope:INTEGER 1..12;
    uint8_t month;
    //--Scope:INTEGER 1..31;
    uint8_t day;
    //--Scope:INTEGER 0..23;
    uint8_t hour;
    //--Scope:INTEGER 0..59;
    uint8_t minute;
    //--Scope:INTEGER 0..59;
    uint8_t second;

};

class NEVCoordinatesLongLat_Model: public VdsDataBase
{
public:
    NEVCoordinatesLongLat_Model():
    longitude(),
    latitude()
    {
    }
    //--Scope:INTEGER -2147483648..2147483647;
    uint64_t longitude;
    //--Scope:INTEGER -2147483648..2147483647;
    uint64_t latitude;

};

class DIDData_Model: public VdsDataBase
{
public:
    DIDData_Model():
    id(),
    data()
    {
    }
    //-- ID for the specific DID Scope:INTEGER 0..65535;
    uint16_t id;
    //-- DID READOUT Scope:OCTET STRING SIZE 0..100;
    std::string data;

};

class DTCData_Model: public VdsDataBase
{
public:
    DTCData_Model():
    id(),
    data()
    {
    }
    //-- ID for the specific DTC Scope:INTEGER 0..16777215;
    uint32_t id;
    //-- DTC BYTE Scope:OCTET STRING SIZE 0..100;
    std::string data;

};

class SIMInfo_Model: public VdsDataBase
{
public:
    SIMInfo_Model():
    msisdn_presence(),
    msisdn(),
    imsi_presence(),
    imsi(),
    iccId_presence(),
    iccId()
    {
    }
    //--Scope:PrintableString SIZE 1..32;
    bool  msisdn_presence;
    std::string msisdn;
    //--Scope:PrintableString SIZE 1..32;
    bool  imsi_presence;
    std::string imsi;
    //--Scope:PrintableString SIZE 1..32;
    bool  iccId_presence;
    std::string iccId;

};

class BatteryStatus_Model: public VdsDataBase
{
public:
    BatteryStatus_Model():
    stateOfCharge_presence(),
    stateOfCharge(),
    chargeLevel_presence(),
    chargeLevel(),
    stateOfHealth_presence(),
    stateOfHealth(),
    powerLevel_presence(),
    powerLevel(),
    energyLevel_presence(),
    energyLevel(),
    voltage_presence(),
    voltage()
    {
    }
    //-- status, e.g. empty, low, normal, high, etc.;
    bool  stateOfCharge_presence;
    int32_t stateOfCharge;
    //-- the charge percentage;
    bool  chargeLevel_presence;
    int32_t chargeLevel;
    //-- the health of battery, to indicate whether the battery need to be replaced or not;
    bool  stateOfHealth_presence;
    int32_t stateOfHealth;
    //-- This parameter is only valid when Usage Mode = Driving Scope:INTEGER 0..15;
    bool  powerLevel_presence;
    uint8_t powerLevel;
    //-- This parameter is only valid when Usage Mode != Driving      Scope:INTEGER 0..15;
    bool  energyLevel_presence;
    uint8_t energyLevel;
    //-- V;
    bool  voltage_presence;
    int32_t voltage;

};

class ECUDiagnostic_Model: public VdsDataBase
{
public:
    ECUDiagnostic_Model():
    ecuID(),
    diagnosticResult(),
    dtcs_presence(),
    dtcs(),
    exceptionCode_presence(),
    exceptionCode()
    {
    }
    //--ECU ID which will be OEM specific, Scope:PrintableString SIZE 1..100;
    std::string ecuID;
    //-- Diagnostic results;
    Vds_HealthStatus diagnosticResult;
    //--the DTCs for the ECU;
    bool  dtcs_presence;
    std::vector<DiagnosticTroubleCode_Model> dtcs;
    //--the code to identify the exception where the ECU cannot provide the DTCs, OEM specific;
    bool  exceptionCode_presence;
    int32_t exceptionCode;

};

class FragItem_Model: public VdsDataBase
{
public:
    FragItem_Model():
    id(),
    activated(),
    level_presence(),
    level(),
    code_presence(),
    code()
    {
    }
    //--Scope:INTEGER 0..30;
    uint8_t id;
    //-- 0x0 yes,0x1 no Scope:INTEGER 0..1;
    uint8_t activated;
    //--Scope:INTEGER 0..30;
    bool  level_presence;
    uint8_t level;
    //-- tpye code Scope:PrintableString SIZE 1..20;
    bool  code_presence;
    std::string code;

};

class RvdcForcedSetting_Model: public VdsDataBase
{
public:
    RvdcForcedSetting_Model():
    visibility(),
    suppress(),
    lock()
    {
    }
    //-- Shall be set to true when make the RVDC setting invisible in the HMI. 0 is Invisible, 1 is Visible;
    bool visibility;
    //-- Shall be set to true when make the RVDC setting suppress in the HMI. 0 is not suppress, 1 is suppress;
    bool suppress;
    //-- Shall be set to true when make the RVDC setting lock in the HMI. 0 is unlock, 1 is lock;
    bool lock;

};

class RvdcVehicleSetting_Model: public VdsDataBase
{
public:
    RvdcVehicleSetting_Model():
    rvdcFunction(),
    collectOffSetTime()
    {
    }
    bool rvdcFunction;
    //--Scope:INTEGER 1..512;
    uint16_t collectOffSetTime;

};

class MaSetting_Model: public VdsDataBase
{
public:
    MaSetting_Model():
    collectFunction(),
    authorizationStatus()
    {
    }
    //-- True if the RVDC function is activated and if set to false ,no matter what dataCollection and remoteDiagnostics to set,;
    bool collectFunction;
    AuthorizationStatus_Model authorizationStatus;

};

class MeasurementAssignment_Model: public VdsDataBase
{
public:
    MeasurementAssignment_Model():
    maId(),
    maVersion(),
    vin(),
    maFormatVersion(),
    maExecutionPriority(),
    telematicsParameters(),
    usageModesAllowed(),
    startCondition(),
    stopCondition(),
    retentiondate(),
    authorizationStatus(),
    executionTrigger(),
    ecusAndPartNumbers(),
    diagnosticCommand(),
    dataIntegrityCheck(),
    serviceParameters_presence(),
    serviceParameters()
    {
    }
    //--Scope:INTEGER 1..4294967295;
    uint32_t maId;
    //--Scope:INTEGER 1..4294967295;
    uint32_t maVersion;
    //-- The VIN is used to make sure a MA is not sent to cars which are not supposed to have the MA. This is also used for Signature verification. Scope:PrintableString SIZE 1..128;
    std::string vin;
    //-- Describes the protocol version of the MA. This must be the same as the SW in vehicle gateway Scope:INTEGER 1..4294967295;
    uint32_t maFormatVersion;
    //-- The execution priority of the MA Scope:INTEGER 1..65535;
    uint16_t maExecutionPriority;
    //-- This contains telematics parameters;
    TelematicsParameters_Model telematicsParameters;
    //-- Shall equal the usage mode signal, only values active or driving are allowed. Scope:INTEGER 1..4294967295;
    uint32_t usageModesAllowed;
    //-- The start time for the MA Scope:PrintableString SIZE 1..32;
    std::string startCondition;
    //-- The condition to stop the MA. At least one stop condition must be chosen. This shall be checked during parsing.;
    StopCondition_Model stopCondition;
    //-- The retention date for the MA. Scope:PrintableString SIZE 1..32;
    std::string retentiondate;
    //-- The authorization status needed to perform the MA		;
    AuthorizationStatus_Model authorizationStatus;
    //-- The type of execution trigger in the MA. Only one trigger type can be chosen.;
    ExecutionTrigger_Model executionTrigger;
    //-- The number of ECUs which are needed to perform MA.;
    std::vector<EcusAndPartNumber_Model> ecusAndPartNumbers;
    //-- Specifies the diagnostic commands to be executed;
    std::vector<DiagnosticCommand_Model> diagnosticCommand;
    //-- This shall be a CRC32 over the entire payload. Scope:PrintableString SIZE 1..64;
    std::string dataIntegrityCheck;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Model> serviceParameters;

};

class Mdp_Model: public VdsDataBase
{
public:
    Mdp_Model():
    mdpFormatVersion(),
    vin(),
    ma(),
    sequenceNumber(),
    timestampReadOutStart(),
    udsTriggerId_presence(),
    udsTriggerId(),
    diagnosticResponse()
    {
    }
    //-- This is version of the MDP format which the ECU sends the data. Scope:INTEGER 1..256;
    uint16_t mdpFormatVersion;
    //-- The VIN of the vehicle, used to make sure signature is car unique. Scope:PrintableString SIZE 1..128;
    std::string vin;
    Ma_Model ma;
    //-- Shall contain the MA Sequence counter value. Scope:INTEGER 1..4294967295;
    uint32_t sequenceNumber;
    //-- The timestamp of the first diagnostic response was collected Scope:PrintableString SIZE 1..32;
    std::string timestampReadOutStart;
    //-- If the execution trigger was "UDS", this ELEMENT contains the ID of the condition which triggered the readout if the condition operator was set to "OR". Scope:PrintableString SIZE 1..64;
    bool  udsTriggerId_presence;
    std::string udsTriggerId;
    //--The raw response of the diagnostic request. The string must have dynamic length. Scope:PrintableString SIZE 1..65535;
    std::string diagnosticResponse;

};

class Instruction_Model: public VdsDataBase
{
public:
    Instruction_Model():
    ecuRemaining(),
    installationInstructionSversion(),
    requiredPreparationTime(),
    expectedInstallationTime(),
    area1112SecurityCode(),
    ecuInstructions(),
    assignmentValidations()
    {
    }
    //--Scope:INTEGER 1..255;
    uint8_t ecuRemaining;
    //--Scope:PrintableString SIZE 1..256;
    std::string installationInstructionSversion;
    //--Scope:INTEGER 1..4294967295;
    uint32_t requiredPreparationTime;
    //--Scope:INTEGER 1..4294967295;
    uint32_t expectedInstallationTime;
    //--Scope:PrintableString SIZE 1..32;
    std::string area1112SecurityCode;
    std::vector<EcuInstruction_Model> ecuInstructions;
    std::vector<AssignmentValidation_Model> assignmentValidations;

};

class EcuSummary_Model: public VdsDataBase
{
public:
    EcuSummary_Model():
    ecuAddress(),
    ecuRetries(),
    ecuStatus(),
    softwarePartSummarys()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //--Scope:INTEGER 1..100;
    uint8_t ecuRetries;
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuStatus;
    std::vector<SoftwarePartSummary_Model> softwarePartSummarys;

};

class ExceptionReport_Model: public VdsDataBase
{
public:
    ExceptionReport_Model():
    installationOrderId_presence(),
    installationOrderId(),
    timestamp(),
    issuerId(),
    clientConfigurationId_presence(),
    clientConfigurationId(),
    dataFileName_presence(),
    dataFileName(),
    ecuAddress_presence(),
    ecuAddress(),
    validationKey_presence(),
    validationKey(),
    partIdentifier_presence(),
    partIdentifier(),
    dataBlockNumber_presence(),
    dataBlockNumber(),
    exceptionMessage()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    bool  installationOrderId_presence;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //--Scope:PrintableString SIZE 1..256;
    std::string issuerId;
    //--Scope:PrintableString SIZE 1..256;
    bool  clientConfigurationId_presence;
    std::string clientConfigurationId;
    //--Scope:UTF8String SIZE 1..256;
    bool  dataFileName_presence;
    std::string dataFileName;
    //--Scope:PrintableString SIZE 1..256;
    bool  ecuAddress_presence;
    std::string ecuAddress;
    //--Scope:PrintableString SIZE 1..256;
    bool  validationKey_presence;
    std::string validationKey;
    //--Scope:PrintableString SIZE 1..256;
    bool  partIdentifier_presence;
    std::string partIdentifier;
    //--Scope:INTEGER 1..65535;
    bool  dataBlockNumber_presence;
    uint16_t dataBlockNumber;
    ExceptionMessage_Model exceptionMessage;

};

class AssignmentFileInfo_Model: public VdsDataBase
{
public:
    AssignmentFileInfo_Model():
    partidentifier(),
    softwarePartSignature(),
    fileCheckSum()
    {
    }
    //--Scope:PrintableString SIZE 1..256;
    std::string partidentifier;
    //--Scope:PrintableString SIZE 1..256;
    std::string softwarePartSignature;
    //--Scope:PrintableString SIZE 1..256;
    std::string fileCheckSum;

};

class NEVDRPositionData_Model: public VdsDataBase
{
public:
    NEVDRPositionData_Model():
    longLat(),
    heading(),
    speedKmph(),
    hdopX10(),
    numSat(),
    fixTime(),
    fixType(),
    drType(),
    drDistance()
    {
    }
    NEVCoordinatesLongLat_Model longLat;
    //--Scope:INTEGER 0..360;
    uint16_t heading;
    //--255=unknown  Scope:INTEGER 0..255;
    uint8_t speedKmph;
    //--HDOP multiplied by 10. 255=unknown  Scope:INTEGER 0..255;
    uint8_t hdopX10;
    //--Number of satellites used in fix. 127=unknown  Scope:INTEGER 0..127;
    uint8_t numSat;
    //--GPS time received with the fix ;
    NEVDateTime_Model fixTime;
    //--Specifies the current GNSS fix, not regarding DR ;
    Vds_NEVGnssFixType fixType;
    Vds_NEVDeadReckoningType drType;
    //--Distance in meters driven since last fix  Scope:INTEGER 0..65535;
    uint16_t drDistance;

};

class ECUData_Model: public VdsDataBase
{
public:
    ECUData_Model():
    ecu(),
    diagnosticNumber(),
    didData(),
    dtcData()
    {
    }
    //-- The ECU ID  Scope:INTEGER 0..65535;
    uint16_t ecu;
    //-- Diagnostic Part number   Scope:PrintableString SIZE 1..20;
    std::string diagnosticNumber;
    std::vector<DIDData_Model> didData;
    std::vector<DTCData_Model> dtcData;

};

class TrackPoint_Model: public VdsDataBase
{
public:
    TrackPoint_Model():
    systemTime(),
    position(),
    speed(),
    direction_presence(),
    direction(),
    isSVT_presence(),
    isSVT(),
    odometer_presence(),
    odometer(),
    travelDistanceSinceLastWaypoint_presence(),
    travelDistanceSinceLastWaypoint(),
    fuelConsumptionSinceLastWaypoint_presence(),
    fuelConsumptionSinceLastWaypoint(),
    electricConsumptionSinceLastSample_presence(),
    electricConsumptionSinceLastSample(),
    electricRegenerationSinceLastSample_presence(),
    electricRegenerationSinceLastSample()
    {
    }
    TimeStamp_Model systemTime;
    Position_Model position;
    //-- kmph;
    int32_t speed;
    //-- 0 means North, clockwise				 Scope:INTEGER 0..360;
    bool  direction_presence;
    uint16_t direction;
    bool  isSVT_presence;
    bool isSVT;
    bool  odometer_presence;
    int32_t odometer;
    bool  travelDistanceSinceLastWaypoint_presence;
    int32_t travelDistanceSinceLastWaypoint;
    bool  fuelConsumptionSinceLastWaypoint_presence;
    int32_t fuelConsumptionSinceLastWaypoint;
    bool  electricConsumptionSinceLastSample_presence;
    int32_t electricConsumptionSinceLastSample;
    bool  electricRegenerationSinceLastSample_presence;
    int32_t electricRegenerationSinceLastSample;

};

class PostAddress_Model: public VdsDataBase
{
public:
    PostAddress_Model():
    street_presence(),
    street(),
    houseNumber_presence(),
    houseNumber(),
    city_presence(),
    city(),
    region_presence(),
    region(),
    postcode_presence(),
    postcode(),
    country_presence(),
    country()
    {
    }
    //--Scope:UTF8String SIZE 0..255;
    bool  street_presence;
    std::string street;
    //--Scope:UTF8String SIZE 0..63;
    bool  houseNumber_presence;
    std::string houseNumber;
    //--Scope:UTF8String SIZE 0..255;
    bool  city_presence;
    std::string city;
    //--Scope:UTF8String SIZE 0..255;
    bool  region_presence;
    std::string region;
    //--Scope:UTF8String SIZE 0..255;
    bool  postcode_presence;
    std::string postcode;
    //--Scope:UTF8String SIZE 0..255;
    bool  country_presence;
    std::string country;

};

class ConfigurationAndIdentity_Model: public VdsDataBase
{
public:
    ConfigurationAndIdentity_Model():
    vin_presence(),
    vin(),
    fuelType_presence(),
    fuelType()
    {
    }
    bool  vin_presence;
    std::string vin;
    bool  fuelType_presence;
    Vds_FuelType fuelType;

};

class NetworkAccessStatus_Model: public VdsDataBase
{
public:
    NetworkAccessStatus_Model():
    simInfo_presence(),
    simInfo(),
    mobileNetwork_presence(),
    mobileNetwork()
    {
    }
    bool  simInfo_presence;
    SIMInfo_Model simInfo;
    //-- 2G/3G/4G/wifi Scope:PrintableString SIZE 0..127;
    bool  mobileNetwork_presence;
    std::string mobileNetwork;

};

class ConnectivityStatus_Model: public VdsDataBase
{
public:
    ConnectivityStatus_Model():
    wakeupTimePosition_presence(),
    wakeupTimePosition(),
    connectEstablishedList_presence(),
    connectEstablishedList(),
    connectTime_presence(),
    connectTime(),
    connectPosition_presence(),
    connectPosition(),
    connectType_presence(),
    connectType(),
    numRetriesConnect_presence(),
    numRetriesConnect(),
    retryReason(),
    disconnectType_presence(),
    disconnectType(),
    lostConnectionsList_presence(),
    lostConnectionsList(),
    sleeptimePosition_presence(),
    sleeptimePosition()
    {
    }
    bool  wakeupTimePosition_presence;
    TimePosition_Model wakeupTimePosition;
    bool  connectEstablishedList_presence;
    std::vector<ConnectivityInfo_Model> connectEstablishedList;
    bool  connectTime_presence;
    TimeStamp_Model connectTime;
    bool  connectPosition_presence;
    Position_Model connectPosition;
    //--Scope:PrintableString SIZE 0..50;
    bool  connectType_presence;
    std::string connectType;
    //--Scope:INTEGER 0..65535;
    bool  numRetriesConnect_presence;
    uint16_t numRetriesConnect;
    //--Scope:PrintableString SIZE 0..255;
    std::string retryReason;
    //--Scope:INTEGER 0..1;
    bool  disconnectType_presence;
    uint8_t disconnectType;
    bool  lostConnectionsList_presence;
    std::vector<ConnectivityInfo_Model> lostConnectionsList;
    bool  sleeptimePosition_presence;
    TimePosition_Model sleeptimePosition;

};

class FragStrs_Model: public VdsDataBase
{
public:
    FragStrs_Model():
    activated(),
    number_presence(),
    number(),
    items_presence(),
    items()
    {
    }
    //-- 0x0 yes,0x1 no Scope:INTEGER 0..1;
    uint8_t activated;
    //--Scope:INTEGER 0..30;
    bool  number_presence;
    uint8_t number;
    bool  items_presence;
    std::vector<FragItem_Model> items;

};

class TowStatus_Model: public VdsDataBase
{
public:
    TowStatus_Model():
    trailerLampActiveChk_presence(),
    trailerLampActiveChk(),
    trailerLampCheckSts_presence(),
    trailerLampCheckSts(),
    trailerBreakLampSts_presence(),
    trailerBreakLampSts(),
    trailerFogLampSts_presence(),
    trailerFogLampSts(),
    trailerPosLampSts_presence(),
    trailerPosLampSts(),
    trailerTurningLampSts_presence(),
    trailerTurningLampSts(),
    trailerReversingLampSts_presence(),
    trailerReversingLampSts()
    {
    }
    //--Scope:INTEGER 0..20;
    bool  trailerLampActiveChk_presence;
    uint8_t trailerLampActiveChk;
    //--Scope:INTEGER 0..20;
    bool  trailerLampCheckSts_presence;
    uint8_t trailerLampCheckSts;
    //--Scope:INTEGER 0..20;
    bool  trailerBreakLampSts_presence;
    uint8_t trailerBreakLampSts;
    //--Scope:INTEGER 0..20;
    bool  trailerFogLampSts_presence;
    uint8_t trailerFogLampSts;
    //--Scope:INTEGER 0..20;
    bool  trailerPosLampSts_presence;
    uint8_t trailerPosLampSts;
    //--Scope:INTEGER 0..20;
    bool  trailerTurningLampSts_presence;
    uint8_t trailerTurningLampSts;
    //--Scope:INTEGER 0..20;
    bool  trailerReversingLampSts_presence;
    uint8_t trailerReversingLampSts;

};

class RvdcSetting_Model: public VdsDataBase
{
public:
    RvdcSetting_Model():
    maSetting_presence(),
    maSetting(),
    vehicleSetting_presence(),
    vehicleSetting(),
    languageSetting_presence(),
    languageSetting(),
    forcedSetting_presence(),
    forcedSetting(),
    serviceParameters_presence(),
    serviceParameters()
    {
    }
    //-- The objective of this interface is to set RVDC Authorization Status for cloud to end or end to cloud;
    bool  maSetting_presence;
    MaSetting_Model maSetting;
    //-- The objective of this interface is to provide RVDC vehicle settings to vehicle.;
    bool  vehicleSetting_presence;
    RvdcVehicleSetting_Model vehicleSetting;
    //-- The objective of this interface is to provide the changed language setting of in-vehicle HMI to RVDC server.;
    bool  languageSetting_presence;
    LanguageSetting_Model languageSetting;
    bool  forcedSetting_presence;
    RvdcForcedSetting_Model forcedSetting;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Model> serviceParameters;

};

class RvdcAssignmentNotification_Model: public VdsDataBase
{
public:
    RvdcAssignmentNotification_Model():
    maCarId_presence(),
    maCarId(),
    maVersion_presence(),
    maVersion(),
    newStatus(),
    timestamp(),
    reason_presence(),
    reason()
    {
    }
    //-- Identifier derived from associated MA Scope:INTEGER 1..4294967295;
    bool  maCarId_presence;
    uint32_t maCarId;
    //-- Version derived from associated MA Scope:INTEGER 1..4294967295;
    bool  maVersion_presence;
    uint32_t maVersion;
    //-- Notification of transition to new status or state by the client (vehicle) Scope:PrintableString SIZE 1..128;
    std::string newStatus;
    //-- The client recorded time of the notification Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //-- Reason for the transition Scope:PrintableString SIZE 1..512;
    bool  reason_presence;
    std::string reason;

};

class MaSynchronizationReq_Model: public VdsDataBase
{
public:
    MaSynchronizationReq_Model():
    storageQuota(),
    maFormatVersion(),
    installedMa_presence(),
    installedMa(),
    serviceParameters_presence(),
    serviceParameters()
    {
    }
    //--Scope:INTEGER 1..4294967295;
    uint32_t storageQuota;
    //-- The cloud sends corresponding data according to the version reported by the vehicle. If the version of the vehicle changes, the cloud needs to be notified first. Scope:INTEGER 1..4294967295;
    uint32_t maFormatVersion;
    //-- installed measurements in vehicle;
    bool  installedMa_presence;
    std::vector<Ma_Model> installedMa;
    //-- generic service parameters used for extension,;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Model> serviceParameters;

};

class MaSynchronization_Model: public VdsDataBase
{
public:
    MaSynchronization_Model():
    removeMas_presence(),
    removeMas(),
    newMas_presence(),
    newMas(),
    serviceParameters_presence(),
    serviceParameters()
    {
    }
    //-- These MAs shall be removed;
    bool  removeMas_presence;
    std::vector<Ma_Model> removeMas;
    //-- New or updated MAs;
    bool  newMas_presence;
    std::vector<MeasurementAssignment_Model> newMas;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Model> serviceParameters;

};

class MeasurementDataPackage_Model: public VdsDataBase
{
public:
    MeasurementDataPackage_Model():
    telematicsParameters(),
    serviceParameters_presence(),
    serviceParameters(),
    mdp()
    {
    }
    //-- the onboard telematics unit parameters;
    TelematicsParameters_Model telematicsParameters;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Model> serviceParameters;
    //-- actual data payload;
    Mdp_Model mdp;

};

class RvdcExceptionHandling_Model: public VdsDataBase
{
public:
    RvdcExceptionHandling_Model():
    exceptionName(),
    description(),
    publisher(),
    severity_presence(),
    severity(),
    impact_presence(),
    impact(),
    action_presence(),
    action()
    {
    }
    //--Scope:UTF8String SIZE 1..128;
    std::string exceptionName;
    //--Scope:UTF8String SIZE 1..512;
    std::string description;
    //--Scope:UTF8String SIZE 1..128;
    std::string publisher;
    //--Scope:UTF8String SIZE 1..128;
    bool  severity_presence;
    std::string severity;
    //--Scope:UTF8String SIZE 1..128;
    bool  impact_presence;
    std::string impact;
    //--Scope:UTF8String SIZE 1..128;
    bool  action_presence;
    std::string action;

};

class ClientConfSyncRequest_Model: public VdsDataBase
{
public:
    ClientConfSyncRequest_Model():
    timestamp(),
    clientVersion(),
    ecuInventory_presence(),
    ecuInventory()
    {
    }
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //--Scope:PrintableString SIZE 1..256;
    std::string clientVersion;
    bool  ecuInventory_presence;
    std::vector<EcuInventory_Model> ecuInventory;

};

class InventoryInstruction_Model: public VdsDataBase
{
public:
    InventoryInstruction_Model():
    inventoryInstructionVersion(),
    ecuInstructions()
    {
    }
    //--Scope:PrintableString SIZE 1..32;
    std::string inventoryInstructionVersion;
    std::vector<EcuInventory_Model> ecuInstructions;

};

class AvailableAssignmentRequest_Model: public VdsDataBase
{
public:
    AvailableAssignmentRequest_Model():
    languageSetting()
    {
    }
    LanguageSetting_Model languageSetting;

};

class AvailableAssignment_Model: public VdsDataBase
{
public:
    AvailableAssignment_Model():
    installationOrderId(),
    timestamp(),
    workshopInstallation(),
    downloadsize(),
    totalinstallationTime(),
    newstatus(),
    reason_presence(),
    reason(),
    name(),
    shortDescription(),
    longDescription_presence(),
    longDescription()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    bool workshopInstallation;
    //--Scope:INTEGER 1..2147483647;
    uint32_t downloadsize;
    //--Scope:INTEGER 1..36000000;
    uint32_t totalinstallationTime;
    //--Scope:PrintableString SIZE 1..64;
    std::string newstatus;
    //--Scope:UTF8String SIZE 1..256;
    bool  reason_presence;
    std::string reason;
    //--Scope:UTF8String SIZE 1..256;
    std::string name;
    //--Scope:UTF8String SIZE 1..1024;
    std::string shortDescription;
    //--Scope:UTF8String SIZE 1..65535;
    bool  longDescription_presence;
    std::string longDescription;

};

class InstallationInstruction_Model: public VdsDataBase
{
public:
    InstallationInstruction_Model():
    installationOrderId(),
    instructions()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    std::vector<Instruction_Model> instructions;

};

class FotaAssignmentNotification_Model: public VdsDataBase
{
public:
    FotaAssignmentNotification_Model():
    installationOrderId_presence(),
    installationOrderId(),
    timestamp(),
    newStatus(),
    reason_presence(),
    reason(),
    deltaTime_presence(),
    deltaTime(),
    serviceParams_presence(),
    serviceParams()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    bool  installationOrderId_presence;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //--Scope:PrintableString SIZE 1..256;
    std::string newStatus;
    //--Scope:UTF8String SIZE 1..256;
    bool  reason_presence;
    std::string reason;
    //--Scope:INTEGER 1..604800;
    bool  deltaTime_presence;
    uint32_t deltaTime;
    bool  serviceParams_presence;
    std::vector<ServiceParameter_Model> serviceParams;

};

class InstallationSummary_Model: public VdsDataBase
{
public:
    InstallationSummary_Model():
    installationOrderId(),
    timestamp(),
    repeatresets(),
    totalInstallationTime(),
    ecuSummarys()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //--Scope:INTEGER 1..255;
    uint8_t repeatresets;
    //--Scope:INTEGER 1..4294967295;
    uint32_t totalInstallationTime;
    std::vector<EcuSummary_Model> ecuSummarys;

};

class EcuExceptionReport_Model: public VdsDataBase
{
public:
    EcuExceptionReport_Model():
    exceptionReportMsgRemaining(),
    exceptionReports()
    {
    }
    //--Scope:INTEGER 1..65535;
    uint16_t exceptionReportMsgRemaining;
    //-- Shall be maximum 3 reports;
    std::vector<ExceptionReport_Model> exceptionReports;

};

class ClientHmiVehicleSettings_Model: public VdsDataBase
{
public:
    ClientHmiVehicleSettings_Model():
    otaSetting(),
    autoSync(),
    autoDownload(),
    autoInstallation()
    {
    }
    bool otaSetting;
    bool autoSync;
    bool autoDownload;
    bool autoInstallation;

};

class AssignmentData_Model: public VdsDataBase
{
public:
    AssignmentData_Model():
    installationOrderId(),
    timestamp(),
    urls()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    std::vector<std::string> urls;

};

class OtaAssignmentFileInfo_Model: public VdsDataBase
{
public:
    OtaAssignmentFileInfo_Model():
    installationOrderId(),
    assignFileInfos()
    {
    }
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    std::vector<AssignmentFileInfo_Model> assignFileInfos;

};

class EcuData_Model: public VdsDataBase
{
public:
    EcuData_Model():
    noDiagData_choice(),
    data_choice(),
    data()
    {
    }
    //-- No diagnostic data included in the MDP due to Charging, end of driving cycle or tester with higher priority is present. ;
    bool  noDiagData_choice;
    //-- ECU data;
    bool  data_choice;
    std::vector<ECUData_Model> data;

};

class NEVDRVehiclePosition_Model: public VdsDataBase
{
public:
    NEVDRVehiclePosition_Model():
    noValidData_choice(),
    drPosition_choice(),
    drPosition()
    {
    }
    //--Used if the server can not provide a position of sufficient quality ;
    bool  noValidData_choice;
    bool  drPosition_choice;
    NEVDRPositionData_Model drPosition;

};

class Error_Model: public VdsDataBase
{
public:
    Error_Model():
    code(),
    vehicleErrorCode_presence(),
    vehicleErrorCode(),
    message_presence(),
    message()
    {
    }
    Vds_ErrorCode code;
    //-- the error code from vehicle/TEM, OEM specific;
    bool  vehicleErrorCode_presence;
    int32_t vehicleErrorCode;
    //-- the addtional error description Scope:PrintableString SIZE 0..255;
    bool  message_presence;
    std::string message;

};

class ConfigurationItem_Model: public VdsDataBase
{
public:
    ConfigurationItem_Model():
    name(),
    parameters()
    {
    }
    std::string name;
    std::vector<ServiceParameter_Model> parameters;

};

class ContactInfo_Model: public VdsDataBase
{
public:
    ContactInfo_Model():
    email_presence(),
    email(),
    address_presence(),
    address(),
    phone1_presence(),
    phone1(),
    phone2_presence(),
    phone2()
    {
    }
    //--Scope:UTF8String SIZE 0..255;
    bool  email_presence;
    std::string email;
    bool  address_presence;
    PostAddress_Model address;
    //--Scope:PrintableString SIZE 0..27;
    bool  phone1_presence;
    std::string phone1;
    //--Scope:PrintableString SIZE 0..27;
    bool  phone2_presence;
    std::string phone2;

};

class Notification_Model: public VdsDataBase
{
public:
    Notification_Model():
    reason(),
    time(),
    code_presence(),
    code(),
    parameters_presence(),
    parameters()
    {
    }
    Vds_NotificationReason reason;
    TimeStamp_Model time;
    bool  code_presence;
    std::string code;
    bool  parameters_presence;
    std::vector<ServiceParameter_Model> parameters;

};

class RuleCondition_Model: public VdsDataBase
{
public:
    RuleCondition_Model():
    condition()
    {
    }
    //-- format: <vehicle-status> <operator> <target> Scope:PrintableString SIZE 1..1000;
    std::string condition;

};

class EventAction_Model: public VdsDataBase
{
public:
    EventAction_Model():
    triggerTimes_presence(),
    triggerTimes(),
    notifyInterval_presence(),
    notifyInterval()
    {
    }
    //-- Trigger notification times, 0 means always trigger;
    bool  triggerTimes_presence;
    int32_t triggerTimes;
    //-- notify interval ;
    bool  notifyInterval_presence;
    int32_t notifyInterval;

};

class TimerInfo_Model: public VdsDataBase
{
public:
    TimerInfo_Model():
    timerActivation(),
    duration_presence(),
    duration(),
    dayofWeek_presence(),
    dayofWeek(),
    startTimeofDay_presence(),
    startTimeofDay(),
    endtimeofDay_presence(),
    endtimeofDay()
    {
    }
    //--Scope:INTEGER 0..1;
    uint8_t timerActivation;
    //--Scope:INTEGER 0..3153600;
    bool  duration_presence;
    uint32_t duration;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  dayofWeek_presence;
    std::string dayofWeek;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  startTimeofDay_presence;
    std::string startTimeofDay;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  endtimeofDay_presence;
    std::string endtimeofDay;

};

class BasicVehicleStatus_Model: public VdsDataBase
{
public:
    BasicVehicleStatus_Model():
    vin_presence(),
    vin(),
    position(),
    speed(),
    speedUnit_presence(),
    speedUnit(),
    speedValidity_presence(),
    speedValidity(),
    direction_presence(),
    direction(),
    engineStatus_presence(),
    engineStatus(),
    keyStatus_presence(),
    keyStatus(),
    usageMode_presence(),
    usageMode()
    {
    }
    bool  vin_presence;
    std::string vin;
    Position_Model position;
    //-- km/h   ;
    int32_t speed;
    bool  speedUnit_presence;
    std::string speedUnit;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  speedValidity_presence;
    uint8_t speedValidity;
    //--Scope:INTEGER 0..360;
    bool  direction_presence;
    uint16_t direction;
    bool  engineStatus_presence;
    Vds_EngineStatus engineStatus;
    bool  keyStatus_presence;
    Vds_KeyStatus keyStatus;
    //-- only for CMA car;
    bool  usageMode_presence;
    int32_t usageMode;

};

class LockStatus_Model: public VdsDataBase
{
public:
    LockStatus_Model():
    winStatusDriver_presence(),
    winStatusDriver(),
    winStatusPassenger_presence(),
    winStatusPassenger(),
    winStatusDriverRear_presence(),
    winStatusDriverRear(),
    winStatusPassengerRear_presence(),
    winStatusPassengerRear(),
    winStatusDriverWarning_presence(),
    winStatusDriverWarning(),
    winStatusPassengerWarning_presence(),
    winStatusPassengerWarning(),
    winStatusDriverRearWarning_presence(),
    winStatusDriverRearWarning(),
    winStatusPassengerRearWarning_presence(),
    winStatusPassengerRearWarning(),
    winPosDriver_presence(),
    winPosDriver(),
    winPosPassenger_presence(),
    winPosPassenger(),
    winPosDriverRear_presence(),
    winPosDriverRear(),
    winPosPassengerRear_presence(),
    winPosPassengerRear(),
    sunroofOpenStatus_presence(),
    sunroofOpenStatus(),
    sunroofPos_presence(),
    sunroofPos(),
    sunroofOpenStatusWarning_presence(),
    sunroofOpenStatusWarning(),
    curtainOpenStatus_presence(),
    curtainOpenStatus(),
    curtainPos_presence(),
    curtainPos(),
    curtainWarning_presence(),
    curtainWarning(),
    tankFlapStatus_presence(),
    tankFlapStatus(),
    chargeLidRearStatus_presence(),
    chargeLidRearStatus(),
    chargeLidFrontStatus_presence(),
    chargeLidFrontStatus(),
    ventilateStatus_presence(),
    ventilateStatus(),
    doorOpenStatusDriver_presence(),
    doorOpenStatusDriver(),
    doorOpenStatusPassenger_presence(),
    doorOpenStatusPassenger(),
    doorOpenStatusDriverRear_presence(),
    doorOpenStatusDriverRear(),
    doorOpenStatusPassengerRear_presence(),
    doorOpenStatusPassengerRear(),
    doorPosDriver_presence(),
    doorPosDriver(),
    doorPosPassenger_presence(),
    doorPosPassenger(),
    doorPosDriverRear_presence(),
    doorPosDriverRear(),
    doorPosPassengerRear_presence(),
    doorPosPassengerRear(),
    doorLockStatusDriver_presence(),
    doorLockStatusDriver(),
    doorLockStatusPassenger_presence(),
    doorLockStatusPassenger(),
    doorLockStatusDriverRear_presence(),
    doorLockStatusDriverRear(),
    doorLockStatusPassengerRear_presence(),
    doorLockStatusPassengerRear(),
    doorGripStatusDriver_presence(),
    doorGripStatusDriver(),
    doorGripStatusPassenger_presence(),
    doorGripStatusPassenger(),
    doorGripStatusDriverRear_presence(),
    doorGripStatusDriverRear(),
    doorGripStatusPassengerRear_presence(),
    doorGripStatusPassengerRear(),
    trunkOpenStatus_presence(),
    trunkOpenStatus(),
    trunkLockStatus_presence(),
    trunkLockStatus(),
    engineHoodOpenStatus_presence(),
    engineHoodOpenStatus(),
    engineHoodLockStatus_presence(),
    engineHoodLockStatus(),
    centralLockingStatus_presence(),
    centralLockingStatus(),
    centralLockingDisStatus_presence(),
    centralLockingDisStatus(),
    privateLockStatus_presence(),
    privateLockStatus(),
    vehicleAlarm_presence(),
    vehicleAlarm(),
    winCloseReminder_presence(),
    winCloseReminder(),
    handBrakeStatus_presence(),
    handBrakeStatus(),
    electricParkBrakeStatus_presence(),
    electricParkBrakeStatus()
    {
    }
    //--Scope:INTEGER 0..15;
    bool  winStatusDriver_presence;
    uint8_t winStatusDriver;
    //--Scope:INTEGER 0..15;
    bool  winStatusPassenger_presence;
    uint8_t winStatusPassenger;
    //--Scope:INTEGER 0..15;
    bool  winStatusDriverRear_presence;
    uint8_t winStatusDriverRear;
    //--Scope:INTEGER 0..15;
    bool  winStatusPassengerRear_presence;
    uint8_t winStatusPassengerRear;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  winStatusDriverWarning_presence;
    uint8_t winStatusDriverWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  winStatusPassengerWarning_presence;
    uint8_t winStatusPassengerWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  winStatusDriverRearWarning_presence;
    uint8_t winStatusDriverRearWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  winStatusPassengerRearWarning_presence;
    uint8_t winStatusPassengerRearWarning;
    //--Scope:INTEGER 0..1000;
    bool  winPosDriver_presence;
    uint16_t winPosDriver;
    //--Scope:INTEGER 0..1000;
    bool  winPosPassenger_presence;
    uint16_t winPosPassenger;
    //--Scope:INTEGER 0..1000;
    bool  winPosDriverRear_presence;
    uint16_t winPosDriverRear;
    //--Scope:INTEGER 0..1000;
    bool  winPosPassengerRear_presence;
    uint16_t winPosPassengerRear;
    //--Scope:INTEGER 0..15;
    bool  sunroofOpenStatus_presence;
    uint8_t sunroofOpenStatus;
    //--Scope:INTEGER 0..1000;
    bool  sunroofPos_presence;
    uint16_t sunroofPos;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  sunroofOpenStatusWarning_presence;
    uint8_t sunroofOpenStatusWarning;
    //--Scope:INTEGER 0..15;
    bool  curtainOpenStatus_presence;
    uint8_t curtainOpenStatus;
    //--Scope:INTEGER 0..1000;
    bool  curtainPos_presence;
    uint16_t curtainPos;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  curtainWarning_presence;
    uint8_t curtainWarning;
    //--Scope:INTEGER 0..15;
    bool  tankFlapStatus_presence;
    uint8_t tankFlapStatus;
    //--Scope:INTEGER 0..15;
    bool  chargeLidRearStatus_presence;
    uint8_t chargeLidRearStatus;
    //--Scope:INTEGER 0..15;
    bool  chargeLidFrontStatus_presence;
    uint8_t chargeLidFrontStatus;
    //--Scope:INTEGER 0..1;
    bool  ventilateStatus_presence;
    uint8_t ventilateStatus;
    //--Scope:INTEGER 0..15;
    bool  doorOpenStatusDriver_presence;
    uint8_t doorOpenStatusDriver;
    //--Scope:INTEGER 0..15;
    bool  doorOpenStatusPassenger_presence;
    uint8_t doorOpenStatusPassenger;
    //--Scope:INTEGER 0..15;
    bool  doorOpenStatusDriverRear_presence;
    uint8_t doorOpenStatusDriverRear;
    //--Scope:INTEGER 0..15;
    bool  doorOpenStatusPassengerRear_presence;
    uint8_t doorOpenStatusPassengerRear;
    //--Scope:INTEGER 0..1000;
    bool  doorPosDriver_presence;
    uint16_t doorPosDriver;
    //--Scope:INTEGER 0..1000;
    bool  doorPosPassenger_presence;
    uint16_t doorPosPassenger;
    //--Scope:INTEGER 0..1000;
    bool  doorPosDriverRear_presence;
    uint16_t doorPosDriverRear;
    //--Scope:INTEGER 0..1000;
    bool  doorPosPassengerRear_presence;
    uint16_t doorPosPassengerRear;
    //--Scope:INTEGER 0..15;
    bool  doorLockStatusDriver_presence;
    uint8_t doorLockStatusDriver;
    //--Scope:INTEGER 0..15;
    bool  doorLockStatusPassenger_presence;
    uint8_t doorLockStatusPassenger;
    //--Scope:INTEGER 0..15;
    bool  doorLockStatusDriverRear_presence;
    uint8_t doorLockStatusDriverRear;
    //--Scope:INTEGER 0..15;
    bool  doorLockStatusPassengerRear_presence;
    uint8_t doorLockStatusPassengerRear;
    //--Scope:INTEGER 0..15;
    bool  doorGripStatusDriver_presence;
    uint8_t doorGripStatusDriver;
    //--Scope:INTEGER 0..15;
    bool  doorGripStatusPassenger_presence;
    uint8_t doorGripStatusPassenger;
    //--Scope:INTEGER 0..15;
    bool  doorGripStatusDriverRear_presence;
    uint8_t doorGripStatusDriverRear;
    //--Scope:INTEGER 0..15;
    bool  doorGripStatusPassengerRear_presence;
    uint8_t doorGripStatusPassengerRear;
    //--Scope:INTEGER 0..15;
    bool  trunkOpenStatus_presence;
    uint8_t trunkOpenStatus;
    //--Scope:INTEGER 0..1;
    bool  trunkLockStatus_presence;
    uint8_t trunkLockStatus;
    //--Scope:INTEGER 0..15;
    bool  engineHoodOpenStatus_presence;
    uint8_t engineHoodOpenStatus;
    //--Scope:INTEGER 0..15;
    bool  engineHoodLockStatus_presence;
    uint8_t engineHoodLockStatus;
    //--Scope:INTEGER 0..15;
    bool  centralLockingStatus_presence;
    uint8_t centralLockingStatus;
    //--Scope:INTEGER 0..15;
    bool  centralLockingDisStatus_presence;
    uint8_t centralLockingDisStatus;
    //--Scope:INTEGER 0..15;
    bool  privateLockStatus_presence;
    uint8_t privateLockStatus;
    bool  vehicleAlarm_presence;
    int32_t vehicleAlarm;
    //-- rain close the window to remind Scope:INTEGER 0..7;
    bool  winCloseReminder_presence;
    uint8_t winCloseReminder;
    //--Scope:INTEGER 0..15;
    bool  handBrakeStatus_presence;
    uint8_t handBrakeStatus;
    //--Scope:INTEGER 0..15;
    bool  electricParkBrakeStatus_presence;
    uint8_t electricParkBrakeStatus;

};

class MaintenanceStatus_Model: public VdsDataBase
{
public:
    MaintenanceStatus_Model():
    mainBatteryStatus_presence(),
    mainBatteryStatus(),
    tyreStatusDriver_presence(),
    tyreStatusDriver(),
    tyreStatusPassenger_presence(),
    tyreStatusPassenger(),
    tyreStatusDriverRear_presence(),
    tyreStatusDriverRear(),
    tyreStatusPassengerRear_presence(),
    tyreStatusPassengerRear(),
    tyreTempDriver_presence(),
    tyreTempDriver(),
    tyreTempPassenger_presence(),
    tyreTempPassenger(),
    tyreTempDriverRear_presence(),
    tyreTempDriverRear(),
    tyreTempPassengerRear_presence(),
    tyreTempPassengerRear(),
    engineHrsToService_presence(),
    engineHrsToService(),
    distanceToService_presence(),
    distanceToService(),
    daysToService_presence(),
    daysToService(),
    serviceWarningStatus_presence(),
    serviceWarningStatus(),
    serviceWarningTrigger_presence(),
    serviceWarningTrigger(),
    engineOilLevelStatus_presence(),
    engineOilLevelStatus(),
    engineOilTemperature_presence(),
    engineOilTemperature(),
    engineOilPressureWarning_presence(),
    engineOilPressureWarning(),
    engineCoolantTemperature_presence(),
    engineCoolantTemperature(),
    engineCoolantLevelStatus_presence(),
    engineCoolantLevelStatus(),
    brakeFluidLevelStatus_presence(),
    brakeFluidLevelStatus(),
    washerFluidLevelStatus_presence(),
    washerFluidLevelStatus(),
    indcrTurnLeWarning_presence(),
    indcrTurnLeWarning(),
    indcrTurnRiWarning_presence(),
    indcrTurnRiWarning(),
    indcrTurnOnPassSideWarning_presence(),
    indcrTurnOnPassSideWarning(),
    indcrTurnOnDrvrSideWarning_presence(),
    indcrTurnOnDrvrSideWarning(),
    indcrTurnLeFrntWarning_presence(),
    indcrTurnLeFrntWarning(),
    indcrTurnRiFrntWarning_presence(),
    indcrTurnRiFrntWarning(),
    fogLiReWarning_presence(),
    fogLiReWarning(),
    bulbStatus_presence(),
    bulbStatus()
    {
    }
    //-- Vehicle battery status;
    bool  mainBatteryStatus_presence;
    BatteryStatus_Model mainBatteryStatus;
    bool  tyreStatusDriver_presence;
    int32_t tyreStatusDriver;
    bool  tyreStatusPassenger_presence;
    int32_t tyreStatusPassenger;
    bool  tyreStatusDriverRear_presence;
    int32_t tyreStatusDriverRear;
    bool  tyreStatusPassengerRear_presence;
    int32_t tyreStatusPassengerRear;
    bool  tyreTempDriver_presence;
    int32_t tyreTempDriver;
    bool  tyreTempPassenger_presence;
    int32_t tyreTempPassenger;
    bool  tyreTempDriverRear_presence;
    int32_t tyreTempDriverRear;
    bool  tyreTempPassengerRear_presence;
    int32_t tyreTempPassengerRear;
    bool  engineHrsToService_presence;
    int32_t engineHrsToService;
    //-- km;
    bool  distanceToService_presence;
    int32_t distanceToService;
    //--Scope:INTEGER -150..1890;
    bool  daysToService_presence;
    uint64_t daysToService;
    bool  serviceWarningStatus_presence;
    int32_t serviceWarningStatus;
    bool  serviceWarningTrigger_presence;
    Vds_ServiceWarningTriggerReason serviceWarningTrigger;
    bool  engineOilLevelStatus_presence;
    int32_t engineOilLevelStatus;
    bool  engineOilTemperature_presence;
    int32_t engineOilTemperature;
    bool  engineOilPressureWarning_presence;
    int32_t engineOilPressureWarning;
    bool  engineCoolantTemperature_presence;
    int32_t engineCoolantTemperature;
    bool  engineCoolantLevelStatus_presence;
    int32_t engineCoolantLevelStatus;
    //-- the brake fluid level status, e.g. normal, low, high, etc.;
    bool  brakeFluidLevelStatus_presence;
    int32_t brakeFluidLevelStatus;
    //-- indicate washer fluid level low or not low;
    bool  washerFluidLevelStatus_presence;
    int32_t washerFluidLevelStatus;
    //-- 0x0: Invalid, 0x1: valid  Scope:INTEGER 0..1;
    bool  indcrTurnLeWarning_presence;
    uint8_t indcrTurnLeWarning;
    //-- 0x0: Invalid, 0x1: valid  Scope:INTEGER 0..1;
    bool  indcrTurnRiWarning_presence;
    uint8_t indcrTurnRiWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  indcrTurnOnPassSideWarning_presence;
    uint8_t indcrTurnOnPassSideWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  indcrTurnOnDrvrSideWarning_presence;
    uint8_t indcrTurnOnDrvrSideWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  indcrTurnLeFrntWarning_presence;
    uint8_t indcrTurnLeFrntWarning;
    //-- 0x0: Invalid, 0x1: valid   Scope:INTEGER 0..1;
    bool  indcrTurnRiFrntWarning_presence;
    uint8_t indcrTurnRiFrntWarning;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  fogLiReWarning_presence;
    uint8_t fogLiReWarning;
    bool  bulbStatus_presence;
    int32_t bulbStatus;

};

class RunningStatus_Model: public VdsDataBase
{
public:
    RunningStatus_Model():
    srsStatus_presence(),
    srsStatus(),
    srsCrashStatus_presence(),
    srsCrashStatus(),
    odometer_presence(),
    odometer(),
    fuelLevel_presence(),
    fuelLevel(),
    fuelLevelStatus_presence(),
    fuelLevelStatus(),
    fuelEnLevel_presence(),
    fuelEnLevel(),
    fuelEnCnsFild_presence(),
    fuelEnCnsFild(),
    fuelEnCns_presence(),
    fuelEnCns(),
    fuelLow1WarningDriver_presence(),
    fuelLow1WarningDriver(),
    fuelLow2WarningDriver_presence(),
    fuelLow2WarningDriver(),
    distanceToEmpty_presence(),
    distanceToEmpty(),
    trvlDistance_presence(),
    trvlDistance(),
    aveFuelConsumption_presence(),
    aveFuelConsumption(),
    aveFuelConsumptionInLatestDrivingCycle_presence(),
    aveFuelConsumptionInLatestDrivingCycle(),
    avgSpeed_presence(),
    avgSpeed(),
    avgSpeedLatestDrivingCycle_presence(),
    avgSpeedLatestDrivingCycle(),
    aveFuelConsumptionUnit_presence(),
    aveFuelConsumptionUnit(),
    indFuelConsumption_presence(),
    indFuelConsumption(),
    notifForEmgyCallStatus_presence(),
    notifForEmgyCallStatus(),
    tyrePreWarningDriver_presence(),
    tyrePreWarningDriver(),
    tyrePreWarningPassenger_presence(),
    tyrePreWarningPassenger(),
    tyrePreWarningDriverRear_presence(),
    tyrePreWarningDriverRear(),
    tyrePreWarningPassengerRear_presence(),
    tyrePreWarningPassengerRear(),
    tyreTempWarningDriver_presence(),
    tyreTempWarningDriver(),
    tyreTempWarningPassenger_presence(),
    tyreTempWarningPassenger(),
    tyreTempWarningDriverRear_presence(),
    tyreTempWarningDriverRear(),
    tyreTempWarningPassengerRear_presence(),
    tyreTempWarningPassengerRear(),
    seatBeltStatusDriver_presence(),
    seatBeltStatusDriver(),
    seatBeltStatusPassenger_presence(),
    seatBeltStatusPassenger(),
    seatBeltStatusDriverRear_presence(),
    seatBeltStatusDriverRear(),
    seatBeltStatusPassengerRear_presence(),
    seatBeltStatusPassengerRear(),
    seatBeltStatusMidRear_presence(),
    seatBeltStatusMidRear(),
    seatBeltStatusThDriverRear_presence(),
    seatBeltStatusThDriverRear(),
    seatBeltStatusThPassengerRear_presence(),
    seatBeltStatusThPassengerRear(),
    seatBeltStatusThMidRear_presence(),
    seatBeltStatusThMidRear(),
    brakePedalDepressed_presence(),
    brakePedalDepressed(),
    gearManualStatus_presence(),
    gearManualStatus(),
    gearAutoStatus_presence(),
    gearAutoStatus(),
    engineSpeed_presence(),
    engineSpeed(),
    transimissionGearPostion_presence(),
    transimissionGearPostion(),
    cruiseControlStatus_presence(),
    cruiseControlStatus(),
    engineBlockedStatus_presence(),
    engineBlockedStatus(),
    tripMeter1_presence(),
    tripMeter1(),
    tripMeter2_presence(),
    tripMeter2()
    {
    }
    //-- SRS(airbag) status, deployed or non-deployed;
    bool  srsStatus_presence;
    Vds_SRSStatus srsStatus;
    //-- CrashStsSafe;
    bool  srsCrashStatus_presence;
    int32_t srsCrashStatus;
    //-- meter, distance in total;
    bool  odometer_presence;
    int32_t odometer;
    //-- fuel level in liters or percentage;
    bool  fuelLevel_presence;
    int32_t fuelLevel;
    bool  fuelLevelStatus_presence;
    int32_t fuelLevelStatus;
    //-- fuel in enginer level in liters or percentage;
    bool  fuelEnLevel_presence;
    int32_t fuelEnLevel;
    bool  fuelEnCnsFild_presence;
    int32_t fuelEnCnsFild;
    bool  fuelEnCns_presence;
    int32_t fuelEnCns;
    bool  fuelLow1WarningDriver_presence;
    int32_t fuelLow1WarningDriver;
    bool  fuelLow2WarningDriver_presence;
    int32_t fuelLow2WarningDriver;
    //-- km;
    bool  distanceToEmpty_presence;
    int32_t distanceToEmpty;
    //-- m;
    bool  trvlDistance_presence;
    int32_t trvlDistance;
    bool  aveFuelConsumption_presence;
    int32_t aveFuelConsumption;
    bool  aveFuelConsumptionInLatestDrivingCycle_presence;
    int32_t aveFuelConsumptionInLatestDrivingCycle;
    bool  avgSpeed_presence;
    int32_t avgSpeed;
    bool  avgSpeedLatestDrivingCycle_presence;
    int32_t avgSpeedLatestDrivingCycle;
    //--Scope:PrintableString SIZE 1..10;
    bool  aveFuelConsumptionUnit_presence;
    std::string aveFuelConsumptionUnit;
    bool  indFuelConsumption_presence;
    int32_t indFuelConsumption;
    bool  notifForEmgyCallStatus_presence;
    int32_t notifForEmgyCallStatus;
    bool  tyrePreWarningDriver_presence;
    int32_t tyrePreWarningDriver;
    bool  tyrePreWarningPassenger_presence;
    int32_t tyrePreWarningPassenger;
    bool  tyrePreWarningDriverRear_presence;
    int32_t tyrePreWarningDriverRear;
    bool  tyrePreWarningPassengerRear_presence;
    int32_t tyrePreWarningPassengerRear;
    bool  tyreTempWarningDriver_presence;
    int32_t tyreTempWarningDriver;
    bool  tyreTempWarningPassenger_presence;
    int32_t tyreTempWarningPassenger;
    bool  tyreTempWarningDriverRear_presence;
    int32_t tyreTempWarningDriverRear;
    bool  tyreTempWarningPassengerRear_presence;
    int32_t tyreTempWarningPassengerRear;
    //-- ture = fastened;
    bool  seatBeltStatusDriver_presence;
    bool seatBeltStatusDriver;
    bool  seatBeltStatusPassenger_presence;
    bool seatBeltStatusPassenger;
    bool  seatBeltStatusDriverRear_presence;
    bool seatBeltStatusDriverRear;
    bool  seatBeltStatusPassengerRear_presence;
    bool seatBeltStatusPassengerRear;
    bool  seatBeltStatusMidRear_presence;
    bool seatBeltStatusMidRear;
    bool  seatBeltStatusThDriverRear_presence;
    bool seatBeltStatusThDriverRear;
    bool  seatBeltStatusThPassengerRear_presence;
    bool seatBeltStatusThPassengerRear;
    bool  seatBeltStatusThMidRear_presence;
    bool seatBeltStatusThMidRear;
    bool  brakePedalDepressed_presence;
    bool brakePedalDepressed;
    bool  gearManualStatus_presence;
    int32_t gearManualStatus;
    bool  gearAutoStatus_presence;
    int32_t gearAutoStatus;
    bool  engineSpeed_presence;
    int32_t engineSpeed;
    bool  transimissionGearPostion_presence;
    int32_t transimissionGearPostion;
    bool  cruiseControlStatus_presence;
    int32_t cruiseControlStatus;
    bool  engineBlockedStatus_presence;
    Vds_EngineBlockedStatus engineBlockedStatus;
    //-- KM;
    bool  tripMeter1_presence;
    int32_t tripMeter1;
    //-- KM;
    bool  tripMeter2_presence;
    int32_t tripMeter2;

};

class ClimateStatus_Model: public VdsDataBase
{
public:
    ClimateStatus_Model():
    interiorTemp_presence(),
    interiorTemp(),
    exteriorTemp_presence(),
    exteriorTemp(),
    preClimateActive_presence(),
    preClimateActive(),
    airCleanSts_presence(),
    airCleanSts(),
    drvHeatSts_presence(),
    drvHeatSts(),
    passHeatingSts_presence(),
    passHeatingSts(),
    rlHeatingSts_presence(),
    rlHeatingSts(),
    rrHeatingSts_presence(),
    rrHeatingSts(),
    drvVentSts_presence(),
    drvVentSts(),
    passVentSts_presence(),
    passVentSts(),
    rrVentSts_presence(),
    rrVentSts(),
    rlVentSts_presence(),
    rlVentSts(),
    interCO2Warning_presence(),
    interCO2Warning(),
    fragStrs_presence(),
    fragStrs()
    {
    }
    //-- degree celcius;
    bool  interiorTemp_presence;
    int32_t interiorTemp;
    //-- degree celcius;
    bool  exteriorTemp_presence;
    int32_t exteriorTemp;
    //--for ON/Off explaining if the PreClimate is running or not (ClimaActv);
    bool  preClimateActive_presence;
    bool preClimateActive;
    //-- air conditioner clean status Scope:INTEGER 0..1;
    bool  airCleanSts_presence;
    uint8_t airCleanSts;
    //-- driver seat heating status Scope:INTEGER 0..7;
    bool  drvHeatSts_presence;
    uint8_t drvHeatSts;
    //-- front passenger seat heating status Scope:INTEGER 0..7;
    bool  passHeatingSts_presence;
    uint8_t passHeatingSts;
    //-- left rear seat heating status Scope:INTEGER 0..7;
    bool  rlHeatingSts_presence;
    uint8_t rlHeatingSts;
    //-- right rear seat heating status Scope:INTEGER 0..7;
    bool  rrHeatingSts_presence;
    uint8_t rrHeatingSts;
    //-- driver seat ventilation status Scope:INTEGER 0..7;
    bool  drvVentSts_presence;
    uint8_t drvVentSts;
    //-- front passenger seat ventilation status Scope:INTEGER 0..7;
    bool  passVentSts_presence;
    uint8_t passVentSts;
    //-- right rear seat ventilation status Scope:INTEGER 0..7;
    bool  rrVentSts_presence;
    uint8_t rrVentSts;
    //-- left rear seat ventilation status Scope:INTEGER 0..7;
    bool  rlVentSts_presence;
    uint8_t rlVentSts;
    //--Scope:INTEGER 0..7;
    bool  interCO2Warning_presence;
    uint8_t interCO2Warning;
    //--fragrance status;
    bool  fragStrs_presence;
    FragStrs_Model fragStrs;

};

class PollutionStatus_Model: public VdsDataBase
{
public:
    PollutionStatus_Model():
    interiorPM25_presence(),
    interiorPM25(),
    exteriorPM25_presence(),
    exteriorPM25(),
    interiorPM25Level_presence(),
    interiorPM25Level(),
    exteriorPM25Level_presence(),
    exteriorPM25Level(),
    airQualityIndex_presence(),
    airQualityIndex(),
    airParticleConcentration_presence(),
    airParticleConcentration()
    {
    }
    //--Scope:INTEGER -1..2048;
    bool  interiorPM25_presence;
    uint64_t interiorPM25;
    //--Scope:INTEGER -1..2048;
    bool  exteriorPM25_presence;
    uint64_t exteriorPM25;
    //--Scope:INTEGER 0..20;
    bool  interiorPM25Level_presence;
    uint8_t interiorPM25Level;
    //--Scope:INTEGER 0..20;
    bool  exteriorPM25Level_presence;
    uint8_t exteriorPM25Level;
    //--Scope:INTEGER 0..1023;
    bool  airQualityIndex_presence;
    uint16_t airQualityIndex;
    //--Scope:INTEGER 0..1023;
    bool  airParticleConcentration_presence;
    uint16_t airParticleConcentration;

};

class ElectricStatus_Model: public VdsDataBase
{
public:
    ElectricStatus_Model():
    isCharging_presence(),
    isCharging(),
    isPluggedIn_presence(),
    isPluggedIn(),
    stateOfCharge_presence(),
    stateOfCharge(),
    chargeLevel_presence(),
    chargeLevel(),
    timeToFullyCharged_presence(),
    timeToFullyCharged(),
    statusOfChargerConnection_presence(),
    statusOfChargerConnection(),
    chargerState_presence(),
    chargerState(),
    distanceToEmptyOnBatteryOnly_presence(),
    distanceToEmptyOnBatteryOnly(),
    ems48VSOC_presence(),
    ems48VSOC(),
    ems48VDisSOC_presence(),
    ems48VDisSOC(),
    emsHVRBSError_presence(),
    emsHVRBSError(),
    emsRBSModeDisplay_presence(),
    emsRBSModeDisplay(),
    emsOVPActive_presence(),
    emsOVPActive(),
    ems48vPackTemp1_presence(),
    ems48vPackTemp1(),
    ems48vPackTemp2_presence(),
    ems48vPackTemp2(),
    emsBMSLBuildSwVersion_presence(),
    emsBMSLBuildSwVersion(),
    emsCBRemainingLife(),
    chargeHvSts_presence(),
    chargeHvSts(),
    ptReady_presence(),
    ptReady(),
    averPowerConsumption_presence(),
    averPowerConsumption(),
    indPowerConsumption_presence(),
    indPowerConsumption(),
    dcDcActvd_presence(),
    dcDcActvd(),
    dcDcConnectStatus_presence(),
    dcDcConnectStatus(),
    dcChargeIAct_presence(),
    dcChargeIAct(),
    dcChargeSts_presence(),
    dcChargeSts(),
    wptObjt_presence(),
    wptObjt(),
    wptFineAlignt_presence(),
    wptFineAlignt(),
    wptActived_presence(),
    wptActived(),
    wptChargeIAct_presence(),
    wptChargeIAct(),
    wptChargeUAct_presence(),
    wptChargeUAct(),
    wptChargeSts_presence(),
    wptChargeSts(),
    chargeIAct_presence(),
    chargeIAct(),
    chargeUAct_presence(),
    chargeUAct(),
    chargeSts_presence(),
    chargeSts(),
    disChargeConnectStatus_presence(),
    disChargeConnectStatus(),
    disChargeIAct_presence(),
    disChargeIAct(),
    disChargeUAct_presence(),
    disChargeUAct(),
    disChargeSts_presence(),
    disChargeSts()
    {
    }
    bool  isCharging_presence;
    bool isCharging;
    bool  isPluggedIn_presence;
    bool isPluggedIn;
    //-- status, e.g. empty, low, normal, high, etc. Scope:INTEGER 0..1000;
    bool  stateOfCharge_presence;
    uint16_t stateOfCharge;
    //-- the charge percentage Scope:INTEGER 0..1000;
    bool  chargeLevel_presence;
    uint16_t chargeLevel;
    //-- unit is defined according vehicle model Scope:INTEGER 0..2047;
    bool  timeToFullyCharged_presence;
    uint16_t timeToFullyCharged;
    //-- Disconnected, ConnectedWithoutPower etc. Scope:INTEGER 0..10;
    bool  statusOfChargerConnection_presence;
    uint8_t statusOfChargerConnection;
    //-- idle, PreStrt etc. Scope:INTEGER 0..10;
    bool  chargerState_presence;
    uint8_t chargerState;
    //--Scope:INTEGER 0..1023;
    bool  distanceToEmptyOnBatteryOnly_presence;
    uint16_t distanceToEmptyOnBatteryOnly;
    //--Scope:INTEGER 0..2000;
    bool  ems48VSOC_presence;
    uint16_t ems48VSOC;
    //--Scope:INTEGER 0..2000;
    bool  ems48VDisSOC_presence;
    uint16_t ems48VDisSOC;
    //--Scope:INTEGER 0..1;
    bool  emsHVRBSError_presence;
    uint8_t emsHVRBSError;
    //--Scope:INTEGER 0..7;
    bool  emsRBSModeDisplay_presence;
    uint8_t emsRBSModeDisplay;
    //--Scope:INTEGER 0..1;
    bool  emsOVPActive_presence;
    uint8_t emsOVPActive;
    //--Scope:INTEGER 0..255;
    bool  ems48vPackTemp1_presence;
    uint8_t ems48vPackTemp1;
    //--Scope:INTEGER 0..255;
    bool  ems48vPackTemp2_presence;
    uint8_t ems48vPackTemp2;
    //--Scope:PrintableString SIZE 1..255;
    bool  emsBMSLBuildSwVersion_presence;
    std::string emsBMSLBuildSwVersion;
    //--Scope:INTEGER 0..100;
    uint8_t emsCBRemainingLife;
    //-- 0x0: Undefined, 0x1: Failed, 0x2: Successful, 0x3: Reserved Scope:INTEGER 0..3;
    bool  chargeHvSts_presence;
    uint8_t chargeHvSts;
    //-- 0x0: PTReady Not Active, 0x1: PTReady Active Scope:INTEGER 0..3;
    bool  ptReady_presence;
    uint8_t ptReady;
    //--Scope:INTEGER 0..5000;
    bool  averPowerConsumption_presence;
    uint16_t averPowerConsumption;
    //--Scope:INTEGER 0..5000;
    bool  indPowerConsumption_presence;
    uint16_t indPowerConsumption;
    //-- 0x0:Not active ,0x1:active ,0x2:error,0x3:undefine Scope:INTEGER 0..10;
    bool  dcDcActvd_presence;
    uint8_t dcDcActvd;
    //--Scope:INTEGER 0..20;
    bool  dcDcConnectStatus_presence;
    uint8_t dcDcConnectStatus;
    //--Scope:INTEGER 0..65535;
    bool  dcChargeIAct_presence;
    uint16_t dcChargeIAct;
    //--Scope:INTEGER 0..20;
    bool  dcChargeSts_presence;
    uint8_t dcChargeSts;
    //--Scope:INTEGER 0..1;
    bool  wptObjt_presence;
    uint8_t wptObjt;
    //--Scope:INTEGER 0..1;
    bool  wptFineAlignt_presence;
    uint8_t wptFineAlignt;
    //--Scope:INTEGER 0..1;
    bool  wptActived_presence;
    uint8_t wptActived;
    //--Scope:INTEGER 0..5000;
    bool  wptChargeIAct_presence;
    uint16_t wptChargeIAct;
    //--Scope:INTEGER 0..5000;
    bool  wptChargeUAct_presence;
    uint16_t wptChargeUAct;
    //--Scope:INTEGER 0..20;
    bool  wptChargeSts_presence;
    uint8_t wptChargeSts;
    //--Scope:INTEGER 0..5000;
    bool  chargeIAct_presence;
    uint16_t chargeIAct;
    //--Scope:INTEGER 0..5000;
    bool  chargeUAct_presence;
    uint16_t chargeUAct;
    //--Scope:INTEGER 0..20;
    bool  chargeSts_presence;
    uint8_t chargeSts;
    //--Scope:INTEGER 0..20;
    bool  disChargeConnectStatus_presence;
    uint8_t disChargeConnectStatus;
    //--Scope:INTEGER 0..5000;
    bool  disChargeIAct_presence;
    uint16_t disChargeIAct;
    //--Scope:INTEGER 0..5000;
    bool  disChargeUAct_presence;
    uint16_t disChargeUAct;
    //--Scope:INTEGER 0..20;
    bool  disChargeSts_presence;
    uint8_t disChargeSts;

};

class TEMStatus_Model: public VdsDataBase
{
public:
    TEMStatus_Model():
    vin_presence(),
    vin(),
    powerSource(),
    powerMode(),
    state_presence(),
    state(),
    serviceProvisoned_presence(),
    serviceProvisoned(),
    healthStatus_presence(),
    healthStatus(),
    diagnostics_presence(),
    diagnostics(),
    backupBattery_presence(),
    backupBattery(),
    sleepCycleNextWakeupTime_presence(),
    sleepCycleNextWakeupTime(),
    serialNumber_presence(),
    serialNumber(),
    completeECUPartNumbers_presence(),
    completeECUPartNumbers(),
    networkAccessStatus_presence(),
    networkAccessStatus(),
    imei_presence(),
    imei(),
    connectivityStatus_presence(),
    connectivityStatus(),
    hwVersion_presence(),
    hwVersion(),
    swVersion_presence(),
    swVersion(),
    mpuVersion_presence(),
    mpuVersion(),
    mcuVersion_presence(),
    mcuVersion()
    {
    }
    bool  vin_presence;
    std::string vin;
    Vds_PowerSource powerSource;
    Vds_TEMPowerMode powerMode;
    bool  state_presence;
    Vds_TEMState state;
    //-- refer to the TEM provision, to sync the status between TEM and CSP ;
    bool  serviceProvisoned_presence;
    bool serviceProvisoned;
    bool  healthStatus_presence;
    Vds_HealthStatus healthStatus;
    //-- for TEM DTC codes;
    bool  diagnostics_presence;
    ECUDiagnostic_Model diagnostics;
    bool  backupBattery_presence;
    BatteryStatus_Model backupBattery;
    //-- Thursday, 1 January 1970 means no information;
    bool  sleepCycleNextWakeupTime_presence;
    TimeStamp_Model sleepCycleNextWakeupTime;
    bool  serialNumber_presence;
    std::string serialNumber;
    bool  completeECUPartNumbers_presence;
    std::string completeECUPartNumbers;
    //-- the network access information;
    bool  networkAccessStatus_presence;
    NetworkAccessStatus_Model networkAccessStatus;
    bool  imei_presence;
    std::string imei;
    bool  connectivityStatus_presence;
    ConnectivityStatus_Model connectivityStatus;
    bool  hwVersion_presence;
    std::string hwVersion;
    bool  swVersion_presence;
    std::string swVersion;
    bool  mpuVersion_presence;
    std::string mpuVersion;
    bool  mcuVersion_presence;
    std::string mcuVersion;

};

class CarConfig_Model: public VdsDataBase
{
public:
    CarConfig_Model():
    heatedFrontSeats_presence(),
    heatedFrontSeats(),
    parkingClimateLevel_presence(),
    parkingClimateLevel(),
    heatedSteeringWheel_presence(),
    heatedSteeringWheel(),
    items_presence(),
    items()
    {
    }
    //--  Heated front seats;
    bool  heatedFrontSeats_presence;
    int32_t heatedFrontSeats;
    //-- Parking Climate level;
    bool  parkingClimateLevel_presence;
    int32_t parkingClimateLevel;
    //-- Heated steering wheel;
    bool  heatedSteeringWheel_presence;
    int32_t heatedSteeringWheel;
    bool  items_presence;
    std::vector<ServiceParameter_Model> items;

};

class AdditionalStatus_Model: public VdsDataBase
{
public:
    AdditionalStatus_Model():
    confAndId_presence(),
    confAndId(),
    ecuWarningMessages_presence(),
    ecuWarningMessages(),
    towStatus_presence(),
    towStatus(),
    signals_presence(),
    signals(),
    data_presence(),
    data()
    {
    }
    bool  confAndId_presence;
    ConfigurationAndIdentity_Model confAndId;
    bool  ecuWarningMessages_presence;
    std::vector<GenericVehicleSignal_Model> ecuWarningMessages;
    bool  towStatus_presence;
    TowStatus_Model towStatus;
    bool  signals_presence;
    std::vector<GenericVehicleSignal_Model> signals;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  data_presence;
    std::string data;

};

class OEMMSD_Model: public VdsDataBase
{
public:
    OEMMSD_Model():
    automaticActivation(),
    testCall(),
    position(),
    vehicleType_presence(),
    vehicleType(),
    vin_presence(),
    vin(),
    fuelType(),
    direction(),
    numberOfPassengers_presence(),
    numberOfPassengers(),
    recentPositions_presence(),
    recentPositions(),
    speed_presence(),
    speed(),
    speedValidity_presence(),
    speedValidity(),
    signals_presence(),
    signals()
    {
    }
    //-- Activation type: true - automatic call, false - manual call;
    bool automaticActivation;
    //-- Call type: true - test call, false emergency call;
    bool testCall;
    Position_Model position;
    //-- vehicle type, e.g. passenger vehicle, bus, cargo vehicle, motor cycle, etc, in  CSP this is passenger vehicle as always;
    bool  vehicleType_presence;
    Vds_VehicleType vehicleType;
    bool  vin_presence;
    std::string vin;
    Vds_FuelType fuelType;
    //-- 0 means North, clockwise Scope:INTEGER 0..360;
    uint16_t direction;
    //--Scope:INTEGER 0..64;
    bool  numberOfPassengers_presence;
    uint8_t numberOfPassengers;
    bool  recentPositions_presence;
    std::vector<Position_Model> recentPositions;
    //-- kmph;
    bool  speed_presence;
    int32_t speed;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  speedValidity_presence;
    uint8_t speedValidity;
    //-- The optional additional data;
    bool  signals_presence;
    std::vector<GenericVehicleSignal_Model> signals;

};

class TheftNotification_Model: public VdsDataBase
{
public:
    TheftNotification_Model():
    time(),
    activated()
    {
    }
    TimeStamp_Model time;
    //-- To indicate the reason of the theft notification trigger, e.g. doors/trunk/hood open, tow away;
    int32_t activated;

};

class RVDC_Model: public VdsDataBase
{
public:
    RVDC_Model():
    appId(),
    maOrderId(),
    timestamp(),
    vin(),
    maBusinessType(),
    rvdcSetting_presence(),
    rvdcSetting(),
    assignmentNotification_presence(),
    assignmentNotification(),
    maSynchronizationReq_presence(),
    maSynchronizationReq(),
    maSynchronization_presence(),
    maSynchronization(),
    exceptionHandling_presence(),
    exceptionHandling(),
    measurementDataPackage_presence(),
    measurementDataPackage()
    {
    }
    //-- Which platform to use OTA Server;
    Vds_AppId appId;
    //-- The orderId is generated by the command initiator, and the responder needs to response it to the initiator.	;
    std::string maOrderId;
    //-- The client recorded time of the notification Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //-- Vehicle identification code  Scope:PrintableString SIZE 1..128;
    std::string vin;
    //-- the business type of the current operation;
    Vds_MaBusinessType maBusinessType;
    bool  rvdcSetting_presence;
    RvdcSetting_Model rvdcSetting;
    bool  assignmentNotification_presence;
    RvdcAssignmentNotification_Model assignmentNotification;
    bool  maSynchronizationReq_presence;
    MaSynchronizationReq_Model maSynchronizationReq;
    //-- Support vehicle request to cloud for response and cloud to vehicle for push;
    bool  maSynchronization_presence;
    MaSynchronization_Model maSynchronization;
    bool  exceptionHandling_presence;
    RvdcExceptionHandling_Model exceptionHandling;
    bool  measurementDataPackage_presence;
    MeasurementDataPackage_Model measurementDataPackage;

};

class FOTA_Model: public VdsDataBase
{
public:
    FOTA_Model():
    appId(),
    vin(),
    dataType(),
    clientConfSyncRequest_presence(),
    clientConfSyncRequest(),
    inventoryInstruction_presence(),
    inventoryInstruction(),
    availableAssignmentRequest_presence(),
    availableAssignmentRequest(),
    availableAssignment_presence(),
    availableAssignment(),
    installationInstruction_presence(),
    installationInstruction(),
    installationSummary_presence(),
    installationSummary(),
    ecuExceptionReport_presence(),
    ecuExceptionReport(),
    clientHmiVehicleSettings_presence(),
    clientHmiVehicleSettings(),
    assignmentData_presence(),
    assignmentData(),
    assignmentNotification_presence(),
    assignmentNotification(),
    otaAssignmentFileInfo_presence(),
    otaAssignmentFileInfo()
    {
    }
    //-- Which platform to use OTA Server;
    Vds_AppId appId;
    //-- Vehicle identification code  Scope:PrintableString SIZE 1..128;
    std::string vin;
    Vds_DataType dataType;
    //-- provide Client Configuration to OTA server.;
    bool  clientConfSyncRequest_presence;
    ClientConfSyncRequest_Model clientConfSyncRequest;
    //-- The objective of this interface is to provide Inventory Instruction to vehicle;
    bool  inventoryInstruction_presence;
    InventoryInstruction_Model inventoryInstruction;
    //-- The objective of this interface is to request available assignment from OTA server.;
    bool  availableAssignmentRequest_presence;
    AvailableAssignmentRequest_Model availableAssignmentRequest;
    //-- The objective of this interface is to provide available assignment to vehicle.;
    bool  availableAssignment_presence;
    AvailableAssignment_Model availableAssignment;
    //-- The objective of this interface is to provide Installation Instructions to vehicle;
    bool  installationInstruction_presence;
    InstallationInstruction_Model installationInstruction;
    //-- The objective of this interface is to provide Installation Summary to OTA server.;
    bool  installationSummary_presence;
    InstallationSummary_Model installationSummary;
    //-- The objective of this interface is to provide Exception Reports to OTA server;
    bool  ecuExceptionReport_presence;
    EcuExceptionReport_Model ecuExceptionReport;
    //-- Due to fleet management and administrative reasons, the backend may lock certain vehicle settings in order to force a setting value for specific vehicles;
    bool  clientHmiVehicleSettings_presence;
    ClientHmiVehicleSettings_Model clientHmiVehicleSettings;
    //-- The objective of this interface is to provide assignment data to vehicle	;
    bool  assignmentData_presence;
    AssignmentData_Model assignmentData;
    bool  assignmentNotification_presence;
    FotaAssignmentNotification_Model assignmentNotification;
    bool  otaAssignmentFileInfo_presence;
    OtaAssignmentFileInfo_Model otaAssignmentFileInfo;

};

class NEVServiceData_Model: public VdsDataBase
{
public:
    NEVServiceData_Model():
    vehicleVin(),
    testVehicle(),
    rvdcTriggerCondition(),
    mdpID(),
    maID(),
    position(),
    ecuData()
    {
    }
    //--Scope:PrintableString SIZE 17;
    std::string vehicleVin;
    //-- TRUE=test vehicle, FALSE=customer vehicle;
    bool testVehicle;
    //-- periodicTrg (1) UM Driving (periodic MDP), umChangeTrg (2) UM Driving -> UM != Driving (1 MDP), chargeStartTrg (3) Charging start (1 MDP), chargeStopTrg (4) Charging Stop (1 MDP), disableRmsTrg (5) Rms disabled;
    Vds_NEVTriggerCondition rvdcTriggerCondition;
    //--Increase by one by each measurement cycle. Starting from 0.  Scope:INTEGER 0..4294967295;
    uint32_t mdpID;
    //--Partnumber for MA file Scope:PrintableString SIZE 0..20;
    std::string maID;
    //--GPS Data;
    NEVDRVehiclePosition_Model position;
    //-- ECU data or if the MDP has no diagnostic data included ;
    EcuData_Model ecuData;

};

class ServiceTrigger_Model: public VdsDataBase
{
public:
    ServiceTrigger_Model():
    scheduledTime_presence(),
    scheduledTime(),
    recurrentOperation_presence(),
    recurrentOperation(),
    duration_presence(),
    duration(),
    interval_presence(),
    interval(),
    occurs_presence(),
    occurs(),
    endTime_presence(),
    endTime(),
    startTimeofDay_presence(),
    startTimeofDay()
    {
    }
    bool  scheduledTime_presence;
    TimeStamp_Model scheduledTime;
    bool  recurrentOperation_presence;
    bool recurrentOperation;
    //--Scope:INTEGER 0..3153600;
    bool  duration_presence;
    uint32_t duration;
    //--Scope:INTEGER 0..86400;
    bool  interval_presence;
    uint32_t interval;
    //--Scope:INTEGER -1..65534;
    bool  occurs_presence;
    uint64_t occurs;
    bool  endTime_presence;
    TimeStamp_Model endTime;
    //-- minutes of day Scope:INTEGER 0..86400;
    bool  startTimeofDay_presence;
    uint32_t startTimeofDay;

};

class ServiceResult_Model: public VdsDataBase
{
public:
    ServiceResult_Model():
    operationSucceeded(),
    error_presence(),
    error(),
    operationType_presence(),
    operationType()
    {
    }
    bool operationSucceeded;
    bool  error_presence;
    Error_Model error;
    bool  operationType_presence;
    std::string operationType;

};

class ServiceStatus_Model: public VdsDataBase
{
public:
    ServiceStatus_Model():
    remoteControlInhibited()
    {
    }
    bool remoteControlInhibited;

};

class OTA_Model: public VdsDataBase
{
public:
    OTA_Model():
    swModel_presence(),
    swModel(),
    swVersion_presence(),
    swVersion(),
    downloadUrl(),
    verificationCode_presence(),
    verificationCode()
    {
    }
    bool  swModel_presence;
    std::string swModel;
    //-- 0 means no version information;
    bool  swVersion_presence;
    std::string swVersion;
    std::string downloadUrl;
    bool  verificationCode_presence;
    std::string verificationCode;

};

class Configuration_Model: public VdsDataBase
{
public:
    Configuration_Model():
    version(),
    items()
    {
    }
    std::string version;
    std::vector<ConfigurationItem_Model> items;

};

class GeneralMessage_Model: public VdsDataBase
{
public:
    GeneralMessage_Model():
    message_choice(),
    message(),
    ocetMessage_choice(),
    ocetMessage()
    {
    }
    //--Scope:UTF8String SIZE 0..65535;
    bool  message_choice;
    std::string message;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  ocetMessage_choice;
    std::string ocetMessage;

};

class Trip_Model: public VdsDataBase
{
public:
    Trip_Model():
    tripId(),
    startTime(),
    startOdometer_presence(),
    startOdometer(),
    fuelConsumption_presence(),
    fuelConsumption(),
    traveledDistance(),
    trackpoints(),
    waypoints_presence(),
    waypoints(),
    avgSpeed(),
    endTime(),
    endOdometer_presence(),
    endOdometer(),
    electricConsumption_presence(),
    electricConsumption(),
    electricRegeneration_presence(),
    electricRegeneration()
    {
    }
    int32_t tripId;
    TimeStamp_Model startTime;
    bool  startOdometer_presence;
    int32_t startOdometer;
    bool  fuelConsumption_presence;
    int32_t fuelConsumption;
    //-- km;
    int32_t traveledDistance;
    std::vector<TrackPoint_Model> trackpoints;
    bool  waypoints_presence;
    std::vector<Position_Model> waypoints;
    int32_t avgSpeed;
    TimeStamp_Model endTime;
    bool  endOdometer_presence;
    int32_t endOdometer;
    bool  electricConsumption_presence;
    int32_t electricConsumption;
    bool  electricRegeneration_presence;
    int32_t electricRegeneration;

};

class TripFragment_Model: public VdsDataBase
{
public:
    TripFragment_Model():
    startTime(),
    startPosition_presence(),
    startPosition(),
    startOdometer_presence(),
    startOdometer(),
    startFuelLevel_presence(),
    startFuelLevel(),
    startElectricConsumption_presence(),
    startElectricConsumption(),
    startElectricRegeneration_presence(),
    startElectricRegeneration(),
    endTime_presence(),
    endTime(),
    endPosition_presence(),
    endPosition(),
    endOdometer_presence(),
    endOdometer(),
    endFuelLevel_presence(),
    endFuelLevel(),
    endElectricConsumption_presence(),
    endElectricConsumption(),
    endElectricRegeneration_presence(),
    endElectricRegeneration()
    {
    }
    TimeStamp_Model startTime;
    bool  startPosition_presence;
    Position_Model startPosition;
    bool  startOdometer_presence;
    int32_t startOdometer;
    bool  startFuelLevel_presence;
    int32_t startFuelLevel;
    bool  startElectricConsumption_presence;
    int32_t startElectricConsumption;
    bool  startElectricRegeneration_presence;
    int32_t startElectricRegeneration;
    bool  endTime_presence;
    TimeStamp_Model endTime;
    bool  endPosition_presence;
    Position_Model endPosition;
    bool  endOdometer_presence;
    int32_t endOdometer;
    bool  endFuelLevel_presence;
    int32_t endFuelLevel;
    bool  endElectricConsumption_presence;
    int32_t endElectricConsumption;
    bool  endElectricRegeneration_presence;
    int32_t endElectricRegeneration;

};

class PointOfInterest_Model: public VdsDataBase
{
public:
    PointOfInterest_Model():
    name(),
    position_presence(),
    position(),
    type_presence(),
    type(),
    contactInfo_presence(),
    contactInfo(),
    url_presence(),
    url(),
    attributes_presence(),
    attributes()
    {
    }
    //--Scope:UTF8String SIZE 0..100;
    std::string name;
    bool  position_presence;
    Position_Model position;
    //--Scope:UTF8String SIZE 0..255;
    bool  type_presence;
    std::string type;
    bool  contactInfo_presence;
    ContactInfo_Model contactInfo;
    //--Scope:UTF8String SIZE 0..255;
    bool  url_presence;
    std::string url;
    //--Scope:SEQUENCE SIZE 0..15 OF ServiceParameter;
    bool  attributes_presence;
    std::vector<ServiceParameter_Model> attributes;

};

class EventRule_Model: public VdsDataBase
{
public:
    EventRule_Model():
    ruleId(),
    startTime_presence(),
    startTime(),
    endTime_presence(),
    endTime(),
    conditions_presence(),
    conditions(),
    interval_presence(),
    interval(),
    action_presence(),
    action()
    {
    }
    //-- Rule identity;
    int32_t ruleId;
    //-- Rule should be effective since this time in TEM;
    bool  startTime_presence;
    TimeStamp_Model startTime;
    //-- Rule should not be effective after this time in TEM;
    bool  endTime_presence;
    TimeStamp_Model endTime;
    bool  conditions_presence;
    std::vector<RuleCondition_Model> conditions;
    //-- Unit is second;
    bool  interval_presence;
    int32_t interval;
    bool  action_presence;
    EventAction_Model action;

};

class TimerData_Model: public VdsDataBase
{
public:
    TimerData_Model():
    timerId(),
    timers_presence(),
    timers()
    {
    }
    //--Scope:INTEGER 0..1024;
    uint16_t timerId;
    bool  timers_presence;
    std::vector<TimerInfo_Model> timers;

};

class VehicleStatus_Model: public VdsDataBase
{
public:
    VehicleStatus_Model():
    basicVehicleStatus_presence(),
    basicVehicleStatus(),
    lockStatus_presence(),
    lockStatus(),
    maintenanceStatus_presence(),
    maintenanceStatus(),
    runningStatus_presence(),
    runningStatus(),
    climateStatus_presence(),
    climateStatus(),
    electricStatus_presence(),
    electricStatus(),
    pollutionStatus_presence(),
    pollutionStatus(),
    temStatus_presence(),
    temStatus(),
    carConfig_presence(),
    carConfig(),
    additionalStatus_presence(),
    additionalStatus(),
    msd_presence(),
    msd(),
    diagnostics_presence(),
    diagnostics(),
    theftNotification_presence(),
    theftNotification(),
    notification_presence(),
    notification()
    {
    }
    //-- the basic vehicle status;
    bool  basicVehicleStatus_presence;
    BasicVehicleStatus_Model basicVehicleStatus;
    //-- the vehicle lock status;
    bool  lockStatus_presence;
    LockStatus_Model lockStatus;
    //-- the Vehicle maintenance status;
    bool  maintenanceStatus_presence;
    MaintenanceStatus_Model maintenanceStatus;
    //-- the Vehicle Running status;
    bool  runningStatus_presence;
    RunningStatus_Model runningStatus;
    //-- the Vehicle climate status;
    bool  climateStatus_presence;
    ClimateStatus_Model climateStatus;
    bool  electricStatus_presence;
    ElectricStatus_Model electricStatus;
    bool  pollutionStatus_presence;
    PollutionStatus_Model pollutionStatus;
    //-- the TEM status;
    bool  temStatus_presence;
    TEMStatus_Model temStatus;
    bool  carConfig_presence;
    CarConfig_Model carConfig;
    bool  additionalStatus_presence;
    AdditionalStatus_Model additionalStatus;
    //-- Minimal Set of Data;
    bool  msd_presence;
    OEMMSD_Model msd;
    //-- Diagnostic Information for ECU;
    bool  diagnostics_presence;
    std::vector<ECUDiagnostic_Model> diagnostics;
    //-- the theft notification;
    bool  theftNotification_presence;
    TheftNotification_Model theftNotification;
    //-- generic notification, e.g., vehicle unlock notification;
    bool  notification_presence;
    Notification_Model notification;

};

class ServiceData_Model: public VdsDataBase
{
public:
    ServiceData_Model():
    serviceTrigger_presence(),
    serviceTrigger(),
    serviceParameters_presence(),
    serviceParameters(),
    serviceCommand_presence(),
    serviceCommand(),
    serviceResult_presence(),
    serviceResult(),
    serviceStatus_presence(),
    serviceStatus(),
    message_presence(),
    message(),
    pois_presence(),
    pois(),
    configuration_presence(),
    configuration(),
    vehicleStatus_presence(),
    vehicleStatus(),
    ota_presence(),
    ota(),
    fota_presence(),
    fota(),
    rvdc_presence(),
    rvdc(),
    trip_presence(),
    trip(),
    tripFragments_presence(),
    tripFragments(),
    trackPoints_presence(),
    trackPoints(),
    nevServiceData_presence(),
    nevServiceData(),
    eventRule_presence(),
    eventRule(),
    timerData_presence(),
    timerData()
    {
    }
    bool  serviceTrigger_presence;
    ServiceTrigger_Model serviceTrigger;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Model> serviceParameters;
    bool  serviceCommand_presence;
    Vds_ServiceCommand serviceCommand;
    bool  serviceResult_presence;
    ServiceResult_Model serviceResult;
    bool  serviceStatus_presence;
    ServiceStatus_Model serviceStatus;
    //-- The general message used to push notificaton;
    bool  message_presence;
    GeneralMessage_Model message;
    bool  pois_presence;
    std::vector<PointOfInterest_Model> pois;
    bool  configuration_presence;
    Configuration_Model configuration;
    //-- defined in specific device status ASN.1 file);
    bool  vehicleStatus_presence;
    VehicleStatus_Model vehicleStatus;
    bool  ota_presence;
    OTA_Model ota;
    //-- FOTA upgrade;
    bool  fota_presence;
    FOTA_Model fota;
    //-- GEEA2.0 RVDC;
    bool  rvdc_presence;
    RVDC_Model rvdc;
    bool  trip_presence;
    Trip_Model trip;
    bool  tripFragments_presence;
    std::vector<TripFragment_Model> tripFragments;
    bool  trackPoints_presence;
    std::vector<TrackPoint_Model> trackPoints;
    bool  nevServiceData_presence;
    NEVServiceData_Model nevServiceData;
    bool  eventRule_presence;
    EventRule_Model eventRule;
    bool  timerData_presence;
    TimerData_Model timerData;

};

class RequestHeader_Model: public VdsDataBase
{
public:
    RequestHeader_Model():
    requestid(),
    timeStamp(),
    eventId_presence(),
    eventId(),
    creatorId_presence(),
    creatorId(),
    messageTTL_presence(),
    messageTTL(),
    requestType_presence(),
    requestType(),
    ackRequired_presence(),
    ackRequired()
    {
    }
    //-- An identifier of this request, must be unique to the sending party. If this request is an ACK, this is the identifier of the request being acknowledged Scope:RequestId;
    uint8_t requestid;
    //-- The time when this message was created;
    TimeStamp_Model timeStamp;
    //-- Identifier of the service event that is being executed, the value is the time when the event is created;
    bool  eventId_presence;
    TimeStamp_Model eventId;
    //-- Identifies if this event was created by a TEM unit or CVC back-end;
    bool  creatorId_presence;
    Vds_CreatorId creatorId;
    //-- Message time-to-live after which this request message should be discarded, no present means no limit Scope:MessageTTL;
    bool  messageTTL_presence;
    uint32_t messageTTL;
    //-- Identifies the type of message, no presents means serviceRequest;
    bool  requestType_presence;
    Vds_RequestType requestType;
    //-- If this request needs to be acknowledged, no present means false;
    bool  ackRequired_presence;
    bool ackRequired;

};

class RequestBody_Model: public VdsDataBase
{
public:
    RequestBody_Model():
    serviceId_presence(),
    serviceId(),
    serviceData_presence(),
    serviceData(),
    seq_presence(),
    seq()
    {
    }
    //-- Determines the service that is being executed (defined in service specific ASN.1 file);
    bool  serviceId_presence;
    Vds_ServiceId serviceId;
    //-- Service specific data (defined in service specific ASN.1 file);
    bool  serviceData_presence;
    ServiceData_Model serviceData;
    //-- Message sequence to prevent the replay attack Scope:MessageSequence;
    bool  seq_presence;
    uint8_t seq;

};

class VDServiceRequest_Model: public VdsDataBase
{
public:
    VDServiceRequest_Model():
    header(),
    body_presence(),
    body()
    {
    }
    //-- A request header, common for all requests;
    RequestHeader_Model header;
    //-- A request body specific for the type of service being executed;
    bool  body_presence;
    RequestBody_Model body;

};


#endif
