///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file res_signal_adapt.h

// @project     GLY_TCAM
// @subsystem   FSM
// @author      uis93888
// @Init date   05-Mar-2019
///////////////////////////////////////////////////////////////////
#ifndef RES_SIGNAL_ADAPT_H_
#define RES_SIGNAL_ADAPT_H_

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

/**
 * @breif this class is used for transact Conti class to 
 * Geely ASN data format. and get data from Geely ASN structure.
 */
class ResSignalAdapt: public SignalAdaptBase
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
    ResSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj):SignalAdaptBase(vdsObj){};

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  void vdsObj
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    ResSignalAdapt(void* vdsObj);

    ////////////////////////////////////////////////////////////
    // @brief :pack RMT_EngineStartResult_t to ASN structure.
    // @param[in]  RMT_EngineStartResult_t result
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetStartResultPayload(const fsm::RMT_EngineStartResult_t& result);

    ////////////////////////////////////////////////////////////
    // @brief :pack RMT_EngineStopResult_t to ASN structure.
    // @param[in]  RMT_EngineStopResult_t result
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetStopResultPayload(const fsm::RMT_EngineStopResult_t& result);

    ////////////////////////////////////////////////////////////
    // @brief :pack RMT_EngineErrorResult_t to ASN structure.
    // @param[in]  RMT_EngineErrorResult_t result
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetErrorResultPayload(const fsm::RMT_EngineErrorResult_t& result);

    ////////////////////////////////////////////////////////////
    // @brief :pack pack RMT_EngineNotify_t to ASN structure. to ASN structure.
    // @param[in]  RMT_EngineNotify_t notify
    // @return
    // @author     uia93888, 5-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetNotifyPayload(const fsm::RMT_EngineNotify_t& notify);
};

#endif //RES_SIGNAL_ADAPT_H_