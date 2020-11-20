/**
 * Copyright (C) 2017 Continental Automotive AG and Subsidiaries
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     admin_confirmation_transaction.h
 *  \brief    Device pairing admin confirmation transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "journey_log/car_journeylog_transaction.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "timestamp.h"
#include "dlt/dlt.h"

#include "voc_framework/signal_sources/vocmo_signal_source.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

static fsm::VpomTransactionId position_transaction_id;

CarJourneyLogTransaction::CarJourneyLogTransaction() : fsm::SmartTransaction(kStop),
    vpom_gnss_service_object(fsm::VpomSignalSource::GetInstance().GetPositioningServiceObject())
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "JL:%s()m_currentJLData = %d", __FUNCTION__, (void *) m_currentJLData.get());
    fsm::VdServiceTransactionId vdsTransId;
    asn_wrapper::VDServiceRequest *vdmsg = nullptr;
    m_currentJLData =  std::dynamic_pointer_cast<GlyVdsJLSignal> (GlyVdsJLSignal::CreateGlyVdsJLSignal(vdsTransId, vdmsg));
    if (m_currentJLData.get() == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "JL:%s() m_currentJLData.get() == nullptr", __FUNCTION__);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "JL:%s()m_currentJLData = %d", __FUNCTION__, (void *) m_currentJLData.get());

    StateMap state_map = {{kStart,              {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kCollectData, kStop}}}, // valid transitions
                          {kCollectData,        {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kStop}}},             // valid transitions
                          {kStop,               {nullptr,               // state function
                                               SignalFunctionMap(),   // signal function map
                                               {kStart}}}};                 // valid transitions
    state_map[kStart].state_function =
        std::bind(&CarJourneyLogTransaction::HandleBeginJLService,this);

    state_map[kCollectData].signal_function_map[fsm::Signal::kTelmDshb] =
        std::bind(&CarJourneyLogTransaction::HandleCollectTelmDshbData,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kGNSSPositionDataSignal] =
        std::bind(&CarJourneyLogTransaction::HandleCollectPositionData,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kEndOfTrip] =
        std::bind(&CarJourneyLogTransaction::HandleEndOfTrip,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kTimeout] =
        std::bind(&CarJourneyLogTransaction::HandleTimeOut,
                  this,
                  std::placeholders::_1);

    state_map[kStop].state_function =
        std::bind(&CarJourneyLogTransaction::HandleStopJLService,this);

    state_map[kStop].signal_function_map[fsm::Signal::kCarUsageMode] =
        std::bind(&CarJourneyLogTransaction::HandleStartJLService,
                  this,
                  std::placeholders::_1);

    SetStateMap(state_map);

    tripId = 0;

    MapSignalType(fsm::Signal::kCarUsageMode);
    MapSignalType(fsm::Signal::kTelmDshb);
    MapSignalType(fsm::Signal::kEndOfTrip);
    MapSignalType(fsm::Signal::kGNSSPositionDataSignal);
}

void CarJourneyLogTransaction::HandleBeginJLService()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s : state %d", __FUNCTION__, GetState());

    m_kRequestPositionDataId = RequestTimeout(static_cast<std::chrono::seconds>(10), true);
    MapTransactionId(m_kRequestPositionDataId.GetSharedCopy());
    Timestamp now;
    startTime.seconds = (now.epochMicroseconds()/1000000);
    startTime.milliseconds = (now.epochMicroseconds()%1000000)/1000;

    SetState(kCollectData);
}

bool CarJourneyLogTransaction::HandleStartJLService(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    std::shared_ptr<fsm::CarUsageModeSignal> carUsageModeSignal = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s usageMode %d",
                    __FUNCTION__, signal->ToString().c_str(), carUsageModeSignal->GetData()->usagemode);

    if (carUsageModeSignal->GetData()->usagemode == vc::CAR_DRIVING)
    {
        SetState(kStart);
    }

    return true;
}

bool CarJourneyLogTransaction::HandleCollectTelmDshbData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    std::shared_ptr<fsm::TelmDshbSiganl> telmDshbSignal =
                    std::static_pointer_cast<fsm::TelmDshbSiganl>(signal);
    const OpTelmDshb_NotificationCyclic_Data data = telmDshbSignal->GetPayload();
    currentData.avgSpeed = data.journalLog.vehSpdAvgIndcd.vehSpbIndcd;
    currentData.odoMeter = data.journalLog.bkpOfDstTrvld;
    currentData.fuelConsumption = data.journalLog.fuCnsIndcd.fuCnsIndcdVal;
    currentData.traveledDistance = data.journalLog.dstTrvld2;
    currentData.electricConsumption = data.journalLog.pwrCnsIndcd.pwrCns;
    SaveJLDataToQue();
    return true;
}

bool CarJourneyLogTransaction::HandleCollectPositionData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    if( signal->GetTransactionId() == position_transaction_id )
    {
        std::shared_ptr<fsm::VpomGNSSPositionDataSignal> positionDataSignal =
                        std::static_pointer_cast<fsm::VpomGNSSPositionDataSignal>(signal);

        const vpom::GNSSData data = positionDataSignal->GetPayload();

        PositionData_t positionData;
        positionData.latitude = data.position.longlat.latitude;
        positionData.longitude = data.position.longlat.longitude;
        positionData.altitude = data.position.altitude;
        positionData.posCanBeTrusted = true;
        positionData.carLocatorStatUploadEn = true;
        positionData.marsCoordinates = true;

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, latitude:%d", __FUNCTION__, positionData.latitude);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, longitude:%d", __FUNCTION__, positionData.longitude);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, altitude:%d", __FUNCTION__, positionData.altitude);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, posCanBeTrusted:%d", __FUNCTION__, positionData.posCanBeTrusted);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, carLocatorStatUploadEn:%d", __FUNCTION__, positionData.carLocatorStatUploadEn);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, marsCoordinates:%d", __FUNCTION__, positionData.marsCoordinates);

        m_positionList.push_back(positionData);
    }
    return true;
}

bool CarJourneyLogTransaction::HandleUploadDataToTSP(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "signal type %d", signal->GetSignalType());
    UploadJLDataToTSP();
    return true;
}

bool CarJourneyLogTransaction::HandleEndOfTrip(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:start", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "signal type %d", signal->GetSignalType());
    SetState(kStop);
    return true;
}

bool CarJourneyLogTransaction::HandleTimeOut(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:start", __FUNCTION__);
    fsm::TimeoutTransactionId* tsId = (fsm::TimeoutTransactionId*)&(signal->GetTransactionId());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "signal type %d %l",
        signal->GetSignalType(),
        tsId->GetId());

    if (signal->GetTransactionId() == m_kRequestPositionDataId)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:m_kRequestPositionDataId", __FUNCTION__);
        vpom_gnss_service_object.GNSSPositionDataRawRequest(position_transaction_id.GetId());
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:unknown timer", __FUNCTION__);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:end", __FUNCTION__);
    return true;
}

void CarJourneyLogTransaction::HandleStopJLService()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    UploadJLDataToTSP();

    m_journeyDataList.clear();
    m_positionList.clear();

    RemoveTimeout(m_kRequestPositionDataId);
    RemoveMappedTransactions();

    memset(&startTime, 0, sizeof(TimeStamp_t));
}

void CarJourneyLogTransaction::UploadJLDataToTSP()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:start", __FUNCTION__);
    JourneyData_t startdata;
    JourneyData_t enddata;
    memset(&startdata, 0, sizeof(JourneyData_t));
    memset(&enddata, 0, sizeof(JourneyData_t));

    if (m_journeyDataList.size() != 0)
    {
        startdata = m_journeyDataList.front();
        enddata = m_journeyDataList.back();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, tripId:%d", __FUNCTION__, enddata.tripId);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, odoMeter:%d", __FUNCTION__, enddata.odoMeter);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, fuelConsumption:%d", __FUNCTION__, enddata.fuelConsumption);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, traveledDistance:%d", __FUNCTION__, enddata.traveledDistance);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, avgSpeed:%d", __FUNCTION__, enddata.avgSpeed);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, wayPointSize:%d", __FUNCTION__, m_positionList.size());
    }

    Timestamp now;
    endTime.seconds = (now.epochMicroseconds()/1000000);
    endTime.milliseconds = (now.epochMicroseconds()%1000000)/1000;

    JournalData_t jlData;
    jlData.tripId = getTripId();
    jlData.startTime.seconds = startTime.seconds;
    jlData.startTime.milliseconds = startTime.milliseconds;

    jlData.startOdometer = startdata.odoMeter;
    jlData.fuelConsumption = enddata.fuelConsumption;
    jlData.traveledDistance = enddata.traveledDistance;
    jlData.electricConsumption = enddata.electricConsumption;

    for (std::list<PositionData_t>::iterator it = m_positionList.begin(); it != m_positionList.end(); ++it)
    {
        PositionJL_t p;
        p.latitude = it->latitude;
        p.longitude = it->longitude;
        p.altitude = it->altitude;
        p.posCanBeTrusted = it->posCanBeTrusted;
        p.carLocatorStatUploadEn = it->carLocatorStatUploadEn;
        p.marsCoordinates = it->marsCoordinates;
        jlData.waypoints.push_back(p);
    }

    jlData.avgSpeed = enddata.avgSpeed;

    jlData.endTime.seconds = endTime.seconds;
    jlData.endTime.milliseconds = endTime.milliseconds;

    jlData.endOdometer = enddata.odoMeter;
    jlData.electricRegeneration = 0;

    if ( m_currentJLData->SetJLPayload(jlData) == false )
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: SetJLPayload failed.", __FUNCTION__);
        return;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "JL:%s\n, tripId is :%d", __FUNCTION__, jlData.tripId);

    fsm::VocmoSignalSource vocmo_signal_source;
    if (vocmo_signal_source.GeelySendMessage(m_currentJLData))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "JL: upload actvd ok.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "JL: upload actvd fail.");
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:end", __FUNCTION__);
}

void CarJourneyLogTransaction::SaveJLDataToQue()
{
    currentData.tripId = getTripId();

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "journeyLogInfo: tirpId:%d odo:%d fuel:%d traveled:%d avgspd:%d",
                           currentData.tripId, currentData.odoMeter, currentData.fuelConsumption, currentData.traveledDistance, currentData.avgSpeed);

    m_journeyDataList.push_back(currentData);
}

uint32_t CarJourneyLogTransaction::getTripId()
{
    return tripId ++;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
