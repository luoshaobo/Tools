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

/** @file vc_common.cpp
 * This file implements general functions used by several parts of the VehicleComm module.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vc_utility.hpp"

namespace vc {

/**
    @brief Get request_id from response_id

    Get a request-ID when you know the response-ID for a method.
    This is done by iterating through the message_pair_lookup array.

    @param[in]  response_id     GDBus bus connection
    @return     The request-ID found, or REQ_UNKNOWN if not found
*/
RequestID RequestLookup(ResponseID response_id)
{
    for (int i = 0; i < LENGTH(message_pair_lookup); i++)
        if (message_pair_lookup[i].response_id == response_id)
            return message_pair_lookup[i].request_id;

    return REQ_UNKNOWN;
}

/**
    @brief Get response_id from request_id

    Get a response-ID when you know the request-ID for a method.
    This is done by iterating through the message_pair_lookup array.

    @param[in]  requst_id     GDBus bus connection
    @return     The response-ID found, or REQ_UNKNOWN if not found
*/
ResponseID ResponseLookup(RequestID request_id)
{
    for (int i = 0; i < LENGTH(message_pair_lookup); i++)
        if (message_pair_lookup[i].request_id == request_id)
            return message_pair_lookup[i].response_id;

    return RES_UNKNOWN;
}

/**
    @brief Get a friendly name for a message-ID.

    Finds and returns a string corresponding to a MessageID in
    the list specified by the MessageType.
    This is done by iterating through the message_pair_lookup array.

    @param[in]  id     The message-ID enum
    @param[in]  type   The message-type enum
    @return     Char pointer to a message-ID name
*/
const char *MessageStr(MessageID id, MessageType type)
{
#if 1 //nieyj fix klocwork 197556 & 279013
    int size = 0;
    switch (type) {
    case MESSAGE_REQUEST:
        size = sizeof(VCRequestStr) / sizeof(char*);
        break;
    case MESSAGE_RESPONSE:
        size = sizeof(ResponseStr) / sizeof(char*);
        break;
    case MESSAGE_EVENT:
        size = sizeof(EventStr) / sizeof(char*);
        break;
    case MESSAGE_ERROR:
        size = sizeof(ReturnValueStr) / sizeof(char*);
        break;
    default:
        return NULL;
    }

    if (id >= size)
    {
        return NULL;
    }
#endif
    switch (type) {
    case MESSAGE_REQUEST:
        return VCRequestStr[id];
    case MESSAGE_RESPONSE:
        return ResponseStr[id];
    case MESSAGE_EVENT:
        return EventStr[id];
    case MESSAGE_ERROR:
        return ReturnValueStr[id];
    default:
        return NULL;
    }
}

const char *MessageStr(MessageBase *m)
{
    return MessageStr(m->id_, m->type_);
}

RequestID RequestNameLookup(const std::string& req_name)
{
    for (int i = 0; i < LENGTH(VCRequestStr); i++)
        if (req_name.compare(VCRequestStr[i]) == 0)
            return (RequestID)i;

    return REQ_UNKNOWN;
}

ResponseID ResponseNameLookup(const std::string& res_name)
{
    for (int i = 0; i < LENGTH(ResponseStr); i++)
        if (res_name.compare(ResponseStr[i]) == 0)
            return (ResponseID)i;

    return RES_UNKNOWN;
}

EventID EventNameLookup(const std::string& ev_name)
{
    for (int i = 0; i < LENGTH(EventStr); i++)
        if (ev_name.compare(EventStr[i]) == 0)
            return (EventID)i;

    return EVENT_UNKNOWN;
}

bool MessageNameLookup(const std::string& msg_name, MessageType& type, MessageID& id)
{
    RequestID id_req = RequestNameLookup(msg_name);
    if (id_req != REQ_UNKNOWN) {
        type = MESSAGE_REQUEST;
        id = (MessageID)id_req;
        return true;
    }

    ResponseID id_res = ResponseNameLookup(msg_name);
    if (id_res != RES_UNKNOWN) {
        type = MESSAGE_RESPONSE;
        id = (MessageID)id_res;
        return true;
    }

    EventID id_ev = EventNameLookup(msg_name);
    if (id_ev != EVENT_UNKNOWN) {
        type = MESSAGE_EVENT;
        id = (MessageID)id_ev;
        return true;
    }

    return false;
}

/**
    @brief Create a new Message.

    Creates a new Message from the input by the caller.

    @param[in]  id          The message-ID enum
    @param[in]  type        The message-type enum
    @param[in]  client_id   Identifier for a VehicleCommClient
    @param[in]  session_id  Identifier for a request/response pair
    @param[in]  src         The source node of the message
    @param[in]  src         The destination node of the message
    @param[in]  data        Pointer to the data of the specific message
    @return     The created message
*/
MessageBase *InitMessage(MessageID id, MessageType type, int client_id, long session_id, Endpoint src,
                        Endpoint dst, const unsigned char *data)
{
    MessageBase *m;

    LOG(LOG_DEBUG, "%s (id = %s[%d], type = %s).", __FUNCTION__, MessageStr(id, type), id, MessageTypeStr[type]);

    switch (type) {
    case MESSAGE_REQUEST:
        switch ((RequestID)id) {

        // VuC
        case REQ_DOORLOCKUNLOCK: {
            m = (MessageBase*)new Message<ReqDoorLockUnlock>(data);
            break;
        }
        case REQ_VUCTRACE_CONFIG: {
            m = (MessageBase*)new Message<ReqVucTraceConfig>(data);
            break;
        }
        case REQ_VUCTRACE_CONTROL: {
            m = (MessageBase*)new Message<ReqVucTraceControl>(data);
            break;
        }
        case REQ_HORNNLIGHT: {
            m = (MessageBase*)new Message<ReqHornNLight>(data);
            break;
        }
        case REQ_SWITCHTOBOOT: {
            m = (MessageBase*)new Message<ReqSwitchToBoot>(data);
            break;
        }
        case REQ_ANTENNASWITCH: {
            m = (MessageBase*)new Message<ReqAntennaSwitch>(data);
            break;
        }
        case REQ_LANLINKTRIGGER: {
            m = (MessageBase*)new Message<ReqLanLinkTrigger>(data);
            break;
        }
        case REQ_GETCARCONFIG: {
            m = (MessageBase*)new Message<ReqGetCarConfig>(data);
            break;
        }
        case REQ_CARCONFIGFACTRESTORE: {
            m = (MessageBase*)new Message<ReqCarConfigFactRestore>(data);
            break;
        }
        case REQ_ADCVALUE: {
            m = (MessageBase*)new Message<ReqAdcValue>(data);
            break;
        }
        case REQ_BLE_DISCONNECT: {
            m = (MessageBase*)new Message<ReqBleDisconnect>(data);
            break;
        }
        case REQ_BLE_DISCONNECTALL: {
            m = (MessageBase*)new Message<ReqBleDisconnectAll>(data);
            break;
        }
        case REQ_BLE_GENERATEBDAK: {
            m = (MessageBase*)new Message<ReqBleGenBdak>(data);
            break;
        }
        case REQ_BLE_GETBDAK: {
            m = (MessageBase*)new Message<ReqBleGetBdak>(data);
            break;
        }
        case REQ_BLE_CONFIG_DATACOMSERVICE: {
            m = (MessageBase*)new Message<ReqBleCfgDataCommService>(data);
            break;
        }
        case REQ_BLE_CONFIG_IBEACON: {
            m = (MessageBase*)new Message<ReqBleCfgIbeacon>(data);
            break;
        }
        case REQ_BLE_CONFIG_GENERICACCESS: {
            m = (MessageBase*)new Message<ReqBleCfgGenAcc>(data);
            break;
        }
        case REQ_BLE_STARTPARINGSERVICE: {
            m = (MessageBase*)new Message<ReqBleStartPairService>(data);
            break;
        }
        case REQ_BLE_STOPPARINGSERVICE: {
            m = (MessageBase*)new Message<ReqBleStopPairService>(data);
            break;
        }
        case REQ_BLE_WAKEUP_RESPONSE: {
            m = (MessageBase*)new Message<ReqBleWakeupResponse>(data);
            break;
        }
        case REQ_BLE_DATAACCESSAUTHCONFIRMED: {
            m = (MessageBase*)new Message<ReqBleDataAccAuthCon>(data);
            break;
        }
        case REQ_BLE_STARTADVERTISING: {
            m = (MessageBase*)new Message<ReqBleStartAdvertising>(data);
            break;
        }
        case REQ_BLE_STOPADVERTISING: {
            m = (MessageBase*)new Message<ReqBleStopAdvertising>(data);
            break;
        }
        case REQ_DTCENTRIES: {
            m = (MessageBase*)new Message<ReqDtcEntries>(data);
            break;
        }
        case REQ_DTCCTLSETTINGS: {
            m = (MessageBase*)new Message<ReqDtcCtlSettings>(data);
            break;
        }
        case REQ_IPWAKEUP: {
            m = (MessageBase*)new Message<ReqIpWakeup>(data);
            break;
        }
        case REQ_SETOHCLED: {
            m = (MessageBase*)new Message<ReqSetOhcLed>(data);
            break;
        }
        case REQ_VINNUMBER: {
            m = (MessageBase*)new Message<ReqVinNumber>(data);
            break;
        }
        case REQ_GETVUCBLESWPARTNUMBER: {
            m = (MessageBase*)new Message<ReqGetVucBleSwPartNumber>(data);
            break;
        }
        case REQ_GETBELTINFORMATION: {
            m = (MessageBase*)new Message<ReqGetBeltInformation>(data);
            break;
        }
        case REQ_GETDIAGERRREPORT: {
            m = (MessageBase*)new Message<ReqGetDiagErrReport>(data);
            break;
        }
        case REQ_GETVEHICLESTATE: {
            m = (MessageBase*)new Message<ReqGetVehicleState>(data);
            break;
        }
        case REQ_GETDIDGLOBALSNAPSHOTDATA: {
            m = (MessageBase*)new Message<ReqGetDIDGlobalSnapshotData>(data);
            break;
        }
        case REQ_DTCSDETECTED: {
            m = (MessageBase*)new Message<ReqDTCsDetected>(data);
            break;
        }
        case REQ_PSIMDATA: {
            m = (MessageBase*)new Message<ReqPSIMData>(data);
            break;
        }
        case REQ_THERMALMITIGATION: {
            m = (MessageBase*)new Message<ReqThermalMitigataion>(data);
            break;
        }
        case REQ_CARCFGPARAMETERFAULT: {
            m = (MessageBase*)new Message<ReqCarCfgParameterFault>(data);
            break;
        }
        case REQ_NETWORK_MANAGEMENT_HISTORY: {
            m = (MessageBase*)new Message<ReqNetworkManageHistory>(data);
            break;
        }
        case REQ_PROGRAM_PRECOND_CHK: {
            m = (MessageBase*)new Message<ReqProgramPrecond_Chk>(data);
            break;
        }
#if 1 // nieyj
        case REQ_WINCTRL: {
            m = (MessageBase*)new Message<ReqWinCtrl>(data);
            break;
        }
        case REQ_ROOFCTRL: {
            m = (MessageBase*)new Message<ReqRoofCurtCtrl>(data);
            break;
        }
        case REQ_WINVENTI: {
            m = (MessageBase*)new Message<ReqWinVentilation>(data);
            break;
        }
        case REQ_PM25ENQUIRE: {
            m = (MessageBase*)new Message<ReqPM25Enquire>(data);
            break;
        }
        case REQ_VFCACTIVATE: {
            m = (MessageBase*)new Message<ReqVFCActivate>(data);
            break;
        }
#endif
        //uia93888
        case REQ_RMTENGINE:{
            m = (MessageBase*)new Message<Req_RMTEngine>(data);
            break;
        }
        case REQ_RMTENGINESECURITYRESPONSE:{
            m = (MessageBase*)new Message<Req_RMTEngineSecurityResponse>(data);
            break;
        }
        case REQ_RTCAWAKEVUC:
        {
            m = (MessageBase*)new Message<Req_RTCAwakeVuc>(data);
            break;
        }
        case REQ_GETRMTSTATUS:{
            m = (MessageBase*)new Message<Req_GetRMTStatus>(data);
            break;
        }
        case REQ_GETENGINESTATES:{
            m = (MessageBase*)new Message<Req_GetEngineStates>(data);
            break;
        }
        case REQ_DELAYENGINERUNNINGTIME:
        {
            m = (MessageBase*)new Message<Req_DelayEngineRunngTime>(data);
            break;
        }
        case REQ_OPERATEREMOTECLIMATE:{
            m = (MessageBase*)new Message<Req_OperateRMTClimate>(data);
            break;
        }
        case REQ_OPERATERMTSEATHEAT:{
            m = (MessageBase*)new Message<Req_OperateRMTSeatHeat>(data);
            break;
        }
        case REQ_OPERATERMTSEATVENTILATION:{
            m = (MessageBase*)new Message<Req_OperateRMTSeatVenti>(data);
            break;
        }
        case REQ_OPERATERMTSTEERWHLHEAT:{
            m = (MessageBase*)new Message<Req_OperateRMTSteerWhlHeat_S>(data);
            break;
        }
        case REQ_PARKINGCLIMATEOPER:{
            m = (MessageBase*)new Message<Req_ParkingClimateOper_S>(data);
            break;
        }
        //end uia93888
        case REQ_ROLLOVERVALUE: {
            m = (MessageBase*)new Message<ReqRolloverValue>(data);
            break;
        }
        case REQ_SETROLLOVERCFG: {
            m = (MessageBase*)new Message<ReqSetRolloverCfg>(data);
            break;
        }
        // VGM
        case REQ_SENDPOSNFROMSATLTCON: {
            m = (MessageBase*)new Message<ReqSendPosnFromSatltCon>(data);
            break;
        }

        // IHU

        // Data-storage
        case REQ_GETDSPARAMETER: {
            m = (MessageBase*)new Message<ReqGetDSParameter>(data);
            break;
        }

        // Message-processor
        case REQ_LASTBUBSTATE: {
            m = (MessageBase*)new Message<ReqLastBubState>(data);
            break;
        }
        case REQ_CARUSAGEMODE: {
            m = (MessageBase*)new Message<ReqCarUsageMode>(data);
            break;
        }
        case REQ_CARMODE: {
            m = (MessageBase*)new Message<ReqCarMode>(data);
            break;
        }
        case REQ_ELECTENGLVL: {
            m = (MessageBase*)new Message<ReqElectEngLvl>(data);
            break;
        }
        case REQ_GETVUCPOWERMODE: {
            m = (MessageBase*)new Message<ReqGetVucPowerMode>(data);
            break;
        }
        case REQ_GETVUCWAKEUPREASON: {
            m = (MessageBase*)new Message<ReqGetVucWakeupReason>(data);
            break;
        }
        case REQ_GETWIFIDATA: {
            m = (MessageBase*)new Message<ReqGetWifiData>(data);
            break;
        }
        case REQ_GETPSIMAVAILABILITY: {
            m = (MessageBase*)new Message<ReqGetPSIMAvailability>(data);
            break;
        }
        case REQ_GETFUELTYPE: {
            m = (MessageBase*)new Message<ReqGetFuelType>(data);
            break;
        }
        case REQ_GETPROPULSIONTYPE: {
            m = (MessageBase*)new Message<ReqGetPropulsionType>(data);
            break;
        }
        case REQ_GETOHCBTNSTATE: {
            m = (MessageBase*)new Message<ReqGetOhcBtnState>(data);
            break;
        }
        case REQ_GETNUMBEROFDOORS: {
            m = (MessageBase*)new Message<ReqGetNumberOfDoors>(data);
            break;
        }
        case REQ_GETSTEERINGWHEELPOSITION: {
            m = (MessageBase*)new Message<ReqGetSteeringWheelPosition>(data);
            break;
        }
        case REQ_GETFUELTANKVOLUME: {
            m = (MessageBase*)new Message<ReqGetFuelTankVolume>(data);
            break;
        }
        case REQ_GETASSISTANCESERVICES: {
            m = (MessageBase*)new Message<ReqGetAssistanceServices>(data);
            break;
        }
        case REQ_GETAUTONOMOUSDRIVE: {
            m = (MessageBase*)new Message<ReqGetAutonomousDrive>(data);
            break;
        }
        case REQ_GETCOMBINEDINSTRUMENT: {
            m = (MessageBase*)new Message<ReqGetCombinedInstrument>(data);
            break;
        }
        case REQ_GETTELEMATICMODULE: {
            m = (MessageBase*)new Message<ReqGetTelematicModule>(data);
            break;
        }
        case REQ_GETCONNECTIVITY: {
            m = (MessageBase*)new Message<ReqGetConnectivity>(data);
            break;
        }
        case REQ_GETCONNECTEDSERVICEBOOKING: {
            m = (MessageBase*)new Message<ReqGetConnectedServiceBooking>(data);
            break;
        }
        case REQ_GETCARCFGTHEFTNOTIFICATION: {
            m = (MessageBase*)new Message<ReqGetTheftNotification>(data);
            break;
        }
        case REQ_GETCARCFGSTOLENVEHICLETRACKING: {
            m = (MessageBase*)new Message<ReqGetStolenVehicleTracking>(data);
            break;
        }
        case REQ_GETGNSSRECEIVER: {
            m = (MessageBase*)new Message<ReqGetGNSS>(data);
            break;
        }
        case REQ_GETAPPROVEDCELLULARSTANDARDS: {
            m = (MessageBase*)new Message<ReqGetApprovedCellularStandards>(data);
            break;
        }
        case REQ_GETDIGITALVIDEORECORDER: {
            m = (MessageBase*)new Message<ReqGetDigitalVideoRecorder>(data);
            break;
        }
        case REQ_GETDRIVERALERTCONTROL: {
            m = (MessageBase*)new Message<ReqGetDriverAlertControl>(data);
            break;
        }
        case REQ_GETDOORSTATE: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }
        // Multi
        case REQ_BLESWVERSION: {
            m = (MessageBase*)new Message<ReqBleSwVersion>(data);
            break;
        }
        case REQ_VUCSWVERSION: {
            m = (MessageBase*)new Message<ReqVucSwVersion>(data);
            break;
        }
        case REQ_GETBLESWBUILD: {
            m = (MessageBase*)new Message<ReqGetBleSwBuild>(data);
            break;
        }
        case REQ_GETVUCSWBUILD: {
            m = (MessageBase*)new Message<ReqGetVucSwBuild>(data);
            break;
        }
        case REQ_GETFBLSWBUILD: {
            m = (MessageBase*)new Message<ReqGetFblSwBuild>(data);
            break;
        }
        case REQ_GETMSGCATVERSION: {
            m = (MessageBase*)new Message<ReqGetMsgCatVer>(data);
            break;
        }
        case REQ_GETTCAMHWVERSION: {
            m = (MessageBase*)new Message<ReqGetTcamHwVer>(data);
            break;
        }
        case REQ_GETRFVERSION: {
            m = (MessageBase*)new Message<ReqGetRfVersion>(data);
            break;
        }
        // Test
        case REQ_VGMTESTMSG: {
            m = (MessageBase*)new Message<ReqVGMTestMsg>(data);
            break;
        }
        case REQ_IHUTESTMSG: {
            m = (MessageBase*)new Message<ReqIHUTestMsg>(data);
            break;
        }
        case REQ_TEST_TOGGLEUSAGEMODE: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }
        case REQ_TEST_TOGGLECARMODE: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }
        case REQ_TEST_CRASHSTATE: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }
        case REQ_TEST_OHCSTATE: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }
        case REQ_TEST_SETFAKEVALUESDEFAULT: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }
        case REQ_TEST_SETFAKEVALUEINT: {
            m = (MessageBase*)new Message<ReqTestSetFakeValueInt>(data);
            break;
        }
        case REQ_TEST_SETFAKEVALUESTR: {
            m = (MessageBase*)new Message<ReqTestSetFakeValueStr>(data);
            break;
        }
        case REQ_TEST_SETLOCALCONFIG: {
            m = (MessageBase*)new Message<ReqTestSetLocalConfig>(data);
            break;
        }
        default:
            LOG(LOG_WARN, "Common: %s: Unknown request-ID (%d)!", __FUNCTION__, id);
            return NULL;
        }

        break;
    case MESSAGE_RESPONSE:
        switch ((ResponseID)id) {
        case RES_REQUESTSENT: {
            m = (MessageBase*)new Message<Empty>;
            break;
        }

        // VuC
        case RES_HORNNLIGHT: {
            m = (MessageBase*)new Message<ResHornNLight>(data);
            break;
        }
        case RES_ANTENNASWITCH: {
            m = (MessageBase*)new Message<ResAntennaSwitch>(data);
            break;
        }
        case RES_VUCTRACE_CONFIG: {
            m = (MessageBase*)new Message<ResVucTraceConfig>(data);
            break;
        }
        case RES_VUCTRACE_CONTROL: {
            m = (MessageBase*)new Message<ResVucTraceControl>(data);
            break;
        }
        case RES_GETCARCONFIG: {
            m = (MessageBase*)new Message<ResGetCarConfig>(data);
            break;
        }
        case RES_CARCONFIGFACTRESTORE: {
            m = (MessageBase*)new Message<ResCarConfigFactRestore>(data);
            break;
        }
        case RES_ADCVALUE: {
            m = (MessageBase*)new Message<ResAdcValue>(data);
            break;
        }
        case RES_BLE_DISCONNECT: {
            m = (MessageBase*)new Message<ResBleDisconnect>(data);
            break;
        }
        case RES_BLE_DISCONNECTALL: {
            m = (MessageBase*)new Message<ResBleDisconnectAll>(data);
            break;
        }
        case RES_BLE_GENERATEBDAK: {
            m = (MessageBase*)new Message<ResBleGenBdak>(data);
            break;
        }
        case RES_BLE_GETBDAK: {
            m = (MessageBase*)new Message<ResBleGetBdak>(data);
            break;
        }
        case RES_BLE_CONFIG_DATACOMSERVICE: {
            m = (MessageBase*)new Message<ResBleCfgDataCommService>(data);
            break;
        }
        case RES_BLE_CONFIG_IBEACON: {
            m = (MessageBase*)new Message<ResBleCfgIbeacon>(data);
            break;
        }
        case RES_BLE_CONFIG_GENERICACCESS: {
            m = (MessageBase*)new Message<ResBleCfgGenAcc>(data);
            break;
        }
        case RES_BLE_STARTPARINGSERVICE: {
            m = (MessageBase*)new Message<ResBleStartPairService>(data);
            break;
        }
        case RES_BLE_STOPPARINGSERVICE: {
            m = (MessageBase*)new Message<ResBleStopPairService>(data);
            break;
        }
        case RES_BLE_STARTADVERTISING: {
            m = (MessageBase*)new Message<ResBleStartAdvertising>(data);
            break;
        }
        case RES_BLE_STOPADVERTISING: {
            m = (MessageBase*)new Message<ResBleStopAdvertising>(data);
            break;
        }
        case RES_DTCCTLSETTINGS: {
            m = (MessageBase*)new Message<ResDtcCtlSettings>(data);
            break;
        }
        case RES_GETVUCBLESWPARTNUMBER: {
            m = (MessageBase*)new Message<ResGetVucBleSwPartNumber>(data);
            break;
        }
        case RES_GETBELTINFORMATION: {
            m = (MessageBase*)new Message<ResGetBeltInformation>(data);
            break;
        }
        case RES_GETDIAGERRREPORT: {
            m = (MessageBase*)new Message<ResGetDiagErrReport>(data);
            break;
        }
        case RES_GETVEHICLESTATE: {
            m = (MessageBase*)new Message<ResGetVehicleState>(data);
            break;
        }
        case RES_GETDIDGLOBALSNAPSHOTDATA: {
            m = (MessageBase*)new Message<ResGetDIDGlobalSnapshotData>(data);
            break;
        }
        case RES_DTCSDETECTED: {
            m = (MessageBase*)new Message<ResDTCsDetected>(data);
            break;
        }
        case RES_PSIMDATA: {
            m = (MessageBase*)new Message<ResPSIMData>(data);
            break;
        }
        case RES_THERMALMITIGATION: {
            m = (MessageBase*)new Message<ResThermalMitigation>(data);
            break;
        }
        case RES_CARCFGPARAMETERFAULT: {
            m = (MessageBase*)new Message<ResCarCfgParameterFault>(data);
            break;
        }
        case RES_NETWORK_MANAGEMENT_HISTORY: {
            m = (MessageBase*)new Message<ResNetworkManageHistory>(data);
            break;
        }
        case RES_PROGRAM_PRECOND_CHK: {
            m = (MessageBase*)new Message<ResProgramPrecond_Chk>(data);
            break;
        }
#if 1 // nieyj
        case RES_VFCACTIVATE: {
            m = (MessageBase*)new Message<ResVFCActivate>(data);
            break;
        }

#endif
        //uia93888 remote start engine
        case RES_RMTENGINESECURITYRANDOM:{
            m = (MessageBase*)new Message<Res_RMTEngineSecurityRandom>(data);
            break;
        }
        case RES_RMTENGINESECURITYRESULT:{
            m = (MessageBase*)new Message<Res_RMTEngineSecurityResult>(data);
            break;
        }
        case RES_RMTENGINE:{
            m = (MessageBase*)new Message<Res_RMTEngine>(data);
            break;
        }
        case RES_RMTSTATUS:{
            m = (MessageBase*)new Message<ResRMTStatus>(data);
            break;
        }
        case RES_ENGINESTATES:{
            m = (MessageBase*)new Message<Res_RMTEngine>(data);
            break;
        }
        case RES_DELAYENGINERUNNINGTIME:{
            m = (MessageBase*)new Message<Res_DelayEngRunngTime>(data);
            break;
        }
        case RES_OPERATEREMOTECLIMATE:{
            m = (MessageBase*)new Message<Res_OperateRMTClimate>(data);
            break;
        }
        // end uia93888
        case RES_ROLLOVERVALUE:{
            m = (MessageBase*)new Message<ResRolloverValue>(data);
            break;
        }
        case RES_SETROLLOVERCFG:{
            m = (MessageBase*)new Message<ResSetRolloverCfg>(data);
            break;
        }
        // VGM

        // IHU

        // Data-storage
        case RES_GETDSPARAMETER_INT: {
            m = (MessageBase*)new Message<ResGetDSParameterInt>(data);
            break;
        }
        case RES_GETDSPARAMETER_STR: {
            m = (MessageBase*)new Message<ResGetDSParameterStr>(data);
            break;
        }
        case RES_GETDSPARAMETER_VECT: {
            m = (MessageBase*)new Message<ResGetDSParameterVect>(data);
            break;
        }
        case RES_CARMODE: {
            m = (MessageBase*)new Message<ResCarMode>(data);
            break;
        }
        case RES_CARUSAGEMODE: {
            m = (MessageBase*)new Message<ResCarUsageMode>(data);
            break;
        }
        case RES_VINNUMBER: {
            m = (MessageBase*)new Message<ResVINNumber>(data);
            break;
        }
        case RES_ELECTENGLVL: {
            m = (MessageBase*)new Message<ResElectEngLvl>(data);
            break;
        }
        case RES_GETVUCPOWERMODE: {
            m = (MessageBase*)new Message<ResGetVucPowerMode>(data);
            break;
        }
        case RES_GETVUCWAKEUPREASON: {
            m = (MessageBase*)new Message<ResGetVucWakeupReason>(data);
            break;
        }

        // Message-processor
        case RES_VUCSWVERSION: {
            m = (MessageBase*)new Message<ResVucSwVersion>(data);
            break;
        }
        case RES_BLESWVERSION: {
            m = (MessageBase*)new Message<ResBleSwVersion>(data);
            break;
        }
        case RES_LASTBUBSTATE: {
            m = (MessageBase*)new Message<ResLastBubState>(data);
            break;
        }
        case RES_GETWIFIDATA: {
            m = (MessageBase*)new Message<ResGetWifiData>(data);
            break;
        }
        case RES_GETPSIMAVAILABILITY: {
            m = (MessageBase*)new Message<ResGetPSIMAvailability>(data);
            break;
        }
        case RES_GETFUELTYPE: {
            m = (MessageBase*)new Message<ResGetFuelType>(data);
            break;
        }
        case RES_GETPROPULSIONTYPE: {
            m = (MessageBase*)new Message<ResGetPropulsionType>(data);
            break;
        }
        case RES_GETOHCBTNSTATE: {
            m = (MessageBase*)new Message<ResGetOhcBtnState>(data);
            break;
        }
        case RES_GETNUMBEROFDOORS: {
            m = (MessageBase*)new Message<ResGetNumberOfDoors>(data);
            break;
        }
        case RES_GETSTEERINGWHEELPOSITION: {
            m = (MessageBase*)new Message<ResGetSteeringWheelPosition>(data);
            break;
        }
        case RES_GETFUELTANKVOLUME: {
            m = (MessageBase*)new Message<ResGetFuelTankVolume>(data);
            break;
        }
        case RES_GETASSISTANCESERVICES: {
            m = (MessageBase*)new Message<ResGetAssistanceServices>(data);
            break;
        }
        case RES_GETAUTONOMOUSDRIVE: {
            m = (MessageBase*)new Message<ResGetAutonomousDrive>(data);
            break;
        }
        case RES_GETCOMBINEDINSTRUMENT: {
            m = (MessageBase*)new Message<ResGetCombinedInstrument>(data);
            break;
        }
        case RES_GETTELEMATICMODULE: {
            m = (MessageBase*)new Message<ResGetTelematicModule>(data);
            break;
        }
        case RES_GETCONNECTIVITY: {
            m = (MessageBase*)new Message<ResGetConnectivity>(data);
            break;
        }
        case RES_GETCONNECTEDSERVICEBOOKING: {
            m = (MessageBase*)new Message<ResGetConnectedServiceBooking>(data);
            break;
        }
        case RES_GETCARCFGTHEFTNOTIFICATION: {
            m = (MessageBase*)new Message<ResGetTheftNotification>(data);
            break;
        }
        case RES_GETCARCFGSTOLENVEHICLETRACKING: {
            m = (MessageBase*)new Message<ResGetStolenVehicleTracking>(data);
            break;
        }
        case RES_GETGNSSRECEIVER: {
            m = (MessageBase*)new Message<ResGetGnss>(data);
            break;
        }
        case RES_GETAPPROVEDCELLULARSTANDARDS: {
            m = (MessageBase*)new Message<ResGetApprovedCellularStandards>(data);
            break;
        }
        case RES_GETDIGITALVIDEORECORDER: {
            m = (MessageBase*)new Message<ResGetDigitalVideoRecorder>(data);
            break;
        }
        case RES_GETDRIVERALERTCONTROL: {
            m = (MessageBase*)new Message<ResGetDriverAlertControl>(data);
            break;
        }
        case RES_GETDOORSTATE: {
            m = (MessageBase*)new Message<ResGetDoorState>(data);
            break;
        }

        // Multi
        case RES_GETVUCSWBUILD: {
            m = (MessageBase*)new Message<ResGetVucSwBuild>(data);
            break;
        }
        case RES_GETBLESWBUILD: {
            m = (MessageBase*)new Message<ResGetBleSwBuild>(data);
            break;
        }
        case RES_GETFBLSWBUILD: {
            m = (MessageBase*)new Message<ResGetFblSwBuild>(data);
            break;
        }
        case RES_GETMSGCATVERSION: {
            m = (MessageBase*)new Message<ResGetMsgCatVer>(data);
            break;
        }
        case RES_GETTCAMHWVERSION: {
            m = (MessageBase*)new Message<ResGetTcamHwVer>(data);
            break;
        }
        case RES_GETRFVERSION: {
            m = (MessageBase*)new Message<ResGetRfVersion>(data);
            break;
        }
        // Test
        case RES_VGMTESTMSG: {
            m = (MessageBase*)new Message<ResVGMTestMsg>(data);
            break;
        }
        case RES_IHUTESTMSG: {
            m = (MessageBase*)new Message<ResIHUTestMsg>(data);
            break;
        }

        default:
            LOG(LOG_WARN, "Common: %s: Unknown response-ID (%d)!", __FUNCTION__, id);
            return NULL;
        }

        break;
    case MESSAGE_EVENT:
        switch ((EventID)id) {

        // VuC
        case EVENT_HORNNLIGHTSTATE: {
            m = (MessageBase*)new Message<EventHornNLightState>(data);
            break;
        }
        case EVENT_CRASHSTATE: {
            m = (MessageBase*)new Message<EventCrashState>(data);
            break;
        }
        case EVENT_BLE_CONNECTIONSTATUS: {
            m = (MessageBase*)new Message<EventBleConnStatus>(data);
            break;
        }
        case EVENT_BLE_RESET_NOTIFICATION: {
            m = (MessageBase*)new Message<EventBleResetNotification>(data);
            break;
        }
        case EVENT_BLE_MTUSIZE: {
            m = (MessageBase*)new Message<EventBleMtuSize>(data);
            break;
        }
        case EVENT_DTCFULLREPORT: {
            m = (MessageBase*)new Message<EventDtcFullReport>(data);
            break;
        }
        case EVENT_DTCSTATUSREPORT: {
            m = (MessageBase*)new Message<EventDtcStatusReport>(data);
            break;
        }
        case EVENT_AMPLIFIERSTATE: {
            m = (MessageBase*)new Message<EventAmplifierState>(data);
            break;
        }
        case EVENT_RFKLINEERRREPORT: {
            m = (MessageBase*)new Message<EventRfKlineErrReport>(data);
            break;
        }
        case EVENT_LASTBUBSTATE: {
            m = (MessageBase*)new Message<EventLastBubState>(data);
            break;
        }
        case EVENT_OHCBTNSTATE: {
            m = (MessageBase*)new Message<EventOhcBtnState>(data);
            break;
        }
        case EVENT_VEHICLESPEEDSTATE: {
            m = (MessageBase*)new Message<EventVehicleSpeedState>(data);
            break;
        }
        case EVENT_CANVOLTAGEERRREPORT: {
            m = (MessageBase*)new Message<EventCanVoltageErrReport>(data);
            break;
        }
        case EVENT_DIAGCOUPLEREPORT: {
            m = (MessageBase*)new Message<EventDiagCoupleReport>(data);
            break;
        }
        case EVENT_NUMBEROFDOORS: {
            m = (MessageBase*)new Message<EventNumberOfDoors>(data);
            break;
        }
        case EVENT_FUELTYPE: {
            m = (MessageBase*)new Message<EventFuelType>(data);
            break;
        }
        case EVENT_STEERINGWHEELPOSITION: {
            m = (MessageBase*)new Message<EventSteeringWheelPosition>(data);
            break;
        }
        case EVENT_FUELTANKVOLUME: {
            m = (MessageBase*)new Message<EventFuelTankVolume>(data);
            break;
        }
        case EVENT_PROPULSIONTYPE: {
            m = (MessageBase*)new Message<EventPropulsionType>(data);
            break;
        }
        case EVENT_ASSISTANCESERVICES: {
            m = (MessageBase*)new Message<EventAssistanceServices>(data);
            break;
        }
        case EVENT_AUTONOMOUSDRIVE: {
            m = (MessageBase*)new Message<EventAutonomousDrive>(data);
            break;
        }
        case EVENT_COMBINEDINSTRUMENT: {
            m = (MessageBase*)new Message<EventCombinedInstrument>(data);
            break;
        }
        case EVENT_TELEMATICMODULE: {
            m = (MessageBase*)new Message<EventTelematicModule>(data);
            break;
        }
        case EVENT_CONNECTIVITY: {
            m = (MessageBase*)new Message<EventConnectivity>(data);
            break;
        }
        case EVENT_CONNECTEDSERVICEBOOKING: {
            m = (MessageBase*)new Message<EventConnectedServiceBooking>(data);
            break;
        }
        case EVENT_PSIMAVAILABILITY: {
            m = (MessageBase*)new Message<EventPSIMAvailability>(data);
            break;
        }
        case EVENT_WIFIFUNCTIONALITYCONTROL: {
            m = (MessageBase*)new Message<EventWifiFunctionalityControl>(data);
            break;
        }
        case EVENT_WIFIFREQUENCYBAND: {
            m = (MessageBase*)new Message<EventWifiFrequencyBand>(data);
            break;
        }
        case EVENT_WIFIWORKSHOPMODE: {
            m = (MessageBase*)new Message<EventWifiWorkshopMode>(data);
            break;
        }
        case EVENT_APPROVEDCELLULARSTANDARDS: {
            m = (MessageBase*)new Message<EventApprovedCellularStandards>(data);
            break;
        }
        case EVENT_DIGITALVIDEORECORDER: {
            m = (MessageBase*)new Message<EventDigitalVideoRecorder>(data);
            break;
        }
        case EVENT_DRIVERALERTCONTROL: {
            m = (MessageBase*)new Message<EventDriverAlertControl>(data);
            break;
        }
        // VGM
        case EVENT_POSNFROMSATLTCON: {
            m = (MessageBase*)new Message<EventPosnFromSatltCon>(data);
            break;
        }

        // Data-storage
        case EVENT_DSPARAMETER_INT: {
            m = (MessageBase*)new Message<EventDSParameterInt>(data);
            break;
        }
        case EVENT_DSPARAMETER_STR: {
            m = (MessageBase*)new Message<EventDSParameterStr>(data);
            break;
        }
        case EVENT_DSPARAMETER_VECT: {
            m = (MessageBase*)new Message<EventDSParameterVect>(data);
            break;
        }
        case EVENT_CARMODE: {
            m = (MessageBase*)new Message<EventCarMode>(data);
            break;
        }
        case EVENT_CARUSAGEMODE: {
            m = (MessageBase*)new Message<EventCarUsageMode>(data);
            break;
        }
        case EVENT_DOORLOCKSTATE: {
            m = (MessageBase*)new Message<EventDoorLockState>(data);
            break;
        }
#if 1 // nieyj
        case EVENT_WINOPENSTATE: {
            m = (MessageBase*)new Message<EventWinOpenState>(data);
            break;
        }
        case EVENT_PMLVL: {
            m = (MessageBase*)new Message<EventPMLevelState>(data);
            break;
        }

#endif
        //uia93888 remote start engine
        case EVENT_RMTENGINESECURITYRANDOM:{
            m = (MessageBase*)new Message<Event_RMTEngineSecurityRandom>(data);
            break;
        }
        case EVENT_RMTENGINESECURITYRESULT:{
            m = (MessageBase*)new Message<Event_RMTEngineSecurityResult>(data);
            break;
        }
        case EVENT_RMTENGINE:{
            m = (MessageBase*)new Message<Event_RMTEngine>(data);
            break;
        }
        case EVENT_DELAYENGINERUNNINGTIME:{
            m = (MessageBase*)new Message<Event_DelayEngRunngTime>(data);
            break;
        }
        case EVENT_OPERATEREMOTECLIMATE:{
            m = (MessageBase*)new Message<Event_OperateRMTClimate>(data);
            break;
        }
        case EVENT_OPERATERMTSEATHEAT:{
            m = (MessageBase*)new Message<Event_OperateRMTSeatHeat_S>(data);
            break;
        }
        case EVENT_OPERATERMTSEATVENTILATION:{
            m = (MessageBase*)new Message<Event_OperateRMTSeatVenti_S>(data);
            break;
        }
        case EVENT_OPERATERMTSTEERWHLHEAT:{
            m = (MessageBase*)new Message<Event_OperateRMTSteerWhlHeat_S>(data);
            break;
        }
        case EVENT_PARKINGCLIMATEOPER:{
            m = (MessageBase*)new Message<Event_ParkingClimateOper_S>(data);
            break;
        }
        //end uia93888
        case EVENT_DIAGERRREPORT: {
            m = (MessageBase*)new Message<EventDiagErrReport>(data);
            break;
        }
        case EVENT_ELECTENGLVL: {
            m = (MessageBase*)new Message<EventElectEngLvl>(data);
            break;
        }
        case EVENT_ENDOFTRIP: {
            m = (MessageBase*)new Message<EventEndOfTrip>(data);
            break;
        }
        case EVENT_VUCPOWERMODE: {
            m = (MessageBase*)new Message<EventVucPowerMode>(data);
            break;
        }
        case EVENT_VUCWAKEUPREASON: {
            m = (MessageBase*)new Message<EventVucWakeupReason>(data);
            break;
        }
        case EVENT_VINNUMBER: {
            m = (MessageBase*)new Message<EventVINNumber>(data);
            break;
        }

        // Test
        case EVENT_TESTFROMVGM: {
            m = (MessageBase*)new Message<EventTestFromVGM>(data);
            break;
        }
        case EVENT_TESTFROMIHU: {
            m = (MessageBase*)new Message<EventTestFromIHU>(data);
            break;
        }
        case EVENT_TESTFROMVC: {
            m = (MessageBase*)new Message<EventTestFromVC>(data);
            break;
        }

        default:
            LOG(LOG_WARN, "Common: %s: Unknown event-ID (%d)!", __FUNCTION__, id);
            return NULL;
        }

        break;
    case MESSAGE_ERROR:
        m = (MessageBase*)new Message<Empty>;
        break;
    default:
        LOG(LOG_WARN, "Common: %s: Unknown message-type (%d)!", __FUNCTION__, type);
        return NULL;
    }

    m->id_ = id;
    m->type_ = type;
    m->client_id_ = client_id;
    m->session_id_ = session_id;
    m->src_ = src;
    m->dst_ = dst;

    return m;
}

MessageBase *InitMessage(MessageID id, MessageType type, int client_id, long session_id, Endpoint src, const unsigned char *data)
{
    return InitMessage(id, type, client_id, session_id, src, ENDPOINT_UNKNOWN, data);
}

MessageBase *CopyMessage(MessageBase *msg)
{
    LOG(LOG_DEBUG, "%s (id = %d, type = %d).", __FUNCTION__, msg->id_, msg->type_);

    MessageBase* cpy_msg = InitMessage(msg->id_, msg->type_, msg->client_id_, msg->session_id_, msg->src_, msg->dst_, NULL);
    LOG(LOG_DEBUG, "%s.", __FUNCTION__);
    if (cpy_msg == NULL) {
        LOG(LOG_WARN, "%s: failed!.", __FUNCTION__);
        return NULL;
    }
    memcpy(cpy_msg->GetData(), msg->GetData(), cpy_msg->GetSize());
    return cpy_msg;
}

/**
    @brief Print the parameters of a Message.

    This function prints all parameters of a Message, including
    the parameters specific to the actual data of the message.

    @param[in]  id          The message-ID enum
    @param[in]  type        The message-type enum
    @param[in]  client_id   Identifier for a VehicleCommClient
    @param[in]  session_id  Identifier for a request/response pair
    @param[in]  src         The source node of the message
    @param[in]  data        Pointer to the data of the specific message
    @return     The created message
*/
ReturnValue PrintMessage(MessageBase *m)
{
    LOG(LOG_DEBUG, "%s.", __FUNCTION__);

    if (m == NULL) {
        LOG(LOG_WARN, "Common: %s: No message!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    LOG(LOG_INFO, "Common: %s: Message:", __FUNCTION__);
    LOG(LOG_INFO, "Common: %s:     ID = %s", __FUNCTION__, MessageStr(m->id_, m->type_));
    LOG(LOG_INFO, "Common: %s:     uniqueID = %d", __FUNCTION__, m->unique_id_);
    LOG(LOG_INFO, "Common: %s:     sessionID = %d", __FUNCTION__, m->session_id_);
    LOG(LOG_INFO, "Common: %s:     src = %s", __FUNCTION__, EndpointStr[m->src_]);
    LOG(LOG_INFO, "Common: %s:     dst = %s", __FUNCTION__, EndpointStr[m->dst_]);
    LOG(LOG_INFO, "Common: %s:     Data:", __FUNCTION__);

    switch (m->type_) {
    case MESSAGE_REQUEST:
        switch ((RequestID)m->id_) {
        // VuC
        case REQ_DOORLOCKUNLOCK: {
            Message<ReqDoorLockUnlock> *mm = (Message<ReqDoorLockUnlock>*)m;
            LOG(LOG_DEBUG, "Common: %s:         centralLockReq: %d", __FUNCTION__, mm->message.centralLockReq);
            break;
        }
        case REQ_VUCTRACE_CONFIG: {
            Message<ReqVucTraceConfig> *mm = (Message<ReqVucTraceConfig>*)m;
            LOG(LOG_DEBUG, "Common: %s:         bus: %d", __FUNCTION__, mm->message.bus);
            for (uint8_t i = 0; i < sizeof(mm->message.config)/sizeof(TraceModuleCfg); i++) {
                LOG(LOG_DEBUG, "Common: %s:         config[%d] -> module_id: %d  module_lvl: %d",
                        __FUNCTION__, i, mm->message.config[i].module_id, mm->message.config[i].module_lvl);
            }
            break;
        }
        case REQ_VUCTRACE_CONTROL: {
            Message<ReqVucTraceControl> *mm = (Message<ReqVucTraceControl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d", __FUNCTION__, mm->message.mode);
            LOG(LOG_DEBUG, "Common: %s:         interval: %d", __FUNCTION__, mm->message.interval);
            break;
        }
        case REQ_HORNNLIGHT: {
            Message<ReqHornNLight> *mm = (Message<ReqHornNLight>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d", __FUNCTION__, mm->message.mode);
            break;
        }
        case REQ_SWITCHTOBOOT:
            break;
        case REQ_ANTENNASWITCH: {
            Message<ReqAntennaSwitch> *mm = (Message<ReqAntennaSwitch>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d", __FUNCTION__, mm->message.mode);
            break;
        }
        case REQ_LANLINKTRIGGER: {
            Message<ReqLanLinkTrigger> *mm = (Message<ReqLanLinkTrigger>*)m;
            LOG(LOG_DEBUG, "Common: %s:         trigger: %d", __FUNCTION__, mm->message.trigger);
            LOG(LOG_DEBUG, "Common: %s:         session_state: %d", __FUNCTION__, mm->message.session_state);
            break;
        }
        case REQ_GETCARCONFIG:
            break;
        case REQ_CARCONFIGFACTRESTORE:
            break;
        case REQ_ADCVALUE: {
            Message<ReqAdcValue> *mm = (Message<ReqAdcValue>*)m;
            LOG(LOG_DEBUG, "Common: %s:         id: %d", __FUNCTION__, mm->message.id);
            break;
        }
        case REQ_BLE_DISCONNECT: {
            Message<ReqBleDisconnect> *mm = (Message<ReqBleDisconnect>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connection_id: %d", __FUNCTION__, mm->message.connection_id);
            break;
        }
        case REQ_BLE_DISCONNECTALL:
            break;
        case REQ_BLE_GENERATEBDAK:
            break;
        case REQ_BLE_GETBDAK:
            break;
        case REQ_BLE_CONFIG_DATACOMSERVICE: {
            Message<ReqBleCfgDataCommService> *mm = (Message<ReqBleCfgDataCommService>*)m;
            LOG(LOG_DEBUG, "Common: %s:         service_uuid: %02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX", __FUNCTION__,
                mm->message.service_uuid[0], mm->message.service_uuid[1], mm->message.service_uuid[2], mm->message.service_uuid[3],
                mm->message.service_uuid[4], mm->message.service_uuid[5], mm->message.service_uuid[6], mm->message.service_uuid[7],
                mm->message.service_uuid[8], mm->message.service_uuid[9], mm->message.service_uuid[10], mm->message.service_uuid[11],
                mm->message.service_uuid[12], mm->message.service_uuid[13], mm->message.service_uuid[14], mm->message.service_uuid[15]);
            break;
        }
        case REQ_BLE_CONFIG_IBEACON: {
            Message<ReqBleCfgIbeacon> *mm = (Message<ReqBleCfgIbeacon>*)m;
            LOG(LOG_DEBUG, "Common: %s:         proximity_uuid: %02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX", __FUNCTION__,
                mm->message.proximity_uuid[0], mm->message.proximity_uuid[1], mm->message.proximity_uuid[2], mm->message.proximity_uuid[3],
                mm->message.proximity_uuid[4], mm->message.proximity_uuid[5], mm->message.proximity_uuid[6], mm->message.proximity_uuid[7],
                mm->message.proximity_uuid[8], mm->message.proximity_uuid[9], mm->message.proximity_uuid[10], mm->message.proximity_uuid[11],
                mm->message.proximity_uuid[12], mm->message.proximity_uuid[13], mm->message.proximity_uuid[14], mm->message.proximity_uuid[15]);
            LOG(LOG_DEBUG, "Common: %s:         major: %d", __FUNCTION__, mm->message.major);
            LOG(LOG_DEBUG, "Common: %s:         minor: %d", __FUNCTION__, mm->message.minor);
            break;
        }
        case REQ_BLE_CONFIG_GENERICACCESS: {
            Message<ReqBleCfgGenAcc> *mm = (Message<ReqBleCfgGenAcc>*)m;
            LOG(LOG_DEBUG, "Common: %s:         id_string: %s", __FUNCTION__, mm->message.id_string);
            break;
        }
        case REQ_BLE_STARTPARINGSERVICE: {
            Message<ReqBleStartPairService> *mm = (Message<ReqBleStartPairService>*)m;
            LOG(LOG_DEBUG, "Common: %s:         service_uuid: %04X", __FUNCTION__, mm->message.service_uuid);
            LOG(LOG_DEBUG, "Common: %s:         name_char_string: %s", __FUNCTION__, mm->message.name_char_string);
            break;
        }
        case REQ_BLE_STOPPARINGSERVICE:
            break;
        case REQ_BLE_WAKEUP_RESPONSE:
            break;
        case REQ_BLE_DATAACCESSAUTHCONFIRMED: {
            Message<ReqBleDataAccAuthCon> *mm = (Message<ReqBleDataAccAuthCon>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connection_id: %d", __FUNCTION__, mm->message.connection_id);
            break;
        }
        case REQ_BLE_STARTADVERTISING: {
            Message<ReqBleStartAdvertising> *mm = (Message<ReqBleStartAdvertising>*)m;
            LOG(LOG_DEBUG, "Common: %s:         adv_frame: %d", __FUNCTION__, mm->message.adv_frame);
        }
        case REQ_BLE_STOPADVERTISING:
            break;
        case REQ_DTCENTRIES: {
            Message<ReqDtcEntries> *mm = (Message<ReqDtcEntries>*)m;
            LOG(LOG_DEBUG, "Common: %s:         request_id: %d", __FUNCTION__, mm->message.request_id);
            LOG(LOG_DEBUG, "Common: %s:         event_id: %d", __FUNCTION__, mm->message.event_id);
            break;
        }
        case REQ_DTCCTLSETTINGS: {
            Message<ReqDtcCtlSettings> *mm = (Message<ReqDtcCtlSettings>*)m;
            LOG(LOG_DEBUG, "Common: %s:         settings: %d", __FUNCTION__, mm->message.settings);
            break;
        }
        case REQ_IPWAKEUP: {
            Message<ReqIpWakeup> *mm = (Message<ReqIpWakeup>*)m;
            LOG(LOG_DEBUG, "Common: %s:         prio: %d", __FUNCTION__, mm->message.prio);
            LOG(LOG_DEBUG, "Common: %s:         res_group: %d", __FUNCTION__, mm->message.res_group);
            break;
        }
        case REQ_SETOHCLED: {
            Message<ReqSetOhcLed> *mm = (Message<ReqSetOhcLed>*)m;
            LOG(LOG_DEBUG, "Common: %s:         sos_red_status_blink (led 11): %d", __FUNCTION__, mm->message.sos_red_status_blink);
            LOG(LOG_DEBUG, "Common: %s:         sos_red_status_repeat (led 11): %d", __FUNCTION__, mm->message.sos_red_status_repeat);
            LOG(LOG_DEBUG, "Common: %s:         sos_red_duty (led 12): %d", __FUNCTION__, mm->message.sos_red_duty);
            LOG(LOG_DEBUG, "Common: %s:         sos_white_status_blink (led 21): %d", __FUNCTION__, mm->message.sos_white_status_blink);
            LOG(LOG_DEBUG, "Common: %s:         sos_white_status_repeat (led 21): %d", __FUNCTION__, mm->message.sos_white_status_repeat);
            LOG(LOG_DEBUG, "Common: %s:         sos_white_duty (led 22): %d", __FUNCTION__, mm->message.sos_white_duty);
            LOG(LOG_DEBUG, "Common: %s:         voc_status_blink (led 31): %d", __FUNCTION__, mm->message.voc_status_blink);
            LOG(LOG_DEBUG, "Common: %s:         voc_status_repeat (led 32): %d", __FUNCTION__, mm->message.voc_status_repeat);
            LOG(LOG_DEBUG, "Common: %s:         voc_duty: %d", __FUNCTION__, mm->message.voc_duty);
            LOG(LOG_DEBUG, "Common: %s:         brightness: %d", __FUNCTION__, mm->message.brightness);
            break;
        }
        case REQ_GETVUCBLESWPARTNUMBER:
            break;
        case REQ_GETBELTINFORMATION:
            break;
        case REQ_GETDIAGERRREPORT:
            break;
        case REQ_GETVEHICLESTATE:
            break;
        case REQ_GETDIDGLOBALSNAPSHOTDATA:
            break;
        case REQ_DTCSDETECTED: {
            Message<ReqDTCsDetected> *mm = (Message<ReqDTCsDetected>*)m;
            for (uint8_t i = 0; i < LENGTH(mm->message.status); i++) {
                LOG(LOG_DEBUG, "Common: %s:         data[%d]: 0x%02hhX", __FUNCTION__, i, mm->message.status[i]);
            }
        }
        case REQ_PSIMDATA: {
            Message<ReqPSIMData> *mm = (Message<ReqPSIMData>*)m;
#if 1 //nieyj fix klocwork 190273
            for (uint16_t i = 0; i < LENGTH(mm->message.data); i++) {
#else
            for (uint8_t i = 0; i < LENGTH(mm->message.data); i++) {
#endif
                LOG(LOG_DEBUG, "Common: %s:         data[%d]: 0x%02hhX", __FUNCTION__, i, mm->message.data[i]);
            }
            break;
        }
        case REQ_THERMALMITIGATION: {
            Message<ReqThermalMitigataion> *mm = (Message<ReqThermalMitigataion>*)m;
            LOG(LOG_DEBUG, "Common: %s:         nad_temperature: %d (C)", __FUNCTION__, mm->message.nad_temperature);
            break;
        }
        case REQ_CARCFGPARAMETERFAULT: {
            LOG(LOG_DEBUG, "ReqCarCfgParameterFault: ");
            break;
        }
        case REQ_NETWORK_MANAGEMENT_HISTORY: {
            LOG(LOG_DEBUG, "Reqnetworkmanagementhistory: ");
            break;
        }
        case REQ_PROGRAM_PRECOND_CHK: {
            LOG(LOG_DEBUG, "REQ_PROGRAM_PRECOND_CHK: ");
            break;
        }
#if 1 // nieyj
        case REQ_WINCTRL: {
            Message<ReqWinCtrl> *mm = (Message<ReqWinCtrl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d (C)", __FUNCTION__, mm->message.mode);
            break;
        }
        case REQ_ROOFCTRL: {
            Message<ReqRoofCurtCtrl> *mm = (Message<ReqRoofCurtCtrl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d (C)", __FUNCTION__, mm->message.mode);
            break;
        }
        case REQ_WINVENTI: {
            Message<ReqWinVentilation> *mm = (Message<ReqWinVentilation>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d (C)", __FUNCTION__, mm->message.mode);
            LOG(LOG_DEBUG, "Common: %s:         value: %d (C)", __FUNCTION__, mm->message.value);
            break;
        }
        case REQ_PM25ENQUIRE: {
            Message<ReqPM25Enquire> *mm = (Message<ReqPM25Enquire>*)m;
            LOG(LOG_DEBUG, "Common: %s:         req: %d (C)", __FUNCTION__, mm->message.req);
            break;
        }
        case REQ_VFCACTIVATE: {
            Message<ReqVFCActivate> *mm = (Message<ReqVFCActivate>*)m;
            LOG(LOG_DEBUG, "Common: %s:         id: %d (C)", __FUNCTION__, mm->message.id);
            LOG(LOG_DEBUG, "Common: %s:         type: %d (C)", __FUNCTION__, mm->message.type);
            break;
        }
#endif
        //uia93888 remote start engine
        case REQ_RMTENGINE:{
            Message<Req_RMTEngine> *mm = (Message<Req_RMTEngine>*)m;
            LOG(LOG_DEBUG, "Common: %s:  ersCmd: %d (C)", __FUNCTION__, mm->message.ersCmd);
            LOG(LOG_DEBUG, "Common: %s:  ersRunTime: %d (C)", __FUNCTION__, mm->message.ersRunTime);
            break;
        }
        case REQ_RTCAWAKEVUC:{
            Message<Req_RTCAwakeVuc> *mm = (Message<Req_RTCAwakeVuc>*)m;
            if(1 == mm->message.secondAlarm_enable){
                LOG(LOG_DEBUG, "Common: %s:  secondAlarm_unit: %d (C)", __FUNCTION__, mm->message.secondAlarm_unit);
                LOG(LOG_DEBUG, "Common: %s:  secondAlarm_ten: %d (C)", __FUNCTION__, mm->message.secondAlarm_ten);
            }
            if(1 == mm->message.minuteAlarm_enable){
                LOG(LOG_DEBUG, "Common: %s:  minuteAlarm_unit: %d (C)", __FUNCTION__, mm->message.minuteAlarm_unit);
                LOG(LOG_DEBUG, "Common: %s:  minuteAlarm_ten: %d (C)", __FUNCTION__, mm->message.minuteAlarm_ten);
            }
            if(1 == mm->message.hourAlarm_enable){
                LOG(LOG_DEBUG, "Common: %s:  hourAlarm_unit: %d (C)", __FUNCTION__, mm->message.hourAlarm_unit);
                LOG(LOG_DEBUG, "Common: %s:  hourAlarm_ten: %d (C)", __FUNCTION__, mm->message.hourAlarm_ten);
            }
            if(1 == mm->message.secondAlarm_enable){
                LOG(LOG_DEBUG, "Common: %s:  dayAlarm_unit: %d (C)", __FUNCTION__, mm->message.dayAlarm_unit);
                LOG(LOG_DEBUG, "Common: %s:  dayAlarm_ten: %d (C)", __FUNCTION__, mm->message.dayAlarm_ten);
            }
            break;
        }
        case REQ_DELAYENGINERUNNINGTIME:
        {
            Message<Req_DelayEngineRunngTime> *mm = (Message<Req_DelayEngineRunngTime>*)m;
            LOG(LOG_DEBUG, "Common: %s:  delayRunngTime: %d (C)", __FUNCTION__, mm->message.telmEngDelayTi);
            break;
        }
        case REQ_OPERATEREMOTECLIMATE:{
            Message<Req_OperateRMTClimate> *mm = (Message<Req_OperateRMTClimate>*)m;
            LOG(LOG_DEBUG, "Common: %s:  telmClimaReq: %d (C)", __FUNCTION__, mm->message.telmClimaReq);
            LOG(LOG_DEBUG, "Common: %s:  telmClimaTSetTempRange: %d (C)", __FUNCTION__, mm->message.telmClimaTSetTempRange);
            LOG(LOG_DEBUG, "Common: %s:  hmiCmptmtTSpSpcl: %d (C)", __FUNCTION__, mm->message.hmiCmptmtTSpSpcl);
            break;
        }
        case REQ_OPERATERMTSEATHEAT:{
            LOG(LOG_DEBUG, "Common: %s: REQ_OPERATERMTSEATHEAT", __FUNCTION__);
            Message<Req_OperateRMTSeatHeat> *mm = (Message<Req_OperateRMTSeatHeat>*)m;
            LOG(LOG_DEBUG, "Common: %s:  telmSeatDrvHeat: %d (C)", __FUNCTION__, mm->message.telmSeatDrvHeat);
            LOG(LOG_DEBUG, "Common: %s:  telmSeatPassHeat: %d (C)", __FUNCTION__, mm->message.telmSeatPassHeat);
            LOG(LOG_DEBUG, "Common: %s:  telmSeatSecLeHeat: %d (C)", __FUNCTION__, mm->message.telmSeatSecLeHeat);
            LOG(LOG_DEBUG, "Common: %s:  telmSeatSecRiHeat: %d (C)", __FUNCTION__, mm->message.telmSeatSecRiHeat);
            break;
        }
        case REQ_OPERATERMTSEATVENTILATION:{
            LOG(LOG_DEBUG, "Common: %s: REQ_OPERATERMTSEATVENTILATION", __FUNCTION__);
            Message<Req_OperateRMTSeatVenti> *mm = (Message<Req_OperateRMTSeatVenti>*)m;
            LOG(LOG_DEBUG, "Common: %s:  telmSeatDrvVenti: %d (C)", __FUNCTION__, mm->message.telmSeatDrvVenti);
            LOG(LOG_DEBUG, "Common: %s:  telmSeatPassVenti: %d (C)", __FUNCTION__, mm->message.telmSeatPassVenti);
            LOG(LOG_DEBUG, "Common: %s:  telmSeatSecLeVenti: %d (C)", __FUNCTION__, mm->message.telmSeatSecLeVenti);
            LOG(LOG_DEBUG, "Common: %s:  telmSeatSecRiVenti: %d (C)", __FUNCTION__, mm->message.telmSeatSecRiVenti);
            break;
        }
        case REQ_OPERATERMTSTEERWHLHEAT:{
            LOG(LOG_DEBUG, "Common: %s: REQ_OPERATERMTSTEERWHLHEAT", __FUNCTION__);
            Message<Req_OperateRMTSteerWhlHeat_S> *mm = (Message<Req_OperateRMTSteerWhlHeat_S>*)m;
            LOG(LOG_DEBUG, "Common: %s:  SteerWhlHeatgDurgClimaEnadFromTelm: %d (C)", __FUNCTION__, mm->message.SteerWhlHeatgDurgClimaEnadFromTelm);
            break;
        }
        case REQ_PARKINGCLIMATEOPER:{
            LOG(LOG_DEBUG, "Common: %s: REQ_PARKINGCLIMATEOPER", __FUNCTION__);
            break;
        }
        //end uia93888
        case REQ_ROLLOVERVALUE: {
            Message<ReqRolloverValue> *mm = (Message<ReqRolloverValue>*)m;
            LOG(LOG_DEBUG, "Common: %s:     RolloverItem: %d (C)", __FUNCTION__, mm->message.configItem);
            break;
        }
        case REQ_SETROLLOVERCFG: {
            Message<ReqSetRolloverCfg> *mm = (Message<ReqSetRolloverCfg>*)m;
            LOG(LOG_DEBUG, "Common: %s:     RolloverItem: %d (C)", __FUNCTION__, mm->message.configItem);
            LOG(LOG_DEBUG, "Common: %s:     length: %d (C)", __FUNCTION__, mm->message.length);
            break;
        }
        // VGM
        case REQ_SENDPOSNFROMSATLTCON: {
            Message<ReqSendPosnFromSatltCon> *mm = (Message<ReqSendPosnFromSatltCon>*)m;
            LOG(LOG_DEBUG, "Common: %s:         Latitude: %d",__FUNCTION__, mm->message.PosnLat);
            LOG(LOG_DEBUG, "Common: %s:         Longitude: %d",__FUNCTION__, mm->message.PosnLgt);
            LOG(LOG_DEBUG, "Common: %s:         Altitide from mean sea level: %d",__FUNCTION__, mm->message.PosnAlti);
            LOG(LOG_DEBUG, "Common: %s:         Speed: %d",__FUNCTION__, mm->message.PosnSpd);
            LOG(LOG_DEBUG, "Common: %s:         Horizontal velocity: %d",__FUNCTION__, mm->message.PosnVHozl);
            LOG(LOG_DEBUG, "Common: %s:         Vertical velocity: %d",__FUNCTION__, mm->message.PosnVVert);
            LOG(LOG_DEBUG, "Common: %s:         Heading from true north: %d",__FUNCTION__, mm->message.PosnDir);
            LOG(LOG_DEBUG, "Common: %s:         UTC year: %d",__FUNCTION__, mm->message.TiForYr);
            LOG(LOG_DEBUG, "Common: %s:         UTC month: %d",__FUNCTION__, mm->message.TiForMth);
            LOG(LOG_DEBUG, "Common: %s:         UTC day: %d",__FUNCTION__, mm->message.TiForDay);
            LOG(LOG_DEBUG, "Common: %s:         UTC hour: %d",__FUNCTION__, mm->message.TiForHr);
            LOG(LOG_DEBUG, "Common: %s:         UTC minute: %d",__FUNCTION__, mm->message.TiForMins);
            LOG(LOG_DEBUG, "Common: %s:         UTC second: %d",__FUNCTION__, mm->message.TiForSec);
            LOG(LOG_DEBUG, "Common: %s:         GPS used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo1InUse);
            LOG(LOG_DEBUG, "Common: %s:         GLONASS used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo2InUse);
            LOG(LOG_DEBUG, "Common: %s:         Galileo used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo3InUse);
            LOG(LOG_DEBUG, "Common: %s:         SBAS used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo4InUse);
            LOG(LOG_DEBUG, "Common: %s:         QZSS L1 used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo5InUse);
            LOG(LOG_DEBUG, "Common: %s:         QZSS L1-SAIF used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo6InUse);
            LOG(LOG_DEBUG, "Common: %s:         Navigation solution status: %d",__FUNCTION__, mm->message.SatltPosnStsPrm1);
            LOG(LOG_DEBUG, "Common: %s:         DGPS usage in solution: %d",__FUNCTION__, mm->message.SatltPosnStsPrm2);
            LOG(LOG_DEBUG, "Common: %s:         Self ephemeris data usage in solution: %d",__FUNCTION__, mm->message.SatltPosnStsPrm3);
            LOG(LOG_DEBUG, "Common: %s:         Number of GPS satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo1);
            LOG(LOG_DEBUG, "Common: %s:         Number of GLONASS satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo2);
            LOG(LOG_DEBUG, "Common: %s:         Number of Galileo satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo3);
            LOG(LOG_DEBUG, "Common: %s:         Number of SBAS satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo4);
            LOG(LOG_DEBUG, "Common: %s:         Number of QZSS L1 satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo5);
            LOG(LOG_DEBUG, "Common: %s:         Number of QZSS L1-SAIF satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo6);
            LOG(LOG_DEBUG, "Common: %s:         Position Dilution Of Precision: %d",__FUNCTION__, mm->message.PrePosnDil);
            LOG(LOG_DEBUG, "Common: %s:         Horizontal Dilution Of Precision: %d",__FUNCTION__, mm->message.PreHozlDil);
            LOG(LOG_DEBUG, "Common: %s:         Vertical Dilution Of Precision: %d",__FUNCTION__, mm->message.PreVertDil);
            LOG(LOG_DEBUG, "Common: %s:         Time Dilution Of Precision: %d",__FUNCTION__, mm->message.PreTiDil);
            break;
        }

        // IHU

        // Data-storage
        case REQ_GETDSPARAMETER: {
            Message<ReqGetDSParameter> *mm = (Message<ReqGetDSParameter>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            break;
        }

        // Message-processor
        case REQ_VUCSWVERSION:
            break;
        case REQ_BLESWVERSION:
            break;
        case REQ_LASTBUBSTATE:
            break;
        case REQ_CARUSAGEMODE:
            break;
        case REQ_CARMODE:
            break;
        case REQ_ELECTENGLVL:
            break;
        case REQ_GETVUCPOWERMODE:
            break;
        case REQ_GETVUCWAKEUPREASON:
            break;
        case REQ_GETWIFIDATA:
            break;
        case REQ_GETPSIMAVAILABILITY:
            break;
        case REQ_GETFUELTYPE:
            break;
        case REQ_GETPROPULSIONTYPE:
            break;
        case REQ_GETOHCBTNSTATE:
            break;
        case REQ_GETNUMBEROFDOORS:
            break;
        case REQ_GETSTEERINGWHEELPOSITION:
            break;
        case REQ_GETFUELTANKVOLUME:
            break;
        case REQ_GETASSISTANCESERVICES:
            break;
        case REQ_GETAUTONOMOUSDRIVE:
            break;
        case REQ_GETCOMBINEDINSTRUMENT:
            break;
        case REQ_GETDIGITALVIDEORECORDER:
            break;
        case REQ_GETDRIVERALERTCONTROL:
            break;
        case REQ_GETTELEMATICMODULE:
            break;
        case REQ_GETCONNECTIVITY:
            break;
        case REQ_GETCONNECTEDSERVICEBOOKING:
            break;
        // Multi
        case REQ_GETMSGCATVERSION: {
            Message<ReqGetMsgCatVer> *mm = (Message<ReqGetMsgCatVer>*)m;
            LOG(LOG_DEBUG, "Common: %s:         version: 0x%02hhX", __FUNCTION__, mm->message.version);
            break;
        }
        case REQ_GETTCAMHWVERSION:
            break;
        case REQ_GETVUCSWBUILD:
            break;
        case REQ_GETBLESWBUILD:
            break;
        case REQ_GETRFVERSION:
            break;
        case REQ_GETFBLSWBUILD:
            break;
        // Test
        case REQ_VGMTESTMSG: {
            Message<ReqVGMTestMsg> *mm = (Message<ReqVGMTestMsg>*)m;
            LOG(LOG_DEBUG, "Common: %s:     id: %d", __FUNCTION__, mm->message.id);
            LOG(LOG_DEBUG, "Common: %s:     company: %s", __FUNCTION__, mm->message.company);
            break;
        }
        case REQ_IHUTESTMSG: {
            Message<ReqIHUTestMsg> *mm = (Message<ReqIHUTestMsg>*)m;
            LOG(LOG_DEBUG, "Common: %s:     id: %d", __FUNCTION__, mm->message.id);
            LOG(LOG_DEBUG, "Common: %s:     city: %s", __FUNCTION__, mm->message.city);
            break;
        }
        case REQ_TEST_TOGGLEUSAGEMODE:
            break;
        case REQ_TEST_TOGGLECARMODE:
            break;
        case REQ_TEST_CRASHSTATE:
            break;
        case REQ_TEST_OHCSTATE:
            break;
        case REQ_TEST_SETFAKEVALUESDEFAULT:
            break;
        case REQ_TEST_SETFAKEVALUEINT: {
            Message<ReqTestSetFakeValueInt> *mm = (Message<ReqTestSetFakeValueInt>*)m;
            LOG(LOG_DEBUG, "Common: %s:     collection: %d", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:     name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:     value: %d", __FUNCTION__, mm->message.value);
            break;
        }
        case REQ_TEST_SETFAKEVALUESTR: {
            Message<ReqTestSetFakeValueStr> *mm = (Message<ReqTestSetFakeValueStr>*)m;
            LOG(LOG_DEBUG, "Common: %s:     collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:     name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:     value: %s", __FUNCTION__, mm->message.value);
            break;
        }
        case REQ_TEST_SETLOCALCONFIG: {
            break;
        }
        default:
            LOG(LOG_WARN, "Common: %s: Unknown message-ID (%d)!", __FUNCTION__ , m->id_);
            return RET_ERR_INVALID_ARG;
        }

        break;
    case MESSAGE_RESPONSE:
        switch ((ResponseID)m->id_) {

        case RES_REQUESTSENT:
            break;

        // VuC
        case RES_HORNNLIGHT: {
            Message<ResHornNLight> *mm = (Message<ResHornNLight>*)m;
            LOG(LOG_DEBUG, "Common: %s:         return_code: %d", __FUNCTION__, mm->message.return_code);
            break;
        }
        case RES_ANTENNASWITCH: {
            Message<ResAntennaSwitch> *mm = (Message<ResAntennaSwitch>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d", __FUNCTION__, mm->message.mode);
            break;
        }
        case RES_VUCTRACE_CONFIG: {
            Message<ResVucTraceConfig> *mm = (Message<ResVucTraceConfig>*)m;
            LOG(LOG_DEBUG, "Common: %s:         bus: %d", __FUNCTION__, mm->message.bus);
            for (uint8_t i = 0; i < sizeof(mm->message.config)/sizeof(TraceModuleCfgStatus); i++) {
                LOG(LOG_DEBUG, "Common: %s:         config[%d] -> module_id: %d  status: %d",
                        __FUNCTION__, i, mm->message.config[i].module_id, mm->message.config[i].status);
            }
            break;
        }
        case RES_VUCTRACE_CONTROL: {
            Message<ResVucTraceControl> *mm = (Message<ResVucTraceControl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d", __FUNCTION__, mm->message.mode);
            break;
        }
        case RES_BLE_DISCONNECT: {
            Message<ResBleDisconnect> *mm = (Message<ResBleDisconnect>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_DISCONNECTALL: {
            Message<ResBleDisconnectAll> *mm = (Message<ResBleDisconnectAll>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_GENERATEBDAK: {
            Message<ResBleGenBdak> *mm = (Message<ResBleGenBdak>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_GETBDAK: {
            Message<ResBleGetBdak> *mm = (Message<ResBleGetBdak>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            LOG(LOG_DEBUG, "Common: %s:         bdak: %02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX", __FUNCTION__,
                mm->message.bdak[0], mm->message.bdak[1], mm->message.bdak[2], mm->message.bdak[3],
                mm->message.bdak[4], mm->message.bdak[5], mm->message.bdak[6], mm->message.bdak[7],
                mm->message.bdak[8], mm->message.bdak[9], mm->message.bdak[10], mm->message.bdak[11],
                mm->message.bdak[12], mm->message.bdak[13], mm->message.bdak[14], mm->message.bdak[15]);
            break;
        }
        case RES_BLE_CONFIG_DATACOMSERVICE: {
            Message<ResBleCfgDataCommService> *mm = (Message<ResBleCfgDataCommService>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_CONFIG_IBEACON: {
            Message<ResBleCfgIbeacon> *mm = (Message<ResBleCfgIbeacon>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_CONFIG_GENERICACCESS: {
            Message<ResBleCfgGenAcc> *mm = (Message<ResBleCfgGenAcc>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_STARTPARINGSERVICE: {
            Message<ResBleStartPairService> *mm = (Message<ResBleStartPairService>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_STOPPARINGSERVICE: {
            Message<ResBleStopPairService> *mm = (Message<ResBleStopPairService>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_STARTADVERTISING: {
            Message<ResBleStartAdvertising> *mm = (Message<ResBleStartAdvertising>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_BLE_STOPADVERTISING: {
            Message<ResBleStopAdvertising> *mm = (Message<ResBleStopAdvertising>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_DTCCTLSETTINGS: {
            Message<ResDtcCtlSettings> *mm = (Message<ResDtcCtlSettings>*)m;
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case RES_GETCARCONFIG:
            break;
        case RES_CARCONFIGFACTRESTORE:
            break;
        case RES_ADCVALUE: {
            Message<ResAdcValue> *mm = (Message<ResAdcValue>*)m;
            LOG(LOG_DEBUG, "Common: %s:         id: %d", __FUNCTION__, mm->message.id);
            LOG(LOG_DEBUG, "Common: %s:         ref_value: %d", __FUNCTION__, mm->message.ref_value);
            LOG(LOG_DEBUG, "Common: %s:         adc_value: %d", __FUNCTION__, mm->message.adc_value);
            break;
        }
        case RES_GETMSGCATVERSION: {
            Message<ResGetMsgCatVer> *mm = (Message<ResGetMsgCatVer>*)m;
            LOG(LOG_DEBUG, "Common: %s:         version: 0x%04X", __FUNCTION__, mm->message.version);
            break;
        }
        case RES_GETTCAMHWVERSION: {
            Message<ResGetTcamHwVer> *mm = (Message<ResGetTcamHwVer>*)m;
            LOG(LOG_DEBUG, "Common: %s:         eol_hw_version: 0x%x", __FUNCTION__, mm->message.eol_hw_version);
            LOG(LOG_DEBUG, "Common: %s:         eol_data: 0x%x", __FUNCTION__, mm->message.eol_data);
            LOG(LOG_DEBUG, "Common: %s:         eol_hw_variant: 0x%x", __FUNCTION__, mm->message.eol_hw_variant);
            LOG(LOG_DEBUG, "Common: %s:         eol_volvo_ecu_serial_number: 0x%02X%02X%02X%02X", __FUNCTION__, mm->message.eol_volvo_ecu_serial_number[0],
                mm->message.eol_volvo_ecu_serial_number[1], mm->message.eol_volvo_ecu_serial_number[2], mm->message.eol_volvo_ecu_serial_number[3]);
            LOG(LOG_DEBUG, "Common: %s:         eol_volvo_delivery_part_number: %.*s", __FUNCTION__, LENGTH(mm->message.eol_volvo_delivery_part_number), mm->message.eol_volvo_delivery_part_number);
            LOG(LOG_DEBUG, "Common: %s:         eol_volvo_core_part_number: %.*s", __FUNCTION__, LENGTH(mm->message.eol_volvo_core_part_number), mm->message.eol_volvo_core_part_number);
            LOG(LOG_DEBUG, "Common: %s:         eol_volvo_production_year: 0x%04X", __FUNCTION__, mm->message.eol_volvo_production_year);
            LOG(LOG_DEBUG, "Common: %s:         eol_volvo_production_month: 0x%02X", __FUNCTION__, mm->message.eol_volvo_production_month);
            LOG(LOG_DEBUG, "Common: %s:         eol_volvo_production_day: 0x%02X", __FUNCTION__, mm->message.eol_volvo_production_day);
            break;
        }
        case RES_GETRFVERSION: {
            Message<ResGetRfVersion> *mm = (Message<ResGetRfVersion>*)m;
            LOG(LOG_DEBUG, "Common: %s:         rf_sw_version: 0x%04X", __FUNCTION__, mm->message.sw_version);
            LOG(LOG_DEBUG, "Common: %s:         rf_hw_version: %02hhX-%02hhX-%02hhX-%02hhX", __FUNCTION__, mm->message.hw_version[0],
                mm->message.hw_version[1], mm->message.hw_version[2], mm->message.hw_version[3]);
            LOG(LOG_DEBUG, "Common: %s:         rf_sw_variant: 0x%x", __FUNCTION__, mm->message.sw_variant);
            LOG(LOG_DEBUG, "Common: %s:         rf_hw_variant: 0x%x", __FUNCTION__, mm->message.hw_variant);
            LOG(LOG_DEBUG, "Common: %s:         rf_hw_serial_number: %02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX-%02hhX", __FUNCTION__,
                mm->message.hw_serial_number[0], mm->message.hw_serial_number[1], mm->message.hw_serial_number[2],mm->message.hw_serial_number[3],
                mm->message.hw_serial_number[4], mm->message.hw_serial_number[5], mm->message.hw_serial_number[6],mm->message.hw_serial_number[7]);
            break;
        }
        case RES_GETVUCBLESWPARTNUMBER: {
            Message<ResGetVucBleSwPartNumber> *mm = (Message<ResGetVucBleSwPartNumber>*)m;
            LOG(LOG_DEBUG, "Common: %s:         vuc_part_number: %.*s", __FUNCTION__, LENGTH(mm->message.vuc_part_number), mm->message.vuc_part_number);
            LOG(LOG_DEBUG, "Common: %s:         vuc_version: %.*s", __FUNCTION__, LENGTH(mm->message.vuc_version), mm->message.vuc_version);
            LOG(LOG_DEBUG, "Common: %s:         ble_part_number: %.*s", __FUNCTION__, LENGTH(mm->message.ble_part_number), mm->message.ble_part_number);
            LOG(LOG_DEBUG, "Common: %s:         ble_version: %.*s", __FUNCTION__, LENGTH(mm->message.ble_version), mm->message.ble_version);
            break;
        }
        case RES_GETBELTINFORMATION: {
            Message<ResGetBeltInformation> *mm = (Message<ResGetBeltInformation>*)m;
            LOG(LOG_DEBUG, "Common: %s:         driver.lock_state: %d", __FUNCTION__, mm->message.driver.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         driver.lock_status: %d", __FUNCTION__, mm->message.driver.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         driver.equipped_status: %d", __FUNCTION__, mm->message.driver.equipped_status);

            LOG(LOG_DEBUG, "Common: %s:         passenger.lock_state: %d", __FUNCTION__, mm->message.passenger.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         passenger.lock_status: %d", __FUNCTION__, mm->message.passenger.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         passenger.equipped_status: %d", __FUNCTION__, mm->message.passenger.equipped_status);

            LOG(LOG_DEBUG, "Common: %s:         second_row_left.lock_state: %d", __FUNCTION__, mm->message.second_row_left.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         second_row_left.lock_status: %d", __FUNCTION__, mm->message.second_row_left.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         second_row_left.equipped_status: %d", __FUNCTION__, mm->message.second_row_left.equipped_status);

            LOG(LOG_DEBUG, "Common: %s:         second_row_middle.lock_state: %d", __FUNCTION__, mm->message.second_row_middle.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         second_row_middle.lock_status: %d", __FUNCTION__, mm->message.second_row_middle.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         second_row_middle.equipped_status: %d", __FUNCTION__, mm->message.second_row_middle.equipped_status);

            LOG(LOG_DEBUG, "Common: %s:         second_row_right.lock_state: %d", __FUNCTION__, mm->message.second_row_right.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         second_row_right.lock_status: %d", __FUNCTION__, mm->message.second_row_right.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         second_row_right.equipped_status: %d", __FUNCTION__, mm->message.second_row_right.equipped_status);

            LOG(LOG_DEBUG, "Common: %s:         third_row_left.lock_state: %d", __FUNCTION__, mm->message.third_row_left.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         third_row_left.lock_status: %d", __FUNCTION__, mm->message.third_row_left.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         third_row_left.equipped_status: %d", __FUNCTION__, mm->message.third_row_left.equipped_status);

            LOG(LOG_DEBUG, "Common: %s:         third_row_right.lock_state: %d", __FUNCTION__, mm->message.third_row_right.lock_state);
            LOG(LOG_DEBUG, "Common: %s:         third_row_right.lock_status: %d", __FUNCTION__, mm->message.third_row_right.lock_status);
            LOG(LOG_DEBUG, "Common: %s:         third_row_right.equipped_status: %d", __FUNCTION__, mm->message.third_row_right.equipped_status);
            break;
        }
        case RES_GETDIAGERRREPORT: {
            Message<ResGetDiagErrReport> *mm = (Message<ResGetDiagErrReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         phone_ant_open_load_test_status: %d",__FUNCTION__, mm->message.phone_ant_open_load_test_status);
            LOG(LOG_DEBUG, "Common: %s:         phone_ant_open_load_test_state: %d",__FUNCTION__, mm->message.phone_ant_open_load_test_state);
            LOG(LOG_DEBUG, "Common: %s:         ecall_ant_open_load_test_status: %d",__FUNCTION__, mm->message.ecall_ant_open_load_test_status);
            LOG(LOG_DEBUG, "Common: %s:         ecall_ant_open_load_test_state: %d",__FUNCTION__, mm->message.ecall_ant_open_load_test_state);
            break;
        }
        case RES_GETVEHICLESTATE: {
            Message<ResGetVehicleState> *mm = (Message<ResGetVehicleState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         carmode: %d ",__FUNCTION__, mm->message.carmode);
            LOG(LOG_DEBUG, "Common: %s:         usagemode: %d ",__FUNCTION__, mm->message.usagemode);
            break;
        }
        case RES_GETDIDGLOBALSNAPSHOTDATA: {
            Message<ResGetDIDGlobalSnapshotData> *mm = (Message<ResGetDIDGlobalSnapshotData>*)m;
            LOG(LOG_DEBUG, "Common: %s:         dd00: 0x%02hhX 0x%02hhX 0x%02hhX 0x%02hhX",__FUNCTION__,
                mm->message.dd00[0], mm->message.dd00[1], mm->message.dd00[2], mm->message.dd00[3]);
            LOG(LOG_DEBUG, "Common: %s:         dd01: 0x%02hhX 0x%02hhX 0x%02hhX",__FUNCTION__,
                mm->message.dd01[0], mm->message.dd01[1], mm->message.dd01[2]);
            LOG(LOG_DEBUG, "Common: %s:         dd02: 0x%02hhX",__FUNCTION__, mm->message.dd02);
            LOG(LOG_DEBUG, "Common: %s:         dd0a: 0x%02hhX",__FUNCTION__, mm->message.dd0a);
            LOG(LOG_DEBUG, "Common: %s:         dd0c: 0x%02hhX",__FUNCTION__, mm->message.dd0c);
            break;
        }
        case RES_DTCSDETECTED:
            break;
        case RES_PSIMDATA: {
            Message<ResPSIMData> *mm = (Message<ResPSIMData>*)m;
#if 1 //nieyj fix klocwork 190274
            for (uint16_t i = 0; i < LENGTH(mm->message.data); i++) {
#else
            for (uint8_t i = 0; i < LENGTH(mm->message.data); i++) {
#endif
                LOG(LOG_DEBUG, "Common: %s:         data[%d]: 0x%02hhX", __FUNCTION__, i, mm->message.data[i]);
            }
            break;
        }
        case RES_THERMALMITIGATION:
            break;
        case RES_CARCFGPARAMETERFAULT:{
            Message<ResCarCfgParameterFault> *mm = (Message<ResCarCfgParameterFault>*)m;
            LOG(LOG_DEBUG, "Common: %s:  Number of invalid car configuration parameter values recognised: %d", __FUNCTION__, mm->message.numInvalidCfg);
            for (uint8_t i = 0; i < LENGTH(mm->message.parameterFault); i++){
            LOG(LOG_DEBUG, "Common: %s:  Number of invalid car configuration parameter values recognised: %d", __FUNCTION__, mm->message.parameterFault[i].numData);
            LOG(LOG_DEBUG, "Common: %s:  Number of invalid car configuration parameter values recognised: %d", __FUNCTION__, mm->message.parameterFault[i].value);
            }
            break;
        }
        case RES_NETWORK_MANAGEMENT_HISTORY:{
            Message<ResNetworkManageHistory> *mm = (Message<ResNetworkManageHistory>*)m;
            LOG(LOG_DEBUG, "Common: %s:  Latest State NM History1: %d", __FUNCTION__, mm->message.nmHistoryStateLast1);
            LOG(LOG_DEBUG, "Common: %s:  Latest State Global Real Time1: %d", __FUNCTION__, mm->message.globalRealTimeStateLast1);
            LOG(LOG_DEBUG, "Common: %s:  Latest State NM History4: %d", __FUNCTION__, mm->message.nmHistoryStateLast4);
            LOG(LOG_DEBUG, "Common: %s:  Latest State Global Real Time4: %d", __FUNCTION__, mm->message.globalRealTimeStateLast4);
	     LOG(LOG_DEBUG, "Common: %s:  Latest State NM History5: %d", __FUNCTION__, mm->message.nmHistoryStateLast5);
            LOG(LOG_DEBUG, "Common: %s:  Latest State Global Real Time5: %d", __FUNCTION__, mm->message.globalRealTimeStateLast5);
            LOG(LOG_DEBUG, "Common: %s:  Latest State NM History9: %d", __FUNCTION__, mm->message.nmHistoryStateLast9);
            LOG(LOG_DEBUG, "Common: %s:  Latest State Global Real Time9: %d", __FUNCTION__, mm->message.globalRealTimeStateLast9);
	     LOG(LOG_DEBUG, "Common: %s:  Latest State NM History10: %d", __FUNCTION__, mm->message.nmHistoryStateLast10);
            LOG(LOG_DEBUG, "Common: %s:  Latest State Global Real Time10: %d", __FUNCTION__, mm->message.globalRealTimeStateLast10);
            break;
        }

        case RES_PROGRAM_PRECOND_CHK:{
            Message<ResProgramPrecond_Chk> *mm = (Message<ResProgramPrecond_Chk>*)m;
            LOG(LOG_DEBUG, "Common: %s:  usage mode: %d", __FUNCTION__, mm->message.UsgModSts);
	     LOG(LOG_DEBUG, "Common: %s:  vehicle speed: %d", __FUNCTION__, mm->message.VehSpdLgt);
            LOG(LOG_DEBUG, "Common: %s:  voltage status: %d", __FUNCTION__, mm->message.KL30Sts);
            break;
        }

        case RES_ROLLOVERVALUE: {
            Message<ResRolloverValue> *mm = (Message<ResRolloverValue>*)m;
            LOG(LOG_DEBUG, "Common: %s:         configItem",__FUNCTION__, mm->message.configItem);
            LOG(LOG_DEBUG, "Common: %s:         length",__FUNCTION__, mm->message.length);
            LOG(LOG_DEBUG, "Common: %s:         data",__FUNCTION__, mm->message.data);
            break;
        }
        case RES_SETROLLOVERCFG: {
            Message<ResSetRolloverCfg> *mm = (Message<ResSetRolloverCfg>*)m;
            LOG(LOG_DEBUG, "Common: %s:         configItem",__FUNCTION__, mm->message.configItem);
            LOG(LOG_DEBUG, "Common: %s:         result",__FUNCTION__, mm->message.result);
            break;
        }
        // VGM

        // IHU

        // Data-storage
        case RES_GETDSPARAMETER_INT: {
            Message<ResGetDSParameterInt> *mm = (Message<ResGetDSParameterInt>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:         value: %d", __FUNCTION__, mm->message.value);
            break;
        }
        case RES_GETDSPARAMETER_STR: {
            Message<ResGetDSParameterStr> *mm = (Message<ResGetDSParameterStr>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:         value: %s", __FUNCTION__, mm->message.value);
            break;
        }
        case RES_GETDSPARAMETER_VECT: {
            Message<ResGetDSParameterVect> *mm = (Message<ResGetDSParameterVect>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:         size: %d", __FUNCTION__, mm->message.size);
            for (uint8_t i = 0; i < mm->message.size; i++) {
                LOG(LOG_DEBUG, "Common: %s:         value[%d]: 0x%x", __FUNCTION__, i, mm->message.value);
            }
            break;
        }
        case RES_CARMODE: {
            Message<ResCarMode> *mm = (Message<ResCarMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         carmode: %d", __FUNCTION__, mm->message.carmode);
            break;
        }
        case RES_CARUSAGEMODE: {
            Message<ResCarUsageMode> *mm = (Message<ResCarUsageMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         usagemode: %d ", __FUNCTION__, mm->message.usagemode);
            break;
        }
        case RES_ELECTENGLVL: {
            Message<ResElectEngLvl> *mm = (Message<ResElectEngLvl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         level: %d", __FUNCTION__, mm->message.level);
            break;
        }
        case RES_VINNUMBER: {
            Message<ResVINNumber> *mm = (Message<ResVINNumber>*)m;
            LOG(LOG_DEBUG, "Common: %s:         vin_number: %s", __FUNCTION__, mm->message.vin_number);
            break;
        }
        case RES_GETVUCPOWERMODE: {
            Message<ResGetVucPowerMode> *mm = (Message<ResGetVucPowerMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         power_mode: %d", __FUNCTION__, mm->message.power_mode);
            break;
        }
        case RES_GETVUCWAKEUPREASON: {
            Message<ResGetVucWakeupReason> *mm = (Message<ResGetVucWakeupReason>*)m;
            LOG(LOG_DEBUG, "Common: %s:         wakeup_reason: %d", __FUNCTION__, mm->message.wakeup_reason);
            break;
        }

        // Message-processor
        case RES_GETVUCSWBUILD: {
            Message<ResGetVucSwBuild> *mm = (Message<ResGetVucSwBuild>*)m;
            LOG(LOG_DEBUG, "Common: %s:         year: 0x%04X", __FUNCTION__, mm->message.year);
            LOG(LOG_DEBUG, "Common: %s:         month: 0x%02hhX", __FUNCTION__, mm->message.month);
            LOG(LOG_DEBUG, "Common: %s:         day: 0x%02hhX", __FUNCTION__, mm->message.day);
            LOG(LOG_DEBUG, "Common: %s:         hour: 0x%02hhX", __FUNCTION__, mm->message.hour);
            LOG(LOG_DEBUG, "Common: %s:         min: 0x%02hhX", __FUNCTION__, mm->message.min);
            LOG(LOG_DEBUG, "Common: %s:         sec: 0x%02hhX", __FUNCTION__, mm->message.sec);
            LOG(LOG_DEBUG, "Common: %s:         checksum: 0x%04X", __FUNCTION__, mm->message.checksum);
            LOG(LOG_DEBUG, "Common: %s:         sw_version: 0x%04X", __FUNCTION__, mm->message.sw_version);
            break;
        }
        case RES_GETBLESWBUILD: {
            Message<ResGetBleSwBuild> *mm = (Message<ResGetBleSwBuild>*)m;
            LOG(LOG_DEBUG, "Common: %s:         year: 0x%04X", __FUNCTION__, mm->message.year);
            LOG(LOG_DEBUG, "Common: %s:         month: 0x%02hhX", __FUNCTION__, mm->message.month);
            LOG(LOG_DEBUG, "Common: %s:         day: 0x%02hhX", __FUNCTION__, mm->message.day);
            LOG(LOG_DEBUG, "Common: %s:         hour: 0x%02hhX", __FUNCTION__, mm->message.hour);
            LOG(LOG_DEBUG, "Common: %s:         min: 0x%02hhX", __FUNCTION__, mm->message.min);
            LOG(LOG_DEBUG, "Common: %s:         sec: 0x%02hhX", __FUNCTION__, mm->message.sec);
            LOG(LOG_DEBUG, "Common: %s:         checksum: 0x%04X", __FUNCTION__, mm->message.checksum);
            LOG(LOG_DEBUG, "Common: %s:         sw_version: 0x%04X", __FUNCTION__, mm->message.sw_version);
            LOG(LOG_DEBUG, "Common: %s:         stack_version[0]: 0x%02hhX", __FUNCTION__, mm->message.stack_version[0]);
            LOG(LOG_DEBUG, "Common: %s:         stack_version[1]: 0x%02hhX", __FUNCTION__, mm->message.stack_version[1]);
            LOG(LOG_DEBUG, "Common: %s:         stack_version[2]: 0x%02hhX", __FUNCTION__, mm->message.stack_version[2]);
            break;
        }
        case RES_VUCSWVERSION: {
            Message<ResVucSwVersion> *mm = (Message<ResVucSwVersion>*)m;
            LOG(LOG_DEBUG, "Common: %s:         vuc_part_number: %.*s", __FUNCTION__, LENGTH(mm->message.vuc_part_number), mm->message.vuc_part_number);
            LOG(LOG_DEBUG, "Common: %s:         vuc_version: %.*s", __FUNCTION__, LENGTH(mm->message.vuc_version), mm->message.vuc_version);
            break;
        }
        case RES_BLESWVERSION: {
            Message<ResBleSwVersion> *mm = (Message<ResBleSwVersion>*)m;
            LOG(LOG_DEBUG, "Common: %s:         ble_part_number: %.*s", __FUNCTION__, LENGTH(mm->message.ble_part_number), mm->message.ble_part_number);
            LOG(LOG_DEBUG, "Common: %s:         ble_version: %.*s", __FUNCTION__, LENGTH(mm->message.ble_version), mm->message.ble_version);
            break;
        }
        case RES_LASTBUBSTATE: {
            Message<ResLastBubState> *mm = (Message<ResLastBubState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         battery_used: %d", __FUNCTION__, mm->message.battery_used);
            LOG(LOG_DEBUG, "Common: %s:         battery_state: %d", __FUNCTION__, mm->message.battery_state);
            LOG(LOG_DEBUG, "Common: %s:         charge_level: %d", __FUNCTION__, mm->message.charge_level);
            LOG(LOG_DEBUG, "Common: %s:         backup_battery_temp: %d", __FUNCTION__, mm->message.backup_battery_temp);
            LOG(LOG_DEBUG, "Common: %s:         ambient_temp: %d", __FUNCTION__, mm->message.ambient_temp);
            LOG(LOG_DEBUG, "Common: %s:         battery_voltage: %.2f", __FUNCTION__, mm->message.battery_voltage);
            LOG(LOG_DEBUG, "Common: %s:         kl30_voltage: %.2f", __FUNCTION__, mm->message.kl30_voltage);
            break;
        }
        case RES_GETWIFIDATA: {
            Message<ResGetWifiData> *mm = (Message<ResGetWifiData>*)m;
            LOG(LOG_DEBUG, "Common: %s:         wifi_functionality_control: %d", __FUNCTION__, mm->message.wifi_functionality_control);
            LOG(LOG_DEBUG, "Common: %s:         wifi_functionality_control_validity: %d", __FUNCTION__, mm->message.wifi_functionality_control_validity);
            LOG(LOG_DEBUG, "Common: %s:         wifi_frequency_band: %d", __FUNCTION__, mm->message.wifi_frequency_band);
            LOG(LOG_DEBUG, "Common: %s:         wifi_frequency_band_validity: %d", __FUNCTION__, mm->message.wifi_frequency_band_validity);
            LOG(LOG_DEBUG, "Common: %s:         wifi_workshop_mode: %d", __FUNCTION__, mm->message.wifi_workshop_mode);
            LOG(LOG_DEBUG, "Common: %s:         wifi_workshop_mode_validity: %d", __FUNCTION__, mm->message.wifi_workshop_mode_validity);
            break;
        }
        case RES_GETPSIMAVAILABILITY: {
            Message<ResGetPSIMAvailability> *mm = (Message<ResGetPSIMAvailability>*)m;
            LOG(LOG_DEBUG, "Common: %s:         psim_availability: %d", __FUNCTION__, mm->message.psim_availability);
            LOG(LOG_DEBUG, "Common: %s:         psim_availability_validity: %d", __FUNCTION__, mm->message.psim_availability_validity);
            break;
        }
        case RES_GETFUELTYPE: {
            Message<ResGetFuelType> *mm = (Message<ResGetFuelType>*)m;
            LOG(LOG_DEBUG, "Common: %s:         fuel_type: %d", __FUNCTION__, mm->message.fuel_type);
            LOG(LOG_DEBUG, "Common: %s:         fuel_type_validity: %d", __FUNCTION__, mm->message.fuel_type_validity);
            break;
        }
        case RES_GETPROPULSIONTYPE: {
            Message<ResGetPropulsionType> *mm = (Message<ResGetPropulsionType>*)m;
            LOG(LOG_DEBUG, "Common: %s:         propulsion_type: %d", __FUNCTION__, mm->message.propulsion_type);
            LOG(LOG_DEBUG, "Common: %s:         propulsion_type_validity: %d", __FUNCTION__, mm->message.propulsion_type_validity);
            break;
        }
        case RES_GETOHCBTNSTATE: {
            Message<ResGetOhcBtnState> *mm = (Message<ResGetOhcBtnState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         ecall_btn: %d", __FUNCTION__, mm->message.ecall_btn);
            LOG(LOG_DEBUG, "Common: %s:         icall_btn: %d", __FUNCTION__, mm->message.icall_btn);
            break;
        }
        case RES_GETNUMBEROFDOORS: {
            Message<ResGetNumberOfDoors> *mm = (Message<ResGetNumberOfDoors>*)m;
            LOG(LOG_DEBUG, "Common: %s:         nbr_of_doors: %d", __FUNCTION__, mm->message.nbr_of_doors);
            LOG(LOG_DEBUG, "Common: %s:         nbr_of_doors_validity: %d", __FUNCTION__, mm->message.nbr_of_doors_validity);
            break;
        }
        case RES_GETSTEERINGWHEELPOSITION: {
            Message<ResGetSteeringWheelPosition> *mm = (Message<ResGetSteeringWheelPosition>*)m;
            LOG(LOG_DEBUG, "Common: %s:         steering_wheel_position: %d", __FUNCTION__, mm->message.steering_wheel_position);
            LOG(LOG_DEBUG, "Common: %s:         steering_wheel_position_validity: %d", __FUNCTION__, mm->message.steering_wheel_position_validity);
            break;
        }
        case RES_GETFUELTANKVOLUME: {
            Message<ResGetFuelTankVolume> *mm = (Message<ResGetFuelTankVolume>*)m;
            LOG(LOG_DEBUG, "Common: %s:         fuel_tank_volume: %d", __FUNCTION__, mm->message.fuel_tank_volume);
            LOG(LOG_DEBUG, "Common: %s:         fuel_tank_volume_validity: %d", __FUNCTION__, mm->message.fuel_tank_volume_validity);
            break;
        }
        case RES_GETASSISTANCESERVICES: {
            Message<ResGetAssistanceServices> *mm = (Message<ResGetAssistanceServices>*)m;
            LOG(LOG_DEBUG, "Common: %s:         assistance_services: %d", __FUNCTION__, mm->message.assistance_services);
            LOG(LOG_DEBUG, "Common: %s:         assistance_services_validity: %d", __FUNCTION__, mm->message.assistance_services_validity);
            break;
        }
        case RES_GETAUTONOMOUSDRIVE: {
            Message<ResGetAutonomousDrive> *mm = (Message<ResGetAutonomousDrive>*)m;
            LOG(LOG_DEBUG, "Common: %s:         autonomous_drive: %d", __FUNCTION__, mm->message.autonomous_drive);
            LOG(LOG_DEBUG, "Common: %s:         autonomous_drive_validity: %d", __FUNCTION__, mm->message.autonomous_drive_validity);
            break;
        }
        case RES_GETCOMBINEDINSTRUMENT: {
            Message<ResGetCombinedInstrument> *mm = (Message<ResGetCombinedInstrument>*)m;
            LOG(LOG_DEBUG, "Common: %s:         combined_instrument: %d", __FUNCTION__, mm->message.combined_instrument);
            LOG(LOG_DEBUG, "Common: %s:         combined_instrument_validity: %d", __FUNCTION__, mm->message.combined_instrument_validity);
            break;
        }
        case RES_GETTELEMATICMODULE: {
            Message<ResGetTelematicModule> *mm = (Message<ResGetTelematicModule>*)m;
            LOG(LOG_DEBUG, "Common: %s:         telematic_module: %d", __FUNCTION__, mm->message.telematic_module);
            LOG(LOG_DEBUG, "Common: %s:         telematic_module_validity: %d", __FUNCTION__, mm->message.telematic_module_validity);
            break;
        }
        case RES_GETCONNECTIVITY: {
            Message<ResGetConnectivity> *mm = (Message<ResGetConnectivity>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connectivity: %d", __FUNCTION__, mm->message.connectivity);
            LOG(LOG_DEBUG, "Common: %s:         connectivity_validity: %d", __FUNCTION__, mm->message.connectivity_validity);
            break;
        }
        case RES_GETCONNECTEDSERVICEBOOKING: {
            Message<ResGetConnectedServiceBooking> *mm = (Message<ResGetConnectedServiceBooking>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connected_service_booking: %d", __FUNCTION__, mm->message.connected_service_booking);
            LOG(LOG_DEBUG, "Common: %s:         connected_service_booking_validity: %d", __FUNCTION__, mm->message.connected_service_booking_validity);
            break;
        }
        case RES_GETFBLSWBUILD: {
            Message<ResGetFblSwBuild> *mm = (Message<ResGetFblSwBuild>*)m;
            LOG(LOG_DEBUG, "Common: %s:         year: 0x%04X", __FUNCTION__, mm->message.year);
            LOG(LOG_DEBUG, "Common: %s:         month: 0x%02hhX", __FUNCTION__, mm->message.month);
            LOG(LOG_DEBUG, "Common: %s:         day: 0x%02hhX", __FUNCTION__, mm->message.day);
            LOG(LOG_DEBUG, "Common: %s:         hour: 0x%02hhX", __FUNCTION__, mm->message.hour);
            LOG(LOG_DEBUG, "Common: %s:         min: 0x%02hhX", __FUNCTION__, mm->message.min);
            LOG(LOG_DEBUG, "Common: %s:         sec: 0x%02hhX", __FUNCTION__, mm->message.sec);
            LOG(LOG_DEBUG, "Common: %s:         checksum: 0x%04X", __FUNCTION__, mm->message.checksum);
            LOG(LOG_DEBUG, "Common: %s:         sw_version: 0x%04X", __FUNCTION__, mm->message.sw_version);
            break;
        }
        case RES_GETCARCFGTHEFTNOTIFICATION: {
            Message<ResGetTheftNotification> *mm = (Message<ResGetTheftNotification>*)m;
            LOG(LOG_DEBUG, "Common: %s:         theft_notification: %d",__FUNCTION__, mm->message.theft_notification);
            LOG(LOG_DEBUG, "Common: %s:         theft_notification_validity: %d",__FUNCTION__, mm->message.theft_notification_validity);
            break;
        }
        case RES_GETCARCFGSTOLENVEHICLETRACKING: {
            Message<ResGetStolenVehicleTracking> *mm = (Message<ResGetStolenVehicleTracking>*)m;
            LOG(LOG_DEBUG, "Common: %s:         stolen_vehicle_tracking: %d",__FUNCTION__, mm->message.stolen_vehicle_tracking);
            LOG(LOG_DEBUG, "Common: %s:         stolen_vehicle_tracking_validity: %d",__FUNCTION__, mm->message.stolen_vehicle_tracking_validity);
            break;
        }
        case RES_GETGNSSRECEIVER: {
            Message<ResGetGnss> *mm = (Message<ResGetGnss>*)m;
            LOG(LOG_DEBUG, "Common: %s:         gnss_receiver: %d",__FUNCTION__, mm->message.gnss_receiver);
            LOG(LOG_DEBUG, "Common: %s:         gnss_receiver_validity: %d",__FUNCTION__, mm->message.gnss_receiver_validity);
            break;
        }
        case RES_GETAPPROVEDCELLULARSTANDARDS: {
            Message<ResGetApprovedCellularStandards> *mm = (Message<ResGetApprovedCellularStandards>*)m;
            LOG(LOG_DEBUG, "Common: %s:         approved_cellular_standards: %d", __FUNCTION__, mm->message.approvedcellularstandards);
            LOG(LOG_DEBUG, "Common: %s:         approved_cellular_standards_validity: %d", __FUNCTION__, mm->message.approvedcellularstandards_validity);
            break;
        }
        case RES_GETDIGITALVIDEORECORDER: {
            Message<ResGetDigitalVideoRecorder> *mm = (Message<ResGetDigitalVideoRecorder>*)m;
            LOG(LOG_DEBUG, "Common: %s:         digital_video_recorder: %d", __FUNCTION__, mm->message.digital_video_recorder);
            LOG(LOG_DEBUG, "Common: %s:         digital_video_recorder_validity: %d", __FUNCTION__, mm->message.digital_video_recorder_validity);
            break;
        }
        case RES_GETDRIVERALERTCONTROL: {
            Message<ResGetDriverAlertControl> *mm = (Message<ResGetDriverAlertControl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         driver_alert_control: %d", __FUNCTION__, mm->message.driver_alert_control);
            LOG(LOG_DEBUG, "Common: %s:         driver_alert_control_validity: %d", __FUNCTION__, mm->message.driver_alert_control_validity);
            break;
        }
        case RES_GETDOORSTATE: {
            Message<ResGetDoorState> *mm = (Message<ResGetDoorState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         driverDoorState: %d", __FUNCTION__, mm->message.driverDoorState);
            LOG(LOG_DEBUG, "Common: %s:         driverSideRearDoorState: %d", __FUNCTION__, mm->message.driverSideRearDoorState);
            LOG(LOG_DEBUG, "Common: %s:         driverDoorLockState: %d", __FUNCTION__, mm->message.driverDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         driverSideRearDoorLockState: %d", __FUNCTION__, mm->message.driverSideRearDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         passengerDoorState: %d", __FUNCTION__, mm->message.passengerDoorState);
            LOG(LOG_DEBUG, "Common: %s:         passengerSideRearDoorState: %d", __FUNCTION__, mm->message.passengerSideRearDoorState);
            LOG(LOG_DEBUG, "Common: %s:         passengerDoorLockState: %d", __FUNCTION__, mm->message.passengerDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         passengerSideRearDoorLockState: %d", __FUNCTION__, mm->message.passengerSideRearDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         tailgateOrBootlidState: %d", __FUNCTION__, mm->message.tailgateOrBootlidState);
            LOG(LOG_DEBUG, "Common: %s:         tailgateOrBootlidLockState: %d", __FUNCTION__, mm->message.tailgateOrBootlidLockState);
            LOG(LOG_DEBUG, "Common: %s:         hoodState: %d", __FUNCTION__, mm->message.hoodState);
            LOG(LOG_DEBUG, "Common: %s:         centralLockingStateForUserFeedback: %d", __FUNCTION__, mm->message.centralLockingStateForUserFeedback);
            LOG(LOG_DEBUG, "Common: %s:         centralLockState: %d", __FUNCTION__, mm->message.centralLockState);
            LOG(LOG_DEBUG, "Common: %s:         centralLockStateTrigSource: %d", __FUNCTION__, mm->message.centralLockStateTrigSource);
            LOG(LOG_DEBUG, "Common: %s:         centralLockStateNewEvent: %d", __FUNCTION__, mm->message.centralLockStateNewEvent);
            break;
        }
        // Test
        case RES_VGMTESTMSG: {
            Message<ResVGMTestMsg> *mm = (Message<ResVGMTestMsg>*)m;
            LOG(LOG_DEBUG, "Common: %s:     id: %d", __FUNCTION__, mm->message.id);
            LOG(LOG_DEBUG, "Common: %s:     company: %s", __FUNCTION__, mm->message.company);
            break;
        }
        case RES_IHUTESTMSG: {
            Message<ResIHUTestMsg> *mm = (Message<ResIHUTestMsg>*)m;
            LOG(LOG_DEBUG, "Common: %s:     id: %d", __FUNCTION__, mm->message.id);
            LOG(LOG_DEBUG, "Common: %s:     city: %s", __FUNCTION__, mm->message.city);
            break;
        }

        default:
            LOG(LOG_WARN, "Common: %s: Unknown response-ID (%d)!", __FUNCTION__ , m->id_);
            return RET_ERR_INVALID_ARG;
        }

        break;
    case (MESSAGE_EVENT):
        switch ((EventID)m->id_) {

        // VuC
        case EVENT_HORNNLIGHTSTATE: {
            Message<EventHornNLightState> *mm = (Message<EventHornNLightState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         mode: %d", __FUNCTION__, mm->message.mode);
            LOG(LOG_DEBUG, "Common: %s:         state: %d", __FUNCTION__, mm->message.state);
            break;
        }
        case EVENT_DIAGERRREPORT: {
            Message<EventDiagErrReport> *mm = (Message<EventDiagErrReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         phone_ant_open_load_test_status: %d",__FUNCTION__, mm->message.phone_ant_open_load_test_status);
            LOG(LOG_DEBUG, "Common: %s:         phone_ant_open_load_test_state: %d",__FUNCTION__, mm->message.phone_ant_open_load_test_state);
            LOG(LOG_DEBUG, "Common: %s:         ecall_ant_open_load_test_status: %d",__FUNCTION__, mm->message.ecall_ant_open_load_test_status);
            LOG(LOG_DEBUG, "Common: %s:         ecall_ant_open_load_test_state: %d",__FUNCTION__, mm->message.ecall_ant_open_load_test_state);
            break;
        }
        case EVENT_CRASHSTATE: {
            Message<EventCrashState> *mm = (Message<EventCrashState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         resume_from_power_loss: %d", __FUNCTION__, mm->message.resume_from_power_loss);
            LOG(LOG_DEBUG, "Common: %s:         state: %d", __FUNCTION__, mm->message.state);
            break;
        }
        case EVENT_BLE_CONNECTIONSTATUS: {
            Message<EventBleConnStatus> *mm = (Message<EventBleConnStatus>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connectiod_id: %d", __FUNCTION__, mm->message.connection_id);
            LOG(LOG_DEBUG, "Common: %s:         state: %d", __FUNCTION__, mm->message.state);
            break;
        }
        case EVENT_BLE_RESET_NOTIFICATION: {
            break;
        }
        case EVENT_BLE_MTUSIZE: {
            Message<EventBleMtuSize> *mm = (Message<EventBleMtuSize>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connection_id: %d", __FUNCTION__, mm->message.connection_id);
            LOG(LOG_DEBUG, "Common: %s:         mtu_size: %d", __FUNCTION__, mm->message.mtu_size);
            break;
        }
        case EVENT_DTCFULLREPORT: {
            Message<EventDtcFullReport> *mm = (Message<EventDtcFullReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         event_id: %d", __FUNCTION__, mm->message.event_id);
            LOG(LOG_DEBUG, "Common: %s:         no_of_events: %d", __FUNCTION__, mm->message.no_of_events);
            LOG(LOG_DEBUG, "Common: %s:         hexcode[0-2]: %x %x %x", __FUNCTION__, mm->message.hexcode[0],
                    mm->message.hexcode[1], mm->message.hexcode[2]);
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            LOG(LOG_DEBUG, "Common: %s:         ext_data10: %d", __FUNCTION__, mm->message.ext_data10);
            LOG(LOG_DEBUG, "Common: %s:         ext_data01: %d", __FUNCTION__, mm->message.ext_data01);
            LOG(LOG_DEBUG, "Common: %s:         ext_data02: %d", __FUNCTION__, mm->message.ext_data02);
            LOG(LOG_DEBUG, "Common: %s:         ext_data03: %d", __FUNCTION__, mm->message.ext_data03);
            LOG(LOG_DEBUG, "Common: %s:         ext_data04: %d", __FUNCTION__, mm->message.ext_data04);
            LOG(LOG_DEBUG, "Common: %s:         ext_data06: %d", __FUNCTION__, mm->message.ext_data06);
            LOG(LOG_DEBUG, "Common: %s:         ext_data07: %d", __FUNCTION__, mm->message.ext_data07);
            LOG(LOG_DEBUG, "Common: %s:         ext_data11: %d", __FUNCTION__, mm->message.ext_data11);
            LOG(LOG_DEBUG, "Common: %s:         ext_data12: %d", __FUNCTION__, mm->message.ext_data12);
            LOG(LOG_DEBUG, "Common: %s:         ext_data20[0-3]: %d %d %d %d", __FUNCTION__, mm->message.ext_data20[0],
                    mm->message.ext_data20[1], mm->message.ext_data20[2], mm->message.ext_data20[3]);
            LOG(LOG_DEBUG, "Common: %s:         ext_data21[0-3]: %d %d %d %d", __FUNCTION__, mm->message.ext_data21[0],
                    mm->message.ext_data21[1], mm->message.ext_data21[2], mm->message.ext_data21[3]);
            LOG(LOG_DEBUG, "Common: %s:         ext_data30: %d", __FUNCTION__, mm->message.ext_data30);
            for (uint8_t i = 0 ; i < sizeof(mm->message.snapshot_data20); i++) {
                LOG(LOG_DEBUG, "Common: %s:         snapshot_data20[%d]: %d", __FUNCTION__, i, mm->message.snapshot_data20[i]);
            }
            break;
        }
        case EVENT_DTCSTATUSREPORT: {
            Message<EventDtcStatusReport> *mm = (Message<EventDtcStatusReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         event_id: %d", __FUNCTION__, mm->message.event_id);
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            LOG(LOG_DEBUG, "Common: %s:         ext_data10: %d", __FUNCTION__, mm->message.ext_data10);
            LOG(LOG_DEBUG, "Common: %s:         ext_data11: %d", __FUNCTION__, mm->message.ext_data11);
            LOG(LOG_DEBUG, "Common: %s:         ext_data12: %d", __FUNCTION__, mm->message.ext_data12);
            LOG(LOG_DEBUG, "Common: %s:         ext_data30: %d", __FUNCTION__, mm->message.ext_data30);
            break;
        }
        case EVENT_AMPLIFIERSTATE: {
            Message<EventAmplifierState> *mm = (Message<EventAmplifierState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         state: %d", __FUNCTION__, mm->message.state);
            LOG(LOG_DEBUG, "Common: %s:         status: %d", __FUNCTION__, mm->message.status);
            break;
        }
        case EVENT_RFKLINEERRREPORT: {
            Message<EventRfKlineErrReport> *mm = (Message<EventRfKlineErrReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         rf_error_code: %d", __FUNCTION__, mm->message.rf_error_code);
            LOG(LOG_DEBUG, "Common: %s:         kline_error_code: %d", __FUNCTION__, mm->message.kline_error_code);
            break;
        }
        case EVENT_LASTBUBSTATE: {
            Message<EventLastBubState> *mm = (Message<EventLastBubState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         battery_used: %d", __FUNCTION__, mm->message.battery_used);
            LOG(LOG_DEBUG, "Common: %s:         battery_state: %d", __FUNCTION__, mm->message.battery_state);
            LOG(LOG_DEBUG, "Common: %s:         charge_level: %d", __FUNCTION__, mm->message.charge_level);
            LOG(LOG_DEBUG, "Common: %s:         backup_battery_temp: %d", __FUNCTION__, mm->message.backup_battery_temp);
            LOG(LOG_DEBUG, "Common: %s:         ambient_temp: %d", __FUNCTION__, mm->message.ambient_temp);
            LOG(LOG_DEBUG, "Common: %s:         battery_voltage: %.2f", __FUNCTION__, mm->message.battery_voltage);
            LOG(LOG_DEBUG, "Common: %s:         kl30_voltage: %.2f", __FUNCTION__, mm->message.kl30_voltage);
            break;
        }
        case EVENT_OHCBTNSTATE: {
            Message<EventOhcBtnState> *mm = (Message<EventOhcBtnState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         ecall_btn: %d", __FUNCTION__, mm->message.ecall_btn);
            LOG(LOG_DEBUG, "Common: %s:         icall_btn: %d", __FUNCTION__, mm->message.icall_btn);
            break;
        }
        case EVENT_VEHICLESPEEDSTATE: {
            Message<EventVehicleSpeedState> *mm = (Message<EventVehicleSpeedState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         vehicle_speed_below_v_scan_full: %d", __FUNCTION__, mm->message.vehicle_speed_below_v_scan_full);
            LOG(LOG_DEBUG, "Common: %s:         vehicle_speed_below_v_scan_limited: %d", __FUNCTION__, mm->message.vehicle_speed_below_v_scan_limited);
            break;
        }
        case EVENT_CANVOLTAGEERRREPORT: {
            Message<EventCanVoltageErrReport> *mm = (Message<EventCanVoltageErrReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         can_under_voltage: %d", __FUNCTION__, mm->message.can_under_voltage);
            LOG(LOG_DEBUG, "Common: %s:         kl30_under_voltage: %d", __FUNCTION__, mm->message.kl30_under_voltage);
            LOG(LOG_DEBUG, "Common: %s:         kl30_over_voltage: %d", __FUNCTION__, mm->message.kl30_over_voltage);
            break;
        }
        case EVENT_DIAGCOUPLEREPORT: {
            Message<EventDiagCoupleReport> *mm = (Message<EventDiagCoupleReport>*)m;
            LOG(LOG_DEBUG, "Common: %s:         req: %d", __FUNCTION__, mm->message.req);
            break;
        }
        case EVENT_NUMBEROFDOORS: {
            Message<EventNumberOfDoors> *mm = (Message<EventNumberOfDoors>*)m;
            LOG(LOG_DEBUG, "Common: %s:         nbr_of_doors: %d", __FUNCTION__, mm->message.nbr_of_doors);
            LOG(LOG_DEBUG, "Common: %s:         nbr_of_doors_validity: %d", __FUNCTION__, mm->message.nbr_of_doors_validity);
            break;
        }
        case EVENT_FUELTYPE: {
            Message<EventFuelType> *mm = (Message<EventFuelType>*)m;
            LOG(LOG_DEBUG, "Common: %s:         fuel_type: %d", __FUNCTION__, mm->message.fuel_type);
            LOG(LOG_DEBUG, "Common: %s:         fuel_type_validity: %d", __FUNCTION__, mm->message.fuel_type_validity);
            break;
        }
        case EVENT_STEERINGWHEELPOSITION: {
            Message<EventSteeringWheelPosition> *mm = (Message<EventSteeringWheelPosition>*)m;
            LOG(LOG_DEBUG, "Common: %s:         steering_wheel_position: %d", __FUNCTION__, mm->message.steering_wheel_position);
            LOG(LOG_DEBUG, "Common: %s:         steering_wheel_position_validity: %d", __FUNCTION__, mm->message.steering_wheel_position_validity);
            break;
        }
        case EVENT_FUELTANKVOLUME: {
            Message<EventFuelTankVolume> *mm = (Message<EventFuelTankVolume>*)m;
            LOG(LOG_DEBUG, "Common: %s:         fuel_tank_volume: %d", __FUNCTION__, mm->message.fuel_tank_volume);
            LOG(LOG_DEBUG, "Common: %s:         fuel_tank_volume_validity: %d", __FUNCTION__, mm->message.fuel_tank_volume_validity);
            break;
        }
        case EVENT_PROPULSIONTYPE: {
            Message<EventPropulsionType> *mm = (Message<EventPropulsionType>*)m;
            LOG(LOG_DEBUG, "Common: %s:         propulsion_type: %d", __FUNCTION__, mm->message.propulsion_type);
            LOG(LOG_DEBUG, "Common: %s:         propulsion_type_validity: %d", __FUNCTION__, mm->message.propulsion_type_validity);
            break;
        }
        case EVENT_ASSISTANCESERVICES: {
            Message<EventAssistanceServices> *mm = (Message<EventAssistanceServices>*)m;
            LOG(LOG_DEBUG, "Common: %s:         assistance_services: %d", __FUNCTION__, mm->message.assistance_services);
            LOG(LOG_DEBUG, "Common: %s:         assistance_services_validity: %d", __FUNCTION__, mm->message.assistance_services_validity);
            break;
        }
        case EVENT_AUTONOMOUSDRIVE: {
            Message<EventAutonomousDrive> *mm = (Message<EventAutonomousDrive>*)m;
            LOG(LOG_DEBUG, "Common: %s:         autonomous_drive: %d", __FUNCTION__, mm->message.autonomous_drive);
            LOG(LOG_DEBUG, "Common: %s:         autonomous_drive_validity: %d", __FUNCTION__, mm->message.autonomous_drive_validity);
            break;
        }
        case EVENT_COMBINEDINSTRUMENT: {
            Message<EventCombinedInstrument> *mm = (Message<EventCombinedInstrument>*)m;
            LOG(LOG_DEBUG, "Common: %s:         combined_instrument: %d", __FUNCTION__, mm->message.combined_instrument);
            LOG(LOG_DEBUG, "Common: %s:         combined_instrument_validity: %d", __FUNCTION__, mm->message.combined_instrument_validity);
            break;
        }
        case EVENT_TELEMATICMODULE: {
            Message<EventTelematicModule> *mm = (Message<EventTelematicModule>*)m;
            LOG(LOG_DEBUG, "Common: %s:         telematic_module: %d", __FUNCTION__, mm->message.telematic_module);
            LOG(LOG_DEBUG, "Common: %s:         telematic_module_validity: %d", __FUNCTION__, mm->message.telematic_module_validity);
            break;
        }
        case EVENT_CONNECTIVITY: {
            Message<EventConnectivity> *mm = (Message<EventConnectivity>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connectivity: %d", __FUNCTION__, mm->message.connectivity);
            LOG(LOG_DEBUG, "Common: %s:         connectivity_validity: %d", __FUNCTION__, mm->message.connectivity_validity);
            break;
        }
        case EVENT_CONNECTEDSERVICEBOOKING: {
            Message<EventConnectedServiceBooking> *mm = (Message<EventConnectedServiceBooking>*)m;
            LOG(LOG_DEBUG, "Common: %s:         connected_service_booking: %d", __FUNCTION__, mm->message.connected_service_booking);
            LOG(LOG_DEBUG, "Common: %s:         connected_service_booking_validity: %d", __FUNCTION__, mm->message.connected_service_booking_validity);
            break;
        }
        case EVENT_PSIMAVAILABILITY: {
            Message<EventPSIMAvailability> *mm = (Message<EventPSIMAvailability>*)m;
            LOG(LOG_DEBUG, "Common: %s:         psim_availability: %d", __FUNCTION__, mm->message.psim_availability);
            LOG(LOG_DEBUG, "Common: %s:         psim_availability_validity: %d", __FUNCTION__, mm->message.psim_availability_validity);
            break;
        }
        case EVENT_WIFIFUNCTIONALITYCONTROL: {
            Message<EventWifiFunctionalityControl> *mm = (Message<EventWifiFunctionalityControl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         wifi_functionality_control: %d", __FUNCTION__, mm->message.wifi_functionality_control);
            LOG(LOG_DEBUG, "Common: %s:         wifi_functionality_control_validity: %d", __FUNCTION__, mm->message.wifi_functionality_control_validity);
            break;
        }
        case EVENT_WIFIFREQUENCYBAND: {
            Message<EventWifiFrequencyBand> *mm = (Message<EventWifiFrequencyBand>*)m;
            LOG(LOG_DEBUG, "Common: %s:         wifi_frequency_band: %d", __FUNCTION__, mm->message.wifi_frequency_band);
            LOG(LOG_DEBUG, "Common: %s:         wifi_frequency_band_validity: %d", __FUNCTION__, mm->message.wifi_frequency_band_validity);
            break;
        }
        case EVENT_WIFIWORKSHOPMODE: {
            Message<EventWifiWorkshopMode> *mm = (Message<EventWifiWorkshopMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         wifi_workshop_mode: %d", __FUNCTION__, mm->message.wifi_workshop_mode);
            LOG(LOG_DEBUG, "Common: %s:         wifi_workshop_mode_validity: %d", __FUNCTION__, mm->message.wifi_workshop_mode_validity);
            break;
        }
        case EVENT_DIGITALVIDEORECORDER: {
            Message<EventDigitalVideoRecorder> *mm = (Message<EventDigitalVideoRecorder>*)m;
            LOG(LOG_DEBUG, "Common: %s:         digital_video_recorder: %d", __FUNCTION__, mm->message.digital_video_recorder);
            LOG(LOG_DEBUG, "Common: %s:         digital_video_recorder_validity: %d", __FUNCTION__, mm->message.digital_video_recorder_validity);
            break;
        }
        case EVENT_DRIVERALERTCONTROL: {
            Message<EventDriverAlertControl> *mm = (Message<EventDriverAlertControl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         driver_alert_control: %d", __FUNCTION__, mm->message.driver_alert_control);
            LOG(LOG_DEBUG, "Common: %s:         driver_alert_control_validity: %d", __FUNCTION__, mm->message.driver_alert_control_validity);
            break;
        }
        // VGM
        case EVENT_POSNFROMSATLTCON: {
            Message<EventPosnFromSatltCon> *mm = (Message<EventPosnFromSatltCon>*)m;
            LOG(LOG_DEBUG, "Common: %s:         Latitude: %d",__FUNCTION__, mm->message.PosnLat);
            LOG(LOG_DEBUG, "Common: %s:         Longitude: %d",__FUNCTION__, mm->message.PosnLgt);
            LOG(LOG_DEBUG, "Common: %s:         Altitide from mean sea level: %d",__FUNCTION__, mm->message.PosnAlti);
            LOG(LOG_DEBUG, "Common: %s:         Speed: %d",__FUNCTION__, mm->message.PosnSpd);
            LOG(LOG_DEBUG, "Common: %s:         Horizontal velocity: %d",__FUNCTION__, mm->message.PosnVHozl);
            LOG(LOG_DEBUG, "Common: %s:         Vertical velocity: %d",__FUNCTION__, mm->message.PosnVVert);
            LOG(LOG_DEBUG, "Common: %s:         Heading from true north: %d",__FUNCTION__, mm->message.PosnDir);
            LOG(LOG_DEBUG, "Common: %s:         UTC year: %d",__FUNCTION__, mm->message.TiForYr);
            LOG(LOG_DEBUG, "Common: %s:         UTC month: %d",__FUNCTION__, mm->message.TiForMth);
            LOG(LOG_DEBUG, "Common: %s:         UTC day: %d",__FUNCTION__, mm->message.TiForDay);
            LOG(LOG_DEBUG, "Common: %s:         UTC hour: %d",__FUNCTION__, mm->message.TiForHr);
            LOG(LOG_DEBUG, "Common: %s:         UTC minute: %d",__FUNCTION__, mm->message.TiForMins);
            LOG(LOG_DEBUG, "Common: %s:         UTC second: %d",__FUNCTION__, mm->message.TiForSec);
            LOG(LOG_DEBUG, "Common: %s:         GPS used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo1InUse);
            LOG(LOG_DEBUG, "Common: %s:         GLONASS used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo2InUse);
            LOG(LOG_DEBUG, "Common: %s:         Galileo used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo3InUse);
            LOG(LOG_DEBUG, "Common: %s:         SBAS used for positioning	Enum: %d",__FUNCTION__, mm->message.SatltSysNo4InUse);
            LOG(LOG_DEBUG, "Common: %s:         QZSS L1 used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo5InUse);
            LOG(LOG_DEBUG, "Common: %s:         QZSS L1-SAIF used for positioning: %d",__FUNCTION__, mm->message.SatltSysNo6InUse);
            LOG(LOG_DEBUG, "Common: %s:         Navigation solution status: %d",__FUNCTION__, mm->message.SatltPosnStsPrm1);
            LOG(LOG_DEBUG, "Common: %s:         DGPS usage in solution: %d",__FUNCTION__, mm->message.SatltPosnStsPrm2);
            LOG(LOG_DEBUG, "Common: %s:         Self ephemeris data usage in solution: %d",__FUNCTION__, mm->message.SatltPosnStsPrm3);
            LOG(LOG_DEBUG, "Common: %s:         Number of GPS satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo1);
            LOG(LOG_DEBUG, "Common: %s:         Number of GLONASS satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo2);
            LOG(LOG_DEBUG, "Common: %s:         Number of Galileo satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo3);
            LOG(LOG_DEBUG, "Common: %s:         Number of SBAS satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo4);
            LOG(LOG_DEBUG, "Common: %s:         Number of QZSS L1 satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo5);
            LOG(LOG_DEBUG, "Common: %s:         Number of QZSS L1-SAIF satellites used for positioning: %d",__FUNCTION__, mm->message.NoOfSatltForSysNo6);
            LOG(LOG_DEBUG, "Common: %s:         Position Dilution Of Precision: %d",__FUNCTION__, mm->message.PrePosnDil);
            LOG(LOG_DEBUG, "Common: %s:         Horizontal Dilution Of Precision: %d",__FUNCTION__, mm->message.PreHozlDil);
            LOG(LOG_DEBUG, "Common: %s:         Vertical Dilution Of Precision: %d",__FUNCTION__, mm->message.PreVertDil);
            LOG(LOG_DEBUG, "Common: %s:         Time Dilution Of Precision: %d",__FUNCTION__, mm->message.PreTiDil);
            break;
        }
        // Data-storage
        case EVENT_DSPARAMETER_STR: {
            Message<EventDSParameterStr> *mm = (Message<EventDSParameterStr>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:         value: %s", __FUNCTION__, mm->message.value);
            break;
        }
        case EVENT_DSPARAMETER_INT: {
            Message<EventDSParameterInt> *mm = (Message<EventDSParameterInt>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection:: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:         value: %d", __FUNCTION__, mm->message.value);
            break;
        }
        case EVENT_DSPARAMETER_VECT: {
            Message<EventDSParameterVect> *mm = (Message<EventDSParameterVect>*)m;
            LOG(LOG_DEBUG, "Common: %s:         collection: %s", __FUNCTION__, mm->message.collection);
            LOG(LOG_DEBUG, "Common: %s:         name: %s", __FUNCTION__, mm->message.name);
            LOG(LOG_DEBUG, "Common: %s:         size: %d", __FUNCTION__, mm->message.size);
            for (uint8_t i = 0; i < mm->message.size; i++) {
                LOG(LOG_DEBUG, "Common: %s:         value[%d]: 0x%x", __FUNCTION__, i, mm->message.value);
            }
            break;
        }
        case EVENT_CARMODE: {
            Message<EventCarMode> *mm = (Message<EventCarMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         carmode: %d", __FUNCTION__, mm->message.carmode);
            break;
        }
        case EVENT_CARUSAGEMODE: {
            Message<EventCarUsageMode> *mm = (Message<EventCarUsageMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         usagemode: %d", __FUNCTION__, mm->message.usagemode);
            break;
        }
        case EVENT_DOORLOCKSTATE: {
            Message<EventDoorLockState> *mm = (Message<EventDoorLockState>*)m;
            LOG(LOG_DEBUG, "Common: %s:         driverDoorState: %d", __FUNCTION__, mm->message.driverDoorState);
            LOG(LOG_DEBUG, "Common: %s:         driverSideRearDoorState: %d", __FUNCTION__, mm->message.driverSideRearDoorState);
            LOG(LOG_DEBUG, "Common: %s:         driverDoorLockState: %d", __FUNCTION__, mm->message.driverDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         driverSideRearDoorLockState: %d", __FUNCTION__, mm->message.driverSideRearDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         passengerDoorState: %d", __FUNCTION__, mm->message.passengerDoorState);
            LOG(LOG_DEBUG, "Common: %s:         passengerSideRearDoorState: %d", __FUNCTION__, mm->message.passengerSideRearDoorState);
            LOG(LOG_DEBUG, "Common: %s:         passengerDoorLockState: %d", __FUNCTION__, mm->message.passengerDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         passengerSideRearDoorLockState: %d", __FUNCTION__, mm->message.passengerSideRearDoorLockState);
            LOG(LOG_DEBUG, "Common: %s:         tailgateOrBootlidState: %d", __FUNCTION__, mm->message.tailgateOrBootlidState);
            LOG(LOG_DEBUG, "Common: %s:         tailgateOrBootlidLockState: %d", __FUNCTION__, mm->message.tailgateOrBootlidLockState);
            LOG(LOG_DEBUG, "Common: %s:         hoodState: %d", __FUNCTION__, mm->message.hoodState);
            LOG(LOG_DEBUG, "Common: %s:         centralLockingStateForUserFeedback: %d", __FUNCTION__, mm->message.centralLockingStateForUserFeedback);
            LOG(LOG_DEBUG, "Common: %s:         centralLockState: %d", __FUNCTION__, mm->message.centralLockState);
            LOG(LOG_DEBUG, "Common: %s:         centralLockStateTrigSource: %d", __FUNCTION__, mm->message.centralLockStateTrigSource);
            LOG(LOG_DEBUG, "Common: %s:         centralLockStateNewEvent: %d", __FUNCTION__, mm->message.centralLockStateNewEvent);
            break;
        }
        case EVENT_ELECTENGLVL: {
            Message<EventElectEngLvl> *mm = (Message<EventElectEngLvl>*)m;
            LOG(LOG_DEBUG, "Common: %s:         level: %d", __FUNCTION__, mm->message.level);
            break;
        }
        case EVENT_VUCPOWERMODE: {
            Message<EventVucPowerMode> *mm = (Message<EventVucPowerMode>*)m;
            LOG(LOG_DEBUG, "Common: %s:         power_mode: %d", __FUNCTION__, mm->message.power_mode);
            break;
        }
        case EVENT_VUCWAKEUPREASON: {
            Message<EventVucWakeupReason> *mm = (Message<EventVucWakeupReason>*)m;
            LOG(LOG_DEBUG, "Common: %s:         wakeup_reason: %d", __FUNCTION__, mm->message.wakeup_reason);
            break;
        }
        case EVENT_VINNUMBER: {
            Message<EventVINNumber> *mm = (Message<EventVINNumber>*)m;
            LOG(LOG_DEBUG, "Common: %s:         vin_number: %s", __FUNCTION__, mm->message.vin_number);
            break;
        }
        // Message-processor
        case EVENT_ENDOFTRIP:
            break;
        // Test
        case EVENT_TESTFROMVGM: {
            Message<EventTestFromVGM> *mm = (Message<EventTestFromVGM>*)m;
            LOG(LOG_DEBUG, "Common: %s:     vgm: %d", __FUNCTION__, mm->message.vgm);
            break;
        }
        case EVENT_TESTFROMIHU: {
            Message<EventTestFromIHU> *mm = (Message<EventTestFromIHU>*)m;
            LOG(LOG_DEBUG, "Common: %s:     ihu: %d", __FUNCTION__, mm->message.ihu);
            break;
        }
        case EVENT_TESTFROMVC: {
            Message<EventTestFromVC> *mm = (Message<EventTestFromVC>*)m;
            LOG(LOG_DEBUG, "Common: %s:     vc: %d", __FUNCTION__, mm->message.vc);
            break;
        }

        default:
            LOG(LOG_WARN, "Common: %s: Unknown event-ID (%d)!", __FUNCTION__, m->id_);
            return RET_ERR_INVALID_ARG;
        }
        break;
    case (MESSAGE_ERROR):
        LOG(LOG_DEBUG, "Common: %s :     error-ID: (%d) (%s)", __FUNCTION__, m->id_, ReturnValueStr[m->id_]);
        break;
    default:
        LOG(LOG_WARN, "Common: %s: Unknown message-ID (%d)!", __FUNCTION__ , m->id_);
        return RET_ERR_INVALID_ARG;
    }
    return RET_OK;
}

/**
    @brief Generates an error message.

    This method will instantiate an error-message based
    on a request and the error message that was returned
    trying to send it. It will put it to the queue and send
    it to the requester.

    @param[in]  m_req          Outgoing request that failed
    @param[in]  error          Error code for failure
*/
void SendErrorMessage(MessageQueue *mq, MessageBase *m_req, ReturnValue error)
{
    ReturnValue ret;
    if (m_req == NULL) {
        LOG(LOG_WARN, "VehicleComm: %s: m is NULL! Cannot respond.", __FUNCTION__);
        return;
    }
    if (error == RET_OK) {
        LOG(LOG_INFO, "VehicleComm: %s: There is no error to send!", __FUNCTION__);
        return;
    }
    if (m_req->type_ != MESSAGE_REQUEST) {
        LOG(LOG_WARN, "VehicleComm: %s: Message is not a request!", __FUNCTION__);
        return;
    }

    MessageBase *m_err = InitMessage((MessageID)error, MESSAGE_ERROR, 0, m_req->session_id_,
                    m_req->dst_, m_req->src_, 0);
    if(m_err == NULL) {
        LOG(LOG_WARN, "VehicleComm: %s: Failed to create error-message!", __FUNCTION__);
        return;
    }

    ret = mq->AddMessage(m_err);
    if (ret != RET_OK) {
        LOG(LOG_WARN, "VehicleComm: %s: Failed to send error-message (err = %s)!", __FUNCTION__,
                ReturnValueStr[ret]);
        return;
    }
}

/**
    @brief Send reponse function for incoming requests
    @param[in]  m_req     Request to generate response from
    @param[in]  data    Payload for the response
    @return     A VC return value
*/
ReturnValue SendResponseMessage(MessageQueue *mq, MessageBase *m_req, const unsigned char *data)
{
    if (m_req == NULL) {
        LOG(LOG_WARN, "VehicleComm: %s: m is NULL! Cannot respond.", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }
    if (m_req->type_ != MESSAGE_REQUEST) {
        LOG(LOG_WARN, "VehicleComm: %s: Message is not a request!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    ResponseID response_id = ResponseLookup((RequestID) m_req->id_);
    if (response_id == 0) {
        LOG(LOG_INFO, "MessageProcessor: %s: Unknown message (message_id = %d)!", __FUNCTION__, m_req->id_);
        return RET_ERR_INTERNAL;
    }

    MessageBase *m = InitMessage((MessageID)response_id, MESSAGE_RESPONSE, m_req->client_id_, m_req->session_id_,
                                m_req->dst_, m_req->src_, data);

    if (m == NULL) {
        LOG(LOG_WARN, "MessageProcessor: %s InitMessage returned NULL not adding to queue.", __FUNCTION__);
        return RET_ERR_INTERNAL;
    } else
        (void)mq->AddMessage(m);

    return RET_OK;
}

bool HasGenericResponse(MessageBase *m)
{
    return ((m->type_ == MESSAGE_REQUEST) && (ResponseLookup((RequestID)m->id_) == RES_REQUESTSENT));
}

}// namespace vc
