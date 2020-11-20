
/////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file mobuildapp_service.h
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Jianhui Li
// @Init date   17-Jan-2019
////////////////////////////////////////////////////////////////////////////

#ifndef VOC_TRANSACTIONS_MTA_TRANSACTION_H_
#define VOC_TRANSACTIONS_MTA_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "signals/geely/gly_vds_mta_signal.h"

namespace volvo_on_call
{
class MTATransaction: public fsm::SmartTransaction
{
 public:
    ////////////////////////////////////////////////////////////
    // @brief  Creates a "mta" transaction.
    // @author Li Jianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    MTATransaction();

#ifndef UNIT_TESTS
 private:
#endif

    /**
     * \brief Enumerates the states this transaction can be in.
     */
    enum States
    {
        kStart,
        kStartCollectData,
        kCollectData,
        kUpload,
        kStop          ///< Finished.
    };

    ////////////////////////////////////////////////////////////
    // @brief     handle stop MTA service
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleStopMTAService();

    ////////////////////////////////////////////////////////////
    // @brief     handle upload MTA
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleUploadMTA();

    ////////////////////////////////////////////////////////////
    // @brief     handle MTA Time Out
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleTimeOut(std::shared_ptr<fsm::Signal> signal);

     ////////////////////////////////////////////////////////////
    // @brief     handle upload MTA signal
    // @param[in] signal to handle.
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleStartUploadMTA(std::shared_ptr<fsm::Signal> signal);

     ////////////////////////////////////////////////////////////
    // @brief     handle start collect MTA signal
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleStartCollectMTAData();

    ////////////////////////////////////////////////////////////
    // @brief     handle VIN Number
    // @param[in] signal to handle.
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleVINNumber(std::shared_ptr<fsm::Signal> signal);

    ////////////////////////////////////////////////////////////
    // @brief     handle Get Tcam HwVer
    // @param[in] signal to handle.
    // @return    only return true
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool HandleGetTcamHwVer(std::shared_ptr<fsm::Signal> signal);

    ////////////////////////////////////////////////////////////
    // @brief     Request VehicleComm TcamHwVersion
    // @return    return true/false
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool RequesTcamHwVersion();

    ////////////////////////////////////////////////////////////
    // @brief    Request VehicleComm VinNumber
    // @return    return true/false
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool RequestVinNumber();

    ////////////////////////////////////////////////////////////
    // @brief     start timer and retry
    // @return    return true/false
    // @author    Lijianhui, 15-Feb-2019
    ////////////////////////////////////////////////////////////
    bool StartTimerAndRetry();

private:

    std::shared_ptr<GlyVdsMTASignal> m_mtaSignal;
    fsm::MTAData_t m_mtaData;

    const uint32_t m_kTimeout60 = 60;
    const uint32_t m_maxRetryTimes = 3;
    uint32_t m_currentRetryCount;

    fsm::TimeoutTransactionId m_timeoutTransactionId;

    bool m_requestVinResult;
    bool m_requestEcuSerialNumerResult;

    bool m_handleVin;
    bool m_handleEcuSerialNumer;

};

} // namespace volvo_on_call
#endif

/** \}    end of addtogroup */
