///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file mcu_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef MCU_SIGNAL_ADAPT_H
#define MCU_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"

class McuSignalAdapt: public SignalAdaptBase
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack mcu RequestBody. 
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData) override;


public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return     
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    McuSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj) : SignalAdaptBase(vdsObj){};
    McuSignalAdapt(void * vdsObj);
    
    ////////////////////////////////////////////////////////////
    // @brief :pack McuServiceResult_t into ASN1C structure.
    // @param[in]  response, McuServiceResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(const McuServiceResult_t &response);
	

};

#endif

