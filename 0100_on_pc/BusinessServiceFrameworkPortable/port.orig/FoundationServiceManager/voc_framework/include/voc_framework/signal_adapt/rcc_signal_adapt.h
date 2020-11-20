///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    @file rcc_signal_adapt.h
//    geely rcc signal.

// @project        GLY_TCAM
// @subsystem    fsm
// @author        uia93888
// @Init date    20-Mar-2019
///////////////////////////////////////////////////////////////////
#ifndef RCC_SIGNAL_ADAPT_H_
#define RCC_SIGNAL_ADAPT_H_

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

namespace fsm{

class RccSignalAdapt: public SignalAdaptBase
{
protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack svt RequestBody.
    // @return     True if successfully set, false otherwise
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;

public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    RccSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj):SignalAdaptBase(vdsObj){};

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  void vdsObj
    // @return
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    RccSignalAdapt(void* vdsObj);

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  void vdsObj
    // @return
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetStrtStopResult(const fsm::Parking_CliamteStrtStopResult_t& result);

        ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  void vdsObj
    // @return
    // @author     uia93888, 20-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetErrorResult(const fsm::Parking_ErrorResult_t& result);
};

}
#endif //RCC_SIGNAL_ADAPT_H_