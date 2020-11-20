// Copyright (C) 2019 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @project     GLY_TCAM
// @subsystem   FSM
// @author      TanChang
// @Init date   27-Feb-2019

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/journeylog_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

JourneyLogSignalAdapt::JourneyLogSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), memset m_vdsData begin  \n", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), memset m_vdsData end  \n", __FUNCTION__);
}

bool JourneyLogSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s\n", __FUNCTION__);

    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new JourneyLogRequest_t();
    JourneyLogRequest_t *journeyLogData = dynamic_cast<JourneyLogRequest_t*>(appData);
    if(journeyLogData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s journeyLogData == nullptr.\n", __FUNCTION__);
        return false;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s completed.\n", __FUNCTION__);

    return true;
}

bool JourneyLogSignalAdapt::SetResultPayload(const JourneyLogServiceResult_t &response)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s\n", __FUNCTION__);

    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_jou;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.trip_presence = true;
    vdsData.body.serviceData.trip.tripId = response.tripId;
    vdsData.body.serviceData.trip.startTime.seconds = response.startTime.seconds;
    vdsData.body.serviceData.trip.startTime.milliseconds_presence = true;
    vdsData.body.serviceData.trip.startTime.milliseconds = response.startTime.milliseconds;
    vdsData.body.serviceData.trip.startOdometer_presence = true;
    vdsData.body.serviceData.trip.startOdometer = response.startOdometer;
    vdsData.body.serviceData.trip.fuelConsumption_presence = true;
    vdsData.body.serviceData.trip.fuelConsumption = response.fuelConsumption;
    vdsData.body.serviceData.trip.traveledDistance = response.traveledDistance;
    vdsData.body.serviceData.trip.electricConsumption_presence = true;
    vdsData.body.serviceData.trip.electricConsumption = response.electricConsumption;

    TrackPoint_Model trackpoint;
    trackpoint.systemTime.seconds = response.endTime.seconds;
    trackpoint.systemTime.milliseconds_presence = true;
    trackpoint.systemTime.milliseconds = response.endTime.milliseconds;
    trackpoint.position.latitude_presence = true;
    trackpoint.position.latitude = 1;
    trackpoint.position.longitude_presence = true;
    trackpoint.position.longitude = 2;
    trackpoint.position.altitude_presence = true;
    trackpoint.position.altitude = 3;
    trackpoint.position.posCanBeTrusted_presence = true;
    trackpoint.position.posCanBeTrusted = true;
    trackpoint.position.carLocatorStatUploadEn_presence = true;
    trackpoint.position.carLocatorStatUploadEn = true;
    trackpoint.position.marsCoordinates_presence = true;
    trackpoint.position.marsCoordinates = true;
    vdsData.body.serviceData.trip.trackpoints.push_back(trackpoint);

    vdsData.body.serviceData.trip.waypoints_presence = true;
    for (std::list<App_Position>::const_iterator it = response.waypoints.begin(); it != response.waypoints.end(); ++it)
    {
        Position_Model waypoint;
        waypoint.latitude_presence = true;
        waypoint.latitude = it->latitude;
        waypoint.longitude_presence = true;
        waypoint.longitude = it->longitude;
        waypoint.altitude_presence = true;
        waypoint.altitude = it->altitude;
        waypoint.posCanBeTrusted_presence = true;
        waypoint.posCanBeTrusted = it->posCanBeTrusted;
        waypoint.carLocatorStatUploadEn_presence = true;
        waypoint.carLocatorStatUploadEn = it->carLocatorStatUploadEn;
        waypoint.marsCoordinates_presence = true;
        waypoint.marsCoordinates = it->marsCoordinates;
        vdsData.body.serviceData.trip.waypoints.push_back(waypoint);
    }

    vdsData.body.serviceData.trip.avgSpeed = response.avgSpeed;
    vdsData.body.serviceData.trip.endTime.seconds = response.endTime.seconds;
    vdsData.body.serviceData.trip.endTime.milliseconds_presence = true;
    vdsData.body.serviceData.trip.endTime.milliseconds = response.endTime.milliseconds;
    vdsData.body.serviceData.trip.endOdometer_presence = true;
    vdsData.body.serviceData.trip.endOdometer = response.endOdometer;

    bool result = PackService(vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "JourneyLogSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

