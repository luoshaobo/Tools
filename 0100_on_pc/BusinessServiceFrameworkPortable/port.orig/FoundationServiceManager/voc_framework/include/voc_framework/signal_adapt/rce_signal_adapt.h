///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file rce_signal_adapt.h

// @project     GLY_TCAM
// @subsystem   FSM
// @author      uis93888
// @Init date   11-Mar-2019
///////////////////////////////////////////////////////////////////
#ifndef RCE_SIGNAL_ADAPT_H_
#define RCE_SIGNAL_ADAPT_H_

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"


//remote climate
const std::string REC_TEMP  = "rce.temp";
const std::string REC_LEVEL = "rce.level";
const std::string REC_HEAT  = "rce.heat";
const std::string REC_VENTI = "rce.ventilation";

/**
 * @breif this class is used for transact Conti class to 
 * Geely ASN data format. and get data from Geely ASN structure.
 * It's used for rce serviceId.
 */
class RceSignalAdapt: public SignalAdaptBase
{
protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack svt RequestBody.
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 05-Mar-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;

public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    RceSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj):SignalAdaptBase(vdsObj){};

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  void vdsObj
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    RceSignalAdapt(void* vdsObj);

    ////////////////////////////////////////////////////////////
    // @brief :pack RMT_ClimateSrtStopResult_t to ASN structure.
    //          change response structure, if  TSP sends start/stop request
    // @param[in]  RMT_ClimateSrtStopResult_t result
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetStrtStopResultPayload(const fsm::RMT_ClimateSrtStopResult_t& result);

    ////////////////////////////////////////////////////////////
    // @brief :pack RMT_CliamteAutoStopResult_t to ASN structure.
    //         Notify TSP when climate has been automatic stopped.(Timeout/Engine_stop)
    // @param[in]  RMT_CliamteAutoStopResult_t result
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetAutoStopResultPayload(const fsm::RMT_CliamteAutoStopResult_t& result);

    ////////////////////////////////////////////////////////////
    // @brief :pack RMT_CliamtizationErrorResult_t to ASN structure.
    // @param[in]  RMT_CliamtizationErrorResult_t result
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetErrorResultPayload(const fsm::RMT_RemoteStartCommErrorResult_t& result);

    ////////////////////////////////////////////////////////////
    // @brief :pack pack RMT_EngineNotify_t to ASN structure. to ASN structure.
    // @param[in]  RMT_EngineNotify_t notify
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    //bool SetNotifyPayload(const RMT_EngineNotify_t& notify);
};

#endif //RCE_SIGNAL_ADAPT_H_