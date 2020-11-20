///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_jl_signal.cc
// geely journal signal.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Hu Tingting
// @Init date     26-Sep-2018
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_jl_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/journeylog_signal_adapt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

/************************************************************/
// @brief :create journal signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Hu Tingting, 26-Sep-2018
/************************************************************/
std::shared_ptr<fsm::Signal> GlyVdsJLSignal::CreateGlyVdsJLSignal(fsm::VdServiceTransactionId& transaction_id,void* vdsService)
{
#if 0
    GlyVdsJLSignal* signal = new GlyVdsJLSignal(transaction_id, vdsService);

    return std::shared_ptr<GlyVdsJLSignal>(signal);
#endif
    return std::make_shared<GlyVdsJLSignal>(transaction_id, vdsService);
}


/************************************************************/
// @brief :create journal signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Hu Tingting, 26-Sep-2018
/************************************************************/
GlyVdsJLSignal::GlyVdsJLSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceJou),
                   m_vdsService((asn_wrapper::VDServiceRequest *)vdsService),
                   m_response()
{
//    bool result = UnpackPayload();
    bool result = 1;
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,get JL request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,get JL request payload fail.\n", __FUNCTION__);
    }
}


/************************************************************/
// @brief :destruct signal.
// @param[in]  none
// @return
// @author     Hu Tingting, 26-Sep-2018
/************************************************************/
GlyVdsJLSignal::~GlyVdsJLSignal ()
{
    if ( m_vdsService )
    {
        delete m_vdsService;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): freeed m_vdsService", __FUNCTION__);
    }
}


/************************************************************/
// @brief :unpack ASN1C structure.Currently used to receive ack msg.
// @return     True if successfully set, false otherwise
// @author     Hu Tingting, 26-Sep-2018
/************************************************************/
bool GlyVdsJLSignal::UnpackPayload()
{
    m_JLAck = std::make_shared<JournalAck_t>();

    if(m_JLAck == 0)
        return false;

    if(m_vdsService == nullptr)
        return false;

    JourneyLogSignalAdapt adapt(m_vdsService);
    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();

    std::shared_ptr<JourneyLogRequest_t> journeyLogRequest = std::dynamic_pointer_cast<JourneyLogRequest_t>(appData);

    if(journeyLogRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc,DLT_LOG_INFO, "%s(): journeyLogRequest == nullptr", __FUNCTION__);
        return false;
    }

    return true;

}


/************************************************************/
// @brief :pack JournalData_t into ASN1C structure. upload body msg.
// @param[in]  response, JournalData_t structure.
// @return     True if successfully set, false otherwise
// @author     Hu Tingting, 26-Sep-2018
/************************************************************/
bool GlyVdsJLSignal::SetJLPayload(JournalData_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsJLSignal::%s\n", __FUNCTION__);

    m_response = response;

    return true;
}

//////////////////////////////////////////////////////
// @brief : encode vds
// @param[in]  vdServiceRequest
// @return   int
// @author     Tan Chang, Mar 6, 2019
//////////////////////////////////////////////////////
int GlyVdsJLSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s() entered \n", __FUNCTION__);
    if (vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s() vdServiceRequest is null \n", __FUNCTION__);
        return -1;
    }
    JourneyLogServiceResult_t notification = JourneyLogServiceResult_t();
    notification.header.isInitiator = true;

    notification.tripId = m_response.tripId;
    notification.startTime.seconds = m_response.startTime.seconds;
    notification.startTime.milliseconds = m_response.startTime.milliseconds;
    notification.startOdometer = m_response.startOdometer;
    notification.fuelConsumption = m_response.fuelConsumption;
    notification.traveledDistance = m_response.traveledDistance;
    notification.electricConsumption = m_response.electricConsumption;

    for (std::list<PositionJL_t>::iterator it = m_response.waypoints.begin(); it != m_response.waypoints.end(); ++it)
    {
        App_Position waypoint;
        waypoint.latitude = it->latitude;
        waypoint.longitude = it->longitude;
        waypoint.altitude = it->altitude;
        waypoint.posCanBeTrusted = it->posCanBeTrusted;
        waypoint.carLocatorStatUploadEn = it->carLocatorStatUploadEn;
        waypoint.marsCoordinates = it->marsCoordinates;
        notification.waypoints.push_back(waypoint);
    }
    
    notification.avgSpeed = m_response.avgSpeed;
    notification.endTime.seconds = m_response.endTime.seconds;
    notification.endTime.milliseconds = m_response.endTime.milliseconds;
    notification.endOdometer = m_response.endOdometer;
    
    JourneyLogSignalAdapt adapt(vdServiceRequest);
    bool ret = adapt.SetResultPayload(notification);
    return (ret == true? 0 : -1);
}

} // namespace volvo_on_call
