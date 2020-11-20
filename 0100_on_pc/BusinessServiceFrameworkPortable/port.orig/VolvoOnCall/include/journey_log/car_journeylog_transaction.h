
/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
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

#ifndef VOC_TRANSACTIONS_CAR_JOURNEYLOG_TRANSACTION_H_
#define VOC_TRANSACTIONS_CAR_JOURNEYLOG_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "vpom_IPositioningService.hpp"
#include <list>

#include "signals/geely/gly_vds_jl_signal.h"
namespace volvo_on_call
{
#define TY_TEST

class CarJourneyLogTransaction: public fsm::SmartTransaction
{
 public:

    /**
     * \brief Creates a "journey log" transaction.
     * \param[in] initial_signal fsm::Signal which caused transaction to be created.
     */
    CarJourneyLogTransaction();

 private:

    /**
     * \brief Enumerates the states this transaction can be in.
     */
    enum States
    {
        kStart = 0,     ///< Start.
        kCollectData,   ///< Collect journey data.
        kStop,          ///< Finished.
    };

    void HandleBeginJLService();
    bool HandleStartJLService(std::shared_ptr<fsm::Signal> signal);
    bool HandleCollectTelmDshbData(std::shared_ptr<fsm::Signal> signal);
    bool HandleCollectPositionData(std::shared_ptr<fsm::Signal> signal);
    bool HandleUploadDataToTSP(std::shared_ptr<fsm::Signal> signal);
    bool HandleEndOfTrip(std::shared_ptr<fsm::Signal> signal);

    bool HandleTimeOut(std::shared_ptr<fsm::Signal> signal);

    void HandleStopJLService();

    void UploadJLDataToTSP();
    void SaveJLDataToQue();
    unsigned int getTripId();

private:

    typedef struct {
        uint32_t tripId;
        uint32_t odoMeter;       // optinal: 0-no data
        uint32_t fuelConsumption;       // optinal: 0-no data
        uint32_t traveledDistance;      // unit: KM
        uint32_t avgSpeed;
        uint32_t electricConsumption;
    } JourneyData_t;

    typedef struct {
        int latitude;
        int longitude;
        int altitude;
        bool posCanBeTrusted;
        bool carLocatorStatUploadEn;
        bool marsCoordinates;
    } PositionData_t;
    std::list<PositionData_t> m_positionList;
    std::list<JourneyData_t> m_journeyDataList;
    JourneyData_t currentData;

    uint32_t tripId;
    TimeStamp_t startTime;
    TimeStamp_t endTime;

#ifdef TY_TEST
    fsm::TimeoutTransactionId m_kTelmRoadTripTestTimerId;
    fsm::TimeoutTransactionId m_kTelmDshbTestTimerId;
#endif
    VpomIPositioningService& vpom_gnss_service_object;
    fsm::TimeoutTransactionId m_kRequestPositionDataId;
    std::shared_ptr<GlyVdsJLSignal> m_currentJLData;
};

} // namespace volvo_on_call
#endif

/** \}    end of addtogroup */
