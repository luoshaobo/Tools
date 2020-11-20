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

#include "vpom_gdbus_client.hpp"
#include "dlt/dlt.h"

DLT_DECLARE_CONTEXT(dlt_vpom_gdbus_client);

VpomGdbusClient client;

VpomGdbusClient::VpomGdbusClient() :
    vpomPositioningProxy_(VpomIClient::GetPositioningService())
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "VpomGdbusClient: %s", __FUNCTION__);
}

VpomGdbusClient& VpomGdbusClient::getInstance()
{
    static VpomGdbusClient instance;

    return instance;
}

void VpomGdbusClient::GNSSPositionDataRawResponseCb(vpom::GNSSData *param, uint64_t request_id)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "%s GNSSPositionDataRaw RESPONSE received", __FUNCTION__);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "year=%d", param->utcTime.year);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "month=%d", param->utcTime.month);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "day=%d", param->utcTime.day);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "hour=%d", param->utcTime.hour);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "minute=%d", param->utcTime.minute);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "second=%d", param->utcTime.second);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "weekNumber=%d", param->gpsTime.weekNumber);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "timeOfWeek=%ld", param->gpsTime.timeOfWeek);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "longitude=%ld", param->position.longlat.longitude);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "latitude=%ld", param->position.longlat.latitude);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "altitude=%d", param->position.altitude);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "speed=%ld", param->movement.speed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "horizontalVelocity=%ld", param->movement.horizontalVelocity);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "verticalVelocity=%ld", param->movement.verticalVelocity);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "heading=%d", param->heading);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "gpsIsUsed=%d", param->gnssStatus.gpsIsUsed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "glonassIsUsed=%d", param->gnssStatus.glonassIsUsed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "galileoIsUsed=%d", param->gnssStatus.galileoIsUsed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "sbasIsUsed=%d", param->gnssStatus.sbasIsUsed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "qzssL1IsUsed=%d", param->gnssStatus.qzssL1IsUsed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "qzssL1SAIFIsUsed=%d", param->gnssStatus.qzssL1SAIFIsUsed);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "fixType=%d", param->positioningStatus.fixType);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "dgpsIsUsed=%d", param->positioningStatus.dgpsIsUsed);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "selfEphemerisDataUsage=%d", param->positioningStatus.selfEphemerisDataUsage);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesVisible.gps=%d", param->satelliteInfo.nrOfSatellitesVisible.gps);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesVisible.glonass=%d", param->satelliteInfo.nrOfSatellitesVisible.glonass);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesVisible.galileo=%d", param->satelliteInfo.nrOfSatellitesVisible.galileo);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesVisible.sbas=%d", param->satelliteInfo.nrOfSatellitesVisible.sbas);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesVisible.qzssL1=%d", param->satelliteInfo.nrOfSatellitesVisible.qzssL1);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesVisible.qzssL1SAIF=%d", param->satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesUsed.gps=%d", param->satelliteInfo.nrOfSatellitesUsed.gps);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesUsed.glonass=%d", param->satelliteInfo.nrOfSatellitesUsed.glonass);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesUsed.galileo=%d", param->satelliteInfo.nrOfSatellitesUsed.galileo);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesUsed.sbas=%d", param->satelliteInfo.nrOfSatellitesUsed.sbas);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesUsed.qzssL1=%d", param->satelliteInfo.nrOfSatellitesUsed.qzssL1);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "nrOfSatellitesUsed.qzssL1SAIF=%d", param->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "hdop=%d", param->precision.hdop);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "vdop=%d", param->precision.vdop);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "pdop=%d", param->precision.pdop);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "tdop=%d", param->precision.tdop);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "receiverChannels.min_size=%d", param->receiverChannels.min_size);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "receiverChannels.max_size=%d", param->receiverChannels.max_size);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "receiverChannels.actual_size=%d", param->receiverChannels.actual_size);
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_INFO, "receiverChannels.data_array=%d", param->receiverChannels.data_array);

    DLT_LOG(dlt_vpom_gdbus_client, DLT_LOG_INFO, DLT_STRING("request_id="), DLT_HEX64(request_id));
}

bool VpomGdbusClient::Init()
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "VpomGdbusClient: %s", __FUNCTION__);

    signal(SIGINT, SignalHandler);

    // Init TPSys. Only allowed once per process.
    if (tpSYS_initIPC(E_SYS_IPC_DBUS, NULL, NULL) != E_SYS_IPC_RET_SUCCESS) {
        DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_ERROR, "VpomGdbusClient: tpSYS Failed to intitialize DBUS.");
        return false;
    }

    if (VpomIClient::Init() != true) {
        DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_ERROR, "VpomGdbusClient: %s: VpomIClient::Init() failed", __FUNCTION__);
        Deinit();
        return false;
    }

    vpomPositioningProxy_.RegisterGNSSPositionDataRawResponseCb(GNSSPositionDataRawResponseCb);

    return true;
}

void VpomGdbusClient::Deinit()
{
    VpomIClient::Deinit();
}

void VpomGdbusClient::SignalHandler(int signum)
{
    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "VpomGdbusClient: %s SIGINT (%d) -> good bye!", __FUNCTION__, signum);

    //KILL THREAD HOW??

    signal(signum, SIG_DFL);
    raise(signum);

    tpSYS_deinitIPC();
    exit(0);
}

void VpomGdbusClient::helpText()
{
    DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "------------------------");
    DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "Usage:");
    DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "01 Send GNSSPositionDataRaw request");
    DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "99 End test client");
    DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "------------------------");
}

int main(int argc, char **argv)
{
    DLT_REGISTER_APP("VPOC", "VPOM GDBus Test Client");
    DLT_REGISTER_CONTEXT_LL_TS(dlt_vpom_gdbus_client, "VPOC", "VPOM GDBus Test Client", DLT_LOG_VERBOSE, DLT_TRACE_STATUS_ON);

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "VpomGdbusTestClient: %s", __FUNCTION__);

    //Register the client towards gdbus proxy
    if (client.Init() == false) {
        client.Deinit();
        return 1;
    }

    DLT_LOG_STRINGF(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "VpomGdbusTestClient: %s helptext****", __FUNCTION__);

    //Print helptext
    client.helpText();

    std::string line;
    static uint64_t request_id = 0x0000000000000001;

    while(true) {
        std::getline(std::cin, line);
        if (line.size() < 2) {
            DLT_LOG_STRING(dlt_vpom_gdbus_client, DLT_LOG_DEBUG, "WRONG USAGE! Please try again!");
            client.helpText();
            continue;
        } else if (line.compare(0, 2, "01") == 0) {
            //Send GNSSPositionDataRaw request
            DLT_LOG(dlt_vpom_gdbus_client, DLT_LOG_DEBUG,
                                           DLT_STRING("main() GNSSPositionDataRaw request with id:"),
                                           DLT_HEX64(request_id));
            client.vpomPositioningProxy_.GNSSPositionDataRawRequest(request_id++);
        } else if (line.compare(0, 2, "99") == 0) {
            //End
            break;
        }
    }

    client.Deinit();

    DLT_UNREGISTER_CONTEXT(dlt_vpom_gdbus_client);
    DLT_UNREGISTER_APP();

    return 0;
}
