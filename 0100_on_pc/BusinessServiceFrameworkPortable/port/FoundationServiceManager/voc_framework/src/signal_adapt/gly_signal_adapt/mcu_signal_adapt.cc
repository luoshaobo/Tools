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

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/mcu_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

McuSignalAdapt::McuSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj)
{}

bool McuSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s\n", __FUNCTION__);
    
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new McuRequest_t();
    McuRequest_t *mcuData = dynamic_cast<McuRequest_t*>(appData);
    if(mcuData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s mcuData == nullptr.\n", __FUNCTION__);
        return false;
    }
    
    mcuData->serviceId = vdsData->body.serviceId;
    mcuData->serviceCommand = vdsData->body.serviceData.serviceCommand;

    for (auto item = vdsData->body.serviceData.configuration.items.begin(); item != vdsData->body.serviceData.configuration.items.end();) 
    {
        for (auto parameter = item->parameters.begin(); parameter != item->parameters.end();) 
        {
            McuConfiguration_t mcuConfig = {"", "", 0, "", 0 , 0, "", 0, 0};
            mcuConfig.item = item->name;
            mcuConfig.key = parameter->key;
            if(parameter->intVal_presence)
            {
                mcuConfig.valueType = INT_VAL;
                mcuConfig.intValue = parameter->intVal;
            }           
            if(parameter->stringVal_presence)
            {
                mcuConfig.valueType = STRING_VAL;
                mcuConfig.strValue = parameter->stringVal;
            }   
            if(parameter->boolVal_presence)
            {
                mcuConfig.valueType = BOOL_VAL;
                mcuConfig.boolValue = parameter->boolVal;
            }
            if(parameter->timestampVal_presence)
            {
                mcuConfig.valueType = TIMESTAMP_VAL;
                mcuConfig.timeSeconds = parameter->timestampVal.seconds;
                if(parameter->timestampVal.milliseconds_presence)
                {
                    mcuConfig.timeMilliseconds = parameter->timestampVal.milliseconds;
                }
            }
            
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "GlyVdsMcuSignal::%s item:%s, key:%s, intvalue:%d, strvalue:%s, valueType:%d.\n", 
                __FUNCTION__, mcuConfig.item.c_str(), mcuConfig.key.c_str(), mcuConfig.intValue, mcuConfig.strValue.c_str(), mcuConfig.valueType);
            
            mcuData->configItems.push_back(mcuConfig);

            ++parameter;
        }
        
        ++item;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s completed.\n", __FUNCTION__);
    
    return true;
}

bool McuSignalAdapt::SetResultPayload(const McuServiceResult_t &response)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;
    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_mcu;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = response.operationSucceeded;
    if(!response.operationSucceeded)
    {
        vdsData.body.serviceData.serviceResult.error_presence = true;
        vdsData.body.serviceData.serviceResult.error.code = (Vds_ErrorCode)response.errorCode;
        
        if(response.vehicleErrorCode > 0)
        {
            vdsData.body.serviceData.serviceResult.error.vehicleErrorCode_presence = true;
            vdsData.body.serviceData.serviceResult.error.vehicleErrorCode = response.vehicleErrorCode;
        }
        
        if(!response.message.empty())
        {
            vdsData.body.serviceData.serviceResult.error.message_presence = true;
            vdsData.body.serviceData.serviceResult.error.message = response.message;
        }
    }
    
    bool result = PackService(vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

