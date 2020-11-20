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

/** @file vuc_comm.cpp
 * This file handles the communication with the VuC, using the OTP VCS-interface
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <cstring>
#include <thread>

#include "vuc_trace.hpp"
#include "vuc_comm.hpp"
#include "vc_utility.hpp"
#include "local_config_interface.hpp"

namespace vc {

MessageQueue *VuCComm::mq_;
DataStorage *VuCComm::ds_;
SessionList VuCComm::slist_;

/**
    @brief Find VCRequest from VucMessageID

    @param[in]  request_id     VC request id
    @param[in]  message_id     VuC message id
    @return     A VC return value
*/
ReturnValue VuCComm::VuCRequestLookup(RequestID request_id, VuCMessageID& message_id)
{
    for (int i = 0; i < LENGTH(vuc_request_lookup); i++)
        if (vuc_request_lookup[i].request_id == request_id) {
            message_id = vuc_request_lookup[i].vuc_request_id;
            return RET_OK;
        }

    LOG(LOG_WARN, "VuCComm: %s Unknown request id = %d.", __FUNCTION__, request_id);
    return RET_ERR_INTERNAL;
};

/**
    @brief Find VCResponse from VucMessageID

    @param[in]  response_id    VC response id
    @param[in]  message_id     VuC message id
    @return     A VC return value
*/
ReturnValue VuCComm::VuCResponseLookup(ResponseID response_id, VuCMessageID& message_id)
{
    for (int i = 0; i < LENGTH(vuc_response_lookup); i++)
        if (vuc_response_lookup[i].response_id == response_id) {
            message_id = vuc_response_lookup[i].vuc_response_id;
            return RET_OK;
        }

    LOG(LOG_WARN, "VuCComm: %s Unknown response id = %d.", __FUNCTION__, response_id);
    return RET_ERR_INTERNAL;
};


/**
    @brief Get a Endpoint from a RequestID.

    This method will go through the entries in the routing-table
    and find the Endpoint for a RequestID.

    @param[in]  request_id     A RequestID to be looked up.
    @return     A VCDestination
*/
Endpoint VuCComm::DestinationLookup(RequestID request_id)
{
    for (int i = 0; i < LENGTH(routing_table_vuc); i++)
        if (routing_table_vuc[i].request_id == request_id)
            return routing_table_vuc[i].dst;

    return ENDPOINT_UNKNOWN;
}

/**
    @brief Called from TCAM-INC when recevied string message from VuC

    @param[in]  signal         Type of signal
    @param[in]  data           Content of signal
*/
void VuCComm::SignalIntCallback(const gchar *signal, const gint data)
{
    LOG(LOG_DEBUG, "VuCComm: %s received signal : %s value: %d", __FUNCTION__, signal, data);

    if(strncmp(signal, VCS_SIGNAL_INT_WAKEUP_REASON_VALUE, sizeof(VCS_SIGNAL_INT_WAKEUP_REASON_VALUE)) == 0) {
        LOG(LOG_DEBUG, "VuCComm: %s (signal: %s  value: %d)", __FUNCTION__, signal, data);
    }
    else {
        LOG(LOG_WARN, "VuCComm: %s received un-handled int signal : %s  data: %s", __FUNCTION__, signal, data);
    }
}

/**
    @brief Called from TCAM-INC when recevied crash message from VuC

    @param[in]  data           Type of crash message
*/
void VuCComm::CrashMessageCallback(const tpVCS_CrashMessage_t data)
{
    LOG(LOG_DEBUG, "VuCComm: %s received CrashMessageCallback event resumeFromPowerLoss: %d, crashData: %d",
                        __FUNCTION__, data.resumeFromPowerLoss, data.crashData);

    EventCrashState event;
    switch (data.crashData) {
    case E_VCS_CRASH_DATA_STATE_LEVEL_A: {
        event.state = CRASH_STATE_LEVEL_A;
        break;
    }
    case E_VCS_CRASH_DATA_STATE_LEVEL_B: {
        event.state = CRASH_STATE_LEVEL_B;
        break;
    }
    case E_VCS_CRASH_DATA_STATE_LEVEL_C: {
        event.state = CRASH_STATE_LEVEL_C;
        break;
    }
    case E_VCS_CRASH_DATA_STATE_LEVEL_D: {
        event.state = CRASH_STATE_LEVEL_D;
        break;
    }
    default:
        LOG(LOG_WARN, "VuCComm: %s received un-handled crash state : %d", __FUNCTION__, data.crashData);
        return;
    }
    event.resume_from_power_loss = data.resumeFromPowerLoss;
    SendEventMessage(EVENT_CRASHSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Called from TCAM-INC when recevied adc response from VuC

    @param[in]  data           Type of adc response message
*/
void VuCComm::AdcResponseCallback(const tpVCS_AdcResponseInformation_t data)
{
    LOG(LOG_DEBUG, "VuCComm: %s received AdcResponseCallback adcid: %d refvalue: %d adcvalue: %d",
                        __FUNCTION__, data.adcId, data.referenceValue, data.adcValue);

    ResAdcValue res;
    res.id =  static_cast<AdcId>(data.adcId);
    res.ref_value = data.referenceValue;
    res.adc_value = data.adcValue;
    FinishRequest(REQ_ADCVALUE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Finish a request in the session-list

    @param[in]  request_id     VC-request to be finished
    @param[in]  return_id      VC-return type to be finished
    @param[in]  data           Payload for response-message
*/
void VuCComm::FinishRequest(RequestID request_id, ReturnValue return_id, const unsigned char *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s: Message: %s.", __FUNCTION__, VCRequestStr[request_id]);

    MessageBase *m_req = slist_.GetRequestMessage(request_id);
    if (!m_req) {
        LOG(LOG_WARN, "VuCComm: %s Request %s not found in session-list!", __FUNCTION__, VCRequestStr[request_id]);
        return;
    }

    ResponseID response_id = ResponseLookup(request_id);
    if (response_id == RES_UNKNOWN) {
        LOG(LOG_WARN, "SessionList: %s: Response-ID not found (request_id = %d)! No response sent.",
                __FUNCTION__, request_id);
        return;
    }

    long session_id = slist_.RemoveSession_RequestID(request_id);

    MessageBase *m = NULL;
    if (return_id == RET_OK) {
        m = InitMessage((MessageID)response_id, MESSAGE_RESPONSE, m_req->client_id_, session_id,
                            ENDPOINT_VUC, m_req->src_, data);
    } else {
        m = InitMessage((MessageID)return_id, MESSAGE_ERROR, m_req->client_id_, session_id,
                            ENDPOINT_VUC, m_req->src_, 0);
    }

    if (m == NULL) {
        LOG(LOG_WARN, "VuCComm: %s InitMessage failed! No response sent.", __FUNCTION__);
        return;
    }

    (void)mq_->AddMessage(m);
}

/**
    @brief Create VC event message

    @param[in]  message_id     VC event id
    @param[in]  data           Payload for response message
    @param[in]  endpoint       Endpoint to sent the event to
*/
void VuCComm::SendEventMessage(EventID message_id, const unsigned char *data, Endpoint endpoint)
{
    LOG(LOG_DEBUG, "VuCComm: Message %s (%d).", EventStr[message_id], message_id);

    MessageBase *m = InitMessage((MessageID)message_id, MESSAGE_EVENT, -1, -1,
                                ENDPOINT_VUC, endpoint, data);
    if(m == NULL) {
        LOG(LOG_WARN, "VuCComm: %s InitMessage returned NULL! Not added to queue.", __FUNCTION__);
        return;
    }

    (void)mq_->AddMessage(m);
}

/**
    @brief Create VC request message

    @param[in]  message_id     VC request id
    @param[in]  data           Payload for response message
    @param[in]  endpoint       Endpoint to sent the event to
*/
void VuCComm::SendRequestMessage(RequestID message_id, const unsigned char *data)
{
    LOG(LOG_DEBUG, "VuCComm: Message %s (%d).", VCRequestStr[message_id], message_id);

    Endpoint dst = DestinationLookup(message_id);
    LOG(LOG_DEBUG, "VuCComm: %s: Message-destination: %s", __FUNCTION__, EndpointStr[dst]);

    MessageBase *m = InitMessage((MessageID)message_id, MESSAGE_REQUEST, -1, -1,
                                ENDPOINT_VUC, dst, data);
    if(m == NULL) {
        LOG(LOG_WARN, "VuCComm: %s InitMessage returned NULL! Not added to queue.", __FUNCTION__);
        return;
    }

    (void)mq_->AddMessage(m);
}

// Handle-functions for individual incoming messages

/**
    @brief Handle incoming horn and light messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvHornAndLight(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucHornULightStateType), data->size, REQ_HORNNLIGHT);
    const VucHornULightStateType *rep = reinterpret_cast<const VucHornULightStateType *>(data->buffer);
    LOG(LOG_DEBUG, "VuCComm %s: rep->respType:%d", __FUNCTION__, rep->respType);
    //Send response if response type is request
    if (rep->respType == 0) {
        ResHornNLight res;
        res.return_code = static_cast<HNLReturnCode>(rep->errCode);
        res.carLocatorSts = rep->carLocatorSts;
        LOG(LOG_DEBUG, "VuCComm %s: errCode:%d, carLocatorSts:%d", __FUNCTION__, res.return_code, res.carLocatorSts);
        FinishRequest(REQ_HORNNLIGHT, RET_OK, reinterpret_cast<const unsigned char *>(&res));
    }

    //Send state event if no error occured
    if (rep->errCode == 0) {
        EventHornNLightState event;
        event.mode = static_cast<HNLMode>(rep->hornUlight);

        // if mode is no request state shall be stoped.
        if (event.mode == 0)
            event.state = HL_ABORTED;
        else
            event.state = static_cast<HNLState>(rep->respType);

        if (!((rep->respType == 1) && (rep->hornUlight == 0)))
            SendEventMessage(EVENT_HORNNLIGHTSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
    }
}

/**
    @brief Handle incoming vehicle gen state report messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvVehicleGenState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    CHECK_SIZE(sizeof(VucVehGenStateReportType), data->size, 0);
    const VucVehGenStateReportType *rep = reinterpret_cast<const VucVehGenStateReportType*>(data->buffer);
    LOG(LOG_DEBUG, "VuCComm: %s:  vehicleModeMngtGlobalSafeUseMode: %d, vehicleModeMngtGlobalSafe: %d, volvoSpecWakeupReason: %d",
                __FUNCTION__, rep->vehicleModeMngtGlobalSafeUseMode,  rep->vehicleModeMngtGlobalSafe, rep->volvoSpecWakeupReason);
    if (ds_->ChangeIntParameter("VehicleState", "vehicleModeMngtGlobalSafeUseMode", rep->vehicleModeMngtGlobalSafeUseMode, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change vehicleModeMngtGlobalSafeUseMode!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VehicleState", "vehicleModeMngtGlobalSafe", rep->vehicleModeMngtGlobalSafe, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change vehicleModeMngtGlobalSafe!", __FUNCTION__);
    }

    std::string strUsageMode;
    std::string strCarMode;
    try {
        strUsageMode = CarUsageModeStateMap.at(static_cast<CarUsageModeState>(rep->vehicleModeMngtGlobalSafeUseMode));
    }
    catch (const std::out_of_range & oor){
        strUsageMode = "Unknown";
    }
    try {
        strCarMode = CarModeStateMap.at(static_cast<CarModeState>(rep->vehicleModeMngtGlobalSafe));
    }
    catch (const std::out_of_range & oor){
        strCarMode = "Unknown";
    }

    LOG(LOG_DEBUG, "VuCComm: %s:  usage_mode: %s, car_mode: %s", __FUNCTION__, strUsageMode.c_str(), strCarMode.c_str());

}

/**
    @brief Handle incoming vehicle gen state response messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResVehicleGenState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    CHECK_SIZE(sizeof(VucVehGenStateRespType), data->size, 0);

    const VucVehGenStateRespType *rep = reinterpret_cast<const VucVehGenStateRespType*>(data->buffer);
    LOG(LOG_DEBUG, "VuCComm: %s:  vehicleModeMngtGlobalSafeUseMode: %d, vehicleModeMngtGlobalSafe: %d, volvoSpecWakeupReason: %d",
                __FUNCTION__, rep->vehicleModeMngtGlobalSafeUseMode,  rep->vehicleModeMngtGlobalSafe, rep->volvoSpecWakeupReason);
    if ((rep->vehicleModeMngtGlobalSafeUseMode > CAR_DRIVING) || (rep->vehicleModeMngtGlobalSafe > CAR_DYNO)) {
        LOG(LOG_WARN, "VuCComm: %s: values not yet available from CAN!", __FUNCTION__);
        FinishRequest(REQ_GETVEHICLESTATE, RET_ERR_VALUE_NOT_AVAILABLE, 0);
        return;
    }
    if (ds_->ChangeIntParameter("VehicleState", "vehicleModeMngtGlobalSafeUseMode", rep->vehicleModeMngtGlobalSafeUseMode, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change vehicleModeMngtGlobalSafeUseMode!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VehicleState", "vehicleModeMngtGlobalSafe", rep->vehicleModeMngtGlobalSafe, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change vehicleModeMngtGlobalSafe!", __FUNCTION__);
    }
    LOG(LOG_DEBUG, "VuCComm: %s:  usage_mode: %d, car_mode: %d",
                __FUNCTION__, rep->vehicleModeMngtGlobalSafeUseMode,
                rep->vehicleModeMngtGlobalSafe);

    std::string strUsageMode;
    std::string strCarMode;
    try {
        strUsageMode = CarUsageModeStateMap.at(static_cast<CarUsageModeState>(rep->vehicleModeMngtGlobalSafeUseMode));
    }
    catch (const std::out_of_range & oor){
        strUsageMode = "Unknown";
    }
    try {
        strCarMode = CarModeStateMap.at(static_cast<CarModeState>(rep->vehicleModeMngtGlobalSafe));
    }
    catch (const std::out_of_range & oor){
        strCarMode = "Unknown";
    }

    LOG(LOG_DEBUG, "VuCComm: %s:  usage_mode: %s, car_mode: %s", __FUNCTION__, strUsageMode.c_str(), strCarMode.c_str());


    ResGetVehicleState res;
    res.carmode = static_cast<CarModeState>(rep->vehicleModeMngtGlobalSafe);
    res.usagemode = static_cast<CarUsageModeState>(rep->vehicleModeMngtGlobalSafeUseMode);
    FinishRequest(REQ_GETVEHICLESTATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle incoming Car-config response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResGetCarConfig(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    CHECK_SIZE(sizeof(VucCarConfRespType), data->size, REQ_GETCARCONFIG);
    const VucCarConfRespType *rep = reinterpret_cast<const VucCarConfRespType*>(data->buffer);
    bool parameter_found = false;

    ResGetCarConfig res;
    res.param_id = rep->paramId;
    res.validity = rep->validity;
    res.value = rep->ccp;

    for (int j = 0; j < LENGTH(car_config_params); j++) {
        if (car_config_params[j].car_config_id == rep->paramId) {
            if (ds_->ChangeIntParameter("CarConfig", car_config_params[j].par_name, rep->ccp | rep->validity << 8, false) != RET_OK) {
                LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, car_config_params[j].par_name.c_str());
                FinishRequest(REQ_GETCARCONFIG, RET_ERR_INTERNAL, reinterpret_cast<const unsigned char *>(&res));
            }
            parameter_found = true;
        }
    }
    if (parameter_found) {
        LOG(LOG_WARN, "VuCComm: %s: paramid: '%d' value: '%d' validity: '%d'!", __FUNCTION__, rep->paramId, rep->ccp, rep->validity);
        FinishRequest(REQ_GETCARCONFIG, RET_OK, reinterpret_cast<const unsigned char *>(&res));
    } else {
        LOG(LOG_WARN, "VuCComm: %s: paramid: '%d' not found!", __FUNCTION__, rep->paramId);
        FinishRequest(REQ_GETCARCONFIG, RET_ERR_VALUE_NOT_AVAILABLE, reinterpret_cast<const unsigned char *>(&res));
    }
}

/**
    @brief Handle incoming Car-config event

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvGetCarConfig(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    CHECK_SIZE(sizeof(VucCarConfigReportRepType), data->size, 0);
    const VucCarConfigReportRepType *rep = reinterpret_cast<const VucCarConfigReportRepType*>(data->buffer);;

    for (int j = 0; j < LENGTH(car_config_params); j++) {
        if (car_config_params[j].car_config_id == rep->paramId) {
            if (ds_->ChangeIntParameter("CarConfig", car_config_params[j].par_name, rep->ccp | rep->validity << 8, false) != RET_OK) {
                LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, car_config_params[j].par_name.c_str());
            }
            switch (car_config_params[j].event_id)
            {
            case EVENT_NUMBEROFDOORS: {
                EventNumberOfDoors event;
                event.nbr_of_doors = static_cast<NbrOfDoors>(rep->ccp);
                event.nbr_of_doors_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_FUELTYPE: {
                EventFuelType event;
                event.fuel_type = static_cast<FuelType>(rep->ccp);
                event.fuel_type_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_STEERINGWHEELPOSITION: {
                EventSteeringWheelPosition event;
                event.steering_wheel_position = static_cast<SteeringWheelPosition>(rep->ccp);
                event.steering_wheel_position_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_FUELTANKVOLUME: {
                EventFuelTankVolume event;
                event.fuel_tank_volume = static_cast<FuelTankVolume>(rep->ccp);
                event.fuel_tank_volume_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_PROPULSIONTYPE: {
                EventPropulsionType event;
                event.propulsion_type = static_cast<PropulsionType>(rep->ccp);
                event.propulsion_type_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_ASSISTANCESERVICES: {
                EventAssistanceServices event;
                event.assistance_services = static_cast<AssistanceServices>(rep->ccp);
                event.assistance_services_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_AUTONOMOUSDRIVE: {
                EventAutonomousDrive event;
                event.autonomous_drive = static_cast<AutonomousDrive>(rep->ccp);
                event.autonomous_drive_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_COMBINEDINSTRUMENT: {
                EventCombinedInstrument event;
                event.combined_instrument = static_cast<CombinedInstrument>(rep->ccp);
                event.combined_instrument_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_TELEMATICMODULE: {
                EventTelematicModule event;
                event.telematic_module = static_cast<TelematicModule>(rep->ccp);
                event.telematic_module_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_CONNECTIVITY: {
                EventConnectivity event;
                event.connectivity = static_cast<Connectivity>(rep->ccp);
                event.connectivity_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_CONNECTEDSERVICEBOOKING: {
                EventConnectedServiceBooking event;
                event.connected_service_booking = static_cast<ConnectedServiceBooking>(rep->ccp);
                event.connected_service_booking_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_PSIMAVAILABILITY: {
                EventPSIMAvailability event;
                event.psim_availability = static_cast<PSIMAvailability>(rep->ccp);
                event.psim_availability_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_WIFIFUNCTIONALITYCONTROL: {
                EventWifiFunctionalityControl event;
                event.wifi_functionality_control = static_cast<WiFiFunctionalityControl>(rep->ccp);
                event.wifi_functionality_control_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_WIFIFREQUENCYBAND: {
                EventWifiFrequencyBand event;
                event.wifi_frequency_band = static_cast<WiFiFrequencyBand>(rep->ccp);
                event.wifi_frequency_band_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_WIFIWORKSHOPMODE: {
                EventWifiWorkshopMode event;
                event.wifi_workshop_mode = static_cast<WiFiWorkshopMode>(rep->ccp);
                event.wifi_workshop_mode_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_CARCFG_THEFTNOTIFICATION: {
                EventTheftNotification event;
                event.theft_notification = static_cast<TheftNotifcation>(rep->ccp);
                event.theft_notification_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_CARCFG_STOLENVEHICLETRACKING: {
                EventStolenVehicleTracking event;
                event.stolen_vehicle_tracking = static_cast<StolenVehicleTracking>(rep->ccp);
                event.stolen_vehicle_tracking_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_GNSSRECEIVER: {
                EventGnss event;
                event.gnss_receiver = static_cast<GnssReceiver>(rep->ccp);
                event.gnss_receiver_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_DIGITALVIDEORECORDER: {
                EventDigitalVideoRecorder event;
                event.digital_video_recorder = static_cast<DigitalVideoRecorder>(rep->ccp);
                event.digital_video_recorder_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            case EVENT_DRIVERALERTCONTROL: {
                EventDriverAlertControl event;
                event.driver_alert_control = static_cast<DriverAlertControl>(rep->ccp);
                event.driver_alert_control_validity = static_cast<CarConfValidity>(rep->validity);
                SendEventMessage(car_config_params[j].event_id, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
                break;
            }
            default:
                LOG(LOG_WARN, "VuCComm: %s event not handled paramid: %d", __FUNCTION__, rep->paramId);
            }
        }
    }
}

/**
    @brief Handle resp for car-config factory restore

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResCarConfigFactRest(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);
    UNUSED(data);
    ResCarConfigFactRestore res;
    FinishRequest(REQ_CARCONFIGFACTRESTORE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle resp for antenna switch

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResAntennaSwitch(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);
    CHECK_SIZE(sizeof(VucAntennaSwitchRespType), data->size, REQ_ANTENNASWITCH);
    const VucAntennaSwitchRespType *rep = reinterpret_cast<const VucAntennaSwitchRespType *>(data->buffer);
    ResAntennaSwitch res;
    res.mode = static_cast<AntennaMode>(rep->swControl);
    FinishRequest(REQ_ANTENNASWITCH, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle diagnostic error response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResDiagErr(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucDiagErrRespType), data->size, 0);
    const VucDiagErrRespType *rep = reinterpret_cast<const VucDiagErrRespType *>(data->buffer);

    ResGetDiagErrReport res;
    res.phone_ant_open_load_test_status = static_cast<DiagErrStatus>(rep->diagPhoneAntOpenLoadTestFailed);
    res.phone_ant_open_load_test_state = static_cast<DiagErrState>(rep->diagPhoneAntOpenLoadTestCompleted);
    res.ecall_ant_open_load_test_status = static_cast<DiagErrStatus>(rep->diagEcallAntOpenLoadTestFailed);
    res.ecall_ant_open_load_test_state = static_cast<DiagErrState>(rep->diagEcallAntOpenLoadTestCompleted);
    FinishRequest(REQ_GETDIAGERRREPORT, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle incoming VuC software build messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResVucSwBuild(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucSwBuildDateRepType), data->size, REQ_GETVUCSWBUILD);
    const VucSwBuildDateRepType *rep = reinterpret_cast<const VucSwBuildDateRepType *>(data->buffer);

    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildDay", rep->timestamp[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildDay!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildMonth", rep->timestamp[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildMonth!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildYear", (rep->timestamp[2] << 8 | rep->timestamp[3]), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildYear!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildHour", rep->timestamp[4], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildHour!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildMin", rep->timestamp[5], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildMin!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildSec", rep->timestamp[6], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildSec!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildSwVersion", rep->sw_version[0] << 8 | rep->sw_version[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildSwVersion!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VucSwBuild", "VucSwBuildCheckSum", rep->checksum[1] << 8 | rep->checksum[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucSwBuildCheckSum!", __FUNCTION__);
    }
    //Insert into datastorage when we get correct datatype
    ds_->StopReadWriteDepValues();

    ResGetVucSwBuild res = { .year=(uint16_t)(rep->timestamp[2] << 8 | rep->timestamp[3]),
                             .month=rep->timestamp[1],
                             .day=rep->timestamp[0],
                             .hour=rep->timestamp[4],
                             .min=rep->timestamp[5],
                             .sec=rep->timestamp[6],
                             .sw_version=(uint16_t)(rep->sw_version[0] << 8 | rep->sw_version[1]),
                             .checksum=(uint16_t)(rep->checksum[1] << 8 | rep->checksum[0])};

    FinishRequest(REQ_GETVUCSWBUILD, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle incoming BLE software build messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleSwBuild(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleSwBuildDateRepType), data->size, REQ_GETBLESWBUILD);
    const VucBleSwBuildDateRepType *rep = reinterpret_cast<const VucBleSwBuildDateRepType *>(data->buffer);
    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildDay", rep->timestamp[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildDay!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildMonth", rep->timestamp[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildMonth!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildYear", (rep->timestamp[2] << 8 | rep->timestamp[3]), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildYear!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildHour", rep->timestamp[4], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildHour!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildMin", rep->timestamp[5], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildMin!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildSec", rep->timestamp[6], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildSec!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildSwVersion", rep->sw_version[0] << 8 | rep->sw_version[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildSwVersion!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleSwBuildCheckSum", rep->checksum[1] << 8 | rep->checksum[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleSwBuildCheckSum!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleStackVersion_0", rep->stack_version[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleStackVersion_0!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleStackVersion_1", rep->stack_version[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleStackVersion_1!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BleSwBuild", "BleStackVersion_2", rep->stack_version[2], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleStackVersion_2!", __FUNCTION__);
    }
    //Insert into datastorage when we get correct datatype
    ds_->StopReadWriteDepValues();

    ResGetBleSwBuild res = { .year=(uint16_t)(rep->timestamp[2] << 8 | rep->timestamp[3]),
                             .month=rep->timestamp[1],
                             .day=rep->timestamp[0],
                             .hour=rep->timestamp[4],
                             .min=rep->timestamp[5],
                             .sec=rep->timestamp[6],
                             .sw_version=(uint16_t)(rep->sw_version[0] << 8 | rep->sw_version[1]),
                             .checksum=(uint16_t)(rep->checksum[1] << 8 | rep->checksum[0]),
                             .stack_version = {rep->stack_version[0], rep->stack_version[1], rep->stack_version[2] } };

    FinishRequest(REQ_GETBLESWBUILD, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle incoming VuC trace messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvVucTrace(const tpVCS_CustomMessage_t *data)
{

    // Dont use CHECK_SIZE since data->size can be dynamic between 5-22 (byte 3-20 in payload)
    if ((sizeof(VucNadTraceType) < data->size) && (data->size >= 5)) {
        LOG(LOG_WARN, "VuCComm %s: input data is of invalid size = %d, should be between 5-22", __FUNCTION__, data->size);
        return;
    }
    const VucNadTraceType *rep = reinterpret_cast<const VucNadTraceType *>(data->buffer);
    if (rep->messageName > 3 || rep->type > 2 || (rep->payload[0]&0x3F) >= TRACE_MODULE_ID_MAX) {
        LOG(LOG_VERB, "VuCComm %s: Invalid trace input (messagename: %d type: %d id: %d  max_id: %d)", __FUNCTION__,
            rep->messageName, rep->type, rep->payload[0]&0x3F, TRACE_MODULE_ID_MAX-1);
        return;
    }
    switch (rep->messageName) {
    case 0: { //TRACE_ENTRY
        //mn (messageName) fcodeid(fc), trace module id (mod-id), trace module report level (mod-lvl), trace alive counter (count)
        VUCTRACE(VUCTRACE_INFO, "VuCComm %s: mn: %s  type: %s  prio: %d  fc: 0x%x  mod-id: %s  mod-lvl: %d  count: %d  desc: %.*s",
                    __FUNCTION__, vuclog_message_name_str[rep->messageName], vuclog_type_str[rep->type], rep->priority,
                    rep->fcodeid, vuclog_trace_module_str[(rep->payload[0] & 0x3F)], (rep->payload[0] >> 6),
                    (rep->payload[1] << 8 | rep->payload[2]), 17-(sizeof(VucNadTraceType)-data->size), reinterpret_cast<const char*>(&rep->payload[3]));
        break;
    }
    case 1:
    case 3: { //TRACE_CONFIG_IPC && TRACE_CONFIG_CAN
        if (rep->type == 1) { //if response
            ResVucTraceConfig res;
            res.bus = static_cast<TraceBus>(rep->messageName);
            for (uint8_t i = 0; i < sizeof(rep->payload)/sizeof(uint8_t); i++) {
                res.config[i].module_id = static_cast<TraceModuleId>(rep->payload[i] & 0x3F);
                res.config[i].status = static_cast<TraceCfgStatus>(rep->payload[i] >> 6);
            }
            FinishRequest(REQ_VUCTRACE_CONFIG, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        }
        break;
    }
    case 2: { //TRACE_CONTROL
        if (rep->type == 1) { //if response
            ResVucTraceControl res;
            res.mode = static_cast<TracingMode>(rep->payload[0]);
            FinishRequest(REQ_VUCTRACE_CONTROL, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        }
        break;
    }
    default:
        LOG(LOG_WARN, "VuCComm %s: unknown message name: %d type: %d prio: %d fcodeid: 0x%x", __FUNCTION__,
            rep->messageName, rep->type, rep->priority, rep->fcodeid);
    }
}

/**
    @brief Handle incoming diagnostic error messages

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvVucDiagErr(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucDiagErrRepType), data->size, 0);
    const VucDiagErrRepType *rep = reinterpret_cast<const VucDiagErrRepType *>(data->buffer);

    EventDiagErrReport event;
    event.phone_ant_open_load_test_status = static_cast<DiagErrStatus>(rep->diagPhoneAntOpenLoadTestFailed);
    event.phone_ant_open_load_test_state = static_cast<DiagErrState>(rep->diagPhoneAntOpenLoadTestCompleted);
    event.ecall_ant_open_load_test_status = static_cast<DiagErrStatus>(rep->diagEcallAntOpenLoadTestFailed);
    event.ecall_ant_open_load_test_state = static_cast<DiagErrState>(rep->diagEcallAntOpenLoadTestCompleted);
    SendEventMessage(EVENT_DIAGERRREPORT, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle incoming electrical energy level message report

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvElecEngLvl(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucElectEngLvlRepType), data->size, 0);
    const VucElectEngLvlRepType *rep = reinterpret_cast<const VucElectEngLvlRepType *>(data->buffer);

    //Printing subdata but it is not reported further.
    LOG(LOG_DEBUG,"VuCComm %s:  mainData (EgyLvlElecMain): %d, subData(EgyLvlElecSubtype): %d",
                __FUNCTION__, rep->mainData,  rep->subData);

    if (ds_->ChangeIntParameter("ElectEnergyLvl", "EgyLvlElecMain", rep->mainData, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EgyLvlElecMain!", __FUNCTION__);
    }
}

/**
    @brief Handle incoming electrical energy level message response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResElecEngLvl(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucElectEngLvlRespType), data->size, 0);
    const VucElectEngLvlRespType *rep = reinterpret_cast<const VucElectEngLvlRespType *>(data->buffer);

    //Printing subdata but it is not reported further.
    LOG(LOG_DEBUG,"VuCComm %s:  mainData (EgyLvlElecMain): %d, subData(EgyLvlElecSubtype): %d",
                __FUNCTION__, rep->mainData,  rep->subData);

    if (ds_->ChangeIntParameter("ElectEnergyLvl", "EgyLvlElecMain", rep->mainData, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EgyLvlElecMain!", __FUNCTION__);
    }
    ResElectEngLvl res;
    res.level= static_cast<ElectEngLvl>(rep->mainData);
    FinishRequest(REQ_ELECTENGLVL, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle incoming backup battery state message event

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvBubState(const tpVCS_CustomMessage_t *data)
{
    const VucBubStateRepType *rep = reinterpret_cast<const VucBubStateRepType *>(data->buffer);

    CHECK_SIZE(sizeof(VucBubStateRepType), data->size, 0);
    LOG(LOG_DEBUG,"VuCComm %s:  BatteryUsed: %d  State: 0x%x, Chargelvl: 0x%x  BackupTmp: 0x%x  AmbTmp: 0x%x BattV: %d KL30V: %d",
                __FUNCTION__, rep->battUsed, rep->state, rep->chagLvl, rep->bubTemp, rep->ambTemp, rep->battVoltage, rep->kl30Voltage);

    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("BackupBatteryState", "BatteryUsed", rep->battUsed, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BatteryUsed!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "State", rep->state, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change State!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "ChargeLvl", rep->chagLvl, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change ChargeLvl!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "BackupBatTemp", rep->bubTemp, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BackupBatTemp!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "AmbientTemp", rep->ambTemp, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change AmbientTemp!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "BatteryVoltage", rep->battVoltage, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BatteryVoltage!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "KL30Voltage", rep->kl30Voltage, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change KL30Voltage!", __FUNCTION__);
    }
    ds_->StopReadWriteDepValues();

    EventLastBubState event;
    event.battery_used = static_cast<BatteryUsed>(rep->battUsed);
    event.battery_state = static_cast<BatteryState>(rep->state);
    event.charge_level = rep->chagLvl;
    event.backup_battery_temp = rep->bubTemp < 0xfe ? rep->bubTemp-45 : rep->bubTemp;
    event.ambient_temp = rep->ambTemp < 0xfe ? rep->ambTemp-45 : rep->ambTemp;
    event.battery_voltage = rep->battVoltage < 0xffff ? static_cast<float>(rep->battVoltage*0.01) : rep->battVoltage;
    event.kl30_voltage = rep->kl30Voltage < 0xffff ? static_cast<float>(rep->kl30Voltage*0.01) : rep->kl30Voltage;
    SendEventMessage(EVENT_LASTBUBSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);

}

/**
    @brief Handle incoming backup battery state message response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBubState(const tpVCS_CustomMessage_t *data)
{
    const VucBubStateRespType *rep = reinterpret_cast<const VucBubStateRespType *>(data->buffer);

    CHECK_SIZE(sizeof(VucBubStateRespType), data->size, 0);
    LOG(LOG_DEBUG,"VuCComm %s:  BatteryUsed: %d  State: 0x%x, Chargelvl: 0x%x  BackupTmp: 0x%x  AmbTmp: 0x%x BattV: %d KL30V: %d",
                __FUNCTION__, rep->battUsed, rep->state, rep->chagLvl, rep->bubTemp, rep->ambTemp, rep->battVoltage, rep->kl30Voltage);

    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("BackupBatteryState", "BatteryUsed", rep->battUsed, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BatteryUsed!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "State", rep->state, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change State!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "ChargeLvl", rep->chagLvl, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change ChargeLvl!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "BackupBatTemp", rep->bubTemp, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BackupBatTemp!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "AmbientTemp", rep->ambTemp, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change AmbientTemp!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "BatteryVoltage", rep->battVoltage, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BatteryVoltage!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("BackupBatteryState", "KL30Voltage", rep->kl30Voltage, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change KL30Voltage!", __FUNCTION__);
    }
    ds_->StopReadWriteDepValues();

    ResLastBubState res;
    res.battery_used = static_cast<BatteryUsed>(rep->battUsed);
    res.battery_state = static_cast<BatteryState>(rep->state);
    res.charge_level = rep->chagLvl;
    res.backup_battery_temp = rep->bubTemp < 0xfe ? rep->bubTemp-45 : rep->bubTemp;
    res.ambient_temp = rep->ambTemp < 0xfe ? rep->ambTemp-45 : rep->ambTemp;
    res.battery_voltage = rep->battVoltage < 0xffff ? static_cast<float>(rep->battVoltage*0.01) : rep->battVoltage;
    res.kl30_voltage = rep->kl30Voltage < 0xffff ? static_cast<float>(rep->kl30Voltage*0.01) : rep->kl30Voltage;
    FinishRequest(REQ_LASTBUBSTATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));

}

/**
    @brief Handle incoming car configuration parameter 
    faults message response
    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResCarCfgFaults(const tpVCS_CustomMessage_t *data)
{
    const VucCarCfgFaultsType *rep = reinterpret_cast<const VucCarCfgFaultsType *>(data->buffer);

    CHECK_SIZE(sizeof(VucCarCfgFaultsType), data->size, 0);
    LOG(LOG_DEBUG,"VuCComm %s:  numInvalidCfg: %d  ", __FUNCTION__, rep->numInvalidCfg);
    for(uint8_t i = 0; i < LENGTH(rep->parameterFault); i++)
    {
        LOG(LOG_DEBUG,"VuCComm %s:  No.%d numInvalidCfg: %d  ", __FUNCTION__, i, rep->parameterFault[i].numData);
        LOG(LOG_DEBUG,"VuCComm %s:  No.%d numInvalidCfg: %d  ", __FUNCTION__, i, rep->parameterFault[i].value);

    }
    ResCarCfgParameterFault res;
	
    res.numInvalidCfg = rep->numInvalidCfg;
    for(uint8_t i = 0; i < LENGTH(rep->parameterFault); i++)
    {
         res.parameterFault[i].numData = rep->parameterFault[i].numData;
         res.parameterFault[i].value = rep->parameterFault[i].value;
    }
    
    FinishRequest(REQ_CARCFGPARAMETERFAULT, RET_OK, reinterpret_cast<const unsigned char *>(&res));

}

/**
    @brief Handle incoming network management history
    
    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResNetworkManageHistory(const tpVCS_CustomMessage_t *data)
{
    const ResNMHistoryType *rep = reinterpret_cast<const ResNMHistoryType* >(data->buffer);
    for(uint8_t i =0; i <50; i++)
    {
         LOG(LOG_DEBUG,"VuCComm %s:  HandleResNetworkManageHistory data->buff %d  ", __FUNCTION__, data->buffer[i]);
    }

    CHECK_SIZE(sizeof(ResNMHistoryType), data->size, 0);
    ResNetworkManageHistory res;
    res.nmHistoryStateLast1 = rep->nmHistoryStateLast1;
    res.globalRealTimeStateLast1= rep->globalRealTimeStateLast1;
    res.nmHistoryStateLast2 = rep->nmHistoryStateLast2;
    res.globalRealTimeStateLast2= rep->globalRealTimeStateLast2;
    res.nmHistoryStateLast3 = rep->nmHistoryStateLast3;
    res.globalRealTimeStateLast3= rep->globalRealTimeStateLast3;
    res.nmHistoryStateLast4 = rep->nmHistoryStateLast4;
    res.globalRealTimeStateLast4= rep->globalRealTimeStateLast4;
    res.nmHistoryStateLast5 = rep->nmHistoryStateLast5;
    res.globalRealTimeStateLast5= rep->globalRealTimeStateLast5;
    res.nmHistoryStateLast6 = rep->nmHistoryStateLast6;
    res.globalRealTimeStateLast6= rep->globalRealTimeStateLast6;
    res.nmHistoryStateLast7 = rep->nmHistoryStateLast7;
    res.globalRealTimeStateLast7= rep->globalRealTimeStateLast7;
    res.nmHistoryStateLast8 = rep->nmHistoryStateLast8;
    res.globalRealTimeStateLast8= rep->globalRealTimeStateLast8;
    res.nmHistoryStateLast9 = rep->nmHistoryStateLast9;
    res.globalRealTimeStateLast9= rep->globalRealTimeStateLast9;
    res.nmHistoryStateLast10 = rep->nmHistoryStateLast10;
    res.globalRealTimeStateLast10= rep->globalRealTimeStateLast10;

    FinishRequest(REQ_NETWORK_MANAGEMENT_HISTORY, RET_OK, reinterpret_cast<const unsigned char *>(&res));

}

/**
    @brief Handle incoming network management history
    
    @param[in]  data           Payload from VuC
*/

void VuCComm::HandleResProgramPreCheck(const tpVCS_CustomMessage_t *data)
{
    const VucProgramPrecond_Chk *rep = reinterpret_cast<const VucProgramPrecond_Chk *>(data->buffer);

    CHECK_SIZE(sizeof(VucProgramPrecond_Chk), data->size, 0);
    LOG(LOG_DEBUG,"VuCComm %s:  UsgModSts: %d  ", __FUNCTION__, rep->UsgModSts);
    LOG(LOG_DEBUG,"VuCComm %s:  VehSpdLgt: %d  ", __FUNCTION__, rep->VehSpdLgt);
    LOG(LOG_DEBUG,"VuCComm %s:  KL30Sts: %d  ", __FUNCTION__, rep->KL30Sts);
    ResProgramPrecond_Chk res;
    res.UsgModSts = rep->UsgModSts;
    res.VehSpdLgt = rep->VehSpdLgt;
    res.KL30Sts = rep->KL30Sts;
    FinishRequest(REQ_PROGRAM_PRECOND_CHK, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE Disconnect

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleDisconnect(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleDisconnectRespType), data->size, REQ_BLE_DISCONNECT);
    const VucBleDisconnectRespType *rep = reinterpret_cast<const VucBleDisconnectRespType *>(data->buffer);
    ResBleDisconnect res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_DISCONNECT, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE Disconnect All

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleDisconnectAll(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleDisconnectAllRespType), data->size, REQ_BLE_DISCONNECTALL);
    const VucBleDisconnectAllRespType *rep = reinterpret_cast<const VucBleDisconnectAllRespType *>(data->buffer);
    ResBleDisconnectAll res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_DISCONNECTALL, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE generate bdak

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleGenBdak(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleGenBdakRespType), data->size, REQ_BLE_GENERATEBDAK);
    const VucBleGenBdakRespType *rep = reinterpret_cast<const VucBleGenBdakRespType *>(data->buffer);
    ResBleGenBdak res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_GENERATEBDAK, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE get bdak

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleGetBdak(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleGetBdakRespType), data->size, REQ_BLE_GETBDAK);
    const VucBleGetBdakRespType *rep = reinterpret_cast<const VucBleGetBdakRespType *>(data->buffer);
    ResBleGetBdak res;
    res.status = static_cast<BleReturnCode>(rep->status);
    std::memcpy(res.bdak, rep->bdak, sizeof(res.bdak));
    FinishRequest(REQ_BLE_GETBDAK, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE configure data service

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleCfgDataServ(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleCfgDataCommServRespType), data->size, REQ_BLE_CONFIG_DATACOMSERVICE);
    const VucBleCfgDataCommServRespType *rep = reinterpret_cast<const VucBleCfgDataCommServRespType *>(data->buffer);
    ResBleCfgDataCommService res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_CONFIG_DATACOMSERVICE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE configure ibeacon

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleCfgIbeacon(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleCfgIbeaconRespType), data->size, REQ_BLE_CONFIG_IBEACON);
    const VucBleCfgIbeaconRespType *rep = reinterpret_cast<const VucBleCfgIbeaconRespType *>(data->buffer);
    ResBleCfgIbeacon res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_CONFIG_IBEACON, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE configure generic access

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleCfgGenAcc(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleCfgGenAccRespType), data->size, REQ_BLE_CONFIG_GENERICACCESS);
    const VucBleCfgGenAccRespType *rep = reinterpret_cast<const VucBleCfgGenAccRespType *>(data->buffer);
    ResBleCfgGenAcc res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_CONFIG_GENERICACCESS, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE start service

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleStartService(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleStartPairServRespType), data->size, REQ_BLE_STARTPARINGSERVICE);
    const VucBleStartPairServRespType *rep = reinterpret_cast<const VucBleStartPairServRespType *>(data->buffer);
    ResBleStartPairService res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_STARTPARINGSERVICE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE stop service

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleStopService(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleStopPairServRespType), data->size, REQ_BLE_STOPPARINGSERVICE);
    const VucBleStopPairServRespType *rep = reinterpret_cast<const VucBleStopPairServRespType *>(data->buffer);
    ResBleStopPairService res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_STOPPARINGSERVICE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE connection status

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvBleConnStatus(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleConnStatusRepType), data->size, 0);
    const VucBleConnStatusRepType *rep = reinterpret_cast<const VucBleConnStatusRepType *>(data->buffer);
    EventBleConnStatus event;
    event.connection_id = rep->connectionID;
    event.state = static_cast<BleConnState>(rep->connectionStatus);
    SendEventMessage(EVENT_BLE_CONNECTIONSTATUS, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle BLE reset notification

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvBleResetNotification(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    UNUSED(data);
    EventBleResetNotification event;
    SendEventMessage(EVENT_BLE_RESET_NOTIFICATION, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle BLE Start advertising response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleStartAdvertising(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleStartAdvRespType), data->size, REQ_BLE_STARTADVERTISING);
    const VucBleStartAdvRespType *rep = reinterpret_cast<const VucBleStartAdvRespType *>(data->buffer);
    ResBleStartAdvertising res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_STARTADVERTISING, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE Stop advertising response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBleStopAdvertising(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleStopAdvRespType), data->size, REQ_BLE_STOPADVERTISING);
    const VucBleStopAdvRespType *rep = reinterpret_cast<const VucBleStopAdvRespType *>(data->buffer);
    ResBleStopAdvertising res;
    res.status = static_cast<BleReturnCode>(rep->status);
    FinishRequest(REQ_BLE_STOPADVERTISING, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle BLE mtu size event

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvBleMtuSize(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBleMtuSizeRepType), data->size, 0);
    const VucBleMtuSizeRepType *rep = reinterpret_cast<const VucBleMtuSizeRepType *>(data->buffer);
    EventBleMtuSize event;
    event.connection_id = rep->connectionID;
    event.mtu_size = rep->mtuSize;
    SendEventMessage(EVENT_BLE_MTUSIZE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle DTC handling response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResDtcHandling(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucDtcHandlingRespType), data->size, REQ_DTCCTLSETTINGS);
    const VucDtcHandlingRespType *rep = reinterpret_cast<const VucDtcHandlingRespType *>(data->buffer);
    ResDtcCtlSettings res;
    res.status = static_cast<DtcCtlSettingStatus>(rep->handlingResponse);
    FinishRequest(REQ_DTCCTLSETTINGS, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}


/**
    @brief Handle DTC full entry report

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvDtcFullEntry(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucFullDtcEntryReportType), data->size, 0);
    const VucFullDtcEntryReportType *rep = reinterpret_cast<const VucFullDtcEntryReportType *>(data->buffer);
    EventDtcFullReport event;
    memcpy(&event, rep, sizeof(EventDtcFullReport));
    SendEventMessage(EVENT_DTCFULLREPORT, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle DTC status entry report

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvDtcStatusEntry(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucDtcStatusEntryReportType), data->size, 0);
    const VucDtcStatusEntryReportType *rep = reinterpret_cast<const VucDtcStatusEntryReportType *>(data->buffer);
    EventDtcStatusReport event;
    memcpy(&event, rep, sizeof(EventDtcStatusReport));
    SendEventMessage(EVENT_DTCSTATUSREPORT, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle amplifier state

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvAmplifierState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucAmplifierStateType), data->size, 0);
    const VucAmplifierStateType *rep = reinterpret_cast<const VucAmplifierStateType *>(data->buffer);
    EventAmplifierState event;
    event.state = static_cast<AmplifierState>(rep->amplifierState);
    event.status = static_cast<AmplifierStatus>(rep->amplifierError);
    SendEventMessage(EVENT_AMPLIFIERSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle Rf Kline error codes

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvRfKlineErrorCodes(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucRfKlineErrorCodesType), data->size, 0);
    const VucRfKlineErrorCodesType *rep = reinterpret_cast<const VucRfKlineErrorCodesType *>(data->buffer);
    EventRfKlineErrReport event;
    event.rf_error_code = rep->rfErrorCodes;
    event.kline_error_code = rep->klineErrorCodes;
    SendEventMessage(EVENT_RFKLINEERRREPORT, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle power mode

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvPowerMode(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucPowerModeRepType), data->size, 0);
    const VucPowerModeRepType *rep = reinterpret_cast<const VucPowerModeRepType *>(data->buffer);

    if (ds_->ChangeIntParameter("VuCPowerMode", "VuCPowerMode", rep->powerMode, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VuCPowerMode!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("VuCPowerMode", "VuCWakeupReason", rep->wakeupReason, true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VuCWakeupReason!", __FUNCTION__);
    }
}

/**
    @brief Handle ohc button state

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvOhcBtnState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucOHCButtonStateRepType), data->size, 0);
    const VucOHCButtonStateRepType *rep = reinterpret_cast<const VucOHCButtonStateRepType *>(data->buffer);

    ds_->StartReadWriteDepValues();

    if (ds_->ChangeIntParameter("OhcBtnState", "EcallBtn", rep->ecallRequest, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EcallBtn!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("OhcBtnState", "IcallBtn", rep->icallRequest, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change IcallBtn!", __FUNCTION__);
    }

    ds_->StopReadWriteDepValues();

    EventOhcBtnState event;
    event.ecall_btn = static_cast<OhcBtnState>(rep->ecallRequest);
    event.icall_btn = static_cast<OhcBtnState>(rep->icallRequest);
    if (event.ecall_btn == OHCBTN_ACTIVE || event.icall_btn == OHCBTN_ACTIVE)
    {
        SendEventMessage(EVENT_OHCBTNSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
    }
}

/**
    @brief Handle ohc button response state

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResOhcBtnState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucOHCButtonStateRespType), data->size, 0);
    const VucOHCButtonStateRespType *rep = reinterpret_cast<const VucOHCButtonStateRespType *>(data->buffer);

    ds_->StartReadWriteDepValues();

    if (ds_->ChangeIntParameter("OhcBtnState", "EcallBtn", rep->ecallRequest, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EcallBtn!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("OhcBtnState", "IcallBtn", rep->icallRequest, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change IcallBtn!", __FUNCTION__);
    }

    ds_->StopReadWriteDepValues();

    ResGetOhcBtnState res;
    res.ecall_btn = static_cast<OhcBtnState>(rep->ecallRequest);
    res.icall_btn = static_cast<OhcBtnState>(rep->icallRequest);
    FinishRequest(REQ_GETOHCBTNSTATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle vinnumber response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResVINNumber(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucVINRepType), data->size, REQ_VINNUMBER);
    const VucVINRepType *rep = reinterpret_cast<const VucVINRepType *>(data->buffer);
    bool all_zeroes = true;
    for (uint8_t i = 0; i < LENGTH(rep->vin); i++) {
        if (rep->vin[i] != 0) {
            all_zeroes = false;
            break;
        }
    }

    if (all_zeroes) {
        LOG(LOG_WARN, "VuCComm: %s: Vin can not yet be provided by VuC!", __FUNCTION__);
        FinishRequest(REQ_VINNUMBER, RET_ERR_VALUE_NOT_AVAILABLE, 0);
        return;
    }

    if (ds_->ChangeStringParameter("VIN", "VinNumber", std::string((const char*)rep->vin,17), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VinNumber!", __FUNCTION__);
    }

    ResVINNumber res;
    memset(res.vin_number, 0, sizeof(res.vin_number)/sizeof(res.vin_number[0]));
    memcpy(res.vin_number, rep->vin, 17);
    FinishRequest(REQ_VINNUMBER, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle message catalogue response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResMsgCatVersion(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucMsgCatalogVerType), data->size, REQ_GETMSGCATVERSION);
    const VucMsgCatalogVerType *rep = reinterpret_cast<const VucMsgCatalogVerType *>(data->buffer);

    LOG(LOG_DEBUG, "VuCComm: %s VuC catalogue version is: %02x.%02x.", __FUNCTION__, rep->vucMsgCatalogByte1, rep->vucMsgCatalogByte2);
    ResGetMsgCatVer res;
    res.version[0] = rep->vucMsgCatalogByte1;
    res.version[1] = rep->vucMsgCatalogByte2;
    res.version[2] = rep->vucMsgCatalogByte3;
    FinishRequest(REQ_GETMSGCATVERSION, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle message catalogue response

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResTcamHwVersion(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucTcamHwVersionType), data->size, REQ_GETTCAMHWVERSION);
    const VucTcamHwVersionType *rep = reinterpret_cast<const VucTcamHwVersionType *>(data->buffer);
    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("TcamHwVersion", "EolHwVersion",rep->eolhwVersion, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change HwVersion!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("TcamHwVersion", "EolData", rep->eolDataVersion, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolData!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("TcamHwVersion", "EolHwVariant", rep->eolhwVariant, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolHwVariant!", __FUNCTION__);
    }
    if (ds_->ChangeCharArrayParameter("TcamHwVersion", "EolVolvoSerialNumber", std::vector<unsigned char>(rep->eolVolvoSerialNumber, rep->eolVolvoSerialNumber+LENGTH(rep->eolVolvoSerialNumber)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolVolvoSerialNumber!", __FUNCTION__);
    }
    if (ds_->ChangeStringParameter("TcamHwVersion", "EolVolvoDelivPartNumber", std::string((const char*)rep->eolVolvoDelivPartNumber, LENGTH(rep->eolVolvoDelivPartNumber)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolVolvoDelivPartNumber!", __FUNCTION__);
    }
    if (ds_->ChangeStringParameter("TcamHwVersion", "EolVolvoCorePartNumber", std::string((const char*)rep->eolVolvoCorePartNumber, LENGTH(rep->eolVolvoCorePartNumber)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolVolvoCorePartNumber!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("TcamHwVersion", "EolProductionDay", rep->eolProductionDate[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolProductionDay!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("TcamHwVersion", "EolProductionMonth", rep->eolProductionDate[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolProductionMonth!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("TcamHwVersion", "EolProductionYear", (rep->eolProductionDate[2] << 8 | rep->eolProductionDate[3]), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change EolProductionYear!", __FUNCTION__);
    }

    ds_->StopReadWriteDepValues();
    //LOG(LOG_INFO, "VuCComm %s: TCAM HARDWARE VERSION: 0x%x  eol_data_version: 0x%x", __FUNCTION__, rep->hwVersion, rep->eolDataVersion);
    ResGetTcamHwVer res;
    res.eol_hw_version = static_cast<TCAMEolHwVersion>(rep->eolhwVersion);
    res.eol_data = static_cast<TCAMEolData>(rep->eolDataVersion);
    res.eol_hw_variant = static_cast<TCAMEolHwVariant>(rep->eolhwVariant);
    memcpy(res.eol_volvo_ecu_serial_number, rep->eolVolvoSerialNumber, LENGTH(res.eol_volvo_ecu_serial_number));
    memcpy(res.eol_volvo_delivery_part_number, rep->eolVolvoDelivPartNumber, LENGTH(res.eol_volvo_delivery_part_number));
    memcpy(res.eol_volvo_core_part_number, rep->eolVolvoCorePartNumber, LENGTH(res.eol_volvo_core_part_number));
    res.eol_volvo_production_year = (uint16_t)(rep->eolProductionDate[2] << 8 | rep->eolProductionDate[3]);
    res.eol_volvo_production_month = rep->eolProductionDate[1];
    res.eol_volvo_production_day = rep->eolProductionDate[0];

    FinishRequest(REQ_GETTCAMHWVERSION, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle VuC & BLE software part number

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResVucBleSwPartNumber(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucVucBleSwHwVersionRepType), data->size, REQ_GETVUCBLESWPARTNUMBER);
    ResGetVucBleSwPartNumber res;
    const VucVucBleSwHwVersionRepType *rep = reinterpret_cast<const VucVucBleSwHwVersionRepType *>(data->buffer);
    ds_->StartReadWriteDepValues();
    if (ds_->ChangeStringParameter("VucSwVersion", "VucPartNumber", std::string((const char*)rep->vucPartNumber, LENGTH(res.vuc_part_number)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucPartNumber!", __FUNCTION__);
    }
    if (ds_->ChangeStringParameter("VucSwVersion", "VucVersion", std::string((const char*)(rep->vucPartNumber+LENGTH(res.vuc_part_number)), LENGTH(res.vuc_version)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VucVersion!", __FUNCTION__);
    }
    if (ds_->ChangeStringParameter("BleSwVersion", "BlePartNumber", std::string((const char*)rep->blePartNumber, LENGTH(res.ble_part_number)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BlePartNumber!", __FUNCTION__);
    }
    if (ds_->ChangeStringParameter("BleSwVersion", "BleVersion", std::string((const char*)(rep->blePartNumber+LENGTH(res.ble_part_number)), LENGTH(res.ble_version)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change BleVersion!", __FUNCTION__);
    }
    ds_->StopReadWriteDepValues();

    memcpy(res.vuc_part_number, rep->vucPartNumber, LENGTH(res.vuc_part_number));
    memcpy(res.vuc_version, (rep->vucPartNumber+LENGTH(res.vuc_part_number)), LENGTH(res.vuc_version));
    memcpy(res.ble_part_number, rep->blePartNumber, LENGTH(res.ble_part_number));
    memcpy(res.ble_version, (rep->blePartNumber+LENGTH(res.ble_part_number)), LENGTH(res.ble_version));
    FinishRequest(REQ_GETVUCBLESWPARTNUMBER, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle RF software and hardware version

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResRfSwHwVersion(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucRfSwHwVersionRespType), data->size, REQ_GETRFVERSION);
    const VucRfSwHwVersionRespType *rep = reinterpret_cast<const VucRfSwHwVersionRespType *>(data->buffer);
    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("RfVersion", "RfSwVersion", (rep->rfSwVersion[0] << 8 | rep->rfSwVersion[1]) , false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change RfSwVersion!", __FUNCTION__);
    }
    if (ds_->ChangeCharArrayParameter("RfVersion", "RfHwVersion", std::vector<unsigned char>(rep->rfHwVersion, rep->rfHwVersion+LENGTH(rep->rfHwVersion)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change RfHwVersion!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("RfVersion", "RfSwMarketVar", rep->rfSwMarketVar, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change RfSwMarketVar!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("RfVersion", "RfHwMarketVar", (rep->rfHwMarketVar[0] <<8 | rep->rfHwMarketVar[1]), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change RfHwMarketVar!", __FUNCTION__);
    }
    if (ds_->ChangeCharArrayParameter("RfVersion", "RfHwSerialNumber", std::vector<unsigned char>(rep->rfHwSerialNumber, rep->rfHwSerialNumber+LENGTH(rep->rfHwSerialNumber)), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change RfHwSerialNumber!", __FUNCTION__);
    }
    ds_->StopReadWriteDepValues();
    ResGetRfVersion res;
    res.sw_version = rep->rfSwVersion[0] << 8 | rep->rfSwVersion[1];
    memcpy(res.hw_version, rep->rfHwVersion, LENGTH(res.hw_version));
    res.sw_variant = static_cast<RFSWVariant>(rep->rfSwMarketVar);
    res.hw_variant = static_cast<RFHWVariant>(rep->rfHwMarketVar[0] <<8 | rep->rfHwMarketVar[1]);
    memcpy(res.hw_serial_number, rep->rfHwSerialNumber, LENGTH(res.hw_serial_number));
    FinishRequest(REQ_GETRFVERSION, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle incoming vinnumber messages

    @param[in]  data           VINNumber
*/
void VuCComm::HandleEvVINNumber(const tpVCS_CustomMessage_t *data)
{

    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucVINRepType), data->size, 0);
    const VucVINRepType *rep = reinterpret_cast<const VucVINRepType *>(data->buffer);

    if (ds_->ChangeStringParameter("VIN", "VinNumber", std::string((const char*)rep->vin,17), true) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change VinNumber!", __FUNCTION__);
    }
}

/**
    @brief Handle FBL software build date

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResFblSwBuild(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucFblSwBuildDateRespType), data->size, REQ_GETFBLSWBUILD);
    const VucFblSwBuildDateRespType *rep = reinterpret_cast<const VucFblSwBuildDateRespType *>(data->buffer);
    ds_->StartReadWriteDepValues();
    if (ds_->ChangeIntParameter("FblVersion", "FblSwBuildDay", rep->timestamp[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwBuildDay!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwBuildMonth", rep->timestamp[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwBuildMonth!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwBuildYear", (rep->timestamp[2] << 8 | rep->timestamp[3]), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwBuildYear!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwBuildHour", rep->timestamp[4], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwBuildHour!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwBuildMin", rep->timestamp[5], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwBuildMin!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwBuildSec", rep->timestamp[6], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwBuildSec!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwChecksum", rep->checksum[1] << 8 | rep->checksum[0], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblChecksum!", __FUNCTION__);
    }
    if (ds_->ChangeIntParameter("FblVersion", "FblSwVersion", rep->sw_version[0] << 8 | rep->sw_version[1], false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to change FblSwVersion!", __FUNCTION__);
    }
    ds_->StopReadWriteDepValues();

    ResGetFblSwBuild res = { .year=(uint16_t)(rep->timestamp[2] << 8 | rep->timestamp[3]),
                             .month=rep->timestamp[1],
                             .day=rep->timestamp[0],
                             .hour=rep->timestamp[4],
                             .min=rep->timestamp[5],
                             .sec=rep->timestamp[6],
                             .sw_version=(uint16_t)(rep->sw_version[0] << 8 | rep->sw_version[1]),
                             .checksum=(uint16_t)(rep->checksum[1] << 8 | rep->checksum[0]) };

    FinishRequest(REQ_GETFBLSWBUILD, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle Belt information

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResBeltInformation(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucBeltInfoRespType), data->size, REQ_GETBELTINFORMATION);
    const VucBeltInfoRespType *rep = reinterpret_cast<const VucBeltInfoRespType *>(data->buffer);

    ResGetBeltInformation res;
    res.driver.lock_state = static_cast<BeltLockState>(rep->BitLockStADrvrForBltLockSt1);
    res.driver.lock_status = static_cast<BeltLockStatus>(rep->BitLockStADrvrForBltLockSts);
    res.driver.equipped_status = BELT_LOCK_EQUIPPED; // Drivers belt is always equipped

    res.passenger.lock_state = static_cast<BeltLockState>(rep->BitLockStAPassForBltLockSt1);
    res.passenger.lock_status = static_cast<BeltLockStatus>(rep->BitLockStAPassForBltLockSts);
    res.passenger.equipped_status = BELT_LOCK_EQUIPPED; // Passenger belt is always equipped

    res.second_row_left.lock_state = static_cast<BeltLockState>(rep->BitLockStARowSecLeForBltLockSt1);
    res.second_row_left.lock_status = static_cast<BeltLockStatus>(rep->BitLockStARowSecLeForBltLockSts);
    res.second_row_left.equipped_status = static_cast<BeltLockEquippedStatus>(rep->BitLockStARowSecLeForBltLockEq);

    res.second_row_middle.lock_state = static_cast<BeltLockState>(rep->BitLockStARowSecMidForBltLockSt1);
    res.second_row_middle.lock_status = static_cast<BeltLockStatus>(rep->BitLockStARowSecMidForBltLockSts);
    res.second_row_middle.equipped_status = static_cast<BeltLockEquippedStatus>(rep->BitLockStARowSecMidForBltLockEq);

    res.second_row_right.lock_state = static_cast<BeltLockState>(rep->BitLockStARowSecRiForBltLockSt1);
    res.second_row_right.lock_status = static_cast<BeltLockStatus>(rep->BitLockStARowSecRiForBltLockSts);
    res.second_row_right.equipped_status = static_cast<BeltLockEquippedStatus>(rep->BitLockStARowSecRiForBltLockEq);

    res.third_row_left.lock_state = static_cast<BeltLockState>(rep->BitLockStARowThrdLeForBltLockSt1);
    res.third_row_left.lock_status = static_cast<BeltLockStatus>(rep->BitLockStARowThrdLeForBltLockSts);
    res.third_row_left.equipped_status = static_cast<BeltLockEquippedStatus>(rep->BitLockStARowThrdLeForBltLockEq);

    res.third_row_right.lock_state = static_cast<BeltLockState>(rep->BitLockStARowThrdRiForBltLockSt1);
    res.third_row_right.lock_status = static_cast<BeltLockStatus>(rep->BitLockStARowThrdRiForBltLockSts);
    res.third_row_right.equipped_status = static_cast<BeltLockEquippedStatus>(rep->BitLockStARowThrdRiForBltLockEq);

    FinishRequest(REQ_GETBELTINFORMATION, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle vehicle speed state

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvVehicleSpeedState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucVehicleSpeedStateRepType), data->size, 0);
    const VucVehicleSpeedStateRepType *rep = reinterpret_cast<const VucVehicleSpeedStateRepType *>(data->buffer);
    EventVehicleSpeedState event;

    event.vehicle_speed_below_v_scan_full = static_cast<bool>(rep->vScanFull);
    event.vehicle_speed_below_v_scan_limited =  static_cast<bool>(rep->vScanLimited);

    SendEventMessage(EVENT_VEHICLESPEEDSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle can voltage error report

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvCanVoltageErr(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucCANVoltageErrorRepType), data->size, 0);
    const VucCANVoltageErrorRepType *rep = reinterpret_cast<const VucCANVoltageErrorRepType *>(data->buffer);
    EventCanVoltageErrReport event;

    event.can_under_voltage = static_cast<CanVoltageStatus>(rep->canUnderVoltage);
    event.kl30_under_voltage = static_cast<CanVoltageStatus>(rep->kl30UnderVoltage);
    event.kl30_over_voltage = static_cast<CanVoltageStatus>(rep->kl30OverVoltage);

    SendEventMessage(EVENT_CANVOLTAGEERRREPORT, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle local config request from vuc

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleReqLocalCfgParam(const tpVCS_CustomMessage_t *data)
{

    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    tpVCS_CustomMessage_t msg;
    CHECK_SIZE(sizeof(VucLocalConfigParamReqType), data->size, 0);
    const VucLocalConfigParamReqType *rep = reinterpret_cast<const VucLocalConfigParamReqType *>(data->buffer);

    if (rep->localCfgParam < LCFG_TIMETOWAITFORIPLMAFTERBOOT || rep->localCfgParam >= LCFG_MAX) {
        LOG(LOG_WARN, "VuCComm %s VuC local config request param: %d out is out of range, returning", __FUNCTION__, rep->localCfgParam);
        return;
    }

    NadLocalConfigParamRespType res;
    lcfg::ILocalConfig* lcfg = lcfg::ILocalConfig::GetInstance();

    LOG(LOG_DEBUG, "VuCComm %s VuC local config request param: %d", __FUNCTION__, rep->localCfgParam);
    switch (static_cast<VucLocalConfigParam>(rep->localCfgParam)) {
    case LCFG_TIMETOWAITFORIPLMAFTERBOOT : {
        int value = 120;
        lcfg->GetInt("VUC", "time_to_wait_for_iplm_after_boot", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_TIMERDOOROPENUSAGEMODE : {
        int value = 1000;
        lcfg->GetInt("VUC", "timer_door_open_usage_mode", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_STANDBYMODETIMER : {
        int value = 10;
        lcfg->GetInt("VUC", "standby_mode_timer", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_POLLINGMODETIMER : {
        int value = 180;
        lcfg->GetInt("VUC", "polling_mode_timer", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_RESOURCEGROUPMEMBER : {
        int value = 14;
        lcfg->GetInt("VUC", "resource_groupmember", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_PSIM_DISABLED : {
        int value = 0;
        lcfg->GetInt("VUC", "psim_disabled", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_ERS_VFC_TIMEOUT : {
        int value = 0;
        lcfg->GetInt("VUC", "ers_vfc_timeout", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_ERS_TIMEOUT_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "ers_timeout_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_ERS_RESPONSE_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "ers_response_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_ERS_RESPONSE_VALID_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "ers_response_valid_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_ERS_STATUS_UPDATE_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "ers_status_update_time", value);
        res.localCfgParamValue = value;
    }
    case LCFG_CLHAL_VFC_TIMEOUT : {
        int value = 0;
        lcfg->GetInt("VUC", "clhal_vfc_timeout", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_CLIMA_REQ_TIMEOUT : {
        int value = 0;
        lcfg->GetInt("VUC", "clima_req_timeout", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_CLIMA_WARN_STATE_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "clima_warn_state_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_RPC_VFC_TIMEOUT : {
        int value = 0;
        lcfg->GetInt("VUC", "rpc_vfc_timeout", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_DASHBOARD_SEND_DELAY : {
        int value = 0;
        lcfg->GetInt("VUC", "dashboard_send_delay", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_RDU_VFC_TIMEOUT : {
        int value = 0;
        lcfg->GetInt("VUC", "rdu_vfc_timeout", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_LOCK_VALID_STATUS_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "lock_valid_status_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_LOCK_STATUS_UPDATE_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "lock_status_update_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_UNLOCK_CHECK_TIME : {
        int value = 0;
        lcfg->GetInt("VUC", "unlock_check_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_GSD_VFC_TIMEOUT : {
        int value = 0;
        lcfg->GetInt("VUC", "gsd_vfc_timeout", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_DISABLE_WORKSHOP_WIFI_MOD : {
        int value = 0;
        lcfg->GetInt("VUC", "disable_workshop_wifi_mod", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_WIFI_FREQ_BAND : {
        int value = 0;
        lcfg->GetInt("VUC", "wifi_freq_band", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_WIFI_FUNCTION_CONTROL : {
        int value = 0;
        lcfg->GetInt("VUC", "wifi_function_control", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_V_SCAN_FULL : {
        int value = 10;
        lcfg->GetInt("VUC", "v_scan_full", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_V_SCAN_LIMITED : {
        int value = 10;
        lcfg->GetInt("VUC", "v_scan_limited", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_SLEEP_TIME : {
        int value = 58;
        lcfg->GetInt("VUC", "sleep_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_RECEIVE_TIME : {
        int value = 120;
        lcfg->GetInt("VUC", "receive_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_INTERVAL_ACTIVE : {
        int value = 100;
        lcfg->GetInt("VUC", "ble_adv_interval_active", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_INTERVAL_STANDBY : {
        int value = 100;
        lcfg->GetInt("VUC", "ble_adv_interval_standby", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_INTERVAL_POLLING : {
        int value = 100;
        lcfg->GetInt("VUC", "ble_adv_interval_polling", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_INTERVAL_NETOFF : {
        int value = 100;
        lcfg->GetInt("VUC", "ble_adv_interval_netoff", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_BROADCAST_ACTIVE : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_adv_broadcast_active", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_BROADCAST_STANDBY : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_adv_broadcast_standby", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_BROADCAST_POLLING : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_adv_broadcast_polling", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_ADV_BROADCAST_NETOFF : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_adv_broadcast_netoff", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_POWERLEVEL_ACTIVE : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_tx_powerlever_active", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_POWERLEVEL_STANDBY : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_tx_powerlever_standby", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_POWERLEVEL_POLLING : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_tx_powerlever_polling", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_POWERLEVEL_NETOFF : {
        int value = 7;
        lcfg->GetInt("VUC", "ble_tx_powerlever_netoff", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_MEASURED_POWER_ACTIVE : {
        int value = 0;
        lcfg->GetInt("VUC", "ble_tx_measured_power_active", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_MEASURED_POWER_STANDBY : {
        int value = 0;
        lcfg->GetInt("VUC", "ble_tx_measured_power_standby", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_MEASURED_POWER_POLLING : {
        int value = 0;
        lcfg->GetInt("VUC", "ble_tx_measured_power_polling", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_TX_MEASURED_POWER_NETOFF : {
        int value = 0;
        lcfg->GetInt("VUC", "ble_tx_measured_power_netoff", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_BLE_CHECK_CLIENT_AUTHENTICATION_TIME : {
        int value = 30;
        lcfg->GetInt("VUC", "ble_check_client_authentication_time", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_PNCBITS_CHECK_PERIOD: {
        int value = 5000;
        lcfg->GetInt("VUC", "pncbits_check_period", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_SLEEP_POLL_CYCLE2: {
        int value = 168;
        lcfg->GetInt("VUC", "sleep_poll2_check_period", value);
        res.localCfgParamValue = value;
        break;
    }
    case LCFG_POLLING_INTERVAL2: {
        int value = 240;
        lcfg->GetInt("VUC", "polling_interval2_check_period", value);
        res.localCfgParamValue = value;
        break;
    }
    default:
        LOG(LOG_WARN, "VuCComm: %s: Unknown BLE request-param.", __FUNCTION__);
        return;
    }

    res.localCfgParam = rep->localCfgParam;
    msg.size = sizeof(NadLocalConfigParamRespType);
    msg.buffer = reinterpret_cast<gchar*>(&res);

    if (tpVCS_sendCustomMessage(IPC_NAD_LOCAL_CONFIG_PARAM_RESP, &msg) != E_VCS_ERROR_NONE)
        LOG(LOG_WARN, "VuCComm: Failed to send CustomMessage (error = %d) (vuc_id = IPC_NAD_LOCAL_CONFIG_PARAM_RESP)");
    else
        LOG(LOG_DEBUG, "VuCComm: %s: IPC_NAD_LOCAL_CONFIG_PARAM_RESP message sent.", __FUNCTION__);
}


/**
    @brief Handle diag couple events

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvDiagCoupleReport(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucDiagCoupleRepType), data->size, 0);
    const VucDiagCoupleRepType *rep = reinterpret_cast<const VucDiagCoupleRepType *>(data->buffer);
    EventDiagCoupleReport event;
    event.req = static_cast<WifiWorkshopConnect>(rep->request);
    SendEventMessage(EVENT_DIAGCOUPLEREPORT, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle get did global snapshot data

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResDIDGlobalSnapshotData(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucGlobalSSDataDidRespType), data->size, REQ_GETDIDGLOBALSNAPSHOTDATA);
    const VucGlobalSSDataDidRespType *rep = reinterpret_cast<const VucGlobalSSDataDidRespType *>(data->buffer);
    ResGetDIDGlobalSnapshotData res;
    memcpy(&res, rep, sizeof(ResGetDIDGlobalSnapshotData));
    FinishRequest(REQ_GETDIDGLOBALSNAPSHOTDATA, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle dtcs detected request

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResDTCsDetected(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(0, data->size, REQ_DTCSDETECTED);
    Empty res;
    FinishRequest(REQ_DTCSDETECTED, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle psim data request

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResPSIMData(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucPSimDataCommunicationRespType), data->size, REQ_PSIMDATA);
    ResPSIMData res;
    const VucPSimDataCommunicationRespType *rep = reinterpret_cast<const VucPSimDataCommunicationRespType *>(data->buffer);
    memcpy(res.data, rep, sizeof(res.data));
    FinishRequest(REQ_PSIMDATA, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}

/**
    @brief Handle psim data request

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResThermalMitigation(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(0, data->size, REQ_THERMALMITIGATION);
    Empty res;
    FinishRequest(REQ_THERMALMITIGATION, RET_OK, reinterpret_cast<const unsigned char *>(&res));
}
#if 1 // nieyj
/**
    @brief Handle psim data request

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleResVFCActivate(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

//    CHECK_SIZE(0, data->size, REQ_VFCACTIVATE);
    CHECK_SIZE(sizeof(ResVFCActivate), data->size, REQ_VFCACTIVATE);
    const ResVFCActivate *res = reinterpret_cast<const ResVFCActivate *>(data->buffer);
    
    LOG(LOG_DEBUG, "VuCComm id = %d, result = %d: ", res->id, res->resule);

    FinishRequest(REQ_VFCACTIVATE, RET_OK, reinterpret_cast<const unsigned char *>(res));
}
#endif

////////////////////////////////////////////////////////////
// @brief : Handle remote engine challenge code
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResRMTEngineChallengeCode(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(Res_RMTEngineSecurityRandom), data->size, 0);
    const Res_RMTEngineSecurityRandom *rep = reinterpret_cast<const Res_RMTEngineSecurityRandom *>(data->buffer);
    // imobRemMgrChk
    if (ds_->ChangeIntParameter("remoteEngineChallengeCode", "imobRemMgrChk", rep->imobRemMgrChk, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobRemMgrChk");
    }
    // imobDataRemMgrChk0
    if (ds_->ChangeIntParameter("remoteEngineChallengeCode", "imobDataRemMgrChk0", rep->imobDataRemMgrChk0, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobDataRemMgrChk0");
    }
    // imobDataRemMgrChk1
    if (ds_->ChangeIntParameter("remoteEngineChallengeCode", "imobDataRemMgrChk1", rep->imobDataRemMgrChk1, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobDataRemMgrChk1");
    }
    // imobDataRemMgrChk2
    if (ds_->ChangeIntParameter("remoteEngineChallengeCode", "imobDataRemMgrChk2", rep->imobDataRemMgrChk2, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobDataRemMgrChk2");
    }
    // imobDataRemMgrChk3
    if (ds_->ChangeIntParameter("remoteEngineChallengeCode", "imobDataRemMgrChk3", rep->imobDataRemMgrChk3, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobRemMgrChk");
    }
    // imobDataRemMgrChk4
    if (ds_->ChangeIntParameter("remoteEngineChallengeCode", "imobDataRemMgrChk4", rep->imobDataRemMgrChk4, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobRemMgrChk");
    }
    Res_RMTEngineSecurityRandom res;
    res.imobRemMgrChk  = rep->imobRemMgrChk;
    res.imobDataRemMgrChk0  = rep->imobDataRemMgrChk0;
    res.imobDataRemMgrChk1  = rep->imobDataRemMgrChk1;
    res.imobDataRemMgrChk2  = rep->imobDataRemMgrChk2;
    res.imobDataRemMgrChk3  = rep->imobDataRemMgrChk3;
    res.imobDataRemMgrChk4  = rep->imobDataRemMgrChk4;
    SendEventMessage(EVENT_RMTENGINESECURITYRANDOM, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}

////////////////////////////////////////////////////////////
// @brief : Handle remote start engine cert result
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResRMTEngineCertResult(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(Res_RMTEngineSecurityResult), data->size, 0);
    const Res_RMTEngineSecurityResult *rep = reinterpret_cast<const Res_RMTEngineSecurityResult *>(data->buffer);
    // imobVehRemMgrSts
    if (ds_->ChangeIntParameter("remoteEngineCertResult", "imobVehRemMgrSts", rep->imobVehRemMgrSts, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "imobVehRemMgrSts");
    }
    // spdLimRemMgrSts
    if (ds_->ChangeIntParameter("remoteEngineCertResult", "spdLimRemMgrSts", rep->spdLimRemMgrSts, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "spdLimRemMgrSts");
    }
    Res_RMTEngineSecurityResult res;
    res.imobVehRemMgrSts  = rep->imobVehRemMgrSts;
    res.spdLimRemMgrSts     = rep->spdLimRemMgrSts;
    SendEventMessage(EVENT_RMTENGINESECURITYRESULT, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}


////////////////////////////////////////////////////////////
// @brief : Handle remote start engine data request
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResRMTEngine(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(RMT_EngineState), data->size, 0);
    const RMT_EngineState *rep = reinterpret_cast<const RMT_EngineState *>(data->buffer);
    // engSt1WdSts
    if (ds_->ChangeIntParameter("RMT_EngineStates", "engSt1WdSts", rep->engSt1WdSts, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "engSt1WdSts");
    }
    //ersStrtApplSts
    if (ds_->ChangeIntParameter("RMT_EngineStates", "ersStrtApplSts", rep->ersStrtApplSts, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "ersStrtApplSts");
    }
    //ersStrtRes
    if (ds_->ChangeIntParameter("RMT_EngineStates", "ersStrtRes", rep->ersStrtRes, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "ersStrtRes");
    }
    Res_RMTEngine res;
    res.ersStrtApplSts  = (ErsStrtApplSts)rep->ersStrtApplSts;
    res.engSt1WdSts     = (EngStlWdSts)rep->engSt1WdSts;
    res.ersStrtRes      = (ErsStrtRes)rep->ersStrtRes;
    SendEventMessage(EVENT_RMTENGINE, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
    /*LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(0, data->size, REQ_RMTENGINE);
    Res_RMTEngine res;
    FinishRequest(REQ_RMTENGINE, RET_OK, reinterpret_cast<const unsigned char *>(&res));*/
}

////////////////////////////////////////////////////////////
// @brief : Handle delay engine running time IPC
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResDelayEngineRunngTime(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(EngineDelay), data->size, 0);
    const EngineDelay *rep = reinterpret_cast<const EngineDelay *>(data->buffer);
    // ersDelayTiCfm
    if (ds_->ChangeIntParameter("DelayEngine", "ersDelayTiCfm", rep->ersDelayTiCfm, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "ersDelayTiCfm");
    }
    Res_DelayEngRunngTime res;
    res.ersDelayTiCfm = (ErsDelayTiCfm)rep->ersDelayTiCfm;
    SendEventMessage(EVENT_DELAYENGINERUNNINGTIME, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}

////////////////////////////////////////////////////////////
// @brief : Handle operate remote climate start/stop IPC
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResOperateRemoteClimate(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
#if 1
    CHECK_SIZE(sizeof(Res_OperateRMTClimate), data->size, 0);
    const Res_OperateRMTClimate *rep = reinterpret_cast<const Res_OperateRMTClimate *>(data->buffer);
    // ersDelayTiCfm
    if (ds_->ChangeIntParameter("remoteClimate", "remStrtClimaActv", rep->remStrtClimaActv, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "ersDelayTiCfm");
    }
    // ersDelayTiCfm
    if (ds_->ChangeIntParameter("remoteClimate", "prkgClimaWarn", rep->prkgClimaWarn, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "ersDelayTiCfm");
    }
    Res_OperateRMTClimate res;
    res.remStrtClimaActv = rep->remStrtClimaActv;
    res.prkgClimaWarn = rep->prkgClimaWarn;
#elif 0
    CHECK_SIZE(sizeof(VucDoorState), data->size, 0);
    Res_OperateRMTClimate res;
    res.remStrtClimaActv = OnOff1_On;
    res.prkgClimaWarn = ClimaWarn_NoWarn;
#endif
    SendEventMessage(EVENT_OPERATEREMOTECLIMATE, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}

////////////////////////////////////////////////////////////
// @brief : Handle operate remote seatheat start/stop IPC
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResOperateRemoteSeatHeat(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(Res_OperateRMTSeatHeat_S), data->size, 0);
    const Res_OperateRMTSeatHeat_S *rep = reinterpret_cast<const Res_OperateRMTSeatHeat_S *>(data->buffer);
    // SeatHeatgAvlStsRowFrstLe
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgAvlStsRowFrstLe", rep->SeatHeatgAvlStsRowFrstLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgAvlStsRowFrstLe");
    }
    // SeatHeatgLvlStsRowFrstLe
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgLvlStsRowFrstLe", rep->SeatHeatgLvlStsRowFrstLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgLvlStsRowFrstLe");
    }
    // SeatHeatgAvlStsRowFrstRi
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgAvlStsRowFrstRi", rep->SeatHeatgAvlStsRowFrstRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgAvlStsRowFrstRi");
    }
    // SeatHeatgLvlStsRowFrstRi
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgLvlStsRowFrstRi", rep->SeatHeatgLvlStsRowFrstRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgLvlStsRowFrstRi");
    }
    // SeatHeatgAvlStsRowSecLe
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgAvlStsRowSecLe", rep->SeatHeatgAvlStsRowSecLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgAvlStsRowSecLe");
    }
    // SeatHeatgLvlStsRowSecLe
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgLvlStsRowSecLe", rep->SeatHeatgLvlStsRowSecLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgLvlStsRowSecLe");
    }
    // SeatHeatgAvlStsRowSecRi
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgAvlStsRowSecRi", rep->SeatHeatgAvlStsRowSecRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgAvlStsRowSecRi");
    }
    // SeatHeatgLvlStsRowSecRi
    if (ds_->ChangeIntParameter("remoteSeatHeat", "SeatHeatgLvlStsRowSecRi", rep->SeatHeatgLvlStsRowSecRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatHeatgLvlStsRowSecRi");
    }
    Res_OperateRMTSeatHeat_S res;
    res.SeatHeatgAvlStsRowFrstLe = rep->SeatHeatgAvlStsRowFrstLe;
    res.SeatHeatgLvlStsRowFrstLe = rep->SeatHeatgLvlStsRowFrstLe;

    res.SeatHeatgAvlStsRowFrstRi = rep->SeatHeatgAvlStsRowFrstRi;
    res.SeatHeatgLvlStsRowFrstRi = rep->SeatHeatgLvlStsRowFrstRi;

    res.SeatHeatgAvlStsRowSecLe = rep->SeatHeatgAvlStsRowSecLe;
    res.SeatHeatgLvlStsRowSecLe = rep->SeatHeatgLvlStsRowSecLe;

    res.SeatHeatgAvlStsRowSecRi = rep->SeatHeatgAvlStsRowSecRi;
    res.SeatHeatgLvlStsRowSecRi = rep->SeatHeatgLvlStsRowSecRi;
    SendEventMessage(EVENT_OPERATERMTSEATHEAT, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}

////////////////////////////////////////////////////////////
// @brief : Handle operate remote seatVenti start/stop IPC
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResOperateRemoteSeatVenti(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(Res_OperateRMTSeatVenti_S), data->size, 0);
    const Res_OperateRMTSeatVenti_S *rep = reinterpret_cast<const Res_OperateRMTSeatVenti_S *>(data->buffer);
    // SeatVentAvlStsRowFrstLe
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentAvlStsRowFrstLe", rep->SeatVentAvlStsRowFrstLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentAvlStsRowFrstLe");
    }
    // SeatVentnLvlStsRowFrstLe
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentnLvlStsRowFrstLe", rep->SeatVentnLvlStsRowFrstLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentnLvlStsRowFrstLe");
    }
    // SeatVentAvlStsRowFrstRi
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentAvlStsRowFrstRi", rep->SeatVentAvlStsRowFrstRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentAvlStsRowFrstRi");
    }
    // SeatVentnLvlStsRowFrstRi
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentnLvlStsRowFrstRi", rep->SeatVentnLvlStsRowFrstRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentnLvlStsRowFrstRi");
    }
    // SeatVentAvlStsRowSecLe
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentAvlStsRowSecLe", rep->SeatVentAvlStsRowSecLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentAvlStsRowSecLe");
    }
    // SeatVentnLvlStsRowSecLe
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentnLvlStsRowSecLe", rep->SeatVentnLvlStsRowSecLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentnLvlStsRowSecLe");
    }
    // SeatVentAvlStsRowSecRi
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentAvlStsRowSecRi", rep->SeatVentAvlStsRowSecRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentAvlStsRowSecRi");
    }
    // SeatVentnLvlStsRowSecRi
    if (ds_->ChangeIntParameter("remoteSeatVenti", "SeatVentnLvlStsRowSecRi", rep->SeatVentnLvlStsRowSecRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SeatVentnLvlStsRowSecRi");
    }
    Res_OperateRMTSeatVenti_S res;
    res.SeatVentAvlStsRowFrstLe = rep->SeatVentAvlStsRowFrstLe;
    res.SeatVentnLvlStsRowFrstLe = rep->SeatVentnLvlStsRowFrstLe;

    res.SeatVentAvlStsRowFrstRi = rep->SeatVentAvlStsRowFrstRi;
    res.SeatVentnLvlStsRowFrstRi = rep->SeatVentnLvlStsRowFrstRi;

    res.SeatVentAvlStsRowSecLe = rep->SeatVentAvlStsRowSecLe;
    res.SeatVentnLvlStsRowSecLe = rep->SeatVentnLvlStsRowSecLe;

    res.SeatVentAvlStsRowSecRi = rep->SeatVentAvlStsRowSecRi;
    res.SeatVentnLvlStsRowSecRi = rep->SeatVentnLvlStsRowSecRi;
    SendEventMessage(EVENT_OPERATERMTSEATVENTILATION, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}

////////////////////////////////////////////////////////////
// @brief : Handle operate remote steer wheel heat start/stop IPC
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResOperateRemoteSteerWhlHeat(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(Res_OperateRMTSteerWhlHeat_S), data->size, 0);
    const Res_OperateRMTSteerWhlHeat_S *rep = reinterpret_cast<const Res_OperateRMTSteerWhlHeat_S *>(data->buffer);
    // SteerWhlHeatgAvlSts
    if (ds_->ChangeIntParameter("remoteSteerWhlHeat", "SteerWhlHeatgAvlSts", rep->SteerWhlHeatgAvlSts, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "SteerWhlHeatgAvlSts");
    }
    Res_OperateRMTSteerWhlHeat_S res;
    res.SteerWhlHeatgAvlSts = rep->SteerWhlHeatgAvlSts;
    SendEventMessage(EVENT_OPERATERMTSTEERWHLHEAT, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}

////////////////////////////////////////////////////////////
// @brief : Handle operate parking climate start/stop IPC
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     uia93888, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResParkingClimate(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    CHECK_SIZE(sizeof(Res_ParkingClimateOper_S), data->size, 0);
    const Res_ParkingClimateOper_S *rep = reinterpret_cast<const Res_ParkingClimateOper_S *>(data->buffer);
    // prkgClimaWarn
    if (ds_->ChangeIntParameter("parkingClimate", "prkgClimaWarn", rep->prkgClimaWarn, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "prkgClimaWarn");
    }
    // climaActv
    if (ds_->ChangeIntParameter("parkingClimate", "climaActv", rep->climaActv, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "climaActv");
    }
    Res_ParkingClimateOper_S res;
    res.prkgClimaWarn = rep->prkgClimaWarn;
    res.climaActv = rep->climaActv;
    SendEventMessage(EVENT_PARKINGCLIMATEOPER, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
}



////////////////////////////////////////////////////////////
// @brief : Handle door state data response
//
// @param[in]  data   	Payload from VuC
// @return     void
// @author     ty, Sep 17, 2018
////////////////////////////////////////////////////////////
void VuCComm::HandleResDoorLockUnlockState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);

    CHECK_SIZE(sizeof(VucDoorState), data->size, 0);
    const VucDoorState *rep = reinterpret_cast<const VucDoorState *>(data->buffer);

    // driverDoorState
    if (ds_->ChangeIntParameter("DoorState", "driverDoorState", rep->driverDoorState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "driverDoorState");
    }
    // driverSideRearDoorState
    if (ds_->ChangeIntParameter("DoorState", "driverSideRearDoorState", rep->driverSideRearDoorState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "driverSideRearDoorState");
    }
    // driverDoorLockState
    if (ds_->ChangeIntParameter("DoorState", "driverDoorLockState", rep->driverDoorLockState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "driverDoorLockState");
    }
    // driverSideRearDoorLockState
    if (ds_->ChangeIntParameter("DoorState", "driverSideRearDoorLockState", rep->driverSideRearDoorLockState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "driverSideRearDoorLockState");
    }
    // passengerDoorState
    if (ds_->ChangeIntParameter("DoorState", "passengerDoorState", rep->passengerDoorState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "passengerDoorState");
    }
    // passengerSideRearDoorState
    if (ds_->ChangeIntParameter("DoorState", "passengerSideRearDoorState", rep->passengerSideRearDoorState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "passengerSideRearDoorState");
    }
    // passengerDoorLockState
    if (ds_->ChangeIntParameter("DoorState", "passengerDoorLockState", rep->passengerDoorLockState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "passengerDoorLockState");
    }
    // passengerSideRearDoorLockState
    if (ds_->ChangeIntParameter("DoorState", "passengerSideRearDoorLockState", rep->passengerSideRearDoorLockState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "passengerSideRearDoorLockState");
    }
    // tailgateOrBootlidState
    if (ds_->ChangeIntParameter("DoorState", "tailgateOrBootlidState", rep->tailgateOrBootlidState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "tailgateOrBootlidState");
    }
    // tailgateOrBootlidLockState
    if (ds_->ChangeIntParameter("DoorState", "tailgateOrBootlidLockState", rep->tailgateOrBootlidLockState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "tailgateOrBootlidLockState");
    }
    // hoodState
    if (ds_->ChangeIntParameter("DoorState", "hoodState", rep->hoodState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "hoodState");
    }
    // centralLockingStateForUserFeedback
    if (ds_->ChangeIntParameter("DoorState", "centralLockingStateForUserFeedback", rep->centralLockingStateForUserFeedback, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "centralLockingStateForUserFeedback");
    }
    // centralLockState
    if (ds_->ChangeIntParameter("DoorState", "centralLockState", rep->centralLockState, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "centralLockState");
    }
    // centralLockStateTrigSource
    if (ds_->ChangeIntParameter("DoorState", "centralLockStateTrigSource", rep->centralLockStateTrigSource, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "centralLockStateTrigSource");
    }
    // centralLockStateNewEvent
    if (ds_->ChangeIntParameter("DoorState", "centralLockStateNewEvent", rep->centralLockStateNewEvent, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "centralLockStateNewEvent");
    }

    EventDoorLockState event;
    event.driverDoorState = rep->driverDoorState;
    event.driverSideRearDoorState = rep->driverSideRearDoorState;
    event.driverDoorLockState = rep->driverDoorLockState;
    event.driverSideRearDoorLockState = rep->driverSideRearDoorLockState;
    event.passengerDoorState = rep->passengerDoorState;
    event.passengerSideRearDoorState = rep->passengerSideRearDoorState;
    event.passengerDoorLockState = rep->passengerDoorLockState;
    event.passengerSideRearDoorLockState = rep->passengerSideRearDoorLockState;
    event.tailgateOrBootlidState = rep->tailgateOrBootlidState;
    event.tailgateOrBootlidLockState = rep->tailgateOrBootlidLockState;
    event.hoodState = rep->hoodState;
    event.centralLockingStateForUserFeedback = rep->centralLockingStateForUserFeedback;
    event.centralLockState = rep->centralLockState;
    event.centralLockStateTrigSource = rep->centralLockStateTrigSource;
    event.centralLockStateNewEvent = rep->centralLockStateNewEvent;
    
    SendEventMessage(EVENT_DOORLOCKSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle Sun roof and windows state

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvSunroofWinState(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    
    CHECK_SIZE(sizeof(VucWinOpenState), data->size, 0);
    const VucWinOpenState *rep = reinterpret_cast<const VucWinOpenState *>(data->buffer);

    // sunroofPosnSts
    if (ds_->ChangeIntParameter("WinState", "sunroofPosnSts", rep->sunroofPosnSts, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "sunroofPosnSts");
    }
    // winPosnStsAtDrv
    if (ds_->ChangeIntParameter("WinState", "winPosnStsAtDrv", rep->winPosnStsAtDrv, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "winPosnStsAtDrv");
    }
    // winPosnStsAtPass
    if (ds_->ChangeIntParameter("WinState", "winPosnStsAtPass", rep->winPosnStsAtPass, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "winPosnStsAtPass");
    }
    // winPosnStsReLe
    if (ds_->ChangeIntParameter("WinState", "winPosnStsReLe", rep->winPosnStsReLe, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "winPosnStsReLe");
    }
    // winPosnStsReRi
    if (ds_->ChangeIntParameter("WinState", "winPosnStsReRi", rep->winPosnStsReRi, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "winPosnStsReRi");
    }

    EventWinOpenState event;
    event.sunroofPosnSts = rep->sunroofPosnSts;
    event.winPosnStsAtDrv = rep->winPosnStsAtDrv;
    event.winPosnStsAtPass = rep->winPosnStsAtPass;
    event.winPosnStsReLe = rep->winPosnStsReLe;
    event.winPosnStsReRi = rep->winPosnStsReRi;
    
    SendEventMessage(EVENT_WINOPENSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

/**
    @brief Handle PM 2.5 data

    @param[in]  data           Payload from VuC
*/
void VuCComm::HandleEvPM25EnquiresReport(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm %s: ", __FUNCTION__);
    
    CHECK_SIZE(sizeof(VucPMLevelState), data->size, 0);
    const VucPMLevelState *rep = reinterpret_cast<const VucPMLevelState *>(data->buffer);
    LOG(LOG_DEBUG, "VuCComm %s inLvl = %d, outLvl = %d: inHigh = %d, inLow = %d, outHigh = %d, outLow = %d", 
        __FUNCTION__, rep->IntPm25LvlFrmClima, rep->OutdPm25LvlFrmClima, 
        rep->IntPm25VluFrmClimaHigh, rep->IntPm25VluFrmClimaLow, 
        rep->OutdPm25VluFrmClimaHigh, rep->OutdPm25VluFrmClimaLow);

    EventPMLevelState event;
    event.intPm25Lvl = rep->IntPm25LvlFrmClima;
    event.outdPm25Lvl = rep->OutdPm25LvlFrmClima;
    event.intPm25Vlu = (rep->IntPm25VluFrmClimaHigh<<8) +  rep->IntPm25VluFrmClimaLow;
    event.outdPm25Vlu = (rep->OutdPm25VluFrmClimaHigh<<8) + rep->OutdPm25VluFrmClimaLow;

    // cmpmtInAirPMLvl
    if (ds_->ChangeIntParameter("CmpmtAirState", "cmpmtInAirPMLvl", event.intPm25Lvl, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "cmpmtInAirPMLvl");
    }
    // cmpmtOutAirPMLvl
    if (ds_->ChangeIntParameter("CmpmtAirState", "cmpmtOutAirPMLvl", event.outdPm25Lvl, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "cmpmtOutAirPMLvl");
    }
    // intPm25Vlu
    if (ds_->ChangeIntParameter("CmpmtAirState", "intPm25Vlu", event.intPm25Vlu, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "intPm25Vlu");
    }
    // outdPm25Vlu
    if (ds_->ChangeIntParameter("CmpmtAirState", "outdPm25Vlu", event.outdPm25Vlu, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set parameter '%s'!", __FUNCTION__, "outdPm25Vlu");
    }

    SendEventMessage(EVENT_PMLVL, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
}

void VuCComm::HandleResRolloverValue(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);
    CHECK_SIZE(sizeof(ResRolloverValue), data->size, RES_ROLLOVERVALUE);
    const ResRolloverValue *res = reinterpret_cast<const ResRolloverValue *>(data->buffer);

    LOG(LOG_DEBUG, "VuCComm Rollover config = %d, length = %d: data = %s", res->configItem, res->length, res->data);

    FinishRequest(REQ_ROLLOVERVALUE, RET_OK, reinterpret_cast<const unsigned char *>(res));
}

void VuCComm::HandleResSetRolloverCfg(const tpVCS_CustomMessage_t *data)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);
    CHECK_SIZE(sizeof(ResSetRolloverCfg), data->size, RES_SETROLLOVERCFG);
    const ResSetRolloverCfg *res = reinterpret_cast<const ResSetRolloverCfg *>(data->buffer);

    LOG(LOG_DEBUG, "VuCComm Rollover config = %d, result = %d", res->configItem, res->result);

    FinishRequest(REQ_SETROLLOVERCFG, RET_OK, reinterpret_cast<const unsigned char *>(res));
}

// Public

/**
    @brief Called from TCAM-INC when recevied custom message from VuC

    @param[in]  messageID      VuCMessageID
    @param[in]  data           Payload and size of message
*/
void VuCComm::CustomMessageCallback(const guint32 messageId, const tpVCS_CustomMessage_t data)
{
    switch(messageId) {
    case IPC_VUC_DOOR_LOCK_UNLOCK_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_DOOR_LOCK_UNLOCK_RESP received.", __FUNCTION__);
        HandleResDoorLockUnlockState(&data);
        break;
    }
    case IPC_VUC_HORN_LIGHT_STATE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_HORN_LIGHT_STATE_REP received.", __FUNCTION__);
        HandleEvHornAndLight(&data);
        break;
    }
    case IPC_VUC_SOFTWARE_BUILD_DATE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_SOFTWARE_BUILD_DATE_RESP received.", __FUNCTION__);
        HandleResVucSwBuild(&data);
        break;
    }
    case IPC_VUC_BLE_SOFTWARE_BUILD_DATE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_SOFTWARE_BUILD_DATE_RESP received.", __FUNCTION__);
        HandleResBleSwBuild(&data);
        break;
    }
    case IPC_VUC_TRACE_REP: {
        // Dont log this happens to often
        HandleEvVucTrace(&data);
        break;
    }
    case IPC_VUC_DIAG_ERR_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_DIAG_ERR_REP received.", __FUNCTION__);
        HandleEvVucDiagErr(&data);
        break;
    }
    case IPC_VUC_ELECT_ENG_LVL_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_ELECT_ENG_LVL_REP received.", __FUNCTION__);
        HandleEvElecEngLvl(&data);
        break;
    }
    case IPC_VUC_ELECT_ENG_LVL_RESPONSE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_ELECT_ENG_LVL_RESPONSE_RESP received.", __FUNCTION__);
        HandleResElecEngLvl(&data);
        break;
    }
    case IPC_VUC_BUB_STATE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BUB_STATE_REP received.", __FUNCTION__);
        HandleEvBubState(&data);
        break;
    }
    case IPC_VUC_BUB_STATE_RESPONSE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BUB_STATE_RESPONSE_RESP received.", __FUNCTION__);
        HandleResBubState(&data);
        break;
    }
    case IPC_VUC_CAR_CFG_PARAM_FAULT_RESP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CAR_CFG_PARAM_FAULT_RESP received.", __FUNCTION__);
        HandleResCarCfgFaults(&data);
    break;
}
    case IPC_VUC_NETWORK_MANAGEMENT_HISTORY_RESP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_NETWORK_MANAGEMENT_HISTORY_RESP received.", __FUNCTION__);
        HandleResNetworkManageHistory(&data);
    break;
}
    case IPC_VUC_PROGRAM_PRECOND_CHK_RESP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_PROGRAM_PRECOND_CHK_RESP received.", __FUNCTION__);
        HandleResProgramPreCheck(&data);
    break;
}
    case IPC_VUC_VEHICLE_GEN_STATE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VEHICLE_GEN_STATE_REP received.", __FUNCTION__);
        HandleEvVehicleGenState(&data);
        break;
    }
    case IPC_VUC_VEHICLE_GEN_STATE_RESPONSE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VEHICLE_GEN_STATE_RESPONSE_RESP received.", __FUNCTION__);
        HandleResVehicleGenState(&data);
        break;
    }
    case IPC_VUC_CAR_CONFIG_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CAR_CONFIG_RESP received.", __FUNCTION__);
        HandleResGetCarConfig(&data);
        break;
    }
    case IPC_VUC_CAR_CONFIG_REPORT_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CAR_CONFIG_REPORT_REP received.", __FUNCTION__);
        HandleEvGetCarConfig(&data);
        break;
    }
    case IPC_VUC_CAR_CONFIG_FACTORY_RESTORE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CAR_CONFIG_FACTORY_RESTORE_RESP received.", __FUNCTION__);
        HandleResCarConfigFactRest(&data);
        break;
    }
    case IPC_VUC_ANTENNA_SWITCH_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_ANTENNA_SWITCH_RESP received.", __FUNCTION__);
        HandleResAntennaSwitch(&data);
        break;
    }
    case IPC_VUC_DIAG_ERR_RESPONSE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_DIAG_ERR_RESPONSE_RESP received.", __FUNCTION__);
        HandleResDiagErr(&data);
        break;
    }
    case IPC_VUC_BLE_DISCONNECT_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_DISCONNECT_RESP received.", __FUNCTION__);
        HandleResBleDisconnect(&data);
        break;
    }
    case IPC_VUC_BLE_DISCONNECT_ALL_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_DISCONNECT_ALL_RESP received.", __FUNCTION__);
        HandleResBleDisconnectAll(&data);
        break;
    }
    case IPC_VUC_BLE_GENERATE_BDAK_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_GENERATE_BDAK_RESP received.", __FUNCTION__);
        HandleResBleGenBdak(&data);
        break;
    }
    case IPC_VUC_BLE_GET_BDAK_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_GET_BDAK_RESP received.", __FUNCTION__);
        HandleResBleGetBdak(&data);
        break;
    }
    case IPC_VUC_BLE_CONFIG_DATACOM_SERVICE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_CONFIG_DATACOM_SERVICE_RESP received.", __FUNCTION__);
        HandleResBleCfgDataServ(&data);
        break;
    }
    case IPC_VUC_BLE_CONFIG_IBEACON_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_CONFIG_IBEACON_RESP received.", __FUNCTION__);
        HandleResBleCfgIbeacon(&data);
        break;
    }
    case IPC_VUC_BLE_CONFIG_GENERIC_ACCESS_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_CONFIG_GENERIC_ACCESS_RESP received.", __FUNCTION__);
        HandleResBleCfgGenAcc(&data);
        break;
    }
    case IPC_VUC_BLE_START_PARING_SERVICE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_START_PARING_SERVICE_RESP received.", __FUNCTION__);
        HandleResBleStartService(&data);
        break;
    }
    case IPC_VUC_BLE_STOP_PARING_SERVICE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_STOP_PARING_SERVICE_RESP received.", __FUNCTION__);
        HandleResBleStopService(&data);
        break;
    }
    case IPC_VUC_BLE_CONNECTION_STATUS_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_CONNECTION_STATUS_REP received.", __FUNCTION__);
        HandleEvBleConnStatus(&data);
        break;
    }
    case IPC_VUC_BLE_RESET_NOTIFICATION_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_RESET_NOTIFICATION_REP received.", __FUNCTION__);
        HandleEvBleResetNotification(&data);
        break;
    }
    case IPC_VUC_BLE_START_ADVERTISING_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_START_ADVERTISING_RESP received.", __FUNCTION__);
        HandleResBleStartAdvertising(&data);
        break;
    }
    case IPC_VUC_BLE_STOP_ADVERTISING_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_STOP_ADVERTISING_RESP received.", __FUNCTION__);
        HandleResBleStopAdvertising(&data);
        break;
    }
    case IPC_VUC_BLE_MTU_SIZE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BLE_MTU_SIZE_REP received.", __FUNCTION__);
        HandleEvBleMtuSize(&data);
        break;
    }
    case IPC_VUC_DTC_HANDLING_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_DTC_HANDLING_RESP received.", __FUNCTION__);
        HandleResDtcHandling(&data);
        break;
    }
    case IPC_VUC_FULL_DTC_ENTRY_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_FULL_DTC_ENTRY_REP received.", __FUNCTION__);
        HandleEvDtcFullEntry(&data);
        break;
    }
    case IPC_VUC_DTC_STATUS_ENTRY_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_DTC_STATUS_ENTRY_REP received.", __FUNCTION__);
        HandleEvDtcStatusEntry(&data);
        break;
    }
    case IPC_VUC_AMPLIFIER_STATE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_AMPLIFIER_STATE_REP received.", __FUNCTION__);
        HandleEvAmplifierState(&data);
        break;
    }
    case IPC_VUC_POWER_MODE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_POWER_MODE_REP received.", __FUNCTION__);
        HandleEvPowerMode(&data);
        break;
    }
    case IPC_VUC_OHC_BUTTON_STATE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_OHC_BUTTON_STATE_REP received.", __FUNCTION__);
        HandleEvOhcBtnState(&data);
        break;
    }
    case IPC_VUC_OHC_BUTTON_STATE_RESPONSE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_OHC_BUTTON_STATE_RESPONSE_RESP received.", __FUNCTION__);
        HandleResOhcBtnState(&data);
        break;
    }
    case IPC_VUC_VIN_CUSTOM_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VIN_CUSTOM_RESP received.", __FUNCTION__);
        HandleResVINNumber(&data);
        break;
    }
    case IPC_VUC_VIN_CUSTOM_REPORT_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VIN_CUSTOM_REPORT_REP received.", __FUNCTION__);
        HandleEvVINNumber(&data);
        break;
    }
    case IPC_VUC_MESSAGE_CATALOGUE_VERSION_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_MESSAGE_CATALOGUE_VERSION_RESP received.", __FUNCTION__);
        HandleResMsgCatVersion(&data);
        break;
    }
    case IPC_VUC_TCAM_HARDWARE_VERSION_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_TCAM_HARDWARE_VERSION_RESP received.", __FUNCTION__);
        HandleResTcamHwVersion(&data);
        break;
    }
    case IPC_VUC_VUC_BLE_SW_PARTNUMBER_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VUC_BLE_SW_PARTNUMBER_RESP received.", __FUNCTION__);
        HandleResVucBleSwPartNumber(&data);
        break;
    }
    case IPC_VUC_RF_SW_HW_VERSION_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_RF_SW_HW_VERSION_RESP received.", __FUNCTION__);
        HandleResRfSwHwVersion(&data);
        break;
    }
    case IPC_VUC_RF_KLINE_ERROR_CODES_REP: {
        //LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_RF_KLINE_ERROR_CODES_REP received.", __FUNCTION__);
        //HandleEvRfKlineErrorCodes(&data);
        break;
    }
    case IPC_VUC_FBL_SOFTWARE_BUILD_DATE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_FBL_SOFTWARE_BUILD_DATE_RESP received.", __FUNCTION__);
        HandleResFblSwBuild(&data);
        break;
    }
    case IPC_VUC_BELT_INFO_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_BELT_INFO_RESP received.", __FUNCTION__);
        HandleResBeltInformation(&data);
        break;
    }
    case IPC_VUC_VEHICLE_SPEED_STATE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VEHICLE_SPEED_STATE_REP received.", __FUNCTION__);
        HandleEvVehicleSpeedState(&data);
        break;
    }
    case IPC_VUC_CAN_VOLTAGE_ERROR_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CAN_VOLTAGE_ERROR_REP received.", __FUNCTION__);
        HandleEvCanVoltageErr(&data);
        break;
    }
    case IPC_VUC_LOCAL_CONFIG_PARAM_REQ: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_LOCAL_CONFIG_PARAM_REQ received.", __FUNCTION__);
        HandleReqLocalCfgParam(&data);
        break;
    }
    case IPC_VUC_DIAG_COUPLE_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_DIAG_COUPLE_REP received.", __FUNCTION__);
        HandleEvDiagCoupleReport(&data);
        break;
    }
    case IPC_VUC_GLOBAL_SS_DATA_DID_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_GLOBAL_SS_DATA_DID_RESP received.", __FUNCTION__);
        HandleResDIDGlobalSnapshotData(&data);
        break;
    }
    case IPC_VUC_SOC_DID_STATUS_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_SOC_DID_STATUS_RESP received.", __FUNCTION__);
        HandleResDTCsDetected(&data);
        break;
    }
    case IPC_VUC_PSIM_DATA_COMMUNICATION_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_PSIM_DATA_COMMUNICATION_RESP received.", __FUNCTION__);
        HandleResPSIMData(&data);
        break;
    }
    case IPC_VUC_THERMAL_MITIGATION_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_THERMAL_MITIGATION_RESP received.", __FUNCTION__);
        HandleResThermalMitigation(&data);
        break;
    }
    case IPC_VUC_SUNROOF_WIN_STATE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_SUNROOF_WIN_STATE_RESP received.", __FUNCTION__);
        HandleEvSunroofWinState(&data);
        break;
    }
    case IPC_VUC_PM25_ENQUIRES_REPORT_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_PM25_ENQUIRES_REPORT_RESP received.", __FUNCTION__);
        HandleEvPM25EnquiresReport(&data);
        break;
    }
    case IPC_VUC_VFC_RESPONSE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_VFC_RESPONSE_RESP received.", __FUNCTION__);
        HandleResVFCActivate(&data);
        break;
    }
    case IPC_VUC_ENGINE_REM_START_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_ENGINE_REM_START_RESP received.", __FUNCTION__);
        HandleResRMTEngine(&data);
        break;
    }
    case IPC_VUC_CHALLENGE_CODES_TCAM_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CHALLENGE_CODES_TCAM_REP received.", __FUNCTION__);
        //TODO
        break;
    }
    case IPC_VUC_CEM_CERT_RESULT_TCAM_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_CEM_CERT_RESULT_TCAM_REP received.", __FUNCTION__);
        //TODO
        break;
    }
    case IPC_VUC_REM_ENG_RUNNING_DELAY_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_REM_ENG_RUNNING_DELAY_REP received.", __FUNCTION__);
        HandleResDelayEngineRunngTime(&data);
        break;
    }
    case IPC_VUC_REMOTE_CLIMATIZATION_REP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_REMOTE_CLIMATIZATION_REP received.", __FUNCTION__);
        HandleResOperateRemoteClimate(&data);
    break;
    }
    case IPC_VUC_REM_SEAT_HEATING_REP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_REM_SEAT_HEATING_REP received.", __FUNCTION__);
        HandleResOperateRemoteSeatHeat(&data);
        break;
    }
    case IPC_VUC_REM_SEAT_VENTILATION_REP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_REM_SEAT_VENTILATION_REP received.", __FUNCTION__);
        HandleResOperateRemoteSeatVenti(&data);
        break;
    }
    case IPC_VUC_REM_STEERING_WHEEL_HEATING_CTRL_REP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_REM_SEAT_VENTILATION_REP received.", __FUNCTION__);
        HandleResOperateRemoteSteerWhlHeat(&data);
        break;
    }
    case IPC_VUC_PARKING_CLIMA_CTRL_REP:{
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_PARKING_CLIMA_CTRL_REP received.", __FUNCTION__);
        HandleResParkingClimate(&data);
        break;
    }
    case IPC_VUC_ROLLOVER_PARAM_VALUE_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_ROLLOVER_PARAM_VALUE_RESPs received [%d].", 
            __FUNCTION__, IPC_VUC_ROLLOVER_PARAM_VALUE_RESP);
        HandleResRolloverValue(&data);
    }
    case IPC_VUC_ROLLOVER_PARAM_CFG_RESP: {
        LOG(LOG_INFO, "VuCComm: %s: IPC_VUC_ROLLOVER_PARAM_CFG_RESP received [%d].", 
            __FUNCTION__, IPC_VUC_ROLLOVER_PARAM_CFG_RESP);
        HandleResSetRolloverCfg(&data);
        break;
    }
    default: {
        LOG(LOG_INFO, "VuCComm: %s: Unknown message (vuc_message_id = %d)!", __FUNCTION__, messageId);
    }
    }
}
VuCComm::VuCComm(MessageQueue *mq, DataStorage *ds)
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    mq_ = mq;
    ds_ = ds;
}

VuCComm::~VuCComm()
{
    tpVCS_Error_t tpVCS_ret = E_VCS_ERROR_NONE;

    VUCTRACE_DEINIT();

    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    tpVCS_ret = tpVCS_deregisterCustomMessageCallback();
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to de-register CustomMessageCallback (error = %d)!", tpVCS_ret);
    }

    tpVCS_ret = tpVCS_deregisterSignalIntCallback(VCS_SIGNAL_INT_WAKEUP_REASON_VALUE);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to de-register SignalIntCallback(%s): (error = %d)!",
                        VCS_SIGNAL_INT_WAKEUP_REASON_VALUE, tpVCS_ret);
    }

    tpVCS_ret = tpVCS_deregisterSignalStringCallback(VCS_SIGNAL_STR_VIN_VALUE);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to de-register SignalStringCallback(%s): (error = %d)!",
                        VCS_SIGNAL_STR_VIN_VALUE, tpVCS_ret);
    }

    tpVCS_ret = tpVCS_deregisterCrashMessageCallback();
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to de-register CrashMessageCallback (error = %d)!", tpVCS_ret);
    }

    tpVCS_ret = tpVCS_deinit();
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to de-init VCS (error = %d)!", tpVCS_ret);
    }
}

/**
    @brief Initialize VuCComm.

    Do all initialization stuff needed for VuCComm.
*/
ReturnValue VuCComm::Init()
{
    ReturnValue ret;
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    tpVCS_Error_t tpVCS_ret = E_VCS_ERROR_NONE;

    // Init VCS
    tpVCS_ret = tpVCS_init();
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to init VCS (error = %d)!", tpVCS_ret);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VuCComm: VCS initialized.");

    tpVCS_ret = tpVCS_registerCustomMessageCallback(VuCComm::CustomMessageCallback);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to register CustomMessageCallback (error = %d)!", tpVCS_ret);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VuCComm: CustomMessageCallback registered.");

    tpVCS_ret = tpVCS_registerSignalIntCallback(VCS_SIGNAL_INT_WAKEUP_REASON_VALUE, VuCComm::SignalIntCallback);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to register SignalIntCallback VCS_SIGNAL_INT_WAKEUP_REASON_VALUE (error = %d)!", tpVCS_ret);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VuCComm: SignalIntCallback VCS_SIGNAL_INT_WAKEUP_REASON_VALUE registered.");

    tpVCS_ret = tpVCS_registerCrashMessageCallback(VuCComm::CrashMessageCallback);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to register CrashMessageCallback (error = %d)!", tpVCS_ret);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VuCComm: CrashMessageCallback registered.");

    tpVCS_ret = tpVCS_registerAdcResponseCallback(VuCComm::AdcResponseCallback);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_ERR, "VuCComm: Unable to register AdcResponseCallback (error = %d)!", tpVCS_ret);
        return RET_ERR_EXTERNAL;
    } else
        LOG(LOG_INFO, "VuCComm: AdcResponseCallback registered.");

    SessionCfgValues scfg = {};


    int value = RESPONSE_TRIES;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "vuc_response_tries", value))
        LOG(LOG_WARN, "VuCComm: local_config value for vuc_response_tries, not found using default!");
    scfg.response_tries = value;

    value = RESPONSE_TIMEOUT_MS;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "vuc_response_timeout_ms", value))
        LOG(LOG_WARN, "VuCComm: local_config value for vuc_response_timeout_ms, not found using default!");
    scfg.response_timeout_ms = value;

    value = TIME_RESOLUTION_MS;
    if (!lcfg::ILocalConfig::GetInstance()->GetInt("VehicleComm", "vuc_time_resolution_ms", value))
        LOG(LOG_WARN, "VuCComm: local_config value for vuc_time_resolution_ms, not found using default!");
    scfg.time_resolution_ms = value;

    ret = slist_.Init(mq_, WD_THREAD_VUCTIMER, scfg);
    if (ret != RET_OK)
        return ret;

    VUCTRACE_INIT("VUCT", "Vuc Trace logs");

    return RET_OK;
}

/**
    @brief Stop execution of VuCComm thread.

    Currently, VuCComm does not need its own thread.
    It also calls stop for VuCComs SessionList thread.

    @return     A VC return-value
*/
ReturnValue VuCComm::Stop()
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    slist_.Stop();

    return RET_OK;
}


/**
    @brief Start execution of VuCComm thread.

    Currently, VuCComm does not need its own thread.
    It also calls start for VuCComs SessionList thread.

    @return     A VC return-value
*/
ReturnValue VuCComm::Start()
{
    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);
    slist_.Start();
    return RET_OK;
}

/**
    @brief  Sends a vuc custom message

    @return     A VC return-value
*/
ReturnValue VuCComm::SendCustomMessage(MessageBase *m, tpVCS_CustomMessage_t *msg)
{
    VuCMessageID vuc_id = IPC_LAST_MESSAGE;
    if (m->type_ == MESSAGE_REQUEST) {
        if (VuCRequestLookup((RequestID)m->id_, vuc_id) != RET_OK)
            return RET_ERR_INTERNAL;
    }
    else if (m->type_ == MESSAGE_RESPONSE) {
        if (VuCResponseLookup((ResponseID)m->id_, vuc_id) != RET_OK)
            return RET_ERR_INTERNAL;
    }

    tpVCS_Error_t tpVCS_ret = tpVCS_sendCustomMessage(vuc_id, msg);
    LOG(LOG_DEBUG, "VuCComm: %s: Sending message %s." ,__FUNCTION__, VCRequestStr[m->id_]);
    if (tpVCS_ret != E_VCS_ERROR_NONE) {
        LOG(LOG_WARN, "VuCComm: Failed to send CustomMessage (error = %d) (vuc_id = %d)", tpVCS_ret, vuc_id);
        return RET_ERR_EXTERNAL;
    }
    return RET_OK;
}

/**
    @brief  Converts and sends a message to the vuc

    This method create a vuc message out of a Message
    and sends it.

    @return     A VC return-value
*/
ReturnValue VuCComm::SendVuCMessage(MessageBase *m)
{
    switch(m->type_) {
    case MESSAGE_REQUEST: {
        LOG(LOG_DEBUG, "VuCComm: %s: Sending message %s (unique_id = %d, src = %s, dst = %s).",
            __FUNCTION__, VCRequestStr[m->id_], m->unique_id_, EndpointStr[m->src_], EndpointStr[m->dst_]);
        switch((RequestID)m->id_) {
        case REQ_HORNNLIGHT: {
            tpVCS_CustomMessage_t msg;
            NadHornULightReqType req;
            req.hornUlight = reinterpret_cast<const ReqHornNLight*>(m->GetData())->mode;
            msg.size = sizeof(NadHornULightReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_SWITCHTOBOOT: {
            tpVCS_CustomMessage_t msg;
            NadSwitchToBootReqType req;
            req.mode = 1;
            req.reserved = 0x1a;
            msg.size = sizeof(NadSwitchToBootReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_ANTENNASWITCH: {
            tpVCS_CustomMessage_t msg;
            NadAntennaSwitchType req;
            req.SwCntrl = reinterpret_cast<const ReqAntennaSwitch*>(m->GetData())->mode;
            msg.size = sizeof(NadAntennaSwitchType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_LANLINKTRIGGER: {
            tpVCS_CustomMessage_t msg;
            NadLanLinkTrigReqType req;
            req.linkTrigger = reinterpret_cast<const ReqLanLinkTrigger*>(m->GetData())->trigger;
            req.DiagComSes = reinterpret_cast<const ReqLanLinkTrigger*>(m->GetData())->session_state;
            msg.size = sizeof(NadLanLinkTrigReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETCARCONFIG: {
            tpVCS_CustomMessage_t msg;
            NadCarConfigReqType req;
            req.paramId = reinterpret_cast<const ReqGetCarConfig*>(m->GetData())->param_id;
            msg.size = sizeof(NadCarConfigReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_CARCONFIGFACTRESTORE: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_ADCVALUE: {
            tpVCS_AdcId_t msg = static_cast<tpVCS_AdcId_t>(reinterpret_cast<const ReqAdcValue*>(m->GetData())->id);
            tpVCS_Error_t tpVCS_ret = tpVCS_sendAdcRequest(msg);
            if (tpVCS_ret != E_VCS_ERROR_NONE) {
                LOG(LOG_WARN, "VuCComm: Failed to send AdcRequest (error = %d)", tpVCS_ret);
                return RET_ERR_EXTERNAL;
            }
            return RET_OK;
        }
        case REQ_VUCTRACE_CONFIG: {
            tpVCS_CustomMessage_t msg;
            NadVucTraceType req;
            req.messageName = (uint8_t)(reinterpret_cast<const ReqVucTraceConfig*>(m->GetData())->bus);
            req.type = 2; //Request
            req.priority = 1; // Value specified by documentation
            req.fcodeid = 1; // Value specified by documentation
            for (uint8_t i = 0; i < sizeof(req.payload)/sizeof(uint8_t); i++) {
                req.payload[i] = (uint8_t)(reinterpret_cast<const ReqVucTraceConfig*>(m->GetData())->config[i].module_id) |
                                    (uint8_t)((reinterpret_cast<const ReqVucTraceConfig*>(m->GetData())->config[i].module_lvl) << 6);
            }
            msg.size = sizeof(NadVucTraceType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_VUCTRACE_CONTROL: {
            tpVCS_CustomMessage_t msg;
            NadVucTraceType req;
            req.messageName = 2; // TRACE_CONTROL
            req.type = 2; // Request
            req.priority = 2; // Value specified by documentation
            req.fcodeid = 3; // Value specified by documentation
            req.payload[0] = (uint8_t)(reinterpret_cast<const ReqVucTraceControl*>(m->GetData())->interval) << 2|
                                uint8_t(reinterpret_cast<const ReqVucTraceControl*>(m->GetData())->mode);
            msg.size = sizeof(NadVucTraceType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_DISCONNECT: {
            tpVCS_CustomMessage_t msg;
            NadBleDisconnectReqType req;
            req.connectionID = (reinterpret_cast<const ReqBleDisconnect*>(m->GetData())->connection_id & 0x3FFF);
            msg.size = sizeof(NadBleDisconnectReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_DISCONNECTALL: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_GENERATEBDAK: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_GETBDAK: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_CONFIG_DATACOMSERVICE: {
            tpVCS_CustomMessage_t msg;
            NadBleCfgDataCommServReqType req;
            std::memcpy(req.serviceUUID, reinterpret_cast<const ReqBleCfgDataCommService*>(m->GetData())->service_uuid, sizeof(req.serviceUUID));
            msg.size = sizeof(NadBleCfgDataCommServReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_CONFIG_IBEACON: {
            tpVCS_CustomMessage_t msg;
            NadBleCfgIbeaconReqType req;
            std::memcpy(req.proximityUUID, reinterpret_cast<const ReqBleCfgIbeacon*>(m->GetData())->proximity_uuid, sizeof(req.proximityUUID));
            req.major = reinterpret_cast<const ReqBleCfgIbeacon*>(m->GetData())->major;
            req.minor = reinterpret_cast<const ReqBleCfgIbeacon*>(m->GetData())->minor;
            msg.size = sizeof(NadBleCfgIbeaconReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_CONFIG_GENERICACCESS: {
            tpVCS_CustomMessage_t msg;
            NadBleCfgGenAccReqType req;
            std::memcpy(req.idString, reinterpret_cast<const ReqBleCfgGenAcc*>(m->GetData())->id_string, sizeof(req.idString));
            msg.size = sizeof(NadBleCfgGenAccReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_STARTPARINGSERVICE: {
            tpVCS_CustomMessage_t msg;
            NadBleStartPairServReqType req;
            req.serviceUUID = reinterpret_cast<const ReqBleStartPairService*>(m->GetData())->service_uuid;
            std::memcpy(req.nameCharString, reinterpret_cast<const ReqBleStartPairService*>(m->GetData())->name_char_string, sizeof(req.nameCharString));
            msg.size = sizeof(NadBleStartPairServReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_STOPPARINGSERVICE: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_STARTADVERTISING: {
            tpVCS_CustomMessage_t msg;
            NadBleStartAdvReqType req;
            req.adv_frame = reinterpret_cast<const ReqBleStartAdvertising*>(m->GetData())->adv_frame;
            msg.size = sizeof(NadBleStartAdvReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);;
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_STOPADVERTISING: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_WAKEUP_RESPONSE: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_BLE_DATAACCESSAUTHCONFIRMED: {
            tpVCS_CustomMessage_t msg;
            NadBleDataAccAuthConReqType req;
            req.connectionID = (reinterpret_cast<const ReqBleDataAccAuthCon*>(m->GetData())->connection_id & 0x3FFF);
            msg.size = sizeof(NadBleDataAccAuthConReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_DTCENTRIES: {
            tpVCS_CustomMessage_t msg;
            NadDtcHandlingReqType req;
            req.requestType = reinterpret_cast<const ReqDtcEntries*>(m->GetData())->request_id;
            req.eventIdentifier = reinterpret_cast<const ReqDtcEntries*>(m->GetData())->event_id;
            msg.size = sizeof(NadDtcHandlingReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_DTCCTLSETTINGS: {
            tpVCS_CustomMessage_t msg;
            NadDtcHandlingReqType req;
            req.requestType = reinterpret_cast<const ReqDtcCtlSettings*>(m->GetData())->settings;
            req.eventIdentifier = 0; //Since it cannot be 5 for ctl settings its not used.
            msg.size = sizeof(NadDtcHandlingReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_IPWAKEUP: {
            tpVCS_CustomMessage_t msg;
            NadIpWakeupReqType req;
            req.prio = reinterpret_cast<const ReqIpWakeup*>(m->GetData())->prio;
            req.resourceGroup = reinterpret_cast<const ReqIpWakeup*>(m->GetData())->res_group;
            msg.size = sizeof(NadIpWakeupReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_SETOHCLED: {
            tpVCS_CustomMessage_t msg;
            const ReqSetOhcLed *tmpmsg = reinterpret_cast<const ReqSetOhcLed*>(m->GetData());
            if ((tmpmsg->sos_red_duty > 100) || (tmpmsg->sos_white_duty > 100) || (tmpmsg->voc_duty > 100)) {
                return RET_ERR_INVALID_ARG;
            }
            NadOhcLedType req;
            req.led1_1 = tmpmsg->sos_red_status_blink | tmpmsg->sos_red_status_repeat;
            req.led1_2 = tmpmsg->sos_red_duty;
            req.led2_1 = tmpmsg->sos_white_status_blink | tmpmsg->sos_white_status_repeat;
            req.led2_2 = tmpmsg->sos_white_duty;
            req.led3_1 = tmpmsg->voc_status_blink | tmpmsg->voc_status_repeat;
            req.led3_2 = tmpmsg->voc_duty;
            req.brightness = tmpmsg->brightness;
            msg.size = sizeof(NadOhcLedType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_VINNUMBER: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETMSGCATVERSION: {
            tpVCS_CustomMessage_t msg;
            NadMsgCatalogVerType req;
            req.vucMsgCatalogByte1 = (reinterpret_cast<const ReqGetMsgCatVer*>(m->GetData())->version[0]);
            req.vucMsgCatalogByte2 = (reinterpret_cast<const ReqGetMsgCatVer*>(m->GetData())->version[1]);
            req.vucMsgCatalogByte3 = (reinterpret_cast<const ReqGetMsgCatVer*>(m->GetData())->version[2]);
            msg.size = sizeof(NadMsgCatalogVerType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETTCAMHWVERSION: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETVUCBLESWPARTNUMBER: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETVUCSWBUILD: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETBLESWBUILD: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETRFVERSION: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETFBLSWBUILD: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETBELTINFORMATION: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETDIAGERRREPORT: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETVEHICLESTATE: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETDIDGLOBALSNAPSHOTDATA: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_DTCSDETECTED: {
            tpVCS_CustomMessage_t msg;
            const ReqDTCsDetected *tmpmsg = reinterpret_cast<const ReqDTCsDetected*>(m->GetData());
            NadStatusSOCDidReqType req;
            msg.size = sizeof(NadStatusSOCDidReqType);
            for (uint8_t i = 0; i < LENGTH(tmpmsg->status)/8; i++)
            {
                req.detectionStatusDTC[i] = tmpmsg->status[(i*8)] | tmpmsg->status[(i*8)+1] << 1 | tmpmsg->status[(i*8)+2] << 2 |
                                            tmpmsg->status[(i*8)+3] << 3 | tmpmsg->status[(i*8)+4] << 4 | tmpmsg->status[(i*8)+5] << 5 | 
                                            tmpmsg->status[(i*8)+6] << 6 | tmpmsg->status[(i*8)+7] << 7;
            }
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_PSIMDATA: {
            tpVCS_CustomMessage_t msg;
            NadPSimDataCommunicationReqType req;
            msg.size = sizeof(NadStatusSOCDidReqType);
            memcpy(req.payload, reinterpret_cast<const ReqPSIMData*>(m->GetData())->data, sizeof(req.payload));
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_GETOHCBTNSTATE: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_LASTBUBSTATE: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_ELECTENGLVL: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_THERMALMITIGATION: {
            tpVCS_CustomMessage_t msg;
            NadThermalMitigationReqType req;
            const ReqThermalMitigataion *tmpmsg = reinterpret_cast<const ReqThermalMitigataion*>(m->GetData());
            req.temperature = (uint8_t)(tmpmsg->nad_temperature)+128;
            msg.size = sizeof(NadThermalMitigationReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_CARCFGPARAMETERFAULT: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_NETWORK_MANAGEMENT_HISTORY: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_PROGRAM_PRECOND_CHK: {
            tpVCS_CustomMessage_t msg;
            Empty req;
            msg.size = sizeof(Empty);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
#if 1 // nieyj
        case REQ_DOORLOCKUNLOCK: {
            tpVCS_CustomMessage_t msg;
            NadDoorUnlockRequestType req;
            req.centralLockReq = reinterpret_cast<const ReqDoorLockUnlock*>(m->GetData())->centralLockReq;
            msg.size = sizeof(NadDoorUnlockRequestType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_WINCTRL: {
            tpVCS_CustomMessage_t msg;
            NadWindowOpenCloseReqType req;
            req.windowOpenCloseReq = reinterpret_cast<const ReqWinCtrl*>(m->GetData())->mode;
            req.value = reinterpret_cast<const ReqWinCtrl*>(m->GetData())->openValue;
            msg.size = sizeof(NadWindowOpenCloseReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_ROOFCTRL: {
            tpVCS_CustomMessage_t msg;
            NadSunRoofOpenCloseReqType req;
            req.sunRoofOpenCloseReq = reinterpret_cast<const ReqRoofCurtCtrl*>(m->GetData())->mode;
            msg.size = sizeof(NadSunRoofOpenCloseReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_WINVENTI: {
            tpVCS_CustomMessage_t msg;
            NadWindowVentilationReqType req;
            req.windowVentilationReq = reinterpret_cast<const ReqWinVentilation*>(m->GetData())->mode;
            req.winJawOpening = reinterpret_cast<const ReqWinVentilation*>(m->GetData())->value;
            msg.size = sizeof(NadWindowVentilationReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_PM25ENQUIRE: {
            tpVCS_CustomMessage_t msg;
            NadPMEnquiresReqType req;
            req.pmEnquiresReq = 1;    // 1-Req
            msg.size = sizeof(NadPMEnquiresReqType);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
         case REQ_VFCACTIVATE: {
#ifndef VFC_ENABLE
            tpVCS_CustomMessage_t msg;
            ReqVFCActivate req;
            req.id = reinterpret_cast<const ReqVFCActivate*>(m->GetData())->id;
            req.type = reinterpret_cast<const ReqVFCActivate*>(m->GetData())->type;
            msg.size = sizeof(ReqVFCActivate);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
#else
            LOG(LOG_WARN, "VuCComm: %s: REQ_VFCACTIVATE has not been implement", __FUNCTION__);
            return RET_ERR_INVALID_ARG;
#endif
        }
#endif
        //uia93888 res
        case REQ_RMTENGINE:{
            LOG(LOG_WARN, "VuCComm: %s :REQ_RMTENGINE, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            tpVCS_CustomMessage_t msg;
            Req_RMTEngine req;
            const Req_RMTEngine* _pReq = reinterpret_cast<const Req_RMTEngine*>(m->GetData());
            req.ersCmd = _pReq->ersCmd;
            req.ersRunTime = _pReq->ersRunTime;
            req.imobVehRemReqCmd = _pReq->imobVehRemReqCmd;
            req.imobVehDataRemReq0 = _pReq->imobVehDataRemReq0;
            req.imobVehDataRemReq1 = _pReq->imobVehDataRemReq1;
            req.imobVehDataRemReq2 = _pReq->imobVehDataRemReq2;
            req.imobVehDataRemReq3 = _pReq->imobVehDataRemReq3;
            req.imobVehDataRemReq4 = _pReq->imobVehDataRemReq4;
            req.imobVehRemTmrOrSpdLim = _pReq->imobVehRemTmrOrSpdLim;
            msg.size = sizeof(Req_RMTEngine);
            msg.buffer = reinterpret_cast<gchar*>(&req);
#if 0
            return SendCustomMessage(m, &msg);
#elif 1     // when vuc complete the security authentication, will not use these code
            LOG(LOG_WARN, "VuCComm: %s, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            SendCustomMessage(m, &msg);
            //PerformSimulationOperations(m);
            return RET_OK;
#endif

        }
        case REQ_RMTENGINESECURITYRESPONSE:{
            tpVCS_CustomMessage_t msg;
            Req_RMTEngineSecurityResponse req;
            const Req_RMTEngineSecurityResponse* _req = reinterpret_cast<const Req_RMTEngineSecurityResponse*>(m->GetData());
            req.imobVehRemReqCmd = _req->imobVehRemReqCmd;
            req.imobVehDataRemReq0 = _req->imobVehDataRemReq0;
            req.imobVehDataRemReq1 = _req->imobVehDataRemReq1;
            req.imobVehDataRemReq2 = _req->imobVehDataRemReq2;
            req.imobVehDataRemReq3 = _req->imobVehDataRemReq3;
            req.imobVehDataRemReq4 = _req->imobVehDataRemReq4;
            req.imobVehRemTmrOrSpdLim = _req->imobVehRemTmrOrSpdLim;
            msg.size = sizeof(Req_RMTEngineSecurityResponse);
            msg.buffer = reinterpret_cast<gchar*>(&req);
#if 0
            return SendCustomMessage(m, &msg);
#elif 1
            LOG(LOG_WARN, "VuCComm: %s, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            SendCustomMessage(m, &msg);
            //PerformSimulationOperations(m);
            return RET_OK;
#endif
            break;
        }
        case REQ_RTCAWAKEVUC:{
            tpVCS_CustomMessage_t msg;
            Req_RTCAwakeVuc req;
            const Req_RTCAwakeVuc* _pReq = reinterpret_cast<const Req_RTCAwakeVuc*>(m->GetData());
            req.secondAlarm_unit = _pReq->secondAlarm_unit;
            req.secondAlarm_ten = _pReq->secondAlarm_ten;
            req.secondAlarm_enable = _pReq->secondAlarm_enable;

            req.minuteAlarm_unit = _pReq->minuteAlarm_unit;
            req.minuteAlarm_ten = _pReq->minuteAlarm_ten;
            req.minuteAlarm_enable = _pReq->minuteAlarm_enable;

            req.hourAlarm_unit = _pReq->hourAlarm_unit;
            req.hourAlarm_ten = _pReq->hourAlarm_ten;
            req.hourAlarm_reserved = _pReq->hourAlarm_reserved;
            req.hourAlarm_enable = _pReq->hourAlarm_enable;

            req.dayAlarm_unit = _pReq->dayAlarm_unit;
            req.dayAlarm_ten = _pReq->dayAlarm_ten;
            req.dayAlarm_reserved = _pReq->dayAlarm_reserved;
            req.dayAlarm_enable = _pReq->dayAlarm_enable;
            msg.size = sizeof(Req_RTCAwakeVuc);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_DELAYENGINERUNNINGTIME:
        {
            tpVCS_CustomMessage_t msg;
            Req_DelayEngineRunngTime req;
            const Req_DelayEngineRunngTime* _pReq = reinterpret_cast<const Req_DelayEngineRunngTime*>(m->GetData());
            req.telmEngDelayTi = _pReq->telmEngDelayTi;
            req.Reserved = _pReq->Reserved;
            msg.size = sizeof(Req_DelayEngineRunngTime);
            msg.buffer = reinterpret_cast<gchar*>(&req);
#if 0
            return SendCustomMessage(m, &msg);
#elif 1
            LOG(LOG_WARN, "VuCComm: %s, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            SendCustomMessage(m, &msg);
            //PerformSimulationOperations(m);
            return RET_OK;
#endif
        }
        case REQ_OPERATEREMOTECLIMATE:
        {
            tpVCS_CustomMessage_t msg;
            Req_OperateRMTClimate req;
            const Req_OperateRMTClimate* _pReq = reinterpret_cast<const Req_OperateRMTClimate*>(m->GetData());
            req.telmClimaReq           = _pReq->telmClimaReq;
            req.reserved1              = _pReq->reserved1;
            req.telmClimaTSetTempRange = _pReq->telmClimaTSetTempRange;
            req.hmiCmptmtTSpSpcl       = _pReq->hmiCmptmtTSpSpcl;
            req.reserved2              = _pReq->reserved2;
            msg.size = sizeof(Req_OperateRMTClimate);
            msg.buffer = reinterpret_cast<gchar*>(&req);
#if 0
            return SendCustomMessage(m, &msg);
#elif 1
            LOG(LOG_WARN, "VuCComm: %s, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            SendCustomMessage(m, &msg);
            //PerformSimulationOperations(m);
            return RET_OK;
#endif
        }
        case REQ_OPERATERMTSEATHEAT:
        {
            tpVCS_CustomMessage_t msg;
            OperateRMTSeatHeat req;
            const Req_OperateRMTSeatHeat* _req = reinterpret_cast<const Req_OperateRMTSeatHeat*>(m->GetData());
            req.telmSeatDrvHeat = _req->telmSeatDrvHeat;
            req.telmSeatPassHeat = _req->telmSeatPassHeat;
            req.telmSeatSecLeHeat = _req->telmSeatSecLeHeat;
            req.telmSeatSecRiHeat = _req->telmSeatSecRiHeat;
            msg.size = sizeof(OperateRMTSeatHeat);
            msg.buffer = reinterpret_cast<gchar*>(&req);
#if 0
            return SendCustomMessage(m, &msg);
#elif 1
            LOG(LOG_WARN, "VuCComm: %s, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            SendCustomMessage(m, &msg);
            //PerformSimulationOperations(m);
            return RET_OK;
#endif
        }
        case REQ_OPERATERMTSEATVENTILATION:
        {
            tpVCS_CustomMessage_t msg;
            OperateRMTSeatVenti req;
            const Req_OperateRMTSeatVenti* _req = reinterpret_cast<const Req_OperateRMTSeatVenti*>(m->GetData());
            req.telmSeatDrvVenti = _req->telmSeatDrvVenti;
            req.telmSeatPassVenti = _req->telmSeatPassVenti;
            req.telmSeatSecLeVenti = _req->telmSeatSecLeVenti;
            req.telmSeatSecRiVenti = _req->telmSeatSecRiVenti;
            msg.size = sizeof(OperateRMTSeatVenti);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_PARKINGCLIMATEOPER:{
            tpVCS_CustomMessage_t msg;
            Req_ParkingClimateOper_S req;
            const Req_ParkingClimateOper_S* _pReq = reinterpret_cast<const Req_ParkingClimateOper_S*>(m->GetData());
            req.climaRqrd1                         = _pReq->climaRqrd1;

            req.telmClimaTmr                       = _pReq->telmClimaTmr;

            req.climaTmrStsTelmTqrd                = _pReq->climaTmrStsTelmTqrd;
            req.telmClimaTSetTempRange             = _pReq->telmClimaTSetTempRange;
            req.hmiCmptmtSpSpcl                    = _pReq->hmiCmptmtSpSpcl;

            req.seatHeatDurgClimaEnadFromTelm      = _pReq->seatHeatDurgClimaEnadFromTelm;

            req.steerWhlHeatgDurgClimaEnadFromTelm = _pReq->steerWhlHeatgDurgClimaEnadFromTelm;
            msg.size = sizeof(Req_ParkingClimateOper_S);
            msg.buffer = reinterpret_cast<gchar*>(&req);
#if 0
            return SendCustomMessage(m, &msg);
#elif 1
            LOG(LOG_WARN, "VuCComm: %s, Remote start engine VuC-request (%d)!", __FUNCTION__, m->id_);
            SendCustomMessage(m, &msg);
            return RET_OK;
#endif
        }
         case REQ_TEST_SETLOCALCONFIG: {
             tpVCS_CustomMessage_t msg;
             NadLocalConfigParamRespType req;
             req.localCfgParam = reinterpret_cast<const ReqTestSetLocalConfig*>(m->GetData())->localCfgParam;
             req.localCfgParamValue = reinterpret_cast<const ReqTestSetLocalConfig*>(m->GetData())->localCfgParamValue;
             msg.size = sizeof(NadLocalConfigParamRespType);
             msg.buffer = reinterpret_cast<gchar*>(&req);
             return SendCustomMessage(m, &msg);
         }
        case REQ_ROLLOVERVALUE: {
            tpVCS_CustomMessage_t msg;
            ReqRolloverValue req;
            req.configItem = reinterpret_cast<const ReqRolloverValue*>(m->GetData())->configItem;
            msg.size = sizeof(ReqRolloverValue);
            msg.buffer = reinterpret_cast<gchar*>(&req);
            return SendCustomMessage(m, &msg);
        }
        case REQ_SETROLLOVERCFG: {
            tpVCS_CustomMessage_t msg;
            msg.size = sizeof(ReqSetRolloverCfg);
            msg.buffer = reinterpret_cast<gchar*>(m->GetData());
            return SendCustomMessage(m, &msg);
        }
        default:
            LOG(LOG_WARN, "VuCComm: %s: Not a known VuC-request (%d)!", __FUNCTION__, m->id_);
            return RET_ERR_INVALID_ARG;
        }
    }
    default :
        LOG(LOG_WARN, "VuCComm: %s: does not currently handle this message type (%d)!", __FUNCTION__, m->type_);
        return RET_ERR_INVALID_ARG;
    }

    return RET_OK;
}

/**
    @brief Send message to VuC.

    This method is used by the user of VuCComm to send a Message to the VuC.

    @return     A VC return-value
*/
ReturnValue VuCComm::SendMessage(MessageBase *m)
{
    ReturnValue ret;

    LOG(LOG_DEBUG, "VuCComm: %s.", __FUNCTION__);

    if (m == NULL) {
        LOG(LOG_WARN, "VuCComm: %s: Message is NULL!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    switch (m->type_) {
    case MESSAGE_REQUEST: {
        // Test
        if (PerformSimulationOperations(m))
            return RET_OK;

        if (!HasGenericResponse(m)) {
            ret = slist_.AddSession(m, -1, AWAITING_RESPONSE, [this](MessageBase *m){return VuCComm::SendVuCMessage(m);});
            if (ret != RET_OK) {
                LOG(LOG_WARN, "VuCComm: %s: Failed to add transfer-session for %s(%d)!",
                        __FUNCTION__, MessageStr(m), m->id_);
                SendErrorMessage(mq_, m, ret);
                return ret;
            }
        }

        ret = SendVuCMessage(m);
        if (ret != RET_OK) {
            LOG(LOG_WARN, "VuCComm: %s: SendVuCMessage failed (error = %s)!", __FUNCTION__, ReturnValueStr[ret]);
            if (!HasGenericResponse(m))
                slist_.RemoveSession_SessionID(m->session_id_);
            SendErrorMessage(mq_, m, ret);
            return ret;
        }

        break;
    }
    case MESSAGE_RESPONSE: {
        ret = SendVuCMessage(m);
        if (ret != RET_OK) {
            LOG(LOG_WARN, "VuCComm: %s: SendVuCMessage failed (error = %s)!", __FUNCTION__, ReturnValueStr[ret]);
            SendErrorMessage(mq_, m, ret);
            return ret;
        }
        break;
    }
    case MESSAGE_ERROR: {
        LOG(LOG_INFO, "VuCComm: %s: error msg received and dropped!", __FUNCTION__);
        return RET_OK;
    }
    default:
        LOG(LOG_WARN, "VuCComm: %s: does not currently handle this message type (%d)!", __FUNCTION__, m->type_);
        return RET_ERR_INVALID_ARG;
    }

    LOG(LOG_DEBUG, "VuCComm: %s: message sent.", __FUNCTION__);

    return RET_OK;
}

// Test

bool VuCComm::PerformSimulationOperations(MessageBase *m)
{
    if ((m->type_ == MESSAGE_REQUEST) && ResponseLookup((RequestID)m->id_) == RES_REQUESTSENT) {
        // Toggle usage-mode
        if ((RequestID)m->id_ == REQ_TEST_TOGGLEUSAGEMODE) {
            LOG(LOG_DEBUG, "VuCComm: %s: Simulating a change of usage-mode.", __FUNCTION__);
            Parameter<int>* par = (Parameter<int>*)(ds_->FindParameter("VehicleState", "vehicleModeMngtGlobalSafeUseMode"));
            const int usage_modes[] = {CAR_ABANDONED, CAR_INACTIVE, CAR_USGMODCNVINC, CAR_ACTIVE, CAR_DRIVING};
            int idx = 0;
            if (par)
                for (int i = 0; i < LENGTH(usage_modes); i++)
                    if (usage_modes[i] == par->value_) {
                        idx = i;
                        break;
                    }
            int value_cur = usage_modes[idx];
            if (++idx == LENGTH(usage_modes))
                idx = 0;
            int value_new = usage_modes[idx];
            if (ds_->ChangeIntParameter("VehicleState", "vehicleModeMngtGlobalSafeUseMode", value_new, true) != RET_OK)
                LOG(LOG_WARN, "VuCComm: %s: Failed to change usage-mode!", __FUNCTION__);
            else
                LOG(LOG_DEBUG, "VuCComm: %s: Usage-mode changed from %d to %d.",
                        __FUNCTION__, value_cur, value_new);
            return true;
        }

        // Toggle car-mode
        if ((RequestID)m->id_ == REQ_TEST_TOGGLECARMODE) {
            LOG(LOG_DEBUG, "VuCComm: %s: Simulating a change of car-mode.", __FUNCTION__);
            Parameter<int>* par = (Parameter<int>*)(ds_->FindParameter("VehicleState", "vehicleModeMngtGlobalSafe"));
            const int car_modes[] = {CAR_NORMAL, CAR_TRANSPORT, CAR_CARMODFCY, CAR_CRASH, CAR_DYNO};
            int idx = 0;
            if (par)
                for (int i = 0; i < LENGTH(car_modes); i++)
                    if (car_modes[i] == par->value_) {
                        idx = i;
                        break;
                    }
            int value_cur = car_modes[idx];
            if (++idx == LENGTH(car_modes))
                idx = 0;
            int value_new = car_modes[idx];
            if (ds_->ChangeIntParameter("VehicleState", "vehicleModeMngtGlobalSafe", value_new, true) != RET_OK)
                LOG(LOG_WARN, "VuCComm: %s: Failed to change car-mode!", __FUNCTION__);
            else
                LOG(LOG_DEBUG, "VuCComm: %s: Car-mode changed from %d to %d.",
                        __FUNCTION__, value_cur, value_new);

            return true;
        }

        // Toggle crash-state
        if ((RequestID)m->id_ == REQ_TEST_CRASHSTATE) {
            tpVCS_CrashMessage_t data;
            data.resumeFromPowerLoss = 0;
            data.crashData = E_VCS_CRASH_DATA_STATE_LEVEL_A;
            CrashMessageCallback(data);
            return true;
        }
        // trigger OHC button pushed
        if ((RequestID)m->id_ == REQ_TEST_OHCSTATE){
            EventOhcBtnState event;
            event.ecall_btn = OHCBTN_ACTIVE;
            event.icall_btn = OHCBTN_INACTIVE;
            SendEventMessage(EVENT_OHCBTNSTATE, reinterpret_cast<const unsigned char *>(&event), ENDPOINT_TS);
            return true;
        }

    }
    /** test uia93888 REQ_RMTENGINE
    if((RequestID)m->id_ == REQ_RMTENGINE) {
        Res_RMTEngine data;
        data.ersStrtApplSts = ErsStrtApplSts::ErsStrtApplSts_ErsStsRunng;
        data.engSt1WdSts = EngStlWdSts::EngSt1_RunngRunng;
        data.ersStrtRes = ErsStrtRes::ErsStrtRes_ErsStrtSuccess;
        //CrashMessageCallback(data);
        LOG(LOG_WARN, "VuCComm: %s: send REQ_RMTENGINE response!", __FUNCTION__);
        SendEventMessage(EVENT_RMTENGINE, reinterpret_cast<const unsigned char *>(&data), ENDPOINT_TS);
        return true;
    }
    */
#if 1  //add by uia93888
    if((RequestID)m->id_ == REQ_RMTENGINE) {
        LOG(LOG_WARN, "VuCComm: %s: send REQ_RMTENGINE response!", __FUNCTION__);
        Event_RMTEngineSecurityRandom res;
        res.imobRemMgrChk = 2;
        res.imobDataRemMgrChk0 = 0;
        res.imobDataRemMgrChk1 = 0;
        res.imobDataRemMgrChk2 = 0;
        res.imobDataRemMgrChk3 = 0;
        res.imobDataRemMgrChk4 = 0;
        SendEventMessage(EVENT_RMTENGINESECURITYRANDOM, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        return false;
    }
    if((RequestID)m->id_ == REQ_RMTENGINESECURITYRESPONSE){
        LOG(LOG_WARN, "VuCComm: %s: send REQ_RMTENGINESECURITYRESPONSE response!", __FUNCTION__);
        Event_RMTEngineSecurityResult res;
        res.imobVehRemMgrSts = 1;
        SendEventMessage(EVENT_RMTENGINESECURITYRESULT, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        return false;
    }
    if((RequestID)m->id_ == REQ_DELAYENGINERUNNINGTIME){
        LOG(LOG_INFO, "VuCComm: %s: REQ_DELAYENGINERUNNINGTIME!", __FUNCTION__);
        Res_DelayEngRunngTime res;
        res.ersDelayTiCfm = NoYes1_Yes;
        LOG(LOG_WARN, "VuCComm: %s: send REQ_DELAYENGINERUNNINGTIME response!", __FUNCTION__);
        SendEventMessage(EVENT_DELAYENGINERUNNINGTIME, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        //FinishRequest(REQ_DELAYENGINERUNNINGTIME, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        return false;
    }
    if ((RequestID)m->id_ == REQ_OPERATEREMOTECLIMATE){
        LOG(LOG_INFO, "VuCComm: %s: REQ_OPERATEREMOTECLIMATE!", __FUNCTION__);
        Res_OperateRMTClimate res;
        res.remStrtClimaActv = OnOff1_On;
        res.prkgClimaWarn = ClimaWarn_NoWarn;
        LOG(LOG_WARN, "VuCComm: %s: send REQ_OPERATEREMOTECLIMATE response!", __FUNCTION__);
        SendEventMessage(EVENT_OPERATEREMOTECLIMATE, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        //FinishRequest(REQ_OPERATEREMOTECLIMATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        return false;
    }
    if(REQ_PARKINGCLIMATEOPER == (RequestID)m->id_){
        LOG(LOG_INFO, "VuCComm: %s: REQ_PARKINGCLIMATEOPER!", __FUNCTION__);
        Res_ParkingClimateOper_S res;
        res.prkgClimaWarn = 0;
        res.climaActv = 1;
        LOG(LOG_WARN, "VuCComm: %s: send REQ_PARKINGCLIMATEOPER response!", __FUNCTION__);
        SendEventMessage(EVENT_PARKINGCLIMATEOPER, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        //FinishRequest(REQ_OPERATEREMOTECLIMATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        return false;
    }
    if(REQ_OPERATERMTSEATHEAT == (RequestID)m->id_){
        LOG(LOG_INFO, "VuCComm: %s: REQ_OPERATERMTSEATHEAT!", __FUNCTION__);
        Res_OperateRMTSeatHeat_S res;
        res.SeatHeatgAvlStsRowFrstLe = 1;
        res.SeatHeatgLvlStsRowFrstLe = 1;

        res.SeatHeatgAvlStsRowFrstRi = 1;
        res.SeatHeatgLvlStsRowFrstRi = 1;

        res.SeatHeatgAvlStsRowSecLe = 1;
        res.SeatHeatgLvlStsRowSecLe = 1;

        res.SeatHeatgAvlStsRowSecRi = 1;
        res.SeatHeatgLvlStsRowSecRi = 1;
        LOG(LOG_WARN, "VuCComm: %s: send REQ_OPERATERMTSEATHEAT response!", __FUNCTION__);
        SendEventMessage(EVENT_OPERATERMTSEATHEAT, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        //FinishRequest(REQ_OPERATEREMOTECLIMATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        return false;
    }
    if(REQ_OPERATERMTSEATVENTILATION == (RequestID)m->id_){
        LOG(LOG_INFO, "VuCComm: %s: REQ_OPERATERMTSEATVENTILATION!", __FUNCTION__);
        Res_OperateRMTSeatVenti_S res;
        res.SeatVentAvlStsRowFrstLe = 1;
        res.SeatVentnLvlStsRowFrstLe = 1;

        res.SeatVentAvlStsRowFrstRi = 1;
        res.SeatVentnLvlStsRowFrstRi = 1;

        res.SeatVentAvlStsRowSecLe = 1;
        res.SeatVentnLvlStsRowSecLe = 1;

        res.SeatVentAvlStsRowSecRi = 1;
        res.SeatVentnLvlStsRowSecRi = 1;
        LOG(LOG_WARN, "VuCComm: %s: send REQ_OPERATERMTSEATVENTILATION response!", __FUNCTION__);
        SendEventMessage(EVENT_OPERATERMTSEATVENTILATION, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        //FinishRequest(REQ_OPERATEREMOTECLIMATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        return false;
    }
    if(REQ_OPERATERMTSTEERWHLHEAT == (RequestID)m->id_){
        LOG(LOG_INFO, "VuCComm: %s: REQ_OPERATERMTSTEERWHLHEAT!", __FUNCTION__);
        Res_OperateRMTSteerWhlHeat_S res;
        res.SteerWhlHeatgAvlSts = 1;
        LOG(LOG_WARN, "VuCComm: %s: send REQ_OPERATERMTSTEERWHLHEAT response!", __FUNCTION__);
        SendEventMessage(EVENT_OPERATERMTSTEERWHLHEAT, reinterpret_cast<const unsigned char *>(&res), ENDPOINT_TS);
        //FinishRequest(REQ_OPERATEREMOTECLIMATE, RET_OK, reinterpret_cast<const unsigned char *>(&res));
        return false;
    }
#endif
    return false;
}

} // namespace vc
