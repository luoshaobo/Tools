///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file mcus.h
//	VOC Service car Mcu request transaction.

// @project		GLY_TCAM
// @subsystem	Application
// @author		Zhou You
// @Init date	1-Aug-2018
///////////////////////////////////////////////////////////////////

#include <time.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <memory>
#include "dlt/dlt.h"
#include "vpom_IPositioningService.hpp"

#include "remote_config/remote_config.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_mcu_signal.h"
#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include "voc_framework/transactions/transaction_id.h"

#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"

#include "local_config_interface.hpp"
#include "remote_config_persist_interface.hpp"



DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{


RemoteConfig::RemoteConfig(std::shared_ptr<fsm::Signal> initial_signal):
    fsm::SmartTransaction(kNew)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s\n", __FUNCTION__);

    SignalFunction handle_mcu_signal =
        std::bind(&RemoteConfig::HandleMcuSignal,
                  this,
                  std::placeholders::_1);

    StateFunction handle_start_state =
        std::bind(&RemoteConfig::HandleStartState,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

    StateFunction handle_done_state =
        std::bind(&RemoteConfig::HandleDoneState,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

    StateMap state_map =
        {{kNew,                      {nullptr,
                                      {{fsm::kVDServiceMcu, handle_mcu_signal}},
                                      {kStart}}},

         {kStart,                    {handle_start_state,
                                      SignalFunctionMap(),
                                      {kDone}}},

         {kDone,                     {handle_done_state,
                                      SignalFunctionMap(),
                                      {}}}};
    
    SetStateMap(state_map);
    
    InitConfigurationAllItems();

    if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }

}
      
RemoteConfig::~RemoteConfig()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s\n", __FUNCTION__);
}

bool RemoteConfig::InitConfigurationAllItems()
{
    m_configurationAllItems = 
    {
        {"pmt", "STANDBY_MAXTIME", INT_VAL, "", 0 , 0, "", 0, 0},
        {"pmt", "SLEEP_POLL_MAXTIME_1", INT_VAL, "", 0 , 0, "", 0, 0},
        {"pmt", "SLEEP_POLL_MAXTIME_2", INT_VAL, "", 0 , 0, "", 0, 0},
        {"pmt", "SLEEP_POLL_INTERVAL_1", INT_VAL, "", 0 , 0, "", 0, 0},
        {"pmt", "SLEEP_POLL_INTERVAL_2", INT_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "ECALL_NUMBER", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "ECALL_NUMBER_B", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "BCALL_NUMBER", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "BCALL_NUMBER_B", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "ICALL_NUMBER", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "ICALL_NUMBER_B", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "OTHER_NUMBERS", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"x-call", "SMS_NUMBER", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "MSG_TIMEOUT", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "MSG_RETRY_NUMBERS", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "MSG_RETRY_DURATION", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "FALLBACK_TIMEOUT", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "TEM_DATA_STORE_MAXTIME", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "MSG_DL_TTL", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "MSG_UL_TTL", INT_VAL, "", 0 , 0, "", 0, 0},
        {"message-delivery", "SERVICE_TIMEOUT_CSP", INT_VAL, "", 0 , 0, "", 0, 0},
        {"status-trigger", "START_DRIVING", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"status-trigger", "IN_DRIVING", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"status-trigger", "ECU_Warning", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"status-trigger", "END_DRIVING", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"status-trigger", "RVS_INTERVAL", INT_VAL, "", 0 , 0, "", 0, 0},
        {"ip-routing", "IP_ROUTING", STRING_VAL, "", 0 , 0, "", 0, 0},
        {"firewall", "FIREWALL", STRING_VAL, "", 0 , 0, "", 0, 0}
    };
    
    return true;
}

bool RemoteConfig::HandleMcuSignal(std::shared_ptr<fsm::Signal> signal)
{
    State state = GetState();
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,current state:%d\n", __FUNCTION__,state);
    
    m_signal = std::dynamic_pointer_cast<GlyVdsMcuSignal>(signal);
    std::shared_ptr<McuRequest_t> request = m_signal->GetRequestPayload();
    SetState(kStart);
    
    return true;
}

bool RemoteConfig::HandleStartState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,current state:%d\n", __FUNCTION__,new_state);
    
    std::shared_ptr<McuRequest_t> request = m_signal->GetRequestPayload();

    if(request->configItems.size() > 0)
    {
        bool retResult = StoreConfigData(request->configItems);
        if(retResult)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s configuration saved ok.\n", __FUNCTION__);
            SendServiceResult(-1);
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s, no configuration items, so send invalidData result.\n",__FUNCTION__);
        SendServiceResult(app_invalidData);
    }
    
    SetState(kDone);

    return true;
}

bool RemoteConfig::HandleDoneState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,current state:%d\n", __FUNCTION__,new_state);
    
    Stop();
    
    return true;
}


bool RemoteConfig::StoreConfigData(std::vector<McuConfiguration_t> configItems)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s\n", __FUNCTION__);
    
    lcfg::ILocalConfigPersistMgr *lcgPerMgr(lcfg::ILocalConfigPersistMgr::GetInstance());

    for (auto it = configItems.begin(); it != configItems.end();) 
 	{
 	    bool retResult = false;
 	    bool checkResult = CheckItem(*it);
        
        if(!checkResult)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,item:%s,key:%s,valuetype:%d checked fail.", 
                __FUNCTION__,it->item.c_str(),it->key.c_str(),it->valueType);
            
            SendServiceResult(app_invalidData);
            return false;
        }

        if(it->valueType == INT_VAL)
        {
            retResult = lcgPerMgr->SetCfgValue(it->item + "_" + it->key, it->intValue);
        }
        else if(it->valueType == STRING_VAL)
        {
            retResult = lcgPerMgr->SetCfgValue(it->item + "_" + it->key, it->strValue);
        }
        else if(it->valueType == BOOL_VAL)
        {
            retResult = lcgPerMgr->SetCfgValue(it->item + "_" + it->key, it->boolValue);
        }
        else if(it->valueType == TIMESTAMP_VAL)
        {
            retResult = lcgPerMgr->SetCfgValue(it->item + "_" + it->key + "s", it->timeSeconds);
            retResult = lcgPerMgr->SetCfgValue(it->item + "_" + it->key + "ms", it->timeMilliseconds);
        }
        else if(it->valueType == DATA_VAL)
        {
            retResult = lcgPerMgr->SetCfgValue(it->item + "_" + it->key, it->dataValue);
        }

        if(!retResult)
        {
            SendServiceResult(app_executionFailure, 9999, "some parameters saved fail.");
            return false;
        }

        ++it;
    }

    return true;

}

bool RemoteConfig::CheckItem(McuConfiguration_t item)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,item:%s,key:%s,valuetype:%d.", 
        __FUNCTION__,item.item.c_str(),item.key.c_str(),item.valueType);

    if(m_configurationAllItems.size() == 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,local configuration items size is zero.", __FUNCTION__);
        return false;
    }
    
    for (auto it = m_configurationAllItems.begin(); it != m_configurationAllItems.end();) 
	{
        if (it->item == item.item
            && it->key == item.key
            && it->valueType == item.valueType)
        {
            return true;
        }
        
        ++it;
    }

    return false;
}

bool RemoteConfig::SendServiceResult(long errorCode, long vehicleErrorCode, std::string message)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s\n", __FUNCTION__);

    McuServiceResult_t serviceResult;
    if(errorCode == -1)
    {
        serviceResult.operationSucceeded = true;
        serviceResult.vehicleErrorCode = 0;
        serviceResult.message = "";
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,operationSucceeded:%d\n", 
            __FUNCTION__,serviceResult.operationSucceeded);
    }
    else
    {
        serviceResult.operationSucceeded = false;
        serviceResult.errorCode = errorCode;
        serviceResult.vehicleErrorCode = vehicleErrorCode;
        serviceResult.message = message;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s,operationSucceeded:%d,errorCode:%ld,vehicleErrorCode:%ld,message:%s\n", 
            __FUNCTION__,serviceResult.operationSucceeded,serviceResult.errorCode,serviceResult.vehicleErrorCode,serviceResult.message.c_str());
    }
    
    m_signal->SetResultPayload(serviceResult);
    
    fsm::VocmoSignalSource vocmo_signal_source;
    bool result = vocmo_signal_source.GeelySendMessage(m_signal, false);
    if (result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteConfig::%s, mcu service result send ok.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RemoteConfig::%s, mcu service result send fail.\n", __FUNCTION__);
    }
    
    return result;
}

}












