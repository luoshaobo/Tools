////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file dataProcessor.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jiaojiao Shen
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <rms/service/data/dataProcessor.h>
#include <rms/util/utils.h>
#include <rms/config/settings.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

static void pushBytesToVector(std::vector<uint8_t>& vec, const uint8_t *pSrc, size_t bytesToCopy);
template<typename T> static void reverseBytes(T *pVal);
static void pushEcuData(std::vector<uint8_t> &mes, uint32_t ecuToPush, const DtcDBusData *pDtcAr, bool prependDtcCount = true);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::DataProcessor(const Mailbox& mbox)
//! \brief The constructor for the DataProcessor class. As DataProcessor is implemented as singleton, the DataProcessor constructor is 
//!        called once when static variable within DataProcessor::getInstance() function is constructed
//! \param[in] mbox A reference to mailbox that will be assigned to DataProcessor object and through which messages will be received
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DataProcessor::DataProcessor(const Mailbox& mbox):
        Service(mbox),
        mState(DP_STANDBY),
        mSampleIndex(0),
        mSampleIndexExt(0)
{
    //creating default data
    {
        mLastSample = boost::make_shared<PeriodicDBUSData>(PeriodicDBUSData());
        uint8_t *pDstPeriodicData = reinterpret_cast<uint8_t *>( mLastSample.get() );
        for(uint32_t i=0; i<sizeof(PeriodicDBUSData); i++)
        {
            pDstPeriodicData[i] = 0;
        }
    }

//    mTimers.insert(TimerProp(TimerId::SAMPLES_TMR,DPConst::SAMPLING_PERIOD, true));//actually this timer is disabled (never started)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::~DataProcessor()
//! \brief The destructor for the DataProcessor class which is called when the DataProcessor object is destroyed. It currently does nothing.
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DataProcessor::~DataProcessor() 
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s call", __FUNCTION__);
}

//------------------------------------------------------------------------------
// Service
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handle(const boost::shared_ptr<Notify>& notify)
//! \brief method for handling incoming messages
//! \param[in] notify Incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handle(const boost::shared_ptr<Notify>& notify) 
{
    typedef EventHandler<
                        NotifyId,
                        DataProcessor,
                        void (DataProcessor::*)(const boost::shared_ptr<Notify>&)
                        > hr;
    static hr amh[] = {
        hr(NotifyId::EVT_DBUS_START,          *this, &DataProcessor::handleStartProcessing),
        hr(NotifyId::MSG_TIMER_EXPIRED,       *this, &DataProcessor::handleTimers),
        hr(NotifyId::EVT_DBUS_SETTINGS_UPDATE,*this, &DataProcessor::handleSettingsUpdate),
        hr(NotifyId::EVT_DBUS_PERODIC_DATA,   *this, &DataProcessor::handlePeriodicData),
        hr(NotifyId::EVT_DBUS_ENTER_WARNING_MODE,*this, &DataProcessor::handleEnterWarningMode),
        hr(NotifyId::EVT_DBUS_WARNING_DATA,   *this, &DataProcessor::handleWarningData),
        hr(NotifyId::EVT_DBUS_STOP,           *this, &DataProcessor::handleStopProcessing),
    };

    for (uint32_t i = 0; i < sizeof(amh)/ sizeof(amh[0]); i++ ) {
        amh[i].handle(notify->getNotifyId(), notify);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::onStart()
//! \brief Function will be called when DataProcessor is started. It subscribes for events that need to be processed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::onStart()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"RMS: %s call", __FUNCTION__);
    subscribe(boost::make_shared<Event>(Event(NotifyId::EVT_DBUS_START)));
    subscribe(boost::make_shared<Event>(Event(NotifyId::EVT_DBUS_SETTINGS_UPDATE)));
    subscribe(boost::make_shared<Event>(Event(NotifyId::EVT_DBUS_PERODIC_DATA)));
    subscribe(boost::make_shared<Event>(Event(NotifyId::EVT_DBUS_ENTER_WARNING_MODE)));
    subscribe(boost::make_shared<Event>(Event(NotifyId::EVT_DBUS_WARNING_DATA)));
    subscribe(boost::make_shared<Event>(Event(NotifyId::EVT_DBUS_STOP)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::onStop()
//! \brief Function will be called when DataProcessor is stopped. It currently does nothing.
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::onStop()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s call", __FUNCTION__);
}

//------------------------------------------------------------------------------
// Timers timeout handlers
//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleTimers(const boost::shared_ptr<Notify>& notify)
//! \brief Function for dispatching timeout notifications from timers
//! \param[in] notify timeout message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleTimers(const boost::shared_ptr<Notify>& notify) 
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s call", __FUNCTION__);
    typedef EventHandler<
        TimerId,
        DataProcessor,
        void (DataProcessor::*)(const TimerProp&)
    > hr;
    static hr th[] = {
        hr(TimerId::SAMPLES_TMR,   *this, &DataProcessor::handleSamplingTimeout),
    };
    const TimerProp* t = reinterpret_cast<const TimerProp*>(
        notify->getPayloadRef().data());
    if (t) {
        for (uint32_t i = 0; i < sizeof(th)/ sizeof(th[0]); i++ ) {
            th[i].handle(static_cast<TimerId>(t->id), *t);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::getInstance()
//! \brief Function for getting DataProcessor object.
//! \return pointer to DataProcessor object
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DataProcessor* DataProcessor::getInstance() 
{
    static DataProcessor dataProc(Mailbox::SERVICE_DATA_PROCESSOR);
    return &dataProc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn reverseBytes(T*pVal)
//! \brief static template function for variables of integer types reverting byte order (conversion between little endian and big endian)
//! \param[in,out] pVal pointer to variable to be converted
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
static void reverseBytes(T*pVal)
{
    size_t typeSize = sizeof(T);
    uint8_t *pLowAddr=reinterpret_cast<uint8_t*>(pVal);
    uint8_t *pHighAddr=pLowAddr+typeSize-1;

    while(pLowAddr < pHighAddr)
    {
        uint8_t buf = *pLowAddr;
        *pLowAddr = *pHighAddr;
        *pHighAddr = buf;
        pHighAddr--;
        pLowAddr++;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn pushBytesToVector(std::vector<uint8_t>& vec, const uint8_t *pSrc, size_t bytesToCopy)
//! \brief static function for pushing data to vector
//! \param[in,out] vec reference to vector where data should be pushed
//! \param[in] pSrc pointer to source data
//! \param[in] bytesToCopy number of source bytes to be pushed
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void pushBytesToVector(std::vector<uint8_t>& vec, const uint8_t *pSrc, size_t bytesToCopy)
{
    for( uint32_t i = 0; i < bytesToCopy; i++ )
    {
        vec.push_back(pSrc[i]);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn pushEcuData(std::vector<uint8_t> &mes, uint32_t ecuToPush, const DtcDBusData *pDtcAr, bool prependDtcCount)
//! \brief static function for pushing DTC data to vector in GBT format
//! \param[in,out] vec reference to vector where data should be pushed
//! \param[in] ecuToPush ecuId of ECU whose DTC data should be pushed
//! \param[in] pDtcAr pointer to source DTC data array
//! \param[in] prependDtcCount flag indicating whether to prepend DTC data with count of DTC values byte
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void pushEcuData(std::vector<uint8_t> &mes, uint32_t ecuToPush, const DtcDBusData *pDtcAr, bool prependDtcCount)
{
    const int32_t ECU_INDEX_NOT_FOUND = -1;
    int32_t foundIndex = ECU_INDEX_NOT_FOUND;
    for(int32_t i=0; i<XEV_ECU_MAX; i++)
    {
        if(ecuToPush == pDtcAr[i].t_ecu_id)
        {
            if(foundIndex != ECU_INDEX_NOT_FOUND)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: duplicate DTC info found for ecuId=%d, index=%d, previousIndex=%d", ecuToPush, i, foundIndex);
                foundIndex = i;
                continue;
            }
            uint32_t dtcCount = pDtcAr[i].t_dtc_included_count;
            if(DIAG_DTC_MAX < dtcCount)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: t_dtc_included_count(%d) > DIAG_DTC_MAX(%d) for ecuId=%d, dtc[%d]", dtcCount, DIAG_DTC_MAX, ecuToPush, i);
                dtcCount = DIAG_DTC_MAX;
            }
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Adding dtcInfo for ecuID=%d, dtcCount=%d, dtc[0]=%02x %02x %02x %02x"
                 , ecuToPush, dtcCount
                 , pDtcAr[i].t_dtc[0], pDtcAr[i].t_dtc[1], pDtcAr[i].t_dtc[2], pDtcAr[i].t_dtc[3]);

            uint32_t dtcBytes = dtcCount*4;
            if(prependDtcCount)
            {
                mes.push_back(dtcCount);
            }
            for(uint32_t j=0; j<dtcBytes; j++)
            {
                mes.push_back(pDtcAr[i].t_dtc[j]);
            }
            foundIndex = i;
        }
    }

    if(foundIndex == ECU_INDEX_NOT_FOUND)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: DTC info for ecuId=%d not found", ecuToPush);
        if(prependDtcCount)
        {
            mes.push_back(0);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn convertToDataUnitOfGbtPacket(std::vector<uint8_t> &dst, const PeriodicDBUSData * pPdd, const DtcDBusData *pDtcAr)
//! \brief function for forming data unit part of final packet of GBT format from phev alert data
//! \param[in, out] dst vector where conversion result will be stored
//! \param[in] pPdd pointer to periodic data sample
//! \param[in] pDtcAr pointer to source DTC data array if it is fault or null otherwise
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void convertToDataUnitOfGbtPacket(std::vector<uint8_t> &dst, const PeriodicDBUSData * pPdd, const DtcDBusData *pDtcAr)
{
    std::vector<uint8_t> &mes=dst;
    mes.reserve(mes.size()+sizeof(PeriodicDBUSData));

    RealtimeInfoData buf;

    const uint8_t* pSrcData = reinterpret_cast<const uint8_t*>(&pPdd->time);
    pushBytesToVector( mes, pSrcData, sizeof(Time) );

    //filling VehicleData
    buf.carrier.vehicleData = pPdd->vehicleData;
    reverseBytes(&buf.carrier.vehicleData.vehicleSpeed);
    reverseBytes(&buf.carrier.vehicleData.accumMileage);
    reverseBytes(&buf.carrier.vehicleData.totalVoltage);
    reverseBytes(&buf.carrier.vehicleData.totalCurrent);
    reverseBytes(&buf.carrier.vehicleData.insulationResistance);
    mes.push_back(IM_VEHICLE_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.vehicleData);
    pushBytesToVector( mes, pSrcData, sizeof(VehicleData) );

    //filling ElectricalMachineData
    memcpy((buf.carrier.electricalDataCollection.data),
            (pPdd->electrmMachineData.elecMachineDataArray),
                sizeof(ElectricalMachineDataElement) * 2);
    //buf.carrier.electricalDataCollection.data = pPdd->electrmMachineData.elecMachineDataArray;
    buf.carrier.electricalDataCollection.size = 2;

    for(int i = 0; i < buf.carrier.electricalDataCollection.size; i++)
    {
        reverseBytes(&buf.carrier.electricalDataCollection.data[i].speed);
        reverseBytes(&buf.carrier.electricalDataCollection.data[i].torque);
        reverseBytes(&buf.carrier.electricalDataCollection.data[i].inVoltage);
        reverseBytes(&buf.carrier.electricalDataCollection.data[i].dcCurrent);  
    }
    mes.push_back(IM_EMACHINE_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.electricalDataCollection);
    pushBytesToVector( mes, pSrcData, sizeof(ElectricalMachineDataCollection) );

    //no IM_FUELCELL_DATA

    //filling EngineData
    buf.carrier.engineData = pPdd->engineData;
    reverseBytes(&buf.carrier.engineData.crankshaftSpeed);
    reverseBytes(&buf.carrier.engineData.fuelConsumption);
    mes.push_back(IM_ENGINE_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.engineData);
    pushBytesToVector( mes, pSrcData, sizeof(EngineData) );

    //filling GPSData
    buf.carrier.gpsData = pPdd->gpsData;
    reverseBytes(&buf.carrier.gpsData.longitude);
    reverseBytes(&buf.carrier.gpsData.latitude);
    mes.push_back(IM_GPS_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.gpsData);
    pushBytesToVector( mes, pSrcData, sizeof(GPSData) );

    //filling LimitValueData
    buf.carrier.limitData = pPdd->limitValueData;
    reverseBytes(&buf.carrier.limitData.maxCellVoltage);
    reverseBytes(&buf.carrier.limitData.minCellVoltage);
    mes.push_back(IM_LIMIT_VALUE_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.limitData);
    pushBytesToVector( mes, pSrcData, sizeof(LimitValueData) );

    //filling WarningData
    buf.carrier.warningData = pPdd->warningData;
    //uint32_t warningDataUnitSize = 0;
    reverseBytes(&buf.carrier.warningData.mark.dword);
    mes.push_back(IM_WARNING_DATA);
    mes.push_back(buf.carrier.warningData.level);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.warningData.mark.dword);
    pushBytesToVector( mes, pSrcData, sizeof(buf.carrier.warningData.mark.dword) );

    //pushing DTC part of WarningData
    uint32_t sizeBeforeDtcInfo = mes.size();
    if(NULL == pDtcAr)
    {
        buf.carrier.warningData.ceswd.cesSize = 0;
        buf.carrier.warningData.emfwd.emfSize = 0;
        buf.carrier.warningData.efwd.efSize = 0;
        buf.carrier.warningData.ofwd.ofSize = 0;

        mes.push_back(buf.carrier.warningData.ceswd.cesSize);
        mes.push_back(buf.carrier.warningData.emfwd.emfSize);
        mes.push_back(buf.carrier.warningData.efwd.efSize);
        mes.push_back(buf.carrier.warningData.ofwd.ofSize);
    }
    else
    {
        pushEcuData(mes, PHEV_BECM_ECU_ID, pDtcAr);

        pushEcuData(mes, PHEV_HPCM_ECU_ID, pDtcAr);

        pushEcuData(mes, PHEV_PCM_ECU_ID, pDtcAr);

        //pushing 'other faults warning data' (ofwd): DCDC, ABS
        const int32_t ECU_INDEX_NOT_FOUND = -1;
        int32_t dcdcEcuIndex = ECU_INDEX_NOT_FOUND;
        uint32_t dcdcDtcCount = 0;
        int32_t absEcuIndex  = ECU_INDEX_NOT_FOUND;
        uint32_t absDtcCount = 0;
        for(int32_t i=0; i<XEV_ECU_MAX; i++)
        {
            if(PHEV_DCDC_ECU_ID == pDtcAr[i].t_ecu_id)
            {
                if(dcdcEcuIndex != ECU_INDEX_NOT_FOUND)
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: duplicate DTC info found for ecuId=%d, index=%d, previousIndex", PHEV_DCDC_ECU_ID, i, dcdcEcuIndex);
                    continue;
                }
                dcdcEcuIndex = i;
                dcdcDtcCount = (DIAG_DTC_MAX >= pDtcAr[i].t_dtc_included_count) ? pDtcAr[i].t_dtc_included_count : DIAG_DTC_MAX;
            }
            else if(PHEV_ABS_ECU_ID == pDtcAr[i].t_ecu_id)
            {
                if(absEcuIndex != ECU_INDEX_NOT_FOUND)
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: duplicate DTC info found for ecuId=%d, index=%d, previousIndex", PHEV_DCDC_ECU_ID, i, absEcuIndex);
                    continue;
                }
                absEcuIndex = i;
                absDtcCount = (DIAG_DTC_MAX >= pDtcAr[i].t_dtc_included_count) ? pDtcAr[i].t_dtc_included_count : DIAG_DTC_MAX;
            }
        }
        uint32_t totalOfDtcCount = dcdcDtcCount + absDtcCount;
        if(totalOfDtcCount > WL_CES_SIZE_MAX)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: Error: Total dtcCount of other faults(%d) exceeds maximum(%d). Setting count to anomaly(%d)", totalOfDtcCount, WL_CES_SIZE_MAX, WL_CES_SIZE_ANOMALY);
            totalOfDtcCount = WL_CES_SIZE_ANOMALY;
        }
        mes.push_back(totalOfDtcCount);

        pushEcuData(mes, PHEV_DCDC_ECU_ID, pDtcAr, false);

        pushEcuData(mes, PHEV_ABS_ECU_ID, pDtcAr, false);
    }
    uint32_t dtcInfoBytes = mes.size() - sizeBeforeDtcInfo;
    uint16_t size;
    //filling voltCESSData
    memcpy(reinterpret_cast<void*>(&buf.carrier.voltageCESSCollection),
            reinterpret_cast<const void*>(&pPdd->voltCESSData),
            sizeof(VoltageCESSDataCollection));
    buf.carrier.voltageCESSCollection.size = 1;
    reverseBytes(&buf.carrier.voltageCESSCollection.data.voltage);
    reverseBytes(&buf.carrier.voltageCESSCollection.data.current);
    reverseBytes(&buf.carrier.voltageCESSCollection.data.totalCells);
    reverseBytes(&buf.carrier.voltageCESSCollection.data.snCell);
    mes.push_back(IM_VOLTAGE_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.voltageCESSCollection);
    pushBytesToVector( mes, pSrcData, sizeof(VoltageCESSDataCollection) );
    size = pPdd->voltCESSData.data.frameCells;
    if ((size == VCL_CESS_FRAME_CELLS_ANOMALY)
            || (size == VCL_CESS_FRAME_CELLS_INVALID)) {
        size = 0;
    }
    for( uint32_t i = 0; i < size && i < VCL_CESS_VOLTAGE_CELLS_REAL; i++ )
    {
        uint16_t temp = pPdd->voltCESSData.data.voltageCells[i];
        reverseBytes(&temp);
        pushBytesToVector(mes, reinterpret_cast<uint8_t*>(&temp),
                sizeof(uint16_t));
    }

    //filling temprCESSData
    memcpy(reinterpret_cast<void*>(&buf.carrier.temprCESSCollection),
            reinterpret_cast<const void*>(&pPdd->tempCESSData),
            sizeof(TemprCESSDataCollection));
    buf.carrier.temprCESSCollection.size = 1;
    reverseBytes(&buf.carrier.temprCESSCollection.data.size);
    mes.push_back(IM_TEMPR_DATA);
    pSrcData = reinterpret_cast<uint8_t*>(&buf.carrier.temprCESSCollection);
    pushBytesToVector( mes, pSrcData, sizeof(TemprCESSDataCollection) );

    size = pPdd->tempCESSData.data.size;
    if ((size == TCL_CESS_TEMPR_SIZE_ANOMALY)
            || (size == TCL_CESS_TEMPR_SIZE_INVALID)) {
        size = 0;
    }

    for( uint32_t i = 0; i < size && i < TCL_CESS_SIZE_REAL; i++ )
    {
        mes.push_back(pPdd->tempCESSData.data.data[i]);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Final DataUnit size=%d, dtcInfoBytes=%d", mes.size(), dtcInfoBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::sendSample(const PeriodicDBUSData * pPdd, const DtcDBusData *pDtcAr, bool fSupplementary)
//! \brief function for converting data and sending result to AlertManager
//! \param[in] pPdd pointer to periodic data sample
//! \param[in] pDtcAr pointer to source DTC data array
//! \param[in] fSupplementary flag indicating whether it is real-time data (real-time if false)
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::sendSample(const PeriodicDBUSData * pPdd, const DtcDBusData *pDtcAr, bool fSupplementary)
{
    std::vector<uint8_t> mes;

    convertToDataUnitOfGbtPacket(mes, pPdd, pDtcAr);

    NotifyId notifyId = fSupplementary ? NotifyId::MSG_DP_SUPPLEMENT_DATA : NotifyId::MSG_DP_REALTIME_DATA;
    if(NULL != pDtcAr)
    {
        notifyId = NotifyId::MSG_DP_FAULT_DATA;
    }
    boost::shared_ptr<Message> messageToSend( new Message(notifyId, mes) );

    sendMessage(Mailbox::SERVICE_ALERT_MANAGER, messageToSend);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleSamplingTimeout(const TimerProp& notify)
//! \brief function to handle timeout event of SAMPLES_TMR timer
//! \param[in] notify timer properties struct
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleSamplingTimeout(const TimerProp& notify)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: unexpected call for disabled timer");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::onTimeToSendSample()
//! \brief function implementing logic concerning sending/not sending particular sample data based on settings and current mode
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::onTimeToSendSample()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s mSampleIndex=%d, mSampleIndexExt=%d, mState=%d", __FUNCTION__,mSampleIndex,
            mSampleIndexExt, mState);

    if(DP_STATE_MONITORING == mState)
    {
        bool fSent = false;
        mSampleIndex++;
        uint32_t publishPeriod = Settings::getInstance()->getDataCollectionParam().dataPublishingPeriod;
        if (((mSampleIndex >= publishPeriod && !mSampleIndexExt))
                || (mSampleIndexExt >= publishPeriod))  {
            sendSample(mLastSample.get(), NULL, false);
            fSent = true;
            mSampleIndex = 0;
            mSampleIndexExt = 0;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: sending sample, monitoringMode, dataPublishingPeriod=%d",
                    publishPeriod);
        }

        mSamples.push_back(SSample(mLastSample, fSent));
    }
    else if(DP_STATE_WARNING_MODE == mState)
    {
        mSampleIndex++;

        if(mSampleIndex > Settings::getInstance()->getDataCollectionParam().warningModeLastingPeriod)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: mSampleIndex=%d, end of warningMode, sending last sample", mSampleIndex);
            sendSample(mLastSample.get(), NULL, false);
            mState = DP_STATE_MONITORING;
            sendEvent(boost::make_shared<Event>(Event(NotifyId::EVT_DP_WARNING_MODE_STOP)));
            mSampleIndex=0;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: sending sample in warningMode, mSampleIndex=%d", mSampleIndex);
            sendSample(mLastSample.get(), NULL, false);
        }

        mSamples.push_back(SSample(mLastSample, true));
    }

    while(mSamples.size() > Settings::getInstance()->getDataCollectionParam().previousDataBufferingPeriod)
    {
        mSamples.pop_front();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleEnterWarningMode(const boost::shared_ptr<Notify>& notify)
//! \brief function implementing switching to warning mode on EVT_DBUS_ENTER_WARNING_MODE event from DBusManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleEnterWarningMode(const boost::shared_ptr<Notify>& notify)
{
    sendEvent(boost::make_shared<Event>(Event(NotifyId::EVT_DP_WARNING_MODE_START)));
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s mState=%d, mSampleIndex=%d", __FUNCTION__, mState, mSampleIndex);

    uint32_t sendingCnt = 0;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: mSample queue has %d data before 30s.",mSamples.size());
    
    for(std::list<SSample>::iterator it=mSamples.begin(); it != mSamples.end(); ++it)
    {
        //sending for each fault even if this sample has already been sent //if(false == it->fSent)
        {
            sendSample(it->data.get(), NULL, true);
            it->fSent = true;
            sendingCnt++;
        }
    }
   
    mSampleIndex = 0;
    mState = DP_STATE_WARNING_MODE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleWarningData(const boost::shared_ptr<Notify>& notify)
//! \brief function processing warning data in EVT_DBUS_WARNING_DATA event from DBusManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleWarningData(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"RMS: %s", __FUNCTION__);
    const Payload &data = notify->getPayloadRef();
    if(data.size() != sizeof(WarningDbusData))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: unexpected payload size=%d", data.size());
        return;
    }
    WarningDbusData warnData;
    uint8_t *pDstWarnData = reinterpret_cast<uint8_t *>( &warnData );
    for(uint32_t i=0; i<data.size(); i++)
    {
        pDstWarnData[i] = data[i];
    }
    warnData.periodicData.time = Settings::getInstance()->getChinaTime(
            warnData.periodicData.time);
    sendSample(&warnData.periodicData, warnData.dtc, true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleStartProcessing(const boost::shared_ptr<Notify>& notify)
//! \brief function handling EVT_DBUS_START event from DBusManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleStartProcessing(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"RMS: %s", __FUNCTION__);

    //disabled timer to ensure synchronization of dataSamples with phev
    //TimerManager* tm = TimerManager::getInstance();
    //tm->startTimer( mMBox, *mTimers.find(TimerId::SAMPLES_TMR) );
    mState = DP_STATE_MONITORING;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleSettingsUpdate(const boost::shared_ptr<Notify>& notify)
//! \brief function applying new settings in EVT_DBUS_SETTINGS_UPDATE event from DBusManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleSettingsUpdate(const boost::shared_ptr<Notify>& notify)
{
    xEVConfigurations dcp = Settings::getInstance()->getDataCollectionParam();

    while( mSamples.size() > dcp.previousDataBufferingPeriod )
    {
        mSamples.pop_front();
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: dataPublishingPeriod=%d, previousDataBufferingPeriod=%d, warningModeLastingPeriod=%d"
         , dcp.dataPublishingPeriod
         , dcp.previousDataBufferingPeriod
         , dcp.warningModeLastingPeriod);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handlePeriodicData(const boost::shared_ptr<Notify>& notify)
//! \brief function processing new periodic data in EVT_DBUS_PERODIC_DATA event from DBusManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handlePeriodicData(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: receive rms_transaction data.");
    
    const Payload &data = notify->getPayloadRef();
    if(data.size() != sizeof(PeriodicDBUSData))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: unexpected payload size=%d", data.size());
        return;
    }
    
    mLastSample = boost::make_shared<PeriodicDBUSData>(PeriodicDBUSData());
    uint8_t *pDstPeriodicData = reinterpret_cast<uint8_t *>( mLastSample.get() );
    for(uint32_t i=0; i<data.size(); i++)
    {
        pDstPeriodicData[i] = data[i];
    }
    
    mLastSample->time = Settings::getInstance()->getChinaTime(mLastSample->time);
    mSampleIndexExt = mLastSample->sampleId;
    onTimeToSendSample();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \fn DataProcessor::handleStopProcessing(const boost::shared_ptr<Notify>& notify)
//! \brief function handling EVT_DBUS_STOP event from DBusManager
//! \param[in] notify incoming message
//! \return void
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataProcessor::handleStopProcessing(const boost::shared_ptr<Notify>& notify)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"RMS: %s", __FUNCTION__);
//    TimerManager* tm = TimerManager::getInstance();
//    tm->stopTimer( mMBox, *mTimers.find(TimerId::SAMPLES_TMR) );
    mState = DP_STANDBY;
}


