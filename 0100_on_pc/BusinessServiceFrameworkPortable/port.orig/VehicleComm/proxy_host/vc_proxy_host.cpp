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

/** @file vc_proxy_host.cpp
 * This file implements communication with telematics over dbus interface
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

extern "C"
{
#include "tplog.h"
}

#include "vc_utility.hpp"
#include "vehicle_comm.hpp"

#include "vc_proxy_host.hpp"

namespace vc {

GDBusConnection *VCProxyHost::vehicle_comm_dbus_conn_ = NULL;
VehicleCommIface *VCProxyHost::its_comm_ = NULL;
std::function<ReturnValue(int, long, int, long, const unsigned char*)> VCProxyHost::addIncMessage_;
volatile gsize VCProxyHost::quark_volatile_ = 0;
std::mutex VCProxyHost::list_mtx_;
std::list<GDBus_Request> VCProxyHost::requests_;

gboolean VCProxyHost::BeginRequest(VehicleCommIface *ifc,GDBusMethodInvocation *invocation, guint16 request_id, guint32 unique_id,
        guint16 client_id, GVariant *request_data, guint16 request_data_size)
{
    ReturnValue ret;
    unsigned char byteArray[request_data_size];
    uint16_t actualLength = 0;
    GVariantIter *iter;
    guchar str;

    long session_id = VehicleComm::GetSessionID();

    LOG(LOG_DEBUG, "VCProxyHost: %s: Incoming request %d (request_id = %s, unique_id = %d, client_id = %d, invocation = %d)",
            __FUNCTION__, session_id, VCRequestStr[request_id], unique_id, client_id, invocation);

    g_variant_get(request_data, "a(y)", &iter);
    while (g_variant_iter_loop (iter, "(y)", &str))
        byteArray[actualLength++] = str;
    g_variant_iter_free (iter);

    list_mtx_.lock();

    LOG(LOG_DEBUG, "VCProxyHost: %s: Add session %d.", __FUNCTION__, session_id);
    requests_.push_back({.id = session_id, .ifc = ifc, .invocation = invocation});

    list_mtx_.unlock();

    // Send message to VehicleComm
    ret = addIncMessage_((MessageID)request_id, unique_id, client_id, session_id, byteArray);
    if(ret != RET_OK) {
        LOG(LOG_WARN, "VCProxyHost: %s addIncMessage failed for request id: %d", __FUNCTION__, request_id);

        std::list<GDBus_Request>::iterator it = FindSession(session_id);
        if (it != requests_.end()) {
            LOG(LOG_DEBUG, "VCProxyHost: %s: Erase session %d", __FUNCTION__, session_id);
            requests_.erase(it);
        } else
            LOG(LOG_WARN, "VCProxyHost: %s: Trying to erase non-existent session (id = %d)", session_id);

        g_dbus_method_invocation_return_error(invocation, quark_volatile_, RET_ERR_INVALID_ARG,
                "Could not add message of requested type: %s", VCRequestStr[request_id]);
    }

    return true;
}

bool VCProxyHost::CompleteRequest(ResponseID response_id, long unique_id, int client_id, long session_id,
                                const unsigned char *response_data, int response_data_size)
{
    for (std::list<GDBus_Request>::iterator it = requests_.begin(); it != requests_.end(); ++it) {
        if (it->id == session_id) {
            LOG(LOG_DEBUG, "VCProxyHost: %s: Complete request %d (ifc = %d, invocation = %d).",
                    __FUNCTION__, session_id, (int)it->ifc, (int)it->invocation);

            GVariant *out_response_data = g_variant_new_from_data(G_VARIANT_TYPE ("a(y)"), response_data, response_data_size, TRUE, NULL, NULL);
            vehicle_comm_iface_complete_request(it->ifc, it->invocation, response_id, unique_id, client_id, out_response_data, response_data_size);

            list_mtx_.lock();
            LOG(LOG_DEBUG, "VCProxyHost: %s: Erase session %d.", __FUNCTION__, it->id);
            requests_.erase(it);
            list_mtx_.unlock();
            return true;
        }
    }

    LOG(LOG_WARN, "VCProxyHost: %s: Session %d not found!", __FUNCTION__, session_id);
    return false;
}

bool VCProxyHost::CompleteRequestWithError(ReturnValue error_id, long unique_id, int client_id, long session_id)
{
    UNUSED(unique_id);
    UNUSED(client_id);
    list_mtx_.lock();
    for (std::list<GDBus_Request>::iterator it = requests_.begin(); it != requests_.end(); ++it) {
        if (it->id == session_id) {
            LOG(LOG_INFO, "VCProxyHost: %s: Complete request with error %d session %d quark: %d (invocation = %d).",
                    __FUNCTION__, error_id, session_id, quark_volatile_, (int)it->invocation);
            g_dbus_method_invocation_return_error(it->invocation, quark_volatile_, error_id, "Complete request with error");
            LOG(LOG_DEBUG, "VCProxyHost: %s: Erase session %d.", __FUNCTION__, it->id);
            requests_.erase(it);
            list_mtx_.unlock();
            return true;
        }
    }

    list_mtx_.unlock();
    return false;
}

static int n_clients = 0;

gboolean VCProxyHost::GetClientID(VehicleCommIface *ifc, GDBusMethodInvocation *invocation)
{
    LOG(LOG_DEBUG, "VCProxyHost: %s: New client = %d.", __FUNCTION__, n_clients);

    vehicle_comm_iface_complete_get_client_id(ifc, invocation, n_clients);

    n_clients++;

    return TRUE;
}

void VCProxyHost::RegisterDbusCallbacks(VehicleCommIface *ifc)
{
    LOG(LOG_DEBUG, "VCProxyHost: %s.", __FUNCTION__);
    g_signal_connect(ifc, "handle-request", G_CALLBACK(BeginRequest), NULL);
    g_signal_connect(ifc, "handle-get-client-id", G_CALLBACK(GetClientID), NULL);
}

void VCProxyHost::OnBusAcquired(GDBusConnection  *connection,
                                    const gchar    *name,
                                    gpointer       user_data)
{
    VehicleCommObjectSkeleton *object;
    GDBusObjectManagerServer *manager = NULL;
    UNUSED(user_data);

    LOG(LOG_DEBUG, "VCProxyHost: %s.", __FUNCTION__);

    // create a new server
    vehicle_comm_dbus_conn_ = connection;
    manager = g_dbus_object_manager_server_new(TSCOMM_DBUS_SERVER_NAME);
    object = vehicle_comm_object_skeleton_new(TSCOMM_DBUS_OBJECT);
    its_comm_ = vehicle_comm_iface_skeleton_new();

    vehicle_comm_object_skeleton_set_iface(object, its_comm_);
    g_object_unref(its_comm_);

    // connect location manager method callbacks to D-Bus signals
    RegisterDbusCallbacks(its_comm_);

    // Export all objects
    g_dbus_object_manager_server_export(manager, G_DBUS_OBJECT_SKELETON(object));
    g_object_unref(object);

    g_dbus_object_manager_server_set_connection(manager, connection);

    g_dbus_error_register_error_domain ("VehicleComm", &quark_volatile_,
        tscomm_error_entries, G_N_ELEMENTS (tscomm_error_entries));

    LOG(LOG_DEBUG, "VCProxyHost: %s: Successfully acquired bus: %s", __FUNCTION__, name);

}


void VCProxyHost::OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    LOG(LOG_DEBUG, "VCProxyHost: %s.", __FUNCTION__);
    UNUSED(connection);
    UNUSED(name);
    UNUSED(user_data);
    // Notify already registered applications that VehicleComm has started of rebooted
    vehicle_comm_iface_emit_event_system(its_comm_, SYSEVENT_STARTED);
    LOG(LOG_DEBUG, "VCProxyHost: %s: SYSEVENT_STARTED sent.", __FUNCTION__);
}

void VCProxyHost::OnNameLost(GDBusConnection *connection, const gchar  *name, gpointer user_data)
{
    LOG(LOG_DEBUG, "VCProxyHost: %s.", __FUNCTION__);
    UNUSED(connection);
    UNUSED(name);
    UNUSED(user_data);
    raise(SIGTERM);
}

std::list<GDBus_Request>::iterator VCProxyHost::FindSession(long session_id)
{
    LOG(LOG_DEBUG, "VCProxyHost: %s.", __FUNCTION__);

    list_mtx_.lock();

    for (std::list<GDBus_Request>::iterator it = requests_.begin(); it != requests_.end(); ++it)
        if (it->id == session_id) {
            list_mtx_.unlock();
            return it;
        }

    list_mtx_.unlock();
    return requests_.end();
}

InterfaceID VCProxyHost::EventIDToInterfaceID(EventID event_id)
{
    InterfaceID interface_id = INTERFACE_GENERAL;
    for (int i = 0; i < LENGTH(event_interface_lookup); i++)
        if (event_interface_lookup[i].event_id == event_id)
            return (InterfaceID)event_interface_lookup[i].interface_id;

    LOG(LOG_DEBUG, "SomeIPServer: %s: Unable to find matching interface-ID (event_id = %s[%d])!",
            __FUNCTION__, EventStr[event_id], event_id);
    return interface_id;
}



// Public

VCProxyHost::VCProxyHost(std::function<ReturnValue(int, long, int, long, const unsigned char*)> addIncMessage)
{
    this->addIncMessage_ = addIncMessage;
    this->nmea_sys_connection_ = NULL;
    this->owner_id_ = 0;
}

ReturnValue VCProxyHost::Init (void)
{
    DBusError err;
    dbus_error_init(&err);
    nmea_sys_connection_ = dbus_bus_get(DBUS_BUS_SYSTEM, &err);

    LOG(LOG_DEBUG, "VCProxyHost: %s.", __FUNCTION__);

    LOG(LOG_DEBUG, "VCProxyHost: %s: Attempting to acquire message bus: %s", __FUNCTION__, TSCOMM_DBUS_SERVICE);

    owner_id_ = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                                TSCOMM_DBUS_SERVICE,
                                G_BUS_NAME_OWNER_FLAGS_NONE,
                                OnBusAcquired,
                                OnNameAcquired,
                                OnNameLost,
                                NULL,
                                NULL);

    if (dbus_error_is_set(&err)) {
        LOG(LOG_ERR, "VCProxyHost: %s: Connection Error (%s).", __FUNCTION__, err.message);
        dbus_error_free(&err);
        return RET_ERR_EXTERNAL;
    }

    if (NULL == nmea_sys_connection_) {
        LOG(LOG_ERR, "VCProxyHost: %s: Unable to initialize systemd D-Bus connection!", __FUNCTION__);
        return RET_ERR_EXTERNAL;
    }

    LOG(LOG_DEBUG, "VCProxyHost: %s: Started D-Bus systemd service.", __FUNCTION__);
    return RET_OK;
}

ReturnValue VCProxyHost::SendMessage(MessageID id, MessageType type, long unique_id, int client_id,
        long session_id, const unsigned char *data, int sz)
{
    LOG(LOG_DEBUG, "VCProxyHost: %s (id = %s, type = %s, unique_id = %d, client_id = %d)",
            __FUNCTION__, MessageStr(id, type), MessageTypeStr[type], unique_id, client_id);

    switch (type) {
    case MESSAGE_REQUEST: {
        LOG(LOG_WARN, "VCProxyHost: %s: Trying to send a request to client!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }
    case MESSAGE_RESPONSE: {
        if (!CompleteRequest((ResponseID)id, unique_id, client_id, session_id, data, sz)) {
            LOG(LOG_WARN, "VCProxyHost: %s: Failed to complete request %d!", __FUNCTION__, session_id);
            return RET_ERR_INTERNAL;
        }

        LOG(LOG_DEBUG, "VCProxyHost: %s: Completed request %d (response_id = %s, unique_id = %d, client_id = %d, data = %d, sz = %d).",
                    __FUNCTION__, session_id, ResponseStr[id], unique_id, client_id, (int)data, sz);
        break;
    }
    case MESSAGE_EVENT: {
        LOG(LOG_DEBUG, "VCProxyHost: %s: Send event to client (event_id = %d, unique_id = %d, client_id = %d, session_id = %d, data = %d, sz = %d).",
                __FUNCTION__, id, unique_id, client_id, session_id, (int)data, sz);

        GVariant *event_data = g_variant_new_from_data(G_VARIANT_TYPE ("a(y)"), data, sz, TRUE, NULL, NULL);
        
        switch (EventIDToInterfaceID((EventID)id)) {
            default:
            case INTERFACE_GENERAL:
                vehicle_comm_iface_emit_event_general(its_comm_, id, unique_id, client_id, event_data, (uint16_t)sz);
                break;
            case INTERFACE_BLE:
                vehicle_comm_iface_emit_event_ble(its_comm_, id, unique_id, client_id, event_data, (uint16_t)sz);
                break;
            case INTERFACE_DIAGNOSTICS:
                vehicle_comm_iface_emit_event_diagnostics(its_comm_, id, unique_id, client_id, event_data, (uint16_t)sz);
                break;
        }

        LOG(LOG_DEBUG, "VCProxyHost: %s: Send event to client (event_id = %d, unique_id = %d, client_id = %d, session_id = %d, event_data = %d, sz = %d).",
                __FUNCTION__, id, unique_id, client_id, session_id, (int)event_data, sz);
        break;
    }
    case MESSAGE_ERROR: {
        if (!CompleteRequestWithError((ReturnValue)id, unique_id, client_id, session_id)) {
            LOG(LOG_WARN, "VCProxyHost: %s: Failed to complete request with error %d!", __FUNCTION__, session_id);
            return RET_ERR_INTERNAL;
        }

        LOG(LOG_DEBUG, "VCProxyHost: %s: Completed request with error :%s (session = %d, unique_id = %d, client_id = %d).",
                    __FUNCTION__, ReturnValueStr[id], session_id, unique_id, client_id);
        break;
    }
    default:
        LOG(LOG_WARN, "VCProxyHost: %s: Unknown message-type (type = %d)!", __FUNCTION__, type);
        return RET_ERR_INVALID_ARG;
    }

    return RET_OK;
}

} // namespace vc
