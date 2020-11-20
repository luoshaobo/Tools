///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file svt_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef SVT_SIGNAL_ADAPT_H
#define SVT_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

class SvtSignalAdapt: public SignalAdaptBase
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack svt RequestBody.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;

public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    SvtSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj) : SignalAdaptBase(vdsObj){};
    SvtSignalAdapt(void * vdsObj);
    ////////////////////////////////////////////////////////////
    // @brief :pack SvtServiceResult_t into ASN1C structure.
    // @param[in]  response, SvtServiceResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(const SvtServiceResult_t &response);

    ////////////////////////////////////////////////////////////
    // @brief :pack SvtTrackPoint_t into ASN1C structure.
    // @param[in]  response, SvtTrackPoint_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetTrackPointPayload(const SvtTrackPoint_t &response);
};

#endif

