////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file dataProcessor.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   21-Sep-2018
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <rms/sys/service.h>
#include <rms/sys/timerManager.h>
#include <rms/dbus_interface/xEVDBusStuctures.h>

void convertToDataUnitOfGbtPacket(std::vector<uint8_t> &dst, const PeriodicDBUSData * pPdd, const DtcDBusData *pDtcAr);

//! \class DataProcessor
//! \brief Implementation of processing of data received via dbus and forming data unit part of final packet of GBT format
class DataProcessor: public Service
{
    //! \brief not implemented default private constructor to prevent misuse
    DataProcessor();

    DataProcessor(const Mailbox& mbox);
public:
    virtual ~DataProcessor();
    static DataProcessor* getInstance();

protected:

    //! \enum TimerId
    //! \brief IDs of timers 
    typedef enum {
        SAMPLES_TMR,
    } TimerId;

    //! \enum DPConst
    //! \brief set of constants
    typedef enum {
        SAMPLING_PERIOD = 1,
        DEFAULT_SAMPLE_INDEX_TO_SEND = 30,//sending once per this number of samples
    } DPConst;

    //! \enum EDPState
    //! \brief set of states of DataProcessor
    enum EDPState{
        DP_STANDBY,
        DP_STATE_MONITORING,
        DP_STATE_WARNING_MODE,
    };

    void handleStartProcessing(const boost::shared_ptr<Notify>& notify);
    void handlePeriodicData(const boost::shared_ptr<Notify>& notify);
    void handleSettingsUpdate(const boost::shared_ptr<Notify>& notify);
    void handleStopProcessing(const boost::shared_ptr<Notify>& notify);
    void handleEnterWarningMode(const boost::shared_ptr<Notify>& notify);
    void handleWarningData(const boost::shared_ptr<Notify>& notify);
    void onTimeToSendSample();
    void sendSample(const PeriodicDBUSData * pPdd, const DtcDBusData *pDtcAr, bool fSupplementary);


    //--------------------------------------------------------------------------
    // Timers timeout handlers
    //--------------------------------------------------------------------------
    void handleTimers(const boost::shared_ptr<Notify>& notify);
    void handleSamplingTimeout(const TimerProp& notify);

    //--------------------------------------------------------------------------
    // Service
    //--------------------------------------------------------------------------
    virtual void handle(const boost::shared_ptr<Notify>& notify);
    virtual void onStart();
    virtual void onStop();

    //! \var mState
    //! \brief A EDPState value for storing state of DataProcessor
    EDPState mState;

    //! \var mSampleIndex
    //! \brief An index of last sample. Used for counting samples and to provide specified frequency of publishing in normal mode
    uint32_t mSampleIndex;

    //! \var mSampleIndexExt
    //! \brief An index of last sample on phev side. Used for synchronizing samples that are sent to FTCP and to government server
    uint32_t mSampleIndexExt;


    //! \struct SSample
    //! \brief structure for storing periodic data sample
    struct SSample 
    {
        SSample(boost::shared_ptr<PeriodicDBUSData> sample, bool sent) : data(sample), fSent(sent) {}

        //! \var data
        //! \brief A PeriodicDBUSData value with sample data
        boost::shared_ptr<PeriodicDBUSData> data;

        //! \var fSent
        //! \brief flag indicating whether given sample has already been sent to government server for control of duplication when history data is sent
        bool fSent;
    };

    //! \var mLastSample
    //! \brief A PeriodicDBUSData value with last sample data.
    boost::shared_ptr<PeriodicDBUSData> mLastSample;

    //! \var mSamples
    //! \brief list with SSample values. History of periodic data samples
    std::list< SSample > mSamples;

    //! \var mTimers
    //! \brief set of TimerProp values. These are timers used by DataProcessor
    std::set<TimerProp> mTimers;
};
