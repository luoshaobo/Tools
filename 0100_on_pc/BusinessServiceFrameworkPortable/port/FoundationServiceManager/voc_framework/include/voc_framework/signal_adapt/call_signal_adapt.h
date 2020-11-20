// Copyright (C) 2019 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @project     GLY_TCAM
// @subsystem   FSM
// @author      TanChang
// @Init date   27-Feb-2019

#ifndef CALL_SIGNAL_ADAPT_H
#define CALL_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

class CallSignalAdapt: public SignalAdaptBase
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack call RequestBody.
    // @return     True if successfully set, false otherwise
    // @author     TanChang, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData) override;


public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return
    // @author     TanChang, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    CallSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj) : SignalAdaptBase(vdsObj){};

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  void vdsObj
    // @return
    // @author     TanChang, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    CallSignalAdapt(void * vdsObj);

    ////////////////////////////////////////////////////////////
    // @brief :pack CallServiceResult_t into ASN1C structure.
    // @param[in]  response, CallServiceResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     TanChang, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(const CallServiceResult_t &response);


};

#endif

