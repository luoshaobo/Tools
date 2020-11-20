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

/** @file ts_comm_lib.cpp
 * This file implements a library to talk to VehicleCommClient (tscomm) over dbus
 *
 * @project     VCC
 * @subsystem   VehicleCommClient
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <cstring>

extern "C"
{
#include "tpsys.h"
#include "tplog.h"
#include "VehicleComm-ifc-generated.h"
}

#include "vc_utility.hpp"
#include "vc_proxy.hpp"

namespace vc {

long MessageBase::unique_id_current = 0;
guint16 VehicleCommClient::client_id_ = -1;

// Responses

/**
    @brief Callback for responses.

    This callback is registered into DBus and handles parsing of the incoming responses.
    It will then call the callback-function registered by the user of the VC-interface.
    @param[out]  ifc            DBus interface pointer
    @param[out]  messageid      The VC response-ID
    @param[out]  clientid       The VC client-ID
    @param[out]  payload        The response data
    @param[out]  payloadsize    The size of the response data
*/
void VehicleCommClient::ResponseCb(VehicleCommIface *ts_comm_proxy_method, GAsyncResult *res, gpointer user_data)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m_req = (MessageBase*)user_data;
    LOG(LOG_DEBUG, "VehicleCommClient: Response for request (request_id = %d, unique_id = %d, client_id = %d)",
            m_req->id_, m_req->unique_id_, m_req->client_id_);

    // Finish the request and store the status
    guint16 out_response_id;
    guint32 out_unique_id;
    guint16 out_client_id;
    guint16 out_response_data_size;
    GVariant *out_response_data = NULL;
    GError *error = NULL;
    ReturnValue status;
    gboolean ret = vehicle_comm_iface_call_request_finish(ts_comm_proxy_method, &out_response_id, &out_unique_id, &out_client_id,
            &out_response_data, &out_response_data_size, res, &error);
    if (!ret) {
        LOG(LOG_DEBUG, "VehicleCommClient: Response received with error '%s' (domain = %d, code = %d)!",
                error->message, error->domain, error->code);
        status = RET_ERR_INTERNAL;
    } else {
        LOG(LOG_DEBUG, "VehicleCommClient: Response received OK (response_id = %d, unique_id = %d, data_size = %d)",
                out_response_id, out_unique_id, out_response_data_size);
        status = RET_OK;
    }
    if (error != NULL)
        g_error_free(error);

    VehicleCommClient& vc = *(VehicleCommClient*)&VehicleCommClient::GetInstance();
    if (vc.response_callback_ == NULL) {
        if (out_response_data != NULL)
            g_variant_unref(out_response_data);
        delete m_req;
        return;
    }

    // Handle the response
    MessageBase *m_res = NULL;
    unsigned char response_data[out_response_data_size];
    if (status == RET_OK) {
        GVariantIter *iter;
        guchar ch;

        uint16_t idx = 0;
        g_variant_get(out_response_data, "a(y)", &iter);
        while (g_variant_iter_loop (iter, "(y)", &ch))
            response_data[idx++] = ch;
        g_variant_iter_free(iter);
    } else
        memset(response_data, 0, out_response_data_size);

    if (out_response_data != NULL)
        g_variant_unref(out_response_data);

    ResponseID response_id = (status == RET_OK) ? (ResponseID)out_response_id : ResponseLookup((RequestID)m_req->id_);
    m_res = InitMessage((MessageID)response_id, MESSAGE_RESPONSE, out_client_id, m_req->session_id_,
                        ENDPOINT_UNKNOWN, response_data);

    delete m_req;

    if(m_res == NULL) {
        LOG(LOG_WARN, "VehicleCommClient: %s: InitMessage failed!", __FUNCTION__);
        return;
    }

    vc.response_callback_(m_res, status);
}

// Events

/**
    @brief Callback for events.

    This callback is registered into DBus and handles parsing of the incoming event.
    It will then call the callback-function registered by the user of the VC-interface.
    @param[out]  ifc            DBus interface pointer
    @param[out]  messageid      The VC event-ID
    @param[out]  clientid       The VC client-ID
    @param[out]  uniqueid       The VC unique-ID
    @param[out]  payload        The event data
    @param[out]  payloadsize    The size of the event data
*/
void VehicleCommClient::EventCb(VehicleCommIface *ifc, guint16 messageid, guint uniqueid, guint16 clientid,
                                        GVariant *payload, guint16 payloadsize)
{
    GVariantIter *iter;
    guchar str;
    unsigned char byteArray[payloadsize];
    uint16_t actualLength = 0;
    MessageBase *message = NULL;

    UNUSED(ifc);
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    VehicleCommClient& vc = *(VehicleCommClient*)&VehicleCommClient::GetInstance();
    if (vc.event_callback_ == NULL)
        return;

    g_variant_get (payload, "a(y)", &iter);
    while (g_variant_iter_loop (iter, "(y)", &str))
        byteArray[actualLength++] = str;
    g_variant_iter_free (iter);

    message = InitMessage((MessageID)messageid, MESSAGE_EVENT, clientid, -1, ENDPOINT_UNKNOWN, byteArray);

    if (message == NULL) {
        LOG(LOG_WARN, "VehicleCommClient: %s Unknown event_id: %d unique_id: %d.", __FUNCTION__, messageid, uniqueid);
        return;
    }

    vc.event_callback_(message);
}

// System-events

/**
    @brief Callback for system events.

    This callback is registered into DBus and handles system events from server side.
    @param[out]  ifc            DBus interface pointer
    @param[out]  event_id       The System event-ID
*/
void VehicleCommClient::SystemEventCb(VehicleCommIface *ifc, guint16 event_id)
{
    UNUSED(ifc);
    GError *error = NULL;
    LOG(LOG_DEBUG, "VehicleCommClient: %s called with : %s.", __FUNCTION__, SystemEventStr[event_id]);
    
    if (event_id == SYSEVENT_STARTED) {
		VehicleCommClient& vc = *(VehicleCommClient*)&IVehicleComm::GetInstance();
        vehicle_comm_iface_call_get_client_id_sync(vc.ts_comm_request_proxy_, &client_id_, NULL, &error);
        if (error != NULL) {
            LOG(LOG_WARN, "VehicleCommClient: %s: Error trying to get a client-ID (%d)! %s", __FUNCTION__, error->code, error->message);
            g_error_free(error);
        } else
            LOG(LOG_INFO, "VehicleCommClient: %s: Got client-ID %d.", __FUNCTION__, client_id_);
    }
}

/**
    @brief Create a request.

    This function is used to create a VC-request from only a request-ID and the
    request-data.
    @param[in]  id          The message-ID enum
    @param[in]  data        Pointer to the data of the specific message
    @return     The created request
*/
MessageBase *VehicleCommClient::CreateRequest(RequestID request_id, const unsigned char *data, long session_id)
{
    return InitMessage((MessageID)request_id, MESSAGE_REQUEST, client_id_, session_id, ENDPOINT_TS, data);
}

/**
    @brief Send a request.

    This function is used to send a VC-request to the server-side (VehicleComm),
    using DBus.
    @param[in]  message     The message
    @return     A VC return value
*/
ReturnValue VehicleCommClient::SendRequest(MessageBase *message)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s", __FUNCTION__);
    
    if (!initialized_) {
        LOG(LOG_DEBUG, "VehicleCommClient: %s: VehicleCommClient not properly initialized!", __FUNCTION__);
        return RET_ERR_INVALID_STATE;
    }

    if ((message == NULL) || (message->type_ != MESSAGE_REQUEST)) {
        LOG(LOG_DEBUG, "VehicleCommClient: %s: Illegal message!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    LOG(LOG_DEBUG, "VehicleCommClient: %s: request_id = %s, unique_id = %d, client_id = %d, size = %d.",
            __FUNCTION__, VCRequestStr[(RequestID)message->id_], message->unique_id_, message->client_id_, message->GetSize());

    GVariant *request_data = g_variant_new_from_data(G_VARIANT_TYPE ("a(y)"), message->GetData(), message->GetSize(), TRUE, NULL, NULL);
    vehicle_comm_iface_call_request(ts_comm_request_proxy_, message->id_, message->unique_id_, message->client_id_, request_data, message->GetSize(),
            NULL, (GAsyncReadyCallback)(VehicleCommClient::ResponseCb), message);

    return RET_OK;
}

// Test

MessageBase *VehicleCommClient::CreateEvent(EventID event_id, const unsigned char *data, long session_id)
{
    return InitMessage((MessageID)event_id, MESSAGE_EVENT, client_id_, session_id, ENDPOINT_TS, data);
}



// Public

/**
    @brief Constructor for the VehicleCommClient-class.
*/
VehicleCommClient::VehicleCommClient()
{
    LOG_INIT("VCCL", "VehicleCommClient");
    
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    initialized_ = false;
    src_ = ENDPOINT_TS;
    
    ts_comm_request_proxy_ = NULL;
    response_status_ = 0;
    response_callback_ = NULL;

    ts_comm_event_proxy_ = NULL;
    event_status_general_ = 0;
    event_status_ble_ = 0;
    event_status_diagnostics_ = 0;
    event_status_system_ = 0;
    event_callback_ = NULL;
}

/**
    @brief Destructor for the VehicleCommClient-class.
*/
VehicleCommClient::~VehicleCommClient()
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_callback_ != NULL) {
        (void)DisableGeneralEvents();
        (void)DisableBLEEvents();
        (void)DisableDiagnosticsEvents();
    }
    //if (response_callback_ != NULL)
    //    (void)DeregisterResponseCallback();

    g_object_unref(ts_comm_request_proxy_);
    g_object_unref(ts_comm_event_proxy_);
    LOG_DEINIT();
}

// IVehicleComm

/**
    @brief Initialization of a VC-client.

    This method takes care of the initialization work needed
    for a VC-client.
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Init(ResponseCallback response_callback, EventCallback event_callback)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    // Make sure TPSys-IPC is initialized
    if (tp_ipc_tx_conn == NULL || tp_ipc_rx_conn == NULL) {
        LOG(LOG_ERR, "VehicleComm: %s: IPC is not initialized! Please run tpSYS_initIPC first.", __FUNCTION__);
        return RET_ERR_EXTERNAL;
    }

    // Requests
    response_callback_ = response_callback;
    LOG(LOG_INFO, "VehicleCommClient: %s: Create VehicleCommClient client proxy", __FUNCTION__);
    GError *error = NULL;
    ts_comm_request_proxy_ = vehicle_comm_iface_proxy_new_sync(
                    tp_ipc_tx_conn,
                    G_DBUS_PROXY_FLAGS_NONE,
                    VC_DBUS_SERVICE,
                    VC_DBUS_OBJECT,
                    NULL, &error);
    if (ts_comm_request_proxy_ == NULL) {
        LOG(LOG_ERR, "VehicleCommClient: %s: Unable to create the VehicleCommClient client message proxy object, %s",
                __FUNCTION__, error->message);
        g_error_free(error);
        return RET_ERR_EXTERNAL;
    }
    LOG(LOG_INFO, "VehicleCommClient: %s: Successfully created request-proxy", __FUNCTION__);
    
    // Events
    event_callback_ = event_callback;
    ts_comm_event_proxy_ = vehicle_comm_iface_proxy_new_sync(
                    tp_ipc_rx_conn,
                    G_DBUS_PROXY_FLAGS_NONE,
                    VC_DBUS_SERVICE,
                    VC_DBUS_OBJECT,
                    NULL, &error);
    if (ts_comm_event_proxy_ == NULL) {
        LOG(LOG_ERR, "VehicleCommClient: %s: Unable to create the event-proxy, %s", __FUNCTION__, error->message);
        g_object_unref(ts_comm_request_proxy_);
        g_error_free(error);
        return RET_ERR_EXTERNAL;
    }
    LOG(LOG_INFO, "VehicleCommClient: %s: Successfully created event-proxy", __FUNCTION__);

    // Events - system
    event_status_system_ = g_signal_connect(ts_comm_event_proxy_, "event-system", G_CALLBACK(VehicleCommClient::SystemEventCb), NULL);
    if (event_status_system_ <= 0) {
        LOG(LOG_ERR, "VehicleCommClient: %s: Failed to register system event signal", __FUNCTION__);
        g_object_unref(ts_comm_request_proxy_);
        g_object_unref(ts_comm_event_proxy_);
        return RET_ERR_EXTERNAL;
    }

    vehicle_comm_iface_call_get_client_id_sync(ts_comm_request_proxy_, &client_id_, NULL, &error);
    if (error != NULL) {
        LOG(LOG_WARN, "VehicleCommClient: %s: Error trying to get a client-ID (%d)! %s", __FUNCTION__, error->code, error->message);
        g_error_free(error);
        g_object_unref(ts_comm_request_proxy_);
        g_object_unref(ts_comm_event_proxy_);
        return RET_ERR_INTERNAL;
    } else
        LOG(LOG_INFO, "VehicleCommClient: %s: Got client-ID %d.", __FUNCTION__, client_id_);

    // Errors
    static volatile gsize quark_volatile = 0;
    g_dbus_error_register_error_domain("VehicleComm", &quark_volatile, vehiclecomm_error_entries,
                                        G_N_ELEMENTS (vehiclecomm_error_entries));
    if (error != NULL) {
        g_error_free(error);
    }
        
    initialized_ = true;

    return RET_OK;
}

/**
    @brief Enable general events.

    This method is called by the client to enable general events,
    by connecting to the corresponding dbus-signal.
    @return     A VC return value
*/
ReturnValue VehicleCommClient::EnableGeneralEvents()
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_callback_ == NULL) {
        LOG(LOG_ERR, "VehicleCommClient: %s: Cannot enable events with no event-callback!", __FUNCTION__);
        return RET_ERR_INVALID_STATE;
    }

    if (event_status_general_ == 0) {
        event_status_general_ = g_signal_connect(ts_comm_event_proxy_, "event-general", G_CALLBACK(VehicleCommClient::EventCb), NULL);
        if (event_status_general_ <= 0) {
            LOG(LOG_ERR, "VehicleCommClient: %s: Couldn't register event signal.", __FUNCTION__);
            event_status_general_ = 0;
            return RET_ERR_EXTERNAL;
        }
    }

    return RET_OK;
}

/**
    @brief Disable general events.

    This method is called by the client to disable general events,
    by unregistering the corresponding signal.
    @return     A VC return value
*/
ReturnValue VehicleCommClient::DisableGeneralEvents(void)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_status_general_ > 0) {
        g_signal_handler_disconnect(ts_comm_event_proxy_, event_status_general_);
        event_status_general_ = 0;
    }

    return RET_OK;
}

/**
    @brief Enable BLE-events.

    This method is called by the client to enable BLE-events,
    by connecting to the corresponding dbus-signal.
    @return     A VC return value
*/
ReturnValue VehicleCommClient::EnableBLEEvents()
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_callback_ == NULL) {
        LOG(LOG_ERR, "VehicleCommClient: %s: Cannot enable events with no event-callback!", __FUNCTION__);
        return RET_ERR_INVALID_STATE;
    }

    if (event_status_ble_ == 0) {
        event_status_ble_ = g_signal_connect(ts_comm_event_proxy_, "event-ble", G_CALLBACK(VehicleCommClient::EventCb), NULL);
        if (event_status_ble_ <= 0) {
            LOG(LOG_ERR, "VehicleCommClient: %s: Couldn't register event signal (status = %d).", __FUNCTION__, event_status_ble_);
            event_status_ble_ = 0;
            return RET_ERR_EXTERNAL;
        }
    }

    return RET_OK;
}

/**
    @brief Disable BLE-events.

    This method is called by the client to disable BLE-events,
    by unregistering the corresponding signal.
*/
ReturnValue VehicleCommClient::DisableBLEEvents(void)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_status_ble_ > 0) {
        g_signal_handler_disconnect(ts_comm_event_proxy_, event_status_ble_);
        event_status_ble_ = 0;
    }

    return RET_OK;
}

/**
    @brief Enable diagnostics-events.

    This method is called by the client to enable diagnostics-events,
    by connecting to the corresponding dbus-signal.
    @return     A VC return value
*/
ReturnValue VehicleCommClient::EnableDiagnosticsEvents()
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_callback_ == NULL) {
        LOG(LOG_ERR, "VehicleCommClient: %s: Cannot enable events with no event-callback!", __FUNCTION__);
        return RET_ERR_INVALID_STATE;
    }

    if (event_status_diagnostics_ == 0) {
        event_status_diagnostics_ = g_signal_connect(ts_comm_event_proxy_, "event-diagnostics", G_CALLBACK(VehicleCommClient::EventCb), NULL);
        if (event_status_diagnostics_ <= 0) {
            LOG(LOG_ERR, "VehicleCommClient: %s: Couldn't register event signal.", __FUNCTION__);
            event_status_diagnostics_ = 0;
            return RET_ERR_EXTERNAL;
        }
    }

    return RET_OK;
}

/**
    @brief Disable diagnostics-events.

    This method is called by the client to disable diagnostics-events,
    by unregistering the corresponding signal.
    @return     A VC return value
*/
ReturnValue VehicleCommClient::DisableDiagnosticsEvents(void)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    if (event_status_diagnostics_ > 0) {
        g_signal_handler_disconnect(ts_comm_event_proxy_, event_status_diagnostics_);
        event_status_diagnostics_ = 0;
    }

    return RET_OK;
}

// IGeneral

/**
    @brief VC-request - Lock/Unlock the door(s)
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_LockDoor(ReqDoorLockUnlock *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_DOORLOCKUNLOCK, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Horn and light
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_HornAndLight(ReqHornNLight *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_HORNNLIGHT, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Antenna switch
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_AntennaSwitch(ReqAntennaSwitch *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_ANTENNASWITCH, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - LAN-link trigger
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_LanLinkTrigger(ReqLanLinkTrigger *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_LANLINKTRIGGER, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Adc (Analog digital converter) value
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_AdcValue(ReqAdcValue *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_ADCVALUE, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Request VIN Number
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_VINNumber(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqVinNumber req;
    MessageBase *m = CreateRequest(REQ_VINNUMBER, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Car usage mode
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_CarUsageMode(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqCarUsageMode req;
    MessageBase *m = CreateRequest(REQ_CARUSAGEMODE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Car mode
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_CarMode(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqCarMode req;
    MessageBase *m = CreateRequest(REQ_CARMODE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Electrical energy level
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_ElectEnergyLevel(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqElectEngLvl req;
    MessageBase *m = CreateRequest(REQ_ELECTENGLVL, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Wifi-related data (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetWifiData(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqGetWifiData req;
    MessageBase *m = CreateRequest(REQ_GETWIFIDATA, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get PSIM-availability (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetPSIMAvailability(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetPSIMAvailability req;
    MessageBase *m = CreateRequest(REQ_GETPSIMAVAILABILITY, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Fuel type (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetFuelType(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetFuelType req;
    MessageBase *m = CreateRequest(REQ_GETFUELTYPE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Propulsion type (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetPropulsionType(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetPropulsionType req;
    MessageBase *m = CreateRequest(REQ_GETPROPULSIONTYPE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Ip wakeup
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_IpWakeup(ReqIpWakeup *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_IPWAKEUP, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Set OHC led
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_SetOhcLed(ReqSetOhcLed *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_SETOHCLED, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - VuC power mode
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetVucPowerMode(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetVucPowerMode req;
    MessageBase *m = CreateRequest(REQ_GETVUCPOWERMODE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - VuC wakeup reason
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetVucWakeupReason(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetVucPowerMode req;
    MessageBase *m = CreateRequest(REQ_GETVUCWAKEUPREASON, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get latest OHC button state
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetOhcBtnState(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetOhcBtnState req;
    MessageBase *m = CreateRequest(REQ_GETOHCBTNSTATE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - Get number of doors
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetNumberOfDoors(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetNumberOfDoors req;
    MessageBase *m = CreateRequest(REQ_GETNUMBEROFDOORS, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - Get steering wheel position
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetSteeringWheelPosistion(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetSteeringWheelPosition req;
    MessageBase *m = CreateRequest(REQ_GETSTEERINGWHEELPOSITION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get fuel tank volume
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetFuelTankVolume(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetFuelTankVolume req;
    MessageBase *m = CreateRequest(REQ_GETFUELTANKVOLUME, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get assistance services
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetAssistanceServices(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetAssistanceServices req;
    MessageBase *m = CreateRequest(REQ_GETASSISTANCESERVICES, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Autonomous Drive
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetAutonomousDrive(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetAutonomousDrive req;
    MessageBase *m = CreateRequest(REQ_GETAUTONOMOUSDRIVE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Combined Instrument
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetCombinedInstrument(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetCombinedInstrument req;
    MessageBase *m = CreateRequest(REQ_GETCOMBINEDINSTRUMENT, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Approved Cellular Standards
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetApprovedCellularStandards(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetApprovedCellularStandards req;
    MessageBase *m = CreateRequest(REQ_GETAPPROVEDCELLULARSTANDARDS, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Digital Video Recorder
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetDigitalVideoRecorder(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetDigitalVideoRecorder req;
    MessageBase *m = CreateRequest(REQ_GETDIGITALVIDEORECORDER, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Driver Alert Control
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetDriverAlertControl(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetDriverAlertControl req;
    MessageBase *m = CreateRequest(REQ_GETDRIVERALERTCONTROL, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get telematic module
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetTelematicModule(long session_id)
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetTelematicModule req;
    MessageBase *m = CreateRequest(REQ_GETTELEMATICMODULE, (const unsigned char *)&req, session_id);
     return SendRequest(m);
}
 
/**
    @brief VC-request - Get connectivity
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetConnectivity(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetConnectivity req;
    MessageBase *m = CreateRequest(REQ_GETCONNECTIVITY, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - Get connected service booking
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetConnectedServiceBooking(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqGetConnectedServiceBooking req;
    MessageBase *m = CreateRequest(REQ_GETCONNECTEDSERVICEBOOKING, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get Belt information
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetBeltInformation(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
     
    ReqCarConfigFactRestore req;
    MessageBase *m = CreateRequest(REQ_GETBELTINFORMATION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get location
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_SendPosnFromSatltCon(ReqSendPosnFromSatltCon req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    
    MessageBase *m = CreateRequest(REQ_SENDPOSNFROMSATLTCON, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get diagnostic error report
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetDiagErrReport(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqGetDiagErrReport req;
    MessageBase *m = CreateRequest(REQ_GETDIAGERRREPORT, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - PSIM data
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_PSIMData(ReqPSIMData *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_PSIMDATA, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Thermal mitigation
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_ThermalMitigation(ReqThermalMitigataion *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_THERMALMITIGATION, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

// Test

ReturnValue VehicleCommClient::Request_GetDSParameter(ReqGetDSParameter&& req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_GETDSPARAMETER, (const unsigned char*)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}
 
ReturnValue VehicleCommClient::Request_VGMTest(ReqVGMTestMsg *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_VGMTESTMSG, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}
 
ReturnValue VehicleCommClient::Request_IHUTest(ReqIHUTestMsg *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_IHUTESTMSG, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
ReturnValue VehicleCommClient::Event_Test_FromVC(EventTestFromVC *ev, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateEvent(EVENT_TESTFROMVC, (const unsigned char*)ev, session_id);
    return SendRequest(m);
}
 
ReturnValue VehicleCommClient::Request_Test_ToggleUsageMode(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_TEST_TOGGLEUSAGEMODE, NULL, session_id);
    return SendRequest(m);
}
 
ReturnValue VehicleCommClient::Request_Test_ToggleCarMode(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_TEST_TOGGLECARMODE, NULL, session_id);
    return SendRequest(m);
}
 
ReturnValue VehicleCommClient::Request_Test_CrashState(long session_id)
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_TEST_CRASHSTATE, NULL, session_id);
    return SendRequest(m);
}

ReturnValue VehicleCommClient::Request_Test_OhcState(long session_id)
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_TEST_OHCSTATE, NULL, session_id);
    return SendRequest(m);
}
 
ReturnValue VehicleCommClient::Request_Test_SetFakeValuesDefault(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_TEST_SETFAKEVALUESDEFAULT, NULL, session_id);
    return SendRequest(m);
}

ReturnValue VehicleCommClient::Request_Test_SetFakeValueInt(ReqTestSetFakeValueInt *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_TEST_SETFAKEVALUEINT, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

ReturnValue VehicleCommClient::Request_Test_SetFakeValueStr(ReqTestSetFakeValueStr *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_TEST_SETFAKEVALUESTR, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

ReturnValue VehicleCommClient::Request_Test_SetLocalConfig(ReqTestSetLocalConfig *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_TEST_SETLOCALCONFIG, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

// IBLE

/**
    @brief VC-request - BLE Disconnect
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleDisconnect(ReqBleDisconnect *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    
    MessageBase *m = CreateRequest(REQ_BLE_DISCONNECT, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Disconnect All
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleDisconnectAll(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqBleDisconnectAll req;
    MessageBase *m = CreateRequest(REQ_BLE_DISCONNECTALL, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Get BDAK
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleGetBdak(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqBleGetBdak req;
    MessageBase *m = CreateRequest(REQ_BLE_GETBDAK, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Generate BDAK
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleGenerateBdak(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqBleGenBdak req;
    MessageBase *m = CreateRequest(REQ_BLE_GENERATEBDAK, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Config datacom service
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleConfigDatacomService(ReqBleCfgDataCommService *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_BLE_CONFIG_DATACOMSERVICE, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - BLE Config ibeacon
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleConfigIbeacon(ReqBleCfgIbeacon *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_BLE_CONFIG_IBEACON, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Config generic access
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleConfigGenericAccess(ReqBleCfgGenAcc *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_BLE_CONFIG_GENERICACCESS, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - BLE Start pairing service
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleStartPairingService(ReqBleStartPairService *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_BLE_STARTPARINGSERVICE, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Stop pairing service
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleStopPairingService(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqBleStopPairService req;
    MessageBase *m = CreateRequest(REQ_BLE_STOPPARINGSERVICE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Wakeup response
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleWakeupResponse(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqBleWakeupResponse req;
    MessageBase *m = CreateRequest(REQ_BLE_WAKEUP_RESPONSE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Data access authentication confirmed
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleDataAccessAuthConfirmed(ReqBleDataAccAuthCon *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_BLE_DATAACCESSAUTHCONFIRMED, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Start advertising
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleStartAdvertising(ReqBleStartAdvertising *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_BLE_STARTADVERTISING, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - BLE Stop advertising
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleStopAdvertising(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    ReqBleStopAdvertising req;
    MessageBase *m = CreateRequest(REQ_BLE_STOPADVERTISING, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
// IDiagnostics

/**
    @brief VC-request - Switch to flash boot loader
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_SwitchToFlashBootloader(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqSwitchToBoot req;
    MessageBase *m = CreateRequest(REQ_SWITCHTOBOOT, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Last battery backup state
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_LastBackupBatteryState(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqLastBubState req;
    MessageBase *m = CreateRequest(REQ_LASTBUBSTATE, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Vuc software build
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetVucSwBuild(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetVucSwBuild req;
    MessageBase *m = CreateRequest(REQ_GETVUCSWBUILD, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Ble software build
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetBleSwBuild(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetBleSwBuild req;
    MessageBase *m = CreateRequest(REQ_GETBLESWBUILD, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Vuc software version
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_VucSwVersion(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqVucSwVersion req;
    MessageBase *m = CreateRequest(REQ_VUCSWVERSION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Ble software version
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_BleSwVersion(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqBleSwVersion req;
    MessageBase *m = CreateRequest(REQ_BLESWVERSION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - DTC entries
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_DtcEntries(ReqDtcEntries *req, long session_id)
 {
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_DTCENTRIES, (const unsigned char *)req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - DTC control settings
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_DtcCtlSettings(ReqDtcCtlSettings *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    MessageBase *m = CreateRequest(REQ_DTCCTLSETTINGS, (const unsigned char *)req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get TCAM hw version
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetTcamHwVersion(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetTcamHwVer req;
    MessageBase *m = CreateRequest(REQ_GETTCAMHWVERSION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}
 
/**
    @brief VC-request - Get Rf version
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetRfVersion(long session_id)
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqGetRfVersion req;
    MessageBase *m = CreateRequest(REQ_GETRFVERSION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get FBL software build
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetFblSwBuild(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetFblSwBuild req;
    MessageBase *m = CreateRequest(REQ_GETFBLSWBUILD, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Restore carconfig to factory defaults 
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_CarConfigFactoryRestore(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
     
    ReqGetBeltInformation req;
    MessageBase *m = CreateRequest(REQ_CARCONFIGFACTRESTORE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Vuc trace config
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_VucTraceConfig(ReqVucTraceConfig *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_VUCTRACE_CONFIG, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Vuc trace control
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_VucTraceControl(ReqVucTraceControl *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_VUCTRACE_CONTROL, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Get DID global snapshot data
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetDIDGlobalSnapshotData(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetDIDGlobalSnapshotData req;
    MessageBase *m = CreateRequest(REQ_GETDIDGLOBALSNAPSHOTDATA, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - DTCs detected
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_DTCsDetected(ReqDTCsDetected *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_DTCSDETECTED, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Rollove Item Value
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_RolloverValue(ReqRolloverValue *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_ROLLOVERVALUE, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - set Rollove Item Value
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_SetRolloverCfg(ReqSetRolloverCfg *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_SETROLLOVERCFG, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Request_CarconfigParameterFaults
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_CarconfigParameterFaults(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqCarCfgParameterFault req;
    MessageBase *m = CreateRequest(REQ_CARCFGPARAMETERFAULT, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Request_NetworkManagementHistory
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_NetworkManagementHistory(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqNetworkManageHistory req;
    MessageBase *m = CreateRequest(REQ_NETWORK_MANAGEMENT_HISTORY, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Request_ProgramPreCheck
    @param[in]  req                    Request data type
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_ProgramPreCheck(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    ReqProgramPrecond_Chk req;
    MessageBase *m = CreateRequest(REQ_PROGRAM_PRECOND_CHK, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

/**
    @brief VC-request - Get theft notification (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetCarCfgTheftNotification(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetTheftNotification req;
    MessageBase *m = CreateRequest(REQ_GETCARCFGTHEFTNOTIFICATION, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get stolen vehicle tracking (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetCarCfgStolenVehicleTracking(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetStolenVehicleTracking req;
    MessageBase *m = CreateRequest(REQ_GETCARCFGSTOLENVEHICLETRACKING, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

/**
    @brief VC-request - Get GNSS (CarConfig)
    @param[in]  session_id             Session-ID that will be given back in the response
    @return     A VC return value
*/
ReturnValue VehicleCommClient::Request_GetGNSSReceiver(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetGNSS req;
    MessageBase *m = CreateRequest(REQ_GETGNSSRECEIVER, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

#if 1 // nieyj
ReturnValue VehicleCommClient::Request_WindowsCtrl(ReqWinCtrl *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_WINCTRL, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    
    return ret;
}
ReturnValue VehicleCommClient::Request_SunRoofAndCurtCtrl(ReqRoofCurtCtrl *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_ROOFCTRL, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    
    return ret;
}
ReturnValue VehicleCommClient::Request_WinVentilation(ReqWinVentilation *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    MessageBase *m = CreateRequest(REQ_WINVENTI, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    
    return ret;
}
ReturnValue VehicleCommClient::Request_PM25Enquire(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqPM25Enquire req;
    req.req = 1;
    MessageBase *m = CreateRequest(REQ_PM25ENQUIRE, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    
    return ret;
}
ReturnValue VehicleCommClient::Request_VFCActivate(ReqVFCActivate *req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.id = %d, state = %d", __FUNCTION__, req->id, req->type);

    MessageBase *m = CreateRequest(REQ_VFCACTIVATE, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    
    return ret;
}
#endif

// uia93888 remote start engine
////////////////////////////////////////////////////////////
// @brief : vuc request - send start request 
// @param[in]  req              request param
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Sep 18, 2018
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_SendRMTEngine(Req_RMTEngine* req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_RMTENGINE, (const unsigned char *)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : vuc request - send send remote engine security authentication response
// @param[in]  req              request param
// @param[in]  session_id       Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Sep 18, 2018
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_RmtEngineSecurityRespose(Req_RMTEngineSecurityResponse* req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_RMTENGINESECURITYRESPONSE, (const unsigned char*)req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}


////////////////////////////////////////////////////////////
// @brief : data storage request - get car state 
// @param[in]  req              request param
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Sep 19 2018
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_GetRMTStatus(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    Req_GetRMTStatus req;
    MessageBase *m = CreateRequest(REQ_GETRMTSTATUS, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : data storage request - get engine states
// @param[in]  req              request param
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Oct 23 2018
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_GetEngineStates(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    Req_GetEngineStates req;
    MessageBase *m = CreateRequest(REQ_GETENGINESTATES, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : data storage request - RTC awake Vuc
// @param[in]  req              request param
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Oct 23 2018
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_RTCAwakeVuc(Req_RTCAwakeVuc* req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_RTCAWAKEVUC, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : delay engine running time
// @param[in]  req              Req_DelayEngineRunngTime
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Jan 25 2019
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_DelayEngineRunningTime(Req_DelayEngineRunngTime* req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_DELAYENGINERUNNINGTIME, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : operate remote climate start/stop
// @param[in]  req              Req_OperateRMTClimate
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Jan 25 2019
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_RemoteClimateOperate(Req_OperateRMTClimate* req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_OPERATEREMOTECLIMATE, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : operate remote seat heat start/stop
// @param[in]  req              Req_OperateRMTClimate
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Jan 25 2019
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_RMTSeatHeat(vc::Req_OperateRMTSeatHeat* req, long session_id){
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_OPERATERMTSEATHEAT, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : operate remote seat ventialtion start/stop
// @param[in]  req              Req_OperateRMTClimate
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Jan 25 2019
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_RMTSeatVentilation(vc::Req_OperateRMTSeatVenti* req, long session_id){
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_OPERATERMTSEATVENTILATION, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}

////////////////////////////////////////////////////////////
// @brief : operate remote steer wheel heat start/stop
// @param[in]  req              Req_OperateRMTClimate
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Jan 25 2019
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_RMTSteerWhlHeat(vc::Req_OperateRMTSteerWhlHeat_S* req, long session_id){
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_OPERATERMTSTEERWHLHEAT, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}


////////////////////////////////////////////////////////////
// @brief : send parking climate start/stop request to Vuc
// @param[in]  req              Req_OperateRMTClimate
// @param[in]  session_id       Session-ID that will be given back in the response
// @return     A VC return value
// @author     uia93888, Jan 25 2019
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_ParkingClimateOperate(vc::Req_ParkingClimateOper_S* req, long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
    MessageBase *m = CreateRequest(REQ_PARKINGCLIMATEOPER, (const unsigned char *)&req, session_id);
    ReturnValue ret = SendRequest(m);
    return ret;
}


////////////////////////////////////////////////////////////
// @brief : VC-request - Get door state
//
// @param[in]  session_id   	Session-ID that will be given back in the response
// @return     A VC return value
// @author     ty, Sep 17, 2018
////////////////////////////////////////////////////////////
ReturnValue VehicleCommClient::Request_GetDoorState(long session_id)
{
    LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);

    ReqGetDoorState req;
    MessageBase *m = CreateRequest(REQ_GETDOORSTATE, (const unsigned char *)&req, session_id);
    return SendRequest(m);
}

// IBLEManager

IGeneral& IVehicleComm::GetGeneralInterface()
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    return *(IGeneral*)&IVehicleComm::GetInstance();
}
 
IBLE& IVehicleComm::GetBLEInterface()
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    return *(IBLE*)&IVehicleComm::GetInstance();
}
 
IDiagnostics& IVehicleComm::GetDiagnosticsInterface()
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    return *(IDiagnostics*)&IVehicleComm::GetInstance();
}
 
IVehicleComm& IVehicleComm::GetInstance()
{
     LOG(LOG_DEBUG, "VehicleCommClient: %s.", __FUNCTION__);
 
    static VehicleCommClient vc_;

    return *(IVehicleComm*)&vc_;
}

} // namespace vc
