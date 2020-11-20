/*
* Copyright (C) 2018 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fstream>
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <mutex>
#include "vpom_gdbus.hpp"
#include "dlt/dlt.h"
#include "vpom_IPositioningService.hpp"

extern "C"
{
}

DLT_IMPORT_CONTEXT(dlt_vpom);

// These are needed when the variables are static
GDBusConnection *VPOM_GDBus::vpom_dbus_conn_ = NULL;
vpomManager *VPOM_GDBus::ivpom_ = NULL;
VolvoPositioningManager *VPOM_GDBus::volvopositioningmanager_ = NULL;

//List used to match request and response
typedef std::list<vpom::RequestResponse> RequestResponseList;
RequestResponseList m_RequestResponse;
std::mutex m_RequestResponseMutex;

std::mutex bus_acquired_m_;
std::condition_variable bus_acquired_;
bool bus_acquired_status_;

VPOM_GDBus::VPOM_GDBus(VolvoPositioningManager *volvopositioningmanager_)
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "VPOM_GDBus Constructor");
    volvopositioningmanager_ = volvopositioningmanager_;
    owner_id_ = 0;
}

VPOM_GDBus::VPOM_GDBus()
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "VPOM_GDBus Constructor");
    owner_id_ = 0;
}

VPOM_GDBus::~VPOM_GDBus()
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "VPOM_GDBus Destructor");
}

VPOM_GDBus& VPOM_GDBus::getInstance()
{
    static VPOM_GDBus instance;

    return instance;
}

// Method begin
gboolean VPOM_GDBus::handleGNSSPositionDataRawRequest_(vpomManager *vpomm,
                                                       GDBusMethodInvocation *invocation,
                                                       guint64 request_id)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());

    //Get position data from locationmgr
    GNSSData gnssData;
    vpom_.LmGetPosition(&gnssData);

    //Send response
    VPOM_GDBus& vpomGdbus_(VPOM_GDBus::getInstance());
    vpomGdbus_.GNSSPositionDataRawResponse(&gnssData, request_id);

    vpom_manager_complete_gnssposition_data_raw_request(vpomm, invocation);

    return true;
}
// Method end

//Signals begin
int VPOM_GDBus::GNSSPositionDataRawResponse(GNSSData *gnssData, uint64_t request_id)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    vpom_manager_emit_gnssposition_data_raw_response_sig(ivpom_,
                                                         gnssData->datastatus,
                                                         gnssData->utcTime.year,
                                                         gnssData->utcTime.month,
                                                         gnssData->utcTime.day,
                                                         gnssData->utcTime.hour,
                                                         gnssData->utcTime.minute,
                                                         gnssData->utcTime.second,
                                                         gnssData->gpsTime.weekNumber,
                                                         gnssData->gpsTime.timeOfWeek,
                                                         gnssData->position.longlat.longitude,
                                                         gnssData->position.longlat.latitude,
                                                         gnssData->position.altitude,
                                                         gnssData->movement.speed,
                                                         gnssData->movement.horizontalVelocity,
                                                         gnssData->movement.verticalVelocity,
                                                         gnssData->heading,
                                                         gnssData->magnetic_heading,
                                                         gnssData->gnssStatus.gpsIsUsed,
                                                         gnssData->gnssStatus.glonassIsUsed,
                                                         gnssData->gnssStatus.galileoIsUsed,
                                                         gnssData->gnssStatus.sbasIsUsed,
                                                         gnssData->gnssStatus.qzssL1IsUsed,
                                                         gnssData->gnssStatus.qzssL1SAIFIsUsed,
                                                         gnssData->positioningStatus.fixType,
                                                         gnssData->positioningStatus.dgpsIsUsed,
                                                         gnssData->positioningStatus.selfEphemerisDataUsage,
                                                         gnssData->satelliteInfo.nrOfSatellitesVisible.gps,
                                                         gnssData->satelliteInfo.nrOfSatellitesVisible.glonass,
                                                         gnssData->satelliteInfo.nrOfSatellitesVisible.galileo,
                                                         gnssData->satelliteInfo.nrOfSatellitesVisible.sbas,
                                                         gnssData->satelliteInfo.nrOfSatellitesVisible.qzssL1,
                                                         gnssData->satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF,
                                                         gnssData->satelliteInfo.nrOfSatellitesUsed.gps,
                                                         gnssData->satelliteInfo.nrOfSatellitesUsed.glonass,
                                                         gnssData->satelliteInfo.nrOfSatellitesUsed.galileo,
                                                         gnssData->satelliteInfo.nrOfSatellitesUsed.sbas,
                                                         gnssData->satelliteInfo.nrOfSatellitesUsed.qzssL1,
                                                         gnssData->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF,
                                                         gnssData->precision.hdop,
                                                         gnssData->precision.vdop,
                                                         gnssData->precision.pdop,
                                                         gnssData->precision.tdop,
                                                         gnssData->receiverChannels.min_size,
                                                         gnssData->receiverChannels.max_size,
                                                         gnssData->receiverChannels.actual_size,
                                                         gnssData->receiverChannels.data_array,
                                                         request_id);

    return 0;
}
//Signals end

void VPOM_GDBus::RegisterDbusCallbacks(vpomManager *vpomm)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);
    g_signal_connect(vpomm, "handle-gnssposition-data-raw-request", G_CALLBACK(handleGNSSPositionDataRawRequest_), NULL);
}

// Callback function used for registration of server DBus object and method callbacks
void VPOM_GDBus::OnBusAcquired(GDBusConnection  *connection,
                               const gchar      *name,
                               gpointer          user_data)
{
    vpomObjectSkeleton *object;
    GDBusObjectManagerServer *manager = NULL;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    // Create a new server
    vpom_dbus_conn_ = connection;
    manager = g_dbus_object_manager_server_new(VPOM_DBUS_SERVER_NAME);
    object = vpom_object_skeleton_new(VPOM_DBUS_OBJECT);
    ivpom_ = vpom_manager_skeleton_new();

    vpom_object_skeleton_set_manager(object, ivpom_);
    g_object_unref(ivpom_);

    // Connect vpom manager method callbacks to D-Bus signals
    RegisterDbusCallbacks(ivpom_);

    // Export all objects
    g_dbus_object_manager_server_export(manager, G_DBUS_OBJECT_SKELETON(object));
    g_object_unref(object);

    g_dbus_object_manager_server_set_connection(manager, connection);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Successfully acquired bus: %s", name);

    bus_acquired_status_ = true;
    bus_acquired_.notify_all();
}

// This callback is not used. Registration is done in OnBusAcquired
void VPOM_GDBus::OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);
}

void VPOM_GDBus::OnNameLost(GDBusConnection *connection, const gchar  *name, gpointer user_data)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);
    raise(SIGTERM);
}

bool VPOM_GDBus::WaitForDbusAcquired(void)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s begin", __FUNCTION__);

    std::unique_lock<std::mutex> lk(bus_acquired_m_);
    bool timeout = !bus_acquired_.wait_for(lk,
                                           std::chrono::milliseconds(VPOM_GDBUS_ACQUIRED_TIMEOUT_MS),
                                           []{return bus_acquired_status_ == true;});
    if (timeout) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "%s bus_acquired_status_ timeout", __FUNCTION__);
    }

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s end %d", __FUNCTION__, timeout);

    return timeout;
}

bool VPOM_GDBus::Initialize()
{
    DBusError err;
    dbus_error_init(&err);
    DBusConnection *dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Attempting to acquire message bus: %s", VPOM_DBUS_SERVICE);

    // Request a unique DBus service name on the bus
    owner_id_ = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                               VPOM_DBUS_SERVICE,
                               G_BUS_NAME_OWNER_FLAGS_NONE,
                               OnBusAcquired,
                               OnNameAcquired,
                               OnNameLost,
                               NULL,
                               NULL);

    if (dbus_error_is_set(&err)) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Connection Error (%s).", err.message);
        dbus_error_free(&err);
        return false;
    }

    if (NULL == dbus_connection) {
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "error unable to initialize systemd D-Bus connection.");
        return false;
    }

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "started dbus systemd service.");

    return true;
}

void VPOM_GDBus::run()
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    Initialize();

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s g_main_loop_run enter", __FUNCTION__);

    auto mainLoop = g_main_loop_new (NULL , false);

    g_main_loop_run(mainLoop);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s g_main_loop_run exit", __FUNCTION__);

    g_main_loop_unref(mainLoop);
}
