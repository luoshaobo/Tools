#ifndef VUC_COMM_HPP
#define VUC_COMM_HPP

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

/** @file vuc_comm.hpp
 * This file handles the communication with the VuC, using the OTP VCS-interface
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include <stdio.h>
#include <string.h>

//To use be used for ipc_api_defines and ipc_api_messages
#define FEATURE_IPC_QCOM_NAD
#define FEATURE_VOLVO
#define FEATURE_HIGH

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define BIT(k,m,n) LAST((k)>>(m),((n)-(m)))

extern "C"
{
#include "tpsys.h"
#include "ipc_api_define.h"
#include "vehicle_comm_service.h"
#include "vehicle_comm_service_shared.h"
}

#include "vehicle_comm_messages.hpp"
#include "vc_message_queue.hpp"
#include "vc_session_list.hpp"
#include "vc_common.hpp"
#include "vc_data_storage.hpp"
#include "vuc_messages.hpp"

namespace vc {

#define RESPONSE_TIMEOUT_MS 2000
#define RESPONSE_TRIES      2
#define TIME_RESOLUTION_MS  100

#define CHECK_SIZE(expected_size, input_size, request_id) do { \
            if (expected_size != input_size) { \
                LOG(LOG_WARN, "VuCComm: %s: size mismatch expected_size = %d  input_size = %d", __FUNCTION__, expected_size, input_size); \
                if (request_id > 0) \
                    FinishRequest((RequestID)request_id, RET_ERR_EXTERNAL, NULL); \
                return; \
            } \
        } while (0)

/**
    @brief A session-entry

    This struct represent one VC-request being sent to the external node of
    that VC-endpoint.
*/
typedef struct {
    int car_config_id;
    std::string par_name;
    EventID event_id;
} CarConfigParameter;

const CarConfigParameter car_config_params[] = {
    {
        2,
        "NumberOfDoors",
        EVENT_NUMBEROFDOORS
    },
    {
        6,
        "FuelType",
        EVENT_FUELTYPE
    },
    {
        8,
        "SteeringWheelPosition",
        EVENT_STEERINGWHEELPOSITION
    },
    {
        11,
        "FuelTankeVolume",
        EVENT_FUELTANKVOLUME
    },
    {
        13,
        "PropulsionType",
        EVENT_PROPULSIONTYPE
    },
    {
        75,
        "CarCfgTheftNotificaion",
        EVENT_CARCFG_THEFTNOTIFICATION
    },
    {
        76,
        "CarCfgStolenVehicleTracking",
        EVENT_CARCFG_STOLENVEHICLETRACKING
    },
    {
        77,
        "AssistanceServices",
        EVENT_ASSISTANCESERVICES
    },
    {
        100,
        "AutonomousDrive",
        EVENT_AUTONOMOUSDRIVE 
    },
    {
        157,
        "CombinedInstrument",
        EVENT_COMBINEDINSTRUMENT 
    },
    {
        191,
        "TelematicModule",
        EVENT_TELEMATICMODULE
    },
    {
        196,
        "WiFiFrequencyBand",
        EVENT_WIFIFREQUENCYBAND
    },
    {
        200,
        "WiFiFunctionalityControl",
        EVENT_WIFIFUNCTIONALITYCONTROL
    },
    {
        201,
        "Connectivity",
        EVENT_CONNECTIVITY
    },
    {
        221,
        "ConnectedServiceBooking",
        EVENT_CONNECTEDSERVICEBOOKING
    },
    {
        361,
        "PSIMAvailability",
        EVENT_PSIMAVAILABILITY
    },
    {
        365,
        "WiFiWorkshopMode",
        EVENT_WIFIWORKSHOPMODE
    },
    {
        453,
        "GNSSReceiver",
        EVENT_GNSSRECEIVER
    },
    {
        461,
        "ApprovedCellularStandards",
        EVENT_APPROVEDCELLULARSTANDARDS
    },
    {
        483,
        "DigitalVideoRecorder",
        EVENT_DIGITALVIDEORECORDER 
    },
    {
        500,
        "DriverAlertControl",
        EVENT_DRIVERALERTCONTROL
    }
};

/**
    @brief Mapping between a VCRequest and a VuC-request

    This struct represents a mapping of one VC request-ID to a VuC request-ID.
*/
struct VuCRequestPair {
    RequestID request_id;
    VuCMessageID vuc_request_id;
};


static const VCRequestDestination routing_table_vuc[] = {
    {
        .request_id = REQ_UNKNOWN,
        .dst = ENDPOINT_UNKNOWN
    },
};

/**
    @brief Lookup-table for VCRequests<->VuC-requests

    A list of all known mappings between VCRequests and VuC-requests.
*/
const VuCRequestPair vuc_request_lookup[] = {
    {
        .request_id = REQ_DOORLOCKUNLOCK,
        .vuc_request_id = IPC_NAD_REMOTE_DOOR_UNLOCK_REQ
    },
    {
        .request_id = REQ_HORNNLIGHT,
        .vuc_request_id = IPC_NAD_HORN_LIGHT_REQ
    },
    {
        .request_id = REQ_VUCTRACE_CONFIG,
        .vuc_request_id = IPC_NAD_TRACE_REP
    },
    {
        .request_id = REQ_VUCTRACE_CONTROL,
        .vuc_request_id = IPC_NAD_TRACE_REP
    },
    {
        .request_id = REQ_SWITCHTOBOOT,
        .vuc_request_id = IPC_NAD_SWITCH_TO_BOOT_REQ
    },
    {
        .request_id = REQ_ANTENNASWITCH,
        .vuc_request_id = IPC_NAD_ANTENNA_SWITCH_REQ
    },
    {
        .request_id = REQ_LANLINKTRIGGER,
        .vuc_request_id = IPC_NAD_LAN_LINK_TRIG_REP
    },
    {
        .request_id = REQ_GETCARCONFIG,
        .vuc_request_id = IPC_NAD_CAR_CONFIG_REQ
    },
    {
        .request_id = REQ_CARCONFIGFACTRESTORE,
        .vuc_request_id = IPC_NAD_CAR_CONFIG_FACTORY_RESTORE_REQ
    },
    {
        .request_id = REQ_BLE_DISCONNECT,
        .vuc_request_id = IPC_NAD_BLE_DISCONNECT_REQ
    },
    {
        .request_id = REQ_BLE_DISCONNECTALL,
        .vuc_request_id = IPC_NAD_BLE_DISCONNECT_ALL_REQ
    },
    {
        .request_id = REQ_BLE_GENERATEBDAK,
        .vuc_request_id = IPC_NAD_BLE_GENERATE_BDAK_REQ
    },
    {
        .request_id = REQ_BLE_GETBDAK,
        .vuc_request_id = IPC_NAD_BLE_GET_BDAK_REQ
    },
    {
        .request_id = REQ_BLE_CONFIG_DATACOMSERVICE,
        .vuc_request_id = IPC_NAD_BLE_CONFIG_DATACOM_SERVICE_REQ
    },
    {
        .request_id = REQ_BLE_CONFIG_IBEACON,
        .vuc_request_id = IPC_NAD_BLE_CONFIG_IBEACON_REQ
    },
    {
        .request_id = REQ_BLE_CONFIG_GENERICACCESS,
        .vuc_request_id = IPC_NAD_BLE_CONFIG_GENERIC_ACCESS_REQ
    },
    {
        .request_id = REQ_BLE_STARTPARINGSERVICE,
        .vuc_request_id = IPC_NAD_BLE_START_PARING_SERVICE_REQ
    },
    {
        .request_id = REQ_BLE_STOPPARINGSERVICE,
        .vuc_request_id = IPC_NAD_BLE_STOP_PARING_SERVICE_REQ
    },
    {
        .request_id = REQ_BLE_WAKEUP_RESPONSE,
        .vuc_request_id = IPC_NAD_BLE_WAKEUP_RESPONSE_REP
    },
    {
        .request_id = REQ_BLE_DATAACCESSAUTHCONFIRMED,
        .vuc_request_id = IPC_NAD_BLE_DATA_ACCESS_AUTH_CONFIRMED_REP
    },
    {
        .request_id = REQ_BLE_STARTADVERTISING,
        .vuc_request_id = IPC_NAD_BLE_START_ADVERTISING_REQ,
    },
    {
        .request_id = REQ_BLE_STOPADVERTISING,
        .vuc_request_id = IPC_NAD_BLE_STOP_ADVERTISING_REQ,
    },
    {
        .request_id = REQ_DTCENTRIES,
        .vuc_request_id = IPC_NAD_DTC_HANDLING_REQ
    },
    {
        .request_id = REQ_DTCCTLSETTINGS,
        .vuc_request_id = IPC_NAD_DTC_HANDLING_REQ
    },
    {
        .request_id = REQ_IPWAKEUP,
        .vuc_request_id = IPC_NAD_IP_WAKEUP_REQ
    },
    {
        .request_id = REQ_SETOHCLED,
        .vuc_request_id = IPC_NAD_OHC_LED_REQ
    },
    {
        .request_id = REQ_VINNUMBER,
        .vuc_request_id = IPC_NAD_VIN_REQ
    },
    {
        .request_id = REQ_GETMSGCATVERSION,
        .vuc_request_id = IPC_NAD_MESSAGE_CATALOGUE_VERSION_REQ
    },
    {
        .request_id = REQ_GETTCAMHWVERSION,
        .vuc_request_id = IPC_NAD_VUC_HARDWARE_VERSION_REQ
    },
    {
        .request_id = REQ_GETVUCBLESWPARTNUMBER,
        .vuc_request_id = IPC_NAD_VUC_BLE_SW_PARTNUMBER_REQ
    },
    {
        .request_id = REQ_GETVUCSWBUILD,
        .vuc_request_id = IPC_NAD_VUC_SW_BUILD_DATE_REQ
    },
    {
        .request_id = REQ_GETBLESWBUILD,
        .vuc_request_id = IPC_NAD_BLE_SW_BUILD_DATE_REQ
    },
    {
        .request_id = REQ_GETRFVERSION,
        .vuc_request_id = IPC_NAD_RF_SW_HW_VERSION_REQ
    },
    {
        .request_id = REQ_GETFBLSWBUILD,
        .vuc_request_id = IPC_NAD_FBL_SW_BUILD_DATE_REQ
    },
    {
        .request_id = REQ_GETBELTINFORMATION,
        .vuc_request_id = IPC_NAD_BELT_INFO_REQ
    },
    {
        .request_id = REQ_GETDIAGERRREPORT,
        .vuc_request_id = IPC_NAD_DIAG_ERR_REQ
    },
    {
        .request_id = REQ_GETVEHICLESTATE,
        .vuc_request_id = IPC_NAD_VEHICLE_GEN_STATE_REQ
    },
    {
        .request_id = REQ_GETOHCBTNSTATE,
        .vuc_request_id = IPC_NAD_OHC_BUTTON_STATE_REQ
    },
    {
        .request_id = REQ_LASTBUBSTATE,
        .vuc_request_id = IPC_NAD_BUB_STATE_REQ
    },
    {
        .request_id = REQ_ELECTENGLVL,
        .vuc_request_id = IPC_NAD_ELECT_ENG_LVL_STATE_REQ
    },
    {
        .request_id = REQ_GETDIDGLOBALSNAPSHOTDATA,
        .vuc_request_id = IPC_NAD_GLOBAL_SS_DATA_DID_REQ
    },
    {
        .request_id = REQ_DTCSDETECTED,
        .vuc_request_id = IPC_NAD_SOC_DID_STATUS_REQ
    },
    {
        .request_id = REQ_PSIMDATA,
        .vuc_request_id = IPC_NAD_PSIM_DATA_COMMUNICATION_REQ
    },
    {
        .request_id = REQ_THERMALMITIGATION,
        .vuc_request_id = IPC_NAD_THERMAL_MITIGATION_REQ
    },
    {
        .request_id = REQ_CARCFGPARAMETERFAULT,
        .vuc_request_id = IPC_NAD_CAR_CFG_PARAM_FAULT_REQ
    },
    {
        .request_id = REQ_NETWORK_MANAGEMENT_HISTORY,
        .vuc_request_id = IPC_NAD_NETWORK_MANAGEMENT_HISTORY_REQ
    },
    {
        .request_id = REQ_PROGRAM_PRECOND_CHK,
        .vuc_request_id = IPC_NAD_PROGRAM_PRECOND_CHK_REQ
    },
    {
        .request_id = REQ_WINCTRL,
        .vuc_request_id = IPC_NAD_WIN_CTRL_REQ_REQ
    },
    {
        .request_id = REQ_ROOFCTRL,
        .vuc_request_id = IPC_NAD_SUNROOF_CTRL_REQ_REQ
    },
    {
        .request_id = REQ_WINVENTI,
        .vuc_request_id = IPC_NAD_ONE_BTN_REQ_REQ
    },
    {
        .request_id = REQ_PM25ENQUIRE,
        .vuc_request_id = IPC_NAD_PM25_ENQUIRES_REQ_REQ
    },
    {
        .request_id = REQ_VFCACTIVATE,
        .vuc_request_id = IPC_NAD_VFC_REQ_REQ
    },
    //uia93888  res
    {
        .request_id = REQ_RMTENGINE,
        .vuc_request_id = IPC_NAD_ENGINE_REMOTE_START_REQ
    },
    {//add by uia93888 feedback cert result
        .request_id = REQ_RMTENGINESECURITYRESPONSE,
        .vuc_request_id = IPC_NAD_FEEDBACK_CERT_RESULT_CEM_REQ
    },
    {//add by uia93888
        .request_id = REQ_RTCAWAKEVUC,
        .vuc_request_id = IPC_NAD_SET_RTC_TMR_REQ
    },
    {//add by uia93888
        .request_id = REQ_DELAYENGINERUNNINGTIME,
        .vuc_request_id = IPC_NAD_REM_ENG_RUNNING_DELAY_REQ
    },
    {//add by uia93888
        .request_id = REQ_OPERATEREMOTECLIMATE,
        .vuc_request_id = IPC_NAD_REMOTE_CLIMATIZATION_REQ
    },
    {//add by uia93888 seat heat
        .request_id = REQ_OPERATERMTSEATHEAT,
        .vuc_request_id = IPC_NAD_REM_SEAT_HEATING_REQ
    },
    {//add by uia93888 seat venti
        .request_id = REQ_OPERATERMTSEATVENTILATION,
        .vuc_request_id = IPC_NAD_REM_SEAT_VENTILATION_REQ
    },
    {//add by uia93888 steer wheel heat
        .request_id = REQ_OPERATERMTSTEERWHLHEAT,
        .vuc_request_id = IPC_NAD_STEEL_WHELL_HEAT_CTRL_REQ
    },
    {//add by uia93888 parking climate
        .request_id = REQ_PARKINGCLIMATEOPER,
        .vuc_request_id = IPC_NAD_PARKING_CLIMA_CTRL_REQ
    },
    {
        .request_id = REQ_TEST_SETLOCALCONFIG,
        .vuc_request_id = IPC_NAD_LOCAL_CONFIG_PARAM_RESP
    },
    {
        .request_id = REQ_ROLLOVERVALUE,
        .vuc_request_id = IPC_NAD_ROLLOVER_PARAM_VALUE_REQ
    },
    {
        .request_id = REQ_SETROLLOVERCFG,
        .vuc_request_id = IPC_NAD_SET_ROLLOVER_PARAM_CFG_REQ
    },
};


/**
    @brief Mapping between a VCResponse and a VuC-response

    This struct represents a mapping of one VC response-ID to a VuC response-ID.
*/
struct VuCResponsePair {
    ResponseID response_id;
    VuCMessageID vuc_response_id;
};

/**
    @brief Lookup-table for VCResponse<->VuC-response

    A list of all known mappings between VCResponse and VuC-response.
*/
const VuCResponsePair vuc_response_lookup[] = {
};



/**
    @brief VC-endpoint handling communication with the VuC

    This class is a VC-endpoint handling requests to the VuC and
    responses and events from the VuC to any other VC-endpoint.
*/
class VuCComm {
private:
    static SessionList slist_;                  /**< The main message-queue, belonging to the VehicleComm-instance */
    static MessageQueue *mq_;                   /**< A list of sessions keeping track of requests sent to the VuC */
    static DataStorage *ds_;                    /**< Pointer to access datastorage directly */

    static Endpoint DestinationLookup(RequestID request_id);
    static void SignalIntCallback(const gchar *signal, const gint data);
    static void AdcResponseCallback(const tpVCS_AdcResponseInformation_t data);
    static void CrashMessageCallback(const tpVCS_CrashMessage_t data);
    static void FinishRequest(RequestID message_id, ReturnValue return_id, const unsigned char *data);
    static void SendEventMessage(EventID message_id, const unsigned char *data, Endpoint endpoint);
    static void SendRequestMessage(RequestID message_id, const unsigned char *data);

    static void HandleEvHornAndLight(const tpVCS_CustomMessage_t *data);
    static void HandleEvVehicleGenState(const tpVCS_CustomMessage_t *data);
    static void HandleResVehicleGenState(const tpVCS_CustomMessage_t *data);
    static void HandleResGetCarConfig(const tpVCS_CustomMessage_t *data);
    static void HandleEvGetCarConfig(const tpVCS_CustomMessage_t *data);
    static void HandleResCarConfigFactRest(const tpVCS_CustomMessage_t *data);
    static void HandleResAntennaSwitch(const tpVCS_CustomMessage_t *data);
    static void HandleResDiagErr(const tpVCS_CustomMessage_t *data);
    static void HandleResVucSwBuild(const tpVCS_CustomMessage_t *data);
    static void HandleResBleSwBuild(const tpVCS_CustomMessage_t *data);
    static void HandleEvVucTrace(const tpVCS_CustomMessage_t *data);
    static void HandleEvVucDiagErr(const tpVCS_CustomMessage_t *data);
    static void HandleEvElecEngLvl(const tpVCS_CustomMessage_t *data);
    static void HandleResElecEngLvl(const tpVCS_CustomMessage_t *data);
    static void HandleEvBubState(const tpVCS_CustomMessage_t *data);
    static void HandleResBubState(const tpVCS_CustomMessage_t *data);
    static void HandleResCarCfgFaults(const tpVCS_CustomMessage_t *data);
    static void HandleResNetworkManageHistory(const tpVCS_CustomMessage_t *data);
    static void HandleResProgramPreCheck(const tpVCS_CustomMessage_t *data);
    static void HandleResBleDisconnect(const tpVCS_CustomMessage_t *data);
    static void HandleResBleDisconnectAll(const tpVCS_CustomMessage_t *data);
    static void HandleResBleGenBdak(const tpVCS_CustomMessage_t *data);
    static void HandleResBleGetBdak(const tpVCS_CustomMessage_t *data);
    static void HandleResBleCfgDataServ(const tpVCS_CustomMessage_t *data);
    static void HandleResBleCfgIbeacon(const tpVCS_CustomMessage_t *data);
    static void HandleResBleCfgGenAcc(const tpVCS_CustomMessage_t *data);
    static void HandleResBleStartService(const tpVCS_CustomMessage_t *data);
    static void HandleResBleStopService(const tpVCS_CustomMessage_t *data);
    static void HandleEvBleConnStatus(const tpVCS_CustomMessage_t *data);
    static void HandleEvBleResetNotification(const tpVCS_CustomMessage_t *data);
    static void HandleResBleStartAdvertising(const tpVCS_CustomMessage_t *data);
    static void HandleResBleStopAdvertising(const tpVCS_CustomMessage_t *data);
    static void HandleEvBleMtuSize(const tpVCS_CustomMessage_t *data);
    static void HandleResDtcHandling(const tpVCS_CustomMessage_t *data);
    static void HandleEvDtcFullEntry(const tpVCS_CustomMessage_t *data);
    static void HandleEvDtcStatusEntry(const tpVCS_CustomMessage_t *data);
    static void HandleEvAmplifierState(const tpVCS_CustomMessage_t *data);
    static void HandleEvRfKlineErrorCodes(const tpVCS_CustomMessage_t *data);
    static void HandleEvPowerMode(const tpVCS_CustomMessage_t *data);
    static void HandleEvOhcBtnState(const tpVCS_CustomMessage_t *data);
    static void HandleResOhcBtnState(const tpVCS_CustomMessage_t *data);
    static void HandleResVINNumber(const tpVCS_CustomMessage_t *data);
    static void HandleResMsgCatVersion(const tpVCS_CustomMessage_t *data);
    static void HandleResTcamHwVersion(const tpVCS_CustomMessage_t *data);
    static void HandleResVucBleSwPartNumber(const tpVCS_CustomMessage_t *data);
    static void HandleResRfSwHwVersion(const tpVCS_CustomMessage_t *data);
    static void HandleEvVINNumber(const tpVCS_CustomMessage_t *data);
    static void HandleResFblSwBuild(const tpVCS_CustomMessage_t *data);
    static void HandleResBeltInformation(const tpVCS_CustomMessage_t *data);
    static void HandleEvVehicleSpeedState(const tpVCS_CustomMessage_t *data);
    static void HandleEvCanVoltageErr(const tpVCS_CustomMessage_t *data);
    static void HandleReqLocalCfgParam(const tpVCS_CustomMessage_t *data);
    static void HandleEvDiagCoupleReport(const tpVCS_CustomMessage_t *data);
    static void HandleResDIDGlobalSnapshotData(const tpVCS_CustomMessage_t *data);
    static void HandleResDTCsDetected(const tpVCS_CustomMessage_t *data);
    static void HandleResPSIMData(const tpVCS_CustomMessage_t *data);
    static void HandleResThermalMitigation(const tpVCS_CustomMessage_t *data);
    static void HandleEvSunroofWinState(const tpVCS_CustomMessage_t *data);
    static void HandleEvPM25EnquiresReport(const tpVCS_CustomMessage_t *data);
#if 1 // nieyj
    static void HandleResVFCActivate(const tpVCS_CustomMessage_t *data);
#endif
    //uia93888 remote start engine 
    static void HandleResRMTEngineChallengeCode(const tpVCS_CustomMessage_t *data);
    static void HandleResRMTEngineCertResult(const tpVCS_CustomMessage_t *data);
    static void HandleResRMTEngine(const tpVCS_CustomMessage_t *data);
    static void HandleResDelayEngineRunngTime(const tpVCS_CustomMessage_t *data);
    static void HandleResOperateRemoteClimate(const tpVCS_CustomMessage_t *data);
    static void HandleResOperateRemoteSeatHeat(const tpVCS_CustomMessage_t *data);
    static void HandleResOperateRemoteSeatVenti(const tpVCS_CustomMessage_t *data);
    static void HandleResOperateRemoteSteerWhlHeat(const tpVCS_CustomMessage_t *data);
    static void HandleResParkingClimate(const tpVCS_CustomMessage_t *data);
    //end uia93888
	static void HandleResDoorLockUnlockState(const tpVCS_CustomMessage_t *data);
    static void HandleResRolloverValue(const tpVCS_CustomMessage_t *data);
    static void HandleResSetRolloverCfg(const tpVCS_CustomMessage_t *data);

    ReturnValue VuCRequestLookup(RequestID request_id, VuCMessageID& message_id);
    ReturnValue VuCResponseLookup(ResponseID response_id, VuCMessageID& message_id);
    ReturnValue SendCustomMessage(MessageBase *m, tpVCS_CustomMessage_t *msg);
    ReturnValue SendVuCMessage(MessageBase *m);

    // Test
    bool PerformSimulationOperations(MessageBase *m);

public:
    static void CustomMessageCallback(const guint32 messageId, const tpVCS_CustomMessage_t data);

    VuCComm(MessageQueue *mq, DataStorage *ds_);
    ~VuCComm();

    ReturnValue Init();
    ReturnValue Start();
    ReturnValue Stop();
    ReturnValue SendMessage(MessageBase *m);
};

} // namespace vc

#endif // VUC_COMM_HPP
