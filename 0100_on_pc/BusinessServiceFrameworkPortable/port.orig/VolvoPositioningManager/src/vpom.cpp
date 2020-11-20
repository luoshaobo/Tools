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
*
*/

#include "vpom.hpp"
#include <csignal>
#include <chrono>
#include <thread>
#include <future>
#include "vpom_gdbus.hpp"
#include "local_config_interface.hpp"

extern "C"
{
#include "tpsys.h"
#include "persistence_client_library.h"
#include "../gpsdata/gpsdata.h"
}

static long int sesion_id = 0xeeee;

DLT_DECLARE_CONTEXT(dlt_vpom);

VolvoPositioningManager::VolvoPositioningManager(void) :
    m_IpcbPosSrv(IpcbIClient::GetPositioningService()),
    ipcbGnssProxy_(IpcbIClient::GetGNSSService())    
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "VolvoPositioningManager Constructor");

    active_ = true;
    lmIsReady = false;
    notifyLoop_ = false;
    notifyLoopEnd_ = false;
    conti_.fake_pos = false;
    conti_.print_gnss_pos_data = 0;
    conti_.gnss_pos_data_interval = 0;
    conti_.start_gnss_pos_data_notifications = 0;
    vc_session_id = 0,
    m_ipNetworkIsActive = true;  //need to add more logic per ip link
    carMode = vc::CAR_NORMAL;
    usageMode = vc::CAR_ABANDONED;
    memset(&satellite_info_, 0, sizeof(satellite_info_));
    memset(&m_posData,0, sizeof(m_posData));    
    m_vehModandDateInfo.usgModSts1 = Ipcb_usgModSts1_UsgModAbdnd;
    m_vehModandDateInfo.carModSts1 = Ipcb_carModSts1_CarModNorm;
    m_vehModandDateInfo.carModSubtypWdCarModSubtyp = 0;
    m_vehModandDateInfo.egyLvlElecMai = 0;
    m_vehModandDateInfo.egyLvlElecSubtyp = 0;
    m_vehModandDateInfo.pwrLvlElecMai = 0;
    m_vehModandDateInfo.pwrLvlElecSubtyp = 0;
    m_vehModandDateInfo.fltEgyCnsWdSts= Ipcb_FltEgyCnsWdSts_fltEgyCns1_NoFlt;
    m_vehModandDateInfo.chks = 0;
    m_vehModandDateInfo.cntr = 0;
}

VolvoPositioningManager::~VolvoPositioningManager()
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "VolvoConnectionManager Destructor");
}

VolvoPositioningManager& VolvoPositioningManager::getInstance()
{
    static VolvoPositioningManager instance;

    return instance;
}

void VolvoPositioningManager::LmInit(void)
{
    tpLM_Error_t lmStatus;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    lmStatus = tpLM_init();
    if (lmStatus != E_LM_ERROR_NONE) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpLM_init failed:%d", lmStatus);

        //TODO retry if failed
        return;
    }

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "LMInit success");

    lmIsReady = true;

    //Register for satellite info
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "tpLM_registerSatelliteDataCallback");
    lmStatus = tpLM_registerSatelliteDataCallback(SatelliteCb);
    if (lmStatus != E_LM_ERROR_NONE) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpLM_registerSatelliteDataCallback failed:%d", lmStatus);
    }
}

bool VolvoPositioningManager::LmExit(void)
{
    bool retval = true;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    return retval;
}

void VolvoPositioningManager::SatelliteCb(tpLM_Satellite_t *info)
{
    if (!info)
        return;

    VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());
    if (vpom_.conti_.print_gnss_pos_data & 4) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

        if (info->num_gps_satellites)
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Num gps:             %d", info->num_gps_satellites);
        if (info->num_sbas_satellites)
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Num sbas:            %d", info->num_sbas_satellites);
        if (info->num_glonass_satellites)
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Num glonass:         %d", info->num_glonass_satellites);
        if (info->num_galileo_satellites)
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Num galileo:         %d", info->num_galileo_satellites);
    }

    vpom_.satellite_info_.is_data_valid = info->is_data_valid;
    if (vpom_.satellite_info_.is_data_valid) {
        vpom_.satellite_info_.num_gps_satellites     = info->num_gps_satellites;
        vpom_.satellite_info_.num_sbas_satellites    = info->num_sbas_satellites;
        vpom_.satellite_info_.num_glonass_satellites = info->num_glonass_satellites;
        vpom_.satellite_info_.num_galileo_satellites = info->num_galileo_satellites;
    } else {
        vpom_.satellite_info_.num_gps_satellites     = 0;
        vpom_.satellite_info_.num_sbas_satellites    = 0;
        vpom_.satellite_info_.num_glonass_satellites = 0;
        vpom_.satellite_info_.num_galileo_satellites = 0;
    }
}

bool VolvoPositioningManager::getGpsTime(guint64 timestamp_ms, GPSSystemTime *gpsTime)
{
    time_t timestamp_sec;
    struct tm timestamp_tm;
    char weekstr[16];
    uint8_t weeknr;
    uint8_t weekday;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    size_t res;

    timestamp_sec = time_t(timestamp_ms / 1000);
    gmtime_r(&timestamp_sec, &timestamp_tm);

    res = strftime (weekstr, 16, "%02W %02w %02H %02M %02S", &timestamp_tm);
    if (res >= 14) {
        weeknr = atoi(&weekstr[0]);
        weekday = atoi(&weekstr[3]);
        hour = atoi(&weekstr[6]);
        min = atoi(&weekstr[9]);
        sec = atoi(&weekstr[12]);

        gpsTime->weekNumber = weeknr;
        gpsTime->timeOfWeek = (weekday * 24 * 60 * 60 * 1000) + (hour * 60 * 60 * 1000) + (min * 60 * 1000) + (sec * 1000);
        gpsTime->timeOfWeek += timestamp_ms % 1000;

        return true;
    } else {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "%s Failed to convert", __FUNCTION__);
        return false;
    }
}

enum vpom::SatelliteSystem VolvoPositioningManager::getGpsSystem(uint16_t id)
{
    if ((id >= VPOM_SAT_GPS_MIN) && (id <= VPOM_SAT_GPS_MAX))
        return vpom::SAT_GPS;
    if ((id >= VPOM_SAT_SBAS_MIN) && (id <= VPOM_SAT_SBAS_MAX))
        return vpom::SAT_SBAS;
    if ((id >= VPOM_SAT_GLONASS_MIN) && (id <= VPOM_SAT_GLONASS_MAX))
        return vpom::SAT_GLONASS;
    if ((id >= VPOM_SAT_QZSS_MIN) && (id <= VPOM_SAT_QZSS_MAX))
        return vpom::SAT_QZSS;
    if ((id >= VPOM_SAT_GALILEO_MIN) && (id <= VPOM_SAT_GALILEO_MAX))
        return vpom::SAT_GALILEO;

    return vpom::SAT_UNDEFINED;
}

void VolvoPositioningManager::sendGnssNotifications()
{
    while (notifyLoop_) 
    {
        if (notifyLoopEnd_) 
        {
            notifyLoop_ = false;
            break;
        }

        if(CheckNotifyCondition())
        {
            if(!m_ipNetworkIsActive)
            {
                //request IP Network
            }
            //send notification
            sendGnssNotification();
        }
        else
        {
            //release IP Network
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(conti_.gnss_pos_data_interval));
    }
}

void VolvoPositioningManager::sendGnssNotification()
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager::%s", __FUNCTION__);

    GNSSData posData;
    if (!LmGetPosition(&posData))
    {
        //m_posData.datastatus = vpom::GNSS_DATA_AGED;
        m_posData.datastatus = GNSS_DATA_AGED;
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "LmGetPosition failed. will provide previously stored position.");
    }
    else if (posData.datastatus == GNSS_DATA_INVALID)
    {
        m_posData.datastatus = GNSS_DATA_AGED;
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "LmGetPosition position is invalid. will provide previously stored position.");
    }
    else
    {
        //store current position
        GnssPositionCopy(&m_posData, &posData);
    }

    //Send position via IPCB, HX change
    //vpom::PosnFromSatltPush vgmPos;
    OpPosnFromSatltPush_NotificationCyclic_Data vgmPos;
    VpomPosToVgmPos(&vgmPos, &m_posData);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "%s Sending position request to VGM", __FUNCTION__);
    
    m_IpcbPosSrv.sendPositioningServiceData(OperationId::PosnFromSatltPush, OperationType::NOTIFICATION_CYCLIC, 0, vgmPos);
    
    //ipcbGnssProxy_.PosnFromSatltPushNotificationCycle(vgmPos);

}

void VolvoPositioningManager::PrintLmPos(tpLM_Position_t *pos)
{
    guint32 idx = 0;

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "LmPos:");
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "status:                                    %u",   pos->status);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Aged time:                                 %lld", pos->aged_time);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Latitude:                                  %lf degrees",  pos->latitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Longitude:                                 %lf degrees",  pos->longitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Horizontal speed:                          %lf meters/sec",  pos->hspeed);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Vertical speed:                            %lf meters/sec",  pos->vspeed);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Horizontal Dilution of Precision:          %lf",  pos->hdop);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Vertical Dilution of Precision:            %lf",  pos->vdop);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Position Dilution of Precision:            %lf",  pos->pdop);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Altitude (with respect to ellipsoid):      %lf meters",  pos->ellipsoid_altitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Altitude (with respect to mean sea level): %lf meters",  pos->msl_altitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Estimated Horizontal Position Error:       %lf meters", pos->ehpe);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Vertical Uncertainty:                      %lf meters", pos->vertical_uncertainty);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Magnetic Deviation:                        %lf",  pos->magnetic_deviation);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Heading:                                   %lf degrees",  pos->heading);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Timestamp:                                 %lld", pos->timestamp);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "Number of Satellites:                      %u",   pos->num_satellites_used);

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "Satellites:");
    for(idx = 0; idx < pos->num_satellites_used; idx++) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, " %d", pos->satellites_used[idx]);
    }
}

void VolvoPositioningManager::PrintVpomPos(GNSSData *pos)
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "IcbPos:");
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "year                   %d", pos->utcTime.year);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "month                  %d", pos->utcTime.month);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "day                    %d", pos->utcTime.day);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "hour                   %d", pos->utcTime.hour);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "minute                 %d", pos->utcTime.minute);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "second                 %d", pos->utcTime.second);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "weeknr                 %d", pos->gpsTime.weekNumber);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "timeofweek             %d", pos->gpsTime.timeOfWeek);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "longitude              %d", pos->position.longlat.longitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "latitude               %d", pos->position.longlat.latitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "altitude               %d", pos->position.altitude);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "speed                  %d", pos->movement.speed);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "hvelocity              %d", pos->movement.horizontalVelocity);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "vvelocity              %d", pos->movement.verticalVelocity);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "heading                %d", pos->heading);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "gpsIsUsed              %d", pos->gnssStatus.gpsIsUsed);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "glonassIsUsed          %d", pos->gnssStatus.glonassIsUsed);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "galileoIsUsed          %d", pos->gnssStatus.galileoIsUsed);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "sbasIsUsed             %d", pos->gnssStatus.sbasIsUsed);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "qzssL1IsUsed           %d", pos->gnssStatus.qzssL1IsUsed);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "qzssL1SAIFIsUsed       %d", pos->gnssStatus.qzssL1SAIFIsUsed);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "fixType                %d", pos->positioningStatus.fixType);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "dgpsIsUsed             %d", pos->positioningStatus.dgpsIsUsed);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "selfEphemerisDataUsage %d", pos->positioningStatus.selfEphemerisDataUsage);

//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrofvisible gps        %d", pos->satelliteInfo.nrOfSatellitesVisible.gps);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrofvisible glonass    %d", pos->satelliteInfo.nrOfSatellitesVisible.glonass);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrofvisible galileo    %d", pos->satelliteInfo.nrOfSatellitesVisible.galileo);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrofvisible sbas       %d", pos->satelliteInfo.nrOfSatellitesVisible.sbas);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrofvisible qzssL1     %d", pos->satelliteInfo.nrOfSatellitesVisible.qzssL1);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrofvisible qzssL1SAIF %d", pos->satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrused gps             %d", pos->satelliteInfo.nrOfSatellitesUsed.gps);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrused glonass         %d", pos->satelliteInfo.nrOfSatellitesUsed.glonass);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrused galileo         %d", pos->satelliteInfo.nrOfSatellitesUsed.galileo);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrused sbas            %d", pos->satelliteInfo.nrOfSatellitesUsed.sbas);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrused qzssL1          %d", pos->satelliteInfo.nrOfSatellitesUsed.qzssL1);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "nrused qzssL1SAIF      %d", pos->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "prec hdop              %d", pos->precision.hdop);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "prec vdop              %d", pos->precision.vdop);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "prec pdop              %d", pos->precision.pdop);
//    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "prec tdop              %d", pos->precision.tdop);

    //TODO
//    pos_->receiverChannels;
}

int32_t VolvoPositioningManager::DegreesDToFixedPoint32 (double degrees)
{
    //return int32_t(degrees * ((double(1<<30)) / 90.0));
    return int32_t(degrees * 3600 * 1000);
}

//HX add
bool VolvoPositioningManager::GnssPositionCopy(GNSSData *p_dest, GNSSData *p_src)
{
    if (!p_src || !p_dest) {
        return false;
    }

    p_dest->position.longlat.latitude = p_src->position.longlat.latitude;
    p_dest->position.longlat.longitude = p_src->position.longlat.longitude;
    p_dest->position.altitude = p_src->position.altitude;
    p_dest->movement.speed = p_src->movement.speed;
    p_dest->movement.horizontalVelocity = p_src->movement.horizontalVelocity;
    p_dest->movement.verticalVelocity = p_src->movement.verticalVelocity;
    p_dest->heading = p_src->heading;
    p_dest->utcTime.year = p_src->utcTime.year;
    p_dest->utcTime.month = p_src->utcTime.month;
    p_dest->utcTime.day = p_src->utcTime.day;
    p_dest->utcTime.hour = p_src->utcTime.hour;
    p_dest->utcTime.minute = p_src->utcTime.minute;
    p_dest->utcTime.second = p_src->utcTime.second;
    p_dest->gnssStatus.gpsIsUsed = p_src->gnssStatus.gpsIsUsed;
    p_dest->gnssStatus.glonassIsUsed = p_src->gnssStatus.glonassIsUsed;
    p_dest->gnssStatus.galileoIsUsed = p_src->gnssStatus.galileoIsUsed;
    p_dest->gnssStatus.sbasIsUsed = p_src->gnssStatus.sbasIsUsed;
    p_dest->gnssStatus.qzssL1IsUsed = p_src->gnssStatus.qzssL1IsUsed;
    p_dest->gnssStatus.qzssL1SAIFIsUsed = p_src->gnssStatus.qzssL1SAIFIsUsed;
    p_dest->positioningStatus.fixType = p_src->positioningStatus.fixType;
    p_dest->positioningStatus.dgpsIsUsed = p_src->positioningStatus.dgpsIsUsed;
    p_dest->positioningStatus.selfEphemerisDataUsage = p_src->positioningStatus.selfEphemerisDataUsage;
    p_dest->satelliteInfo.nrOfSatellitesUsed.gps = p_src->satelliteInfo.nrOfSatellitesUsed.gps;
    p_dest->satelliteInfo.nrOfSatellitesUsed.glonass = p_src->satelliteInfo.nrOfSatellitesUsed.glonass;
    p_dest->satelliteInfo.nrOfSatellitesUsed.galileo = p_src->satelliteInfo.nrOfSatellitesUsed.galileo;
    p_dest->satelliteInfo.nrOfSatellitesUsed.sbas = p_src->satelliteInfo.nrOfSatellitesUsed.sbas;
    p_dest->satelliteInfo.nrOfSatellitesUsed.qzssL1 = p_src->satelliteInfo.nrOfSatellitesUsed.qzssL1;
    p_dest->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF = p_src->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF;
    p_dest->precision.pdop = p_src->precision.pdop;
    p_dest->precision.hdop = p_src->precision.hdop;
    p_dest->precision.vdop = p_src->precision.vdop;
    p_dest->precision.tdop = p_src->precision.tdop;

    return true;
}

bool VolvoPositioningManager::LmPosToVpomPos(tpLM_Position_t     *pos_from,
                                             tpLM_Time_t         *utc,
                                             tpLM_SourceStatus_t *status,
                                             GNSSData      *pos_to)
{
    int count;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    if (!pos_from || !utc || !status || !pos_to) {
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_ERROR, "invalid in params");
        return false;
    }

    if (conti_.print_gnss_pos_data & 1)
        PrintLmPos(pos_from);

    //utcTime
    pos_to->utcTime.year   = utc->year;
    pos_to->utcTime.month  = utc->month;
    pos_to->utcTime.day    = utc->day;
    pos_to->utcTime.hour   = utc->hour;
    pos_to->utcTime.minute = utc->minute;
    pos_to->utcTime.second = utc->second;

    //Retrieve week number and time of week
    getGpsTime(pos_from->timestamp, &pos_to->gpsTime);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "GNSS_data_status-->%d",pos_from->status);
    pos_to->datastatus = (GnssDataStatus)pos_from->status;

    //position
    pos_to->position.longlat.longitude = DegreesDToFixedPoint32(pos_from->longitude);
    pos_to->position.longlat.latitude = DegreesDToFixedPoint32(pos_from->latitude);
    pos_to->position.altitude = static_cast<int>(pos_from->msl_altitude); //meters

    //movement
    //pos_to->movement.speed = 1000 * static_cast<unsigned int>(pos_from->hspeed); //Convert from m/s to mm/s
    pos_to->movement.speed = static_cast<unsigned int>(pos_from->hspeed * 3.6); //Convert from m/s to kmph
    pos_to->movement.horizontalVelocity = 1000 * static_cast<unsigned int>(pos_from->hspeed); //Convert from m/s to mm/s
    pos_to->movement.verticalVelocity = 1000 * static_cast<int>(pos_from->vspeed); //Convert from m/s to mm/s

    //heading
    pos_to->heading = 100 * static_cast<unsigned int>(pos_from->heading); //Convert from degrees to 1/100 degrees

    pos_to->magnetic_heading = 100 * static_cast<unsigned int>(pos_from->magnetic_heading); //Convert from degrees to 1/100 degrees
    //positioningStatus
    switch (pos_from->gnss_fix_status) {
    case E_LM_GNSS_FIX_STATUS_NONE:
        pos_to->positioningStatus.fixType = noFix;
        break;
    case E_LM_GNSS_FIX_STATUS_TIME:
        //TODO
        pos_to->positioningStatus.fixType = notAvailable;
        break;
    case E_LM_GNSS_FIX_STATUS_2D:
        pos_to->positioningStatus.fixType = fix2D;
        break;
    case E_LM_GNSS_FIX_STATUS_3D:
        pos_to->positioningStatus.fixType = fix3D;
        break;
    default:
        pos_to->positioningStatus.fixType = notAvailable;
        break;
    }

    //Not supported
    pos_to->positioningStatus.dgpsIsUsed = false;
    pos_to->positioningStatus.selfEphemerisDataUsage = false;

    //Satellites visible
    pos_to->satelliteInfo.nrOfSatellitesVisible.gps = satellite_info_.num_gps_satellites;
    pos_to->satelliteInfo.nrOfSatellitesVisible.sbas = satellite_info_.num_sbas_satellites;
    pos_to->satelliteInfo.nrOfSatellitesVisible.glonass = satellite_info_.num_glonass_satellites;
    pos_to->satelliteInfo.nrOfSatellitesVisible.galileo = satellite_info_.num_galileo_satellites;

    //Not supported
    pos_to->satelliteInfo.nrOfSatellitesVisible.qzssL1 = 31; //Unknown
    pos_to->satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF = 31; //Unknown

    //Satellites used
    memset(&pos_to->satelliteInfo.nrOfSatellitesUsed, 0, sizeof(pos_to->satelliteInfo.nrOfSatellitesUsed));
    for (count = 0; count < pos_from->num_satellites_used; count++) {
        switch (getGpsSystem(pos_from->satellites_used[count])) {
        case vpom::SAT_GPS:
            pos_to->satelliteInfo.nrOfSatellitesUsed.gps++;
            break;
        case vpom::SAT_SBAS:
            pos_to->satelliteInfo.nrOfSatellitesUsed.sbas++;
            break;
        case vpom::SAT_GLONASS:
            pos_to->satelliteInfo.nrOfSatellitesUsed.glonass++;
            break;
        case vpom::SAT_GALILEO:
            pos_to->satelliteInfo.nrOfSatellitesUsed.galileo++;
            break;
        case vpom::SAT_QZSS:
            pos_to->satelliteInfo.nrOfSatellitesUsed.qzssL1++;
            break;
        default:
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "Unknown system for satellite %d", pos_from->satellites_used[count]);
            break;
        }
    }

    //Not supported
    pos_to->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF = 31; //Unknown

    //System is used
    pos_to->gnssStatus.gpsIsUsed     = pos_to->satelliteInfo.nrOfSatellitesUsed.gps != 0;
    pos_to->gnssStatus.sbasIsUsed    = pos_to->satelliteInfo.nrOfSatellitesUsed.sbas != 0;
    pos_to->gnssStatus.glonassIsUsed = pos_to->satelliteInfo.nrOfSatellitesUsed.glonass != 0;
    pos_to->gnssStatus.galileoIsUsed = pos_to->satelliteInfo.nrOfSatellitesUsed.galileo != 0;
    pos_to->gnssStatus.qzssL1IsUsed  = pos_to->satelliteInfo.nrOfSatellitesUsed.qzssL1 != 0;

    //Not supported
    pos_to->gnssStatus.qzssL1SAIFIsUsed = false;

    //precision
    pos_to->precision.hdop = 10 * static_cast<int>(pos_from->hdop); //Convert to 1/10
    pos_to->precision.vdop = 10 * static_cast<int>(pos_from->vdop); //Convert to 1/10
    pos_to->precision.pdop = 10 * static_cast<int>(pos_from->pdop); //Convert to 1/10

    //Not supported
    pos_to->precision.tdop = 255; //Unknown

    //receiverChannels
    //TODO
    pos_to->receiverChannels.min_size = 0;
    pos_to->receiverChannels.max_size = 0;
    pos_to->receiverChannels.actual_size = 0;
    pos_to->receiverChannels.data_array = 0;

    if (pos_from->aged_time != 0) {
        //We couldn't get an updated good position
        //Set fixType to notAvailable
        pos_to->positioningStatus.fixType = notAvailable;
    }

    if (conti_.print_gnss_pos_data & 2)
        PrintVpomPos(pos_to);

    return true;
}

bool VolvoPositioningManager::VpomPosToVehicleCommPos(GNSSData *pos_from,
                                                      vc::ReqSendPosnFromSatltCon *pos_to)
{
    if (!pos_from || !pos_to) {
        return false;
    }

    pos_to->PosnLat = pos_from->position.longlat.latitude; // Latitude
    pos_to->PosnLgt = pos_from->position.longlat.longitude; // Longitude    
    //pos_to->PosnAlti = pos_from->position.altitude; // Altitide from mean sea level
    pos_to->PosnAlti = 0;     //    
    pos_to->PosnSpd = pos_from->movement.speed; // Speed
    pos_to->PosnVHozl = pos_from->movement.horizontalVelocity; // Horizontal velocity
    pos_to->PosnVVert = pos_from->movement.verticalVelocity; // Vertical velocity
    pos_to->PosnDir = (uint16_t) pos_from->heading; // Heading from true north
    pos_to->TiForYr = (uint8_t) (pos_from->utcTime.year - 2000); // UTC year
    pos_to->TiForMth = (uint8_t) pos_from->utcTime.month; // UTC month
    pos_to->TiForDay = (uint8_t) pos_from->utcTime.day; // UTC day
    pos_to->TiForHr = (uint8_t) pos_from->utcTime.hour; // UTC hour
    pos_to->TiForMins = (uint8_t) pos_from->utcTime.minute; // UTC minute
    pos_to->TiForSec = (uint8_t) pos_from->utcTime.second; // UTC second
    pos_to->SatltSysNo1InUse = pos_from->gnssStatus.gpsIsUsed; // GPS used for positioning
    pos_to->SatltSysNo2InUse = pos_from->gnssStatus.glonassIsUsed; // GLONASS used for positioning
    pos_to->SatltSysNo3InUse = pos_from->gnssStatus.galileoIsUsed; // Galileo used for positioning
    pos_to->SatltSysNo4InUse = pos_from->gnssStatus.sbasIsUsed; // SBAS used for positioning Enum
    pos_to->SatltSysNo5InUse = pos_from->gnssStatus.qzssL1IsUsed; // QZSS L1 used for positioning
    pos_to->SatltSysNo6InUse = pos_from->gnssStatus.qzssL1SAIFIsUsed; // QZSS L1-SAIF used for positioning
    // Navigation solution status
    switch (pos_from->positioningStatus.fixType) {
    case notAvailable:
    case noFix:
    default:
        pos_to->SatltPosnStsPrm1 = vc::NO_NAVIGATION_SOLUTION;
        break;
    case fix2D:
        pos_to->SatltPosnStsPrm1 = vc::THREE_SATELITES_SOLUTION;
        break;
    case fix3D:
        pos_to->SatltPosnStsPrm1 = vc::MORE_THAN_THREE_SATELITES_SOLUTION;
        break;
    case startupMode:
        pos_to->SatltPosnStsPrm1 = vc::COLD_START_FIX;
        break;
    };
    pos_to->SatltPosnStsPrm2 = pos_from->positioningStatus.dgpsIsUsed; // DGPS usage in solution
    pos_to->SatltPosnStsPrm3 = pos_from->positioningStatus.selfEphemerisDataUsage; // Self ephemeris data usage in solution
    pos_to->NoOfSatltForSysNo1 = pos_from->satelliteInfo.nrOfSatellitesUsed.gps; // Number of GPS satellites used for positioning
    pos_to->NoOfSatltForSysNo2 = pos_from->satelliteInfo.nrOfSatellitesUsed.glonass; // Number of GLONASS satellites used for positioning
    pos_to->NoOfSatltForSysNo3 = pos_from->satelliteInfo.nrOfSatellitesUsed.galileo; // Number of Galileo satellites used for positioning
    pos_to->NoOfSatltForSysNo4 = pos_from->satelliteInfo.nrOfSatellitesUsed.sbas; // Number of SBAS satellites used for positioning
    pos_to->NoOfSatltForSysNo5 = pos_from->satelliteInfo.nrOfSatellitesUsed.qzssL1; // Number of QZSS L1 satellites used for positioning
    pos_to->NoOfSatltForSysNo6 = pos_from->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF; // Number of QZSS L1-SAIF satellites used for positioning
    pos_to->PrePosnDil = pos_from->precision.pdop; // Position Dilution Of Precision
    pos_to->PreHozlDil = pos_from->precision.hdop; // Horizontal Dilution Of Precision
    pos_to->PreVertDil = pos_from->precision.vdop; // Vertical Dilution Of Precision
    pos_to->PreTiDil = pos_from->precision.tdop; // Time Dilution Of Precision

    return true;
}


bool VolvoPositioningManager::VpomPosToVgmPos(OpPosnFromSatltPush_NotificationCyclic_Data *pos_to, GNSSData *pos_from)
{
    void *ipData;

    setPosnFromSatlt_Data(pos_to, pos_from);
    
    ipData = (BiasedPosnFromSatlt_Data *)(&pos_to->biasedPosnFromSatlt);
    setOpPosnFromSatltPush_Data((BiasedPosnFromSatlt_Data *)ipData, pos_from);

    ipData = (GNSSSolution_Data *)(&pos_to->gNSSSolution);
    setOpPosnFromSatltPush_Data((GNSSSolution_Data *)ipData, pos_from);

    ipData = (PosnBrief_Data *)(&pos_to->posnBrief);
    setOpPosnFromSatltPush_Data((PosnBrief_Data *)ipData, pos_from);
    return true;
}

bool VolvoPositioningManager::setPosnFromSatlt_Data(OpPosnFromSatltPush_NotificationCyclic_Data *pos_to, GNSSData *pos_from)
{
    if (!pos_from || !pos_to) {
        return false;
    }

    pos_to->posnFromSatlt.posnLat = pos_from->position.longlat.latitude; // Latitude
    pos_to->posnFromSatlt.posnLgt = pos_from->position.longlat.longitude; // Longitude

    /*This message can not be sent in China market because of the laws*/
    //pos_to->posnFromSatlt.posnAlti = pos_from->position.altitude; // Altitide from mean sea level
    pos_to->posnFromSatlt.posnAlti = 0;
    
    pos_to->posnFromSatlt.posnSpd = pos_from->movement.speed; // Speed
    pos_to->posnFromSatlt.posnVHozl = pos_from->movement.horizontalVelocity; // Horizontal velocity
    pos_to->posnFromSatlt.posnVVert = pos_from->movement.verticalVelocity; // Vertical velocity
    pos_to->posnFromSatlt.posnDir = (uint16_t) pos_from->heading; // Heading from true north
    
    pos_to->posnFromSatlt.uTCForYr = (uint8_t) (pos_from->utcTime.year - 2000); // UTC year
    pos_to->posnFromSatlt.uTCForMth = (uint8_t) pos_from->utcTime.month; // UTC month
    pos_to->posnFromSatlt.uTCForDay = (uint8_t) pos_from->utcTime.day; // UTC day
    pos_to->posnFromSatlt.uTCForHr = (uint8_t) pos_from->utcTime.hour; // UTC hour
    pos_to->posnFromSatlt.uTCForMins = (uint8_t) pos_from->utcTime.minute; // UTC minute
    pos_to->posnFromSatlt.uTCForSec = (uint8_t) pos_from->utcTime.second; // UTC second
    
    pos_to->posnFromSatlt.tiForYr = (uint8_t) (pos_from->utcTime.year - 2000); // UTC year
    pos_to->posnFromSatlt.tiForMth = (uint8_t) pos_from->utcTime.month; // UTC month
    pos_to->posnFromSatlt.tiForDay = (uint8_t) pos_from->utcTime.day; // UTC day
    pos_to->posnFromSatlt.tiForHr = (uint8_t) pos_from->utcTime.hour; // UTC hour
    pos_to->posnFromSatlt.tiForMins = (uint8_t) pos_from->utcTime.minute; // UTC minute
    pos_to->posnFromSatlt.tiForSec = (uint8_t) pos_from->utcTime.second; // UTC second
    return true;
}

bool VolvoPositioningManager::setOpPosnFromSatltPush_Data(PosnBrief_Data *pos_to, GNSSData *pos_from)
{
    if (!pos_from || !pos_to) {
        return false;
    }
    pos_to->posnLat = pos_from->position.longlat.latitude; // Latitude;
    pos_to->posnLgt = pos_from->position.longlat.longitude; // Longitude
    return true;
}

bool VolvoPositioningManager::setOpPosnFromSatltPush_Data(BiasedPosnFromSatlt_Data *pos_to, GNSSData *pos_from)
{
    if (!pos_from || !pos_to) {
        return false;
    }

    /*
    pos_to->biasedPosnFromSatlt.posnLat = ;
    pos_to->biasedPosnFromSatlt.posnLgt = ;
    pos_to->biasedPosnFromSatlt.tiForDay = 
    pos_to->biasedPosnFromSatlt.tiForHr = 
    pos_to->biasedPosnFromSatlt.tiForMins = 
    pos_to->biasedPosnFromSatlt.tiForMth = 
    pos_to->biasedPosnFromSatlt.tiForSec = 
    pos_to->biasedPosnFromSatlt.tiForYr = 
    */

    pos_to->posnLat = pos_from->position.longlat.latitude; // Latitude;
    pos_to->posnLgt = pos_from->position.longlat.longitude; // Longitude;
    
    pos_to->tiForYr = (uint8_t) (pos_from->utcTime.year - 2000); // UTC year
    pos_to->tiForMth = (uint8_t) pos_from->utcTime.month; // UTC month
    pos_to->tiForDay = (uint8_t) pos_from->utcTime.day; // UTC day
    pos_to->tiForHr = (uint8_t) pos_from->utcTime.hour; // UTC hour
    pos_to->tiForMins = (uint8_t) pos_from->utcTime.minute; // UTC minute
    pos_to->tiForSec = (uint8_t) pos_from->utcTime.second; // UTC second
    return true;
}


bool VolvoPositioningManager::setOpPosnFromSatltPush_Data(GNSSSolution_Data *pos_to, GNSSData *pos_from)
{
    if (!pos_from || !pos_to) {
        return false;
    }
    
    if(true == pos_from->gnssStatus.gpsIsUsed)
    {
        pos_to->satltSysNo1InUse = Ipcb_SatltSysNo1InUse_noYes1_Yes;
    }
    else
    {
        pos_to->satltSysNo1InUse = Ipcb_SatltSysNo1InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.glonassIsUsed)
    {
        pos_to->satltSysNo2InUse = Ipcb_SatltSysNo2InUse_noYes1_Yes;
    }
    else
    {
        pos_to->satltSysNo2InUse = Ipcb_SatltSysNo2InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.galileoIsUsed)
    {
        pos_to->satltSysNo3InUse = Ipcb_SatltSysNo3InUse_noYes1_Yes;
    }
    else
    {
        pos_to->satltSysNo3InUse = Ipcb_SatltSysNo3InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.sbasIsUsed)
    {
        pos_to->satltSysNo4InUse = Ipcb_SatltSysNo4InUse_noYes1_Yes;
    }
    else
    {
        pos_to->satltSysNo4InUse = Ipcb_SatltSysNo4InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.qzssL1IsUsed)
    {
        pos_to->satltSysNo5InUse = Ipcb_SatltSysNo5InUse_noYes1_Yes;
    }
    else
    {
        pos_to->satltSysNo5InUse = Ipcb_SatltSysNo5InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.qzssL1SAIFIsUsed)
    {
        pos_to->satltSysNo6InUse = Ipcb_SatltSysNo6InUse_noYes1_Yes;
    }
    else
    {
        pos_to->satltSysNo6InUse = Ipcb_SatltSysNo6InUse_noYes1_No;
    }

    // Navigation solution status
    switch (pos_from->positioningStatus.fixType) {
    case notAvailable:
    case noFix:
    default:
        pos_to->satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl1;
        break;
    case fix2D:
        pos_to->satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl2;
        break;
    case fix3D:
        pos_to->satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl3;
        break;
    case startupMode:
        pos_to->satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl4;
        break;
    };
    
    pos_to->satltPosnStsPem2 = pos_from->positioningStatus.dgpsIsUsed; // DGPS usage in solution    
    pos_to->satltPosnStsPem3 = pos_from->positioningStatus.selfEphemerisDataUsage; // Self ephemeris data usage in solution
    
    pos_to->noOfSatltForSysNo1 = pos_from->satelliteInfo.nrOfSatellitesUsed.gps; // Number of GPS satellites used for positioning
    pos_to->noOfSatltForSysNo2 = pos_from->satelliteInfo.nrOfSatellitesUsed.glonass; // Number of GLONASS satellites used for positioning
    pos_to->noOfSatltForSysNo3 = pos_from->satelliteInfo.nrOfSatellitesUsed.galileo; // Number of Galileo satellites used for positioning
    pos_to->noOfSatltForSysNo4 = pos_from->satelliteInfo.nrOfSatellitesUsed.sbas; // Number of SBAS satellites used for positioning
    pos_to->noOfSatltForSysNo5 = pos_from->satelliteInfo.nrOfSatellitesUsed.qzssL1; // Number of QZSS L1 satellites used for positioning
    pos_to->noOfSatltForSysNo6 = pos_from->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF; // Number of QZSS L1-SAIF satellites used for positioning
    pos_to->prePosnDil = pos_from->precision.pdop; // Position Dilution Of Precision
    pos_to->preHozlDil = pos_from->precision.hdop; // Horizontal Dilution Of Precision
    pos_to->preVertDil = pos_from->precision.vdop; // Vertical Dilution Of Precision

    //Need check??
    pos_to->preTiDil = pos_from->precision.tdop; // Time Dilution Of Precision

    //To check if proper. LY.
    pos_to->tiForYr = (uint8_t) (pos_from->utcTime.year - 2000); // UTC year
    pos_to->tiForMth = (uint8_t) pos_from->utcTime.month; // UTC month
    pos_to->tiForDay = (uint8_t) pos_from->utcTime.day; // UTC day
    pos_to->tiForHr = (uint8_t) pos_from->utcTime.hour; // UTC hour
    pos_to->tiForMins = (uint8_t) pos_from->utcTime.minute; // UTC minute
    pos_to->tiForSec = (uint8_t) pos_from->utcTime.second; // UTC second
    return true;
}

/*
bool VolvoPositioningManager::setGNSSSolution_Data(OpPosnFromSatltPush_NotificationCyclic_Data *pos_to, GNSSData *pos_from)
{
    if (!pos_from || !pos_to) {
        return false;
    }
    
    if(true == pos_from->gnssStatus.gpsIsUsed)
    {
        pos_to->gNSSSolution.satltSysNo1InUse = Ipcb_SatltSysNo1InUse_noYes1_Yes;
    }
    else
    {
        pos_to->gNSSSolution.satltSysNo1InUse = Ipcb_SatltSysNo1InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.glonassIsUsed)
    {
        pos_to->gNSSSolution.satltSysNo2InUse = Ipcb_SatltSysNo2InUse_noYes1_Yes;
    }
    else
    {
        pos_to->gNSSSolution.satltSysNo2InUse = Ipcb_SatltSysNo2InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.galileoIsUsed)
    {
        pos_to->gNSSSolution.satltSysNo3InUse = Ipcb_SatltSysNo3InUse_noYes1_Yes;
    }
    else
    {
        pos_to->gNSSSolution.satltSysNo3InUse = Ipcb_SatltSysNo3InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.sbasIsUsed)
    {
        pos_to->gNSSSolution.satltSysNo4InUse = Ipcb_SatltSysNo4InUse_noYes1_Yes;
    }
    else
    {
        pos_to->gNSSSolution.satltSysNo4InUse = Ipcb_SatltSysNo4InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.qzssL1IsUsed)
    {
        pos_to->gNSSSolution.satltSysNo5InUse = Ipcb_SatltSysNo5InUse_noYes1_Yes;
    }
    else
    {
        pos_to->gNSSSolution.satltSysNo5InUse = Ipcb_SatltSysNo5InUse_noYes1_No;
    }

    if(true == pos_from->gnssStatus.qzssL1SAIFIsUsed)
    {
        pos_to->gNSSSolution.satltSysNo6InUse = Ipcb_SatltSysNo6InUse_noYes1_Yes;
    }
    else
    {
        pos_to->gNSSSolution.satltSysNo6InUse = Ipcb_SatltSysNo6InUse_noYes1_No;
    }

    // Navigation solution status
    switch (pos_from->positioningStatus.fixType) {
    case notAvailable:
    case noFix:
    default:
        pos_to->gNSSSolution.satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl1;
        break;
    case fix2D:
        pos_to->gNSSSolution.satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl2;
        break;
    case fix3D:
        pos_to->gNSSSolution.satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl3;
        break;
    case startupMode:
        pos_to->gNSSSolution.satltPosnStsPem1 = Ipcb_satltPosnSts_PosnStsOfLvl4;
        break;
    };
    
    pos_to->gNSSSolution.satltPosnStsPem2 = pos_from->positioningStatus.dgpsIsUsed; // DGPS usage in solution    
    pos_to->gNSSSolution.satltPosnStsPem3 = pos_from->positioningStatus.selfEphemerisDataUsage; // Self ephemeris data usage in solution
    
    pos_to->gNSSSolution.noOfSatltForSysNo1 = pos_from->satelliteInfo.nrOfSatellitesUsed.gps; // Number of GPS satellites used for positioning
    pos_to->gNSSSolution.noOfSatltForSysNo2 = pos_from->satelliteInfo.nrOfSatellitesUsed.glonass; // Number of GLONASS satellites used for positioning
    pos_to->gNSSSolution.noOfSatltForSysNo3 = pos_from->satelliteInfo.nrOfSatellitesUsed.galileo; // Number of Galileo satellites used for positioning
    pos_to->gNSSSolution.noOfSatltForSysNo4 = pos_from->satelliteInfo.nrOfSatellitesUsed.sbas; // Number of SBAS satellites used for positioning
    pos_to->gNSSSolution.noOfSatltForSysNo5 = pos_from->satelliteInfo.nrOfSatellitesUsed.qzssL1; // Number of QZSS L1 satellites used for positioning
    pos_to->gNSSSolution.noOfSatltForSysNo6 = pos_from->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF; // Number of QZSS L1-SAIF satellites used for positioning
    pos_to->gNSSSolution.prePosnDil = pos_from->precision.pdop; // Position Dilution Of Precision
    pos_to->gNSSSolution.preHozlDil = pos_from->precision.hdop; // Horizontal Dilution Of Precision
    pos_to->gNSSSolution.preVertDil = pos_from->precision.vdop; // Vertical Dilution Of Precision

    //Need check??
    pos_to->gNSSSolution.preTiDil = pos_from->precision.tdop; // Time Dilution Of Precision

    //To check if proper. LY.
    pos_to->gNSSSolution.tiForYr = (uint8_t) (pos_from->utcTime.year - 2000); // UTC year
    pos_to->gNSSSolution.tiForMth = (uint8_t) pos_from->utcTime.month; // UTC month
    pos_to->gNSSSolution.tiForDay = (uint8_t) pos_from->utcTime.day; // UTC day
    pos_to->gNSSSolution.tiForHr = (uint8_t) pos_from->utcTime.hour; // UTC hour
    pos_to->gNSSSolution.tiForMins = (uint8_t) pos_from->utcTime.minute; // UTC minute
    pos_to->gNSSSolution.tiForSec = (uint8_t) pos_from->utcTime.second; // UTC second

    return true;
}
*/

void VolvoPositioningManager::gnssCreateFakePos(GNSSData *pos)
{
    pos->datastatus = GNSS_DATA_INVALID;
    pos->utcTime.year   = 2017;
    pos->utcTime.month  = 2;
    pos->utcTime.day    = 8;
    pos->utcTime.hour   = 13;
    pos->utcTime.minute = 48;
    pos->utcTime.second = 43;
    getGpsTime(1486561723197, &pos->gpsTime);
    pos->positioningStatus.fixType = noFix;
    pos->position.longlat.longitude = 0;
    pos->position.longlat.latitude = 0;
    pos->position.altitude = 0;
    pos->movement.speed = 0;
    pos->movement.horizontalVelocity = 0;
    pos->movement.verticalVelocity = 0;
    pos->heading = 0;
    pos->magnetic_heading = 0;
    pos->positioningStatus.dgpsIsUsed = false;
    pos->positioningStatus.selfEphemerisDataUsage = false;
    pos->satelliteInfo.nrOfSatellitesVisible.gps = 0;
    pos->satelliteInfo.nrOfSatellitesVisible.sbas = 0;
    pos->satelliteInfo.nrOfSatellitesVisible.glonass = 0;
    pos->satelliteInfo.nrOfSatellitesVisible.galileo = 0;
    pos->satelliteInfo.nrOfSatellitesVisible.qzssL1 = 0;
    pos->satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF = 0;
    memset(&pos->satelliteInfo.nrOfSatellitesUsed, 0, sizeof(pos->satelliteInfo.nrOfSatellitesUsed));
    pos->satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF = 0;
    pos->gnssStatus.gpsIsUsed     = 0;
    pos->gnssStatus.sbasIsUsed    = 0;
    pos->gnssStatus.glonassIsUsed = 0;
    pos->gnssStatus.galileoIsUsed = 0;
    pos->gnssStatus.qzssL1IsUsed  = 0;
    pos->gnssStatus.qzssL1SAIFIsUsed = false;
    pos->precision.hdop = 0;
    pos->precision.vdop = 0;
    pos->precision.pdop = 0;
    pos->precision.tdop = 0;
    pos->receiverChannels.min_size = 0;
    pos->receiverChannels.max_size = 0;
    pos->receiverChannels.actual_size = 0;
    pos->receiverChannels.data_array = 0;
}

bool VolvoPositioningManager::LmGetPosition(GNSSData *posData)
{
    tpLM_Error_t lmStatus;
    tpLM_Position_t pos;
    tpLM_Time_t utc;
    tpLM_SourceStatus_t status;
    bool retval = true;
    static int shift_init_flag = 0;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s begin", __FUNCTION__);

    if(posData == nullptr)
    {
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_ERROR, "posData is nullptr!");
        return false;        
    }

    if (lmIsReady == false) {
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_ERROR, "lmIsReady is false");
        return false;
    }

    lmStatus = tpLM_setDatum(E_LM_DATUM_WGS84);
    if (lmStatus != E_LM_ERROR_NONE) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpLM_setDatum failed:%d", lmStatus);
    }

    lmStatus = tpLM_getPosition(&pos);
    if (lmStatus != E_LM_ERROR_NONE) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpLM_getPosition failed:%d", lmStatus);
        retval = false;
    }

    if (retval == true) {
        lmStatus = tpLM_getTime(&utc);
        if (lmStatus != E_LM_ERROR_NONE) {
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpLM_getTime failed:%d", lmStatus);
            retval = false;
        }
    }

    if (pos.status == E_LM_DATA_INVALID) {    //HX change from 'E_LM_GNSS_DATA_INVALID'
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_ERROR, "tpLM_getPosition: Position status is INVALID");
        if (conti_.fake_pos) {
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s Using fake position data", __FUNCTION__);
            //We are creating a fake position that will be ok by ASN1 check in lower layers.
            //This is for test purpose to avoid getting error response when real position is not available.
            gnssCreateFakePos(posData);
            return true;
        } else {
            retval = false;
        }
    }

    if (retval == true) {
        lmStatus = tpLM_getSourceStatus(&status);
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "tpLM_getSourceStatus:%d", status);
        if (lmStatus != E_LM_ERROR_NONE) {
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpLM_gnssGetStatus failed:%d", lmStatus);
            retval = false;
        }
    }

    if (retval == true) {
        //Convert tpLMGnssPosition_t to GNSSData
        if (!LmPosToVpomPos(&pos, &utc, &status, posData)) {
            DLT_LOG_STRING(dlt_vpom, DLT_LOG_ERROR, "LmPosToVpomPos failed:%d");
            retval = false;
        }
    }

    //jupeng: Chinese shift
    unsigned int china_long = 0;
    unsigned int china_lat  = 0;
    // print location infor before shifting
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "init flag             %d", shift_init_flag);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "longitude             %d", posData->position.longlat.longitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "latitude              %d", posData->position.longlat.latitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "altitude              %d", posData->position.altitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "week number           %d", posData->gpsTime.weekNumber);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "time of the week      %d", posData->gpsTime.timeOfWeek);

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%d, %d, %d, %d, %d, %d", shift_init_flag, posData->position.longlat.longitude,
                                             posData->position.longlat.latitude, 
                                             posData->position.altitude,posData->gpsTime.weekNumber,
                                             posData->gpsTime.timeOfWeek);
    
    wgtochina_lb(shift_init_flag, posData->position.longlat.longitude, posData->position.longlat.latitude,
                    posData->position.altitude, posData->gpsTime.weekNumber,posData->gpsTime.timeOfWeek,
                    &china_long, &china_lat);
    shift_init_flag == 0 ? shift_init_flag++ : 0;

    posData->position.longlat.longitude = china_long;
    posData->position.longlat.latitude  = china_lat;

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "After position shifting:");
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "longitude              %d", posData->position.longlat.longitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "latitude               %d", posData->position.longlat.latitude);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "altitude               %d", posData->position.altitude);
    

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s end", __FUNCTION__);

    return retval;
}

//Local config begin
//Read from LocalConfig
void VolvoPositioningManager::ReadLocalConfig()
{
    int nrOk;
    int nrFail;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    lcfg::ILocalConfig *lCClient(lcfg::ILocalConfig::GetInstance());

    //Conti; get from LocalConfig
    nrOk = 0;
    nrFail = 0;
    int index = 0;
    while (index < vpom::VPOM_LOCCONFCONTI_SIZE) {
        if (vpomLocConfConti_[index].is_string_value) {
            if (lCClient->GetString(vpomLocConfConti_[index].coll_name,
                                    vpomLocConfConti_[index].par_name,
                                    vpomLocConfConti_[index].str_value) == true) {
                nrOk++;
            } else {
                nrFail++;
                DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "%s conti GetString() %s failed",
                                                         __FUNCTION__, vpomLocConfConti_[index].par_name.c_str());
            }
        } else {
            if (lCClient->GetInt(vpomLocConfConti_[index].coll_name,
                                 vpomLocConfConti_[index].par_name,
                                 vpomLocConfConti_[index].int_value) == true) {
                nrOk++;
            } else {
                nrFail++;
                DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "%s conti GetInt() %s failed",
                                                          __FUNCTION__, vpomLocConfConti_[index].par_name.c_str());
            }
        }
        index++;
    }

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s conti nrOk:%d nrFail:%d", __FUNCTION__, nrOk, nrFail);

    //Save internally
    SetLocalConfigConti();
}

bool VolvoPositioningManager::SetLocalConfigConti(void)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "%s", __FUNCTION__);

    conti_.fake_pos = vpomLocConfConti_[vpom::VPOM_FAKE_POS].int_value;
    conti_.print_gnss_pos_data = vpomLocConfConti_[vpom::VPOM_PRINT_GNSS_POS_DATA].int_value;
    conti_.gnss_pos_data_interval = vpomLocConfConti_[vpom::VPOM_GNSS_POS_DATA_INTERVAL].int_value;
    conti_.start_gnss_pos_data_notifications = vpomLocConfConti_[vpom::VPOM_START_GNSS_POS_DATA_NOTIFICATIONS].int_value;

    return true;
}
//Local config end

void VolvoPositioningManager::PositionDataRawRequest(uint64_t request_id)
{
    GNSSData posData;

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    if (LmGetPosition(&posData)) {
        ipcbGnssProxy_.PositionDataRawResponse(posData.utcTime.year,
                                               posData.utcTime.month,
                                               posData.utcTime.day,
                                               posData.utcTime.hour,
                                               posData.utcTime.minute,
                                               posData.utcTime.second,
                                               posData.gpsTime.weekNumber,
                                               posData.gpsTime.timeOfWeek,
                                               posData.position.longlat.longitude,
                                               posData.position.longlat.latitude,
                                               posData.position.altitude,
                                               posData.movement.speed,
                                               posData.movement.horizontalVelocity,
                                               posData.movement.verticalVelocity,
                                               posData.heading,
                                               posData.gnssStatus.gpsIsUsed,
                                               posData.gnssStatus.glonassIsUsed,
                                               posData.gnssStatus.galileoIsUsed,
                                               posData.gnssStatus.sbasIsUsed,
                                               posData.gnssStatus.qzssL1IsUsed,
                                               posData.gnssStatus.qzssL1SAIFIsUsed,
                                               posData.positioningStatus.fixType,
                                               posData.positioningStatus.dgpsIsUsed,
                                               posData.positioningStatus.selfEphemerisDataUsage,
                                               posData.satelliteInfo.nrOfSatellitesVisible.gps,
                                               posData.satelliteInfo.nrOfSatellitesVisible.glonass,
                                               posData.satelliteInfo.nrOfSatellitesVisible.galileo,
                                               posData.satelliteInfo.nrOfSatellitesVisible.sbas,
                                               posData.satelliteInfo.nrOfSatellitesVisible.qzssL1,
                                               posData.satelliteInfo.nrOfSatellitesVisible.qzssL1SAIF,
                                               posData.satelliteInfo.nrOfSatellitesUsed.gps,
                                               posData.satelliteInfo.nrOfSatellitesUsed.glonass,
                                               posData.satelliteInfo.nrOfSatellitesUsed.galileo,
                                               posData.satelliteInfo.nrOfSatellitesUsed.sbas,
                                               posData.satelliteInfo.nrOfSatellitesUsed.qzssL1,
                                               posData.satelliteInfo.nrOfSatellitesUsed.qzssL1SAIF,
                                               posData.precision.hdop,
                                               posData.precision.vdop,
                                               posData.precision.pdop,
                                               posData.precision.tdop,
                                               posData.receiverChannels.min_size,
                                               posData.receiverChannels.max_size,
                                               posData.receiverChannels.actual_size,
                                               posData.receiverChannels.data_array,
                                               request_id,
                                               true);
    } else {
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "LmGetPosition failed. Gnss notification is NOT sent");
    }
}

void VolvoPositioningManager::PositionDataRawRequestCb(uint64_t request_id)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    //TODO use requestId from subscribe request
    VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());
    std::async(std::launch::async, std::bind(&VolvoPositioningManager::PositionDataRawRequest,
                                             &vpom_, request_id));
}

void VolvoPositioningManager::VCResponseCb(vc::MessageBase* message, vc::ReturnValue status)
{
    if (message == NULL) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VolvoPositioningManager::%s: message is NULL", __FUNCTION__);
        return;
    }

    if ((message->type_ != vc::MESSAGE_RESPONSE) || (status != vc::RET_OK)) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VolvoPositioningManager::%s: error: %d( %s )", __FUNCTION__, status, vc::ReturnValueStr[status]);
        delete message;
        return;
    }

    long session_id = message->session_id_;
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager::%s session_id:%d", __FUNCTION__, session_id);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager::%s message id %d", __FUNCTION__, message->id_);

    switch ((vc::ResponseID)message->id_) {
        case vc::RES_REQUESTSENT: {
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager::VCResponseCb Request Sent status=%d", status);
            break;
        }
        case vc::RES_CARMODE: {           
            //vc::Message<vc::EventCarMode> *m = dynamic_cast<vc::Message<vc::EventCarMode>*>(vcmessagebase);
            //CarModeSignal* sig = new CarModeSignal(Signal::kCarMode, transaction_id, m->message, status);
           vc::ResCarMode& Res_car_mode = ((vc::Message<vc::ResCarMode>*)message)->message;           
           carMode = Res_car_mode.carmode;
           DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s CAR MODE =%d", __FUNCTION__, carMode);     
           break;
        }
        case vc::RES_CARUSAGEMODE: {
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager::VCResponseCb RES_CARMODE status=%d", status);
            vc::ResCarUsageMode& Res_usage_mode = ((vc::Message<vc::ResCarUsageMode>*)message)->message;    
            usageMode = Res_usage_mode.usagemode;            
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s Usage MODE =%d", __FUNCTION__, usageMode);      
            break;
        }
    
        default:
            break;
    }

    delete message;
}


//static void VolvoPositioningManager::VpomServiceProxy(OperationId id, OperationType type, uint64_t requestId, DataBase* data)
void VolvoPositioningManager::VpomServiceProxy(OperationId id, OperationType type, uint64_t requestId, DataBase* data)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);
    
    switch (id)
    {
        case OperationId::VehModandDateInfo:
        {
            VehModMngtGlbSafe1_Data* newdata = dynamic_cast<VehModMngtGlbSafe1_Data*>(data);
            VolvoPositioningManager::getInstance().VehModandDateInfoNotificationCycleCb(newdata);
            
            //VehModandDateInfoNotificationCycleCb((VehModMngtGlbSafe1_Data*)data);
            //PositionDataRawRequestCb(1000);
            break;
        }
        default:
            break;
    }
    
}

//add by HX
void VolvoPositioningManager::VehModandDateInfoNotificationCycleCb(VehModMngtGlbSafe1_Data *info)
{
    //std::lock_guard<std::mutex> guard(m_vehModandDateInfo_mutex);   //To check
    VehModandDateCopy(&m_vehModandDateInfo, info);
    
    //std::lock_guard<std::mutex> guard(m_vehModandDateInfo_mutex);
    PrintVehModandDateInfo(&m_vehModandDateInfo);
}


void VolvoPositioningManager::PrintVehModandDateInfo(VehModMngtGlbSafe1_Data* info)
{
    DLT_LOG_STRING(dlt_vpom, DLT_LOG_DEBUG, "VehModandDateInfo:");
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "usgModSts1 %d", info->usgModSts1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "carModSts1 %d", info->carModSts1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "carModSubtypWdCarModSubtyp %d", info->carModSubtypWdCarModSubtyp);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "egyLvlElecMai %d", info->egyLvlElecMai);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "egyLvlElecSubtyp %d", info->egyLvlElecSubtyp);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "pwrLvlElecMai %d", info->pwrLvlElecMai);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "pwrLvlElecSubtyp %d", info->pwrLvlElecSubtyp);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "fltEgyCnsWdSts %d", info->fltEgyCnsWdSts);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "chks %d", info->chks);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "cntr %d", info->cntr);
#if 0
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "yr1 %d", info->yr1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "mth1 %d", info->mth1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "day %d", info->day);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "hr1 %d", info->hr1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "mins1 %d", info->mins1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "sec1 %d", info->sec1);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "dataValid %d", info->dataValid);
#endif
}

bool VolvoPositioningManager::VehModandDateCopy(VehModMngtGlbSafe1_Data *info_dest, VehModMngtGlbSafe1_Data *info_src)
{
    if (!info_src || !info_dest) 
    {
        return false;
    }

    info_dest->usgModSts1 = info_src->usgModSts1;
    info_dest->carModSts1 = info_src->carModSts1;
    info_dest->carModSubtypWdCarModSubtyp = info_src->carModSubtypWdCarModSubtyp;
    info_dest->egyLvlElecMai = info_src->egyLvlElecMai;
    info_dest->egyLvlElecSubtyp = info_src->egyLvlElecSubtyp;
    info_dest->pwrLvlElecMai = info_src->pwrLvlElecMai;
    info_dest->pwrLvlElecSubtyp = info_src->pwrLvlElecSubtyp;
    info_dest->fltEgyCnsWdSts = info_src->fltEgyCnsWdSts;
    info_dest->chks = info_src->chks;
    info_dest->cntr = info_src->cntr;
#if 0
    info_dest->yr1 = info_src->yr1;
    info_dest->mth1 = info_src->mth1;
    info_dest->day = info_src->day;
    info_dest->hr1 = info_src->hr1;
    info_dest->mins1 = info_src->mins1;
    info_dest->sec1 = info_src->sec1;
    info_dest->dataValid = info_src->dataValid;
#endif

    return true;
}

bool VolvoPositioningManager::CheckNotifyCondition()
{
    
    //To add ...
    if((vc::CAR_TRANSPORT == carMode)||
        (vc::CAR_CARMODFCY == carMode)||
        (vc::CAR_ABANDONED == usageMode))
    {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager: %s = false", __FUNCTION__);
        return true;  //should be 'false', temp set 'true' for development, need change to false in formal code.
    }
    else
    {
        return true;
    }
}

int VolvoPositioningManager::Suspend(void)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager: %s", __FUNCTION__);

    notifyLoopEnd_ = true;

    return 0;
}

int VolvoPositioningManager::Resume(void)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager: %s", __FUNCTION__);

    if (notifyLoopEnd_) {
        notifyLoopEnd_ = false;

        if (conti_.start_gnss_pos_data_notifications) {
            notifyLoop_ = true;
            VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());
            if (notifyThread_.joinable()) {
                notifyThread_.join();
            }
            notifyThread_ = std::thread(std::bind(&VolvoPositioningManager::sendGnssNotifications, &vpom_));
        }
    }

    return 0;
}

int VolvoPositioningManager::Stop(void)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager: %s", __FUNCTION__);

    return 0;
}

int VolvoPositioningManager::LcmHandler(VpomLcmSignal sig)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager: %s", __FUNCTION__);

    VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());

    switch(sig) {
    case VPOM_LCM_RESUME:
        return vpom_.Resume();
    case VPOM_LCM_SUSPEND:
        return vpom_.Suspend();
    case VPOM_LCM_SHUTDOWN:
        return vpom_.Stop();
    default:
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VolvoPositioningManager: %s: signal unknown!", __FUNCTION__);
        return 1;
    }
}

bool VolvoPositioningManager::Init()
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s", __FUNCTION__);

    VpomLCM& VpomLCM_(VpomLCM::getInstance());
    VPOM_GDBus& vpomGdbus_(VPOM_GDBus::getInstance());

    // Init TPSys before accessing gdbus
    if (tpSYS_initIPC(E_SYS_IPC_DBUS, NULL, NULL) != E_SYS_IPC_RET_SUCCESS) {
        DLT_LOG_STRING(dlt_vpom, DLT_LOG_ERROR, "tpSYS Failed to intitialize DBUS.");
        return false;
    }

    tpPCL_Error_t rc = E_PCL_ERROR_NONE;
    rc = tpPCL_init("vpom_persist_data_mgr", nullptr, FALSE);
    if(E_PCL_ERROR_NONE != rc)
    {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "tpPCL_init rc=%x", rc);
    }

    ReadLocalConfig();

    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "Using fake_pos if invalid:%d", conti_.fake_pos);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "Print gnss pos  data:%d",      conti_.print_gnss_pos_data);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "Gnss Notification interval:%d",conti_.gnss_pos_data_interval);
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "Gnss Notifications auto:%d",   conti_.start_gnss_pos_data_notifications);

    std::thread VpomGdbusThr{std::bind(&VPOM_GDBus::run, &vpomGdbus_)};

    //Wait for the bus to be acquired before continuing
    vpomGdbus_.WaitForDbusAcquired();

    if (IpcbIClient::Init() != true) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "%s: IpcbIClient::Init() failed", __FUNCTION__);
        Deinit();
        return false;
    }

    //LUY, may change with other callback.
    //ipcbGnssProxy_.RegisterPositionDataRawRequestCb(PositionDataRawRequestCb);    
    m_IpcbPosSrv.registerPositioningService((fgen)(VpomServiceProxy));

    //Location Manager Init
    LmInit();

    //VehicleComm init
    vc::IVehicleComm& ivc(vc::IVehicleComm::GetInstance());

    vc::ReturnValue vcretval = ivc.Init(std::bind(&VolvoPositioningManager::VCResponseCb, this, std::placeholders::_1, std::placeholders::_2), 
            std::bind(&VolvoPositioningManager::EventCallback, this, std::placeholders::_1));
    if (vcretval != vc::RET_OK) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VolvoPositioningManager::%s: VehicleComm Init failed: %s",
                        __FUNCTION__, vc::ReturnValueStr[vcretval]);
        return false;
    }
    
    vc::IGeneral& igen = ivc.GetGeneralInterface();   
    if (igen.Request_CarMode(sesion_id) == vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "%s requests for car mode sent to VehicleComm.",__FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "%s failed to issue request to VehicleComm.",__FUNCTION__);
    }

    sesion_id++;
    if (igen.Request_CarUsageMode(sesion_id) == vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "%s requests for car usage sent to VehicleComm.",__FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "%s failed to issue car usage request to VehicleComm.",__FUNCTION__);
    }

    //Automatic Gnss position data notifications
    if (conti_.start_gnss_pos_data_notifications) {
        notifyLoop_ = true;
        VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());
        notifyThread_ = std::thread(std::bind(&VolvoPositioningManager::sendGnssNotifications, &vpom_));
    }

    //Start Life cycle manager
    VpomLCM_.Init(LcmHandler);
    VpomGdbusThr.join();
    return true;
}


void VolvoPositioningManager::EventCallback(vc::MessageBase* message)
{
    if (message == NULL) {
        DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_ERROR, "VolvoPositioningManager::%s: message is NULL", __FUNCTION__);
        return;
    }

    switch ((vc::ResponseID)message->id_) {        
        case vc::RES_CARMODE: {           
            //vc::Message<vc::EventCarMode> *m = dynamic_cast<vc::Message<vc::EventCarMode>*>(vcmessagebase);
            //CarModeSignal* sig = new CarModeSignal(Signal::kCarMode, transaction_id, m->message, status);
           vc::ResCarMode& Res_car_mode = ((vc::Message<vc::ResCarMode>*)message)->message;           
           carMode = Res_car_mode.carmode;
           DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s CAR MODE =%d", __FUNCTION__, carMode);           
           break;
        }
        case vc::RES_CARUSAGEMODE: {
            vc::ResCarUsageMode& Res_usage_mode = ((vc::Message<vc::ResCarUsageMode>*)message)->message;    
            usageMode = Res_usage_mode.usagemode;            
            DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "%s Usage MODE =%d", __FUNCTION__, usageMode);           
            break;
        }    
        default:
            break;
    }
}


void VolvoPositioningManager::Deinit()
{
    ipcbGnssProxy_.DeregisterPositionDataRawRequestCb();

    notifyLoopEnd_ = true;
    active_ = false;
}

void SignalHandler(int signum)
{
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_DEBUG, "VolvoPositioningManager: %s SIGINT (%d) -> good bye!",
                                             __FUNCTION__,
                                             signum);

    //KILL THREAD HOW??

    //signal(signum, SIG_DFL);
    //raise(signum);
    VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());
    vpom_.Deinit();

    tpSYS_deinitIPC();
    exit(0);
}

int main()
{
    DLT_REGISTER_APP("VPOM", "Volvo Positioning Manager Application");

    VolvoPositioningManager& vpom_(VolvoPositioningManager::getInstance());

    /** Pick and activate one of the register context below.
     * DLT_REGISTER_CONTEXT shows up to  DLT_LOG_INFO prints in the console
     * DLT_REGISTER_CONTEXT_LL_TS shows also verbose prints in the console
     */
    DLT_REGISTER_CONTEXT_LL_TS(dlt_vpom, "VPOM", "Volvo Positioning Manager", DLT_LOG_VERBOSE, DLT_TRACE_STATUS_ON);
    //DLT_REGISTER_CONTEXT(dlt_vpom, "VPOM","Volvo Positioning Manager");

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "DLT Volvo Positioning Manager Startup");
    DLT_LOG_STRINGF(dlt_vpom, DLT_LOG_INFO, "Last VPOM git commit patch is: %s.", VPOM_GIT_VERSION);

    std::signal(SIGTERM, SignalHandler);
    std::signal(SIGINT, SignalHandler);

    if (vpom_.Init() == false) {
        vpom_.Deinit();
        return 1;
    }

    DLT_UNREGISTER_CONTEXT(dlt_vpom);
    DLT_UNREGISTER_APP();

    DLT_LOG_STRING(dlt_vpom, DLT_LOG_INFO, "DLT Volvo Positioning Manager End");

    return 0;
}
