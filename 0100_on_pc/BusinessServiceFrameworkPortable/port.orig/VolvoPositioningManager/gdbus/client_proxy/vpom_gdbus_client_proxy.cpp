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

#include <algorithm>
#include "vpom_gdbus_client_proxy.hpp"
#include "dlt/dlt.h"

#define MAX_PACKET_SIZE 1400

DLT_DECLARE_CONTEXT(dlt_vpom_gdbus_client_proxy);

VpomGdbusClientProxy::VpomGdbusClientProxy()
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    vpom_proxy_method_ = NULL;
    vpom_proxy_sig_ = NULL;
    signal_connect_gnsspositiondataraw_response_ = 0;
}

VpomGdbusClientProxy& VpomGdbusClientProxy::getInstance()
{
    static VpomGdbusClientProxy instance;

    return instance;
}

//Enable user in callback array
void VpomGdbusClientProxy::CallbacksRegisterCb(vpom::VpomRegisteredItem item, vpom::fgen cb)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s item:%d pid:%d",
                                                __FUNCTION__, item, getpid());

    //Set cb
    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    std::lock_guard<std::mutex> guard(VpomGdbusClientProxy.callbacksMutex_[item]);
    VpomGdbusClientProxy.callbacks_[item] = {.cb = (vpom::fgen)cb,
                                             .requestPending = 0};
}

//Disable user in callback array
void VpomGdbusClientProxy::CallbacksDeregisterCb(vpom::VpomRegisteredItem item)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s item:%d pid:%d",
                                                __FUNCTION__, item, getpid());

    //Clear cb
    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    std::lock_guard<std::mutex> guard(VpomGdbusClientProxy.callbacksMutex_[item]);
    VpomGdbusClientProxy.callbacks_[item] = {.cb = NULL,
                                             .requestPending = 0};
}

//Set requester to 'active' in callback array so that response can be matched later
void VpomGdbusClientProxy::CallbacksRequestAdd(vpom::VpomRegisteredItem item)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s item:%d pid:%d",
                                                 __FUNCTION__, item, getpid());

    //Set request pending
    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    std::lock_guard<std::mutex> guard(VpomGdbusClientProxy.callbacksMutex_[item]);
    vpom::VpomCallbackType &obj = VpomGdbusClientProxy.callbacks_[item];
    obj.requestPending++;
}

//Get callback function from callback array for caller which has request pending
vpom::fgen VpomGdbusClientProxy::CallbacksResponseCbGet(vpom::VpomRegisteredItem item)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s item:%d pid:%d",
                                                 __FUNCTION__, item, getpid());

    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    std::lock_guard<std::mutex> guard(VpomGdbusClientProxy.callbacksMutex_[item]);
    vpom::VpomCallbackType &obj = VpomGdbusClientProxy.callbacks_[item];
    if (obj.requestPending > 0) {
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s item:%d pid:%d found",
                                                     __FUNCTION__, item, getpid());
        obj.requestPending--;
        return obj.cb;
    }

    return NULL;
}

//Get callback function from callback array
vpom::fgen VpomGdbusClientProxy::CallbacksCbGet(vpom::VpomRegisteredItem item)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s item:%d pid:%d",
                                                 __FUNCTION__, item, getpid());

    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    std::lock_guard<std::mutex> guard(VpomGdbusClientProxy.callbacksMutex_[item]);
    vpom::VpomCallbackType &obj = VpomGdbusClientProxy.callbacks_[item];
    return obj.cb;
}

//----- Methods begin -----
bool VpomGdbusClientProxy::GNSSPositionDataRawRequest(uint64_t request_id)
{
    GError *error = NULL;

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    CallbacksRequestAdd(vpom::RegItemGNSSPositionDataRawResponse);

    vpom_manager_call_gnssposition_data_raw_request_sync(vpom_proxy_method_,
                                                         request_id,
                                                         NULL,
                                                         &error);

    if (error != NULL) {
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_ERROR, "%s: Error message: (%s), code:(%d)",
                        __FUNCTION__, error->message, error->code);
        return false;
    }
    return true;
}
//----- Methods end -----

//----- Methods Callback begin -----
//----- Methods Callback end -----

//----- Signals begin -----
void VpomGdbusClientProxy::GNSSPositionDataRawResponseCb(vpomManager *ifc,
                                                         uint32_t datastatus,
                                                         uint32_t year,
                                                         uint32_t month,
                                                         uint32_t day,
                                                         uint32_t hour,
                                                         uint32_t minute,
                                                         uint32_t second,
                                                         uint32_t weekNumber,
                                                         uint32_t timeOfWeek,
                                                         int32_t longitude,
                                                         int32_t latitude,
                                                         int32_t altitude,
                                                         uint32_t speed,
                                                         uint32_t horizontalVelocity,
                                                         int32_t verticalVelocity,
                                                         uint32_t heading,
                                                         uint32_t magnetic_heading,
                                                         bool gpsIsUsed,
                                                         bool glonassIsUsed,
                                                         bool galileoIsUsed,
                                                         bool sbasIsUsed,
                                                         bool qzssL1IsUsed,
                                                         bool qzssL1SAIFIsUsed,
                                                         uint32_t fixType,
                                                         bool dgpsIsUsed,
                                                         bool selfEphemerisDataUsage,
                                                         uint32_t visible_gps,
                                                         uint32_t visible_glonass,
                                                         uint32_t visible_galileo,
                                                         uint32_t visible_sbas,
                                                         uint32_t visible_qzssL1,
                                                         uint32_t visible_qzssL1SAIF,
                                                         uint32_t used_gps,
                                                         uint32_t used_glonass,
                                                         uint32_t used_galileo,
                                                         uint32_t used_sbas,
                                                         uint32_t used_qzssL1,
                                                         uint32_t used_qzssL1SAIF,
                                                         uint32_t hdop,
                                                         uint32_t vdop,
                                                         uint32_t pdop,
                                                         uint32_t tdop,
                                                         uint32_t min_size,
                                                         uint32_t max_size,
                                                         uint32_t actual_size,
                                                         uint32_t data_array,
                                                         uint64_t request_id)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    //Call registered callback
    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    vpom::fgen cb = VpomGdbusClientProxy.CallbacksResponseCbGet(vpom::RegItemGNSSPositionDataRawResponse);
    if (cb) {
        vpom::GNSSData param;

        param.datastatus = (vpom::GnssDataStatus)datastatus;
        param.utcTime.year = year;
        param.utcTime.month = month;
        param.utcTime.day = day;
        param.utcTime.hour = hour;
        param.utcTime.minute = minute;
        param.utcTime.second = second;
        param.gpsTime.weekNumber = weekNumber;
        param.gpsTime.timeOfWeek = timeOfWeek;
        param.position.longlat.longitude = longitude;
        param.position.longlat.latitude = latitude;
        param.position.altitude = altitude;
        param.movement.speed = speed;
        param.movement.horizontalVelocity = horizontalVelocity;
        param.movement.verticalVelocity = verticalVelocity;
        param.heading = heading;
        param.magnetic_heading = magnetic_heading;
        param.gnssStatus.gpsIsUsed = gpsIsUsed;
        param.gnssStatus.glonassIsUsed = glonassIsUsed;
        param.gnssStatus.galileoIsUsed = galileoIsUsed;
        param.gnssStatus.sbasIsUsed = sbasIsUsed;
        param.gnssStatus.qzssL1IsUsed = qzssL1IsUsed;
        param.gnssStatus.qzssL1SAIFIsUsed = qzssL1SAIFIsUsed;
        param.positioningStatus.fixType = (vpom::GnssFixType)fixType;
        param.positioningStatus.dgpsIsUsed = dgpsIsUsed;
        param.positioningStatus.selfEphemerisDataUsage = selfEphemerisDataUsage;
        param.satelliteInfo.nrOfSatellitesVisible.gps = visible_gps;
        param.satelliteInfo.nrOfSatellitesVisible.glonass = visible_glonass;
        param.satelliteInfo.nrOfSatellitesVisible.galileo = visible_galileo;
        param.satelliteInfo.nrOfSatellitesVisible.sbas = visible_sbas;
        param.satelliteInfo.nrOfSatellitesVisible.qzssL1 = visible_qzssL1;
        param.satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF = visible_qzssL1SAIF;
        param.satelliteInfo.nrOfSatellitesUsed.gps = used_gps;
        param.satelliteInfo.nrOfSatellitesUsed.glonass = used_glonass;
        param.satelliteInfo.nrOfSatellitesUsed.galileo = used_galileo;
        param.satelliteInfo.nrOfSatellitesUsed.sbas = used_sbas;
        param.satelliteInfo.nrOfSatellitesUsed.qzssL1 = used_qzssL1;
        param.satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF = used_qzssL1SAIF;
        param.precision.hdop = hdop;
        param.precision.vdop = vdop;
        param.precision.pdop = pdop;
        param.precision.tdop = tdop;
        param.receiverChannels.min_size = min_size;
        param.receiverChannels.max_size = max_size;
        param.receiverChannels.actual_size = actual_size;
        param.receiverChannels.data_array = data_array;

        ((GNSSPositionDataRawResponseCb_t)cb)(&param, request_id);
    }
}

bool VpomGdbusClientProxy::RegisterGNSSPositionDataRawResponseCb(GNSSPositionDataRawResponseCb_t cb)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    VpomGdbusClientProxy.CallbacksRegisterCb(vpom::RegItemGNSSPositionDataRawResponse, (vpom::fgen)cb);

    signal_connect_gnsspositiondataraw_response_ = g_signal_connect(vpom_proxy_sig_, "gnssposition-data-raw-response-sig",
                                                           G_CALLBACK(VpomGdbusClientProxy::GNSSPositionDataRawResponseCb), NULL);
    if (signal_connect_gnsspositiondataraw_response_ <= 0 ) {
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_ERROR, "%s g_signal connect error", __FUNCTION__);
        return false;
    }

    return true;
}

bool VpomGdbusClientProxy::DeregisterGNSSPositionDataRawResponseCb(void)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    if (signal_connect_gnsspositiondataraw_response_ > 0) {
        g_signal_handler_disconnect(vpom_proxy_sig_, signal_connect_gnsspositiondataraw_response_);
        signal_connect_gnsspositiondataraw_response_ = 0;
    }
    VpomGdbusClientProxy& VpomGdbusClientProxy(VpomGdbusClientProxy::getInstance());
    VpomGdbusClientProxy.CallbacksDeregisterCb(vpom::RegItemGNSSPositionDataRawResponse);

    return true;
}

//----- Signals end -----

bool VpomGdbusClientProxy::Init (void)
{
    GError *error = NULL;

    DLT_REGISTER_CONTEXT_LL_TS(dlt_vpom_gdbus_client_proxy, "VPOP", "Volvo Pos Mgr GDBus Client Proxy", DLT_LOG_VERBOSE, DLT_TRACE_STATUS_ON);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s: Create vpom client proxy", __FUNCTION__);

    if (tp_ipc_tx_conn == NULL || tp_ipc_rx_conn == NULL) {
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_ERROR, "%s IPC is not initialized! Please run tpSYS_initIPC first.", __FUNCTION__);
        return false;
    }

    vpom_proxy_method_ = vpom_manager_proxy_new_sync(
                         tp_ipc_tx_conn,
                         G_DBUS_PROXY_FLAGS_NONE,
                         VPOM_DBUS_SERVICE,
                         VPOM_DBUS_OBJECT,
                         NULL,
                         &error);

    if (vpom_proxy_method_ == NULL) {
        // print error and terminate.
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s: Unable to create the vpom client message proxy object, %s",
                        __FUNCTION__, error->message);
        return false;
    }

    if (error != NULL)
        g_error_free(error);

    vpom_proxy_sig_ = vpom_manager_proxy_new_sync(
                      tp_ipc_rx_conn,
                      G_DBUS_PROXY_FLAGS_NONE,
                      VPOM_DBUS_SERVICE,
                      VPOM_DBUS_OBJECT,
                      NULL,
                      &error);

    if (vpom_proxy_sig_ == NULL) {
        // print error and terminate
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s: Unable to create the vpom client signal proxy object, %s",
                        __FUNCTION__, error->message);
        return false;
    }

    if (error != NULL)
        g_error_free(error);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client_proxy, DLT_LOG_DEBUG, "%s: Successfully created vpom client proxy", __FUNCTION__);

//TODO
//    g_dbus_error_register_error_domain ("vpom_error", &quark_volatile, vpom_error_entries,
//                                        G_N_ELEMENTS (vpom_error_entries));

    return true;
}

bool VpomGdbusClientProxy::Deinit (void)
{
    g_object_unref(vpom_proxy_method_);
    g_object_unref(vpom_proxy_sig_);
    DLT_UNREGISTER_CONTEXT(dlt_vpom_gdbus_client_proxy);

    return true;
}
