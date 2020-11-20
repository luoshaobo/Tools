#ifndef VEHICLE_COMM_MESSAGES_HPP
#define VEHICLE_COMM_MESSAGES_HPP

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

#include <stdint.h>
#include <iostream>
#include <string>
#include <map>

namespace vc {

#define N_STR_MAX 64
#define N_VECT_MAX 0xFF



// VehicleComm - General

typedef struct { unsigned value : 4; } uint4_t;



// VehicleComm - Requests

/**
    @brief A list of all supported request messages.

    A RequestID can be one of the requests listed in this enum.
*/
typedef enum {
    REQ_UNKNOWN,
    // VuC
    REQ_DOORLOCKUNLOCK,
    REQ_HORNNLIGHT,
    REQ_SWITCHTOBOOT,
    REQ_ANTENNASWITCH,
    REQ_LANLINKTRIGGER,
    REQ_GETCARCONFIG,
    REQ_CARCONFIGFACTRESTORE,
    REQ_ADCVALUE,
    REQ_VUCTRACE_CONFIG,
    REQ_VUCTRACE_CONTROL,
    REQ_BLE_DISCONNECT,
    REQ_BLE_DISCONNECTALL,
    REQ_BLE_GENERATEBDAK,
    REQ_BLE_GETBDAK,
    REQ_BLE_CONFIG_DATACOMSERVICE,
    REQ_BLE_CONFIG_IBEACON,
    REQ_BLE_CONFIG_GENERICACCESS,
    REQ_BLE_STARTPARINGSERVICE,
    REQ_BLE_STOPPARINGSERVICE,
    REQ_BLE_WAKEUP_RESPONSE,
    REQ_BLE_DATAACCESSAUTHCONFIRMED,
    REQ_BLE_STARTADVERTISING,
    REQ_BLE_STOPADVERTISING,
    REQ_DTCENTRIES,
    REQ_DTCCTLSETTINGS,
    REQ_IPWAKEUP,
    REQ_SETOHCLED,
    REQ_VINNUMBER,
    REQ_GETVUCBLESWPARTNUMBER,
    REQ_GETBELTINFORMATION,
    REQ_GETDIAGERRREPORT,
    REQ_GETVEHICLESTATE,
    REQ_GETDIDGLOBALSNAPSHOTDATA,
    REQ_DTCSDETECTED,
    REQ_PSIMDATA,
    REQ_THERMALMITIGATION,
    REQ_CARCFGPARAMETERFAULT,
    REQ_NETWORK_MANAGEMENT_HISTORY,
    REQ_PROGRAM_PRECOND_CHK,
#if 1 // nieyj
    REQ_WINCTRL,
    REQ_ROOFCTRL,
    REQ_WINVENTI,
    REQ_PM25ENQUIRE,
    REQ_VFCACTIVATE,
#endif
    // uia93888
    REQ_RMTENGINE,
    REQ_RMTENGINESECURITYRESPONSE,
    REQ_GETRMTSTATUS,
    REQ_GETENGINESTATES,
    REQ_RTCAWAKEVUC,
    REQ_DELAYENGINERUNNINGTIME,
    REQ_OPERATEREMOTECLIMATE,
    REQ_OPERATERMTSEATHEAT,
    REQ_OPERATERMTSEATVENTILATION,
    REQ_OPERATERMTSTEERWHLHEAT,
    REQ_PARKINGCLIMATEOPER,
    // uia93888 end
    REQ_ROLLOVERVALUE,
    REQ_SETROLLOVERCFG,
    // VGM
    REQ_SENDPOSNFROMSATLTCON,
    // IHU
    // Data-storage
    REQ_GETDSPARAMETER,
    // Message-processor
    REQ_VUCSWVERSION,
    REQ_BLESWVERSION,
    REQ_LASTBUBSTATE,
    REQ_CARUSAGEMODE,
    REQ_CARMODE,
    REQ_ELECTENGLVL,
    REQ_GETVUCPOWERMODE,
    REQ_GETVUCWAKEUPREASON,
    REQ_GETWIFIDATA,
    REQ_GETPSIMAVAILABILITY,
    REQ_GETFUELTYPE,
    REQ_GETPROPULSIONTYPE,
    REQ_GETOHCBTNSTATE,
    REQ_GETNUMBEROFDOORS,
    REQ_GETSTEERINGWHEELPOSITION,
    REQ_GETFUELTANKVOLUME,
    REQ_GETASSISTANCESERVICES,
    REQ_GETAUTONOMOUSDRIVE,
    REQ_GETCOMBINEDINSTRUMENT,
    REQ_GETTELEMATICMODULE,
    REQ_GETCONNECTIVITY,
    REQ_GETCONNECTEDSERVICEBOOKING,
    REQ_GETCARCFGTHEFTNOTIFICATION,
    REQ_GETCARCFGSTOLENVEHICLETRACKING,
    REQ_GETGNSSRECEIVER,
    REQ_GETAPPROVEDCELLULARSTANDARDS,
    REQ_GETDIGITALVIDEORECORDER,
    REQ_GETDRIVERALERTCONTROL,
    REQ_GETDOORSTATE,
    //Multi
    REQ_GETVUCSWBUILD,
    REQ_GETBLESWBUILD,
    REQ_GETMSGCATVERSION,
    REQ_GETTCAMHWVERSION,
    REQ_GETRFVERSION,
    REQ_GETFBLSWBUILD,
    // Test
    REQ_VGMTESTMSG,
    REQ_IHUTESTMSG,
    REQ_TEST_GETCARCONFIG,
    REQ_TEST_TOGGLEUSAGEMODE,
    REQ_TEST_TOGGLECARMODE,
    REQ_TEST_CRASHSTATE,
    REQ_TEST_OHCSTATE,
    REQ_TEST_SETFAKEVALUESDEFAULT,
    REQ_TEST_SETFAKEVALUEINT,
    REQ_TEST_SETFAKEVALUESTR,
    REQ_TEST_SETLOCALCONFIG,
} RequestID;

const char *const VCRequestStr[] = {
    "REQ_UNKNOWN",
    // VuC
    "REQ_DOORLOCKUNLOCK",
    "REQ_HORNNLIGHT",
    "REQ_SWITCHTOBOOT",
    "REQ_ANTENNASWITCH",
    "REQ_LANLINKTRIGGER",
    "REQ_GETCARCONFIG",
    "REQ_CARCONFIGFACTRESTORE",
    "REQ_ADCVALUE",
    "REQ_VUCTRACE_CONFIG",
    "REQ_VUCTRACE_CONTROL",
    "REQ_BLE_DISCONNECT",
    "REQ_BLE_DISCONNECTALL",
    "REQ_BLE_GENERATEBDAK",
    "REQ_BLE_GETBDAK",
    "REQ_BLE_CONFIG_DATACOMSERVICE",
    "REQ_BLE_CONFIG_IBEACON",
    "REQ_BLE_CONFIG_GENERICACCESS",
    "REQ_BLE_STARTPARINGSERVICE",
    "REQ_BLE_STOPPARINGSERVICE",
    "REQ_BLE_WAKEUP_RESPONSE",
    "REQ_BLE_DATAACCESSAUTHCONFIRMED",
    "REQ_BLE_STARTADVERTISING",
    "REQ_BLE_STOPADVERTISING",
    "REQ_DTCENTRIES",
    "REQ_DTCCTLSETTINGS",
    "REQ_IPWAKEUP",
    "REQ_SETOHCLED",
    "REQ_VINNUMBER",
    "REQ_GETVUCBLESWPARTNUMBER",
    "REQ_GETBELTINFORMATION",
    "REQ_GETDIAGERRREPORT",
    "REQ_GETVEHICLESTATE",
    "REQ_GETDIDGLOBALSNAPSHOTDATA",
    "REQ_DTCSDETECTED",
    "REQ_PSIMDATA",
    "REQ_THERMALMITIGATION",
    "REQ_CARCFGPARAMETERFAULT",
    "REQ_NETWORK_MANAGEMENT_HISTORY",
    "REQ_PROGRAM_PRECOND_CHK",
#if 1 // nieyj
    "REQ_WINCTRL",
    "REQ_ROOFCTRL",
    "REQ_WINVENTI",
    "REQ_PM25ENQUIRE",
    "REQ_VFCACTIVATE",
#endif
    // uia93888 res
    "REQ_RMTENGINE",
    "REQ_RMTENGINESECURITYRESPONSE",
    "REQ_GETRMTSTATUS",
    "REQ_GETENGINESTATES",
    "REQ_RTCAWAKEVUC",
    "REQ_DELAYENGINERUNNINGTIME",
    "REQ_OPERATEREMOTECLIMATE",
    "REQ_OPERATERMTSEATHEAT",
    "REQ_OPERATERMTSEATVENTILATION",
    "REQ_OPERATERMTSTEERWHLHEAT",
    "REQ_PARKINGCLIMATEOPER",
    "REQ_ROLLOVERVALUE",
    "REQ_SETROLLOVERCFG",
    // VGM
    "REQ_SENDPOSNFROMSATLTCON",
    // IHU
    // Data-storage
    "REQ_GETDSPARAMETER",
    // Message-processor
    "REQ_VUCSWVERSION",
    "REQ_BLESWVERSION",
    "REQ_LASTBUBSTATE",
    "REQ_USAGEMODE",
    "REQ_CARMODE",
    "REQ_ELECTENGLVL",
    "REQ_GETVUCPOWERMODE",
    "REQ_GETVUCWAKEUPREASON",
    "REQ_GETWIFIDATA",
    "REQ_GETPSIMAVAILABILITY",
    "REQ_GETFUELTYPE",
    "REQ_GETPROPULSIONTYPE",
    "REQ_GETOHCBTNSTATE",
    "REQ_GETNUMBEROFDOORS",
    "REQ_GETSTEERINGWHEELPOSITION",
    "REQ_GETFUELTANKVOLUME",
    "REQ_GETASSISTANCESERVICES",
    "REQ_GETAUTONOMOUSDRIVE",
    "REQ_GETCOMBINEDINSTRUMENT",
    "REQ_GETTELEMATICMODULE",
    "REQ_GETCONNECTIVITY",
    "REQ_GETCONNECTEDSERVICEBOOKING",
    "REQ_GETCARCFGTHEFTNOTIFICATION",
    "REQ_GETCARCFGSTOLENVEHICLETRACKING",
    "REQ_GETGNSSRECEIVER",
    "REQ_GETAPPROVEDCELLULARSTANDARDS",
    "REQ_GETDIGITALVIDEORECORDER",
    "REQ_GETDRIVERALERTCONTROL",
    "REQ_GETDOORSTATE",
    // Multi
    "REQ_GETVUCSWBUILD",
    "REQ_GETBLESWBUILD",
    "REQ_GETMSGCATVERSION",
    "REQ_GETTCAMHWVERSION",
    "REQ_GETRFVERSION",
    "REQ_GETFBLSWBUILD",
    // Test
    "REQ_VGMTESTMSG",
    "REQ_IHUTESTMSG",
    "REQ_TEST_GETCARCONFIG",
    "REQ_TEST_TOGGLEUSAGEMODE",
    "REQ_TEST_TOGGLECARMODE",
    "REQ_TEST_CRASHSTATE",
    "REQ_TEST_OHCSTATE",
    "REQ_TEST_SETFAKEVALUESDEFAULT",
    "REQ_TEST_SETFAKEVALUEINT",
    "REQ_TEST_SETFAKEVALUESTR",
    "REQ_TEST_SETLOCALCONFIG",
};

// Requests - VuC

// Requests - VuC - REQ_DOORLOCKUNLOCK ---------------------------------------

typedef struct
{
    uint8_t centralLockReq : 2;
    uint8_t reserved : 6;
} ReqDoorLockUnlock;

// Requests - VuC - REQ_HORNNLIGHT -------------------------------------------

typedef enum {
    HL_REQUEST_ABORT = 0,
    HL_REQUEST_HORN = 1,
    HL_REQUEST_LIGHT = 2,
    HL_REQUEST_HORN_AND_LIGHT = 3,
} HNLMode;

typedef struct
{
    HNLMode mode;
} ReqHornNLight;

#if 1 // nieyj
// Requests - VuC - REQ_WINCTRL -------------------------------------------

typedef enum {
    WIN_OPEN_REQ_IDLE = 0,
    WIN_OPEN_REQ_OPEN = 1,
    WIN_OPEN_REQ_CLOSE = 2,
} WinCtrlReqMode;

typedef struct
{
    WinCtrlReqMode mode;
    uint8_t openValue;
} ReqWinCtrl;

// Requests - VuC - REQ_ROOFCTRL -------------------------------------------

typedef enum {
    ROOF_CURT_CTRL_REQ_IDLE = 0,
    ROOF_CURT_CTRL_REQ_OPEN_CURTAIN = 1,
    ROOF_CURT_CTRL_REQ_CLOSE_CURTAIN = 2,
    ROOF_CURT_CTRL_REQ_OPEN_PANEL = 3,
    ROOF_CURT_CTRL_REQ_CLOSE_PANEL = 4,
    ROOF_CURT_CTRL_REQ_OPEN_MANUAL = 5,
    ROOF_CURT_CTRL_REQ_CLOSE_MANUAL = 6,
    ROOF_CURT_CTRL_REQ_OPEN_TULT = 7,
} RoofCurtCtrlReqMode;

typedef struct
{
    RoofCurtCtrlReqMode mode;
} ReqRoofCurtCtrl;

// Requests - VuC - REQ_WINVENTI -------------------------------------------

typedef enum {
    WIN_VENTI_REQ_DEFAULT = 0,
    WIN_VENTI_REQ_OPEN = 1
} WinVentiReqMode;

typedef struct
{
    WinVentiReqMode mode : 1;
    uint8_t         value : 7;
} ReqWinVentilation;

// Requests - VuC - REQ_PM25ENQUIRE -------------------------------------------

typedef struct
{
    uint8_t         req;
} ReqPM25Enquire;

// Requests - VuC - REQ_VFCACTIVATE -------------------------------------------

typedef struct
{
    uint8_t         id;    
    uint8_t         type;   // 0:Deactive, 1:Active
} ReqVFCActivate;
#endif


// uia93888
// Request -Vuc - REQ_RMTENGINE --------------------------------------------
typedef enum
{
	ErsCmd_ErsCmdNotSet = 0,
	ErsCmd_ErsCmdOn,
	ErsCmd_ErsCmdOff
} ErsCmd;
typedef uint8_t ErsRunTime;
typedef struct
{
    uint8_t ersCmd;
    uint8_t ersRunTime;
    uint8_t imobVehRemReqCmd;
    uint8_t imobVehDataRemReq0;
    uint8_t imobVehDataRemReq1;
    uint8_t imobVehDataRemReq2;
    uint8_t imobVehDataRemReq3;
    uint8_t imobVehDataRemReq4;
    uint8_t imobVehRemTmrOrSpdLim;
} Req_RMTEngine;    // remote start engine's request

// Request -Vuc - REQ_RMTENGINESECURITYRESPONSE-----------------------------
typedef struct
{
    uint8_t imobVehRemReqCmd;
    uint8_t imobVehDataRemReq0;
    uint8_t imobVehDataRemReq1;
    uint8_t imobVehDataRemReq2;
    uint8_t imobVehDataRemReq3;
    uint8_t imobVehDataRemReq4;
    uint8_t imobVehRemTmrOrSpdLim;
} Req_RMTEngineSecurityResponse;

// request -data storage - REQ_GETRMTSTATUS ---------------------------------
typedef struct {} Req_GetRMTStatus;

// request -data storage - REQ_GETENGINESTATES ------------------------------
typedef struct {} Req_GetEngineStates;

// request -data storage - REQ_RTCAWAKEVUC  ----------------------------------
typedef struct {
    uint8_t secondAlarm_unit        :4;  // range: 0 ~ 9
    uint8_t secondAlarm_ten         :3;  // range: 0 ~ 5
    uint8_t secondAlarm_enable      :1;  // 0: false(don't use), 1: true(use)

    uint8_t minuteAlarm_unit        :4;  // range: 0 ~ 9
    uint8_t minuteAlarm_ten         :3;  // range: 0 ~ 5
    uint8_t minuteAlarm_enable      :1;  // 0: false(don't use), 1: true(use)

    uint8_t hourAlarm_unit          :4;  // range: 0 ~ 9
    uint8_t hourAlarm_ten           :2;  // range: 0 ~ 2
    uint8_t hourAlarm_reserved      :1;  // reserved
    uint8_t hourAlarm_enable        :1;  // 0: false(don't use), 1: true(use)

    uint8_t dayAlarm_unit           :4;  // range: 0 ~ 9
    uint8_t dayAlarm_ten            :2;  // range: 0 ~ 3
    uint8_t dayAlarm_reserved       :1;  // reserved
    uint8_t dayAlarm_enable         :1;  // 0: false(don't use), 1: true(use)
} Req_RTCAwakeVuc;

// request -data storage - REQ_DELAYENGINERUNNINGTIME------------------------
typedef struct {
    uint8_t telmEngDelayTi          :6;  // range: 0 ~ 59mins
    uint8_t Reserved                :2;
} Req_DelayEngineRunngTime;

// request -data storage - REQ_OPERATEREMOTECLIMATE------------------------
typedef enum{
    OnOffNoReq_NoReq = 0x0,
    OnOffNoReq_On,
    OnOffNoReq_Off
} TelmClimaReq;

typedef enum{
    HmiCmptmtSpSpcl_Norm  = 0x0,
    HmiCmptmtSpSpcl_Lo,
    HmiCmptmtSpSpcl_Hi
} TelmClimaTSetHmiCmptmtTSpSpcl;

typedef struct {
    uint8_t telmClimaReq                       :2;
    uint8_t reserved1                               :6;

    uint8_t telmClimaTSetTempRange                  :5;
    uint8_t hmiCmptmtTSpSpcl  :2;
    uint8_t reserved2                               :1;
} Req_OperateRMTClimate;

// request -data storage - REQ_OPERATERMTSEATHEAT----------------------------
typedef enum {
    TelmSeatCliamtLvl_Closed = 0x0,
    TelmSeatCliamtLvl_Lvl1,
    TelmSeatCliamtLvl_Lvl2,
    TelmSeatCliamtLvl_Lvl3
} TelmSeatCliamtLvls;

typedef struct {
    TelmSeatCliamtLvls telmSeatDrvHeat;
    TelmSeatCliamtLvls telmSeatPassHeat;
    TelmSeatCliamtLvls telmSeatSecLeHeat;
    TelmSeatCliamtLvls telmSeatSecRiHeat;
} Req_OperateRMTSeatHeat;

// request -data storage - REQ_OPERATERMTSEATVENTILATION---------------------

typedef struct {
    TelmSeatCliamtLvls telmSeatDrvVenti;
    TelmSeatCliamtLvls telmSeatPassVenti;
    TelmSeatCliamtLvls telmSeatSecLeVenti;
    TelmSeatCliamtLvls telmSeatSecRiVenti;
} Req_OperateRMTSeatVenti;

// request - data storage - REQ_OPERATERMTSTEERWHLHEAT-----------------------
typedef struct{
    int8_t SteerWhlHeatgDurgClimaEnadFromTelm   :1;
    int8_t Reserved                             :7;
} Req_OperateRMTSteerWhlHeat_S;


// request -Vuc REQ_PARKINGCLIMATEOPER---------------------------------------
typedef struct{
    uint8_t  climaRqrd1                         :2; //0: NoReq, 1: On, 2:off
    uint8_t  reserved1                          :6;

    uint16_t telmClimaTmr;                          //uint: seconds

    uint8_t  climaTmrStsTelmTqrd                :1; //0: off, 1: on
    uint8_t  telmClimaTSetTempRange             :5;
    uint8_t  hmiCmptmtSpSpcl                    :2; //0: Norm, 1:Lo, 2: Hi

    uint8_t  seatHeatDurgClimaEnadFromTelm      :3; //0: SeatHeat Off, 1: SeatDrvOn, 2: SeatPassOn, 3: SeatDrvAndPass, 4: SeatLeftRearOn, 5: SeatRighRearOn
    uint8_t  reserved2                          :5;

    uint8_t  steerWhlHeatgDurgClimaEnadFromTelm :1; //0: off, 1: on
    uint8_t  reserved3                          :7;
}Req_ParkingClimateOper_S;

//end uia93888


// Requests - VuC - REQ_SWITCHTOBOOT -----------------------------------------

typedef struct {} ReqSwitchToBoot;

// Requests - VuC - REQ_ANTENNASWITCH ----------------------------------------

typedef enum
{
    ANTENNA_HANDLED_BY_NAD_REQUEST_PHONE1 = 2,
    ANTENNA_HANDLED_BY_NAD_REQUEST_ECALL
} AntennaMode;

typedef struct
{
    AntennaMode mode;
} ReqAntennaSwitch;

// Requests - VuC - REQ_LANLINKTRIGGER ---------------------------------------

typedef enum
{
    LINK_TRIGGER_NO_LINK_ESTABLISHED = 0,
    LINK_TRIGGER_WLAN_OR_ETH_ESTABLISHED = 1,
} LinkTrigger;

typedef enum
{
    LINK_SESSION_NO_DOIP_ONGOING = 0,
    LINK_SESSION_DOIP_ONGOING = 1,
} LinkSessionState;

typedef struct
{
    LinkTrigger trigger;
    LinkSessionState session_state;
} ReqLanLinkTrigger;

// Requests - VuC - REQ_GETCARCONFIG -----------------------------------------

typedef struct
{
    uint16_t  param_id;
} ReqGetCarConfig;

// Requests - VuC - REQ_CARCONFIGFACTRESTORE ---------------------------------

typedef struct {} ReqCarConfigFactRestore;

// Request - VuC - REQ_ADC ---------------------------------------------------

typedef enum {
    ADC_ID_SW_VBATT,        // Switched battery voltage
    ADC_ID_BUB_VOLTAGE,     // Back up battery voltage
    ADC_ID_BUTP_VOLTAGE,    // Button positive voltage
    ADC_ID_BUTN_VOLTAGE,    // Button negative voltage
    ADC_ID_MICP_VOLTAGE,    // Microphone positive voltage
    ADC_ID_MICN_VOLTAGE,    // Microphone negative voltage
    ADC_ID_GPS_ANT_PWR,     // GPS antenna power
    ADC_ID_GPS_VANT,        // GPS antenna voltage
    ADC_ID_LED1_STATUS,     // LED 1 status
    ADC_ID_LED2_STATUS,     // LED 2 status
    ADC_ID_LED3_STATUS,     // LED 3 status
    ADC_ID_SPKRP_DIAG,      // Speaker positive diagnostics
    ADC_ID_SPKRN_DIAG,      // Speaker negative diagnostics
    ADC_ID_HW_ID,           // Id to determine hardware
    ADC_ID_AMBIENT_TEMP,    // Ambient temperature of board
    ADC_ID_GSM_ANT_1,       // GSM Antenna 1
    ADC_ID_BUTTON_CURRENT,  // Button current
} AdcId;

typedef struct
{
    AdcId id;
} ReqAdcValue;

// Requests - VuC - REQ_VUCTRACE_CONFIG --------------------------------------

typedef enum
{
    TRACE_MODULE_ID_UNUSED,
    TRACE_MODULE_ID_CANH,
    TRACE_MODULE_ID_BLEH,
    TRACE_MODULE_ID_KLBH,
    TRACE_MODULE_ID_A2BD,
    TRACE_MODULE_ID_DIAG,
    TRACE_MODULE_ID_IPC,
    TRACE_MODULE_ID_BUB,
    TRACE_MODULE_ID_CPU,
    TRACE_MODULE_ID_RF,
    TRACE_MODULE_ID_PWR,
    TRACE_MODULE_ID_ALCH,
    TRACE_MODULE_ID_DTCH,
    TRACE_MODULE_ID_ABAL,
    TRACE_MODULE_ID_TRCH,
    TRACE_MODULE_ID_AUAD,
    TRACE_MODULE_ID_ACCH,
    TRACE_MODULE_ID_AVTS,
    TRACE_MODULE_ID_MAX
} TraceModuleId;

typedef enum
{
    TRACE_REPORTS_DISABLE,
    TRACE_REPORTS_ENABLE_LVL_1,
    TRACE_REPORTS_ENABLE_LVL_2,
    TRACE_REPORTS_ENABLE_LVL_3
} TraceModuleLevel;

typedef struct
{
    TraceModuleId module_id;
    TraceModuleLevel module_lvl;
} TraceModuleCfg;

typedef enum {
    TRACE_BUS_IPC = 1,
    TRACE_BUS_CAN = 3
} TraceBus;

typedef struct
{
    TraceBus bus;
    TraceModuleCfg config[20];
} ReqVucTraceConfig;

// Requests - VuC - REQ_VUCTRACE_CONTROL -------------------------------------

typedef enum
{
    TRACING_ENABLED = 0,
    TRACING_DISABLED = 1,
    TRACING_SUSPEND_FOR_TIMEINTERVAL = 2,
    TRACING_SUSPEND_UNTIL_VUC_WAKEUP_OR_RESET = 3
} TracingMode;

/*  Interval  - 0x00 - 100 ms
                0x01 - 200 ms
                ...
                0x3F - 6400 ms */
typedef struct
{
    TracingMode mode;
    uint8_t interval : 6;
} ReqVucTraceControl;

// Requests - VuC - REQ_BLE_DISCONNECT ---------------------------------------

typedef struct
{
    uint16_t connection_id; // 14 bits used
} ReqBleDisconnect;

// Requests - VuC - REQ_BLE_DISCONNECTALL ------------------------------------

typedef struct {} ReqBleDisconnectAll;

// Requests - VuC - REQ_BLE_GENERATEBDAK -------------------------------------

typedef struct {} ReqBleGenBdak;

// Requests - VuC - REQ_BLE_GETBDAK ------------------------------------------

typedef struct {} ReqBleGetBdak;

// Requests - VuC - REQ_BLE_CONFIG_DATACOMSERVICE ----------------------------

typedef struct
{
    char service_uuid[16];
} ReqBleCfgDataCommService;

// Requests - VuC - REQ_BLE_CONFIG_IBEACON -----------------------------------

typedef struct
{
    char proximity_uuid[16];
    uint16_t major;
    uint16_t minor;
} ReqBleCfgIbeacon;

// Requests - VuC - REQ_BLE_CONFIG_GENERICACCESS -----------------------------

typedef struct
{
    char id_string[64];
} ReqBleCfgGenAcc;

// Requests - VuC - REQ_BLE_STARTPARINGSERVICE -------------------------------

typedef struct
{
    uint16_t service_uuid;
    char name_char_string[64];
} ReqBleStartPairService;

// Requests - VuC - REQ_BLE_STOPPARINGSERVICE --------------------------------

typedef struct {} ReqBleStopPairService;

// Requests - VuC - REQ_BLE_WAKEUP_RESPONSE ----------------------------------

typedef struct {} ReqBleWakeupResponse;

// Requests - VuC - REQ_BLE_DATAACCESSAUTHCONFIRMED --------------------------

typedef struct
{
    uint16_t connection_id;
} ReqBleDataAccAuthCon;

// Requests - VuC - REQ_BLE_STARTADVERTISING ---------------------------------

typedef enum {
    ADV_FRAME_1_AND_2 = 2
} BLEAdvType;

typedef struct {
    BLEAdvType adv_frame;
} ReqBleStartAdvertising;

// Requests - VuC - REQ_BLE_STOPADVERTISING ----------------------------------

typedef struct {} ReqBleStopAdvertising;

// Requests - VuC - REQ_DTCENTRIES -------------------------------------------

typedef enum {
    DTC_SEND_ALL_ENTRIES,
    DTC_SEND_ALL_STATUS_AND_FDC,
    DTC_SEND_FULL_REPORT_SPEC_EVENT = 5
} DtcReqEntries;

typedef struct
{
    DtcReqEntries request_id;
    uint8_t event_id; // Only used if request_type = SEND_DTC_FULL_REPORT_SPEC_EVENT
} ReqDtcEntries;

// Requests - VuC - REQ_DTCCTLSETTINGS -------------------------------------

typedef enum {
    DTC_CLEAR_ALL_DIAG_INFO_ENTRIES = 2,
    DTC_CONTROL_SETTINGS_OFF,
    DTC_CONTROL_SETTINGS_ON,
    DTC_CONTROL_CLEAR_SINGLE_DTC = 6
} DtcCtlSetting;

typedef struct
{
    DtcCtlSetting settings;
    uint8_t event_id; // Only used if settings is = DTC_CONTROL_CLEAR_SINGLE_DTC
} ReqDtcCtlSettings;

// Requests - VuC - REQ_IPWAKEUP ---------------------------------------------

typedef enum {
    IP_WAKEUP_PRIO_NORMAL = 0x00,
    IP_WAKEUP_PRIO_HIGH = 0x01
} IpWakeupPrio;

typedef enum {
    IP_WAKEUP_RES_GROUP_NONE = 0x00,
    IP_WAKEUP_RES_GROUP_RESERVED = 0x01,
    IP_WAKEUP_RES_GROUP_1 = 0x02,
    IP_WAKEUP_RES_GROUP_2 = 0x04,
    IP_WAKEUP_RES_GROUP_3 = 0x08,
    IP_WAKEUP_RES_GROUP_4 = 0x10,
    IP_WAKEUP_RES_GROUP_5 = 0x20,
    IP_WAKEUP_RES_GROUP_6 = 0x40,
    IP_WAKEUP_RES_GROUP_7 = 0x80
} IpWakeupResGroup;

typedef struct
{
    uint8_t prio;
    uint8_t res_group;
} ReqIpWakeup;

// Requests - VuC - REQ_SETOHCLED -----------------------------------------------

typedef enum {
    LEDBLINK_OFF   = 0x00,
    LEDBLINK_0_5HZ = 0xA0,
    LEDBLINK_1HZ   = 0x10,
    LEDBLINK_2HZ   = 0x20,
    LEDBLINK_4HZ   = 0x40,
    LEDBLINK_8HZ   = 0x50,
    LEDBLINK_ON    = 0xF0
} LedBlink;

typedef enum {
    LEDREPEAT_ENDLESS   = 0x00,
    LEDREPEAT_1_TIME    = 0x01,
    LEDREPEAT_2_TIMES   = 0x02,
    LEDREPEAT_3_TIMES   = 0x03,
    LEDREPEAT_4_TIMES   = 0x04,
    LEDREPEAT_5_TIMES   = 0x05,
    LEDREPEAT_6_TIMES   = 0x06,
    LEDREPEAT_7_TIMES   = 0x07,
    LEDREPEAT_8_TIMES   = 0x08,
    LEDREPEAT_9_TIMES   = 0x09,
    LEDREPEAT_10_TIMES  = 0x0A,
    LEDREPEAT_11_TIMES  = 0x0B,
    LEDREPEAT_12_TIMES  = 0x0C,
    LEDREPEAT_13_TIMES  = 0x0D,
    LEDREPEAT_GLOW      = 0x0E
} LedRepeat;


typedef struct {
    LedBlink sos_red_status_blink;     // led 11
    LedRepeat sos_red_status_repeat;   // led 11
    uint8_t  sos_red_duty;             // led 12 (valid values 0-100)
    LedBlink sos_white_status_blink;   // led 21
    LedRepeat sos_white_status_repeat; // led 21
    uint8_t sos_white_duty;            // led 22 (valid values 0-100)
    LedBlink voc_status_blink;         // led 31
    LedRepeat voc_status_repeat;       // led 31
    uint8_t voc_duty;                  // led 32 (valid values 0-100)
    uint8_t brightness;                // 1-255 brightness level
} ReqSetOhcLed;

// Requests - VuC - REQ_VINNUMBER ----------------------------------------------

typedef struct {} ReqVinNumber;

// Requests - VuC - REQ_GETVUCBLESWPARTNUMBER ----------------------------------

typedef struct {} ReqGetVucBleSwPartNumber;

// Requests - VuC - REQ_GETBELTINFORMATION -------------------------------------

typedef struct {} ReqGetBeltInformation;

// Requests - VuC - REQ_GETDIAGERRREPORT ---------------------------------------

typedef struct {} ReqGetDiagErrReport;

// Requests - VuC - REQ_GETVEHICLESTATE ----------------------------------------

typedef struct {} ReqGetVehicleState;

// Requests - VuC - REQ_GETDIDGLOBALSNAPSHOTDATA -------------------------------

typedef struct {} ReqGetDIDGlobalSnapshotData;

// Requests - VuC - REQ_DTCSDETECTED -------------------------------------------

typedef enum {
    DTC_NOT_DETECTED,
    DTC_DETECTED
} DTCDetectStatus;

typedef struct {
    DTCDetectStatus status[64]; // status[0] for dtc1, status[1] for dtc2 and so on
} ReqDTCsDetected;

// Requests - VuC - REQ_PSIMDATA -----------------------------------------------

typedef struct {
    uint8_t data[256];
} ReqPSIMData;

// Requests - VuC - REQ_THERMALMITIGATION --------------------------------------

typedef struct {
    int8_t nad_temperature;
} ReqThermalMitigataion;

/* Requests - VuC - CARCFGPARAMETERFAULT  Ljz do 2018/12/12 */
typedef struct{} ReqCarCfgParameterFault;
typedef struct{} ReqNetworkManageHistory;
typedef struct{} ReqProgramPrecond_Chk;

// Requests - VuC - REQ_ROLLOVERVALUE ---------------------------------------
enum class RolloverItem : uint8_t
{
    ROLLOVER_THRESHOLD_ANGLE = 0x01,
    MIM_ROTATION_DETECTED_TIME = 0x02,
    VEHICLE_SPEED_LIEIT = 0x03,
    MIM_END_POS_DETECTED_TIME = 0x04,
    END_POS_THRESHOLD_ANGLE = 0x05,
    MOUNT_ANGLE = 0x06,
    CONFIG_ITEM_MAX
};

typedef struct
{
    RolloverItem configItem;
} ReqRolloverValue;

// Requests - VuC - REQ_SETROLLOVERCFG ---------------------------------------
// Set Rollover Threshold Angle Request 
typedef struct
{
    uint8_t RolloverThAng;          /* Byte 1 */
} SetRolloverThAng;

//  Set Minimal Rotation Detected Time Request 
typedef struct
{
    uint8_t minRotateDetTime;       /* Byte 1 */
} SetMinRotationDetectTime;

// Set Vehicle Speed Limit Request 
typedef struct
{
    uint8_t VehSpeedLimit;          /* Byte 1 */
} SetVehicleSpeedLimit;

// Set Minimal End Position Detected Time Request 
typedef struct
{
    uint8_t minEndPosDetTime;       /* Byte 1 */
} SetMinEndPosDetectTime;

// Set End Position Threshold Angle Request 
typedef struct
{
    uint8_t endPosThAngle;          /* Byte 1 */
} SetEndPosThresholdAngle;

// Set Mount Angle Request 
typedef struct
{
    uint16_t mountAngle_x;              /* Byte 1 ~ 2 */
    uint16_t mountAngle_y;              /* Byte 3 ~ 4 */
    uint16_t mountAngle_z;              /* Byte 5 ~ 6 */
} SetMoungAngle;

// Set Rollover cfg Request 
typedef struct
{
    RolloverItem configItem;
    uint8_t      length;
    uint8_t      data[6];
} ReqSetRolloverCfg;

// Requests - VGM

typedef enum
{
    NO_NAVIGATION_SOLUTION = 0,
    THREE_SATELITES_SOLUTION = 1,
    MORE_THAN_THREE_SATELITES_SOLUTION = 2,
    COLD_START_FIX = 3,
} SatltPosnSts;

typedef struct {
    int32_t PosnLat; // Latitude
    int32_t PosnLgt; // Longitude
    uint32_t PosnAlti; // Altitide from mean sea level
    uint32_t PosnSpd; // Speed
    uint32_t PosnVHozl; // Horizontal velocity
    int32_t PosnVVert; // Vertical velocity
    uint16_t PosnDir; // Heading from true north
    uint8_t TiForYr; // UTC year
    uint8_t TiForMth; // UTC month
    uint8_t TiForDay; // UTC day
    uint8_t TiForHr; // UTC hour
    uint8_t TiForMins; // UTC minute
    uint8_t TiForSec; // UTC second
    bool SatltSysNo1InUse; // GPS used for positioning
    bool SatltSysNo2InUse; // GLONASS used for positioning
    bool SatltSysNo3InUse; // Galileo used for positioning
    bool SatltSysNo4InUse; // SBAS used for positioning	Enum
    bool SatltSysNo5InUse; // QZSS L1 used for positioning
    bool SatltSysNo6InUse; // QZSS L1-SAIF used for positioning
    SatltPosnSts SatltPosnStsPrm1; // Navigation solution status
    bool SatltPosnStsPrm2; // DGPS usage in solution
    bool SatltPosnStsPrm3; // Self ephemeris data usage in solution
    uint8_t NoOfSatltForSysNo1; // Number of GPS satellites used for positioning
    uint8_t NoOfSatltForSysNo2; // Number of GLONASS satellites used for positioning
    uint8_t NoOfSatltForSysNo3; // Number of Galileo satellites used for positioning
    uint8_t NoOfSatltForSysNo4; // Number of SBAS satellites used for positioning
    uint8_t NoOfSatltForSysNo5; // Number of QZSS L1 satellites used for positioning
    uint8_t NoOfSatltForSysNo6; // Number of QZSS L1-SAIF satellites used for positioning
    uint8_t PrePosnDil; // Position Dilution Of Precision
    uint8_t PreHozlDil; // Horizontal Dilution Of Precision
    uint8_t PreVertDil; // Vertical Dilution Of Precision
    uint8_t PreTiDil; // Time Dilution Of Precision
} ReqSendPosnFromSatltCon;

// Requests - IHU

// Requests - Data-storage

// Requests - Data-storage - REQ_GETDSPARAMETER ------------------------------

typedef struct
{
    char collection[N_STR_MAX];
    char name[N_STR_MAX];
} ReqGetDSParameter;

// Requests - Message-processor


// Requests - Message-processor - REQ_VUCSWVERSION ---------------------------

typedef struct {} ReqVucSwVersion;

// Requests - Message-processor - REQ_BLESWVERSION ---------------------------

typedef struct {} ReqBleSwVersion;

// Requests - Message-processor - REQ_LASTBUBSTATE ---------------------------

typedef struct {} ReqLastBubState;

// Requests - Message-processor - REQ_CARUSAGEMODE ---------------------------

typedef struct {} ReqCarUsageMode;

// Requests - Message-processor - REQ_CARMODE --------------------------------

typedef struct {} ReqCarMode;

// Requests - Message-processor - REQ_ELECTENGLVL ----------------------------

typedef struct {} ReqElectEngLvl;

// Requests - Message-processor - REQ_GETVUCPOWERMODE ------------------------

typedef struct {} ReqGetVucPowerMode;

// Requests - Message-processor - REQ_GETVUCWAKEUPREASON ---------------------

typedef struct {} ReqGetVucWakeupReason;

// Requests - Message-processor - REQ_GETWIFIDATA ----------------------------

typedef struct {} ReqGetWifiData;

// Requests - Message-processor - REQ_GETPSIMAVAILABILITY --------------------

typedef struct {} ReqGetPSIMAvailability;

// Requests - Message-processor - REQ_GETFUELTYPE ----------------------------

typedef struct {} ReqGetFuelType;

// Requests - Message-processor - REQ_GETPROPULSIONTYPE ----------------------

typedef struct {} ReqGetPropulsionType;

// Requests - Message-processor - REQ_GETOHCBTNSTATE -------------------------

typedef struct {} ReqGetOhcBtnState;

// Requests - Message-processor - REQ_GETNUMBEROFDOORS -----------------------

typedef struct {} ReqGetNumberOfDoors;

// Requests - Message-processor - REQ_GETSTEERINGWHEELPOSITION ---------------

typedef struct {} ReqGetSteeringWheelPosition;

// Requests - Message-processor - REQ_GETFUELTANKVOLUME ----------------------

typedef struct {} ReqGetFuelTankVolume;

// Requests - Message-processor - REQ_GETASSISTANCESERVICES ------------------

typedef struct {} ReqGetAssistanceServices;

// Requests - Message-processor - REQ_GETAUTONOMOUSDRIVE ---------------------

typedef struct {} ReqGetAutonomousDrive;

// Requests - Message-processor - REQ_GETCOMBINEDINSTRUMENT ------------------

typedef struct {} ReqGetCombinedInstrument;

// Requests - Message-processor - REQ_GETTELEMATICMODULE ---------------------

typedef struct {} ReqGetTelematicModule;

// Requests - Message-processor - REQ_GETCONNECTIVITY ------------------------

typedef struct {} ReqGetConnectivity;

// Requests - Message-processor - REQ_GETCONNECTEDSERVICEBOOKING -------------

typedef struct {} ReqGetConnectedServiceBooking;

// Requests - Message-processor - REQ_GETCARCFGTHEFTNOTIFICATION ----------------------------

typedef struct {} ReqGetTheftNotification;

// Requests - Message-processor - REQ_GETCARCFGSTOLENVEHICLETRACKING ----------------------------

typedef struct {} ReqGetStolenVehicleTracking;

// Requests - Message-processor - REQ_GETGNSSRECEIVER ----------------------------

typedef struct {} ReqGetGNSS;

// Requests - Message-processor - REQ_GETAPPROVEDCELLULARSTANDARDS ---------------------

typedef struct {} ReqGetApprovedCellularStandards;

// Requests - Message-processor - REQ_GETDIGITALVIDEORECORDER ---------------------

typedef struct {} ReqGetDigitalVideoRecorder;

// Requests - Message-processor - REQ_GETDRIVERALERTCONTROL ------------------

typedef struct {} ReqGetDriverAlertControl;

// Requests - Message-processor - REQ_DOORSTATE ----------------------------

typedef struct {} ReqGetDoorState;

// Requests - Multi - REQ_GETMSGCATVERSION -----------------------------------

typedef struct {
    uint8_t version[3];
} ReqGetMsgCatVer;

// Requests - Multi - REQ_GETTCAMHWVERSION -----------------------------------

typedef struct {} ReqGetTcamHwVer;

// Requests - Multi - REQ_GETVUCSWBUILD --------------------------------------

typedef struct {} ReqGetBleSwBuild;

// Requests - Multi - REQ_GETBLESWBUILD --------------------------------------

typedef struct {} ReqGetVucSwBuild;

// Requests - Multi - REQ_GETFBLSWBUILD --------------------------------------

typedef struct {} ReqGetFblSwBuild;

// Requests - Multi - REQ_GETRFVERSION ---------------------------------------

typedef struct {} ReqGetRfVersion;

// Requests - Test

// Requests - Test - REQ_VGMTESTMSG ------------------------------------------

typedef struct
{
    uint8_t id;
    bool return_ack;
    bool return_res;
    char company[N_STR_MAX];
} ReqVGMTestMsg;

// Requests - Test - REQ_IHUTESTMSG ------------------------------------------

typedef struct
{
    uint8_t id;
    bool return_ack;
    bool return_res;
    char city[N_STR_MAX];
} ReqIHUTestMsg;

// Requests - Test - REQ_TEST_GETCARCONFIG -----------------------------------

typedef struct
{
    uint8_t n_req;
    uint8_t delay;
} ReqTestGetCarConfig;


// Requests - Test - REQ_TEST_SETFAKEVALUEINT --------------------------------

typedef struct
{
    char collection[N_STR_MAX];
    char name[N_STR_MAX];
    uint32_t value;
} ReqTestSetFakeValueInt;

// Requests - Test - REQ_TEST_SETFAKEVALUESTR --------------------------------

typedef struct
{
    char collection[N_STR_MAX];
    char name[N_STR_MAX];
    char value[N_STR_MAX];
} ReqTestSetFakeValueStr;

// Request Test
typedef struct
{
    uint16_t localCfgParam;
    uint32_t localCfgParamValue;
} ReqTestSetLocalConfig;
// VehicleComm - Responses

/**
    @brief A list of all supported response messages.

    A ResponseID can be one of the responses listed in this enum.
*/
typedef enum {
    RES_UNKNOWN,
    RES_REQUESTSENT,
    // VuC
    RES_HORNNLIGHT,
    RES_ANTENNASWITCH,
    RES_GETCARCONFIG,
    RES_CARCONFIGFACTRESTORE,
    RES_ADCVALUE,
    RES_VUCTRACE_CONFIG,
    RES_VUCTRACE_CONTROL,
    RES_BLE_DISCONNECT,
    RES_BLE_DISCONNECTALL,
    RES_BLE_GENERATEBDAK,
    RES_BLE_GETBDAK,
    RES_BLE_CONFIG_DATACOMSERVICE,
    RES_BLE_CONFIG_IBEACON,
    RES_BLE_CONFIG_GENERICACCESS,
    RES_BLE_STARTPARINGSERVICE,
    RES_BLE_STOPPARINGSERVICE,
    RES_BLE_STARTADVERTISING,
    RES_BLE_STOPADVERTISING,
    RES_DTCCTLSETTINGS,
    RES_GETVUCBLESWPARTNUMBER,
    RES_GETBELTINFORMATION,
    RES_GETDIAGERRREPORT,
    RES_GETVEHICLESTATE,
    RES_GETDIDGLOBALSNAPSHOTDATA,
    RES_DTCSDETECTED,
    RES_PSIMDATA,
    RES_THERMALMITIGATION,
    RES_CARCFGPARAMETERFAULT,
    RES_NETWORK_MANAGEMENT_HISTORY,
    RES_PROGRAM_PRECOND_CHK,
#if 1 // nieyj
    RES_VFCACTIVATE,
#endif
    //uia93888 remote start engine
    RES_RMTENGINESECURITYRANDOM,
    RES_RMTENGINESECURITYRESULT,
    RES_RMTENGINE,
    RES_RMTSTATUS,
    RES_ENGINESTATES,
    RES_DELAYENGINERUNNINGTIME,
    RES_OPERATEREMOTECLIMATE,
    RES_OPERATERMTSEATHEAT,
    RES_OPERATERMTSEATVENTILATION,
    RES_OPERATERMTSTEERWHLHEAT,
    RES_PARKINGCLIMATEOPER,
    //end uia93888
    RES_ROLLOVERVALUE,
    RES_SETROLLOVERCFG,
    // VGM
    // Data-storage
    RES_GETDSPARAMETER_INT,
    RES_GETDSPARAMETER_STR,
    RES_GETDSPARAMETER_VECT,
    RES_CARMODE,
    RES_CARUSAGEMODE,
    RES_ELECTENGLVL,
    RES_VINNUMBER,
    RES_GETVUCPOWERMODE,
    RES_GETVUCWAKEUPREASON,
    // Message-processor
    RES_VUCSWVERSION,
    RES_BLESWVERSION,
    RES_LASTBUBSTATE,
    RES_GETWIFIDATA,
    RES_GETPSIMAVAILABILITY,
    RES_GETFUELTYPE,
    RES_GETPROPULSIONTYPE,
    RES_GETOHCBTNSTATE,
    RES_GETNUMBEROFDOORS,
    RES_GETSTEERINGWHEELPOSITION,
    RES_GETFUELTANKVOLUME,
    RES_GETASSISTANCESERVICES,
    RES_GETAUTONOMOUSDRIVE,
    RES_GETCOMBINEDINSTRUMENT,
    RES_GETTELEMATICMODULE,
    RES_GETCONNECTIVITY,
    RES_GETCONNECTEDSERVICEBOOKING,
    RES_GETCARCFGTHEFTNOTIFICATION,
    RES_GETCARCFGSTOLENVEHICLETRACKING,
    RES_GETGNSSRECEIVER,
    RES_GETAPPROVEDCELLULARSTANDARDS,
    RES_GETDIGITALVIDEORECORDER,
    RES_GETDRIVERALERTCONTROL,
    RES_GETDOORSTATE,
    // Multi
    RES_GETVUCSWBUILD,
    RES_GETBLESWBUILD,
    RES_GETMSGCATVERSION,
    RES_GETTCAMHWVERSION,
    RES_GETRFVERSION,
    RES_GETFBLSWBUILD,
    // Test
    RES_VGMTESTMSG,
    RES_IHUTESTMSG,
} ResponseID;

const char *const ResponseStr[] = {
    "RES_UNKNOWN",
    "RES_REQUESTSENT",
    // VuC
    "RES_HORNNLIGHT",
    "RES_ANTENNASWITCH",
    "RES_GETCARCONFIG",
    "RES_CARCONFIGFACTRESTORE",
    "RES_ADCVALUE",
    "RES_VUCTRACE_CONFIG",
    "RES_VUCTRACE_CONTROL",
    "RES_BLE_DISCONNECT",
    "RES_BLE_DISCONNECTALL",
    "RES_BLE_GENERATEBDAK",
    "RES_BLE_GETBDAK",
    "RES_BLE_CONFIG_DATACOMSERVICE",
    "RES_BLE_CONFIG_IBEACON",
    "RES_BLE_CONFIG_GENERICACCESS",
    "RES_BLE_STARTPARINGSERVICE",
    "RES_BLE_STOPPARINGSERVICE",
    "RES_BLE_STARTADVERTISING",
    "RES_BLE_STOPADVERTISING",
    "RES_DTCCTLSETTINGS",
    "RES_GETVUCBLESWPARTNUMBER",
    "RES_GETBELTINFORMATION",
    "RES_GETDIAGERRREPORT",
    "RES_GETVEHICLESTATE",
    "RES_GETDIDGLOBALSNAPSHOTDATA",
    "RES_DTCSDETECTED",
    "RES_PSIMDATA",
    "RES_THERMALMITIGATION",
    "RES_CARCFGPARAMETERFAULT",
    "RES_NETWORK_MANAGEMENT_HISTORY",
    "RES_PROGRAM_PRECOND_CHK",
#if 1 // nieyj
    "RES_VFCACTIVATE",
#endif
    //uia93888 remote start engine
    "RES_RMTENGINESECURITYRANDOM",
    "RES_RMTENGINESECURITYRESULT",
    "RES_RMTENGINE",
    "RES_RMTSTATUS",
    "RES_ENGINESTATES",
    "RES_DELAYENGINERUNNINGTIME",
    "RES_OPERATEREMOTECLIMATE",
    "RES_OPERATERMTSEATHEAT",
    "RES_OPERATERMTSEATVENTILATION",
    "RES_OPERATERMTSTEERWHLHEAT",
    "RES_PARKINGCLIMATEOPER",
    //end uia93888
    "RES_ROLLOVERVALUE",
    "RES_SETROLLOVERCFG",
    // VGM
    // Data-storage
    "RES_GETDSPARAMETER_INT",
    "RES_GETDSPARAMETER_STR",
    "RES_GETDSPARAMETER_VECT",
    "RES_CARMODE",
    "RES_CARUSAGEMODE",
    "RES_ELECTENGLVL",
    "RES_VINNUMBER",
    "RES_GETVUCPOWERMODE",
    "RES_GETVUCWAKEUPREASON",
    // Message-processor
    "RES_VUCSWVERSION",
    "RES_BLESWVERSION",
    "RES_LASTBUBSTATE",
    "RES_GETWIFIDATA",
    "RES_GETPSIMAVAILABILITY",
    "RES_GETFUELTYPE",
    "RES_GETPROPULSIONTYPE",
    "RES_GETOHCBTNSTATE",
    "RES_GETNUMBEROFDOORS",
    "RES_GETSTEERINGWHEELPOSITION",
    "RES_GETFUELTANKVOLUME",
    "RES_GETASSISTANCESERVICES",
    "RES_GETAUTONOMOUSDRIVE",
    "RES_GETCOMBINEDINSTRUMENT",
    "RES_GETTELEMATICMODULE",
    "RES_GETCONNECTIVITY",
    "RES_GETCONNECTEDSERVICEBOOKING",
    "RES_GETCARCFGTHEFTNOTIFICATION",
    "RES_GETCARCFGSTOLENVEHICLETRACKING",
    "RES_GETGNSSRECEIVER",
    "RES_GETAPPROVEDCELLULARSTANDARDS",
    "RES_GETDIGITALVIDEORECORDER",
    "RES_GETDRIVERALERTCONTROL",
    "RES_GETDOORSTATE",
    // Multi
    "RES_GETVUCSWBUILD",
    "RES_GETBLESWBUILD",
    "RES_GETMSGCATVERSION",
    "RES_GETTCAMHWVERSION",
    "RES_GETRFVERSION",
    "RES_GETFBLSWBUILD",
    // Test
    "RES_VGMTESTMSG",
    "RES_IHUTESTMSG",
};

// Responses - RES_REQUESTSENT -----------------------------------------

// Responses - VuC

// Responses - VuC - RES_HORNNLIGHT ------------------------------------------

typedef enum {
    HL_OK = 0,
    HL_SEQUENCE_ONGOING = 1,
    HL_WRONG_USAGE_MODE = 2,
    HL_COMMUNICATION_ERROR = 3,
} HNLReturnCode;

typedef struct
{
    HNLReturnCode return_code;
    uint8_t carLocatorSts;
} ResHornNLight;

// Responses - VuC - RES_ANTENNASWITCH -----------------------------------------

typedef ReqAntennaSwitch ResAntennaSwitch;

// Responses - VuC - RES_GETCARCONFIG ------------------------------------------

typedef struct
{
    uint16_t param_id;
    uint8_t validity;
    uint8_t value;
} ResGetCarConfig;


// Responses - VuC - RES_CARCONFIGFACTRESTORE ----------------------------------

typedef struct {} ResCarConfigFactRestore;

// Responses - VuC - RES_ADC ---------------------------------------------------

typedef struct
{
    AdcId id;
    int16_t ref_value;
    int16_t adc_value;
} ResAdcValue;

// Responses - VuC - RES_VUCTRACE_CONFIG ---------------------------------------

typedef enum
{
    TRACE_CONFIGURATION_CHANGED,
    TRACE_MODULE_ID_NOT_SUPPORTED,
} TraceCfgStatus;

typedef struct
{
    TraceModuleId module_id;
    TraceCfgStatus status;
} TraceModuleCfgStatus;

typedef struct
{
    TraceBus bus;
    TraceModuleCfgStatus config[20];
} ResVucTraceConfig;

// Responses - VuC - RES_VUCTRACE_CONTROL --------------------------------------

typedef struct
{
    TracingMode mode;
} ResVucTraceControl;

// Responses - VuC - RES_BLE_DISCONNECT ----------------------------------------

typedef enum {
    BLE_SUCCESS = 0,
    BLE_FAILURE,
} BleReturnCode;

typedef struct
{
    BleReturnCode status;
} ResBleDisconnect;

// Responses - VuC - RES_BLE_DISCONNECTALL -------------------------------------

typedef struct
{
    BleReturnCode status;
} ResBleDisconnectAll;

// Responses - VuC - RES_BLE_GENERATEBDAK --------------------------------------

typedef struct
{
    BleReturnCode status;
} ResBleGenBdak;

// Responses - VuC - RES_BLE_GETBDAK -------------------------------------------

typedef struct
{
    BleReturnCode status;
    char bdak[16];
} ResBleGetBdak;

// Responses - VuC - RES_BLE_CONFIG_DATACOMSERVICE -----------------------------

typedef struct {
    BleReturnCode status;
} ResBleCfgDataCommService;

// Responses - VuC - RES_BLE_CONFIG_IBEACON ------------------------------------

typedef struct {
    BleReturnCode status;
} ResBleCfgIbeacon;

// Responses - VuC - RES_BLE_CONFIG_GENERICACCESS ------------------------------

typedef struct {
    BleReturnCode status;
} ResBleCfgGenAcc;

// Responses - VuC - RES_BLE_STARTPARINGSERVICE --------------------------------

typedef struct {
    BleReturnCode status;
} ResBleStartPairService;

// Responses - VuC - RES_BLE_STOPPARINGSERVICE ---------------------------------

typedef struct {
    BleReturnCode status;
} ResBleStopPairService;

// Responses - VuC - RES_BLE_STARTADVERTISING ----------------------------------

typedef struct {
    BleReturnCode status;
} ResBleStartAdvertising;

// Responses - VuC - RES_BLE_STOPADVERTISING -----------------------------------

typedef struct {
    BleReturnCode status;
} ResBleStopAdvertising;

// Responses - VuC - RES_DTCCTLSETTINGS ----------------------------------------

typedef enum {
    DTC_CLEAR_DIAG_INFO_IN_PROGRESS,
    DTC_CLEAR_DIAG_INFO_ERROR,
    DTC_CLEAR_DIAG_INFO_FINISHED,
    DTC_CTL_DTC_SETTING_OFF,
    DTC_CTL_DTC_SETTING_ON,
    DTC_CLEAR_DIAG_INFO_ERROR_UNSUPPORTED_DTC_IDENTIFIER
} DtcCtlSettingStatus;

typedef struct {
    DtcCtlSettingStatus status;
} ResDtcCtlSettings;

// Responses - VuC - RES_GETMSGCATVERSION --------------------------------------

typedef struct {
    uint8_t version[3];
} ResGetMsgCatVer;

// Responses - VuC - RES_GETTCAMHWVERSION --------------------------------------

typedef enum {
    TCAM_EOL_HW_VERSION_UNDEFINED_BEGIN,
    TCAM_EOL_HW_VERSION_2_0,
    TCAM_EOL_HW_VERSION_2_1,
    TCAM_EOL_HW_VERSION_2_2,
    TCAM_EOL_HW_VERSION_3_0,
    TCAM_EOL_HW_VERSION_4_0 = 0x10,
    TCAM_EOL_HW_VERSION_SOP = 0x20,
    TCAM_EOL_HW_VERSION_UNDEFINED_END = 0xFF
} TCAMEolHwVersion;

typedef enum {
    TCAM_EOL_DATA_UNDEFINED_BEGIN,
    TCAM_EOL_DATA_INITIAL_DEF,
    TCAM_EOL_DATA_NEW_DEFINITION,
    TCAM_EOL_DATA_UNDEFINED_END = 0xFF
} TCAMEolData;

typedef enum {
    TCAM_EOL_HW_VARIANT_UNDEFINED_BEGIN,
    TCAM_EOL_HW_VARIANT_V1_EU_A2C134793XX,
    TCAM_EOL_HW_VARIANT_V2_NA_A2C134794XX,
    TCAM_EOL_HW_VARIANT_V3_JP_A2C134795XX,
    TCAM_EOL_HW_VARIANT_V4_ROW_A2C134796XX,
    TCAM_EOL_HW_VARIANT_V5_RU_A2C134797XX,
    TCAM_EOL_HW_VARIANT_V6_CHN_A2C134798XX,
    TCAM_EOL_HW_VARIANT_UNDEFINED_END = 0xFF
} TCAMEolHwVariant;

typedef struct {
    TCAMEolHwVersion eol_hw_version;
    TCAMEolData eol_data;
    TCAMEolHwVariant eol_hw_variant;
    uint8_t eol_volvo_ecu_serial_number[4];
    uint8_t eol_volvo_delivery_part_number[11];
    uint8_t eol_volvo_core_part_number[11];
    uint16_t eol_volvo_production_year;
    uint8_t eol_volvo_production_month;
    uint8_t eol_volvo_production_day;
} ResGetTcamHwVer;


//Responses - VuC - RES_GETVUCBLESWPARTNUMBER -----------------------------------

typedef struct {
    char vuc_part_number[8];
    char vuc_version[3];
    char ble_part_number[8];
    char ble_version[3];
} ResGetVucBleSwPartNumber;

//Responses - VuC - RES_GETRFVERSION -------------------------------------------

typedef enum {
    RF_SW_VARIANT_JAPAN_315MHZ = 0x10,
    RF_SW_VARIANT_EU_US_CH_434MHZ,
    RF_SW_VARIANT_KOREA_434MHZ
} RFSWVariant;

typedef enum {
    RF_HW_VARIANT_NVM_OF_RF_ERASED,
    RF_HW_VARIANT_V1_434MHZ_RESTOFWORLD_N_KOREA = 0x0100,
    RF_HW_VARIANT_V7_315MHZ_JAPAN = 0x0700,
} RFHWVariant;

typedef struct {
    uint16_t sw_version;
    uint8_t hw_version[4];
    RFSWVariant sw_variant;
    RFHWVariant hw_variant;
    uint8_t hw_serial_number[8];
} ResGetRfVersion;

//Responses - VuC - RES_GETBELTINFORMATION -------------------------------------

typedef enum {
    BELT_STATE_UNLOCKED,
    BELT_STATE_LOCKED
} BeltLockState;

typedef enum {
    BELT_STATUS_OK,
    BELT_STATUS_NOK
} BeltLockStatus;

typedef enum {
    BELT_LOCK_EQUIPPED,
    BELT_LOCK_NOT_EQUIPPED
} BeltLockEquippedStatus;

typedef struct {
    BeltLockState lock_state;
    BeltLockStatus lock_status;
    BeltLockEquippedStatus equipped_status;
} BeltInfo;

typedef struct
{
    BeltInfo driver;
    BeltInfo passenger;
    BeltInfo second_row_left;
    BeltInfo second_row_middle;
    BeltInfo second_row_right;
    BeltInfo third_row_left;
    BeltInfo third_row_right;
} ResGetBeltInformation;

//Responses - VuC - RES_GETDIAGERRREPORT ---------------------------------------

typedef enum
{
    DIAG_PASSED = 0,
    DIAG_FAILED = 1
} DiagErrStatus;

typedef enum
{
    DIAG_TEST_NOT_RUNNING_OR_NOT_COMPLETED,
    DIAG_TEST_COMPLETED,
} DiagErrState;

typedef struct
{
    DiagErrStatus phone_ant_open_load_test_status;
    DiagErrState phone_ant_open_load_test_state;
    DiagErrStatus ecall_ant_open_load_test_status;
    DiagErrState ecall_ant_open_load_test_state;
} ResGetDiagErrReport;

// Responses - VuC - RES_GETVEHICLESTATE ---------------------------------------

typedef enum {
    CAR_NORMAL = 0,
    CAR_TRANSPORT = 1,
    CAR_CARMODFCY = 2,
    CAR_CRASH = 3,
    CAR_DYNO = 5,
} CarModeState;

typedef enum {
    CAR_ABANDONED = 0,
    CAR_INACTIVE = 1,
    CAR_USGMODCNVINC = 2,
    CAR_ACTIVE = 11,
    CAR_DRIVING = 13,
} CarUsageModeState;

typedef struct
{
    CarModeState carmode;
    CarUsageModeState usagemode;
} ResGetVehicleState;

// Responses - VuC - RES_GETDIDGLOBALSNAPSHOTDATA ------------------------------

typedef struct {
    uint8_t dd00[4]; // See flow for more details
    uint8_t dd01[3];
    uint8_t dd02;
    uint8_t dd0a;
    uint8_t dd0c;
} ResGetDIDGlobalSnapshotData;

// Responses - VuC - RES_DTCSDETECTED ------------------------------------------

typedef struct {} ResDTCsDetected;

// Responses - VuC - RES_PSIMDATA ----------------------------------------------

// Responses - VuC - RES_THERMALMITIGATION -------------------------------------
typedef struct {} ResThermalMitigation;

/* Responses - VuC - CARCFGPARAMETERFAULT  Ljz do 2018/12/12 */
/*numInvalidCfg:  Number of invalid car configuration parameter values recognised
*/
#pragma pack(push)
#pragma pack(1)
typedef struct{
    uint16_t        numData;
    uint8_t          value;
}paraInvalidCfg;
typedef struct{
    uint8_t            numInvalidCfg;
    paraInvalidCfg   parameterFault[11];
} ResCarCfgParameterFault;

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
} ResNetworkManageHistory;
typedef struct{
   uint8_t UsgModSts;                            /*usage mode*/
   uint16_t VehSpdLgt;                            /*vehicle speed*/
   uint8_t KL30Sts;                              /*voltage status */
} ResProgramPrecond_Chk;
#pragma pack(pop)

#if 1 // nieyj
// Responses - VuC - RES_VFCACTIVATE ------------------------------

typedef struct {
    uint8_t         id;    
    uint8_t         resule;   // 0:Success
} ResVFCActivate;
#endif

//uia93888 remote start engine
// Response Data-storage - RES_RMTENGINESECURITYRANDOM-------------------
typedef struct{
    uint8_t imobRemMgrChk       :2;
    uint8_t Reserved            :6;
    uint8_t imobDataRemMgrChk0;
    uint8_t imobDataRemMgrChk1;
    uint8_t imobDataRemMgrChk2;
    uint8_t imobDataRemMgrChk3;
    uint8_t imobDataRemMgrChk4;
} Res_RMTEngineSecurityRandom;

// Response - Data-storage - RES_RMTENGINESECURITYRESULT-----------------
typedef struct
{
    uint8_t imobVehRemMgrSts :2;
    uint8_t reserved  :6;
    uint8_t spdLimRemMgrSts;
} Res_RMTEngineSecurityResult;

//Response - remote start engine - RES_RMTENGINE -------------------------------------------
typedef enum{
    EngStl_Ini = 0x0,
    EngStl_Awake,
    EngStl_Rdy,
    EngStl_PreStrtg,
    EngStl_StrtgInProgs,
    EngSt1_RunngRunng,
    EngSt1_RunngStb,
    EngSt1_RunngStrtgInProgs,
    EngSt1_RunngRemStrtd,
    EngSt1_AftRun
}EngStlWdSts;

typedef enum{
    ErsStrtRes_ErsStrtNotSet = 0x0,
    ErsStrtRes_ErsStrtSuccess,  //success, others is failed
    ErsStrtRes_ErsStrtInhMaxNoStart,
    ErsStrtRes_ErsStrtInhCarUnlocked,
    ErsStrtRes_ErsStrtInhKeyInCar,
    ErsStrtRes_ErsStrtInhDoorOpen,
    ErsStrtRes_ErsStrtInhHoodOpen,
    ErsStrtRes_ErsStrtInhGearNotP,
    ErsStrtRes_ErsStrtInhUserInCar,
    ErsStrtRes_ErsStrtInhPedalPressed,
    ErsStrtRes_ErsStrtInhLoFuel,
    ErsStrtRes_ErsStrtInhLoBatt,
    ErsStrtRes_ErsStrtInhEngCoolant,
    ErsStrtRes_ErsStrtInhEngFault,
    ErsStrtRes_ErsStrtInhOther,
    ErsStrtRes_ErsStrtAbrtEngFault,
    ErsStrtRes_ErsStrtAbrtEngCoolant,
    ErsStrtRes_ErsStrtAbrtLoFuel,
    ErsStrtRes_ErsStrtAbrtLoBatt,
    ErsStrtRes_ErsStrtAbrtOther
}ErsStrtRes;

typedef enum
{
    ErsStrtApplSts_ErsStsOff = 0x0,
    ErsStrtApplSts_ErsStsStrtg,  //enter remote start mode
    ErsStrtApplSts_ErsStsRunng
}ErsStrtApplSts;
typedef struct
{
    ErsStrtApplSts ersStrtApplSts;
    EngStlWdSts    engSt1WdSts;
    ErsStrtRes     ersStrtRes;
}Res_RMTEngine;

// Response - Data-storage - RES_RMTSTATUS-----------------------------------

typedef struct
{
    uint8_t    vehSpdIndcd          :4;
    uint8_t    vehSpdIndcdQly       :4; //0 UndefindDataAccur; 1 TmpUndefdData; 2 DataAccurNotWithinSpcn; 3 AccurData
    uint8_t    doorPassLockSts      :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    uint8_t    doorPassSts          :4; //0 Ukwn; 1 Opend; 2 Clsd
    
    uint8_t    doorLeReLockSrs      :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    uint8_t    doorLeReSts          :4; //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t    doorDrvrSts          :4; //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t    doorDrvrLockSts      :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    
    uint8_t    doorRiReLockSts      :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    uint8_t    doorRiReSts          :4; //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t    lockgCenSts          :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    uint8_t    trLockSts            :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    
    uint8_t    trSts                :4; //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t    hoodSts              :4; //0 Ukwn; 1 Opend; 2 Clsd
    uint8_t    lockgCenStsForUsrFb  :4; //0 LockStsUkwn; 1 Unlock; 2 Lockd; 3 SafeLockd
    uint8_t    engOilLvlSts         :4; //0 oilLv10k; 1 oilLvlLo1; 2 oilLvlLo2; 3 oilLvlHi; 4 OilLvlSrvRqrd; 5 Resd
    
    uint8_t    epbSts               :4;
    uint8_t    fuLvlIndcd           :4; //0 - 1023(0.0; 0.2)
    uint8_t    fuLvlLoIndcn         :4; //0 NoTrig; 1 trig
    uint8_t    fulLvlLoWarn         :4; //0 NoTrig; 1 trig

    uint8_t    sunroofPosnSts       :4;
    uint8_t    winPosnStsAtDrv      :4;
    uint8_t    winPosnStsAtPass     :4;
    uint8_t    winPosnStsReLe       :4;
    
    uint8_t    winPosnStsReRi       :4;
    uint8_t    ventilateStatus      :4;
    uint8_t    interiorPM25Lvl      :4;
    uint8_t    exteriorPM25Lvl      :4;

    uint16_t    interiorPM25Vlu;
    uint16_t    exteriorPM25Vlu;
}ResRMTStatus;

// Response - Data-storage - RES_DELAYENGINERUNNINGTIME-----------------------------------
typedef enum{
    NoYes1_No = 0x0,
    NoYes1_Yes
} ErsDelayTiCfm;

typedef struct{
    ErsDelayTiCfm  ersDelayTiCfm;
} Res_DelayEngRunngTime;

// Response - Data-storage - RES_OPERATEREMOTECLIMATE-----------------------------------
typedef enum{
    OnOff1_Off = 0x0,
    OnOff1_On
} RemStrtClimaActv;

typedef enum{
    ClimaWarn_NoWarn = 0x0,
    ClimaWarn_FuLo,
    ClimaWarn_BattLo,
    ClimaWarn_FuAndBatLo,
    ClimaWarn_TLo,
    ClimaWarn_THi,
    ClimaWarn_Error,
    ClimaWarn_HVError,
    ClimaWarn_ActivnLimd,
} PrkgClimaWarn;
typedef struct{
    RemStrtClimaActv remStrtClimaActv;
    PrkgClimaWarn prkgClimaWarn;
} Res_OperateRMTClimate;


// Response - Data-storage - RES_OPERATERMTSEATHEAT,
typedef struct{
    uint8_t SeatHeatgAvlStsRowFrstLe    :3; //range 0 ~ 7
    uint8_t SeatHeatgLvlStsRowFrstLe    :2; //range 0 ~ 3
    uint8_t Reserved1                   :3;

    uint8_t SeatHeatgAvlStsRowFrstRi    :3; //range 0 ~ 7
    uint8_t SeatHeatgLvlStsRowFrstRi    :2; //range 0 ~ 3
    uint8_t Reserved2                   :3;

    uint8_t SeatHeatgAvlStsRowSecLe     :3; //range 0 ~ 7
    uint8_t SeatHeatgLvlStsRowSecLe     :2; //range 0 ~ 3
    uint8_t Reserved3                   :3;

    uint8_t SeatHeatgAvlStsRowSecRi     :3; //range 0 ~ 7
    uint8_t SeatHeatgLvlStsRowSecRi     :2; //range 0 ~ 3
    uint8_t Reserved4                   :3;

}Res_OperateRMTSeatHeat_S;


// Response - Data-storage - RES_OPERATERMTSEATVENTILATION---------------------------------
typedef struct{
    uint8_t SeatVentAvlStsRowFrstLe    :3; //range 0 ~ 7
    uint8_t SeatVentnLvlStsRowFrstLe    :2; //range 0 ~ 3
    uint8_t Reserved1                   :3;

    uint8_t SeatVentAvlStsRowFrstRi    :3; //range 0 ~ 7
    uint8_t SeatVentnLvlStsRowFrstRi    :2; //range 0 ~ 3
    uint8_t Reserved2                   :3;

    uint8_t SeatVentAvlStsRowSecLe     :3; //range 0 ~ 7
    uint8_t SeatVentnLvlStsRowSecLe     :2; //range 0 ~ 3
    uint8_t Reserved3                   :3;

    uint8_t SeatVentAvlStsRowSecRi     :3; //range 0 ~ 7
    uint8_t SeatVentnLvlStsRowSecRi     :2; //range 0 ~ 3
    uint8_t Reserved4                   :3;

}Res_OperateRMTSeatVenti_S;

//Response - Vuc - RES_OPERATERMTSTEERWHLHEAT------------------------------------------------
typedef struct{
    uint8_t SteerWhlHeatgAvlSts         :3; //range: 0~7
    uint8_t Reserved                    :5;
}Res_OperateRMTSteerWhlHeat_S;


//Response - Vuc - RES_PARKINGCLIMATEOPER-----------------------------------------------------
typedef struct{
    uint8_t prkgClimaWarn   :4; //range: 0 ~ 8
    uint8_t climaActv       :1; //0: off, 1: on
    uint8_t reserved        :3;
}Res_ParkingClimateOper_S;

//uia93888 end




typedef ReqPSIMData ResPSIMData;

//Response - read rollover cfg - RES_ROLLOVERVALUE -------------------------------------------
typedef struct 
{
    RolloverItem configItem;
    uint8_t length;
    uint8_t data[6];
}ResRolloverValue;

//Response - set rollover cfg - RES_SETROLLOVERCFG -------------------------------------------
typedef enum
{
    DIAG_OP_SUCCESS,
    DIAG_OP_FAILURE,
    DIAG_OP_PARAM_INV,
    DIAG_OP_NVM_FAILURE,
    DIAG_OP_ACCESS_FAILURE,
    DIAG_OP_DATA_NOT_IN_VUC,
    DIAG_OP_DATA_IN_PLATFORM,
    DIAG_OP_VEHICLE_DEFINATION_FAILURE,
    DIAG_OP_WRONG_INPUT_LENGHT,
    DIAG_OP_WRONG_DATA,
    DIAG_OP_MAX /* Invalid operation status */
} DiagOpStat_t;

typedef struct 
{
    RolloverItem    configItem;
    uint8_t         result;
}ResSetRolloverCfg;

// Responses - VGM

// Responses - IHU

// Responses - Data-storage

// Responses - Data-storage - RES_GETDSPARAMETER_INT -------------------------

typedef struct
{
    char collection[N_STR_MAX];
    char name[N_STR_MAX];
    int value;
} ResGetDSParameterInt;

// Responses - Data-storage - RES_GETDSPARAMETER_STR -------------------------

typedef struct
{
    char collection[N_STR_MAX];
    char name[N_STR_MAX];
    char value[N_STR_MAX];
} ResGetDSParameterStr;

// Responses - Data-storage - RES_GETDSPARAMETER_VECT ------------------------

typedef struct
{
    char collection[N_STR_MAX];
    char name[N_STR_MAX];
    unsigned char value[N_VECT_MAX];
    uint8_t size;
} ResGetDSParameterVect;


// Responses - Data-storage - RES_CARMODE ------------------------------------

// For debug purpose with power box
const std::map<CarModeState,std::string> CarModeStateMap = { {CAR_NORMAL, "CARMODE_NORMAL"},
                                                             {CAR_TRANSPORT, "CARMODE_TRANSPORT"},
                                                             {CAR_CARMODFCY, "CARMODE_CARMODFCY"},
                                                             {CAR_CRASH, "CARMODE_CRASH"},
                                                             {CAR_DYNO, "CARMODE_DYNO"} };
typedef struct
{
    CarModeState carmode;
} ResCarMode;

// Responses - Data-storage - RES_CARUSAGEMODE -------------------------------

// For debug purpose with power box
const std::map<CarUsageModeState,std::string> CarUsageModeStateMap = { {CAR_ABANDONED, "USAGEMODE_ABANDONED"},
                                                                       {CAR_INACTIVE, "USAGEMODE_INACTIVE"},
                                                                       {CAR_USGMODCNVINC, "USAGEMODE_CONVENIENCE"},
                                                                       {CAR_ACTIVE, "USAGEMODE_ACTIVE"},
                                                                       {CAR_DRIVING, "USAGEMODE_DRIVING"} };

typedef struct
{
    CarUsageModeState usagemode;
} ResCarUsageMode;

// Responses - Data-storage - RES_ELECENGLVL ---------------------------------

typedef enum {
    ENERGY_LVL_NORMAL = 0,
    ENERGY_LVL_LOW_ELECTRICAL_ENERGY = 1,
    ENERGY_LVL_EXPENSIVE = 2,
    ENERGY_LVL_CHEAP = 3,
} ElectEngLvl;

typedef struct
{
    ElectEngLvl level;
} ResElectEngLvl;

// Responses - Data-storage - RES_VINNUMBER-----------------------------------

typedef struct
{
    char vin_number[N_STR_MAX];
} ResVINNumber;

// Responses - Data-storage - RES_GETVUCPOWERMODE -------------------------------

typedef enum
{
    VUCPWM_UNDEFINED,
    VUCPWM_DEEPSLEEP,
    VUCPWM_NETOFF,
    VUCPWM_ACTIVE,
    VUCPWM_STANDBY,
    VUCPWM_POLLING,
    VUCPWM_ERA_CALLBACK
} VucPowerMode;

typedef struct
{
    VucPowerMode power_mode;
} ResGetVucPowerMode;

// Responses - Data-storage - RES_GETVUCWAKEUPREASON ----------------------------

typedef enum
{
    VUCWAKEUP_POWER_WAKEUP          = 1,
    VUCWAKEUP_RESET_WAKEUP          = 1 << 1,
    VUCWAKEUP_INTERNAL_RESET_WAKEUP = 1 << 2,
    VUCWAKEUP_INTERNAL_WDG_WAKEUP   = 1 << 3,
    VUCWAKEUP_EXTERNAL_RESET_WAKEUP = 1 << 4,
    VUCWAKEUP_CANRSCAN01_WAKEUP     = 1 << 5,
    VUCWAKEUP_SRXBUS_WAKEUP         = 1 << 6,
    VUCWAKEUP_RTC_WAKEUP            = 1 << 7,
    VUCWAKEUP_RKE_WAKEUP            = 1 << 8,
    VUCWAKEUP_CAN_RXD_WAKEUP        = 1 << 9,
    VUCWAKEUP_BLE_WAKEUP            = 1 << 10,
    VUCWAKEUP_ECALL_WAKEUP          = 1 << 11,
    VUCWAKEUP_MDM_WAKEUP            = 1 << 12
} VucWakeupReason;

typedef struct
{
    VucWakeupReason wakeup_reason;
} ResGetVucWakeupReason;

// Responses - Message-processor

// Responses - Message-processor - RES_GETVUCSWBUILD ----------------------------

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t sw_version;
    uint16_t checksum;
} ResGetVucSwBuild;

// Responses - Message-processor - RES_GETBLESWBUILD ----------------------------

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t sw_version;
    uint16_t checksum;
    uint8_t stack_version[3];
} ResGetBleSwBuild;

// Responses - Message-processor - RES_VUCSWVERSION --------------------------

typedef struct
{
    char vuc_part_number[8];
    char vuc_version[3];
} ResVucSwVersion;

// Responses - Message-processor - RES_BLESWVERSION --------------------------

typedef struct
{
    char ble_part_number[8];
    char ble_version[3];
} ResBleSwVersion;

// Responses - Message-processor - RES_LASTBUBSTATE --------------------------

typedef enum
{
    BATTERY_USED_CAR = 0,
    BATTERY_USED_BACKUP = 1,
} BatteryUsed;

typedef enum
{
    BATTERY_STATE_NOT_PRESENT = 0,
    BATTERY_STATE_PARTIALLY_CHARGED = 1,
    BATTERY_STATE_CHARGING = 2,
    BATTERY_STATE_ERROR_DURING_CHARGING = 0x3f
} BatteryState;

typedef struct
{
    BatteryUsed battery_used;
    BatteryState battery_state;
    uint8_t charge_level; // 0-100 or 0xff for error
    int16_t backup_battery_temp; // 0xff (not available) 0xfe (sensor error)
    int16_t ambient_temp; // 0xff (not available) 0xfe (sensor error)
    float battery_voltage; // 0xffff (not available)
    float kl30_voltage; // 0xffff (not available)
} ResLastBubState;

// Responses - Message-processor - RES_GETDOORSTATE ------------------------------------------
typedef enum
{
    DOORSTATE_UNKNOWN,
    DOORSTATE_OPEN,
    DOORSTATE_CLOSE
} DoorState;

typedef enum
{
    DOORLOCKSTATE_UNKNOWN,
    DOORLOCKSTATE_UNLOCKED,
    DOORLOCKSTATE_LOCKED,
    DOORLOCKSTATE_SAFELOCKED
} DoorLockState;

typedef enum
{
    HOODSTATE_UNKNOWN,
    HOODSTATE_OPEN,
    HOODSTATE_CLOSE
} HoodState;

typedef enum
{
    CENTRALLOCKSTATEFORUSRFEEDBACK_UNKNOWN,
    CENTRALLOCKSTATEFORUSRFEEDBACK_OPEN,
    CENTRALLOCKSTATEFORUSRFEEDBACK_CLOSE,
    CENTRALLOCKSTATEFORUSRFEEDBACK_LOCKED,
    CENTRALLOCKSTATEFORUSRFEEDBACK_SAFELOCKED
} CentralLockingStateForUserFeedback;

typedef enum
{
    CENTRALLOCKSTATE_UNDEFINED,
    CENTRALLOCKSTATE_UNLOCKED,
    CENTRALLOCKSTATE_TRUNLOCKED,
    CENTRALLOCKSTATE_LOCKED
} CentralLockState;

typedef enum
{
    CENTRALLOCKSTATETRIGSOURCE_NOTRIGSRC,
    CENTRALLOCKSTATETRIGSOURCE_KEYREM,
    CENTRALLOCKSTATETRIGSOURCE_KEYLS,
    CENTRALLOCKSTATETRIGSOURCE_INTRSWT,
    CENTRALLOCKSTATETRIGSOURCE_SPDAUT,
    CENTRALLOCKSTATETRIGSOURCE_TMRAUT,
    CENTRALLOCKSTATETRIGSOURCE_SLAM,
    CENTRALLOCKSTATETRIGSOURCE_TELM,
    CENTRALLOCKSTATETRIGSOURCE_CRASH
} CentralLockStateTrigSource;

typedef enum
{
    CENTRALLOCKSTATENEWEVENT_NOEVENT,
    CENTRALLOCKSTATENEWEVENT_EVENT,
} CentralLockStateNewEvent;

typedef struct
{
    uint8_t driverDoorState;                    //0=Unknown 1=Open 2=Close
    uint8_t driverSideRearDoorState;            //0=Unknown 1=Open 2=Close
    uint8_t driverDoorLockState;                //0=Unknown 1=Unlock 2=Lock 3=SafeLocked(Double Locked)
    uint8_t driverSideRearDoorLockState;        //0=Unknown 1=Unlock 2=Lock 3=SafeLocked(Double Locked)

    uint8_t passengerDoorState;                 //0=Unknown 1=Open 2=Close
    uint8_t passengerSideRearDoorState;         //0=Unknown 1=Open 2=Close
    uint8_t passengerDoorLockState;             //0=Unknown 1=Unlock 2=Lock 3=SafeLocked(Double Locked)
    uint8_t passengerSideRearDoorLockState;     //0=Unknown 1=Unlock 2=Lock 3=SafeLocked(Double Locked)

    uint8_t tailgateOrBootlidState;             //0=Unknown 1=Open 2=Close
    uint8_t tailgateOrBootlidLockState;         //0=Unknown 1=Unlock 2=Lock
    uint8_t hoodState;                          //0=Unknown 1=Open 2=Close

    uint8_t centralLockingStateForUserFeedback; //0=Unknown 1=Open 2=Close 3=Locked 4=SafeLocked(Double Locked)
    uint8_t centralLockState;                   //0=Undefined 1=Unlocked 2=TrUnlocked 3=Locked
    uint8_t centralLockStateTrigSource;         //0=NoTrigSrc 1=KeyRem 2=Keyls 3=IntrSwt 4=SpdAut 5=TmrAut 6=Slam 7=Telm 8=Crash
    uint8_t centralLockStateNewEvent;           //0=No event 1=One second since new central locking or unlocking event
} ResGetDoorState;

typedef enum
{
    WINANDROOFANDCURTPOSN_POSNUKWN = 0,
    WINANDROOFANDCURTPOSN_CLSFULL,
    WINANDROOFANDCURTPOSN_PERCOPEN4,
    WINANDROOFANDCURTPOSN_PERCOPEN8,
    WINANDROOFANDCURTPOSN_PERCOPEN12,
    WINANDROOFANDCURTPOSN_PERCOPEN16,
    WINANDROOFANDCURTPOSN_PERCOPEN20,
    WINANDROOFANDCURTPOSN_PERCOPEN24,
    WINANDROOFANDCURTPOSN_PERCOPEN28,
    WINANDROOFANDCURTPOSN_PERCOPEN32,
    WINANDROOFANDCURTPOSN_PERCOPEN36,
    WINANDROOFANDCURTPOSN_PERCOPEN40,
    WINANDROOFANDCURTPOSN_PERCOPEN44,
    WINANDROOFANDCURTPOSN_PERCOPEN48,
    WINANDROOFANDCURTPOSN_PERCOPEN52,
    WINANDROOFANDCURTPOSN_PERCOPEN56,
    WINANDROOFANDCURTPOSN_PERCOPEN60,
    WINANDROOFANDCURTPOSN_PERCOPEN64,
    WINANDROOFANDCURTPOSN_PERCOPEN68,
    WINANDROOFANDCURTPOSN_PERCOPEN72,
    WINANDROOFANDCURTPOSN_PERCOPEN76,
    WINANDROOFANDCURTPOSN_PERCOPEN80,
    WINANDROOFANDCURTPOSN_PERCOPEN84,
    WINANDROOFANDCURTPOSN_PERCOPEN88,
    WINANDROOFANDCURTPOSN_PERCOPEN92,
    WINANDROOFANDCURTPOSN_PERCOPEN96,
    WINANDROOFANDCURTPOSN_OPENFULL,
    WINANDROOFANDCURTPOSN_RESD1,
    WINANDROOFANDCURTPOSN_RESD2,
    WINANDROOFANDCURTPOSN_RESD3,
    WINANDROOFANDCURTPOSN_RESD4,
    WINANDROOFANDCURTPOSN_MOVG
} WinPosnState;

typedef struct
{
    uint8_t sunroofPosnSts;
    uint8_t winPosnStsAtDrv;
    uint8_t winPosnStsAtPass;
    uint8_t winPosnStsReLe;
    uint8_t winPosnStsReRi;
} ResGetWinOpenState;

typedef enum
{
    CMPMTAIRPMLVL_LEVEL1 = 0,
    CMPMTAIRPMLVL_LEVEL2,
    CMPMTAIRPMLVL_LEVEL3,
    CMPMTAIRPMLVL_LEVEL4,
    CMPMTAIRPMLVL_LEVEL5,
    CMPMTAIRPMLVL_LEVEL6,
    RESERVED,
    CMPMTAIRPMLVL_INVALID1
} CmpmtAirPmLvl;
typedef struct
{
	uint8_t intPm25Lvl;
	uint8_t outdPm25Lvl;
	uint16_t intPm25Vlu;
	uint16_t outdPm25Vlu;
} ResGetPMLevelState;
// Responses - Message-processor - RES_GETWIFIDATA ---------------------------

typedef enum {
    CARCONF_NOT_VALID, //Should only be sent on event when something goes wrong
    CARCONF_VALID, // Valid value received from car over CAN
    CARCONF_DEFAULT // Default value set by vuc (if value is requested before vuc received a value from car), use carefully
} CarConfValidity;

typedef enum
{
    WIFI_FUNC_INVALID = 0x00,
    WIFI_FUNC_NO_STA_NO_AP = 0x01,
    WIFI_FUNC_STA_NO_AP = 0x02,
    WIFI_FUNC_NO_STA_AP = 0x03,
    WIFI_FUNC_STA_AP = 0x04
} WiFiFunctionalityControl;

typedef enum
{
    WIFI_FREQ_INVALID,
    WIFI_FREQ_2_4GHZ,
    WIFI_FREQ_5GHZ,
    WIFI_FREQ_2_4GHZ_5GHZ
} WiFiFrequencyBand;

typedef enum
{
    WIFI_WORKSHOP_INVALID = 0x00,
    WIFI_WORKSHOP_DISABLED = 0x01,
    WIFI_WORKSHOP_ENABLED = 0x02
} WiFiWorkshopMode;

typedef enum {
    CONNECTIVITY_INVALID = 0x00,
    NO_CONNECTIVITY = 0x01,
    CONNECTIVITY = 0x02
} Connectivity;

typedef struct
{
    WiFiFunctionalityControl wifi_functionality_control;
    CarConfValidity wifi_functionality_control_validity;
    WiFiFrequencyBand wifi_frequency_band;
    CarConfValidity wifi_frequency_band_validity;
    WiFiWorkshopMode wifi_workshop_mode;
    CarConfValidity wifi_workshop_mode_validity;
    Connectivity connectivity;
    CarConfValidity connectivity_validity;
} ResGetWifiData;

// Responses - Message-processor - RES_GETPSIMAVAILABILITY -------------------

typedef enum
{
    SIM_INVALID = 0x00,
    WITHOUT_SIM = 0x01,
    ONLY_PSIM = 0x02,
    ONLY_TSIM = 0x03,
    TSIM_AND_PSIM = 0x04,
    VOLVO_SIM = 0x05
} PSIMAvailability;

typedef struct
{
    PSIMAvailability psim_availability;
    CarConfValidity psim_availability_validity;
} ResGetPSIMAvailability;


// Responses - Message-processor - RES_GETFUELTYPE ---------------------------
typedef enum
{
    FUEL_TYPE_INVALID= 0x00,
    FUEL_TYPE_PETROL = 0x01,
    FUEL_TYPE_DIESEL= 0x02
} FuelType;

typedef struct
{
    FuelType fuel_type;
    CarConfValidity fuel_type_validity;
} ResGetFuelType;

// Responses - Message-processor - RES_GETPROPULSIONTYPE ---------------------
typedef enum
{
    PROP_TYPE_INVALID = 0x00,
    PROP_TYPE_COMBUSTION = 0x01,
    PROP_TYPE_HYBRID = 0x02,
    PROP_TYPE_PLUGIN_HYBRID = 0x03,
    PROP_TYPE_ELECTRIC = 0x04
} PropulsionType;

typedef struct
{
    PropulsionType propulsion_type;
    CarConfValidity propulsion_type_validity;
} ResGetPropulsionType;


// Responses - Message-processor - RES_GETOHCBTNSTATE ------------------------

typedef enum
{
    OHCBTN_INACTIVE,
    OHCBTN_ACTIVE,
    OHCBTN_ERROR = 0xf,
} OhcBtnState;

typedef struct {
    OhcBtnState ecall_btn;
    OhcBtnState icall_btn;
} ResGetOhcBtnState;

// Requests - Message-processor - RES_GETNUMBEROFDOORS -----------------------

typedef enum
{
    NBR_OF_DOORS_INVALID = 0x00,
    NBR_OF_DOORS_5 = 0x01,
    NBR_OF_DOORS_4 = 0x02,
    NBR_OF_DOORS_2 = 0x03,
    NBR_OF_DOORS_3 = 0x04
} NbrOfDoors;

typedef struct {
    NbrOfDoors nbr_of_doors;
    CarConfValidity nbr_of_doors_validity;
} ResGetNumberOfDoors;

// Requests - Message-processor - RES_GETSTEERINGWHEELPOSITION ---------------

typedef enum
{
    STEERING_WHEEL_POSITION_INVALID = 0x00,
    STEERING_WHEEL_POSITION_LEFT = 0x01,
    STEERING_WHEEL_POSITION_RIGHT = 0x02
} SteeringWheelPosition;

typedef struct {
    SteeringWheelPosition steering_wheel_position;
    CarConfValidity steering_wheel_position_validity;
} ResGetSteeringWheelPosition;

// Requests - Message-processor - RES_GETFUELTANKVOLUME ----------------------
typedef enum
{
    TANK_VOLUME_INVALID = 0x00,
    TANK_VOLUME_71L = 0x01,
    TANK_VOLUME_60L = 0x02,
    TANK_VOLUME_50L = 0x03,
    TANK_VOLUME_55L = 0x04,
    TANK_VOLUME_36L = 0x05,
    TANK_VOLUME_47L = 0x06,
    TANK_VOLUME_54L = 0x07,
    TANK_VOLUME_49L = 0x08,
    TANK_VOLUME_0L =  0x09,
    TANK_VOLUME_70L = 0x0A,
    TANK_VOLUME_40L = 0x80
} FuelTankVolume;


typedef struct {
    FuelTankVolume fuel_tank_volume;
    CarConfValidity fuel_tank_volume_validity;
} ResGetFuelTankVolume;

// Requests - Message-processor - RES_GETASSISTANCESERVICES ------------------

typedef enum {
    ASSISTANCE_SERVICES_INVALID = 0x00,
    WITHOUT_ASSISTANCE_SERVICES = 0x01,
    WITH_ASSISTANCE_SERVICES = 0x02,
    WITH_ASSISTANCE_SERVICES_ECALL_EU = 0x80,
    WITH_ASSISTANCE_SERVICES_ERA_GLONASS =0x81
} AssistanceServices;

typedef struct {
    AssistanceServices assistance_services;
    CarConfValidity assistance_services_validity;
} ResGetAssistanceServices;

// Requests - Message-processor - RES_GETAUTONOMOUSDRIVE ------------------

typedef enum {
    AUTONOMOUS_VEHICLE_INVALID = 0x00,
    NO_AUTONOMOUS_VEHICLE = 0x01,
    AUTONOMOUS_VEHICLE_VOLVO = 0x02,
    AUTONOMOUS_VEHICLE_PREPARATION = 0x03,
    AUTONOMOUS_VEHICLE_GEELY =0x80
} AutonomousDrive;

typedef struct {
    AutonomousDrive autonomous_drive;
    CarConfValidity autonomous_drive_validity;
} ResGetAutonomousDrive;

// Requests - Message-processor - RES_GETCOMBINEDINSTRUMENT ----------------

typedef enum {
    COMBINED_INSTRUMENT_INVALID = 0x00,
    DRIVERS_MODULE_WITH_8_INCH_VOLVO = 0x01,
    DRIVERS_MODULE_WITH_12_POINT_3_INCH_VOLVO = 0x02,
    DRIVERS_MODULE_WITH_4_POINT_2_INCH_VOLVO = 0x03,
    DRIVERS_MODULE_WITH_GEN2_12_POINT_3_INCH_VOLVO =0x04,
    DRIVERS_MODULE_WITH_10_INCH_GEELY = 0x80,
    DRIVERS_MODULE_WITH_GEN2_12_POINT_3_INCH_GEELY = 0x81
} CombinedInstrument;

typedef struct {
    CombinedInstrument combined_instrument;
    CarConfValidity combined_instrument_validity;
} ResGetCombinedInstrument;

// Requests - Message-processor - RES_GETTELEMATICMODULE ---------------------

typedef enum {
    TELEMATIC_MODULE_INVALID = 0x00,
    WITHOUT_TELEMATIC_MODULE = 0x01,
    WITH_TELEMATIC_MODULE = 0x02,
    WITH_TELEMATIC_MODULE_BRAZIL = 0x03,
    WITH_TELEMATIC_MODULE_USA_CDN = 0x04,
    WITH_TELEMATIC_MODULE_CHINA = 0x05,
    WITH_TELEMATIC_MODULE_ROW_LTE = 0x06,
    WITH_TELEMATIC_MODULE_RUSSIA = 0x07,
    WITH_TELEMATIC_MODULE_ONLY_ECALL = 0x08
} TelematicModule;

typedef struct {
    TelematicModule telematic_module;
    CarConfValidity telematic_module_validity;
} ResGetTelematicModule;

// Requests - Message-processor - RES_GETCONNECTIVITY ------------------------

typedef struct {
    Connectivity connectivity;
    CarConfValidity connectivity_validity;
} ResGetConnectivity;

// Requests - Message-processor - RES_GETCONNECTEDSERVICEBOOKING -------------

typedef enum {
    CONNECTED_SERVICE_BOOKING_INVALID = 0x00,
    WITHOUT_CONNECTED_SERVICE_BOOKING = 0x01,
    WITH_CONNECTED_SERVICE_BOOKING = 0x02
} ConnectedServiceBooking;

typedef struct {
    ConnectedServiceBooking connected_service_booking;
    CarConfValidity connected_service_booking_validity;
} ResGetConnectedServiceBooking;

// Requests - Message-processor - RES_GETCARCFGTHEFTNOTIFICATION ---------------

typedef enum
{
    HEFT_NOTIFICATION_INVALID = 0x00,
    WITHOUT_THEFT_NOTIFICATION = 0x01,
    WITH_THEFT_NOTIFCATION = 0x02
} TheftNotifcation;

typedef struct {
    TheftNotifcation theft_notification;
    CarConfValidity theft_notification_validity;
} ResGetTheftNotification;

// Requests - Message-processor - RES_GETCARCFGSTOLENVEHICLETRACKING ---------------

typedef enum
{
    STOLEN_VEHICLE_TRACKING_INVALID = 0x00,
    WITHOUT_STOLEN_VEHICLE_TRACKING = 0x01,
    WITH_STOLEN_VEHICLE_TRACKING = 0x02
} StolenVehicleTracking;

typedef struct {
    StolenVehicleTracking stolen_vehicle_tracking;
    CarConfValidity stolen_vehicle_tracking_validity;
} ResGetStolenVehicleTracking;

// Requests - Message-processor - RES_GETGNSSRECEIVER ---------------

typedef enum
{
    GNSS_RECEIVER_INVALID = 0x00,
    WITHOUT_GNSS_RECEIVER = 0x01,
    GNSS_RECEIVER_IHU = 0x02,
    GNSS_RECEIVER_TEM2 = 0x03,
    GNSS_RECEIVER_IHU_AND_TEM2 = 0x04
} GnssReceiver;

typedef struct {
    GnssReceiver gnss_receiver;
    CarConfValidity gnss_receiver_validity;
} ResGetGnss;

// Requests - Message-processor - RES_GETAPPROVEDCELLULARSTANDARDS ---------------

typedef enum
{
    APPROVEDCELLULARSTANDARDS_INVALID = 0x00,
    APPROVEDCELLULARSTANDARDS_NORESTRICTIONS = 0x01,
    APPROVEDCELLULARSTANDARDS_4GDISABLE = 0x02,
} ApprovedCellularStandards;

typedef struct {
    ApprovedCellularStandards approvedcellularstandards;
    CarConfValidity approvedcellularstandards_validity;
} ResGetApprovedCellularStandards;

// Requests - Message-processor - RES_GETDIGITALVIDEORECORDER ------------------

typedef enum {
    DIGITAL_VIDEO_RECORDER_INVALID = 0x00,
    WITHOUT_DIGITAL_VIDEO_RECORDER = 0x01,
    WITH_DIGITAL_VIDEO_RECORDER = 0x02,
} DigitalVideoRecorder;

typedef struct {
    DigitalVideoRecorder digital_video_recorder;
    CarConfValidity digital_video_recorder_validity;
} ResGetDigitalVideoRecorder;

// Requests - Message-processor - RES_GETDRIVERALERTCONTROL ------------------

typedef enum {
    DRIVER_ALERT_CONTROL_INVALID = 0x00,
    WITHOUT_DRIVER_ALERT_CONTROL = 0x01,
    WITH_DRIVER_ALERT_CONTROL = 0x02,
} DriverAlertControl;

typedef struct {
    DriverAlertControl driver_alert_control;
    CarConfValidity driver_alert_control_validity;
} ResGetDriverAlertControl;

// Responses - Message-processor - RES_GETFBLSWBUILD ----------------------------

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t sw_version;
    uint16_t checksum;
} ResGetFblSwBuild;


// Responses - Test

// Responses - Test - RES_VGMTESTMSG -----------------------------------------

typedef struct
{
    uint8_t id;
    char company[N_STR_MAX];
} ResVGMTestMsg;

// Responses - Test - RES_IHUTESTMSG -----------------------------------------

typedef struct
{
    uint8_t id;
    char city[N_STR_MAX];
} ResIHUTestMsg;



// VehicleComm - Events

/**
    @brief A list of all supported event messages.

    A EventID can be one of the events listed in this enum.
*/
typedef enum {
    EVENT_UNKNOWN,
    // VuC
    EVENT_HORNNLIGHTSTATE,
    EVENT_DIAGERRREPORT,
    EVENT_CRASHSTATE,
    EVENT_BLE_CONNECTIONSTATUS,
    EVENT_BLE_RESET_NOTIFICATION,
    EVENT_BLE_MTUSIZE,
    EVENT_DTCFULLREPORT,
    EVENT_DTCSTATUSREPORT,
    EVENT_AMPLIFIERSTATE,
    EVENT_RFKLINEERRREPORT,
    EVENT_LASTBUBSTATE,
    EVENT_OHCBTNSTATE,
    EVENT_VEHICLESPEEDSTATE,
    EVENT_CANVOLTAGEERRREPORT,
    EVENT_DIAGCOUPLEREPORT,
    EVENT_NUMBEROFDOORS,
    EVENT_FUELTYPE,
    EVENT_STEERINGWHEELPOSITION,
    EVENT_FUELTANKVOLUME,
    EVENT_PROPULSIONTYPE,
    EVENT_ASSISTANCESERVICES,
    EVENT_AUTONOMOUSDRIVE,
    EVENT_COMBINEDINSTRUMENT,
    EVENT_TELEMATICMODULE,
    EVENT_CONNECTIVITY,
    EVENT_CONNECTEDSERVICEBOOKING,
    EVENT_PSIMAVAILABILITY,
    EVENT_WIFIFUNCTIONALITYCONTROL,
    EVENT_WIFIFREQUENCYBAND,
    EVENT_WIFIWORKSHOPMODE,
    EVENT_CARCFG_THEFTNOTIFICATION,
    EVENT_CARCFG_STOLENVEHICLETRACKING,
    EVENT_GNSSRECEIVER,
    EVENT_APPROVEDCELLULARSTANDARDS,
    EVENT_DIGITALVIDEORECORDER,
    EVENT_DRIVERALERTCONTROL,
    // TO VGM
    EVENT_POSNFROMSATLTCON,
    // Data-storage
    EVENT_DSPARAMETER_INT,
    EVENT_DSPARAMETER_STR,
    EVENT_DSPARAMETER_VECT,
    EVENT_CARMODE,
    EVENT_CARUSAGEMODE,
    EVENT_DOORLOCKSTATE,
#if 1 // nieyj
    EVENT_WINOPENSTATE,
    EVENT_PMLVL,
#endif
    // uia93888 remote start engine
    EVENT_RMTENGINESECURITYRANDOM,
    EVENT_RMTENGINESECURITYRESULT,
    EVENT_RMTENGINE,
    EVENT_RMTSTATUS,
    EVENT_RMTENGINESTATES,
    EVENT_DELAYENGINERUNNINGTIME,
    EVENT_OPERATEREMOTECLIMATE,
    EVENT_OPERATERMTSEATHEAT,
    EVENT_OPERATERMTSEATVENTILATION,
    EVENT_OPERATERMTSTEERWHLHEAT,
    EVENT_PARKINGCLIMATEOPER,
    // uia93888 end
    EVENT_ELECTENGLVL,
    EVENT_VUCPOWERMODE,
    EVENT_VUCWAKEUPREASON,
    EVENT_VINNUMBER,
    // Message-processor
    EVENT_ENDOFTRIP,
    // Test
    EVENT_TESTFROMVGM,
    EVENT_TESTFROMIHU,
    EVENT_TESTFROMVC,
} EventID;

const char *const EventStr[] = {
    "EVENT_UNKNOWN",
   // VuC
    "EVENT_HORNNLIGHTSTATE",
    "EVENT_DIAGERRREPORT",
    "EVENT_CRASHSTATE",
    "EVENT_BLE_CONNECTIONSTATUS",
    "EVENT_BLE_RESET_NOTIFICATION",
    "EVENT_BLE_MTUSIZE",
    "EVENT_DTCFULLREPORT",
    "EVENT_DTCSTATUSREPORT",
    "EVENT_AMPLIFIERSTATE",
    "EVENT_RFKLINEERRREPORT",
    "EVENT_LASTBUBSTATE",
    "EVENT_OHCBTNSTATE",
    "EVENT_VEHICLESPEEDSTATE",
    "EVENT_CANVOLTAGEERRREPORT",
    "EVENT_DIAGCOUPLEREPORT",
    "EVENT_NUMBEROFDOORS",
    "EVENT_FUELTYPE",
    "EVENT_STEERINGWHEELPOSITION",
    "EVENT_FUELTANKVOLUME",
    "EVENT_PROPULSIONTYPE",
    "EVENT_ASSISTANCESERVICES",
    "EVENT_AUTONOMOUSDRIVE",
    "EVENT_COMBINEDINSTRUMENT",
    "EVENT_TELEMATICMODULE",
    "EVENT_CONNECTIVITY",
    "EVENT_CONNECTEDSERVICEBOOKING",
    "EVENT_PSIMAVAILABILITY",
    "EVENT_WIFIFUNCTIONALITYCONTROL",
    "EVENT_WIFIFREQUENCYBAND",
    "EVENT_WIFIWORKSHOPMODE",
    "EVENT_CARCFG_THEFTNOTIFICATION",
    "EVENT_CARCFG_STOLENVEHICLETRACKING",
    "EVENT_GNSSRECEIVER",
    "EVENT_DIGITALVIDEORECORDER",
    "EVENT_DRIVERALERTCONTROL",
    // To VGM
    "EVENT_POSNFROMSATLTCON",
    //Data-storage
    "EVENT_DSPARAMETER_INT",
    "EVENT_DSPARAMETER_STR",
    "EVENT_DSPARAMETER_VECT",
    "EVENT_CARMODE",
    "EVENT_CARUSAGEMODE",
    "EVENT_DOORLOCKSTATE",
#if 1   // nieyj
    "EVENT_WINOPENSTATE",
    "EVENT_PMLVL",
#endif
    //uia93888 res
    "EVENT_RMTENGINESECURITYRANDOM",
    "EVENT_RMTENGINESECURITYRESULT",
    "EVENT_RMTENGINE",
    "EVENT_RMTSTATUS",
    "EVENT_ENGINESTATES",
    "EVENT_DELAYENGINERUNNINGTIME",
    "EVENT_OPERATEREMOTECLIMATE",
    "EVENT_OPERATERMTSEATHEAT",
    "EVENT_OPERATERMTSEATVENTILATION",
    "EVENT_OPERATERMTSTEERWHLHEAT",
    "EVENT_PARKINGCLIMATEOPER",
    //uia93888 end
    "EVENT_ELECTENGLVL",
    "EVENT_VUCPOWERMODE",
    "EVENT_VUCWAKEUPREASON",
    "EVENT_VINNUMBER",
    // Message-processor
    "EVENT_ENDOFTRIP",
    // Test
    "EVENT_TESTFROMVGM",
    "EVENT_TESTFROMIHU",
    "EVENT_TESTFROMVC",
};

// Events - VuC

// Events - VuC - EVENT_HORNNLIGHTSTATE --------------------------------------

typedef enum {
    HL_STARTED,
    HL_STOPPED,
    HL_ABORTED,
} HNLState;

typedef struct
{
    HNLState state;
    HNLMode mode;
} EventHornNLightState;

// Events - VuC - EVENT_DIAGERRREPORT ----------------------------------------

typedef struct
{
    DiagErrStatus phone_ant_open_load_test_status;
    DiagErrState phone_ant_open_load_test_state;
    DiagErrStatus ecall_ant_open_load_test_status;
    DiagErrState ecall_ant_open_load_test_state;
} EventDiagErrReport;

// Events - VuC - EVENT_CRASHSTATE -------------------------------------------

typedef enum
{
    CRASH_STATE_LEVEL_A,
    CRASH_STATE_LEVEL_B,
    CRASH_STATE_LEVEL_C,
    CRASH_STATE_LEVEL_D
} CrashState;

typedef struct
{
    bool  resume_from_power_loss;
    CrashState state;
} EventCrashState;

// Events - VuC - EVENT_BLE_CONNECTIONSTATUS ---------------------------------

typedef enum {
    BLE_CONNECTION_INVALID,
    BLE_CONNECTION_DISCONNECTED,
    BLE_CONNECTION_CONNECTED_NOT_AUTH,
    BLE_CONNECTION_CONNECTED_AUTH,
}BleConnState;

typedef struct
{
    uint16_t connection_id;
    BleConnState state;
} EventBleConnStatus;

// Events - VuC - EVENT_BLE_RESET_NOTIFICATION -------------------------------

typedef struct {} EventBleResetNotification;

// Events - VuC - EVENT_BLE_MTUSIZE ------------------------------------------

typedef struct {
    uint16_t connection_id;
    uint8_t mtu_size;
} EventBleMtuSize;

// Events - VuC - EVENT_DTCFULLREPORT ----------------------------------------
typedef struct
{
    uint8_t event_id;
    uint8_t no_of_events;
    uint8_t hexcode[3];
    uint8_t status;
    uint8_t ext_data10;
    uint8_t ext_data01;
    uint8_t ext_data02;
    uint8_t ext_data03;
    uint8_t ext_data04;
    uint8_t ext_data06;
    uint8_t ext_data07;
    uint8_t ext_data11;
    uint8_t ext_data12;
    uint8_t ext_data20[4];
    uint8_t ext_data21[4];
    uint8_t ext_data30;
    uint8_t snapshot_data20[10];
} EventDtcFullReport;

// Events - VuC - EVENT_DTCSTATUSREPORT --------------------------------------

typedef struct
{
    uint8_t event_id;
    uint8_t status;
    uint8_t ext_data10;
    uint8_t ext_data11;
    uint8_t ext_data12;
    uint8_t ext_data30;
} EventDtcStatusReport;

// Events - VuC - EVENT_AMPLIFIERSTATE ---------------------------------------

typedef enum
{
    AMP_MUTED,
    AMP_UNMUTED
} AmplifierState;

typedef enum
{
    AMP_STATUS_NO_ERROR,
    AMP_STATUS_ERROR
} AmplifierStatus;

typedef struct
{
    AmplifierState state;
    AmplifierStatus status;
} EventAmplifierState;

// Events - VuC - EVENT_RFKLINEERRREPORT -------------------------------------

typedef struct
{
    uint8_t rf_error_code;
    uint8_t kline_error_code;
} EventRfKlineErrReport;

// Events - VuC - EVENT_LASTBUBSTATE -----------------------------------------
typedef ResLastBubState EventLastBubState;

// Events - VuC - EVENT_OHCBTNSTATE ------------------------------------------
typedef ResGetOhcBtnState EventOhcBtnState;

// Events - VuC - EVENT_VEHICLESPEEDSTATE ------------------------------------

typedef struct
{
    bool vehicle_speed_below_v_scan_full;
    bool vehicle_speed_below_v_scan_limited;
} EventVehicleSpeedState;

// Events - VuC - EVENT_CANVOLTAGEERRREPORT ----------------------------------

typedef enum {
    CAN_VOLTAGE_OK,
    CAN_VOLTAGE_NOK
} CanVoltageStatus;

typedef struct
{
    CanVoltageStatus can_under_voltage;
    CanVoltageStatus kl30_under_voltage;
    CanVoltageStatus kl30_over_voltage;
} EventCanVoltageErrReport;

// Events - VuC - EVENT_DIAGCOUPLEREPORT ---------------------------------------

typedef enum {
    DO_NOT_CONNECT_TO_WORKSHOP,
    CONNECT_TO_WORKSHOP
} WifiWorkshopConnect;

typedef struct
{
    WifiWorkshopConnect req;
} EventDiagCoupleReport;

// Events - VuC - EVENT_NUMBEROFDOORS ------------------------------------------

typedef ResGetNumberOfDoors EventNumberOfDoors;

// Events - VuC - EVENT_FUELTYPE -----------------------------------------------

typedef ResGetFuelType EventFuelType;

// Events - VuC - EVENT_STEERINGWHEELPOSITION ----------------------------------

typedef ResGetSteeringWheelPosition EventSteeringWheelPosition;

// Events - VuC - EVENT_FUELTANKVOLUME -----------------------------------------

typedef ResGetFuelTankVolume EventFuelTankVolume;

// Events - VuC - EVENT_PROPULSIONTYPE -----------------------------------------

typedef ResGetPropulsionType EventPropulsionType;

// Events - VuC - EVENT_ASSISTANCESERVICES -------------------------------------

typedef ResGetAssistanceServices EventAssistanceServices;

// Events - VuC - EVENT_AUTONOMOUSDRIVE -------------------------------------

typedef ResGetAutonomousDrive EventAutonomousDrive;

// Events - VuC - EVENT_COMBINEDINSTRUMENT -------------------------------------

typedef ResGetCombinedInstrument EventCombinedInstrument;

// Events - VuC - EVENT_TELEMATICMODULE ----------------------------------------

typedef ResGetTelematicModule EventTelematicModule;

// Events - VuC - EVENT_CONNECTIVITY -------------------------------------------

typedef ResGetConnectivity EventConnectivity;

// Events - VuC - EVENT_CONNECTEDSERVICEBOOKING --------------------------------

typedef ResGetConnectedServiceBooking EventConnectedServiceBooking;

// Events - VuC - EVENT_PSIMAVAILABILITY ---------------------------------------

typedef ResGetPSIMAvailability EventPSIMAvailability;

// Events - VuC - EVENT_WIFIFUNCTIONALITYCONTROL -------------------------------

typedef struct {
    WiFiFunctionalityControl wifi_functionality_control;
    CarConfValidity wifi_functionality_control_validity;
} EventWifiFunctionalityControl;

// Events - VuC - EVENT_WIFIFREQUENCYBAND --------------------------------------

typedef struct {
    WiFiFrequencyBand wifi_frequency_band;
    CarConfValidity wifi_frequency_band_validity;
} EventWifiFrequencyBand;

// Events - VuC - EVENT_WIFIWORKSHOPMODE ---------------------------------------

typedef struct {
    WiFiWorkshopMode wifi_workshop_mode;
    CarConfValidity wifi_workshop_mode_validity;
} EventWifiWorkshopMode;

// Events - VuC - EVENT_CARCFG_THEFTNOTIFICATION -----------------------------------------------

typedef ResGetTheftNotification EventTheftNotification;

// Events - VuC - EVENT_CARCFG_STOLENVEHICLETRACKING -----------------------------------------------

typedef ResGetStolenVehicleTracking EventStolenVehicleTracking;

// Events - VuC - EVENT_GNSSRECEIVER -----------------------------------------------

typedef ResGetGnss EventGnss;

// Events - VuC - EVENT_APPROVEDCELLULARSTANDARDS -------------------------------------

typedef ResGetApprovedCellularStandards EventApprovedCellularStandards;

// Events - VuC - EVENT_DIGITALVIDEORECORDER -------------------------------------

typedef ResGetDigitalVideoRecorder EventDigitalVideoRecorder;

// Events - VuC - EVENT_DRIVERALERTCONTROL -------------------------------------

typedef ResGetDriverAlertControl EventDriverAlertControl;

// Events - VGM
typedef ReqSendPosnFromSatltCon EventPosnFromSatltCon;

// Events - Data-storage

// Events - Data-storage - EVENT_DSPARAMETER_INT -----------------------------

typedef ResGetDSParameterInt EventDSParameterInt;

// Events - Data-storage - EVENT_DSPARAMETER_STR -----------------------------

typedef ResGetDSParameterStr EventDSParameterStr;

// Events - Data-storage - EVENT_DSPARAMETER_VECT ----------------------------

typedef ResGetDSParameterVect EventDSParameterVect;

// Events - Data-storage - EVENT_CARMODE -------------------------------------

typedef ResCarMode EventCarMode;

// Events - Data-storage - EVENT_CARUSAGEMODE --------------------------------

typedef ResCarUsageMode EventCarUsageMode;

// Events - Data-storage - EVENT_DOORLOCKSTATE -------------------------------
typedef ResGetDoorState EventDoorLockState;

#if 1 // nieyj
// Events - Data-storage - EVENT_WINOPENSTATE -------------------------------
typedef ResGetWinOpenState EventWinOpenState;

// Events - Data-storage - EVENT_PMLVL -------------------------------
typedef ResGetPMLevelState EventPMLevelState;

#endif

//uia93888 res
//Event - res - EVENT_RMTENGINESECURITYRANDOM------------------------------
typedef Res_RMTEngineSecurityRandom Event_RMTEngineSecurityRandom;

//Event - res - EVENT_RMTENGINESECURITYRESULT------------------------------
typedef Res_RMTEngineSecurityResult Event_RMTEngineSecurityResult;

//Events - res - EVENT_RMTENGINE -------------------------------------------
typedef Res_RMTEngine Event_RMTEngine;

//Events - rce - EVENT_DELAYENGINERUNNINGTIME-------------------------------
typedef Res_DelayEngRunngTime Event_DelayEngRunngTime;

//Events - rce - EVENT_OPERATEREMOTECLIMATE---------------------------------
typedef Res_OperateRMTClimate Event_OperateRMTClimate;

//Events - rec - EVENT_OPERATERMTSEATHEAT-----------------------------------
typedef Res_OperateRMTSeatHeat_S Event_OperateRMTSeatHeat_S;

//Events - rce - EVENT_OPERATERMTSEATVENTILATION----------------------------
typedef Res_OperateRMTSeatVenti_S Event_OperateRMTSeatVenti_S;

//Events - rce - EVENT_OPERATERMTSTEERWHLHEAT-------------------------------
typedef Res_OperateRMTSteerWhlHeat_S Event_OperateRMTSteerWhlHeat_S;

//Events - rcc - EVENT_PARKINGCLIMATEOPER-----------------------------------
typedef Res_ParkingClimateOper_S Event_ParkingClimateOper_S;
//end uis93888

//Events - Data-storage - EVENT_RMTSTATUS ---------------------------------
typedef ResRMTStatus EventRMTStatus;


// Events - Data-storage - EVENT_ELECTENGLVL ---------------------------------

typedef ResElectEngLvl EventElectEngLvl;

// Events - Data-storage - EVENT_VUCPOWERMODE --------------------------------

typedef ResGetVucPowerMode EventVucPowerMode;

// Events - Data-storage - EVENT_WAKEUPREASON --------------------------------

typedef ResGetVucWakeupReason EventVucWakeupReason;

// Events - Data-storage - EVENT_VINNUMBER -----------------------------------

typedef ResVINNumber EventVINNumber;

// Events - Message-processor

// Events - Message-processor - EVENT_ENDOFTRIP ------------------------------

typedef struct {} EventEndOfTrip;

// Events - Test

// Events - Test - EVENT_TESTFROMVGM -----------------------------------------

typedef struct
{
    uint8_t vgm;
} EventTestFromVGM;

// Events - Test - EVENT_TESTFROMIHU -----------------------------------------

typedef struct
{
    uint8_t ihu;
} EventTestFromIHU;

// Events - Test - EVENT_TESTFROMVC ------------------------------------------

typedef struct
{
    uint8_t vc;
} EventTestFromVC;


// VehicleComm - Common

typedef struct {} Empty;



// Request<->Response mapping

/**
    @brief A mapping between a VCRequest and a VCResponse.

    This struct represents a mapping of a VCRequest to a VCResponse.
*/
struct MessagePair {
    RequestID request_id;
    ResponseID response_id;
};

/**
    @brief Lookup-table for VCRequests<->VCResponses.

    A list of all known mappings between VCRequests and VCResponses.
*/

const MessagePair message_pair_lookup[] = {
        {
            REQ_DOORLOCKUNLOCK,
            RES_REQUESTSENT
        },
    // VuC
    // {
        // .request_id = REQ_DOORLOCKUNLOCK,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_HORNNLIGHT,
        // .response_id = RES_HORNNLIGHT
    // },
    // {
        // .request_id = REQ_HORNNLIGHT,
        // .response_id = RES_HORNNLIGHT
    // },
    // {
        // .request_id = REQ_SWITCHTOBOOT,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_ANTENNASWITCH,
        // .response_id = RES_ANTENNASWITCH,
    // },
    // {
        // .request_id = REQ_LANLINKTRIGGER,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_GETCARCONFIG,
        // .response_id = RES_GETCARCONFIG
    // },
    // {
        // .request_id = REQ_CARCONFIGFACTRESTORE,
        // .response_id = RES_CARCONFIGFACTRESTORE,
    // },
    // {
        // .request_id = REQ_ADCVALUE,
        // .response_id = RES_ADCVALUE
    // },
    // {
        // .request_id = REQ_VUCTRACE_CONFIG,
        // .response_id = RES_VUCTRACE_CONFIG
    // },
    // {
        // .request_id = REQ_VUCTRACE_CONTROL,
        // .response_id = RES_VUCTRACE_CONTROL
    // },
    // {
        // .request_id = REQ_BLE_DISCONNECT,
        // .response_id = RES_BLE_DISCONNECT
    // },
    // {
        // .request_id = REQ_BLE_DISCONNECTALL,
        // .response_id = RES_BLE_DISCONNECTALL
    // },
    // {
        // .request_id = REQ_BLE_GENERATEBDAK,
        // .response_id = RES_BLE_GENERATEBDAK
    // },
    // {
        // .request_id = REQ_BLE_GETBDAK,
        // .response_id = RES_BLE_GETBDAK
    // },
    // {
        // .request_id = REQ_BLE_CONFIG_DATACOMSERVICE,
        // .response_id = RES_BLE_CONFIG_DATACOMSERVICE
    // },
    // {
        // .request_id = REQ_BLE_CONFIG_IBEACON,
        // .response_id = RES_BLE_CONFIG_IBEACON
    // },
    // {
        // .request_id = REQ_BLE_CONFIG_GENERICACCESS,
        // .response_id = RES_BLE_CONFIG_GENERICACCESS
    // },
    // {
        // .request_id = REQ_BLE_STARTPARINGSERVICE,
        // .response_id = RES_BLE_STARTPARINGSERVICE
    // },
    // {
        // .request_id = REQ_BLE_STOPPARINGSERVICE,
        // .response_id = RES_BLE_STOPPARINGSERVICE
    // },
    // {
        // .request_id = REQ_BLE_WAKEUP_RESPONSE,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_BLE_DATAACCESSAUTHCONFIRMED,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_BLE_STARTADVERTISING,
        // .response_id = RES_BLE_STARTADVERTISING
    // },
    // {
        // .request_id = REQ_BLE_STOPADVERTISING,
        // .response_id = RES_BLE_STOPADVERTISING
    // },
    // {
        // .request_id = REQ_DTCENTRIES,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_DTCCTLSETTINGS,
        // .response_id = RES_DTCCTLSETTINGS
    // },
    // {
        // .request_id = REQ_IPWAKEUP,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_SETOHCLED,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_VINNUMBER,
        // .response_id = RES_VINNUMBER
    // },
    // {
        // .request_id = REQ_GETVUCBLESWPARTNUMBER,
        // .response_id = RES_GETVUCBLESWPARTNUMBER
    // },
    // {
        // .request_id = REQ_GETBELTINFORMATION,
        // .response_id = RES_GETBELTINFORMATION
    // },
    // {
        // .request_id = REQ_GETDIAGERRREPORT,
        // .response_id = RES_GETDIAGERRREPORT
    // },
    // {
        // .request_id = REQ_GETVEHICLESTATE,
        // .response_id = RES_GETVEHICLESTATE
    // },
    // {
        // .request_id = REQ_GETDIDGLOBALSNAPSHOTDATA,
        // .response_id = RES_GETDIDGLOBALSNAPSHOTDATA
    // },
    // {
        // .request_id = REQ_DTCSDETECTED,
        // .response_id = RES_DTCSDETECTED
    // },
    // {
        // .request_id = REQ_PSIMDATA,
        // .response_id = RES_PSIMDATA
    // },
    // {
        // .request_id = REQ_THERMALMITIGATION,
        // .response_id = RES_THERMALMITIGATION
    // },
    // {
        // .request_id = REQ_CARCFGPARAMETERFAULT,
        // .response_id = RES_CARCFGPARAMETERFAULT
    // },
    // {
        // .request_id = REQ_NETWORK_MANAGEMENT_HISTORY,
        // .response_id = RES_NETWORK_MANAGEMENT_HISTORY
    // },
    // {
        // .request_id = REQ_PROGRAM_PRECOND_CHK,
        // .response_id = RES_PROGRAM_PRECOND_CHK
    // },
// #if 1 // nieyj
    // {
        // .request_id = REQ_VFCACTIVATE,
        // .response_id = RES_VFCACTIVATE
    // },
// #endif
    // //uia93888 remote start engine
    // /*{
        // .request_id = REQ_RMTENGINE,
        // .response_id = RES_RMTENGINE
    // },*/
    // {
        // .request_id = REQ_GETRMTSTATUS,
        // .response_id = RES_RMTSTATUS
    // },
    // {
        // .request_id = REQ_GETENGINESTATES,
        // .response_id = RES_RMTENGINE
    // },
    // {
        // .request_id = REQ_ROLLOVERVALUE,
        // .response_id = RES_ROLLOVERVALUE
    // },
    // {
        // .request_id = REQ_SETROLLOVERCFG,
        // .response_id = RES_SETROLLOVERCFG
    // },
    // // VGM
    // {
        // .request_id = REQ_SENDPOSNFROMSATLTCON,
        // .response_id = RES_REQUESTSENT
    // },
    // // IHU
    // // Data-storage
    // {
        // .request_id = REQ_GETDSPARAMETER,
        // .response_id = RES_GETDSPARAMETER_INT
    // },
    // {
        // .request_id = REQ_GETDSPARAMETER,
        // .response_id = RES_GETDSPARAMETER_STR
    // },
    // // Message-processor
    // {
        // .request_id = REQ_LASTBUBSTATE,
        // .response_id = RES_LASTBUBSTATE
    // },
    // {
        // .request_id = REQ_CARUSAGEMODE,
        // .response_id = RES_CARUSAGEMODE
    // },
    // {
        // .request_id = REQ_CARMODE,
        // .response_id = RES_CARMODE
    // },
    // {
        // .request_id = REQ_ELECTENGLVL,
        // .response_id = RES_ELECTENGLVL
    // },
    // {
        // .request_id = REQ_GETVUCPOWERMODE,
        // .response_id = RES_GETVUCPOWERMODE
    // },
    // {
        // .request_id = REQ_GETVUCWAKEUPREASON,
        // .response_id = RES_GETVUCWAKEUPREASON
    // },
    // {
        // .request_id = REQ_VUCSWVERSION,
        // .response_id = RES_VUCSWVERSION
    // },
    // {
        // .request_id = REQ_BLESWVERSION,
        // .response_id = RES_BLESWVERSION
    // },
    // {
        // .request_id = REQ_GETWIFIDATA,
        // .response_id = RES_GETWIFIDATA
    // },
    // {
        // .request_id = REQ_GETPSIMAVAILABILITY,
        // .response_id = RES_GETPSIMAVAILABILITY
    // },
    // {
        // .request_id = REQ_GETFUELTYPE,
        // .response_id = RES_GETFUELTYPE
    // },
    // {
        // .request_id = REQ_GETPROPULSIONTYPE,
        // .response_id = RES_GETPROPULSIONTYPE
    // },
    // {
        // .request_id = REQ_GETOHCBTNSTATE,
        // .response_id = RES_GETOHCBTNSTATE
    // },
    // {
        // .request_id = REQ_GETNUMBEROFDOORS,
        // .response_id = RES_GETNUMBEROFDOORS
    // },
    // {
        // .request_id = REQ_GETSTEERINGWHEELPOSITION,
        // .response_id = RES_GETSTEERINGWHEELPOSITION
    // },
    // {
        // .request_id = REQ_GETFUELTANKVOLUME,
        // .response_id = RES_GETFUELTANKVOLUME
    // },
    // {
        // .request_id = REQ_GETASSISTANCESERVICES,
        // .response_id = RES_GETASSISTANCESERVICES
    // },
    // {
        // .request_id = REQ_GETAUTONOMOUSDRIVE,
        // .response_id = RES_GETAUTONOMOUSDRIVE
    // },
    // {
        // .request_id = REQ_GETCOMBINEDINSTRUMENT,
        // .response_id = RES_GETCOMBINEDINSTRUMENT
    // },
    // {
        // .request_id = REQ_GETTELEMATICMODULE,
        // .response_id = RES_GETTELEMATICMODULE
    // },
    // {
        // .request_id = REQ_GETCONNECTIVITY,
        // .response_id = RES_GETCONNECTIVITY
    // },
    // {
        // .request_id = REQ_GETCONNECTEDSERVICEBOOKING,
        // .response_id = RES_GETCONNECTEDSERVICEBOOKING
    // },
    // {
        // .request_id = REQ_GETCARCFGTHEFTNOTIFICATION,
        // .response_id = RES_GETCARCFGTHEFTNOTIFICATION,
    // },
    // {
        // .request_id = REQ_GETCARCFGSTOLENVEHICLETRACKING,
        // .response_id = RES_GETCARCFGSTOLENVEHICLETRACKING,
    // },
    // {
        // .request_id = REQ_GETGNSSRECEIVER,
        // .response_id = RES_GETGNSSRECEIVER,
    // },
    // {
        // .request_id = REQ_GETAPPROVEDCELLULARSTANDARDS,
        // .response_id = RES_GETAPPROVEDCELLULARSTANDARDS,
    // },
    // {
        // .request_id = REQ_GETDIGITALVIDEORECORDER,
        // .response_id = RES_GETDIGITALVIDEORECORDER
    // },
    // {
        // .request_id = REQ_GETDRIVERALERTCONTROL,
        // .response_id = RES_GETDRIVERALERTCONTROL
    // },
    // {
        // .request_id = REQ_GETDOORSTATE,
        // .response_id = RES_GETDOORSTATE,
    // },
    // // Multi
    // {
        // .request_id = REQ_GETVUCSWBUILD,
        // .response_id = RES_GETVUCSWBUILD
    // },
    // {
        // .request_id = REQ_GETBLESWBUILD,
        // .response_id = RES_GETBLESWBUILD
    // },
    // {
        // .request_id = REQ_GETMSGCATVERSION,
        // .response_id = RES_GETMSGCATVERSION
    // },
    // {
        // .request_id = REQ_GETTCAMHWVERSION,
        // .response_id = RES_GETTCAMHWVERSION
    // },
    // {
        // .request_id = REQ_GETFBLSWBUILD,
        // .response_id = RES_GETFBLSWBUILD
    // },
    // {
        // .request_id = REQ_GETRFVERSION,
        // .response_id = RES_GETRFVERSION
    // },
    // // Test
    // {
        // .request_id = REQ_VGMTESTMSG,
        // .response_id = RES_VGMTESTMSG
    // },
    // {
        // .request_id = REQ_IHUTESTMSG,
        // .response_id = RES_IHUTESTMSG
    // },
    // {
        // .request_id = REQ_TEST_GETCARCONFIG,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_TOGGLEUSAGEMODE,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_TOGGLECARMODE,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_CRASHSTATE,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_OHCSTATE,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_SETFAKEVALUESDEFAULT,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_SETFAKEVALUEINT,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_SETFAKEVALUESTR,
        // .response_id = RES_REQUESTSENT
    // },
    // {
        // .request_id = REQ_TEST_SETLOCALCONFIG,
        // .response_id = RES_REQUESTSENT
    // },
};

// Interface<->Event mapping

/**
    @brief List of VehicleComm-interfaces.
*/
typedef enum {
    INTERFACE_GENERAL,
    INTERFACE_BLE,
    INTERFACE_DIAGNOSTICS
} InterfaceID;

const char *const InterfaceIDStr[] = {
    "INTERFACE_GENERAL",
    "INTERFACE_BLE",
    "INTERFACE_DIAGNOSTICS",
};

/**
    @brief A mapping between a InterfaceID and a EventID.
*/
struct EventInterfacePair {
    EventID event_id;
    InterfaceID interface_id;
};

/**
    @brief A list of mapping between InterfaceID and EventID.
*/
const EventInterfacePair event_interface_lookup[] = {
        {
            EVENT_HORNNLIGHTSTATE,
            INTERFACE_GENERAL
        },
        
    // VuC
    // {
        // .event_id = EVENT_HORNNLIGHTSTATE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CRASHSTATE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_BLE_CONNECTIONSTATUS,
        // .interface_id = INTERFACE_BLE
    // },
    // {
        // .event_id = EVENT_BLE_RESET_NOTIFICATION,
        // .interface_id = INTERFACE_BLE
    // },
    // {
        // .event_id = EVENT_BLE_MTUSIZE,
        // .interface_id = INTERFACE_BLE
    // },
    // {
        // .event_id = EVENT_DTCFULLREPORT,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // }, 
    // {
        // .event_id = EVENT_DTCSTATUSREPORT,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // },
    // {
        // .event_id = EVENT_AMPLIFIERSTATE,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_RFKLINEERRREPORT,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // }, 
    // {
        // .event_id = EVENT_LASTBUBSTATE,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // }, 
    // {
        // .event_id = EVENT_OHCBTNSTATE,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_VEHICLESPEEDSTATE,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_CANVOLTAGEERRREPORT,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // }, 
    // {
        // .event_id = EVENT_DIAGCOUPLEREPORT,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // },
    // {
        // .event_id = EVENT_NUMBEROFDOORS,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_FUELTYPE,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_STEERINGWHEELPOSITION,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_FUELTANKVOLUME,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_PROPULSIONTYPE,
        // .interface_id = INTERFACE_GENERAL
    // }, 
    // {
        // .event_id = EVENT_ASSISTANCESERVICES,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_AUTONOMOUSDRIVE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_COMBINEDINSTRUMENT,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_TELEMATICMODULE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CONNECTIVITY,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CONNECTEDSERVICEBOOKING,
        // .interface_id = INTERFACE_DIAGNOSTICS
    // },
    // {
        // .event_id = EVENT_PSIMAVAILABILITY,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_WIFIFUNCTIONALITYCONTROL,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_WIFIFREQUENCYBAND,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_WIFIWORKSHOPMODE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CARCFG_THEFTNOTIFICATION,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CARCFG_STOLENVEHICLETRACKING,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_GNSSRECEIVER,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_DIGITALVIDEORECORDER,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_DRIVERALERTCONTROL,
        // .interface_id = INTERFACE_GENERAL
    // },
    // // Data-storage
    // {
        // .event_id = EVENT_DSPARAMETER_INT,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_DSPARAMETER_STR,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_DSPARAMETER_VECT,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CARMODE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_CARUSAGEMODE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_DOORLOCKSTATE,
        // .interface_id = INTERFACE_GENERAL
    // },
// #if 1 //nieyj
    // {
        // .event_id = EVENT_WINOPENSTATE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_PMLVL,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_PARKINGCLIMATEOPER,
        // .interface_id = INTERFACE_GENERAL
    // },
// #endif
    // //uia93888 res
    // {
        // .event_id = EVENT_RMTENGINESECURITYRANDOM,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_RMTENGINESECURITYRESULT,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_RMTENGINE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_DELAYENGINERUNNINGTIME,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_OPERATEREMOTECLIMATE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_OPERATERMTSEATHEAT,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_OPERATERMTSEATVENTILATION,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_OPERATERMTSTEERWHLHEAT,
        // .interface_id = INTERFACE_GENERAL
    // },
    // //end uia93888
    // {
        // .event_id = EVENT_ELECTENGLVL,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_VUCPOWERMODE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_VINNUMBER,
        // .interface_id = INTERFACE_GENERAL
    // },
    // // Message-processor
    // {
        // .event_id = EVENT_ENDOFTRIP,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_HORNNLIGHTSTATE,
        // .interface_id = INTERFACE_GENERAL
    // },
    // // Test
    // {
        // .event_id = EVENT_TESTFROMVGM,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_TESTFROMIHU,
        // .interface_id = INTERFACE_GENERAL
    // },
    // {
        // .event_id = EVENT_TESTFROMVC,
        // .interface_id = INTERFACE_GENERAL
    // },
};

} // namespace vc

#endif // VEHICLE_COMM_MESSAGES_HPP
