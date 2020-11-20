#ifndef TS_SERVER_HPP
#define TS_SERVER_HPP

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

/** @file vc_test.hpp
 * This file implements a test binary simulating telematic service
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <atomic>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <string.h>

#include "vehicle_comm_interface.hpp"
#include "vc_message_queue.hpp"

const char *const VCTestReturnValueStr[] = {
    "VCTESTRET_OK",
    "VCTESTRET_ERR_INVALID_ARG",
    "VCTESTRET_ERR_EXTERNAL",
    "VCTESTRET_ERR_INTERNAL",
};

typedef enum {
    VCTESTRET_OK,
    VCTESTRET_ERR_INVALID_ARG,
    VCTESTRET_ERR_EXTERNAL,
    VCTESTRET_ERR_INTERNAL,
} VCTestReturnValue;

enum Commands {
        LOAD_FILE,
        DELAY,
        WAIT_FOR,
        QUIT,
        REQUEST_LOCKDOOR,
        REQUEST_HORNANDLIGHT,
        REQUEST_SWITCHTOFLASHBOOTLOADER,
        REQUEST_ANTENNASWITCH,
        REQUEST_LANLINKTRIGGER,
        REQUEST_ADC_VALUE,
        REQUEST_VUCTRACE_CONFIG,
        REQUEST_VUCTRACE_CONTROL,
        REQUEST_BLE_DISCONNECT,
        REQUEST_BLE_DISCONNECTALL,
        REQUEST_BLE_GENERATEBDAK,
        REQUEST_BLE_GETBDAK,
        REQUEST_BLE_CONFIG_DATACOMSERVICE,
        REQUEST_BLE_CONFIG_IBEACON,
        REQUEST_BLE_CONFIG_GENERICACCESS,
        REQUEST_BLE_STARTPARINGSERVICE,
        REQUEST_BLE_STOPPARINGSERVICE,
        REQUEST_BLE_WAKEUP_RESPONSE,
        REQUEST_BLE_DATAACCESSAUTHCONFIRMED,
        REQUEST_BLE_STARTADVERTISING,
        REQUEST_BLE_STOPADVERTISING,
        REQUEST_DTC_ENTRIES,
        REQUEST_DTC_CTLSETTINGS,
        REQUEST_IPWAKEUP,
        REQUEST_SETOHCLED,
        REQUEST_VINNUMBER,
        REQUEST_CARMODE,
        REQUEST_CARUSAGEMODE,
        REQUEST_ELECTENERGYLEVEL,
        REQUEST_GETVUCSWBUILD,
        REQUEST_GETBLESWBUILD,
        REQUEST_VUCSWVERSION,
        REQUEST_BLESWVERSION,
        REQUEST_LASTBACKUPBATTERYSTATE,
        REQUEST_GETWIFIDATA_CARCONFIG,
        REQUEST_GETPSIMAVAILABILITY_CARCONFIG,
        REQUEST_GETFUELTYPE_CARCONFIG,
        REQUEST_GETPROPULSIONTYPE_CARCONFIG,
        REQUEST_GETVUCPOWERMODE,
        REQUEST_GETVUCWAKEUPREASON,
        REQUEST_GETOHCBTNSTATE,
        REQUEST_GETNUMBEROFDOORS,
        REQUEST_GETSTEERINGWHEELPOSITION,
        REQUEST_GETFUELTANKVOLUME,
        REQUEST_GETASSISTANCESERVICES,
        REQUEST_GETAUTONOMOUSDRIVE,
        REQUEST_GETCOMBINEDINSTRUMENT,
        REQUEST_GETTELEMATICMODULE,
        REQUEST_GETCONNECTIVITY,
        REQUEST_GETCONNECTEDSERVICEBOOKING,
        REQUEST_GETTCAMHWVERSION,
        REQUEST_GETRFVERSION,
        REQUEST_GETFBLSWBUILD,
        REQUEST_GETBELTINFORMATION,
        REQUEST_CARCONFIGFACTRESTORE,
        REQUEST_GETDIAGERRREPORT,
        REQUEST_SENDPOSNFROMSATLTCON,
        REQUEST_GETDIDGLOBALSNAPSHOTDATA,
        REQUEST_DTCSDETECTED,
        REQUEST_PSIMDATA,
        REQUEST_THERMALMITIGATION,
        REQUEST_GETTHEFTNOTIFICATION,
        REQUEST_GETSTOLENVEHICLETRACKING,
        REQUEST_GETGNSS,
        REQUEST_GETDIGITALVIDEORECORDER,
        REQUEST_GETDRIVERALERTCONTROL,
        REQUEST_GETDOORSTATE,
        REQUEST_WINCTRL,
        REQUEST_SUNROOGCTRL,
        REQUEST_VENTILATION,
        REQUEST_PM25,
        REQUEST_VFCACTIVATE,
        //add by uia93888
        REQUEST_FEEDBACKCERTRESULT,
        REQUEST_ENGINEDELAY,
        REQUEST_CLIMATE,
        REQUEST_SEATHEAT,
        REQUEST_SEATVENTILATION,
        REQUEST_PARKINGCLIMATE,
        // Test
        REQUEST_VGMTEST,
        REQUEST_IHUTEST,
        EVENT_TEST_FROMVC,
        REQUEST_TEST_TOGGLEUSAGEMODE,
        REQUEST_TEST_TOGGLECARMODE,
        REQUEST_TEST_CRASHSTATE,
        REQUEST_TEST_OHCSTATE,
        REQUEST_TEST_SETFAKEVALUESDEFAULT,
        REQUEST_TEST_SETFAKEVALUEINT,
        REQUEST_TEST_SETFAKEVALUESTR,
        SEND_TEST_LOCAL_CONFIG,
};

struct TSCommand {
    std::string name;
    std::vector<std::string> params;
};

TSCommand commands[] = {
    {
        "load",
        {
            "filename",
        },
    },
    {
        "delay",
        {
            "time(sec)",
        },
    },
    {
        "wait_for",
        {
            "name",
            "timeout",
        },
    },
    {
        "quit",
        {
        },
    },
    {
        "Request_LockDoor",
        {
            "centralLockReq",
        },
    },
    {
        "Request_HornAndLight",
        {
            "HNLMode",
        },
    },
    {
        "Request_SwitchToFlashBootloader",
        {
        },
    },
    {
        "Request_AntennaSwitch",
        {
            "mode",
        },
    },
    {
        "Request_LanLinkTrigger",
        {
            "trigger",
            "session_state",
        },
    },
    {
        "Request_ADC_Value",
        {
            "id",
        },
    },
    {
        "Request_VuCTrace_Config",
        {
            "trace_bus",
            "idmodule_id",
            "idmodule_lvl",
        },
    },
    {
        "Request_VuCTrace_Control",
        {
            "mode",
            "interval",
        },
    },
    {
        "Request_BLE_Disconnect",
        {
            "connection_id",
        },
    },
    {
        "Request_BLE_DisconnectAll",
        {
        },
    },
    {
        "Request_BLE_GenerateBdak",
        {
        },
    },
    {
        "Request_BLE_GetBdak",
        {
        },
    },
    {
        "Request_BLE_Config_DatacomService",
        {
        },
    },
    {
        "Request_BLE_Config_Ibeacon",
        {
            "major",
            "minor",
        },
    },
    {
        "Request_BLE_Config_GenericAccess",
        {
        },
    },
    {
        "Request_BLE_StartPairingService",
        {
        },
    },
    {
        "Request_BLE_StopPairingService",
        {
        },
    },
    {
        "Request_BLE_Wakeup_Response",
        {
        },
    },
    {
        "Request_BLE_DataAccessAuthConfirmed",
        {
            "connection_id",
        },
    },
    {
        "Request_BLE_StartAdvertising",
        {
            "adv_frame",
        },
    },
    {
        "Request_BLE_StopAdvertising",
        {
        },
    },
    {
        "Request_DTC_Entries",
        {
            "request_id",
            "event_id",
        },
    },
    {
        "Request_DTC_CtlSettings",
        {
            "settings",
        },
    },
    {
        "Request_IpWakeup",
        {
            "prio",
            "res_group",
        },
    },
    {
        "Request_SetOhcLed",
        {
            "sos_red_status_blink",
            "sos_red_status_repeat",
            "sos_red_duty",
            "sos_white_status_blink",
            "sos_white_status_repeat",
            "sos_white_duty",
            "voc_status_blink",
            "voc_status_repeat",
            "voc_duty",
            "brightness",
        },
    },
    {
        "Request_VINNumber",
        {
        },
    },
    {
        "Request_CarMode",
        {
        },
    },
    {
        "Request_CarUsageMode",
        {
        },
    },
    {
        "Request_ElectEnergyLevel",
        {
        },
    },
    {
        "Request_GetVucSwBuild",
        {
        },
    },
    {
        "Request_GetBleSwBuild",
        {
        },
    },
    {
        "Request_VucSwVersion",
        {
        },
    },
    {
        "Request_BleSwVersion",
        {
        },
    },
    {
        "Request_LastBackupBatteryState",
        {
        },
    },
    {
        "Request_GetWifiData_CarConfig",
        {
        },
    },
    {
        "Request_GetPSIMAvailability_CarConfig",
        {
        },
    },
    {
        "Request_GetFuelType_CarConfig",
        {
        },
    },
    {
        "Request_GetPropulsionType_Carconfig",
        {
        },
    },
    {
        "Request_GetVucPowerMode",
        {
        },
    },
    {
        "Request_GetVucWakeupReason",
        {
        },
    },
    {
        "Request_GetOhcBtnState",
        {
        },
    },
    {
        "Request_GetNumberOfDoors",
        {
        },
    },
    {
        "Request_GetSteeringWheelPosition",
        {
        },
    },
    {
        "Request_GetFuelTankVolume",
        {
        },
    },
    {
        "Request_GetAssistanceServices",
        {
        },
    },
    {
        "Request_GetAutonomousDrive",
        {
        },
    },
    {
        "Request_GetCombinedInstrument",
        {
        },
    },
    {
        "Request_GetTelematicModule",
        {
        },
    },
    {
        "Request_GetConnectivity",
        {
        },
    },
    {
        "Request_GetConnectedServiceBooking",
        {
        },
    },
    {
        "Request_GetTcamHwVersion",
        {
        },
    },
    {
        "Request_GetRfVersion",
        {
        },
    },
    {
        "Request_GetFblSwBuild",
        {
        },
    },
    {
        "Request_GetBeltInformation",
        {
        },
    },
    {
        "Request_CarConfigFactoryRestore",
        {
        },
    },
    {
        "Request_GetDiagErrReport",
        {
        },
    },
    {
        "Request_SendPosnFromSatltCon",
        {
        },
    },
    {
        "Request_GetDIDGlobalSnapshotData",
        {
        },
    },
    {
        "Request_DTCsDetected",
        {
            "dtc id (0-63)",
            "detected"
        },
    },
    {
        "Request_PSIMData",
        {
        },
    },
    {
        "Request_ThermalMitigation",
        {
            "nad_temperature"
        },
    },
    {
        "Request_GetTheftNotification",
        {
        },
    },
    {
        "Request_GetStolenVehicleTracking",
        {
        },
    },
    {
        "Request_GetGNSS",
        {
        },
    },
    {
        "Request_GetDigitalVideoRecorder",
        {
        },
    },
    {
        "Request_GetDriverAlertControl",
        {
        },
    },
    {
        "Request_GetDoorState",
        {
        },
    },
    {
        "Request_WindowsCtrl",
        {
            "mode",
        },
    },
    {
        "Request_SunRoofAndCurtCtrl",
        {
            "mode",
        },
    },
    {
        "Request_WinVentilation",
        {
            "mode",
            "value",
        },
    },
    {
        "Request_PM25Enquire",
        {
        },
    },
    {
        "Request_VFCActivate",
        {
            "id",
            "type",
        },
    },
    //add by uia93888
    {
        "Request_FeedbackCertResult",
        {
        },
    },
    {
        "Reuest_EngineDelay",
        {
        },
    },
    {
        "Request_Climate",
        {
        },
    },
    {
        "Request_SeatHeat",
        {
        },
    },
    {
        "Request_SeatVentilation",
        {
        },
    },
    {
        "Request_ParkingClimate",
        {
        },
    },
    // Test
    {
        "Request_VGMTest",
        {
            "id",
            "return_ack",
            "return_res",
        },
    },
    {
        "Request_IHUTest",
        {
            "id",
            "return_ack",
            "return_res",
        },
    },
    {
        "Event_Test_FromVC",
        {
        },
    },
    {
        "Request_Test_ToggleUsageMode",
        {
        },
    },
    {
        "Request_Test_ToggleCarMode",
        {
        },
    },
    {
        "Request_Test_CrashState",
        {
        },
    },
    {
        "Request_Test_OhcState",
        {
        },
    },
    {
        "Request_Test_SetFakeValuesDefault",
        {
        },
    },
    {
        "Request_Test_SetFakeValueInt",
        {
            "collection",
            "name",
            "value",
        },
    },
    {
        "Request_Test_SetFakeValueStr",
        {
            "collection",
            "name",
            "value",
        },
    },
    {
        "Request_Test_SetLocalConfig",
        {
            "localCfgParam",
            "localCfgParamValue",
        },
    },
};

struct TSMessage {
    vc::MessageType type;
    vc::MessageID id;
    bool arrived;
};


const unsigned int nc = LENGTH(commands);

class VCTest {
  private:
    static std::atomic<bool> exit_;
    static struct termios oldt;

    static void SignalHandler(int signum);

    static void VCTestLog(const char *format, ...);

    long session_id_next = 0;

    vc::IVehicleComm& ivc_;
    vc::IGeneral& igen_;
    vc::IBLE& ible_;
    vc::IDiagnostics& idiag_;


    unsigned int tablookup_idx = 0;
    bool tablookup_active = false;
    bool tabparam_active = false;
    unsigned int match[nc];

    std::list<std::string> cmd_list;

    bool waitfor;
    std::mutex mtx_msg;
    std::mutex mtx_msg2;
    std::condition_variable cv_msg, cv_msg2;
    vc::MessageBase *m_current;

    void EventCallback(vc::MessageBase *m);
    void ResponseCallback(vc::MessageBase *m, vc::ReturnValue status);

    vc::ReturnValue ParseFile(const std::string &fname, std::list<std::string> &cmd_list);

    void PrintPrompt();
    void PrintMenu();

    unsigned int CountMatch();
    unsigned int SetMatch(const char *buffer, unsigned int idx);
    int GetMatch(unsigned int idx);

    unsigned int CountSpaces(const char *buffer, unsigned int idx);
    void ClearBuffer(char *buffer, unsigned int& idx);
    void ClearParam(const char *buffer, unsigned int idx);

    void TabComplete(char *buffer, unsigned int& idx);

    vc::ReturnValue Send_Request_LockDoor(vc::ReqDoorLockUnlock& req);
    vc::ReturnValue Send_Request_HornAndLight(vc::ReqHornNLight& req);
    vc::ReturnValue Send_Request_SwitchToFlashBootloader();
    vc::ReturnValue Send_Request_AntennaSwitch(vc::ReqAntennaSwitch& req);
    vc::ReturnValue Send_Request_LastBackupBatteryState();
    vc::ReturnValue Send_Request_LanLinkTrigger(vc::ReqLanLinkTrigger& req);
    vc::ReturnValue Send_Request_AdcValue(vc::ReqAdcValue& req);
    vc::ReturnValue Send_Request_VucTraceConfig(vc::ReqVucTraceConfig& req);
    vc::ReturnValue Send_Request_VucTraceControl(vc::ReqVucTraceControl& req);

    vc::ReturnValue Send_Request_BleDisconnect(vc::ReqBleDisconnect& req);
    vc::ReturnValue Send_Request_BleDisconnectAll();
    vc::ReturnValue Send_Request_BleGenerateBdak();
    vc::ReturnValue Send_Request_BleGetBdak();
    vc::ReturnValue Send_Request_BleConfigDatacomService(vc::ReqBleCfgDataCommService& req);
    vc::ReturnValue Send_Request_BleConfigIbeacon(vc::ReqBleCfgIbeacon& req);
    vc::ReturnValue Send_Request_BleConfigGenericAccess(vc::ReqBleCfgGenAcc& req);
    vc::ReturnValue Send_Request_BleStartPairingService(vc::ReqBleStartPairService& req);
    vc::ReturnValue Send_Request_BleStopPairingService();
    vc::ReturnValue Send_Request_BleDataAccessAuthConfirmed(vc::ReqBleDataAccAuthCon& req);
    vc::ReturnValue Send_Request_BleWakeupResponse();
    vc::ReturnValue Send_Request_BleStartAdvertising(vc::ReqBleStartAdvertising& req);
    vc::ReturnValue Send_Request_BleStopAdvertising();
    vc::ReturnValue Send_Request_DtcEntries(vc::ReqDtcEntries& req);
    vc::ReturnValue Send_Request_DtcCtlSettings(vc::ReqDtcCtlSettings& req);
    vc::ReturnValue Send_Request_IpWakeup(vc::ReqIpWakeup& req);
    vc::ReturnValue Send_Request_SetOhcLed(vc::ReqSetOhcLed& req);

    vc::ReturnValue Send_Request_VINNumber();
    vc::ReturnValue Send_Request_CarUsageMode();
    vc::ReturnValue Send_Request_CarMode();
    vc::ReturnValue Send_Request_GetVucSwBuild();
    vc::ReturnValue Send_Request_GetBleSwBuild();
    vc::ReturnValue Send_Request_VucSwVersion();
    vc::ReturnValue Send_Request_BleSwVersion();
    vc::ReturnValue Send_Request_ElectEnergyLevel();
    vc::ReturnValue Send_Request_GetWifiData();
    vc::ReturnValue Send_Request_GetPSIMAvailability();
    vc::ReturnValue Send_Request_GetFuelType();
    vc::ReturnValue Send_Request_GetPropulsionType();
    vc::ReturnValue Send_Request_GetVucPowerMode();
    vc::ReturnValue Send_Request_GetVucWakeupReason();
    vc::ReturnValue Send_Request_GetOhcBtnState();

    vc::ReturnValue Send_Request_GetNumberOfDoors();
    vc::ReturnValue Send_Request_GetSteeringWheelPosition();
    vc::ReturnValue Send_Request_GetFuelTankVolume();
    vc::ReturnValue Send_Request_GetAssistanceServices();
    vc::ReturnValue Send_Request_GetAutonomousDrive();
    vc::ReturnValue Send_Request_GetCombinedInstrument();
    vc::ReturnValue Send_Request_GetTelematicModule();
    vc::ReturnValue Send_Request_GetConnectivity();
    vc::ReturnValue Send_Request_GetConnectedServiceBooking();
    vc::ReturnValue Send_Request_GetTcamHwVersion();
    vc::ReturnValue Send_Request_GetRfVersion();
    vc::ReturnValue Send_Request_GetFblSwBuild();
    vc::ReturnValue Send_Request_GetBeltInformation();
    vc::ReturnValue Send_Request_CarConfigFactoryRestore();
    vc::ReturnValue Send_Request_SendPosnFromSatltCon(vc::ReqSendPosnFromSatltCon& req);
    vc::ReturnValue Send_Request_GetDiagErrReport();
    vc::ReturnValue Send_Request_GetDIDGlobalSnapshotData();
    vc::ReturnValue Send_Request_DTCsDetected(vc::ReqDTCsDetected& req);
    vc::ReturnValue Send_Request_PSIMData(vc::ReqPSIMData& req);
    vc::ReturnValue Send_Request_ThermalMitigation(vc::ReqThermalMitigataion& req);
    vc::ReturnValue Send_Request_GetTheftNotification();
    vc::ReturnValue Send_Request_GetStolenVehicleTracking();
    vc::ReturnValue Send_Request_GetGNSS();
    vc::ReturnValue Send_Request_GetDigitalVideoRecorder();
    vc::ReturnValue Send_Request_GetDriverAlertControl();
    vc::ReturnValue Send_Request_GetDoorState();
    vc::ReturnValue Send_Request_WindowsCtrl(vc::ReqWinCtrl& req);
    vc::ReturnValue Send_Request_SunRoofAndCurtCtrl(vc::ReqRoofCurtCtrl& req);
    vc::ReturnValue Send_Request_WinVentilation(vc::ReqWinVentilation& req);
    vc::ReturnValue Send_Request_PM25Enquire();
    vc::ReturnValue Send_Request_VFCActivate(vc::ReqVFCActivate& req);
    vc::ReturnValue Send_Request_RMTFeedbackCertResult(vc::Req_RMTEngineSecurityResponse& req);
    vc::ReturnValue Send_Request_EngineDelay(vc::Req_DelayEngineRunngTime& req);
    vc::ReturnValue Send_Request_Climate(vc::Req_OperateRMTClimate& req);
    vc::ReturnValue Send_Request_SeatHeat(vc::Req_OperateRMTSeatHeat& req);
    vc::ReturnValue Send_Request_SeatVentilation(vc::Req_OperateRMTSeatVenti& req);
    vc::ReturnValue Send_Request_ParkingClimate(vc::Req_ParkingClimateOper_S& req);
    

    // Test
    vc::ReturnValue Send_Request_GetDSParameter(vc::ReqGetDSParameter& req);
    vc::ReturnValue Send_Request_VGMTest(vc::ReqVGMTestMsg& req);
    vc::ReturnValue Send_Request_IHUTest(vc::ReqIHUTestMsg& req);
    vc::ReturnValue Send_Event_Test_FromVC(vc::EventTestFromVC& ev);
    vc::ReturnValue Send_Request_Test_ToggleUsageMode();
    vc::ReturnValue Send_Request_Test_ToggleCarMode();
    vc::ReturnValue Send_Request_Test_CrashState();
    vc::ReturnValue Send_Request_Test_OhcState();
    vc::ReturnValue Send_Request_Test_SetFakeValuesDefault();
    vc::ReturnValue Send_Request_Test_SetFakeValueInt(vc::ReqTestSetFakeValueInt& req);
    vc::ReturnValue Send_Request_Test_SetFakeValueStr(vc::ReqTestSetFakeValueStr& req);
    vc::ReturnValue Send_Request_Test_LocalConfig(vc::ReqTestSetLocalConfig& req);
    
    vc::ReturnValue WaitForMessage(std::unique_lock<std::mutex>& lk, std::list<TSMessage>&& list_msg, int timeout);
    void PrintResult(bool ok);
    int IsCommand(std::string& str);
    int CreateCommand(std::string& cmd_line, std::string& cmd, std::vector<std::string> &args);
    vc::ReturnValue ExecuteCommand(std::string& cmd);

  public:
    VCTest();
    ~VCTest();

    VCTestReturnValue Init();
    VCTestReturnValue Start();
    vc::ReturnValue RunFile(const std::string &fname);
};

#endif // TSSERVER_HPP
