///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive AG and Subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file app_data.h
//     all app enum and data struct define.

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef APP_DATA_H
#define APP_DATA_H

#include <string>
#include <vector>

//#include "ipcb_data.hpp"
#include <list>

//ENUM/////////////////////////////////////

#define VDS_DEFAULT_PRESENCE  true
typedef enum {
	app_ecu = 0,
	app_cvc = 1
}App_CreatorId;

typedef enum {
	app_serviceRequest = 0,
	app_ack = 1
}App_RequestType;

typedef enum {
	app_MaBusinessType_rvdcSetting = 0, //--  set RVDC Authorization Status
	app_MaBusinessType_assignmentNotification = 1, //--   Synchronization status between Server and end.
	app_MaBusinessType_maSynchronizationReq = 2, //--  end to request MAs from RVDC server
	app_MaBusinessType_maSynchronization = 3, //-- Support vehicle request to cloud for response and cloud to vehicle for push
	app_MaBusinessType_exceptionHandling = 4,
	app_MaBusinessType_measurementDataPackage = 5
}App_MaBusinessType;

typedef enum {
	app_startofdcy = 0,
	app_endofdcy = 1
}App_TriggerType;

typedef enum {
	app_or = 0,
	app_and = 1
}App_Operator;

typedef enum {
	app_equalto = 1,
	app_greaterthan = 2,
	app_greaterthanorequal = 3,
	app_lessthan = 4,
	app_lessthanorequal = 5
}App_CompareType;

typedef enum {
	app_DataType_clientConfSyncRequest = 0,
	app_DataType_ecuExceptionReport = 1,
	app_DataType_inventoryInstruction = 2,
	app_DataType_availableAssignmentRequest = 3,
	app_DataType_availableAssignment = 4,
	app_DataType_installationInstruction = 5,
	app_DataType_assignmentNotification = 6,
	app_DataType_clientHmiVehicleSettings = 7,
	app_DataType_assignmentData = 8,
	app_DataType_otaAssignmentFileInfo = 9,
	app_DataType_installationSummary = 10
}App_DataType;

typedef enum {
	app_ecarx = 0, //-- using ecarx OTA Server
	app_geely = 1, //-- using geely It  OTA Server
	app_lynkCoEn = 2, //-- lynk&CO  international  OTA Server
	app_lynkCoCh = 3, //-- lynk&CO  chinese  OTA Server
	app_proton = 4, //-- proton  OTA Server
	app_lotus = 5 //-- lotus  OTA Server
}App_AppId;

typedef enum {
	app_arabic = 1,
	app_bulgarian = 2,
	app_chineseTradCan = 3,
	app_chineseSimpMan = 4,
	app_chineseTradMan = 5,
	app_czech = 6,
	app_danish = 7,
	app_dutch = 8,
	app_australianEnglish = 9,
	app_ukEnglish = 10,
	app_americanEnglish = 11,
	app_estonian = 12,
	app_finnish = 13,
	app_flemish = 14,
	app_canadianFrench = 15,
	app_french = 16,
	app_german = 17,
	app_greek = 18,
	app_hungarian = 19,
	app_italian = 20,
	app_japanese = 21,
	app_korean = 22, //-- (Han'Gul)
	app_latvian = 23,
	app_lithuanian = 24,
	app_norwegian = 25,
	app_polish = 26,
	app_brazilianPortuguese = 27,
	app_portuguese = 28,
	app_romanian = 29,
	app_russian = 30,
	app_slovak = 31,
	app_slovene = 32,
	app_spanish = 33,
	app_americanSpanish = 34,
	app_swedish = 35,
	app_thai = 36,
	app_turkish = 37
}App_Language;

typedef enum {
	app_periodicTrg = 1, //-- UM Driving (periodic MDP)
	app_umChangeTrg = 2, //-- UM Driving -> UM != Driving (1 MDP)
	app_chargeStartTrg = 3, //-- Charging start (1 MDP)
	app_chargeStopTrg = 4, //-- Charging Stop (1 MDP)
	app_disableRmsTrg = 5 //-- CC#14=Active -> cc#14=Inactive (1 MDP)
}App_NEVTriggerCondition;

typedef enum {
	app_notAvailable = 0, //--not available : no GNSS information available (e.g no GNSS antenna connection, GNSS receiver failure)
	app_noFix = 1, //--No Fix: satellites received - no position available
	app_fix2D = 2, //--2D fix : satellites received : 2 dimensional position fix
	app_fix3D = 3, //--3D fix : satellites received : 3 dimensional position fix
	app_startupMode = 4 //--Based on a position saved after last driving cycle (Cold Start Fix)
}App_NEVGnssFixType;

typedef enum {
	app_noDr = 0, //-- No dead reckoning used
	app_drNoMapMatch = 1, //-- Dead reckoning used but not matched to any map
	app_drMapMatched = 2 //-- Position is matched to map
}App_NEVDeadReckoningType;

typedef enum {
	app_mtp = 1, //-- TEM Provision
	app_mtu = 2, //-- TEM Unprovision
	app_mcu = 4, //-- Configuration update
	app_mpm = 5, //-- TEM power mode mgmt.
	app_mta = 6, //-- TEM activation
	app_mul = 7, //-- Upload TEM log
	app_mrs = 8, //-- TEM factory reset
	app_rin = 11, //-- Remote  control inhibition
	app_rdl = 13, //-- Remote  door lock
	app_rdu = 14, //-- Remote  door unlock
	app_rtl = 15, //-- Remote  trunk lock
	app_rtu = 16, //-- Remote  trunk unlock
	app_res = 17, //-- Remote  engine start
	app_rvi = 18, //-- Remote  vehicle immobilization
	app_rcc = 19, //-- Remote  climate control (non-engine)
	app_rce = 20, //-- Remote  climate control
	app_rhl = 21, //-- Remote  honk & light
	app_rsh = 22, //-- Remote  seat heat
	app_rws = 23, //-- Remote  close/open window/sunroof
	app_rpc = 24, //-- Remote  PM 2.5 clean
	app_rpp = 25, //-- Remote  Pull PM 2.5
	app_ren = 26, //-- Remote  event notification
	app_rec = 27, //-- Remote EV charging, schedule
	app_cec = 31, //-- E Call
	app_cep = 32, //-- PSAP E Call
	app_cbc = 33, //-- B Call
	app_cic = 34, //-- I Call
	app_svt = 41, //-- SVT
	app_stn = 44, //-- Theft Notification
	app_rvs = 51, //-- Remote status & dashboard
	app_trs = 52, //-- Triggered Remote status
	app_jou = 61, //-- Journey log
	app_hec = 62, //-- Health check
	app_dia = 63, //-- Remote diagnostics
	app_ota = 67, //-- OTA upgrade
	app_osc = 68, //-- Send to car
	app_vun = 69, //-- Vehicle unlock notification
	app_dkb = 80, //-- Digital key booking
	app_bks = 89, //-- Blue tooth key sharing
	app_bdc = 90, //-- Big Data collect
	app_wcr = 71, //-- windows close reminder
	app_rsv = 72, //-- Remote seat ventilate
	app_mcp = 101, //-- Certificate provision
	app_msr = 102, //-- message sequence reset
	app_fota = 200, //-- FOTA upgrade
	app_rct = 201, //-- Remote Charge location and timer
	app_vdr = 202, //-- Vin Discrepancy report
	app_rdo = 203, //-- Remote open trunk/hood/Tank Flag/Charge Lid
	app_rdc = 204, //-- Remote close trunk/hood/Tank Flag/Charge Lid
	app_sra = 205, //-- Remote Service Reminder
	app_rcr = 206, //-- Remote Connectivity Report
	app_rfd = 207, //-- Remote fragrance dispensing
	app_rqt = 208, //-- Remote Query towing status
	app_rcs = 209, //-- Remote charge settings
	app_uvs = 210, //-- Remote update vehicle settings
	app_rvc = 250, //-- RVDC
	app_pvs = 300, //-- Part of vehicle status
	app_rms = 500 //-- Remote monitor service
}App_ServiceId;

typedef enum {
	app_startService = 0,
	app_requestData = 1,
	app_responseData = 2,
	app_executionAction = 4,
	app_terminateService = 20
}App_ServiceCommand;

typedef enum {
	app_incorrectState = 1,
	app_invalidData = 2,
	app_invalidFormat = 3,
	app_powderModeNotCorrect = 4,
	app_conditionsNotCorrect = 5,
	app_executionFailure = 6,
	app_permissionDenied = 7,
	app_timeout = 8,
	app_backupBatteryUsage = 9
}App_ErrorCode;

typedef enum {
	app_door_unlock = 0,
	app_event_trigger = 1,
	app_status_report = 2
}App_NotificationReason;

typedef enum {
	app_EngineStatus_engine_running = 0,
	app_EngineStatus_engine_off = 1,
	app_EngineStatus_unknown = 2
}App_EngineStatus;

typedef enum {
	app_KeyStatus_no_key_inserted = 0,
	app_KeyStatus_key_inserted = 1,
	app_KeyStatus_unknown = 2
}App_KeyStatus;

typedef enum {
	app_passengerVehicle = 0,
	app_bus = 1,
	app_cargo = 2,
	app_motorCycle = 3
}App_VehicleType;

typedef enum {
	app_unblocked = 0,
	app_blocked = 1
}App_EngineBlockedStatus;

typedef enum {
	app_no_srs_info = 0,
	app_srs_not_deployed = 1,
	app_srs_deployed = 2
}App_SRSStatus;

typedef enum {
	app_mainBattery = 0,
	app_backupBattery = 1
}App_PowerSource;

typedef enum {
	app_normal = 0,
	app_standby = 1,
	app_sleep_poll = 2,
	app_off = 3
}App_TEMPowerMode;

typedef enum {
	app_deactivated = 0,
	app_activatedAndUnprovioned = 1,
	app_activatedAndProvisioned = 2
}App_TEMState;

typedef enum {
	app_HealthStatus_ok = 0,
	app_HealthStatus_faulty = 1,
	app_HealthStatus_unknown = 2,
	app_HealthStatus_invalid = 3
}App_HealthStatus;

typedef enum {
	app_FuelType_petrol = 0,
	app_FuelType_diesel = 1,
	app_FuelType_petrolHybrid = 2,
	app_FuelType_dieselHybrid = 3,
	app_FuelType_ev = 4,
	app_FuelType_hydrogenFuelcellEV = 5,
	app_FuelType_compressedGas = 6,
	app_FuelType_liquefiedPropaneGas = 7,
	app_FuelType_invalid = 8
}App_FuelType;

typedef enum {
	app_testFailed = 0,
	app_testFailedThisOperationCycle = 1,
	app_pendingDTC = 2,
	app_confirmedDTC = 3,
	app_testNotCompletedSinceLastClear = 4,
	app_testFailedSinceLastClear = 5,
	app_testNotCompletedThisOperationCycle = 6,
	app_warningIndicatorRequested = 7
}App_DtcStatusBitsIso14229;

typedef enum {
	app_calendar = 0,
	app_distance = 1,
	app_engineHours = 2,
	app_engineRequest = 3,
	app_other = 4
}App_ServiceWarningTriggerReason;


//ENUM/////////////////////////////////////

////////////////////////////////////////////
class RequestHeader_Data
{
public:
    uint8_t requestid;
    long timeStampSeconds;
    long timeStampMilliseconds_presence;
    long timeStampMilliseconds;
    bool  eventId_presence;
    long eventIdSeconds;
    long eventIdMilliseconds_presence;
    long eventIdMilliseconds;
    bool  creatorId_presence;
    uint32_t creatorId;
    bool  messageTTL_presence;
    uint32_t messageTTL;
    bool  requestType_presence;
    uint32_t requestType;
    bool  ackRequired_presence;
    bool ackRequired;
    bool isInitiator;

    RequestHeader_Data() :
        requestid(),
        timeStampSeconds(),
        timeStampMilliseconds_presence(),
        timeStampMilliseconds(),
        eventId_presence(),
        eventIdSeconds(),
        eventIdMilliseconds_presence(),
        eventIdMilliseconds(),
        creatorId_presence(),
        creatorId(),
        messageTTL_presence(),
        messageTTL(),
        requestType_presence(),
        requestType(),
        ackRequired_presence(),
        ackRequired(),
        isInitiator()
    {}
};

class AppDataBase
{
public:
    AppDataBase() : header(){}
    virtual ~AppDataBase(){}

public:
    RequestHeader_Data header;
};

////////////////////////////////////////////

//SVT/////////////////////////////////////
class SvtRequest_t: public AppDataBase
{
public:
    SvtRequest_t():
    serviceId(),
    serviceCommand(),
    scheduledTimeSeconds(),
    scheduledTimeMilliseconds(),
    endTimeSeconds(),
    endTimeMilliseconds(),
    interval()
    {
    }
    long serviceId;
    long serviceCommand;
    long scheduledTimeSeconds;
    long scheduledTimeMilliseconds;
    long endTimeSeconds;
    long endTimeMilliseconds;
    long interval;
} ;

class SvtTrackPoint_t: public AppDataBase
{
public:
    SvtTrackPoint_t():
    isSVT(),
    systemTimeSeconds(),
    systemTimeMilliseconds(),
    speed(),
    latitude(),
    longitude(),
    altitude(),
    posCanBeTrusted(),
    carLocatorStatUploadEn(),
    marsCoordinates()
    {
    }
    int isSVT;
    long systemTimeSeconds;
    long systemTimeMilliseconds;
    long speed;
    long latitude;
    long longitude;
    long altitude;
    int posCanBeTrusted;
    int carLocatorStatUploadEn;
    int marsCoordinates;
};

class SvtServiceResult_t: public AppDataBase
{
public:
    SvtServiceResult_t():
    operationSucceeded(),
    errorCode(),
    vehicleErrorCode(),
    message()
    {
    }
    int operationSucceeded;
    long errorCode;
    long vehicleErrorCode;
    std::string message;
};
//SVT/////////////////////////////////////

//MCU/////////////////////////////////////
enum SERVICE_PARA_TYPE
{
    UNKNOWN = 0,
    INT_VAL = 1,
    STRING_VAL = 2,
    BOOL_VAL = 3,
    TIMESTAMP_VAL = 4,
    DATA_VAL
};

class McuConfiguration_t
{
public:
    std::string item;
    std::string key;
    int valueType;
    std::string strValue;
    int intValue;
    bool boolValue;
    std::string dataValue;
    long timeSeconds;
    long timeMilliseconds;
};

class McuRequest_t: public AppDataBase
{
public:
    McuRequest_t():
    serviceId(),
    serviceCommand(),
    configItems()
    {
    }
    long serviceId;
    long serviceCommand;
    std::vector<McuConfiguration_t> configItems;
} ;

class McuServiceResult_t: public AppDataBase
{
public:
    McuServiceResult_t():
    operationSucceeded(),
    errorCode(),
    vehicleErrorCode(),
    message()
    {
    }
    int operationSucceeded;
    long errorCode;
    long vehicleErrorCode;
    std::string message;
};

//MCU/////////////////////////////////////

//RVS/////////////////////////////////////
#define RVS_KEY_BUF_SIZE      32
#define RVS_DATA_BUF_SIZE    64
#define RVS_CONDITION_BUF_SIZE    64
#define RVS_VIN_BUF_SIZE       32

class RvsHead_t: public AppDataBase
{
};

typedef struct
{
    long TriggerTimes;
    long NotifyInterval;
}RvsEventAction_t;

typedef struct
{
    char Condition[RVS_CONDITION_BUF_SIZE];
    int ConditonSize;
}RvsRuleCondition_t;

typedef struct
{
    long RuleId;
    long StartTimeSeconds;
    long StartTimeMilliseconds;
    long EndTimeSeconds;
    long EndTimeMilliseconds;
    long RuleConditionCount;
    RvsRuleCondition_t RuleCondition[8];
    long Interval;
    RvsEventAction_t EventAction;
}RvsEventRule_t;

typedef struct
{
    char Key[RVS_KEY_BUF_SIZE];
    int keysize;
    long IntVal;
    char StringVal[RVS_DATA_BUF_SIZE];
    int stringsize;
    bool BoolVal;
    long TimeSeconds;
    long TimeMilliseconds;
    char DataVal[RVS_DATA_BUF_SIZE];
    int datasize;
}RvsServiceParameter_t;

class RvsRequest_t : public AppDataBase
{
public:
    long ServiceId;
    long ServiceCommand;
    bool ServiceCommandIsNull;
    long ServiceParamCount;
    RvsServiceParameter_t ServiceParam[16];
    RvsEventRule_t EventRule;
};

typedef struct {
	long	latitude;
	long	longitude;
	long	altitude;
	int posCanBeTrusted; //BOOLEAN_t
	int carLocatorStatUploadEn; //BOOLEAN_t
	int marsCoordinates; //BOOLEAN_t
} RvsPosition_t;

typedef struct
{
    RvsPosition_t position;
    long speed;             // unit:km/h
    long speedValidity;
    long direction;
    long engineStatus;
    long keyStatus;
    long engineBlockedStatus;
    long distanceToEmpty;   // unit:km
    long usageMode;
}Rvs_BS_t; //basicVehicleStatus

typedef struct{
    uint8_t vin[RVS_VIN_BUF_SIZE];
    long fuelType	;
} Rvs_AS_CI_t;//additionalVehicleStatus.confAndId

typedef struct{
    long	srsStatus;
    long	vehicleAlarm;
    long	doorOpenStatusDriver;
    long	doorOpenStatusPassenger;
    long	doorOpenStatusDriverRear;
    long	doorOpenStatusPassengerRear;
    long	doorLockStatusDriver;
    long	doorLockStatusPassenger;
    long	doorLockStatusDriverRear;
    long	doorLockStatusPassengerRear;
    long	trunkOpenStatus;
    long trunkLockStatus;
    long engineHoodOpenStatus;
    long centralLockingStatus;
    int seatBeltStatusDriver; //BOOLEAN_t
    int seatBeltStatusPassenger; //BOOLEAN_t
    int seatBeltStatusDriverRear; //BOOLEAN_t
    int seatBeltStatusPassengerRear; //BOOLEAN_t
    long	handBrakeStatus;
    long	electricParkBrakeStatus;
    long	electricParkBrakeStatusValidity;
}Rvs_AS_DS_t;//additionalVehicleStatus.drivingSafetyStatus

typedef struct  {
	long	stateOfCharge;
	long	chargeLevel;
	long	stateOfHealth;
	long	voltage;
} RvsBatteryStatus_t;

typedef struct{
	RvsBatteryStatus_t	 mainBatteryStatus;
	long	tyreStatusDriver;
	long	tyreStatusPassenger;
	long	tyreStatusDriverRear;
	long	tyreStatusPassengerRear;
	long	tyrePreWarningDriver;
	long	tyrePreWarningPassenger;
	long	tyrePreWarningDriverRear	;
	long	tyrePreWarningPassengerRear;
	long	engineHrsToService;
	long	distanceToService;
	long	daysToService;
	long	serviceWarningStatus;
	long	serviceWarningTrigger;
	long	odometer;       // unit:meter
	long	brakeFluidLevelStatus;
	long	washerFluidLevelStatus;
} Rvs_AS_MS_t;//additionalVehicleStatus.maintenanceStatus

typedef struct  {
	long	engineOilLevelStatus;
	long	engineOilTemperature	;
	long	engineOilPressureWarning;
	long	engineCoolantTemperature;
	long	engineCoolantTemperatureValidity;
	long	engineCoolantLevelStatus;
	long	fuelLevel	;
	long	fuelLevelStatus;
	long	aveFuelConsumption;
	long	aveFuelConsumptionInLatestDrivingCycle;
	long	avgSpeed;
	long	tripMeter1;     // unit:meter
	long	tripMeter2;     // unit:meter
	long	bulbStatus;
} Rvs_AS_RS_t;//additionalVehicleStatus.runningStatus

typedef struct{
	long	winStatusDriver;
	long	winStatusDriverValidity;
	long	winStatusPassenger;
	long	winStatusPassengerValidity;
	long	winStatusDriverRear;
	long	winStatusDriverRearValidity;
	long	winStatusPassengerRear;
	long	winStatusPassengerRearValidity;
	long	sunroofOpenStatus;
	long	sunroofOpenStatusValidity;
	long	ventilateStatus;
	long	winPosDriver;
	long	winPosPassenger;
	long	winPosDriverRear;
	long	winPosPassengerRear;
	long	interiorTemp;
	long	exteriorTemp;
	long	exteriorTempValidity;
	int preClimateActive; //BOOLEAN_t
	long	airCleanSts;
	long	winCloseReminder;
	long	drvHeatSts;
	long	passHeatingSts;
	long	rlHeatingSts;
	long	rrHeatingSts;
	long	drvVentSts;
	long	passVentSts;
	long	rrVentSts;
	long	rlVentSts;
} Rvs_AS_CS_t;//additionalVehicleStatus.climateStatus

typedef struct {
	int brakePedalDepressed;//BOOLEAN_t
	long	brakePedalDepressedValidity;
	long	engineSpeed;
	long	engineSpeedValidity;
	long	transimissionGearPostion;
	long	cruiseControlStatus;
} Rvs_AS_DB_t;//additionalVehicleStatus.drivingBehaviourStatus

typedef struct{
	long	interiorPM25;
	long	exteriorPM25;
} Rvs_AS_PS_t;//additionalVehicleStatus.pollutionStatus

typedef struct{
	int isCharging;//BOOLEAN_t
	int isPluggedIn;//BOOLEAN_t
	long	stateOfCharge;
	long	chargeLevel;
	long	timeToFullyCharged;
	long	statusOfChargerConnection;
	long	chargerState;
	long	distanceToEmptyOnBatteryOnly;
	long	ems48VSOC;
	long	emsHVRBSError;
	long	emsRBSModeDisplay;
	long	emsOVPActive;
	long	ems48vPackTemp1;
	long	ems48vPackTemp2;
	uint8_t emsBMSLBuildSwVersion[32];
	long	emsCBRemainingLife;
	long	chargeHvSts;
	long	ptReady;
	long	averPowerConsumption;
	long	chargeSts;
}Rvs_AS_ES_t;//additionalVehicleStatus.electricVehicleStatus

typedef struct{
	long	seconds;
	long	milliseconds;
} RvsTimeStamp_t;

typedef struct{
	uint8_t key[32];
	RvsTimeStamp_t collectTime;
	long	value;
	uint8_t stringVal[32];
	bool	boolVal;
	RvsTimeStamp_t timestampVal;
	uint8_t dataVal[32];
} RvsGenericVehicleSignal_t;

typedef struct {
	RvsGenericVehicleSignal_t list[16];
} Rvs_AS_EW_t;//additionalVehicleStatus.ecuWarningMessages

typedef struct{
	long	heatedFrontSeats;
	long	parkingClimateLevel;
	long	heatedSteeringWheel;
} Rvs_AS_CC_t;//additionalVehicleStatus.carConfig

typedef struct{
    RvsGenericVehicleSignal_t list[16];
} Rvs_AS_SIGNALS_KEY_t;//additionalVehicleStatus.signals.<key>

typedef struct{
	uint8_t msisdn[32];
	uint8_t imsi[32];
	uint8_t iccId[32];
} RvsSIMInfo_t;

typedef struct{
	RvsSIMInfo_t	simInfo;
	uint8_t mobileNetwork[32];
}RvsNetworkAccessStatus_t;

typedef struct{
	uint8_t *buf;	/* BIT STRING body */
	int size;	/* Size of the above buffer */
	int bits_unused;/* Unused trailing bits in the last octet (0..7) */
}Rvs_BIT_STRING_t;

typedef struct {
	uint8_t dtc[32];
	Rvs_BIT_STRING_t status;
	long	failureCount;
	RvsTimeStamp_t lastFail;
	RvsTimeStamp_t firtFail;
	long	firstFailureOdometer;
	long	firstFailureBatteryVoltage;
	long	firstFailureVehicleSpeed;
} RvsDiagnosticTroubleCode_t;

typedef struct{
	uint8_t ecuID[32];
	long diagnosticResult;
	RvsDiagnosticTroubleCode_t list[16];
	long	exceptionCode;
} RvsECUDiagnostic_t;

typedef struct{
	uint8_t vin[RVS_VIN_BUF_SIZE];
	long powerSource;
	RvsBatteryStatus_t backupBattery;
	long	 powerMode;
	RvsTimeStamp_t sleepCycleNextWakeupTime;
	int serviceProvisoned; //BOOLEAN_t
	long healthStatus;
	RvsECUDiagnostic_t diagnostics;
	RvsNetworkAccessStatus_t networkAccessStatus;
	uint8_t serialNumber[32];
	uint8_t completeECUPartNumbers[32];
	uint8_t imei[32];
	uint8_t hwVersion[32];
	uint8_t swVersion[32];
	uint8_t mpuVersion[32];
	uint8_t mcuVersion[32];
} Rvs_TS_t;//temStatus

typedef struct{
	Rvs_AS_CI_t confAndId;
	Rvs_AS_DS_t drivingSafetyStatus;
	Rvs_AS_MS_t maintenanceStatus;
	Rvs_AS_RS_t runningStatus;
	Rvs_AS_CS_t climateStatus;
	Rvs_AS_DB_t drivingBehaviourStatus;
	Rvs_AS_PS_t pollutionStatus;
	Rvs_AS_CC_t ecuWarningMessages;
	Rvs_AS_ES_t electricVehicleStatus;
	Rvs_AS_CC_t carConfig;
	Rvs_AS_SIGNALS_KEY_t signals;
	uint8_t data[32];
} Rvs_AS_t;//additionalVehicleStatus


namespace fsm
{
//RVDC////////////////////////////////////
class TimeStamp_Data : public AppDataBase
{
public:
    //-- UTC Timestamp Scope:INTEGER 0..2147483647;
    uint32_t seconds;
    //-- Millisecond within the second Scope:INTEGER 0..999;
    bool  milliseconds_presence;
    uint16_t milliseconds;

    TimeStamp_Data():
    seconds(),
    milliseconds_presence(),
    milliseconds()
    {
    }

};

class Position_Data : public AppDataBase
{
public:
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

class ServiceParameter_Data : public AppDataBase
{
public:
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
    TimeStamp_Data timestampVal;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  dataVal_presence;
    std::string dataVal;

    ServiceParameter_Data():
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

};

class DiagnosticCommand_Data : public AppDataBase
{
public:
    DiagnosticCommand_Data();
    virtual ~DiagnosticCommand_Data();

    //-- This is the ECU target address for the request.  Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //-- This is the UDS service to be used Scope:PrintableString SIZE 1..64;
    std::string serviceId;
    //-- This is the input to be added after the UDS service, this can contain sub service ID,Identifiers and/or other input data to the ECU. Scope:PrintableString SIZE 1..65535;
    std::string input;
    bool  compareType_presence;
    App_CompareType compareType;
    //-- This is the value to compare the ECU response to. Scope:PrintableString SIZE 1..65535;
    bool  compareValue_presence;
    std::string compareValue;
    //-- This is the time to wait between two diagnostic trigger commands, this is specified din seconds. Scope:INTEGER 1..512;
    uint16_t delayUntilNextTriggerCommand;

};

class Trigger_Data : public AppDataBase
{
public:
    //-- This contains if the collection of data shall be carried out on start or end of a DCY. startofdcy (0),endofdcy (1);
    App_TriggerType trigger;
    //--Scope:INTEGER 1..4294967295;
    uint32_t interval;

};

class AuthorizationStatus_Data : public AppDataBase
{
public:
    //-- True if data collection is enabled;
    bool  dataCollection_presence;
    bool dataCollection;
    //--  True if full remote diagnostics is enabled for the vehicle;
    bool  remoteDiagnostics_presence;
    bool remoteDiagnostics;

};

class Ma_Data : public AppDataBase
{
public:
    //--Scope:INTEGER 1..4294967295;
    uint32_t maId;
    //--Scope:INTEGER 1..4294967295;
    uint32_t maVersion;

};

class UDS_Data : public AppDataBase
{
public:
    //-- The operator between the diagnostic trigger commands. 0 = Or 1 = And;
    App_Operator App_operator;
    //-- This contains the time between two trigger checks, it is specified in seconds. Scope:INTEGER 1..4294967295;
    uint32_t delayForNextTriggerCheck;
    //--Scope:INTEGER 1..4294967295;
    uint32_t delayAfterCollection;
    //-- This contains the actual trigger commands;
    std::vector<DiagnosticCommand_Data> diagnosticCommands;

};

class TelematicsParameters_Data : public AppDataBase
{
public:
    //-- This ELEMENT describes how many seconds the maximum transmission time is before the MDP is discarded. Scope:INTEGER 1..4294967295;
    uint32_t timeToLive;
    //-- This contains the type of upload channels the telematics unit can use. Scope:INTEGER 1..255;
    uint8_t uploadPriority;

};

class ValidationKey_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string validationKey;
    bool  partIdentifiers_presence;
    std::vector<std::string> partIdentifiers;

    ValidationKey_Data():
    validationKey(),
    partIdentifiers_presence(),
    partIdentifiers()
    {
    }

};

class SoftwarePartInstallationInstruction_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string partidentifier;
    //--Scope:INTEGER 1..4294967295;
    uint32_t estimatedInstallationtime;

};

class GenericVehicleSignal_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..100;
    std::string key;
    bool  collectTime_presence;
    TimeStamp_Data collectTime;
    bool  value_presence;
    int32_t value;
    //--Scope:UTF8String SIZE 0..65535;
    bool  stringVal_presence;
    std::string stringVal;
    bool  boolVal_presence;
    bool boolVal;
    bool  timestampVal_presence;
    TimeStamp_Data timestampVal;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  dataVal_presence;
    std::string dataVal;

};

class TimePosition_Data: public AppDataBase
{
public:
    TimePosition_Data():
    time_presence(),
    time(),
    position_presence(),
    position()
    {
    }
    bool  time_presence;
    TimeStamp_Data time;
    bool  position_presence;
    Position_Data position;

};

class ConnectivityInfo_Data : public AppDataBase
{
public:
    bool  connectTime_presence;
    TimeStamp_Data connectTime;
    bool  connectPosition_presence;
    Position_Data connectPosition;
    //--Scope:PrintableString SIZE 0..50;
    bool  connectType_presence;
    std::string connectType;
    bool  numRetriesConnect_presence;
    int32_t numRetriesConnect;
    //--Scope:PrintableString SIZE 0..100;
    bool  retryReason_presence;
    std::string retryReason;

};

class DiagnosticTroubleCode_Data : public AppDataBase
{
public:
    //--ID of the trouble code. OEM specific, Scope:PrintableString SIZE 1..100;
    std::string dtc;
    //--status of the DTC. Follows ISO 14229;
    App_DtcStatusBitsIso14229 status;
    //--Failure counter. Scope:INTEGER 0..255;
    bool  failureCount_presence;
    uint8_t failureCount;
    //--Time of last failure;
    bool  lastFail_presence;
    TimeStamp_Data lastFail;
    //--Time of first failure;
    bool  firtFail_presence;
    TimeStamp_Data firtFail;
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

class EcusAndPartNumber_Data : public AppDataBase
{
public:
    //-- This shall contain the ECU ID for the first ECU Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //-- This contains the diagnostic part number Scope:PrintableString SIZE 1..64;
    std::string diagnosticPartNumber;

};

class ExecutionTrigger_Data : public AppDataBase
{
public:
    bool push;
    //-- Trigger is based on collection of UDS data and comparison, see requirement;
    bool  uds_presence;
    UDS_Data uds;
    //-- Trigger is based on time interval;
    bool  time_presence;
    Trigger_Data time;
    //--Trigger is based on driving cycle interval;
    bool  drivingCycle_presence;
    Trigger_Data drivingCycle;

};

class StopCondition_Data : public AppDataBase
{
public:
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

class EcuInventory_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    std::vector<ValidationKey_Data> validationKeys;
    //--Scope:UTF8String SIZE 1..65535;
    bool  exceptionMessage_presence;
    std::string exceptionMessage;

    EcuInventory_Data():
    ecuAddress(),
    validationKeys(),
    exceptionMessage_presence(),
    exceptionMessage()
    {
    }

};

class EcuInstruction_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //--Scope:INTEGER 1..65535;
    uint16_t queuedRequest;
    //--Scope:PrintableString SIZE 1..256;
    std::string securityKey;
    std::vector<SoftwarePartInstallationInstruction_Data> softwarePartInstallationInstructions;

    EcuInstruction_Data():
    ecuAddress(),
    queuedRequest(),
    securityKey(),
    softwarePartInstallationInstructions()
    {
    }
};

class AssignmentValidation_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string validationType;
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //--Scope:PrintableString SIZE 1..256;
    std::string validationkey;
    std::vector<std::string> partidentifiers;

    AssignmentValidation_Data():
    validationType(),
    ecuAddress(),
    validationkey(),
    partidentifiers()
    {
    }

};

class SoftwarePartSummary_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string partIdentifier;
    //--Scope:INTEGER 1..65535;
    uint16_t partRetries;
    //--Scope:INTEGER 1..36000000;
    uint32_t measuredInstallationTime;
    //--Scope:PrintableString SIZE 1..256;
    std::string softwarePartStatus;

    SoftwarePartSummary_Data():
    partIdentifier(),
    partRetries(),
    measuredInstallationTime(),
    softwarePartStatus()
    {
    }
};

class ExceptionMessage_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string activity;
    //--Scope:PrintableString SIZE 1..256;
    std::string action;
    //--Scope:PrintableString SIZE 1..1024;
    std::string exception;

    ExceptionMessage_Data():
    activity(),
    action(),
    exception()
    {
    }

};

class LanguageSetting_Data : public AppDataBase
{
public:
    App_Language language;

    // constructor
    LanguageSetting_Data():
    language()
    {
    }
};

class NEVDateTime_Data : public AppDataBase
{
public:
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

class NEVCoordinatesLongLat_Data : public AppDataBase
{
public:
    //--Scope:INTEGER -2147483648..2147483647;
    uint64_t longitude;
    //--Scope:INTEGER -2147483648..2147483647;
    uint64_t latitude;

};

class DIDData_Data : public AppDataBase
{
public:
    //-- ID for the specific DID Scope:INTEGER 0..65535;
    uint16_t id;
    //-- DID READOUT Scope:OCTET STRING SIZE 0..100;
    std::string data;

};

class DTCData_Data : public AppDataBase
{
public:
    //-- ID for the specific DTC Scope:INTEGER 0..16777215;
    uint32_t id;
    //-- DTC BYTE Scope:OCTET STRING SIZE 0..100;
    std::string data;

};

class SIMInfo_Data : public AppDataBase
{
public:
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

class BatteryStatus_Data: public AppDataBase
{
public:
    BatteryStatus_Data():
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
class ECUDiagnostic_Data : public AppDataBase
{
public:
    //--ECU ID which will be OEM specific, Scope:PrintableString SIZE 1..100;
    std::string ecuID;
    //-- Diagnostic results;
    App_HealthStatus diagnosticResult;
    //--the DTCs for the ECU;
    bool  dtcs_presence;
    std::vector<DiagnosticTroubleCode_Data> dtcs;
    //--the code to identify the exception where the ECU cannot provide the DTCs, OEM specific;
    bool  exceptionCode_presence;
    int32_t exceptionCode;

};

class FragItem_Data : public AppDataBase
{
public:
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

class RvdcForcedSetting_Data : public AppDataBase
{
public:
    RvdcForcedSetting_Data():
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

class RvdcVehicleSetting_Data : public AppDataBase
{
public:
    RvdcVehicleSetting_Data():
    rvdcFunction(),
    collectOffSetTime()
    {
    }
    bool rvdcFunction;
    //--Scope:INTEGER 1..512;
    uint16_t collectOffSetTime;

};

class MaSetting_Data : public AppDataBase
{
public:
    MaSetting_Data():
    collectFunction(),
    authorizationStatus()
    {
    }
    //-- True if the RVDC function is activated and if set to false ,no matter what dataCollection and remoteDiagnostics to set,;
    bool collectFunction;
    AuthorizationStatus_Data authorizationStatus;

};

class MeasurementAssignment_Data : public AppDataBase
{
public:
    MeasurementAssignment_Data():
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
    TelematicsParameters_Data telematicsParameters;
    //-- Shall equal the usage mode signal, only values active or driving are allowed. Scope:INTEGER 1..4294967295;
    uint32_t usageModesAllowed;
    //-- The start time for the MA Scope:PrintableString SIZE 1..32;
    std::string startCondition;
    //-- The condition to stop the MA. At least one stop condition must be chosen. This shall be checked during parsing.;
    StopCondition_Data stopCondition;
    //-- The retention date for the MA. Scope:PrintableString SIZE 1..32;
    std::string retentiondate;
    //-- The authorization status needed to perform the MA		;
    AuthorizationStatus_Data authorizationStatus;
    //-- The type of execution trigger in the MA. Only one trigger type can be chosen.;
    ExecutionTrigger_Data executionTrigger;
    //-- The number of ECUs which are needed to perform MA.;
    std::vector<EcusAndPartNumber_Data> ecusAndPartNumbers;
    //-- Specifies the diagnostic commands to be executed;
    std::vector<DiagnosticCommand_Data> diagnosticCommand;
    //-- This shall be a CRC32 over the entire payload. Scope:PrintableString SIZE 1..64;
    std::string dataIntegrityCheck;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Data> serviceParameters;

};

class Mdp_Data : public AppDataBase
{
public:
    Mdp_Data():
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
    Ma_Data ma;
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

class Instruction_Data : public AppDataBase
{
public:
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
    std::vector<EcuInstruction_Data> ecuInstructions;
    std::vector<AssignmentValidation_Data> assignmentValidations;

    // constructor
    Instruction_Data():
    ecuRemaining(),
    installationInstructionSversion(),
    requiredPreparationTime(),
    expectedInstallationTime(),
    area1112SecurityCode(),
    ecuInstructions(),
    assignmentValidations()
    {
    }

};

class EcuSummary_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuAddress;
    //--Scope:INTEGER 1..100;
    uint8_t ecuRetries;
    //--Scope:PrintableString SIZE 1..256;
    std::string ecuStatus;
    std::vector<SoftwarePartSummary_Data> softwarePartSummarys;

    EcuSummary_Data():
    ecuAddress(),
    ecuRetries(),
    ecuStatus(),
    softwarePartSummarys()
    {
    }
};

class ExceptionReport_Data : public AppDataBase
{
public:
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
    ExceptionMessage_Data exceptionMessage;

    ExceptionReport_Data():
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

};

class AssignmentFileInfo_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..256;
    std::string partidentifier;
    //--Scope:PrintableString SIZE 1..256;
    std::string softwarePartSignature;
    //--Scope:PrintableString SIZE 1..256;
    std::string fileCheckSum;

    AssignmentFileInfo_Data():
    partidentifier(),
    softwarePartSignature(),
    fileCheckSum()
    {
    }

};

class NEVDRPositionData_Data : public AppDataBase
{
public:
    NEVDRPositionData_Data():
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
    NEVCoordinatesLongLat_Data longLat;
    //--Scope:INTEGER 0..360;
    uint16_t heading;
    //--255=unknown  Scope:INTEGER 0..255;
    uint8_t speedKmph;
    //--HDOP multiplied by 10. 255=unknown  Scope:INTEGER 0..255;
    uint8_t hdopX10;
    //--Number of satellites used in fix. 127=unknown  Scope:INTEGER 0..127;
    uint8_t numSat;
    //--GPS time received with the fix ;
    NEVDateTime_Data fixTime;
    //--Specifies the current GNSS fix, not regarding DR ;
    App_NEVGnssFixType fixType;
    App_NEVDeadReckoningType drType;
    //--Distance in meters driven since last fix  Scope:INTEGER 0..65535;
    uint16_t drDistance;

};

class ECUData_Data : public AppDataBase
{
public:
    ECUData_Data():
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
    std::vector<DIDData_Data> didData;
    std::vector<DTCData_Data> dtcData;

};

class TrackPoint_Data : public AppDataBase
{
public:
    TrackPoint_Data():
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
    TimeStamp_Data systemTime;
    Position_Data position;
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

class PostAddress_Data : public AppDataBase
{
public:
    PostAddress_Data():
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

class ConfigurationAndIdentity_Data : public AppDataBase
{
public:
    ConfigurationAndIdentity_Data():
    vin_presence(),
    vin(),
    fuelType_presence(),
    fuelType()
    {
    }
    bool  vin_presence;
    std::string vin;
    bool  fuelType_presence;
    App_FuelType fuelType;

};

class NetworkAccessStatus_Data : public AppDataBase
{
public:
    NetworkAccessStatus_Data():
    simInfo_presence(),
    simInfo(),
    mobileNetwork_presence(),
    mobileNetwork()
    {
    }
    bool  simInfo_presence;
    SIMInfo_Data simInfo;
    //-- 2G/3G/4G/wifi Scope:PrintableString SIZE 0..127;
    bool  mobileNetwork_presence;
    std::string mobileNetwork;

};

class ConnectivityStatus_Data : public AppDataBase
{
public:
    ConnectivityStatus_Data():
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
    TimePosition_Data wakeupTimePosition;
    bool  connectEstablishedList_presence;
    std::vector<ConnectivityInfo_Data> connectEstablishedList;
    bool  connectTime_presence;
    TimeStamp_Data connectTime;
    bool  connectPosition_presence;
    Position_Data connectPosition;
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
    std::vector<ConnectivityInfo_Data> lostConnectionsList;
    bool  sleeptimePosition_presence;
    TimePosition_Data sleeptimePosition;

};

class FragStrs_Data: public AppDataBase
{
public:
    FragStrs_Data():
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
    std::vector<FragItem_Data> items;

};

class TowStatus_Data : public AppDataBase
{
public:
    TowStatus_Data():
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

class RvdcSetting_Data : public AppDataBase
{
public:
    RvdcSetting_Data();
    virtual ~RvdcSetting_Data();

    //-- The objective of this interface is to set RVDC Authorization Status for cloud to end or end to cloud;
    bool  maSetting_presence;
    MaSetting_Data maSetting;
    //-- The objective of this interface is to provide RVDC vehicle settings to vehicle.;
    bool  vehicleSetting_presence;
    RvdcVehicleSetting_Data vehicleSetting;
    //-- The objective of this interface is to provide the changed language setting of in-vehicle HMI to RVDC server.;
    bool  languageSetting_presence;
    LanguageSetting_Data languageSetting;
    bool  forcedSetting_presence;
    RvdcForcedSetting_Data forcedSetting;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Data> serviceParameters;

};

class RvdcAssignmentNotification_Data : public AppDataBase
{
public:
    RvdcAssignmentNotification_Data();
    virtual ~RvdcAssignmentNotification_Data();

    //void Assign(const OpRVDCAssignmentNotification_Notification_Data& data);

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

class MaSynchronizationReq_Data : public AppDataBase
{
public:
    MaSynchronizationReq_Data();
    virtual ~MaSynchronizationReq_Data();

    //--Scope:INTEGER 1..4294967295;
    uint32_t storageQuota;
    //-- The cloud sends corresponding data according to the version reported by the vehicle. If the version of the vehicle changes, the cloud needs to be notified first. Scope:INTEGER 1..4294967295;
    uint32_t maFormatVersion;
    //-- installed measurements in vehicle;
    bool  installedMa_presence;
    std::vector<Ma_Data> installedMa;
    //-- generic service parameters used for extension,;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Data> serviceParameters;

};

class MaSynchronization_Data : public AppDataBase
{
public:
    MaSynchronization_Data();
    virtual ~MaSynchronization_Data();
    //void AssignTo(OpRVDCMeasurementSynchronization_Response_Data& data);

    //-- These MAs shall be removed;
    bool  removeMas_presence;
    std::vector<Ma_Data> removeMas;
    //-- New or updated MAs;
    bool  newMas_presence;
    std::vector<MeasurementAssignment_Data> newMas;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Data> serviceParameters;

};

class MeasurementDataPackage_Data : public AppDataBase
{
public:
    MeasurementDataPackage_Data();
    virtual ~MeasurementDataPackage_Data();
    //void Assign(const OpRVDCMeasurementDataPackage_Request_Data &data);

    //-- the onboard telematics unit parameters;
    TelematicsParameters_Data telematicsParameters;
    //-- generic service parameters used for extension;
    bool  serviceParameters_presence;
    std::vector<ServiceParameter_Data> serviceParameters;
    //-- actual data payload;
    Mdp_Data mdp;

};

class RvdcExceptionHandling_Data : public AppDataBase
{
public:
    RvdcExceptionHandling_Data();
    virtual ~RvdcExceptionHandling_Data();

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

class ClientConfSyncRequest_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //--Scope:PrintableString SIZE 1..256;
    std::string clientVersion;
    bool  ecuInventory_presence;
    std::vector<EcuInventory_Data> ecuInventory;

    // constructor
    ClientConfSyncRequest_Data():
    timestamp(),
    clientVersion(),
    ecuInventory_presence(),
    ecuInventory()
    {
    }

};

class InventoryInstruction_Data : public AppDataBase
{
public:
    //--Scope:PrintableString SIZE 1..32;
    std::string inventoryInstructionVersion;
    std::vector<EcuInventory_Data> ecuInstructions;

    // constructor
    InventoryInstruction_Data():
    inventoryInstructionVersion(),
    ecuInstructions()
    {
    }

};

class AvailableAssignmentRequest_Data : public AppDataBase
{
public:
    LanguageSetting_Data languageSetting;

    // constructor
    AvailableAssignmentRequest_Data():
    languageSetting()
    {
    }
};

class AvailableAssignment_Data : public AppDataBase
{
public:
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

    // constructor
    AvailableAssignment_Data():
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

};

class InstallationInstruction_Data : public AppDataBase
{
public:
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    std::vector<Instruction_Data> instructions;

    // constructor
    InstallationInstruction_Data():
    installationOrderId(),
    instructions()
    {
    }
};

class FotaAssignmentNotification_Data : public AppDataBase
{
public:
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
    std::vector<ServiceParameter_Data> serviceParams;

    FotaAssignmentNotification_Data():
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

};

class InstallationSummary_Data : public AppDataBase
{
public:
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //--Scope:INTEGER 1..255;
    uint8_t repeatresets;
    //--Scope:INTEGER 1..4294967295;
    uint32_t totalInstallationTime;
    std::vector<EcuSummary_Data> ecuSummarys;

    InstallationSummary_Data():
    installationOrderId(),
    timestamp(),
    repeatresets(),
    totalInstallationTime(),
    ecuSummarys()
    {
    }

};

class EcuExceptionReport_Data : public AppDataBase
{
public:
    //--Scope:INTEGER 1..65535;
    uint16_t exceptionReportMsgRemaining;
    //-- Shall be maximum 3 reports;
    std::vector<ExceptionReport_Data> exceptionReports;

    EcuExceptionReport_Data():
    exceptionReportMsgRemaining(),
    exceptionReports()
    {
    }

};

class ClientHmiVehicleSettings_Data : public AppDataBase
{
public:
    bool otaSetting;
    bool autoSync;
    bool autoDownload;
    bool autoInstallation;

    ClientHmiVehicleSettings_Data():
    otaSetting(),
    autoSync(),
    autoDownload(),
    autoInstallation()
    {
    }

};

class AssignmentData_Data : public AppDataBase
{
public:
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    //--Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    std::vector<std::string> urls;

    AssignmentData_Data():
    installationOrderId(),
    timestamp(),
    urls()
    {
    }

};

class OtaAssignmentFileInfo_Data : public AppDataBase
{
public:
    //--Scope:UTF8String SIZE 1..256;
    std::string installationOrderId;
    std::vector<AssignmentFileInfo_Data> assignFileInfos;

    OtaAssignmentFileInfo_Data():
    installationOrderId(),
    assignFileInfos()
    {
    }
};

class EcuData_Data : public AppDataBase
{
public:
    EcuData_Data():
    noDiagData_choice(),
    data_choice(),
    data()
    {
    }
    //-- No diagnostic data included in the MDP due to Charging, end of driving cycle or tester with higher priority is present. ;
    bool  noDiagData_choice;
    //-- ECU data;
    bool  data_choice;
    std::vector<ECUData_Data> data;

};

class NEVDRVehiclePosition_Data : public AppDataBase
{
public:
    NEVDRVehiclePosition_Data():
    noValidData_choice(),
    drPosition_choice(),
    drPosition()
    {
    }
    //--Used if the server can not provide a position of sufficient quality ;
    bool  noValidData_choice;
    bool  drPosition_choice;
    NEVDRPositionData_Data drPosition;

};

class Error_Data : public AppDataBase
{
public:
    Error_Data():
    code(),
    vehicleErrorCode_presence(),
    vehicleErrorCode(),
    message_presence(),
    message()
    {
    }
    App_ErrorCode code;
    //-- the error code from vehicle/TEM, OEM specific;
    bool  vehicleErrorCode_presence;
    int32_t vehicleErrorCode;
    //-- the addtional error description Scope:PrintableString SIZE 0..255;
    bool  message_presence;
    std::string message;

};

class ConfigurationItem_Data : public AppDataBase
{
public:
    ConfigurationItem_Data():
    name(),
    parameters()
    {
    }
    std::string name;
    std::vector<ServiceParameter_Data> parameters;

};

class ContactInfo_Data : public AppDataBase
{
public:
    ContactInfo_Data():
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
    PostAddress_Data address;
    //--Scope:PrintableString SIZE 0..27;
    bool  phone1_presence;
    std::string phone1;
    //--Scope:PrintableString SIZE 0..27;
    bool  phone2_presence;
    std::string phone2;

};

class Notification_Data : public AppDataBase
{
public:
    Notification_Data():
    reason(),
    time(),
    code_presence(),
    code(),
    parameters_presence(),
    parameters()
    {
    }
    App_NotificationReason reason;
    TimeStamp_Data time;
    bool  code_presence;
    std::string code;
    bool  parameters_presence;
    std::vector<ServiceParameter_Data> parameters;

};

class RuleCondition_Data : public AppDataBase
{
public:
    RuleCondition_Data():
    condition()
    {
    }
    //-- format: <vehicle-status> <operator> <target> Scope:PrintableString SIZE 1..1000;
    std::string condition;

};

class EventAction_Data : public AppDataBase
{
public:
    EventAction_Data():
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

class TimerInfo_Data : public AppDataBase
{
public:
    TimerInfo_Data():
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

class BasicVehicleStatus_Data : public AppDataBase
{
public:
    BasicVehicleStatus_Data():
    vin_presence(VDS_DEFAULT_PRESENCE),
    vin(),
    position(),
    speed(),
    speedUnit_presence(VDS_DEFAULT_PRESENCE),
    speedUnit(),
    speedValidity_presence(VDS_DEFAULT_PRESENCE),
    speedValidity(),
    direction_presence(VDS_DEFAULT_PRESENCE),
    direction(),
    engineStatus_presence(VDS_DEFAULT_PRESENCE),
    engineStatus(),
    keyStatus_presence(VDS_DEFAULT_PRESENCE),
    keyStatus(),
    usageMode_presence(VDS_DEFAULT_PRESENCE),
    usageMode()
    {
    }
    bool  vin_presence;
    std::string vin;
    Position_Data position;
    //-- km/h   ;
    int32_t speed;
    //--Scope:PrintableString SIZE 4;
    bool  speedUnit_presence;
    std::string speedUnit;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  speedValidity_presence;
    uint8_t speedValidity;
    //--Scope:INTEGER 0..360;
    bool  direction_presence;
    uint16_t direction;
    bool  engineStatus_presence;
    App_EngineStatus engineStatus;
    bool  keyStatus_presence;
    App_KeyStatus keyStatus;
    //-- only for CMA car;
    bool  usageMode_presence;
    int32_t usageMode;

};

class LockStatus_Data: public AppDataBase
{
public:
    LockStatus_Data():
    winStatusDriver_presence(VDS_DEFAULT_PRESENCE),
    winStatusDriver(),
    winStatusPassenger_presence(VDS_DEFAULT_PRESENCE),
    winStatusPassenger(),
    winStatusDriverRear_presence(VDS_DEFAULT_PRESENCE),
    winStatusDriverRear(),
    winStatusPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    winStatusPassengerRear(),
    winStatusDriverWarning_presence(VDS_DEFAULT_PRESENCE),
    winStatusDriverWarning(),
    winStatusPassengerWarning_presence(VDS_DEFAULT_PRESENCE),
    winStatusPassengerWarning(),
    winStatusDriverRearWarning_presence(VDS_DEFAULT_PRESENCE),
    winStatusDriverRearWarning(),
    winStatusPassengerRearWarning_presence(VDS_DEFAULT_PRESENCE),
    winStatusPassengerRearWarning(),
    winPosDriver_presence(VDS_DEFAULT_PRESENCE),
    winPosDriver(),
    winPosPassenger_presence(VDS_DEFAULT_PRESENCE),
    winPosPassenger(),
    winPosDriverRear_presence(VDS_DEFAULT_PRESENCE),
    winPosDriverRear(),
    winPosPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    winPosPassengerRear(),
    sunroofOpenStatus_presence(VDS_DEFAULT_PRESENCE),
    sunroofOpenStatus(),
    sunroofPos_presence(VDS_DEFAULT_PRESENCE),
    sunroofPos(),
    sunroofOpenStatusWarning_presence(VDS_DEFAULT_PRESENCE),
    sunroofOpenStatusWarning(),
    curtainOpenStatus_presence(VDS_DEFAULT_PRESENCE),
    curtainOpenStatus(),
    curtainPos_presence(VDS_DEFAULT_PRESENCE),
    curtainPos(),
    curtainWarning_presence(VDS_DEFAULT_PRESENCE),
    curtainWarning(),
    tankFlapStatus_presence(VDS_DEFAULT_PRESENCE),
    tankFlapStatus(),
    chargeLidRearStatus_presence(VDS_DEFAULT_PRESENCE),
    chargeLidRearStatus(),
    chargeLidFrontStatus_presence(VDS_DEFAULT_PRESENCE),
    chargeLidFrontStatus(),
    ventilateStatus_presence(VDS_DEFAULT_PRESENCE),
    ventilateStatus(),
    doorOpenStatusDriver_presence(VDS_DEFAULT_PRESENCE),
    doorOpenStatusDriver(),
    doorOpenStatusPassenger_presence(VDS_DEFAULT_PRESENCE),
    doorOpenStatusPassenger(),
    doorOpenStatusDriverRear_presence(VDS_DEFAULT_PRESENCE),
    doorOpenStatusDriverRear(),
    doorOpenStatusPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    doorOpenStatusPassengerRear(),
    doorPosDriver_presence(VDS_DEFAULT_PRESENCE),
    doorPosDriver(),
    doorPosPassenger_presence(VDS_DEFAULT_PRESENCE),
    doorPosPassenger(),
    doorPosDriverRear_presence(VDS_DEFAULT_PRESENCE),
    doorPosDriverRear(),
    doorPosPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    doorPosPassengerRear(),
    doorLockStatusDriver_presence(VDS_DEFAULT_PRESENCE),
    doorLockStatusDriver(),
    doorLockStatusPassenger_presence(VDS_DEFAULT_PRESENCE),
    doorLockStatusPassenger(),
    doorLockStatusDriverRear_presence(VDS_DEFAULT_PRESENCE),
    doorLockStatusDriverRear(),
    doorLockStatusPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    doorLockStatusPassengerRear(),
    doorGripStatusDriver_presence(VDS_DEFAULT_PRESENCE),
    doorGripStatusDriver(),
    doorGripStatusPassenger_presence(VDS_DEFAULT_PRESENCE),
    doorGripStatusPassenger(),
    doorGripStatusDriverRear_presence(VDS_DEFAULT_PRESENCE),
    doorGripStatusDriverRear(),
    doorGripStatusPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    doorGripStatusPassengerRear(),
    trunkOpenStatus_presence(VDS_DEFAULT_PRESENCE),
    trunkOpenStatus(),
    trunkLockStatus_presence(VDS_DEFAULT_PRESENCE),
    trunkLockStatus(),
    engineHoodOpenStatus_presence(VDS_DEFAULT_PRESENCE),
    engineHoodOpenStatus(),
    engineHoodLockStatus_presence(VDS_DEFAULT_PRESENCE),
    engineHoodLockStatus(),
    centralLockingStatus_presence(VDS_DEFAULT_PRESENCE),
    centralLockingStatus(),
    centralLockingDisStatus_presence(VDS_DEFAULT_PRESENCE),
    centralLockingDisStatus(),
    privateLockStatus_presence(VDS_DEFAULT_PRESENCE),
    privateLockStatus(),
    vehicleAlarm_presence(VDS_DEFAULT_PRESENCE),
    vehicleAlarm(),
    winCloseReminder_presence(VDS_DEFAULT_PRESENCE),
    winCloseReminder(),
    handBrakeStatus_presence(VDS_DEFAULT_PRESENCE),
    handBrakeStatus(),
    electricParkBrakeStatus_presence(VDS_DEFAULT_PRESENCE),
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

class MaintenanceStatus_Data: public AppDataBase
{
public:
    MaintenanceStatus_Data():
    mainBatteryStatus_presence(VDS_DEFAULT_PRESENCE),
    mainBatteryStatus(),
    tyreStatusDriver_presence(VDS_DEFAULT_PRESENCE),
    tyreStatusDriver(),
    tyreStatusPassenger_presence(VDS_DEFAULT_PRESENCE),
    tyreStatusPassenger(),
    tyreStatusDriverRear_presence(VDS_DEFAULT_PRESENCE),
    tyreStatusDriverRear(),
    tyreStatusPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    tyreStatusPassengerRear(),
    tyreTempDriver_presence(VDS_DEFAULT_PRESENCE),
    tyreTempDriver(),
    tyreTempPassenger_presence(VDS_DEFAULT_PRESENCE),
    tyreTempPassenger(),
    tyreTempDriverRear_presence(VDS_DEFAULT_PRESENCE),
    tyreTempDriverRear(),
    tyreTempPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    tyreTempPassengerRear(),
    engineHrsToService_presence(VDS_DEFAULT_PRESENCE),
    engineHrsToService(),
    distanceToService_presence(VDS_DEFAULT_PRESENCE),
    distanceToService(),
    daysToService_presence(VDS_DEFAULT_PRESENCE),
    daysToService(),
    serviceWarningStatus_presence(VDS_DEFAULT_PRESENCE),
    serviceWarningStatus(),
    serviceWarningTrigger_presence(VDS_DEFAULT_PRESENCE),
    serviceWarningTrigger(),
    engineOilLevelStatus_presence(VDS_DEFAULT_PRESENCE),
    engineOilLevelStatus(),
    engineOilTemperature_presence(VDS_DEFAULT_PRESENCE),
    engineOilTemperature(),
    engineOilPressureWarning_presence(VDS_DEFAULT_PRESENCE),
    engineOilPressureWarning(),
    engineCoolantTemperature_presence(VDS_DEFAULT_PRESENCE),
    engineCoolantTemperature(),
    engineCoolantLevelStatus_presence(VDS_DEFAULT_PRESENCE),
    engineCoolantLevelStatus(),
    brakeFluidLevelStatus_presence(VDS_DEFAULT_PRESENCE),
    brakeFluidLevelStatus(),
    washerFluidLevelStatus_presence(VDS_DEFAULT_PRESENCE),
    washerFluidLevelStatus(),
    indcrTurnLeWarning_presence(VDS_DEFAULT_PRESENCE),
    indcrTurnLeWarning(),
    indcrTurnRiWarning_presence(VDS_DEFAULT_PRESENCE),
    indcrTurnRiWarning(),
    indcrTurnOnPassSideWarning_presence(VDS_DEFAULT_PRESENCE),
    indcrTurnOnPassSideWarning(),
    indcrTurnOnDrvrSideWarning_presence(VDS_DEFAULT_PRESENCE),
    indcrTurnOnDrvrSideWarning(),
    indcrTurnLeFrntWarning_presence(VDS_DEFAULT_PRESENCE),
    indcrTurnLeFrntWarning(),
    indcrTurnRiFrntWarning_presence(VDS_DEFAULT_PRESENCE),
    indcrTurnRiFrntWarning(),
    fogLiReWarning_presence(VDS_DEFAULT_PRESENCE),
    fogLiReWarning(),
    bulbStatus_presence(VDS_DEFAULT_PRESENCE),
    bulbStatus()
    {
    }
    //-- Vehicle battery status;
    bool  mainBatteryStatus_presence;
    BatteryStatus_Data mainBatteryStatus;
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
    App_ServiceWarningTriggerReason serviceWarningTrigger;
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

class RunningStatus_Data: public AppDataBase
{
public:
    RunningStatus_Data():
    srsStatus_presence(VDS_DEFAULT_PRESENCE),
    srsStatus(),
    srsCrashStatus_presence(VDS_DEFAULT_PRESENCE),
    srsCrashStatus(),
    odometer_presence(VDS_DEFAULT_PRESENCE),
    odometer(),
    fuelLevel_presence(VDS_DEFAULT_PRESENCE),
    fuelLevel(),
    fuelLevelStatus_presence(VDS_DEFAULT_PRESENCE),
    fuelLevelStatus(),
    fuelEnLevel_presence(VDS_DEFAULT_PRESENCE),
    fuelEnLevel(),
    fuelEnCnsFild_presence(VDS_DEFAULT_PRESENCE),
    fuelEnCnsFild(),
    fuelEnCns_presence(VDS_DEFAULT_PRESENCE),
    fuelEnCns(),
    fuelLow1WarningDriver_presence(VDS_DEFAULT_PRESENCE),
    fuelLow1WarningDriver(),
    fuelLow2WarningDriver_presence(VDS_DEFAULT_PRESENCE),
    fuelLow2WarningDriver(),
    distanceToEmpty_presence(VDS_DEFAULT_PRESENCE),
    distanceToEmpty(),
    trvlDistance_presence(VDS_DEFAULT_PRESENCE),
    trvlDistance(),
    aveFuelConsumption_presence(VDS_DEFAULT_PRESENCE),
    aveFuelConsumption(),
    aveFuelConsumptionInLatestDrivingCycle_presence(VDS_DEFAULT_PRESENCE),
    aveFuelConsumptionInLatestDrivingCycle(),
    avgSpeed_presence(VDS_DEFAULT_PRESENCE),
    avgSpeed(),
    avgSpeedLatestDrivingCycle_presence(VDS_DEFAULT_PRESENCE),
    avgSpeedLatestDrivingCycle(),
    aveFuelConsumptionUnit_presence(VDS_DEFAULT_PRESENCE),
    aveFuelConsumptionUnit(),
    indFuelConsumption_presence(VDS_DEFAULT_PRESENCE),
    indFuelConsumption(),
    notifForEmgyCallStatus_presence(VDS_DEFAULT_PRESENCE),
    notifForEmgyCallStatus(),
    tyrePreWarningDriver_presence(VDS_DEFAULT_PRESENCE),
    tyrePreWarningDriver(),
    tyrePreWarningPassenger_presence(VDS_DEFAULT_PRESENCE),
    tyrePreWarningPassenger(),
    tyrePreWarningDriverRear_presence(VDS_DEFAULT_PRESENCE),
    tyrePreWarningDriverRear(),
    tyrePreWarningPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    tyrePreWarningPassengerRear(),
    tyreTempWarningDriver_presence(VDS_DEFAULT_PRESENCE),
    tyreTempWarningDriver(),
    tyreTempWarningPassenger_presence(VDS_DEFAULT_PRESENCE),
    tyreTempWarningPassenger(),
    tyreTempWarningDriverRear_presence(VDS_DEFAULT_PRESENCE),
    tyreTempWarningDriverRear(),
    tyreTempWarningPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    tyreTempWarningPassengerRear(),
    seatBeltStatusDriver_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusDriver(),
    seatBeltStatusPassenger_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusPassenger(),
    seatBeltStatusDriverRear_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusDriverRear(),
    seatBeltStatusPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusPassengerRear(),
    seatBeltStatusMidRear_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusMidRear(),
    seatBeltStatusThDriverRear_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusThDriverRear(),
    seatBeltStatusThPassengerRear_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusThPassengerRear(),
    seatBeltStatusThMidRear_presence(VDS_DEFAULT_PRESENCE),
    seatBeltStatusThMidRear(),
    brakePedalDepressed_presence(VDS_DEFAULT_PRESENCE),
    brakePedalDepressed(),
    gearManualStatus_presence(VDS_DEFAULT_PRESENCE),
    gearManualStatus(),
    gearAutoStatus_presence(VDS_DEFAULT_PRESENCE),
    gearAutoStatus(),
    engineSpeed_presence(VDS_DEFAULT_PRESENCE),
    engineSpeed(),
    transimissionGearPostion_presence(VDS_DEFAULT_PRESENCE),
    transimissionGearPostion(),
    cruiseControlStatus_presence(VDS_DEFAULT_PRESENCE),
    cruiseControlStatus(),
    engineBlockedStatus_presence(VDS_DEFAULT_PRESENCE),
    engineBlockedStatus(),
    tripMeter1_presence(VDS_DEFAULT_PRESENCE),
    tripMeter1(),
    tripMeter2_presence(VDS_DEFAULT_PRESENCE),
    tripMeter2()
    {
    }
    //-- SRS(airbag) status, deployed or non-deployed;
    bool  srsStatus_presence;
    App_SRSStatus srsStatus;
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
    App_EngineBlockedStatus engineBlockedStatus;
    //-- KM;
    bool  tripMeter1_presence;
    int32_t tripMeter1;
    //-- KM;
    bool  tripMeter2_presence;
    int32_t tripMeter2;

};

class ClimateStatus_Data: public AppDataBase
{
public:
    ClimateStatus_Data():
    interiorTemp_presence(VDS_DEFAULT_PRESENCE),
    interiorTemp(),
    exteriorTemp_presence(VDS_DEFAULT_PRESENCE),
    exteriorTemp(),
    preClimateActive_presence(VDS_DEFAULT_PRESENCE),
    preClimateActive(),
    airCleanSts_presence(VDS_DEFAULT_PRESENCE),
    airCleanSts(),
    drvHeatSts_presence(VDS_DEFAULT_PRESENCE),
    drvHeatSts(),
    passHeatingSts_presence(VDS_DEFAULT_PRESENCE),
    passHeatingSts(),
    rlHeatingSts_presence(VDS_DEFAULT_PRESENCE),
    rlHeatingSts(),
    rrHeatingSts_presence(VDS_DEFAULT_PRESENCE),
    rrHeatingSts(),
    drvVentSts_presence(VDS_DEFAULT_PRESENCE),
    drvVentSts(),
    passVentSts_presence(VDS_DEFAULT_PRESENCE),
    passVentSts(),
    rrVentSts_presence(VDS_DEFAULT_PRESENCE),
    rrVentSts(),
    rlVentSts_presence(VDS_DEFAULT_PRESENCE),
    rlVentSts(),
    interCO2Warning_presence(VDS_DEFAULT_PRESENCE),
    interCO2Warning(),
    fragStrs_presence(VDS_DEFAULT_PRESENCE),
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
    FragStrs_Data fragStrs;

};

class PollutionStatus_Data: public AppDataBase
{
public:
    PollutionStatus_Data():
    interiorPM25_presence(VDS_DEFAULT_PRESENCE),
    interiorPM25(),
    exteriorPM25_presence(VDS_DEFAULT_PRESENCE),
    exteriorPM25(),
    interiorPM25Level_presence(VDS_DEFAULT_PRESENCE),
    interiorPM25Level(),
    exteriorPM25Level_presence(VDS_DEFAULT_PRESENCE),
    exteriorPM25Level(),
    airQualityIndex_presence(VDS_DEFAULT_PRESENCE),
    airQualityIndex(),
    airParticleConcentration_presence(VDS_DEFAULT_PRESENCE),
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

class ElectricStatus_Data: public AppDataBase
{
public:
    ElectricStatus_Data():
    isCharging_presence(VDS_DEFAULT_PRESENCE),
    isCharging(),
    isPluggedIn_presence(VDS_DEFAULT_PRESENCE),
    isPluggedIn(),
    stateOfCharge_presence(VDS_DEFAULT_PRESENCE),
    stateOfCharge(),
    chargeLevel_presence(VDS_DEFAULT_PRESENCE),
    chargeLevel(),
    timeToFullyCharged_presence(VDS_DEFAULT_PRESENCE),
    timeToFullyCharged(),
    statusOfChargerConnection_presence(VDS_DEFAULT_PRESENCE),
    statusOfChargerConnection(),
    chargerState_presence(VDS_DEFAULT_PRESENCE),
    chargerState(),
    distanceToEmptyOnBatteryOnly_presence(VDS_DEFAULT_PRESENCE),
    distanceToEmptyOnBatteryOnly(),
    ems48VSOC_presence(VDS_DEFAULT_PRESENCE),
    ems48VSOC(),
    ems48VDisSOC_presence(VDS_DEFAULT_PRESENCE),
    ems48VDisSOC(),
    emsHVRBSError_presence(VDS_DEFAULT_PRESENCE),
    emsHVRBSError(),
    emsRBSModeDisplay_presence(VDS_DEFAULT_PRESENCE),
    emsRBSModeDisplay(),
    emsOVPActive_presence(VDS_DEFAULT_PRESENCE),
    emsOVPActive(),
    ems48vPackTemp1_presence(VDS_DEFAULT_PRESENCE),
    ems48vPackTemp1(),
    ems48vPackTemp2_presence(VDS_DEFAULT_PRESENCE),
    ems48vPackTemp2(),
    emsBMSLBuildSwVersion_presence(VDS_DEFAULT_PRESENCE),
    emsBMSLBuildSwVersion(),
    emsCBRemainingLife(),
    chargeHvSts_presence(VDS_DEFAULT_PRESENCE),
    chargeHvSts(),
    ptReady_presence(),
    ptReady(),
    averPowerConsumption_presence(VDS_DEFAULT_PRESENCE),
    averPowerConsumption(),
    indPowerConsumption_presence(VDS_DEFAULT_PRESENCE),
    indPowerConsumption(),
    dcDcActvd_presence(VDS_DEFAULT_PRESENCE),
    dcDcActvd(),
    dcDcConnectStatus_presence(VDS_DEFAULT_PRESENCE),
    dcDcConnectStatus(),
    dcChargeIAct_presence(VDS_DEFAULT_PRESENCE),
    dcChargeIAct(),
    dcChargeSts_presence(VDS_DEFAULT_PRESENCE),
    dcChargeSts(),
    wptObjt_presence(VDS_DEFAULT_PRESENCE),
    wptObjt(),
    wptFineAlignt_presence(VDS_DEFAULT_PRESENCE),
    wptFineAlignt(),
    wptActived_presence(VDS_DEFAULT_PRESENCE),
    wptActived(),
    wptChargeIAct_presence(VDS_DEFAULT_PRESENCE),
    wptChargeIAct(),
    wptChargeUAct_presence(VDS_DEFAULT_PRESENCE),
    wptChargeUAct(),
    wptChargeSts_presence(VDS_DEFAULT_PRESENCE),
    wptChargeSts(),
    chargeIAct_presence(VDS_DEFAULT_PRESENCE),
    chargeIAct(),
    chargeUAct_presence(VDS_DEFAULT_PRESENCE),
    chargeUAct(),
    chargeSts_presence(VDS_DEFAULT_PRESENCE),
    chargeSts(),
    disChargeConnectStatus_presence(VDS_DEFAULT_PRESENCE),
    disChargeConnectStatus(),
    disChargeIAct_presence(VDS_DEFAULT_PRESENCE),
    disChargeIAct(),
    disChargeUAct_presence(VDS_DEFAULT_PRESENCE),
    disChargeUAct(),
    disChargeSts_presence(VDS_DEFAULT_PRESENCE),
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

class TEMStatus_Data : public AppDataBase
{
public:
    TEMStatus_Data():
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
    App_PowerSource powerSource;
    App_TEMPowerMode powerMode;
    bool  state_presence;
    App_TEMState state;
    //-- refer to the TEM provision, to sync the status between TEM and CSP ;
    bool  serviceProvisoned_presence;
    bool serviceProvisoned;
    bool  healthStatus_presence;
    App_HealthStatus healthStatus;
    //-- for TEM DTC codes;
    bool  diagnostics_presence;
    ECUDiagnostic_Data diagnostics;
    bool  backupBattery_presence;
    BatteryStatus_Data backupBattery;
    //-- Thursday, 1 January 1970 means no information;
    bool  sleepCycleNextWakeupTime_presence;
    TimeStamp_Data sleepCycleNextWakeupTime;
    bool  serialNumber_presence;
    std::string serialNumber;
    bool  completeECUPartNumbers_presence;
    std::string completeECUPartNumbers;
    //-- the network access information;
    bool  networkAccessStatus_presence;
    NetworkAccessStatus_Data networkAccessStatus;
    bool  imei_presence;
    std::string imei;
    bool  connectivityStatus_presence;
    ConnectivityStatus_Data connectivityStatus;
    bool  hwVersion_presence;
    std::string hwVersion;
    bool  swVersion_presence;
    std::string swVersion;
    bool  mpuVersion_presence;
    std::string mpuVersion;
    bool  mcuVersion_presence;
    std::string mcuVersion;

};

class CarConfig_Data : public AppDataBase
{
public:
    CarConfig_Data():
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
    std::vector<ServiceParameter_Data> items;

};

class AdditionalStatus_Data : public AppDataBase
{
public:
    AdditionalStatus_Data():
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
    ConfigurationAndIdentity_Data confAndId;
    bool  ecuWarningMessages_presence;
    std::vector<GenericVehicleSignal_Data> ecuWarningMessages;
    bool  towStatus_presence;
    TowStatus_Data towStatus;
    bool  signals_presence;
    std::vector<GenericVehicleSignal_Data> signals;
    //--Scope:OCTET STRING SIZE 1..65535;
    bool  data_presence;
    std::string data;

};

class OEMMSD_Data : public AppDataBase
{
public:
    OEMMSD_Data():
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
    Position_Data position;
    //-- vehicle type, e.g. passenger vehicle, bus, cargo vehicle, motor cycle, etc, in  CSP this is passenger vehicle as always;
    bool  vehicleType_presence;
    App_VehicleType vehicleType;
    //--Scope:VIN;
    bool  vin_presence;
    std::string vin;
    App_FuelType fuelType;
    //-- 0 means North, clockwise Scope:INTEGER 0..360;
    uint16_t direction;
    //--Scope:INTEGER 0..64;
    bool  numberOfPassengers_presence;
    uint8_t numberOfPassengers;
    bool  recentPositions_presence;
    std::vector<Position_Data> recentPositions;
    //-- kmph;
    bool  speed_presence;
    int32_t speed;
    //-- 0x0: Invalid, 0x1: valid Scope:INTEGER 0..1;
    bool  speedValidity_presence;
    uint8_t speedValidity;
    //-- The optional additional data;
    bool  signals_presence;
    std::vector<GenericVehicleSignal_Data> signals;

};

class TheftNotification_Data : public AppDataBase
{
public:
    TheftNotification_Data():
    time(),
    activated()
    {
    }
    TimeStamp_Data time;
    //-- To indicate the reason of the theft notification trigger, e.g. doors/trunk/hood open, tow away;
    int32_t activated;

};

class RVDC_Data : public AppDataBase
{
public:
    RVDC_Data();
    virtual ~RVDC_Data();

public:
    long serviceId;
    long serviceCommand;
    //-- Which platform to use OTA Server;
    App_AppId appId;
    //-- The orderId is generated by the command initiator, and the responder needs to response it to the initiator.	 Scope:PrintableString SIZE 1..512;
    std::string maOrderId;
    //-- The client recorded time of the notification Scope:PrintableString SIZE 1..32;
    std::string timestamp;
    //-- Vehicle identification code  Scope:PrintableString SIZE 1..128;
    std::string vin;
    //-- the business type of the current operation;
    App_MaBusinessType maBusinessType;
    bool  rvdcSetting_presence;
    RvdcSetting_Data rvdcSetting;
    bool  assignmentNotification_presence;
    RvdcAssignmentNotification_Data assignmentNotification;
    bool  maSynchronizationReq_presence;
    MaSynchronizationReq_Data maSynchronizationReq;
    //-- Support vehicle request to cloud for response and cloud to vehicle for push;
    bool  maSynchronization_presence;
    MaSynchronization_Data maSynchronization;
    bool  exceptionHandling_presence;
    RvdcExceptionHandling_Data exceptionHandling;
    bool  measurementDataPackage_presence;
    MeasurementDataPackage_Data measurementDataPackage;

};

//MTA/////////////////////////////////////
const int MTA_ITEM_MAX_SIZE = 32; // reference asn, buffer is 32 byte

enum MTAPowerMode
{   MTA_Power_Mode_Normal = 0,
    MTA_Power_Mode_Standby = 1,
    MTA_Power_Mode_Sleep_Poll = 2,
    MTA_Power_Mode_Power_Off = 3
};

enum MTAPowerSource
{    MTA_Power_Sourece_Main_Battery = 0,
     MTA_Power_Sourece_Backup_Battery = 1
};

class MTAData_t: public AppDataBase
{
public:
    std::string  vin;                // vehicleStatus->temStatus->vin
    std::string  ecu_serial_number;  // vehicleStatus->temStatus->serialNumber
    std::string  msisdn;             // vehicleStatus->temStatus->networkAccessStatus->SimInfo->msisdn
    std::string  imsi;               // vehicleStatus->temStatus->networkAccessStatus->SimInfo->imsi
    std::string  icc_id;             // vehicleStatus->temStatus->networkAccessStatus->SimInfo->iccId
    std::string  mobile_network;     // vehicleStatus->temStatus->networkAccessStatus->mobileNetwork  // -- 2G/3G/4G/wifi
    MTAPowerSource  power_source;    // mainBattery/backupBattery
    MTAPowerMode  power_mode;        // normal/standby/sleep_poll/off
} ;

///////////////////////////////////////

//RMC////////////////////////////////////
enum RHLType
{
    None = 0, ///< Newly created
    Honk,
    Flash,
    HonkAndFlash
};

enum RWSType
{
    //    None = 0, ///< Newly created
    CloseWindowOnly = 1,
    CloseSunroofOnly,
    CloseBoth,
    OpenWindowOnly,
    OpenSunroofOnly,
    OpenBoth,
    OpenSunshade,
    CloseSunshade,
    Ventilate
};

enum DoorOpenCloseStatusType
{
    DoorStsUnknown = 0,
    DoorStsOpen,
    DoorStsClose
};

enum WinOpenCloseStatusType
{
    WinStsUnknown = 0,
    WinStsOpened,
    WinStsClosed,
    WinStsRaised,
    WinStsFullyOpend
};

class RemoteCtrlBasicRequest : public AppDataBase
{
public:
    RemoteCtrlBasicRequest();
    virtual ~RemoteCtrlBasicRequest();
//private:
    long serviceId;
    long serviceCommand;
    long timeStart;
    long timeWindow;
    uint64_t    requestType;
    uint32_t    doorValue;
    uint32_t    targetValue;
    int8_t      value;
    int8_t      pos;
};

class RemoteCtrlPosition : public AppDataBase
{
public:
    RemoteCtrlPosition();
    virtual ~RemoteCtrlPosition();
//private:
    long latitude;
    long longitude;
    long altitude;
    int posCanBeTrusted; //BOOLEAN_t
    int carLocatorStatUploadEn; //BOOLEAN_t
    int marsCoordinates; //BOOLEAN_t
};

class RemoteCtrlBasicStatus : public AppDataBase
{
public:
    RemoteCtrlBasicStatus();
    virtual ~RemoteCtrlBasicStatus();
//private:
    long serviceId;
    RemoteCtrlPosition position;
    long vehSpdIndcd;       //kmph
    long vehSpdIndcdQly;    //0x0:valid; 0x1:Invalid

    uint8_t doorOpenStatusDriver;
    uint8_t doorOpenStatusPassenger;
    uint8_t doorOpenStatusDriverRear;
    uint8_t doorOpenStatusPassengerRear;

    uint8_t doorLockStatusDriver;
    uint8_t doorLockStatusPassenger;
    uint8_t doorLockStatusDriverRear;
    uint8_t doorLockStatusPassengerRear;

    uint8_t trunkOpenStatus;
    uint8_t trunkLockStatus;
    uint8_t ventilateStatus;
    bool hashoodLockSts;

    uint8_t centralLockingStatus;
    uint8_t hoodSts;         //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t hoodLockSts;         //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t lockgCenStsForUsrFb; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    
    uint8_t winStatusDriver;
    uint8_t winStatusPassenger;
    uint8_t winStatusDriverRear;
    uint8_t winStatusPassengerRear;

    uint8_t winStatusDriverWarning;
    uint8_t winStatusPassengerWarning;
    uint8_t winStatusDriverRearWarning;
    uint8_t winStatusPassengerRearWarning;

    uint8_t sunroofOpenStatus;
    uint8_t curtainOpenStatus;

    long sunroofPos;

    long curtainPos;
    long winPosDriver;
    long winPosPassenger;
    long winPosDriverRear;
    long winPosPassengerRear;

    uint64_t interiorPM25;
    uint64_t exteriorPM25;
    uint8_t  interiorPM25Level;
    uint8_t  exteriorPM25Level;
};

class RemoteCtrlBasicResult : public AppDataBase
{
public:
    RemoteCtrlBasicResult();
    virtual ~RemoteCtrlBasicResult();
//private:
    long serviceId;
    int operationSucceeded;
    long errorCode;
    long vehicleErrorCode;
    std::string message;

};

////////////remote start, add by uia93888////////////////////
//remote engine
class RMT_EngineRequest_t: public AppDataBase
{
public:
    long serviceId;
    long serviceCommand;
    long interval;
};

class RMT_EngineStartResult_t: public AppDataBase
{
public:
    int operationSucceeded;  //0: failed, 1: success
    App_EngineStatus engine_status;
    long speed;
    long doorLockStatusPassenger;
    long doorOpenStatusPassenger;
    long doorLockStatusDriverRear;
    long doorOpenStatusDriverRear;
    long doorLockStatusDriver;
    long doorOpenStatusDriver;
    long doorLockStatusPassengerRear;
    long doorOpenStatusPassengerRear;
    long trunkLockStatus;  //can value: 1,2   map asn value: 0,1
    long trunkOpenStatus;
    long engineHoodOpenStatus;
    long centralLockingStatus;
    long engineOilLevelStatus;
    long electricParkBrakeStatus;
    long fuelLevel;
    long fuelLevelStatus;
};

class RMT_EngineStopResult_t: public AppDataBase
{
public:
    int operationSucceeded;  //0: failed, 1: success
    long usageMode;
    App_EngineStatus engine_status;
    long speed;
};

class RMT_EngineErrorResult_t: public AppDataBase
{
public:
    int operationSucceeded;  //0: failed, 1: success
    long errorCode;
    long vehicleErrorCode;
    std::string message;

};

class RMT_EngineNotify_t: public AppDataBase
{
public:
    App_EngineStatus engine_status;
    long speed;
};

class RMT_CliamtizationRequest_t: public AppDataBase
{
public:
    long serviceId;
    long serviceCommand;
    long interval;
    int32_t rec_temp = -1;   // remote climate
    int32_t rec_Level = -1;  // remote level
    int32_t rec_heat = -1;   // remote heat(ventilation or steering wheel)
    int32_t rec_venti = -1;  // remote ventilation
};

class RMT_ClimateSrtStopResult_t: public AppDataBase
{
public:
    int operationSucceeded;
    App_EngineStatus engine_status;
    long speed;
    long chargeHvSts;
};

class RMT_CliamteAutoStopResult_t: public AppDataBase
{
public:
    int operationSucceeded;
    int preClimateActive;
};

class RMT_RemoteStartCommErrorResult_t: public AppDataBase
{
public:
    int operationSucceeded;  //0: failed, 1: success
    long errorCode;
    long vehicleErrorCode;
    std::string message;
};

//Parking clamite
class Parking_ClimateTimerInfo: public AppDataBase
{
public:
    int timerActivation; //1: activate, 0: inactivate
    int duration;   //default 15mins
    std::string dayofWeek;
    std::string startTimeofDay; //00:00
};

class Parking_ClimateRequest_t: public AppDataBase
{
public:
    //comm
    long serviceId;
    int serviceCommand; //0 1 2 3 4 20
    int timerId;  //range 0 ~ 32
    //start
    std::vector<Parking_ClimateTimerInfo> timers;
    int rec_temp = -1;
    int rec_Level = -1;
    std::vector<int> rec_heat;
};

class Parking_CliamteStrtStopResult_t: public AppDataBase
{
public:
    int operationSuccessed;
    bool preClimateActive;
};

typedef fsm::RMT_RemoteStartCommErrorResult_t Parking_ErrorResult_t;


//////////remote start, add by uia93888 end//////////////////


//DLAgent/////////////////////////////////
class FOTA_Data : public AppDataBase
{
public:
    App_ServiceCommand serviceCommand;

    //-- Which platform to use OTA Server;
    App_AppId appId;
    //-- Vehicle identification code  Scope:PrintableString SIZE 1..128;
    std::string vin;
    App_DataType dataType;
    //-- provide Client Configuration to OTA server.;
    bool  clientConfSyncRequest_presence;
    ClientConfSyncRequest_Data clientConfSyncRequest;
    //-- The objective of this interface is to provide Inventory Instruction to vehicle;
    bool  inventoryInstruction_presence;
    InventoryInstruction_Data inventoryInstruction;
    //-- The objective of this interface is to request available assignment from OTA server.;
    bool  availableAssignmentRequest_presence;
    AvailableAssignmentRequest_Data availableAssignmentRequest;
    //-- The objective of this interface is to provide available assignment to vehicle.;
    bool  availableAssignment_presence;
    AvailableAssignment_Data availableAssignment;
    //-- The objective of this interface is to provide Installation Instructions to vehicle;
    bool  installationInstruction_presence;
    InstallationInstruction_Data installationInstruction;
    //-- The objective of this interface is to provide Installation Summary to OTA server.;
    bool  installationSummary_presence;
    InstallationSummary_Data installationSummary;
    //-- The objective of this interface is to provide Exception Reports to OTA server;
    bool  ecuExceptionReport_presence;
    EcuExceptionReport_Data ecuExceptionReport;
    //-- Due to fleet management and administrative reasons, the backend may lock certain vehicle settings in order to force a setting value for specific vehicles;
    bool  clientHmiVehicleSettings_presence;
    ClientHmiVehicleSettings_Data clientHmiVehicleSettings;
    //-- The objective of this interface is to provide assignment data to vehicle	;
    bool  assignmentData_presence;
    AssignmentData_Data assignmentData;
    bool  assignmentNotification_presence;
    FotaAssignmentNotification_Data assignmentNotification;
    bool  otaAssignmentFileInfo_presence;
    OtaAssignmentFileInfo_Data otaAssignmentFileInfo;

    // constructor
    FOTA_Data():
    serviceCommand(),
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
};

//DLAgent/////////////////////////////////

} // end namespace fsm

//CALL/////////////////////////////////////

class CallRequest_t: public AppDataBase
{
public:
    CallRequest_t():
    serviceId(),
    serviceCommand(),
    etavalue()
    {
    }
    long serviceId;
    long serviceCommand;
    std::vector<int> etavalue;
} ;

class CallServiceResult_t: public AppDataBase
{
public:
    CallServiceResult_t():
    latitude(),
    longitude(),
    altitude(),
    posCanBeTrusted(),
    carLocatorStatUploadEn(),
    marsCoordinates(),
    automaticActivation(),
    testCall(),
    fuelType(),
    vin(),
    direction()
    {
    }
    int latitude;
    int longitude;
    int altitude;
    bool posCanBeTrusted;
    bool carLocatorStatUploadEn;
    bool marsCoordinates;
    bool automaticActivation;
    bool testCall;
    App_FuelType fuelType;
    std::string vin;
    unsigned int direction;
};

//CALL/////////////////////////////////////

//Theft/////////////////////////////////////

class TheftRequest_t: public AppDataBase
{
public:
    TheftRequest_t():
    serviceId()
    {
    }
    long serviceId;
} ;

class TheftServiceResult_t: public AppDataBase
{
public:
    TheftServiceResult_t():
    seconds(),
    milliseconds(),
    activated()
    {
    }
    uint32_t seconds = 0;
    uint16_t milliseconds = 0;
    int32_t activated = 0;
};

//Theft/////////////////////////////////////

//JourneyLog/////////////////////////////////////

class JourneyLogRequest_t: public AppDataBase
{
public:
};
class App_TimeStamp{
public:
    App_TimeStamp():
    seconds(),
    milliseconds()
    {
    }
    int seconds;
    int milliseconds;
};
class App_Position{
public:
    App_Position():
    latitude(),
    longitude(),
    altitude(),
    posCanBeTrusted(),
    carLocatorStatUploadEn(),
    marsCoordinates()
    {
    }
    long   latitude;
    long   longitude;
    long   altitude;
    bool   posCanBeTrusted;
    bool   carLocatorStatUploadEn;
    bool   marsCoordinates;
};
class JourneyLogServiceResult_t: public AppDataBase
{
public:
    JourneyLogServiceResult_t():
    tripId(),
    startTime(),
    startOdometer(),
    fuelConsumption(),
    electricConsumption(),
    traveledDistance(),
    waypoints(),
    avgSpeed(),
    endTime(),
    endOdometer()
    {
    }
    long   tripId;
    App_TimeStamp   startTime;
    long   startOdometer;
    long   fuelConsumption;
    long electricConsumption;
    long   traveledDistance;
    std::list<App_Position>   waypoints;
    long   avgSpeed;
    App_TimeStamp   endTime;
    long   endOdometer;
};

//JourneyLog/////////////////////////////////////


#endif
