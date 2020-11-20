/**
 * Copyright (C) 2018-2019 Continental Automotive IC ShangHai
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     remote_config.h
 *  \brief    remote_config.
 *  \author   Zhou You
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_REMOTE_CONFIG_H_
#define VOC_TRANSACTIONS_REMOTE_CONFIG_H_


#include "voc_framework/transactions/smart_transaction.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_mcu_signal.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include <time.h>
 
namespace volvo_on_call
{

class RemoteConfig: public fsm::SmartTransaction    
{
public:

    RemoteConfig(std::shared_ptr<fsm::Signal> signal);
    ~RemoteConfig();
    
    ////////////////////////////////////////////////////////////
    // @brief :init local configuration items data.
    // @return     only return true
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool InitConfigurationAllItems();
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kVDServiceMcu signal.
    // @param[in]  signal to handle.
    // @return     only return true
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool HandleMcuSignal(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :start to stored configuration items.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool HandleStartState(State old_state, State new_state);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle entering the state kDone.Will stop the transaction.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool HandleDoneState(State old_state, State new_state);
    
    ////////////////////////////////////////////////////////////
    // @brief :store configuration items into local db
    // @param[in]  configItems.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool StoreConfigData(std::vector<McuConfiguration_t> configItems);
    
    ////////////////////////////////////////////////////////////
    // @brief : check tsp item\key\type == db item\key\type
    // @param[in]  item.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool CheckItem(McuConfiguration_t item);
    
    ////////////////////////////////////////////////////////////
    // @brief :Send result to TSP.
    // @param[in]  errorCode
    // @param[in]  vehicleErrorCode 
    // @param[in]  message 
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool SendServiceResult(long errorCode, long vehicleErrorCode = 0, std::string message = "");
    
private:
    
    enum States
    {
        kNew = 0,
        kStart,
        kDone
    };

    std::vector<McuConfiguration_t> m_configurationAllItems;
    std::shared_ptr<GlyVdsMcuSignal> m_signal;
};


}


#endif


























