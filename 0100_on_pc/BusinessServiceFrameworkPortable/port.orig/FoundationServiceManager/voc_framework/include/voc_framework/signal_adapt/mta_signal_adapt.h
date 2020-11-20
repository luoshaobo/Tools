///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file mta_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Li Jianhui
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef MTA_SIGNAL_ADAPT_H
#define MTA_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

class MtaSignalAdapt: public SignalAdaptBase
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack mta RequestBody. 
    // @return     True if successfully set, false otherwise
    // @author     Li Jianhui, 12-Mar-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;

public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return     
    // @author     Li Jianhui, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    MtaSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj) : SignalAdaptBase(vdsObj){};
    MtaSignalAdapt(void* vdsObj) : SignalAdaptBase(vdsObj) {}; 
    ////////////////////////////////////////////////////////////
    // @brief :pack mta Data into ASN1C structure.
    // @param[in]  data, MTAData_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Li Jianhui, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetMtaPayload(const fsm::MTAData_t &data);
};

#endif

