///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_flash_horn.h
//	This file handles the Remote Control Flash and Horn for application.
// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Aug-2018
///////////////////////////////////////////////////////////////////
#ifndef REMOTE_CTRL_FLASH_HORN_H_
#define REMOTE_CTRL_FLASH_HORN_H_

#include "remote_ctrl/remote_ctrl_common.h"
#include "signals/geely/gly_vds_rmc_signal.h"

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/vehicle_comm_signal.h"

#include "asn.h"

namespace volvo_on_call
{

    class RemoteCtrlFlashHorn: public RemoteCtrlCommon
    {
    public:
        /************************************************************/
        // @brief :Constructs
        // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
        // @return
        // @author     Nie Yujin
        /************************************************************/
        RemoteCtrlFlashHorn(std::shared_ptr<fsm::Signal> initial_signal);

    private:
        /************************************************************/
        // @brief :send request to vuc
        // @param[in]  
        // @return     True: success, False:Failed
        // @author     Nie Yujin
        /************************************************************/
        bool SendRequest() override;

        /************************************************************/
        // @brief :send response to TSP
        // @param[in]  
        // @return     True: success, False:Failed
        // @author     Nie Yujin
        /************************************************************/
        bool SendResponse(fsm::RemoteCtrlBasicResult response) override;
        bool SendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status) override;

        /************************************************************/
        // @brief :handle requset result from vuc
        // @param[in]  
        // @return     True: success, False:Failed
        // @author     Nie Yujin
        /************************************************************/
        bool HandleResult(std::shared_ptr<fsm::Signal> signal) override;

    private:
        bool m_bIsActiveService;

        std::chrono::seconds m_timeout;

        fsm::TimeoutTransactionId m_ttidtimeout;
        
    };
} // namespace volvo_on_call

#endif //REMOTE_CTRL_FLASH_HORN_H_

