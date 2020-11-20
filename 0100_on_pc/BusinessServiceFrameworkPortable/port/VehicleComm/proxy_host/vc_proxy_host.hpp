#ifndef TS_COMM_GDBUS_HPP
#define TS_COMM_GDBUS_HPP

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

/** @file vc_proxy_host.hpp
 * This file implements communication with telematics over dbus interface
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        07-Dec-2016
 ***************************************************************************/

#include <list>
#include <functional>
#include <dbus/dbus.h>
#include <dbus/dbus-shared.h>
#include <mutex>

extern "C"
{
#include "VehicleComm-ifc-generated.h"
}
#include "vc_common.hpp"

namespace vc {

#define TSCOMM_DBUS_SERVICE              "com.contiautomotive.tcam.VehicleComm"
#define TSCOMM_DBUS_SERVER_NAME          "/com/contiautomotive/tcam/VehicleComm"
#define TSCOMM_DBUS_OBJECT               "/com/contiautomotive/tcam/VehicleComm/iface"

static const GDBusErrorEntry tscomm_error_entries[] =
{
    {RET_OK,                       "com.contiautomotive.tcam.VehicleComm.Error.None"},
    {RET_ERR_INVALID_ARG,          "com.contiautomotive.tcam.VehicleComm.Error.InvalidArgument"},
    {RET_ERR_INTERNAL,             "com.contiautomotive.tcam.VehicleComm.Error.Internal"},
    {RET_ERR_EXTERNAL,             "com.contiautomotive.tcam.VehicleComm.Error.External"},
    {RET_ERR_OUT_OF_MEMORY,        "com.contiautomotive.tcam.VehicleComm.Error.OutOfMemory"},
    {RET_ERR_TIMEOUT,              "com.contiautomotive.tcam.VehicleComm.Error.Timeout"},
    {RET_ERR_BUSY,                 "com.contiautomotive.tcam.VehicleComm.Error.Busy"},
    {RET_ERR_VALUE_NOT_AVAILABLE,  "com.contiautomotive.tcam.VehicleComm.Error.ValueNotAvailable"},
};

/**
    @brief A request coming from GDBus

    Contains the information needed to complete an incoming GDBus-request.
*/
typedef struct {
    long id;
    VehicleCommIface *ifc;
    GDBusMethodInvocation *invocation;
} GDBus_Request;


/**
    @brief The part of TSComm handling GDBus-interactions.

    This class is a part of the TSComm, and handles everything dealing with the GDBus-interactions 
    with the VehicleCommClients.
*/
class VCProxyHost {
private:
    static GDBusConnection *vehicle_comm_dbus_conn_;
    static VehicleCommIface *its_comm_;
    static std::function<ReturnValue(int, long, int, long, const unsigned char*)> addIncMessage_;
    static long session_id_next_;
    static volatile gsize quark_volatile_;
    static std::list<GDBus_Request> requests_;
    static std::mutex list_mtx_;

    static gboolean GetClientID(VehicleCommIface *ifc, GDBusMethodInvocation *invocation);
    static gboolean BeginRequest(VehicleCommIface *ifc, GDBusMethodInvocation *invocation, guint16 messageid,
                guint32 unique_id, guint16 clientid, GVariant *payload, guint16 payloadsize);
    static bool CompleteRequest(ResponseID response_id, long unique_id, int client_id, long session_id, const unsigned char *response_data,
                int response_data_size);
    static bool CompleteRequestWithError(ReturnValue error_id, long unique_id, int client_id, long session_id);

    static void RegisterDbusCallbacks(VehicleCommIface *ifc);
    static void OnBusAcquired(GDBusConnection  *connection, const gchar *name, gpointer user_data);
    static void OnNameAcquired(GDBusConnection  *connection, const gchar *name, gpointer user_data);
    static void OnNameLost(GDBusConnection *connection, const gchar  *name, gpointer user_data);

    static std::list<GDBus_Request>::iterator FindSession(long session_id);
    InterfaceID EventIDToInterfaceID(EventID event_id);

    DBusConnection *nmea_sys_connection_;
    guint32 owner_id_;

public:
    VCProxyHost(std::function<ReturnValue(int, long, int, long, const unsigned char*)> addIncMessage);
    ReturnValue Init(void);
    ReturnValue SendMessage(MessageID id, MessageType type, long unique_id, int client_id, long session_id, const unsigned char *data, int sz);
};

} // namespace vc

#endif // TSCOMM_GDBUS_HPP
