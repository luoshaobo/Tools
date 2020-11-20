#ifndef VUC_MESSAGES_HPP
#define VUC_MESSAGES_HPP

/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vc_messages.hpp
 * This file defines all messages (and related stuff) used in VehicleComm
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

namespace vc {

#pragma pack(1) // exact fit - no padding

// Use a more appropriate name than the one from ipc_api_define.h
typedef EIPCmessageType VuCMessageID;

// VUC MESSAGES  IN-------------------------------------------------------------

// Maps to IPC_VUC_HORN_LIGHT_STATE_REP (SIpcHornULightStateType)
/** @brief Horn and light event
    hornUlight:     0 - No request for Horn&Light
                    1 - Request for Horn
                    2 - Request for Light
                    3 - Request for Horn and Light
    errCode:        0 - no error
                    1 - Horn&Light sequence already in progress
                    2 - Usage mode not Abandoned, Inactive or Convinience
    respType:       0 - response to NAD request
                    1 - event 
    carLocatorSts   0: Activation_successful 
                    1: System_1_fail 
                    2: System_2_fail 
                    3: Idle
*/
typedef struct
{
    uint8_t hornUlight  : 4;
    uint8_t errCode     : 4;
    uint8_t respType    : 4;
    uint8_t carLocatorSts    : 4;
} VucHornULightStateType;


// Maps to IPC_VUC_VEHICLE_GEN_STATE_REP (SIpcGenStateReportType)
// Maps to IPC_VUC_VEHICLE_GEN_STATE_RESPONSE_RESP (SIpcVehicleGenStateResponseRespType)
/** @brief Vehicle state
    vehicleModeMngtGlobalSafeUseMode:   0 - Abandoned
                                        1 - Inactive
                                        2 - UsgModCnvinc
                                        11 - Active
                                        13 - Driving
    vehicleModeMngtGlobalSafe:          0 - Normal
                                        1 - Transport
                                        2 - CarModFcy
                                        3 - Crash
                                        5 - Dyno
    volvoSpecWakeupReason               0 = See platform message “System State Information” field “Wakeup Reason”
                                        1 = BLE_WAKEUP
                                        2 = ECALL_WAKEUP
                                        3 = RKE_WAKEUP */
typedef struct
{
    uint8_t vehicleModeMngtGlobalSafeUseMode :4; /* Byte 1 */
    uint8_t vehicleModeMngtGlobalSafe :3;
    uint8_t reserved1 :1;

    uint8_t volvoSpecWakeupReason : 4;           /* Byte 2 */
    uint8_t reserved2 : 4;
} VucVehGenStateReportType, VucVehGenStateRespType;


// Maps to IPC_VUC_SOFTWARE_BUILD_DATE_REP (SIpcSwBuildDateRepType)
/** @brief VUC Software build date
    Example of timestamp:
    ASW_BUILD_DAY             0x10
    ASW_BUILD_MONTH           0x11
    ASW_BUILD_YEAR_HI         0x20
    ASW_BUILD_YEAR_LO         0x16
    ASW_BUILD_HOUR            0x14
    ASW_BUILD_MINUTE          0x28
    ASW_BUILD_SECOND          0x33
    The time from this 7 Bytes is 10 November 2016 at 14:28:33 */

typedef struct
{
    uint8_t timestamp[7];
    uint8_t checksum[2];
    uint8_t sw_version[2];
} VucSwBuildDateRepType;


// Maps to IPC_VUC_BLE_SOFTWARE_BUILD_DATE_REP (SIpcBleSwBuildDateRepType)
/** @brief BLE Software build date
    Example of timestamp:
    ASW_BUILD_DAY             0x10
    ASW_BUILD_MONTH           0x11
    ASW_BUILD_YEAR_HI         0x20
    ASW_BUILD_YEAR_LO         0x16
    ASW_BUILD_HOUR            0x14
    ASW_BUILD_MINUTE          0x28
    ASW_BUILD_SECOND          0x33
    The time from this 7 Bytes is 10 November 2016 at 14:28:33 */

typedef struct
{
    uint8_t timestamp[7];
    uint8_t checksum[2];
    uint8_t sw_version[2];
    uint8_t stack_version[3];
} VucBleSwBuildDateRepType;


// Maps to IPC_VUC_TRACE_REP (SIpcVucTraceType)
/** @brief Vuc trace
    messageName     0 = TRACE_ENTRY
                    1 = TRACE_CONFIG
                    2 = TRACE_CONTROL
                    3-7 = Not used
    type            0 = Report
                    1 = Reponse
                    2 = Request
                    3 = Not Used
    priority        0 = highest priority
                    ...
                    7 = lowest priority
    fcodeid         0x00...0xFF
    payload         ASCII description of trace report entry (17 bytes) */

typedef struct
{
    uint8_t messageName :3;
    uint8_t type :2;
    uint8_t priority :3;

    uint8_t fcodeid;
    uint8_t payload[20];
} VucNadTraceType;

// Maps to IPC_VUC_ELECT_ENG_LVL_REP (SIpcElectEngLvlRepType)
// Maps to IPC_VUC_ELECT_ENG_LVL_RESPONSE_RESP (SIpcElectEngLvlResponseRespType)
typedef struct
{
    uint8_t mainData : 4;
    uint8_t subData : 4;
} VucElectEngLvlRepType, VucElectEngLvlRespType;

// Maps to IPC_VUC_BUB_STATE_REP (SIpcBubStateType)
// Maps to IPC_VUC_BUB_STATE_RESPONSE_RESP (SIpcBubStateResponseRespType)
typedef struct
{
    uint8_t battUsed : 2;
    uint8_t state : 6;
    uint8_t chagLvl;
    uint8_t bubTemp;
    uint8_t ambTemp;
    uint16_t battVoltage;
    uint16_t kl30Voltage;

} VucBubStateRepType, VucBubStateRespType;

// Maps to IPC_VUC_PROGRAM_PRECOND_CHK_RESP 
#pragma pack(push)
#pragma pack(1)
typedef struct{
    uint16_t        numData;
    uint8_t          value;
}VucparaInvalidCfg;
typedef struct
{
    uint8_t            numInvalidCfg;
    paraInvalidCfg   parameterFault[11];
}VucCarCfgFaultsType;
typedef struct{
    uint8_t nmHistoryStateLast1;            /* Byte 1 */
    uint32_t globalRealTimeStateLast1;      /* Byte 2 - 5 */
    uint8_t nmHistoryStateLast2;            /* Byte 6 */
    uint32_t globalRealTimeStateLast2;      /* Byte 7 - 10 */
    uint8_t nmHistoryStateLast3;            /* Byte 11 */
    uint32_t globalRealTimeStateLast3;      /* Byte 12 - 15 */
    uint8_t nmHistoryStateLast4;            /* Byte 16 */
    uint32_t globalRealTimeStateLast4;      /* Byte 17 - 20 */
    uint8_t nmHistoryStateLast5;            /* Byte 21 */
    uint32_t globalRealTimeStateLast5;      /* Byte 22 - 25 */
    uint8_t nmHistoryStateLast6;            /* Byte 26 */
    uint32_t globalRealTimeStateLast6;      /* Byte 27 - 30 */
    uint8_t nmHistoryStateLast7;            /* Byte 31 */
    uint32_t globalRealTimeStateLast7;      /* Byte 32 - 35 */
    uint8_t nmHistoryStateLast8;            /* Byte 36 */
    uint32_t globalRealTimeStateLast8;      /* Byte 37 - 40 */
    uint8_t nmHistoryStateLast9;            /* Byte 41 */
    uint32_t globalRealTimeStateLast9;      /* Byte 42 - 45 */
    uint8_t nmHistoryStateLast10;           /* Byte 46 */
    uint32_t globalRealTimeStateLast10;     /* Byte 47 - 50 */
}ResNMHistoryType;
typedef struct{
   uint8_t UsgModSts;                            /*usage mode*/
   uint16_t VehSpdLgt;                            /*vehicle speed*/
   uint8_t KL30Sts;                              /*voltage status */
} VucProgramPrecond_Chk;
#pragma pack(pop)


// Maps to IPC_VUC_DIAG_ERR_REP (SIpcVucDiagErrRepType)
// Maps to IPC_VUC_DIAG_ERR_RESPONSE_RESP (SIpcVucDiagErrResponseRespType)
typedef struct
{
    uint8_t diagPhoneAntOpenLoadTestFailed      : 1;
    uint8_t diagPhoneAntOpenLoadTestCompleted   : 1;
    uint8_t diagEcallAntOpenLoadTestFailed      : 1;
    uint8_t diagEcallAntOpenLoadTestCompleted   : 1;
    uint8_t reserved                            : 4;
} VucDiagErrRepType, VucDiagErrRespType;

// Maps to IPC_VUC_ANTENNA_SWITCH_RESP (SIpcAntennaSwitchRespType)
typedef struct
{
    uint8_t swControl       : 2;          /* Byte 1 */
    uint8_t reserved        : 6;
} VucAntennaSwitchRespType;

// Maps to IPC_VUC_GLOBAL_SS_DATA_DID_RESP (SIpcGlobalSSDataDidRespType)
typedef struct
{
        uint8_t DD00[4];    /* Byte 1 - 4 */
        uint8_t DD01[3];    /* Byte 5 - 7 */
        uint8_t DD02;       /* Byte 8 */
        uint8_t DD0A;       /* Byte 9 */
        uint8_t DD0C;       /* Byte 10 */
 } VucGlobalSSDataDidRespType;

// Maps to IPC_VUC_CAR_CONFIG_REP (SIpcCarConfRespType)
typedef struct
{
    uint16_t paramId     : 14;
    uint16_t validity    : 2;
    uint8_t ccp;
} VucCarConfRespType;

// Maps to IPC_VUC_BLE_DISCONNECT_RESP (SIpcBleDisconnectRespType)
typedef struct
{
    uint8_t status;
} VucBleDisconnectRespType;

// Maps to IPC_VUC_BLE_DISCONNECT_ALL_RESP (SIpcBleDisconnectAllRespType)
typedef struct
{
    uint8_t status;             /* Byte 1 */
} VucBleDisconnectAllRespType;

// Maps to IPC_VUC_BLE_MTU_SIZE_REP (SIpcBleMtuSizeRepType)
typedef struct
{
    uint16_t connectionID       : 14;
    uint16_t reserved           : 2;
    uint8_t mtuSize;
} VucBleMtuSizeRepType;

// Maps to IPC_VUC_BLE_CONNECTION_STATUS (SIpcBleConnectionStatusType)
typedef struct
{
    uint16_t connectionID       : 14;
    uint16_t connectionStatus   : 2;
} VucBleConnStatusRepType;

// Maps to IPC_VUC_BLE_GENERATE_BDAK_RESP (SIpcBleGenerateBDAKRespType)
typedef struct
{
    uint8_t status;
} VucBleGenBdakRespType;

// Maps to IPC_VUC_BLE_GET_BDAK_RESP (SIpcBleGetBDAKRespType)
typedef struct
{
    uint8_t status;
    uint8_t bdak[16];
} VucBleGetBdakRespType;

// Maps to IPC_VUC_BLE_CONFIG_DATACOM_SERVICE_RESP (SIpcBleConfigDatacomServiceRespType)
typedef struct
{
    uint8_t status;
} VucBleCfgDataCommServRespType;

// Maps to IPC_VUC_BLE_CONFIG_IBEACON_RESP (SIpcBleConfigIBeaconRespType)
typedef struct
{
    uint8_t status;
} VucBleCfgIbeaconRespType;

// Maps to IPC_VUC_BLE_CONFIG_GENERIC_ACCESS_RESP (SIpcBleConfigGenericAccessRespType)
typedef struct
{
    uint8_t status;
} VucBleCfgGenAccRespType;

// Maps to IPC_VUC_BLE_START_PAIRING_SERVICE_RESP (SIpcBleStartPairingServiceRespType)
typedef struct
{
    uint8_t status;
} VucBleStartPairServRespType;

// Maps to IPC_VUC_BLE_STOP_PAIRING_SERVICE_RESP (SIpcBleStopPairingServiceRespType)
typedef struct
{
    uint8_t status;
} VucBleStopPairServRespType;

// Maps to IPC_VUC_BLE_START_ADVERTISING_RESP (SIpcBleStartAdvertisingRespType)
typedef struct
{
    uint8_t status;
} VucBleStartAdvRespType;

// Maps to IPC_VUC_BLE_STOP_ADVERTISING_RESP (SIpcBleStopAdvertisingRespType)
typedef struct
{
    uint8_t status;
} VucBleStopAdvRespType;

/* Maps to IPC_VUC_DTC_HANDLING_RESP  (SIpcDtcHandlingResponseType)*/
typedef struct
{
    uint8_t handlingResponse;
} VucDtcHandlingRespType;

/* Maps to IPC_VUC_FULL_DTC_ENTRY_REP (SIpcFullDtcEntryReportType) */
typedef struct
{
    uint8_t eventIdentifier;
    uint8_t nofEvents;
    uint8_t hexCode[3];
    uint8_t statusBits;
    uint8_t extendedData10;
    uint8_t extendedData01;
    uint8_t extendedData02;
    uint8_t extendedData03;
    uint8_t extendedData04;
    uint8_t extendedData06;
    uint8_t extendedData07;
    uint8_t extendedData11;
    uint8_t extendedData12;
    uint8_t extendedData20[4];
    uint8_t extendedData21[4];
    uint8_t extendedData30;
    uint8_t snapshotData20[10];
} VucFullDtcEntryReportType;

/* Maps to IPC_VUC_DTC_STATUS_ENTRY_REP (SIpcDtcStatusEntryReportType) */
typedef struct
{
    uint8_t eventIdentifier;
    uint8_t statusBits;
    uint8_t extendedData10;
    uint8_t extendedData11;
    uint8_t extendedData12;
    uint8_t extendedData30;
} VucDtcStatusEntryReportType;

/* Meps to IPC_VUC_AMPLIFIER_STATE (SIpcAmplifierStateType) */
typedef struct
{
    uint8_t amplifierState  : 1;
    uint8_t amplifierError  : 1;
    uint8_t reserved        : 6;
} VucAmplifierStateType;

/* Maps to IPC_VUC_RF_KLINE_ERROR_CODES (SIpcRfKlineErrorCodesType) */
typedef struct
{
    uint8_t rfErrorCodes;
    uint8_t klineErrorCodes;
} VucRfKlineErrorCodesType;

/* Maps to IPC_VUC_POWER_MODE_REP (SIpcPowerModeRepType) */
typedef struct
{
    uint8_t powerMode;
    uint16_t wakeupReason;
} VucPowerModeRepType;

/* Maps to IPC_VUC_OHC_BUTTON_STATE_REP (SIpcOHCButtonStateRepType) */
/* Maps to IPC_VUC_OHC_BUTTON_STATE_RESPONSE_RESP (SIpcOHCButtonStateResponseRespType) */
typedef struct
{
    uint8_t ecallRequest    : 4;
    uint8_t icallRequest    : 4;
} VucOHCButtonStateRepType, VucOHCButtonStateRespType;

/* Maps to IPC_VUC_VIN_REP (SIpcVINRepType) */
typedef struct
{
    uint8_t vin[17]; /* Byte 1 - 17 */
} VucVINRepType;

/* Maps to IPC_VUC_LOCAL_CONFIG_PARAM_REQ (SIpcLocalConfigParamReqType) */
typedef enum {
    LCFG_TIMETOWAITFORIPLMAFTERBOOT = 1,
    LCFG_TIMERDOOROPENUSAGEMODE,
    LCFG_STANDBYMODETIMER,
    LCFG_POLLINGMODETIMER,
    LCFG_RESOURCEGROUPMEMBER,
    LCFG_PSIM_DISABLED,
    LCFG_ERS_VFC_TIMEOUT,
    LCFG_ERS_TIMEOUT_TIME,
    LCFG_ERS_RESPONSE_TIME,
    LCFG_ERS_RESPONSE_VALID_TIME,
    LCFG_ERS_STATUS_UPDATE_TIME,
    LCFG_CLHAL_VFC_TIMEOUT,
    LCFG_CLIMA_REQ_TIMEOUT,
    LCFG_CLIMA_WARN_STATE_TIME,
    LCFG_RPC_VFC_TIMEOUT,
    LCFG_DASHBOARD_SEND_DELAY,
    LCFG_RDU_VFC_TIMEOUT,
    LCFG_LOCK_VALID_STATUS_TIME,
    LCFG_LOCK_STATUS_UPDATE_TIME,
    LCFG_UNLOCK_CHECK_TIME,
    LCFG_GSD_VFC_TIMEOUT,
    LCFG_DISABLE_WORKSHOP_WIFI_MOD,
    LCFG_WIFI_FREQ_BAND,
    LCFG_WIFI_FUNCTION_CONTROL,
    LCFG_V_SCAN_FULL,
    LCFG_V_SCAN_LIMITED,
    LCFG_SLEEP_TIME,
    LCFG_RECEIVE_TIME,
    LCFG_BLE_ADV_INTERVAL_ACTIVE,
    LCFG_BLE_ADV_INTERVAL_STANDBY,
    LCFG_BLE_ADV_INTERVAL_POLLING,
    LCFG_BLE_ADV_INTERVAL_NETOFF,
    LCFG_BLE_ADV_BROADCAST_ACTIVE,
    LCFG_BLE_ADV_BROADCAST_STANDBY,
    LCFG_BLE_ADV_BROADCAST_POLLING,
    LCFG_BLE_ADV_BROADCAST_NETOFF,
    LCFG_BLE_TX_POWERLEVEL_ACTIVE,
    LCFG_BLE_TX_POWERLEVEL_STANDBY,
    LCFG_BLE_TX_POWERLEVEL_POLLING,
    LCFG_BLE_TX_POWERLEVEL_NETOFF,
    LCFG_BLE_TX_MEASURED_POWER_ACTIVE,
    LCFG_BLE_TX_MEASURED_POWER_STANDBY,
    LCFG_BLE_TX_MEASURED_POWER_POLLING,
    LCFG_BLE_TX_MEASURED_POWER_NETOFF,
    LCFG_BLE_CHECK_CLIENT_AUTHENTICATION_TIME,
    LCFG_PNCBITS_CHECK_PERIOD,
    LCFG_SLEEP_POLL_CYCLE2,
    LCFG_POLLING_INTERVAL2,
    LCFG_MAX
} VucLocalConfigParam;

typedef struct
{
    uint16_t localCfgParam;
} VucLocalConfigParamReqType;

// Maps to IPC_VUC_PSIM_DATA_COMMUNICATION_RESP (SIpcPSimDataCommunicationRespType)
typedef struct
{
    uint8_t payload[256];  /* Byte 1 - 256 */
} VucPSimDataCommunicationRespType;

// Maps to IPC_VUC_MESSAGE_CATALOGUE_VERSION_RESP (SIpcVucMsgCatalogVerType)
typedef struct
{
    uint8_t vucMsgCatalogByte1;
    uint8_t vucMsgCatalogByte2;
    uint8_t vucMsgCatalogByte3;
} VucMsgCatalogVerType;


// Maps to IPC_VUC_TCAM_HARDWARE_VERSION_RESP (SIpcTcamHwVersionType)
typedef struct
{
    uint8_t eolhwVersion;
    uint8_t eolDataVersion;
    uint8_t eolhwVariant;
    uint8_t eolVolvoSerialNumber[4];
    uint8_t eolVolvoDelivPartNumber[11];
    uint8_t eolVolvoCorePartNumber[11];
    uint8_t eolProductionDate[4];
} VucTcamHwVersionType;

// Maps to IPC_VUC_VUC_BLE_SW_PARTNUMBER_RESP (SIpcVucBleSwHwVersionRepType)
typedef struct
{
    uint8_t vucPartNumber[11];
    uint8_t blePartNumber[11];
} VucVucBleSwHwVersionRepType;

// Maps to IPC_VUC_RF_SW_HW_VERSION_RESP (SIpcRfSwHwVersionRespType)
typedef struct
{
    uint8_t rfSwVersion[2];
    uint8_t rfHwVersion[4];
    uint8_t rfSwMarketVar;
    uint8_t rfHwMarketVar[2];
    uint8_t rfHwSerialNumber[8];
} VucRfSwHwVersionRespType;

// Maps to IPC_VUC_FBL_SOFTWARE_BUILD_DATE_RESP (SIpcFblSwBuildDateRespType)
typedef struct
{
    uint8_t timestamp[7];
    uint8_t checksum[2];
    uint8_t sw_version[2];
} VucFblSwBuildDateRespType;

// Maps to IPC_VUC_BELT_INFO_RESP (SIpcBeltInfoRespType)
typedef struct
{
    // Driver
    uint8_t BitLockStADrvrForBltLockSt1         : 1;    /* Byte 1 */
    uint8_t BitLockStADrvrForBltLockSts         : 1;

    // Passenger
    uint8_t BitLockStAPassForBltLockSt1         : 1;
    uint8_t BitLockStAPassForBltLockSts         : 1;

    // Second Row Left
    uint8_t BitLockStARowSecLeForBltLockEq      : 1;
    uint8_t BitLockStARowSecLeForBltLockSt1     : 1;
    uint8_t BitLockStARowSecLeForBltLockSts     : 1;
    uint8_t reserved1                           : 1;

    // Second Row Middle
    uint8_t BitLockStARowSecMidForBltLockEq     : 1;    /* Byte 2 */
    uint8_t BitLockStARowSecMidForBltLockSt1    : 1;
    uint8_t BitLockStARowSecMidForBltLockSts    : 1;

    // Second Row Right
    uint8_t BitLockStARowSecRiForBltLockEq      : 1;
    uint8_t BitLockStARowSecRiForBltLockSt1     : 1;
    uint8_t BitLockStARowSecRiForBltLockSts     : 1;
    uint8_t reserved2                           : 2;

    // Third Row Left
    uint8_t BitLockStARowThrdLeForBltLockEq     : 1;    /* Byte 3 */
    uint8_t BitLockStARowThrdLeForBltLockSt1    : 1;
    uint8_t BitLockStARowThrdLeForBltLockSts    : 1;

    // Third Row Right
    uint8_t BitLockStARowThrdRiForBltLockEq     : 1;
    uint8_t BitLockStARowThrdRiForBltLockSt1    : 1;
    uint8_t BitLockStARowThrdRiForBltLockSts    : 1;
    uint8_t reserved3                           : 2;

} VucBeltInfoRespType;

// Maps to IPC_VUC_VEHICLE_SPEED_STATE_REP (SIpcVehicleSpeedStateRepType)
typedef struct
{
    uint8_t vScanFull       : 1;  /* Byte 1 */
    uint8_t vScanLimited    : 1;
    uint8_t reserved        : 6;
} VucVehicleSpeedStateRepType;

// Maps to IPC_VUC_CAN_VOLTAGE_ERROR_REP (SIpcCANVoltageErrorRepType)
typedef struct
{
    uint8_t canUnderVoltage         : 1;  /* Byte 1 */
    uint8_t kl30UnderVoltage        : 1;
    uint8_t kl30OverVoltage         : 1;
    uint8_t reserved                : 5;
} VucCANVoltageErrorRepType;

// Maps to IPC_VUC_CAR_CONFIG_REPORT_REP (SIpcCarConfigReportRepType)
typedef struct
{
    uint16_t paramId     : 14;  /* Byte 1 - 2 */
    uint16_t validity    : 2;
    uint8_t ccp;                /* Byte 3 */
} VucCarConfigReportRepType;

// Maps to IPC_VUC_DIAG_COUPLE_REP (SIpcDiagCoupleRepType)
typedef struct
{
    uint8_t request     : 1;  /* Byte 1 */
    uint8_t reserved    : 7;
} VucDiagCoupleRepType;

// Maps to IPC_VUC_DOOR_LOCK_UNLOCK_RESP
typedef struct
{
    uint8_t driverDoorState                     : 2;    /* Byte 1 */
    uint8_t driverSideRearDoorState             : 2;
    uint8_t driverDoorLockState                 : 2;
    uint8_t driverSideRearDoorLockState         : 2;

    uint8_t passengerDoorState                  : 2;    /* Byte 2 */
    uint8_t passengerSideRearDoorState          : 2;
    uint8_t passengerDoorLockState              : 2;
    uint8_t passengerSideRearDoorLockState      : 2;

    uint8_t tailgateOrBootlidState              : 2;    /* Byte 3 */
    uint8_t tailgateOrBootlidLockState          : 2;
    uint8_t hoodState                           : 2;
    uint8_t reserved1                           : 2;

    uint8_t centralLockingStateForUserFeedback  : 3;    /* Byte 4 */
    uint8_t centralLockState                    : 2;
    uint8_t reserved2                           : 3;

    uint8_t centralLockStateTrigSource          : 5;    /* Byte 5 */
    uint8_t centralLockStateNewEvent            : 1;
    uint8_t reserved3                           : 2;
} VucDoorState;

// Maps to IPC_VUC_SUNROOF_WIN_STATE_RESP
typedef struct
{
    uint8_t sunroofPosnSts                      : 5;    /* Byte 1 */
    uint8_t reserved1                           : 3;

    uint8_t winPosnStsAtDrv                     : 5;    /* Byte 2 */
    uint8_t reserved2                           : 3;

    uint8_t winPosnStsAtPass                    : 5;    /* Byte 3 */
    uint8_t reserved3                           : 3;

    uint8_t winPosnStsReLe                      : 5;    /* Byte 4 */
    uint8_t reserved4                           : 3;

    uint8_t winPosnStsReRi                      : 5;    /* Byte 5 */
    uint8_t reserved5                           : 3;
} VucWinOpenState;

// Maps to IPC_VUC_PM25_ENQUIRES_REPORT_RESP
typedef struct
{
	uint8_t IntPm25LvlFrmClima		:3; /* Byte 1 */
	uint8_t reserved1				:3;
	uint8_t IntPm25VluFrmClimaHigh	:2;
	uint8_t IntPm25VluFrmClimaLow; 		/* Byte 2 */
	uint8_t OutdPm25LvlFrmClima 	:3; /* Byte 3 */
	uint8_t reserved2				:3;
	uint8_t OutdPm25VluFrmClimaHigh	:2;
	uint8_t OutdPm25VluFrmClimaLow; 	/* Byte 4 */ 
} VucPMLevelState;

// VUC MESSAGES OUT ------------------------------------------------------------

// Maps to IPC_NAD_HORN_LIGHT_REQ (SIpcHornULightReqType)

/** @brief Horn and light request
    hornUlight:     0 - No request for Horn&Light
                    1 - Request for Horn
                    2 - Request for Light
                    3 - Request for Horn and Light */
typedef struct
{
    uint8_t hornUlight : 4;
    uint8_t reserved : 4;

} NadHornULightReqType;


/** @brief Switch to boot
    mode:   0 - normal mode
            1 - boot loader mode */

// Maps to IPC_NAD_SWITCH_TO_BOOT_REQ (SIpcSwitchToBootReqType)

typedef struct
{
    uint8_t mode : 1;
    uint8_t reserved : 7;
} NadSwitchToBootReqType;

// Maps to  IPC_NAD_ANTENNA_SWITCH_REQ (SIpcAntennaSwitchType)
/** @brief Horn and light request
    SwCntrl:    0 - Not valid
                1 - Not valid
                2 - NAD is requesting to switch to PHONE1 antenna
                3 - NAD is requesting to switch to ECALL antenna */
typedef struct
{
    uint8_t SwCntrl : 2;
    uint8_t reserved : 6;
} NadAntennaSwitchType;

// Maps to  IPC_NAD_LAN_LINK_TRIG_REP (SIpcLanLinkTrigReqType)
/** @brief Lan link trigger
    linkTrigger:  0 - no link is established
                  1 = a WLAN or Ethernet link is established
    DiagComSes:   0 - No DoIP session is ongoing
                  1 - DoIP session is ongoing */
typedef struct
{
    uint8_t linkTrigger : 1;
    uint8_t DiagComSes : 1;
    uint8_t reserved : 6;
} NadLanLinkTrigReqType;

// Maps to IPC_NAD_TRACE_REP (SIpcNadVucTraceType)
typedef struct
{
    uint8_t messageName : 3;
    uint8_t type : 2;
    uint8_t priority : 3;
    uint8_t fcodeid;
    uint8_t payload[20];
} NadVucTraceType;

// Maps to IPC_NAD_CAR_CONFIG_REQ (SIpcCarConfigReqType)
typedef struct
{
    uint16_t paramId;
} NadCarConfigReqType;

// Maps to IPC_NAD_BLE_DISCONNECT_REQ (SIpcBleDisconnectReqType)
typedef struct
{
    uint16_t connectionID       : 14;
    uint16_t reserved           : 2;
} NadBleDisconnectReqType;

// Maps to IPC_NAD_BLE_CONFIG_DATACOM_SERVICE_REQ (SIpcBleConfigDatacomServiceReqType)
typedef struct
{
    uint8_t serviceUUID[16];
} NadBleCfgDataCommServReqType;

// Maps to IPC_NAD_BLE_CONFIG_IBEACON_REQ (SIpcBleConfigIBeaconReqType)
typedef struct
{
    uint8_t proximityUUID[16];
    uint16_t major;
    uint16_t minor;
} NadBleCfgIbeaconReqType;

// Maps to IPC_NAD_BLE_CONFIG_GENERIC_ACCESS_REQ (SIpcBleConfigGenericAccessReqType)
typedef struct
{
    uint8_t idString[64];
} NadBleCfgGenAccReqType;

// Maps to IPC_NAD_BLE_START_PAIRING_SERVICE_REQ (SIpcBleStartPairingServiceReqType)
typedef struct
{
    uint16_t serviceUUID;
    uint8_t nameCharString[64];
} NadBleStartPairServReqType;

// Maps to IPC_NAD_BLE_DATA_ACCESS_AUTH_CONF_REQ (SIpcBleDataAccessAuthConfReqType)
typedef struct
{
    uint16_t connectionID   : 14;
    uint16_t reserved       : 2;
} NadBleDataAccAuthConReqType;


// Maps to IPC_NAD_BLE_START_ADVERTISING_REQ (SIpcBleStartAdvertisingReqType)
typedef struct
{
    uint8_t adv_frame;
} NadBleStartAdvReqType;

// Maps to IPC_NAD_DTC_HANDLING_REQ (SIpcDtcHandlingReqType)
typedef struct
{
    uint8_t requestType;
    uint8_t eventIdentifier;
} NadDtcHandlingReqType;

// Maps to IPC_NAD_IP_WAKEUP_REQ (SIpcIpWakeupReqType) */
typedef struct
{
    uint8_t prio        : 1;
    uint8_t reserved    : 7;
    uint8_t resourceGroup;
} NadIpWakeupReqType;

// Maps to IPC_NAD_OHC_LED_REP (SIpcOhcLedType)
typedef struct
{
    uint8_t led1_1;
    uint8_t led1_2;
    uint8_t led2_1;
    uint8_t led2_2;
    uint8_t led3_1;
    uint8_t led3_2;
    uint8_t brightness;
} NadOhcLedType;

// Maps to IPC_NAD_MESSAGE_CATALOGUE_VERSION_REQ (SIpcNadMsgCatalogVerType)
typedef struct
{
    uint8_t vucMsgCatalogByte1;
    uint8_t vucMsgCatalogByte2;
    uint8_t vucMsgCatalogByte3;
} NadMsgCatalogVerType;

// Maps to IPC_NAD_LOCAL_CONFIG_PARAM_RESP (SIpcLocalConfigParamRespType)
typedef struct
{
    uint16_t bleOrVuc           : 1;
    uint16_t localCfgParam      : 15;
    uint32_t localCfgParamValue;
} NadLocalConfigParamRespType;

// Maps to IPC_NAD_PSIM_DATA_COMMUNICATION_REQ (SIpcPSimDataCommunicationReqType)
typedef struct
{
    uint8_t payload[256];
} NadPSimDataCommunicationReqType;

// Maps to IPC_NAD_SOC_DID_STATUS_REQ (SIpcStatusSOCDidReqType)
typedef struct
{
    uint8_t detectionStatusDTC[8];
    uint8_t reserved1;
    uint8_t reserved2;
} NadStatusSOCDidReqType;
//uia93888
//Maps to IPC_VUC_ENGINE_REM_START_RESP
typedef struct
{
    uint8_t ersStrtApplSts      : 2;
    uint8_t reserved1           : 6;
    uint8_t engSt1WdSts         : 4;
    uint8_t reserved2           : 4;
    uint8_t ersStrtRes          : 5;
    uint8_t reserved3           : 3;
}RMT_EngineState;

typedef struct{
    uint8_t ersDelayTiCfm       :1;
    uint8_t reserved       :7;
} EngineDelay;

// Maps to IPC_NAD_THERMAL_MITIGATION_REQ (SIpcThermalMitigationReqType)
typedef struct
{
    uint8_t temperature;
} NadThermalMitigationReqType;

/* Message Template for IPC_NAD_REMOTE_DOOR_UNLOCK_REQ
 * 4.2 Door Lock/Unlock Request
 * Maps to SIpcNadDoorUnlockRequestTypeNew
 */
typedef struct
{
    uint8_t centralLockReq          : 2; /* Byte 1 */
    uint8_t centralLockReqTailgate  : 2;
    uint8_t reserved                : 4;
} NadDoorUnlockRequestType;

/* Message Template for 
 * 4.47	SunRoof Open/Close Request 
 * Maps to SIpcSunRoofOpenCloseReqType
 */
typedef struct
{
    uint8_t sunRoofOpenCloseReq		:3;	/* Byte 1 */
	uint8_t reserved				:5; /* Byte 1 */
} NadSunRoofOpenCloseReqType;

/* Message Template for 
 * 4.48	Windows Open/Close Request 
 * Maps to SIpcWindowOpenCloseReqType
 */
typedef struct
{
    uint8_t windowOpenCloseReq		:2;	/* Byte 1 */
	uint8_t reserved				:6; /* Byte 1 */
    uint8_t value;                      /* Byte 2 */
} NadWindowOpenCloseReqType;

/* Message Template for 
 * 4.49	One Button Ventilation Request
 * Maps to SIpcWindowVentilationReqType
 */
typedef struct
{
    uint8_t windowVentilationReq	:1;	/* Byte 1 */
	uint8_t winJawOpening			:7; /* Byte 1 */
} NadWindowVentilationReqType;

/* Message Template for 
 * 4.50	PM 2.5 Enquires Request 
 * Maps to SIpcPMEnquiresReqType
 */
typedef struct
{
    uint8_t pmEnquiresReq			:1;	/* Byte 1 */
	uint8_t reserved				:7; /* Byte 1 */
} NadPMEnquiresReqType;

//add by uia93888
typedef struct {
    uint8_t telmSeatDrvHeat     :2;
    uint8_t telmSeatPassHeat    :2;
    uint8_t telmSeatSecLeHeat   :2;
    uint8_t telmSeatSecRiHeat   :2;
} OperateRMTSeatHeat;

typedef struct {
    uint8_t telmSeatDrvVenti    :2;
    uint8_t telmSeatPassVenti   :2;
    uint8_t telmSeatSecLeVenti  :2;
    uint8_t telmSeatSecRiVenti  :2;
} OperateRMTSeatVenti;


//end uia93888

#pragma pack() //back to whatever the previous packing mode was

} //namespace vc

#endif //VUC_MESSAGES_HPP
