
#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/rvs_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);
namespace fsm
{
RvsSignalAdapt::RvsSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj)
{
    //lijing-test
    //memset(&m_vdsData, 0, sizeof(m_vdsData));
}

bool RvsSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    
//    bool bRetVal = false;
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "RvsSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new RvsRequest_t();
    RvsRequest_t *rvsData = dynamic_cast<RvsRequest_t*>(appData);
    if(rvsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "RvsSignalAdapt::%s rvsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    if(vdsData->body.serviceId_presence == true)
    {
        rvsData->ServiceId = vdsData->body.serviceId;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, body.serviceId:%d\n", rvsData->ServiceId);
    }

    bool bRetVal = false;
    if(vdsData->body.serviceData_presence == true)
    {
        if(vdsData->body.serviceData.serviceCommand_presence)
        {
            rvsData->ServiceCommandIsNull = false;
            rvsData->ServiceCommand = vdsData->body.serviceData.serviceCommand;
        }
        else
        {
            rvsData->ServiceCommandIsNull = true;
        }

        if(vdsData->body.serviceData.serviceParameters_presence)
        {
            bRetVal = UnpackServiceParameters(rvsData, vdsData);
            if(bRetVal == false)
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                        "RvsSignalAdapt::%s UnpackServiceParameters failed.\n", __FUNCTION__);
            }
        }

        if(vdsData->body.serviceData.eventRule_presence == true)
        {
            bRetVal = UnpackEventRule(rvsData, vdsData);
            if(bRetVal == false)
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                        "RvsSignalAdapt::%s UnpackEventRule failed.\n", __FUNCTION__);
            }
        }
    }    
    else
    {
        rvsData->ServiceCommandIsNull = true;
        rvsData->ServiceParamCount = 0;
        return true;
    }
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s completed.\n", __FUNCTION__);
    
    return true;
}


bool RvsSignalAdapt::UnpackServiceParameters(RvsRequest_t *rvsData,
        std::shared_ptr<VDServiceRequest_Model> vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS=====>%s\n", __FUNCTION__);
    
//    unsigned int keysize = 0;
//    unsigned int datasize = 0;
    unsigned int KeyCount = 0;
    
    KeyCount = vdsData->body.serviceData.serviceParameters.size();
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, KeyCount:%d\n", 
        KeyCount);

    if(KeyCount == 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), RVS, KeyCount error\n",
            __FUNCTION__
            );
        return false;
    }

    rvsData->ServiceParamCount = KeyCount;

    ServiceParameter_Model asnPara;
    for(unsigned int i = 0; i < KeyCount; i++)
    {
        if(i ==0)
        {
            asnPara = vdsData->body.serviceData.serviceParameters[i];
        }
        else
        {
            asnPara = vdsData->body.serviceData.serviceParameters[i];
        }

        //parse key
        rvsData->ServiceParam[i].keysize = asnPara.key.length(); 
        std::memcpy(rvsData->ServiceParam[i].Key, asnPara.key.c_str(), asnPara.key.length());
        rvsData->ServiceParam[i].Key[asnPara.key.length()] = 0;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].keysize: %d, buf:%s\n", 
            i, rvsData->ServiceParam[i].keysize, rvsData->ServiceParam[i].Key);

        //parse IntVal
        if(asnPara.intVal_presence == true)
        {        
            rvsData->ServiceParam[i].IntVal = asnPara.intVal;
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].intVal:%d\n", i, rvsData->ServiceParam[i].IntVal);
        }

        //parse StringVal
        if(asnPara.stringVal_presence == true)
        {
//            datasize = 0;
            rvsData->ServiceParam[i].stringsize = asnPara.stringVal.length();
            std::memcpy(rvsData->ServiceParam[i].StringVal, asnPara.stringVal.c_str(), asnPara.stringVal.length());
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].stringsize:%d, stringVal:%s\n", 
                i, rvsData->ServiceParam[i].stringsize, rvsData->ServiceParam[i].StringVal);
        }

        //parse BoolVal
        if(asnPara.boolVal_presence == true)
        {        
            rvsData->ServiceParam[i].BoolVal = asnPara.boolVal;
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].boolVal:%d\n", i, rvsData->ServiceParam[i].BoolVal);
        }

        //parse timestampVal
        if(asnPara.timestampVal_presence == true)
        { 
            rvsData->ServiceParam[i].TimeSeconds = asnPara.timestampVal.seconds;

            if(asnPara.timestampVal.milliseconds_presence == true)
            {
                rvsData->ServiceParam[i].TimeMilliseconds = asnPara.timestampVal.milliseconds;
            }        

            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].TimeSeconds:%d, TimeMilliseconds:%d\n",
                i, rvsData->ServiceParam[i].TimeSeconds, rvsData->ServiceParam[i].TimeMilliseconds);
        }    

        //parse DataVal
        if(asnPara.dataVal_presence == true)
        {
            rvsData->ServiceParam[i].datasize = asnPara.dataVal.length();
            std::memcpy(rvsData->ServiceParam[i].DataVal, asnPara.dataVal.c_str(), rvsData->ServiceParam[i].datasize);
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].stringsize:%d, stringVal:%s\n", 
                i, rvsData->ServiceParam[i].datasize, rvsData->ServiceParam[i].DataVal);
        }
    }
    return true;
}


bool RvsSignalAdapt::UnpackEventRule(RvsRequest_t *rvsData,
        std::shared_ptr<VDServiceRequest_Model> vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS=====>%s\n", __FUNCTION__);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, eventRule->ruleId:%d\n", 
            vdsData->body.serviceData.eventRule.ruleId);
    rvsData->EventRule.RuleId = vdsData->body.serviceData.eventRule.ruleId;    

    //parse startTime
    if(vdsData->body.serviceData.eventRule.startTime_presence == true)
    {
        rvsData->EventRule.StartTimeSeconds = vdsData->body.serviceData.eventRule.startTime.seconds;
        if(vdsData->body.serviceData.eventRule.startTime.milliseconds_presence)
        {
            rvsData->EventRule.StartTimeMilliseconds = vdsData->body.serviceData.eventRule.startTime.milliseconds;   
        }
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, eventRule->startTime->seconds:%d, milliseconds:%d\n", 
            rvsData->EventRule.StartTimeSeconds, rvsData->EventRule.StartTimeMilliseconds);        
    }

    if(vdsData->body.serviceData.eventRule.endTime_presence == true)
    {
        rvsData->EventRule.EndTimeSeconds = vdsData->body.serviceData.eventRule.endTime.seconds;
        if(vdsData->body.serviceData.eventRule.endTime.milliseconds_presence == true)
        {
            rvsData->EventRule.EndTimeMilliseconds = vdsData->body.serviceData.eventRule.endTime.milliseconds;   
        }
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, eventRule->endTime->seconds:%d, milliseconds:%d\n", 
            rvsData->EventRule.EndTimeSeconds, rvsData->EventRule.EndTimeMilliseconds);
    }

    if(vdsData->body.serviceData.eventRule.interval_presence == true)
    {
        rvsData->EventRule.Interval = vdsData->body.serviceData.eventRule.interval;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, eventRule->interval:%d\n", rvsData->EventRule.Interval);        
    }

    if(vdsData->body.serviceData.eventRule.action_presence == true)
    {
        if(vdsData->body.serviceData.eventRule.action.triggerTimes_presence == true)
        {
            rvsData->EventRule.EventAction.TriggerTimes = vdsData->body.serviceData.eventRule.action.triggerTimes;
        }
        if(vdsData->body.serviceData.eventRule.action.notifyInterval_presence == true)
        {
            rvsData->EventRule.EventAction.NotifyInterval = vdsData->body.serviceData.eventRule.action.notifyInterval;
        }        
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, eventRule->action->triggerTimes:%d\n", rvsData->EventRule.EventAction.TriggerTimes);
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, eventRule->action->notifyInterval:%d\n", rvsData->EventRule.EventAction.NotifyInterval);
    }

    if(vdsData->body.serviceData.eventRule.conditions_presence == true)
    {
        unsigned int KeyCount = 0;
        unsigned int conditionSize = 0;
    
        rvsData->EventRule.RuleConditionCount = vdsData->body.serviceData.eventRule.conditions.size();
        KeyCount = rvsData->EventRule.RuleConditionCount;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, EventRule->RuleConditionCount:%d\n", 
                rvsData->EventRule.RuleConditionCount);

        
        for(unsigned int i = 0; i < KeyCount; i++)
        {
            RuleCondition_Model asnPara;
            if(i ==0)
            {
                asnPara = vdsData->body.serviceData.eventRule.conditions[i];
            }
            else
            {
                asnPara = vdsData->body.serviceData.eventRule.conditions[i];
            }

            //parse condition
            conditionSize = vdsData->body.serviceData.eventRule.conditions[i].condition.length();
            rvsData->EventRule.RuleCondition[i].ConditonSize = conditionSize;
            std::memcpy(rvsData->EventRule.RuleCondition[i].Condition, 
                    vdsData->body.serviceData.eventRule.conditions[i].condition.c_str(), 
                    conditionSize);
            rvsData->EventRule.RuleCondition[i].Condition[conditionSize] = 0;
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS, list.array[%d].ConditonSize: %d, conditionBuf:%s\n", 
            i, rvsData->EventRule.RuleCondition[i].ConditonSize, rvsData->EventRule.RuleCondition[i].Condition);
        }
    }
    return true;  
}

bool RvsSignalAdapt::SetBodyPayload()
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    m_vdsData.body_presence = true;

    m_vdsData.body.serviceId_presence = true;    
    m_vdsData.body.serviceData_presence = true;
    m_vdsData.body.seq_presence = true;
    
    m_vdsData.body.serviceId = (Vds_ServiceId)51;
    m_vdsData.body.seq = 1;

    m_vdsData.body.serviceData.vehicleStatus_presence = true;
    
    return true;
}

bool RvsSignalAdapt:: checkFieldsInAsnScope_long(long inputValue, long minValue, long maxValue)
{
    if((inputValue < minValue)||(inputValue > maxValue))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool RvsSignalAdapt:: checkFieldsInAsnScope_int(int inputValue, int minValue, int maxValue)
{
    if((inputValue < minValue)||(inputValue > maxValue))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool RvsSignalAdapt::SetBsPayload(Rvs_BS_t bs)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }    
    m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence = true;
    
    if(checkFieldsInAsnScope_long(bs.position.latitude, -2147483648, 2147483647))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.latitude_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.latitude = bs.position.latitude;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.position.latitude:%d -> is cross ASN scope(-2147483648, 2147483647), error!!!!", bs.position.latitude);
    }

    if(checkFieldsInAsnScope_long(bs.position.longitude, -2147483648, 2147483647))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.longitude_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.longitude = bs.position.longitude;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.position.longitude:%d -> is cross ASN scope(-2147483648, 2147483647), error!!!!", bs.position.longitude);
    }

    if(checkFieldsInAsnScope_long(bs.position.altitude, -100, 8091))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.altitude_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.altitude = bs.position.altitude;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.position.altitude:%d -> is cross ASN scope(-100, 8091), error!!!!", bs.position.altitude);
    }

    if(checkFieldsInAsnScope_int(bs.position.posCanBeTrusted, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.posCanBeTrusted_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.posCanBeTrusted 
                    = bs.position.posCanBeTrusted;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.position.posCanBeTrusted:%d -> is cross ASN scope(0, 1), error!!!!", bs.position.posCanBeTrusted);
    }

    if(checkFieldsInAsnScope_int(bs.position.carLocatorStatUploadEn, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.carLocatorStatUploadEn_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.carLocatorStatUploadEn = bs.position.carLocatorStatUploadEn;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.position.carLocatorStatUploadEn:%d -> is cross ASN scope(0, 1), error!!!!", bs.position.carLocatorStatUploadEn);
    }

    if(checkFieldsInAsnScope_int(bs.position.marsCoordinates, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.marsCoordinates_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position.marsCoordinates = bs.position.marsCoordinates;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.position.marsCoordinates:%d -> is cross ASN scope(0, 1), error!!!!", bs.position.marsCoordinates);
    }

    m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speed = bs.speed;

    if(checkFieldsInAsnScope_long(bs.speedValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speedValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.speedValidity = bs.speedValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.speedValidity:%d -> is cross ASN scope(0, 1), error!!!!", bs.speedValidity);
    }

    if(checkFieldsInAsnScope_long(bs.direction, 0, 360))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.direction_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.direction = bs.direction;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.direction:%d -> is cross ASN scope(0, 360), error!!!!", bs.direction);
    }

    if(checkFieldsInAsnScope_long(bs.engineStatus, 0, 2))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.engineStatus =(Vds_EngineStatus) bs.engineStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.engineStatus:%d -> is cross ASN scope(0, 2), error!!!!", bs.engineStatus);
    }

    if(checkFieldsInAsnScope_long(bs.keyStatus, 0, 2))
    {
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.keyStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.keyStatus = (Vds_KeyStatus)bs.keyStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.keyStatus:%d -> is cross ASN scope(0, 2), error!!!!", bs.keyStatus);
    }

    if(checkFieldsInAsnScope_long(bs.engineBlockedStatus, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.runningStatus.engineBlockedStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.runningStatus.engineBlockedStatus = (Vds_EngineBlockedStatus)bs.engineBlockedStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetBsPayload, bs.engineBlockedStatus:%d -> is cross ASN scope(0, 1), error!!!!", bs.engineBlockedStatus);
    }
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.distanceToEmpty_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.distanceToEmpty = bs.distanceToEmpty;
    m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.usageMode_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.usageMode = bs.usageMode;    
    return true;
}

bool RvsSignalAdapt::SetTsPayload(Rvs_TS_t ts)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }
    
    m_vdsData.body.serviceData.vehicleStatus.temStatus_presence = true;
    if(checkFieldsInAsnScope_long(ts.powerSource, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.temStatus.powerSource = (Vds_PowerSource)ts.powerSource;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetTsPayload, ts.powerSource:%d -> is cross ASN scope(0, 1), error!!!!, this will set as 0.", ts.powerSource);
        m_vdsData.body.serviceData.vehicleStatus.temStatus.powerSource = (Vds_PowerSource)0;
    }

    if(checkFieldsInAsnScope_long(ts.powerMode, 0, 3))
    {
        m_vdsData.body.serviceData.vehicleStatus.temStatus.powerMode = (Vds_TEMPowerMode) ts.powerMode;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetTsPayload, ts.powerMode:%d -> is cross ASN scope(0, 3), error!!!!, this will set as 0.", ts.powerMode);
        m_vdsData.body.serviceData.vehicleStatus.temStatus.powerMode = (Vds_TEMPowerMode)0;
    }

    m_vdsData.body.serviceData.vehicleStatus.temStatus.sleepCycleNextWakeupTime_presence = true;
    
    if(checkFieldsInAsnScope_long(ts.sleepCycleNextWakeupTime.seconds, 0, 2147483647))
    {
        m_vdsData.body.serviceData.vehicleStatus.temStatus.sleepCycleNextWakeupTime.seconds = ts.sleepCycleNextWakeupTime.seconds;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetTsPayload, ts.sleepCycleNextWakeupTime.seconds:%d -> is cross ASN scope(0, 2147483647), error!!!!this will set as 0.", ts.sleepCycleNextWakeupTime.seconds);
        m_vdsData.body.serviceData.vehicleStatus.temStatus.sleepCycleNextWakeupTime.seconds = 0;
    }

    if(checkFieldsInAsnScope_long(ts.sleepCycleNextWakeupTime.milliseconds, 0, 999))
    {
        m_vdsData.body.serviceData.vehicleStatus.temStatus.sleepCycleNextWakeupTime.milliseconds_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.temStatus.sleepCycleNextWakeupTime.milliseconds = ts.sleepCycleNextWakeupTime.milliseconds;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetTsPayload, ts.sleepCycleNextWakeupTime.milliseconds:%d -> is cross ASN scope(0, 999), error!!!!this will set as 0.", ts.sleepCycleNextWakeupTime.milliseconds);
    }

    if(checkFieldsInAsnScope_long(ts.serviceProvisoned, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.temStatus.serviceProvisoned_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.temStatus.serviceProvisoned = ts.serviceProvisoned;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetTsPayload, ts.serviceProvisoned:%d -> is cross ASN scope(0, 1), error!!!!", ts.serviceProvisoned);
    }

    if(checkFieldsInAsnScope_long(ts.healthStatus, 0, 3))
    {
        m_vdsData.body.serviceData.vehicleStatus.temStatus.healthStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.temStatus.healthStatus = (Vds_HealthStatus)ts.healthStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetTsPayload, ts.healthStatus:%d -> is cross ASN scope(0, 3), error!!!!", ts.healthStatus);
    }
    return true;
}


bool RvsSignalAdapt::SetAsCiPayload(Rvs_AS_CI_t as_ci)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalStatus is null, new additionalStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.confAndId_presence = true;

    //m_vdsResponse->body->serviceData->vehicleStatus->additionalStatus->confAndId->optional.setPresence(asn_ConfigurationAndIdentity__vin);
    //m_vdsResponse->body->serviceData->vehicleStatus->additionalStatus->confAndId->vin = &as_ci.vin;

    if(checkFieldsInAsnScope_long(as_ci.fuelType, 0, 8))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.confAndId.fuelType_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.confAndId.fuelType = (Vds_FuelType)as_ci.fuelType;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCiPayload, as_ci.fuelType:%d -> is cross ASN scope(0, 8), error!!!!", as_ci.fuelType);
    }
    
    
    return true;
}

bool RvsSignalAdapt::SetAsDsPayload(Rvs_AS_DS_t as_ds)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

#if 0
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus = new DrivingSafetyStatus();

    if(checkFieldsInAsnScope_long(as_ds.srsStatus, 0, 2))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.srsStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.srsStatus = as_ds.srsStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, srsStatus:%d -> is cross ASN scope(0-2), error!!!!", as_ds.srsStatus);
    }
    
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.vehicleAlarm_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.vehicleAlarm = as_ds.vehicleAlarm;

    if(checkFieldsInAsnScope_long(as_ds.doorOpenStatusDriver, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusDriver_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusDriver = as_ds.doorOpenStatusDriver;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorOpenStatusDriver:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorOpenStatusDriver);
    }

    if(checkFieldsInAsnScope_long(as_ds.doorOpenStatusPassenger, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusPassenger_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusPassenger = as_ds.doorOpenStatusPassenger;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorOpenStatusPassenger:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorOpenStatusPassenger);
    }
    
    if(checkFieldsInAsnScope_long(as_ds.doorOpenStatusDriverRear, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusDriverRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusDriverRear = as_ds.doorOpenStatusDriverRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorOpenStatusDriverRear:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorOpenStatusDriverRear);
    }

    if(checkFieldsInAsnScope_long(as_ds.doorOpenStatusPassengerRear, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusPassengerRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorOpenStatusPassengerRear = as_ds.doorOpenStatusPassengerRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorOpenStatusPassengerRear:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorOpenStatusPassengerRear);
    }

    if(checkFieldsInAsnScope_long(as_ds.doorLockStatusDriver, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusDriver_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusDriver = as_ds.doorLockStatusDriver;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorLockStatusDriver:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorLockStatusDriver);
    }

    if(checkFieldsInAsnScope_long(as_ds.doorLockStatusPassenger, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusPassenger_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusPassenger= as_ds.doorLockStatusPassenger;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorLockStatusPassenger:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorLockStatusPassenger);
    }

    if(checkFieldsInAsnScope_long(as_ds.doorLockStatusDriverRear, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusDriverRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusDriverRear= as_ds.doorLockStatusDriverRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorLockStatusDriverRear:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorLockStatusDriverRear);
    }

    if(checkFieldsInAsnScope_long(as_ds.doorLockStatusPassengerRear, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusPassengerRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.doorLockStatusPassengerRear= as_ds.doorLockStatusPassengerRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, doorLockStatusPassengerRear:%d -> is cross ASN scope(0-15), error!!!!", as_ds.doorLockStatusPassengerRear);
    }

    if(checkFieldsInAsnScope_long(as_ds.trunkOpenStatus, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.trunkOpenStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.trunkOpenStatus= as_ds.trunkOpenStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, trunkOpenStatus:%d -> is cross ASN scope(0-15), error!!!!", as_ds.trunkOpenStatus);
    }

    if(checkFieldsInAsnScope_long(as_ds.trunkLockStatus, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.trunkLockStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.trunkLockStatus= as_ds.trunkLockStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, trunkLockStatus:%d -> is cross ASN scope(0-1), error!!!!", as_ds.trunkLockStatus);
    }

    if(checkFieldsInAsnScope_long(as_ds.engineHoodOpenStatus, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.engineHoodOpenStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.engineHoodOpenStatus= as_ds.engineHoodOpenStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, engineHoodOpenStatus:%d -> is cross ASN scope(0-15), error!!!!", as_ds.engineHoodOpenStatus);
    }

    if(checkFieldsInAsnScope_long(as_ds.centralLockingStatus, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.centralLockingStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.centralLockingStatus= as_ds.centralLockingStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, centralLockingStatus:%d -> is cross ASN scope(0-15), error!!!!", as_ds.centralLockingStatus);
    }

    if(checkFieldsInAsnScope_int(as_ds.seatBeltStatusDriver, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusDriver_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusDriver= as_ds.seatBeltStatusDriver;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, seatBeltStatusDriver:%d -> is cross ASN scope(0-1), error!!!!", as_ds.seatBeltStatusDriver);
    }
    
    if(checkFieldsInAsnScope_int(as_ds.seatBeltStatusPassenger, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusPassenger_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusPassenger= as_ds.seatBeltStatusPassenger;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, seatBeltStatusPassenger:%d -> is cross ASN scope(0-1), error!!!!", as_ds.seatBeltStatusPassenger);
    }

    if(checkFieldsInAsnScope_int(as_ds.seatBeltStatusDriverRear, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusDriverRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusDriverRear = as_ds.seatBeltStatusDriverRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, seatBeltStatusDriverRear:%d -> is cross ASN scope(0-1), error!!!!", as_ds.seatBeltStatusDriverRear);
    }

    if(checkFieldsInAsnScope_int(as_ds.seatBeltStatusPassengerRear, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusPassengerRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.seatBeltStatusPassengerRear = as_ds.seatBeltStatusPassengerRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, seatBeltStatusPassengerRear:%d -> is cross ASN scope(0-1), error!!!!", as_ds.seatBeltStatusPassengerRear);
    }

    if(checkFieldsInAsnScope_long(as_ds.handBrakeStatus, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.handBrakeStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.handBrakeStatus= as_ds.handBrakeStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, handBrakeStatus:%d -> is cross ASN scope(0-15), error!!!!", as_ds.handBrakeStatus);
    }

    if(checkFieldsInAsnScope_long(as_ds.electricParkBrakeStatus, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.electricParkBrakeStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.electricParkBrakeStatus = as_ds.electricParkBrakeStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, electricParkBrakeStatus:%d -> is cross ASN scope(0-15), error!!!!", as_ds.electricParkBrakeStatus);
    }

    if(checkFieldsInAsnScope_long(as_ds.electricParkBrakeStatusValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.electricParkBrakeStatusValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingSafetyStatus.electricParkBrakeStatusValidity= as_ds.electricParkBrakeStatusValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDsPayload, electricParkBrakeStatusValidity:%d -> is cross ASN scope(0-15), error!!!!", as_ds.electricParkBrakeStatusValidity);
    }
#endif
    return true;
}

bool RvsSignalAdapt::SetAsMsPayload(Rvs_AS_MS_t as_ms)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus_presence = true;

    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.stateOfCharge_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.stateOfCharge = as_ms.mainBatteryStatus.stateOfCharge;    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.chargeLevel_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.chargeLevel = as_ms.mainBatteryStatus.chargeLevel;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.stateOfHealth_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.stateOfHealth = as_ms.mainBatteryStatus.stateOfHealth;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.voltage_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.mainBatteryStatus.voltage = as_ms.mainBatteryStatus.voltage;

    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusDriver_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusDriver = as_ms.tyreStatusDriver;

    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusPassenger_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusPassenger = as_ms.tyreStatusPassenger;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusDriverRear_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusDriverRear = as_ms.tyreStatusDriverRear;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusPassengerRear_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.tyreStatusPassengerRear = as_ms.tyreStatusPassengerRear;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningDriver_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningDriver = as_ms.tyrePreWarningDriver;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningPassenger_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningPassenger = as_ms.tyrePreWarningPassenger;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningDriverRear_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningDriverRear = as_ms.tyrePreWarningDriverRear;
     
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningPassengerRear_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tyrePreWarningPassengerRear = as_ms.tyrePreWarningPassengerRear;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineHrsToService_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineHrsToService = as_ms.engineHrsToService;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.distanceToService_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.distanceToService = as_ms.distanceToService;

    if(checkFieldsInAsnScope_long(as_ms.daysToService, -150, 1890))
    {
        m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.daysToService_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.daysToService = as_ms.daysToService;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsMsPayload, as_ms.daysToService:%d -> is cross ASN scope(-150-1890), error!!!!", as_ms.daysToService);
    }
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.serviceWarningStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.serviceWarningStatus = as_ms.serviceWarningStatus;

    if(checkFieldsInAsnScope_long(as_ms.serviceWarningTrigger, 0, 4))
    {
        m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.serviceWarningTrigger_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.serviceWarningTrigger = (Vds_ServiceWarningTriggerReason)as_ms.serviceWarningTrigger;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsMsPayload, as_ms.serviceWarningTrigger:%d -> is cross ASN scope(0-4), error!!!!", as_ms.serviceWarningTrigger);
    } 
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.odometer_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.odometer = as_ms.odometer;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.brakeFluidLevelStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.brakeFluidLevelStatus = as_ms.brakeFluidLevelStatus;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.washerFluidLevelStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.washerFluidLevelStatus = as_ms.washerFluidLevelStatus;
    return true;
}

bool RvsSignalAdapt::SetAsRsPayload(Rvs_AS_RS_t as_rs)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus_presence = true;

    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilLevelStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilLevelStatus = as_rs.engineOilLevelStatus;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilTemperature_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilTemperature = as_rs.engineOilTemperature;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilPressureWarning_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineOilPressureWarning = as_rs.engineOilPressureWarning;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineCoolantTemperature_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineCoolantTemperature = as_rs.engineCoolantTemperature;

#if 0
    if(checkFieldsInAsnScope_long(as_rs.engineCoolantTemperatureValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineCoolantTemperatureValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineCoolantTemperatureValidity = as_rs.engineCoolantTemperatureValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsRsPayload, as_rs.engineCoolantTemperatureValidity:%d -> is cross ASN scope(0-1), error!!!!", as_rs.engineCoolantTemperatureValidity);
    }
#endif
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineCoolantLevelStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.engineCoolantLevelStatus = as_rs.engineCoolantLevelStatus;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevel_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevel = as_rs.fuelLevel;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevelStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.fuelLevelStatus = as_rs.fuelLevelStatus;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.aveFuelConsumption_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.aveFuelConsumption = as_rs.aveFuelConsumption;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.aveFuelConsumptionInLatestDrivingCycle_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.aveFuelConsumptionInLatestDrivingCycle = as_rs.aveFuelConsumptionInLatestDrivingCycle;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.avgSpeed_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.avgSpeed = as_rs.avgSpeed;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tripMeter1_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tripMeter1= as_rs.tripMeter1;
    
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tripMeter2_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.runningStatus.tripMeter2 = as_rs.tripMeter2;
    
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.bulbStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus.bulbStatus = as_rs.bulbStatus;
    return true;
}    

bool RvsSignalAdapt::SetAsCsPayload(Rvs_AS_CS_t as_cs)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    m_vdsData.body.serviceData.vehicleStatus.climateStatus_presence = true;

    if(checkFieldsInAsnScope_long(as_cs.winStatusDriver, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriver_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriver = as_cs.winStatusDriver;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusDriver:%d -> is cross ASN scope(0-15), error!!!!", as_cs.winStatusDriver);
    }

//winStatusDriverWarning
#if 0
    if(checkFieldsInAsnScope_long(as_cs.winStatusDriverValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriverValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriverValidity = as_cs.winStatusDriverValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusDriverValidity:%d -> is cross ASN scope(0-1), error!!!!", as_cs.winStatusDriverValidity);
    }
#endif
    if(checkFieldsInAsnScope_long(as_cs.winStatusPassenger, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassenger_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassenger = as_cs.winStatusPassenger;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusPassenger:%d -> is cross ASN scope(0-15), error!!!!", as_cs.winStatusPassenger);
    }

    if(checkFieldsInAsnScope_long(as_cs.winStatusPassengerValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassengerWarning_presence = true;
        //winStatusPassengerValidity -> winStatusPassengerWarning
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassengerWarning = as_cs.winStatusPassengerValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusPassengerWarning:%d -> is cross ASN scope(0-1), error!!!!", as_cs.winStatusPassengerValidity);
    }

    if(checkFieldsInAsnScope_long(as_cs.winStatusDriverRear, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriverRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriverRear = as_cs.winStatusDriverRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusDriverRear:%d -> is cross ASN scope(0-15), error!!!!", as_cs.winStatusDriverRear);
    }

    if(checkFieldsInAsnScope_long(as_cs.winStatusDriverRearValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriverRearWarning_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusDriverRearWarning = as_cs.winStatusDriverRearValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusDriverRearValidity:%d -> is cross ASN scope(0-1), error!!!!", as_cs.winStatusDriverRearValidity);
    }

    if(checkFieldsInAsnScope_long(as_cs.winStatusPassengerRear, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassengerRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassengerRear = as_cs.winStatusPassengerRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusPassengerRear:%d -> is cross ASN scope(0-15), error!!!!", as_cs.winStatusPassengerRear);
    }

    if(checkFieldsInAsnScope_long(as_cs.winStatusPassengerRearValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassengerRearWarning_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winStatusPassengerRearWarning = as_cs.winStatusPassengerRearValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winStatusPassengerRearValidity:%d -> is cross ASN scope(0-1), error!!!!", as_cs.winStatusPassengerRearValidity);
    }

    if(checkFieldsInAsnScope_long(as_cs.sunroofOpenStatus, 0, 15))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.sunroofOpenStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.sunroofOpenStatus = as_cs.sunroofOpenStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.sunroofOpenStatus:%d -> is cross ASN scope(0-15), error!!!!", as_cs.sunroofOpenStatus);
    }

    if(checkFieldsInAsnScope_long(as_cs.sunroofOpenStatusValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.sunroofOpenStatusWarning_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.sunroofOpenStatusWarning = as_cs.sunroofOpenStatusValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.sunroofOpenStatusValidity:%d -> is cross ASN scope(0-1), error!!!!", as_cs.sunroofOpenStatusValidity);
    }

    if(checkFieldsInAsnScope_long(as_cs.ventilateStatus, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.ventilateStatus_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.ventilateStatus = as_cs.ventilateStatus;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.ventilateStatus:%d -> is cross ASN scope(0-1), error!!!!", as_cs.ventilateStatus);
    }

    if(checkFieldsInAsnScope_long(as_cs.winPosDriver, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosDriver_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosDriver = as_cs.winPosDriver;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winPosDriver:%d -> is cross ASN scope(0-1000), error!!!!", as_cs.winPosDriver);
    }

    if(checkFieldsInAsnScope_long(as_cs.winPosPassenger, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosPassenger_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosPassenger = as_cs.winPosPassenger;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winPosPassenger:%d -> is cross ASN scope(0-1000), error!!!!", as_cs.winPosPassenger);
    }

    if(checkFieldsInAsnScope_long(as_cs.winPosDriverRear, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosDriverRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosDriverRear = as_cs.winPosDriverRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winPosDriverRear:%d -> is cross ASN scope(0-1000), error!!!!", as_cs.winPosDriverRear);
    }

    if(checkFieldsInAsnScope_long(as_cs.winPosPassengerRear, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosPassengerRear_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winPosPassengerRear = as_cs.winPosPassengerRear;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winPosPassengerRear:%d -> is cross ASN scope(0-1000), error!!!!", as_cs.winPosPassengerRear);
    }
    
    m_vdsData.body.serviceData.vehicleStatus.climateStatus.interiorTemp_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.climateStatus.interiorTemp_presence = as_cs.interiorTemp;
    
    m_vdsData.body.serviceData.vehicleStatus.climateStatus.exteriorTemp_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.climateStatus.exteriorTemp = as_cs.exteriorTemp;

//deleted
#if 0
    if(checkFieldsInAsnScope_long(as_cs.exteriorTempValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.exteriorTempValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.exteriorTempValidity = as_cs.exteriorTempValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.exteriorTempValidity:%d -> is cross ASN scope(0-1), error!!!!", as_cs.exteriorTempValidity);
    }
#endif
    
    m_vdsData.body.serviceData.vehicleStatus.climateStatus.preClimateActive_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.climateStatus.preClimateActive = as_cs.preClimateActive;

    if(checkFieldsInAsnScope_long(as_cs.airCleanSts, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.airCleanSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.airCleanSts = as_cs.airCleanSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.airCleanSts:%d -> is cross ASN scope(0-1), error!!!!", as_cs.airCleanSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.winCloseReminder, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winCloseReminder_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.lockStatus.winCloseReminder = as_cs.winCloseReminder;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.winCloseReminder:%d -> is cross ASN scope(0-7), error!!!!", as_cs.winCloseReminder);
    }

    if(checkFieldsInAsnScope_long(as_cs.drvHeatSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.drvHeatSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.drvHeatSts = as_cs.drvHeatSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.drvHeatSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.drvHeatSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.passHeatingSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.passHeatingSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.passHeatingSts = as_cs.passHeatingSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.passHeatingSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.passHeatingSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.rlHeatingSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rlHeatingSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rlHeatingSts = as_cs.rlHeatingSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.rlHeatingSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.rlHeatingSts);
    }


    if(checkFieldsInAsnScope_long(as_cs.rrHeatingSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rrHeatingSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rrHeatingSts = as_cs.rrHeatingSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.rrHeatingSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.rrHeatingSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.drvVentSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.drvVentSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.drvVentSts = as_cs.drvVentSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.drvVentSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.drvVentSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.passVentSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.passVentSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.passVentSts = as_cs.passVentSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.passVentSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.passVentSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.rrVentSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rrVentSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rrVentSts = as_cs.rrVentSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.rrVentSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.rrVentSts);
    }

    if(checkFieldsInAsnScope_long(as_cs.rlVentSts, 0, 7))
    {
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rlVentSts_presence = true;  
        m_vdsData.body.serviceData.vehicleStatus.climateStatus.rlVentSts = as_cs.rlVentSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsCsPayload, as_cs.rlVentSts:%d -> is cross ASN scope(0-7), error!!!!", as_cs.rlVentSts);
    }
    return true;
}

bool RvsSignalAdapt::SetAsDbPayload(Rvs_AS_DB_t as_db)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsResponse->body->serviceData->vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

#if 0
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus_presence = true;

    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.brakePedalDepressed_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.brakePedalDepressed = as_db.brakePedalDepressed;

    if(checkFieldsInAsnScope_long(as_db.brakePedalDepressedValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.brakePedalDepressedValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.brakePedalDepressedValidity = as_db.brakePedalDepressedValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDbPayload, as_db.brakePedalDepressedValidity:%d -> is cross ASN scope(0-1), error!!!!", as_db.brakePedalDepressedValidity);
    }
    
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.engineSpeed_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.engineSpeed = as_db.engineSpeed;

    if(checkFieldsInAsnScope_long(as_db.engineSpeedValidity, 0, 1))
    {
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.engineSpeedValidity_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.engineSpeedValidity = as_db.engineSpeedValidity;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsDbPayload, as_db.engineSpeedValidity:%d -> is cross ASN scope(0-1), error!!!!", as_db.engineSpeedValidity);
    }
    
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.transimissionGearPostion_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.transimissionGearPostion = as_db.transimissionGearPostion;
    
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.cruiseControlStatus_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus.drivingBehaviourStatus.cruiseControlStatus = as_db.cruiseControlStatus;
#endif
      
    return true;
}

bool RvsSignalAdapt::SetAsEsPayload(Rvs_AS_ES_t as_es)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsData.body.serviceData.vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    //deleted
    m_vdsData.body.serviceData.vehicleStatus.electricStatus_presence = true;

    m_vdsData.body.serviceData.vehicleStatus.electricStatus.isCharging_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.electricStatus.isCharging = as_es.isCharging;
    
    m_vdsData.body.serviceData.vehicleStatus.electricStatus.isPluggedIn_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.electricStatus.isPluggedIn = as_es.isPluggedIn;

    if(checkFieldsInAsnScope_long(as_es.stateOfCharge, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.stateOfCharge_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.stateOfCharge = as_es.stateOfCharge;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.stateOfCharge:%d -> is cross ASN scope(0-1000), error!!!!", as_es.stateOfCharge);
    }

    if(checkFieldsInAsnScope_long(as_es.chargeLevel, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargeLevel_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargeLevel = as_es.chargeLevel;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.chargeLevel:%d -> is cross ASN scope(0-1000), error!!!!", as_es.chargeLevel);
    }

    if(checkFieldsInAsnScope_long(as_es.timeToFullyCharged, 0, 2047))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.timeToFullyCharged_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.timeToFullyCharged = as_es.timeToFullyCharged;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.timeToFullyCharged:%d -> is cross ASN scope(0-2047), error!!!!", as_es.timeToFullyCharged);
    }

    if(checkFieldsInAsnScope_long(as_es.statusOfChargerConnection, 0, 10))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.statusOfChargerConnection_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.statusOfChargerConnection = as_es.statusOfChargerConnection;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.statusOfChargerConnection:%d -> is cross ASN scope(0-10), error!!!!", as_es.statusOfChargerConnection);
    }

    if(checkFieldsInAsnScope_long(as_es.chargerState, 0, 10))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargerState_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargerState = as_es.chargerState;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.chargerState:%d -> is cross ASN scope(0-10), error!!!!", as_es.chargerState);
    }

    if(checkFieldsInAsnScope_long(as_es.distanceToEmptyOnBatteryOnly, 0, 1023))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.distanceToEmptyOnBatteryOnly_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.distanceToEmptyOnBatteryOnly = as_es.distanceToEmptyOnBatteryOnly;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.distanceToEmptyOnBatteryOnly:%d -> is cross ASN scope(0-1023), error!!!!", as_es.distanceToEmptyOnBatteryOnly);
    }

    if(checkFieldsInAsnScope_long(as_es.ems48VSOC, 0, 1000))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.ems48VSOC_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.ems48VSOC = as_es.ems48VSOC;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.ems48VSOC:%d -> is cross ASN scope(0-1000), error!!!!", as_es.ems48VSOC);
    }

    if(checkFieldsInAsnScope_long(as_es.emsHVRBSError, 0, 1))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsHVRBSError_presence = true;
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsHVRBSError = as_es.emsHVRBSError;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.emsHVRBSError:%d -> is cross ASN scope(0-1), error!!!!", as_es.emsHVRBSError);
    }
    
    //m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->electricVehicleStatus.optional.setPresence(asn_emsBMSLBuildSwVersion);
    //m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->electricVehicleStatus->emsBMSLBuildSwVersion = as_es.emsBMSLBuildSwVersion;

    if(checkFieldsInAsnScope_long(as_es.emsRBSModeDisplay, 0, 7))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsRBSModeDisplay = as_es.emsRBSModeDisplay;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.emsRBSModeDisplay:%d -> is cross ASN scope(0-7), error!!!!, this will set as 0.", as_es.emsRBSModeDisplay);
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsRBSModeDisplay = 0;
    }

    if(checkFieldsInAsnScope_long(as_es.emsOVPActive, 0, 1))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsOVPActive = as_es.emsOVPActive;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.emsOVPActive:%d -> is cross ASN scope(0-1), error!!!!, this will set as 0.", as_es.emsOVPActive);
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsOVPActive = 0;
    }

    if(checkFieldsInAsnScope_long(as_es.ems48vPackTemp1, 0, 255))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.ems48vPackTemp1 = as_es.ems48vPackTemp1;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.ems48vPackTemp1:%d -> is cross ASN scope(0-255), error!!!!, this will set as 0.", as_es.ems48vPackTemp1);
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.ems48vPackTemp1 = 0;
    }

    if(checkFieldsInAsnScope_long(as_es.ems48vPackTemp2, 0, 255))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.ems48vPackTemp2 = as_es.ems48vPackTemp2;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.ems48vPackTemp2:%d -> is cross ASN scope(0-255), error!!!!, this will set as 0.", as_es.ems48vPackTemp2);
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.ems48vPackTemp2 = 0;
    }

    if(checkFieldsInAsnScope_long(as_es.emsCBRemainingLife, 0, 100))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsCBRemainingLife = as_es.emsCBRemainingLife;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.emsCBRemainingLife:%d -> is cross ASN scope(0-100), error!!!!, this will set as 0.", as_es.emsCBRemainingLife);
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.emsCBRemainingLife = 0;
    }

    if(checkFieldsInAsnScope_long(as_es.chargeHvSts, 0, 3))
    {
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargeHvSts_presence = true;
         m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargeHvSts = as_es.chargeHvSts;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.chargeHvSts:%d -> is cross ASN scope(0-3), error!!!!", as_es.chargeHvSts);        
    }

    if(checkFieldsInAsnScope_long(as_es.ptReady, 0, 3))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.ptReady_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.ptReady = as_es.ptReady;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.ptReady:%d -> is cross ASN scope(0-3), error!!!!", as_es.ptReady);        
    }

    if(checkFieldsInAsnScope_long(as_es.averPowerConsumption, 0, 5000))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.averPowerConsumption_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.averPowerConsumption = as_es.averPowerConsumption;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.averPowerConsumption:%d -> is cross ASN scope(0-5000), error!!!!", as_es.averPowerConsumption);        
    }

    if(checkFieldsInAsnScope_long(as_es.chargeSts, 0, 3))
    {
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargeSts_presence = true;
        m_vdsData.body.serviceData.vehicleStatus.electricStatus.chargeSts = as_es.chargeSts; 
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsEsPayload, as_es.chargeSts:%d -> is cross ASN scope(0-3), error!!!!", as_es.chargeSts);        
    }

    return true;
}

bool RvsSignalAdapt::SetAsPsPayload(Rvs_AS_PS_t as_ps)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsData.body.serviceData.vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    m_vdsData.body.serviceData.vehicleStatus.pollutionStatus_presence = true;

    if(checkFieldsInAsnScope_long(as_ps.interiorPM25, -1, 2048))
    {
        m_vdsData.body.serviceData.vehicleStatus.pollutionStatus.interiorPM25 = as_ps.interiorPM25;  
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsPsPayload, as_ps.interiorPM25:%d -> is cross ASN scope(-1-2048), error!!!!,this will set as 0.", as_ps.interiorPM25);
        m_vdsData.body.serviceData.vehicleStatus.pollutionStatus.interiorPM25 = 0;
    }

    if(checkFieldsInAsnScope_long(as_ps.exteriorPM25, -1, 2048))
    {
        m_vdsData.body.serviceData.vehicleStatus.pollutionStatus.exteriorPM25 = as_ps.exteriorPM25;  
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::SetAsPsPayload, as_ps.exteriorPM25:%d -> is cross ASN scope(-1-2048), error!!!!,this will set as 0.", as_ps.exteriorPM25);
        m_vdsData.body.serviceData.vehicleStatus.pollutionStatus.exteriorPM25 = 0;
    }
    return true;
}

bool RvsSignalAdapt::SetAsEwPayload(Rvs_AS_EW_t as_ew)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsData.body.serviceData.vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    //m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->optional.setPresence(asn_ecuWarningMessages);
    //m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->ecuWarningMessages->list = as_ew.list
    return true;
}

bool RvsSignalAdapt::SetAsCcPayload(Rvs_AS_CC_t as_cc)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsData.body.serviceData.vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    m_vdsData.body.serviceData.vehicleStatus.carConfig_presence = true;

    m_vdsData.body.serviceData.vehicleStatus.carConfig.heatedFrontSeats_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.carConfig.heatedFrontSeats = as_cc.heatedFrontSeats;
    
    m_vdsData.body.serviceData.vehicleStatus.carConfig.parkingClimateLevel_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.carConfig.parkingClimateLevel = as_cc.parkingClimateLevel;
    
    m_vdsData.body.serviceData.vehicleStatus.carConfig.heatedSteeringWheel_presence = true;
    m_vdsData.body.serviceData.vehicleStatus.carConfig.heatedSteeringWheel = as_cc.heatedSteeringWheel;
    return true;
}

bool RvsSignalAdapt::SetAsSignalsKeyPayload(Rvs_AS_SIGNALS_KEY_t as_signals_key)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    if (m_vdsData.body.serviceData.vehicleStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, m_vdsData.body.serviceData.vehicleStatus is null, return false!!!\n", __FUNCTION__);
        return false;
    }

    if (m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence == false)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RVS::%s, additionalVehicleStatus is null, new AdditionalVehicleStatus()!", __FUNCTION__);
        m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    }

    //m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->optional.setPresence(asn_AdditionalVehicleStatus__signals);
    //m_vdsResponse->body->serviceData->vehicleStatus->additionalVehicleStatus->signals = new CarConfig(); //list
    return true;
}

bool RvsSignalAdapt::SetBatteryStatus(const BatteryStatus_Data &appData, BatteryStatus_Model &vdsData)
{
    bool result = true;

    if (appData.stateOfCharge_presence)
    {
        vdsData.stateOfCharge_presence = true;
        vdsData.stateOfCharge = appData.stateOfCharge;
    }
    if (appData.chargeLevel_presence)
    {
        vdsData.chargeLevel_presence = true;
        vdsData.chargeLevel = appData.chargeLevel;
    }
    if (appData.stateOfHealth_presence)
    {
        vdsData.stateOfHealth_presence = true;
        vdsData.stateOfHealth = appData.stateOfHealth;
    }
    if (appData.powerLevel_presence)
    {
        vdsData.powerLevel_presence = true;
        vdsData.powerLevel = appData.powerLevel;
    }
    if (appData.energyLevel_presence)
    {
        vdsData.energyLevel_presence = true;
        vdsData.energyLevel = appData.energyLevel;
    }
    if (appData.voltage_presence)
    {
        vdsData.voltage_presence = true;
        vdsData.voltage = appData.voltage;
    }

    
    return result;
}

bool RvsSignalAdapt::SetFragStrs(const FragStrs_Data &appData, FragStrs_Model &vdsData)
{
    bool result = true;

    vdsData.activated = appData.activated;
    if (appData.number_presence)
    {
        vdsData.number_presence = true;
        vdsData.number = appData.number;
    }
    if (appData.items_presence)
    {
        vdsData.items_presence = true;
        
        for (auto p_item = appData.items.begin(); p_item != appData.items.end();) 
        {
            FragItem_Model item;
            item.id = p_item->id;
            item.activated = p_item->activated;
            if (p_item->level_presence)
            {
                item.level_presence = true;
                item.level = p_item->level;
            }
            if (p_item->code_presence)
            {
                item.code_presence = true;
                item.code = p_item->code;
            }
            vdsData.items.push_back(item);

        
            ++p_item;
        }
        
    }

    
    return result;
}

bool RvsSignalAdapt::SetPosition(const Position_Data &appData, Position_Model &vdsData)
{
    bool result = true;

    if (appData.latitude_presence)
    {
        vdsData.latitude_presence = true;
        vdsData.latitude = appData.latitude;
    }
    if (appData.longitude_presence)
    {
        vdsData.longitude_presence = true;
        vdsData.longitude = appData.longitude;
    }
    if (appData.altitude_presence)
    {
        vdsData.altitude_presence = true;
        vdsData.altitude = appData.altitude;
    }
    if (appData.posCanBeTrusted_presence)
    {
        vdsData.posCanBeTrusted_presence = true;
        vdsData.posCanBeTrusted = appData.posCanBeTrusted;
    }
    if (appData.carLocatorStatUploadEn_presence)
    {
        vdsData.carLocatorStatUploadEn_presence = true;
        vdsData.carLocatorStatUploadEn = appData.carLocatorStatUploadEn;
    }
    if (appData.marsCoordinates_presence)
    {
        vdsData.marsCoordinates_presence = true;
        vdsData.marsCoordinates = appData.marsCoordinates;
    }

    
    return result;
}

bool RvsSignalAdapt::SetConfigurationAndIdentity(const ConfigurationAndIdentity_Data &appData, ConfigurationAndIdentity_Model &vdsData)
{
    bool result = true;

    if (appData.vin_presence)
    {
        vdsData.vin_presence = true;
        vdsData.vin = appData.vin;
    }
    if (appData.fuelType_presence)
    {
        vdsData.fuelType_presence = true;
        vdsData.fuelType = static_cast<Vds_FuelType>(appData.fuelType);
    }

    
    return result;
}

bool RvsSignalAdapt::SetTimeStamp(const TimeStamp_Data &appData, TimeStamp_Model &vdsData)
{
    bool result = true;

    vdsData.seconds = appData.seconds;
    if (appData.milliseconds_presence)
    {
        vdsData.milliseconds_presence = true;
        vdsData.milliseconds = appData.milliseconds;
    }

    
    return result;
}

bool RvsSignalAdapt::SetTowStatus(const TowStatus_Data &appData, TowStatus_Model &vdsData)
{
    bool result = true;

    if (appData.trailerLampActiveChk_presence)
    {
        vdsData.trailerLampActiveChk_presence = true;
        vdsData.trailerLampActiveChk = appData.trailerLampActiveChk;
    }
    if (appData.trailerLampCheckSts_presence)
    {
        vdsData.trailerLampCheckSts_presence = true;
        vdsData.trailerLampCheckSts = appData.trailerLampCheckSts;
    }
    if (appData.trailerBreakLampSts_presence)
    {
        vdsData.trailerBreakLampSts_presence = true;
        vdsData.trailerBreakLampSts = appData.trailerBreakLampSts;
    }
    if (appData.trailerFogLampSts_presence)
    {
        vdsData.trailerFogLampSts_presence = true;
        vdsData.trailerFogLampSts = appData.trailerFogLampSts;
    }
    if (appData.trailerPosLampSts_presence)
    {
        vdsData.trailerPosLampSts_presence = true;
        vdsData.trailerPosLampSts = appData.trailerPosLampSts;
    }
    if (appData.trailerTurningLampSts_presence)
    {
        vdsData.trailerTurningLampSts_presence = true;
        vdsData.trailerTurningLampSts = appData.trailerTurningLampSts;
    }
    if (appData.trailerReversingLampSts_presence)
    {
        vdsData.trailerReversingLampSts_presence = true;
        vdsData.trailerReversingLampSts = appData.trailerReversingLampSts;
    }

    
    return result;
}

bool RvsSignalAdapt::SetLockStatus(const LockStatus_Data &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);

    bool result = true;

    if (appData.winStatusDriver_presence)
    {
        vdsData.winStatusDriver_presence = true;
        vdsData.winStatusDriver = appData.winStatusDriver;
    }
    if (appData.winStatusPassenger_presence)
    {
        vdsData.winStatusPassenger_presence = true;
        vdsData.winStatusPassenger = appData.winStatusPassenger;
    }
    if (appData.winStatusDriverRear_presence)
    {
        vdsData.winStatusDriverRear_presence = true;
        vdsData.winStatusDriverRear = appData.winStatusDriverRear;
    }
    if (appData.winStatusPassengerRear_presence)
    {
        vdsData.winStatusPassengerRear_presence = true;
        vdsData.winStatusPassengerRear = appData.winStatusPassengerRear;
    }
    if (appData.winStatusDriverWarning_presence)
    {
        vdsData.winStatusDriverWarning_presence = true;
        vdsData.winStatusDriverWarning = appData.winStatusDriverWarning;
    }
    if (appData.winStatusPassengerWarning_presence)
    {
        vdsData.winStatusPassengerWarning_presence = true;
        vdsData.winStatusPassengerWarning = appData.winStatusPassengerWarning;
    }
    if (appData.winStatusDriverRearWarning_presence)
    {
        vdsData.winStatusDriverRearWarning_presence = true;
        vdsData.winStatusDriverRearWarning = appData.winStatusDriverRearWarning;
    }
    if (appData.winStatusPassengerRearWarning_presence)
    {
        vdsData.winStatusPassengerRearWarning_presence = true;
        vdsData.winStatusPassengerRearWarning = appData.winStatusPassengerRearWarning;
    }
    if (appData.winPosDriver_presence)
    {
        vdsData.winPosDriver_presence = true;
        vdsData.winPosDriver = appData.winPosDriver;
    }
    if (appData.winPosPassenger_presence)
    {
        vdsData.winPosPassenger_presence = true;
        vdsData.winPosPassenger = appData.winPosPassenger;
    }
    if (appData.winPosDriverRear_presence)
    {
        vdsData.winPosDriverRear_presence = true;
        vdsData.winPosDriverRear = appData.winPosDriverRear;
    }
    if (appData.winPosPassengerRear_presence)
    {
        vdsData.winPosPassengerRear_presence = true;
        vdsData.winPosPassengerRear = appData.winPosPassengerRear;
    }
    if (appData.sunroofOpenStatus_presence)
    {
        vdsData.sunroofOpenStatus_presence = true;
        vdsData.sunroofOpenStatus = appData.sunroofOpenStatus;
    }
    if (appData.sunroofPos_presence)
    {
        vdsData.sunroofPos_presence = true;
        vdsData.sunroofPos = appData.sunroofPos;
    }
    if (appData.sunroofOpenStatusWarning_presence)
    {
        vdsData.sunroofOpenStatusWarning_presence = true;
        vdsData.sunroofOpenStatusWarning = appData.sunroofOpenStatusWarning;
    }
    if (appData.curtainOpenStatus_presence)
    {
        vdsData.curtainOpenStatus_presence = true;
        vdsData.curtainOpenStatus = appData.curtainOpenStatus;
    }
    if (appData.curtainPos_presence)
    {
        vdsData.curtainPos_presence = true;
        vdsData.curtainPos = appData.curtainPos;
    }
    if (appData.curtainWarning_presence)
    {
        vdsData.curtainWarning_presence = true;
        vdsData.curtainWarning = appData.curtainWarning;
    }
    if (appData.tankFlapStatus_presence)
    {
        vdsData.tankFlapStatus_presence = true;
        vdsData.tankFlapStatus = appData.tankFlapStatus;
    }
    if (appData.chargeLidRearStatus_presence)
    {
        vdsData.chargeLidRearStatus_presence = true;
        vdsData.chargeLidRearStatus = appData.chargeLidRearStatus;
    }
    if (appData.chargeLidFrontStatus_presence)
    {
        vdsData.chargeLidFrontStatus_presence = true;
        vdsData.chargeLidFrontStatus = appData.chargeLidFrontStatus;
    }
    if (appData.ventilateStatus_presence)
    {
        vdsData.ventilateStatus_presence = true;
        vdsData.ventilateStatus = appData.ventilateStatus;
    }
    if (appData.doorOpenStatusDriver_presence)
    {
        vdsData.doorOpenStatusDriver_presence = true;
        vdsData.doorOpenStatusDriver = appData.doorOpenStatusDriver;
    }
    if (appData.doorOpenStatusPassenger_presence)
    {
        vdsData.doorOpenStatusPassenger_presence = true;
        vdsData.doorOpenStatusPassenger = appData.doorOpenStatusPassenger;
    }
    if (appData.doorOpenStatusDriverRear_presence)
    {
        vdsData.doorOpenStatusDriverRear_presence = true;
        vdsData.doorOpenStatusDriverRear = appData.doorOpenStatusDriverRear;
    }
    if (appData.doorOpenStatusPassengerRear_presence)
    {
        vdsData.doorOpenStatusPassengerRear_presence = true;
        vdsData.doorOpenStatusPassengerRear = appData.doorOpenStatusPassengerRear;
    }
    if (appData.doorPosDriver_presence)
    {
        vdsData.doorPosDriver_presence = true;
        vdsData.doorPosDriver = appData.doorPosDriver;
    }
    if (appData.doorPosPassenger_presence)
    {
        vdsData.doorPosPassenger_presence = true;
        vdsData.doorPosPassenger = appData.doorPosPassenger;
    }
    if (appData.doorPosDriverRear_presence)
    {
        vdsData.doorPosDriverRear_presence = true;
        vdsData.doorPosDriverRear = appData.doorPosDriverRear;
    }
    if (appData.doorPosPassengerRear_presence)
    {
        vdsData.doorPosPassengerRear_presence = true;
        vdsData.doorPosPassengerRear = appData.doorPosPassengerRear;
    }
    if (appData.doorLockStatusDriver_presence)
    {
        vdsData.doorLockStatusDriver_presence = true;
        vdsData.doorLockStatusDriver = appData.doorLockStatusDriver;
    }
    if (appData.doorLockStatusPassenger_presence)
    {
        vdsData.doorLockStatusPassenger_presence = true;
        vdsData.doorLockStatusPassenger = appData.doorLockStatusPassenger;
    }
    if (appData.doorLockStatusDriverRear_presence)
    {
        vdsData.doorLockStatusDriverRear_presence = true;
        vdsData.doorLockStatusDriverRear = appData.doorLockStatusDriverRear;
    }
    if (appData.doorLockStatusPassengerRear_presence)
    {
        vdsData.doorLockStatusPassengerRear_presence = true;
        vdsData.doorLockStatusPassengerRear = appData.doorLockStatusPassengerRear;
    }
    if (appData.doorGripStatusDriver_presence)
    {
        vdsData.doorGripStatusDriver_presence = true;
        vdsData.doorGripStatusDriver = appData.doorGripStatusDriver;
    }
    if (appData.doorGripStatusPassenger_presence)
    {
        vdsData.doorGripStatusPassenger_presence = true;
        vdsData.doorGripStatusPassenger = appData.doorGripStatusPassenger;
    }
    if (appData.doorGripStatusDriverRear_presence)
    {
        vdsData.doorGripStatusDriverRear_presence = true;
        vdsData.doorGripStatusDriverRear = appData.doorGripStatusDriverRear;
    }
    if (appData.doorGripStatusPassengerRear_presence)
    {
        vdsData.doorGripStatusPassengerRear_presence = true;
        vdsData.doorGripStatusPassengerRear = appData.doorGripStatusPassengerRear;
    }
    if (appData.trunkOpenStatus_presence)
    {
        vdsData.trunkOpenStatus_presence = true;
        vdsData.trunkOpenStatus = appData.trunkOpenStatus;
    }
    if (appData.trunkLockStatus_presence)
    {
        vdsData.trunkLockStatus_presence = true;
        vdsData.trunkLockStatus = appData.trunkLockStatus;
    }
    if (appData.engineHoodOpenStatus_presence)
    {
        vdsData.engineHoodOpenStatus_presence = true;
        vdsData.engineHoodOpenStatus = appData.engineHoodOpenStatus;
    }
    if (appData.engineHoodLockStatus_presence)
    {
        vdsData.engineHoodLockStatus_presence = true;
        vdsData.engineHoodLockStatus = appData.engineHoodLockStatus;
    }
    if (appData.centralLockingStatus_presence)
    {
        vdsData.centralLockingStatus_presence = true;
        vdsData.centralLockingStatus = appData.centralLockingStatus;
    }
    if (appData.centralLockingDisStatus_presence)
    {
        vdsData.centralLockingDisStatus_presence = true;
        vdsData.centralLockingDisStatus = appData.centralLockingDisStatus;
    }
    if (appData.privateLockStatus_presence)
    {
        vdsData.privateLockStatus_presence = true;
        vdsData.privateLockStatus = appData.privateLockStatus;
    }
    if (appData.vehicleAlarm_presence)
    {
        vdsData.vehicleAlarm_presence = true;
        vdsData.vehicleAlarm = appData.vehicleAlarm;
    }
    if (appData.winCloseReminder_presence)
    {
        vdsData.winCloseReminder_presence = true;
        vdsData.winCloseReminder = appData.winCloseReminder;
    }
    if (appData.handBrakeStatus_presence)
    {
        vdsData.handBrakeStatus_presence = true;
        vdsData.handBrakeStatus = appData.handBrakeStatus;
    }
    if (appData.electricParkBrakeStatus_presence)
    {
        vdsData.electricParkBrakeStatus_presence = true;
        vdsData.electricParkBrakeStatus = appData.electricParkBrakeStatus;
    }

    
    return result;
}

bool RvsSignalAdapt::SetMaintenanceStatus(const MaintenanceStatus_Data &appData, MaintenanceStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    bool result = true;

    if (appData.mainBatteryStatus_presence)
    {
        vdsData.mainBatteryStatus_presence = true;
        SetBatteryStatus(appData.mainBatteryStatus, vdsData.mainBatteryStatus);
    }
    if (appData.tyreStatusDriver_presence)
    {
        vdsData.tyreStatusDriver_presence = true;
        vdsData.tyreStatusDriver = appData.tyreStatusDriver;
    }
    if (appData.tyreStatusPassenger_presence)
    {
        vdsData.tyreStatusPassenger_presence = true;
        vdsData.tyreStatusPassenger = appData.tyreStatusPassenger;
    }
    if (appData.tyreStatusDriverRear_presence)
    {
        vdsData.tyreStatusDriverRear_presence = true;
        vdsData.tyreStatusDriverRear = appData.tyreStatusDriverRear;
    }
    if (appData.tyreStatusPassengerRear_presence)
    {
        vdsData.tyreStatusPassengerRear_presence = true;
        vdsData.tyreStatusPassengerRear = appData.tyreStatusPassengerRear;
    }
    if (appData.tyreTempDriver_presence)
    {
        vdsData.tyreTempDriver_presence = true;
        vdsData.tyreTempDriver = appData.tyreTempDriver;
    }
    if (appData.tyreTempPassenger_presence)
    {
        vdsData.tyreTempPassenger_presence = true;
        vdsData.tyreTempPassenger = appData.tyreTempPassenger;
    }
    if (appData.tyreTempDriverRear_presence)
    {
        vdsData.tyreTempDriverRear_presence = true;
        vdsData.tyreTempDriverRear = appData.tyreTempDriverRear;
    }
    if (appData.tyreTempPassengerRear_presence)
    {
        vdsData.tyreTempPassengerRear_presence = true;
        vdsData.tyreTempPassengerRear = appData.tyreTempPassengerRear;
    }
    if (appData.engineHrsToService_presence)
    {
        vdsData.engineHrsToService_presence = true;
        vdsData.engineHrsToService = appData.engineHrsToService;
    }
    if (appData.distanceToService_presence)
    {
        vdsData.distanceToService_presence = true;
        vdsData.distanceToService = appData.distanceToService;
    }
    if (appData.daysToService_presence)
    {
        vdsData.daysToService_presence = true;
        vdsData.daysToService = appData.daysToService;
    }
    if (appData.serviceWarningStatus_presence)
    {
        vdsData.serviceWarningStatus_presence = true;
        vdsData.serviceWarningStatus = appData.serviceWarningStatus;
    }
    if (appData.serviceWarningTrigger_presence)
    {
        vdsData.serviceWarningTrigger_presence = true;
        vdsData.serviceWarningTrigger = static_cast<Vds_ServiceWarningTriggerReason>(appData.serviceWarningTrigger);
    }
    if (appData.engineOilLevelStatus_presence)
    {
        vdsData.engineOilLevelStatus_presence = true;
        vdsData.engineOilLevelStatus = appData.engineOilLevelStatus;
    }
    if (appData.engineOilTemperature_presence)
    {
        vdsData.engineOilTemperature_presence = true;
        vdsData.engineOilTemperature = appData.engineOilTemperature;
    }
    if (appData.engineOilPressureWarning_presence)
    {
        vdsData.engineOilPressureWarning_presence = true;
        vdsData.engineOilPressureWarning = appData.engineOilPressureWarning;
    }
    if (appData.engineCoolantTemperature_presence)
    {
        vdsData.engineCoolantTemperature_presence = true;
        vdsData.engineCoolantTemperature = appData.engineCoolantTemperature;
    }
    if (appData.engineCoolantLevelStatus_presence)
    {
        vdsData.engineCoolantLevelStatus_presence = true;
        vdsData.engineCoolantLevelStatus = appData.engineCoolantLevelStatus;
    }
    if (appData.brakeFluidLevelStatus_presence)
    {
        vdsData.brakeFluidLevelStatus_presence = true;
        vdsData.brakeFluidLevelStatus = appData.brakeFluidLevelStatus;
    }
    if (appData.washerFluidLevelStatus_presence)
    {
        vdsData.washerFluidLevelStatus_presence = true;
        vdsData.washerFluidLevelStatus = appData.washerFluidLevelStatus;
    }
    if (appData.indcrTurnLeWarning_presence)
    {
        vdsData.indcrTurnLeWarning_presence = true;
        vdsData.indcrTurnLeWarning = appData.indcrTurnLeWarning;
    }
    if (appData.indcrTurnRiWarning_presence)
    {
        vdsData.indcrTurnRiWarning_presence = true;
        vdsData.indcrTurnRiWarning = appData.indcrTurnRiWarning;
    }
    if (appData.indcrTurnOnPassSideWarning_presence)
    {
        vdsData.indcrTurnOnPassSideWarning_presence = true;
        vdsData.indcrTurnOnPassSideWarning = appData.indcrTurnOnPassSideWarning;
    }
    if (appData.indcrTurnOnDrvrSideWarning_presence)
    {
        vdsData.indcrTurnOnDrvrSideWarning_presence = true;
        vdsData.indcrTurnOnDrvrSideWarning = appData.indcrTurnOnDrvrSideWarning;
    }
    if (appData.indcrTurnLeFrntWarning_presence)
    {
        vdsData.indcrTurnLeFrntWarning_presence = true;
        vdsData.indcrTurnLeFrntWarning = appData.indcrTurnLeFrntWarning;
    }
    if (appData.indcrTurnRiFrntWarning_presence)
    {
        vdsData.indcrTurnRiFrntWarning_presence = true;
        vdsData.indcrTurnRiFrntWarning = appData.indcrTurnRiFrntWarning;
    }
    if (appData.fogLiReWarning_presence)
    {
        vdsData.fogLiReWarning_presence = true;
        vdsData.fogLiReWarning = appData.fogLiReWarning;
    }
    if (appData.bulbStatus_presence)
    {
        vdsData.bulbStatus_presence = true;
        vdsData.bulbStatus = appData.bulbStatus;
    }

    
    return result;
}

bool RvsSignalAdapt::SetRunningStatus(const RunningStatus_Data &appData, RunningStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    bool result = true;

    if (appData.srsStatus_presence)
    {
        vdsData.srsStatus_presence = true;
        vdsData.srsStatus = static_cast<Vds_SRSStatus>(appData.srsStatus);
    }
    if (appData.srsCrashStatus_presence)
    {
        vdsData.srsCrashStatus_presence = true;
        vdsData.srsCrashStatus = appData.srsCrashStatus;
    }
    if (appData.odometer_presence)
    {
        vdsData.odometer_presence = true;
        vdsData.odometer = appData.odometer;
    }
    if (appData.fuelLevel_presence)
    {
        vdsData.fuelLevel_presence = true;
        vdsData.fuelLevel = appData.fuelLevel;
    }
    if (appData.fuelLevelStatus_presence)
    {
        vdsData.fuelLevelStatus_presence = true;
        vdsData.fuelLevelStatus = appData.fuelLevelStatus;
    }
    if (appData.fuelEnLevel_presence)
    {
        vdsData.fuelEnLevel_presence = true;
        vdsData.fuelEnLevel = appData.fuelEnLevel;
    }
    if (appData.fuelEnCnsFild_presence)
    {
        vdsData.fuelEnCnsFild_presence = true;
        vdsData.fuelEnCnsFild = appData.fuelEnCnsFild;
    }
    if (appData.fuelEnCns_presence)
    {
        vdsData.fuelEnCns_presence = true;
        vdsData.fuelEnCns = appData.fuelEnCns;
    }
    if (appData.fuelLow1WarningDriver_presence)
    {
        vdsData.fuelLow1WarningDriver_presence = true;
        vdsData.fuelLow1WarningDriver = appData.fuelLow1WarningDriver;
    }
    if (appData.fuelLow2WarningDriver_presence)
    {
        vdsData.fuelLow2WarningDriver_presence = true;
        vdsData.fuelLow2WarningDriver = appData.fuelLow2WarningDriver;
    }
    if (appData.distanceToEmpty_presence)
    {
        vdsData.distanceToEmpty_presence = true;
        vdsData.distanceToEmpty = appData.distanceToEmpty;
    }
    if (appData.trvlDistance_presence)
    {
        vdsData.trvlDistance_presence = true;
        vdsData.trvlDistance = appData.trvlDistance;
    }
    if (appData.aveFuelConsumption_presence)
    {
        vdsData.aveFuelConsumption_presence = true;
        vdsData.aveFuelConsumption = appData.aveFuelConsumption;
    }
    if (appData.aveFuelConsumptionInLatestDrivingCycle_presence)
    {
        vdsData.aveFuelConsumptionInLatestDrivingCycle_presence = true;
        vdsData.aveFuelConsumptionInLatestDrivingCycle = appData.aveFuelConsumptionInLatestDrivingCycle;
    }
    if (appData.avgSpeed_presence)
    {
        vdsData.avgSpeed_presence = true;
        vdsData.avgSpeed = appData.avgSpeed;
    }
    if (appData.avgSpeedLatestDrivingCycle_presence)
    {
        vdsData.avgSpeedLatestDrivingCycle_presence = true;
        vdsData.avgSpeedLatestDrivingCycle = appData.avgSpeedLatestDrivingCycle;
    }
    if (appData.aveFuelConsumptionUnit_presence)
    {
        vdsData.aveFuelConsumptionUnit_presence = true;
        vdsData.aveFuelConsumptionUnit = appData.aveFuelConsumptionUnit;
    }
    if (appData.indFuelConsumption_presence)
    {
        vdsData.indFuelConsumption_presence = true;
        vdsData.indFuelConsumption = appData.indFuelConsumption;
    }
    if (appData.notifForEmgyCallStatus_presence)
    {
        vdsData.notifForEmgyCallStatus_presence = true;
        vdsData.notifForEmgyCallStatus = appData.notifForEmgyCallStatus;
    }
    if (appData.tyrePreWarningDriver_presence)
    {
        vdsData.tyrePreWarningDriver_presence = true;
        vdsData.tyrePreWarningDriver = appData.tyrePreWarningDriver;
    }
    if (appData.tyrePreWarningPassenger_presence)
    {
        vdsData.tyrePreWarningPassenger_presence = true;
        vdsData.tyrePreWarningPassenger = appData.tyrePreWarningPassenger;
    }
    if (appData.tyrePreWarningDriverRear_presence)
    {
        vdsData.tyrePreWarningDriverRear_presence = true;
        vdsData.tyrePreWarningDriverRear = appData.tyrePreWarningDriverRear;
    }
    if (appData.tyrePreWarningPassengerRear_presence)
    {
        vdsData.tyrePreWarningPassengerRear_presence = true;
        vdsData.tyrePreWarningPassengerRear = appData.tyrePreWarningPassengerRear;
    }
    if (appData.tyreTempWarningDriver_presence)
    {
        vdsData.tyreTempWarningDriver_presence = true;
        vdsData.tyreTempWarningDriver = appData.tyreTempWarningDriver;
    }
    if (appData.tyreTempWarningPassenger_presence)
    {
        vdsData.tyreTempWarningPassenger_presence = true;
        vdsData.tyreTempWarningPassenger = appData.tyreTempWarningPassenger;
    }
    if (appData.tyreTempWarningDriverRear_presence)
    {
        vdsData.tyreTempWarningDriverRear_presence = true;
        vdsData.tyreTempWarningDriverRear = appData.tyreTempWarningDriverRear;
    }
    if (appData.tyreTempWarningPassengerRear_presence)
    {
        vdsData.tyreTempWarningPassengerRear_presence = true;
        vdsData.tyreTempWarningPassengerRear = appData.tyreTempWarningPassengerRear;
    }
    if (appData.seatBeltStatusDriver_presence)
    {
        vdsData.seatBeltStatusDriver_presence = true;
        vdsData.seatBeltStatusDriver = appData.seatBeltStatusDriver;
    }
    if (appData.seatBeltStatusPassenger_presence)
    {
        vdsData.seatBeltStatusPassenger_presence = true;
        vdsData.seatBeltStatusPassenger = appData.seatBeltStatusPassenger;
    }
    if (appData.seatBeltStatusDriverRear_presence)
    {
        vdsData.seatBeltStatusDriverRear_presence = true;
        vdsData.seatBeltStatusDriverRear = appData.seatBeltStatusDriverRear;
    }
    if (appData.seatBeltStatusPassengerRear_presence)
    {
        vdsData.seatBeltStatusPassengerRear_presence = true;
        vdsData.seatBeltStatusPassengerRear = appData.seatBeltStatusPassengerRear;
    }
    if (appData.seatBeltStatusMidRear_presence)
    {
        vdsData.seatBeltStatusMidRear_presence = true;
        vdsData.seatBeltStatusMidRear = appData.seatBeltStatusMidRear;
    }
    if (appData.seatBeltStatusThDriverRear_presence)
    {
        vdsData.seatBeltStatusThDriverRear_presence = true;
        vdsData.seatBeltStatusThDriverRear = appData.seatBeltStatusThDriverRear;
    }
    if (appData.seatBeltStatusThPassengerRear_presence)
    {
        vdsData.seatBeltStatusThPassengerRear_presence = true;
        vdsData.seatBeltStatusThPassengerRear = appData.seatBeltStatusThPassengerRear;
    }
    if (appData.seatBeltStatusThMidRear_presence)
    {
        vdsData.seatBeltStatusThMidRear_presence = true;
        vdsData.seatBeltStatusThMidRear = appData.seatBeltStatusThMidRear;
    }
    if (appData.brakePedalDepressed_presence)
    {
        vdsData.brakePedalDepressed_presence = true;
        vdsData.brakePedalDepressed = appData.brakePedalDepressed;
    }
    if (appData.gearManualStatus_presence)
    {
        vdsData.gearManualStatus_presence = true;
        vdsData.gearManualStatus = appData.gearManualStatus;
    }
    if (appData.gearAutoStatus_presence)
    {
        vdsData.gearAutoStatus_presence = true;
        vdsData.gearAutoStatus = appData.gearAutoStatus;
    }
    if (appData.engineSpeed_presence)
    {
        vdsData.engineSpeed_presence = true;
        vdsData.engineSpeed = appData.engineSpeed;
    }
    if (appData.transimissionGearPostion_presence)
    {
        vdsData.transimissionGearPostion_presence = true;
        vdsData.transimissionGearPostion = appData.transimissionGearPostion;
    }
    if (appData.cruiseControlStatus_presence)
    {
        vdsData.cruiseControlStatus_presence = true;
        vdsData.cruiseControlStatus = appData.cruiseControlStatus;
    }
    if (appData.engineBlockedStatus_presence)
    {
        vdsData.engineBlockedStatus_presence = true;
        vdsData.engineBlockedStatus = static_cast<Vds_EngineBlockedStatus>(appData.engineBlockedStatus);
    }
    if (appData.tripMeter1_presence)
    {
        vdsData.tripMeter1_presence = true;
        vdsData.tripMeter1 = appData.tripMeter1;
    }
    if (appData.tripMeter2_presence)
    {
        vdsData.tripMeter2_presence = true;
        vdsData.tripMeter2 = appData.tripMeter2;
    }

    
    return result;
}

bool RvsSignalAdapt::SetClimateStatus(const ClimateStatus_Data &appData, ClimateStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    bool result = true;

    if (appData.interiorTemp_presence)
    {
        vdsData.interiorTemp_presence = true;
        vdsData.interiorTemp = appData.interiorTemp;
    }
    if (appData.exteriorTemp_presence)
    {
        vdsData.exteriorTemp_presence = true;
        vdsData.exteriorTemp = appData.exteriorTemp;
    }
    if (appData.preClimateActive_presence)
    {
        vdsData.preClimateActive_presence = true;
        vdsData.preClimateActive = appData.preClimateActive;
    }
    if (appData.airCleanSts_presence)
    {
        vdsData.airCleanSts_presence = true;
        vdsData.airCleanSts = appData.airCleanSts;
    }
    if (appData.drvHeatSts_presence)
    {
        vdsData.drvHeatSts_presence = true;
        vdsData.drvHeatSts = appData.drvHeatSts;
    }
    if (appData.passHeatingSts_presence)
    {
        vdsData.passHeatingSts_presence = true;
        vdsData.passHeatingSts = appData.passHeatingSts;
    }
    if (appData.rlHeatingSts_presence)
    {
        vdsData.rlHeatingSts_presence = true;
        vdsData.rlHeatingSts = appData.rlHeatingSts;
    }
    if (appData.rrHeatingSts_presence)
    {
        vdsData.rrHeatingSts_presence = true;
        vdsData.rrHeatingSts = appData.rrHeatingSts;
    }
    if (appData.drvVentSts_presence)
    {
        vdsData.drvVentSts_presence = true;
        vdsData.drvVentSts = appData.drvVentSts;
    }
    if (appData.passVentSts_presence)
    {
        vdsData.passVentSts_presence = true;
        vdsData.passVentSts = appData.passVentSts;
    }
    if (appData.rrVentSts_presence)
    {
        vdsData.rrVentSts_presence = true;
        vdsData.rrVentSts = appData.rrVentSts;
    }
    if (appData.rlVentSts_presence)
    {
        vdsData.rlVentSts_presence = true;
        vdsData.rlVentSts = appData.rlVentSts;
    }
    if (appData.interCO2Warning_presence)
    {
        vdsData.interCO2Warning_presence = true;
        vdsData.interCO2Warning = appData.interCO2Warning;
    }
    if (appData.fragStrs_presence)
    {
        vdsData.fragStrs_presence = true;
        SetFragStrs(appData.fragStrs, vdsData.fragStrs);
    }

    
    return result;
}

bool RvsSignalAdapt::SetPollutionStatus(const PollutionStatus_Data &appData, PollutionStatus_Model &vdsData)
{
    bool result = true;

    if (appData.interiorPM25_presence)
    {
        vdsData.interiorPM25_presence = true;
        vdsData.interiorPM25 = appData.interiorPM25;
    }
    if (appData.exteriorPM25_presence)
    {
        vdsData.exteriorPM25_presence = true;
        vdsData.exteriorPM25 = appData.exteriorPM25;
    }
    if (appData.interiorPM25Level_presence)
    {
        vdsData.interiorPM25Level_presence = true;
        vdsData.interiorPM25Level = appData.interiorPM25Level;
    }
    if (appData.exteriorPM25Level_presence)
    {
        vdsData.exteriorPM25Level_presence = true;
        vdsData.exteriorPM25Level = appData.exteriorPM25Level;
    }
    if (appData.airQualityIndex_presence)
    {
        vdsData.airQualityIndex_presence = true;
        vdsData.airQualityIndex = appData.airQualityIndex;
    }
    if (appData.airParticleConcentration_presence)
    {
        vdsData.airParticleConcentration_presence = true;
        vdsData.airParticleConcentration = appData.airParticleConcentration;
    }

    
    return result;
}

bool RvsSignalAdapt::SetElectricStatus(const ElectricStatus_Data &appData, ElectricStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    bool result = true;

    if (appData.isCharging_presence)
    {
        vdsData.isCharging_presence = true;
        vdsData.isCharging = appData.isCharging;
    }
    if (appData.isPluggedIn_presence)
    {
        vdsData.isPluggedIn_presence = true;
        vdsData.isPluggedIn = appData.isPluggedIn;
    }
    if (appData.stateOfCharge_presence)
    {
        vdsData.stateOfCharge_presence = true;
        vdsData.stateOfCharge = appData.stateOfCharge;
    }
    if (appData.chargeLevel_presence)
    {
        vdsData.chargeLevel_presence = true;
        vdsData.chargeLevel = appData.chargeLevel;
    }
    if (appData.timeToFullyCharged_presence)
    {
        vdsData.timeToFullyCharged_presence = true;
        vdsData.timeToFullyCharged = appData.timeToFullyCharged;
    }
    if (appData.statusOfChargerConnection_presence)
    {
        vdsData.statusOfChargerConnection_presence = true;
        vdsData.statusOfChargerConnection = appData.statusOfChargerConnection;
    }
    if (appData.chargerState_presence)
    {
        vdsData.chargerState_presence = true;
        vdsData.chargerState = appData.chargerState;
    }
    if (appData.distanceToEmptyOnBatteryOnly_presence)
    {
        vdsData.distanceToEmptyOnBatteryOnly_presence = true;
        vdsData.distanceToEmptyOnBatteryOnly = appData.distanceToEmptyOnBatteryOnly;
    }
    if (appData.ems48VSOC_presence)
    {
        vdsData.ems48VSOC_presence = true;
        vdsData.ems48VSOC = appData.ems48VSOC;
    }
    if (appData.ems48VDisSOC_presence)
    {
        vdsData.ems48VDisSOC_presence = true;
        vdsData.ems48VDisSOC = appData.ems48VDisSOC;
    }
    if (appData.emsHVRBSError_presence)
    {
        vdsData.emsHVRBSError_presence = true;
        vdsData.emsHVRBSError = appData.emsHVRBSError;
    }
    if (appData.emsRBSModeDisplay_presence)
    {
        vdsData.emsRBSModeDisplay_presence = true;
        vdsData.emsRBSModeDisplay = appData.emsRBSModeDisplay;
    }
    if (appData.emsOVPActive_presence)
    {
        vdsData.emsOVPActive_presence = true;
        vdsData.emsOVPActive = appData.emsOVPActive;
    }
    if (appData.ems48vPackTemp1_presence)
    {
        vdsData.ems48vPackTemp1_presence = true;
        vdsData.ems48vPackTemp1 = appData.ems48vPackTemp1;
    }
    if (appData.ems48vPackTemp2_presence)
    {
        vdsData.ems48vPackTemp2_presence = true;
        vdsData.ems48vPackTemp2 = appData.ems48vPackTemp2;
    }
    if (appData.emsBMSLBuildSwVersion_presence)
    {
        vdsData.emsBMSLBuildSwVersion_presence = true;
        vdsData.emsBMSLBuildSwVersion = appData.emsBMSLBuildSwVersion;
    }
    vdsData.emsCBRemainingLife = appData.emsCBRemainingLife;
    if (appData.chargeHvSts_presence)
    {
        vdsData.chargeHvSts_presence = true;
        vdsData.chargeHvSts = appData.chargeHvSts;
    }
    if (appData.ptReady_presence)
    {
        vdsData.ptReady_presence = true;
        vdsData.ptReady = appData.ptReady;
    }
    if (appData.averPowerConsumption_presence)
    {
        vdsData.averPowerConsumption_presence = true;
        vdsData.averPowerConsumption = appData.averPowerConsumption;
    }
    if (appData.indPowerConsumption_presence)
    {
        vdsData.indPowerConsumption_presence = true;
        vdsData.indPowerConsumption = appData.indPowerConsumption;
    }
    if (appData.dcDcActvd_presence)
    {
        vdsData.dcDcActvd_presence = true;
        vdsData.dcDcActvd = appData.dcDcActvd;
    }
    if (appData.dcDcConnectStatus_presence)
    {
        vdsData.dcDcConnectStatus_presence = true;
        vdsData.dcDcConnectStatus = appData.dcDcConnectStatus;
    }
    if (appData.dcChargeIAct_presence)
    {
        vdsData.dcChargeIAct_presence = true;
        vdsData.dcChargeIAct = appData.dcChargeIAct;
    }
    if (appData.dcChargeSts_presence)
    {
        vdsData.dcChargeSts_presence = true;
        vdsData.dcChargeSts = appData.dcChargeSts;
    }
    if (appData.wptObjt_presence)
    {
        vdsData.wptObjt_presence = true;
        vdsData.wptObjt = appData.wptObjt;
    }
    if (appData.wptFineAlignt_presence)
    {
        vdsData.wptFineAlignt_presence = true;
        vdsData.wptFineAlignt = appData.wptFineAlignt;
    }
    if (appData.wptActived_presence)
    {
        vdsData.wptActived_presence = true;
        vdsData.wptActived = appData.wptActived;
    }
    if (appData.wptChargeIAct_presence)
    {
        vdsData.wptChargeIAct_presence = true;
        vdsData.wptChargeIAct = appData.wptChargeIAct;
    }
    if (appData.wptChargeUAct_presence)
    {
        vdsData.wptChargeUAct_presence = true;
        vdsData.wptChargeUAct = appData.wptChargeUAct;
    }
    if (appData.wptChargeSts_presence)
    {
        vdsData.wptChargeSts_presence = true;
        vdsData.wptChargeSts = appData.wptChargeSts;
    }
    if (appData.chargeIAct_presence)
    {
        vdsData.chargeIAct_presence = true;
        vdsData.chargeIAct = appData.chargeIAct;
    }
    if (appData.chargeUAct_presence)
    {
        vdsData.chargeUAct_presence = true;
        vdsData.chargeUAct = appData.chargeUAct;
    }
    if (appData.chargeSts_presence)
    {
        vdsData.chargeSts_presence = true;
        vdsData.chargeSts = appData.chargeSts;
    }
    if (appData.disChargeConnectStatus_presence)
    {
        vdsData.disChargeConnectStatus_presence = true;
        vdsData.disChargeConnectStatus = appData.disChargeConnectStatus;
    }
    if (appData.disChargeIAct_presence)
    {
        vdsData.disChargeIAct_presence = true;
        vdsData.disChargeIAct = appData.disChargeIAct;
    }
    if (appData.disChargeUAct_presence)
    {
        vdsData.disChargeUAct_presence = true;
        vdsData.disChargeUAct = appData.disChargeUAct;
    }
    if (appData.disChargeSts_presence)
    {
        vdsData.disChargeSts_presence = true;
        vdsData.disChargeSts = appData.disChargeSts;
    }

    
    return result;
}

bool RvsSignalAdapt::SetBasicVehicleStatus(const BasicVehicleStatus_Data &appData, BasicVehicleStatus_Model &vdsData)
{
    bool result = true;

    if (appData.vin_presence)
    {
        vdsData.vin_presence = true;
        vdsData.vin = appData.vin;
    }
    SetPosition(appData.position, vdsData.position);
    vdsData.speed = appData.speed;
    if (appData.speedUnit_presence)
    {
        vdsData.speedUnit_presence = true;
        vdsData.speedUnit = appData.speedUnit;
    }
    if (appData.speedValidity_presence)
    {
        vdsData.speedValidity_presence = true;
        vdsData.speedValidity = appData.speedValidity;
    }
    if (appData.direction_presence)
    {
        vdsData.direction_presence = true;
        vdsData.direction = appData.direction;
    }
    if (appData.engineStatus_presence)
    {
        vdsData.engineStatus_presence = true;
        vdsData.engineStatus = static_cast<Vds_EngineStatus>(appData.engineStatus);
    }
    if (appData.keyStatus_presence)
    {
        vdsData.keyStatus_presence = true;
        vdsData.keyStatus = static_cast<Vds_KeyStatus>(appData.keyStatus);
    }
    if (appData.usageMode_presence)
    {
        vdsData.usageMode_presence = true;
        vdsData.usageMode = appData.usageMode;
    }

    
    return result;
}

bool RvsSignalAdapt::SetAdditionalStatus(const AdditionalStatus_Data &appData, AdditionalStatus_Model &vdsData)
{
    bool result = true;

    if (appData.confAndId_presence)
    {
        vdsData.confAndId_presence = true;
        SetConfigurationAndIdentity(appData.confAndId, vdsData.confAndId);
    }
    if (appData.ecuWarningMessages_presence)
    {
        vdsData.ecuWarningMessages_presence = true;
        
        for (auto p_ecuWarningMessage = appData.ecuWarningMessages.begin(); p_ecuWarningMessage != appData.ecuWarningMessages.end();) 
        {
            GenericVehicleSignal_Model ecuWarningMessage;
            ecuWarningMessage.key = p_ecuWarningMessage->key;
            if (p_ecuWarningMessage->collectTime_presence)
            {
                ecuWarningMessage.collectTime_presence = true;
                SetTimeStamp(p_ecuWarningMessage->collectTime, ecuWarningMessage.collectTime);
            }
            if (p_ecuWarningMessage->value_presence)
            {
                ecuWarningMessage.value_presence = true;
                ecuWarningMessage.value = p_ecuWarningMessage->value;
            }
            if (p_ecuWarningMessage->stringVal_presence)
            {
                ecuWarningMessage.stringVal_presence = true;
                ecuWarningMessage.stringVal = p_ecuWarningMessage->stringVal;
            }
            if (p_ecuWarningMessage->boolVal_presence)
            {
                ecuWarningMessage.boolVal_presence = true;
                ecuWarningMessage.boolVal = p_ecuWarningMessage->boolVal;
            }
            if (p_ecuWarningMessage->timestampVal_presence)
            {
                ecuWarningMessage.timestampVal_presence = true;
                SetTimeStamp(p_ecuWarningMessage->timestampVal, ecuWarningMessage.timestampVal);
            }
            if (p_ecuWarningMessage->dataVal_presence)
            {
                ecuWarningMessage.dataVal_presence = true;
                ecuWarningMessage.dataVal = p_ecuWarningMessage->dataVal;
            }
            vdsData.ecuWarningMessages.push_back(ecuWarningMessage);

        
            ++p_ecuWarningMessage;
        }
        
    }
    if (appData.towStatus_presence)
    {
        vdsData.towStatus_presence = true;
        SetTowStatus(appData.towStatus, vdsData.towStatus);
    }
    if (appData.signals_presence)
    {
        vdsData.signals_presence = true;
        
        for (auto p_signal = appData.signals.begin(); p_signal != appData.signals.end();) 
        {
            GenericVehicleSignal_Model signal;
            signal.key = p_signal->key;
            if (p_signal->collectTime_presence)
            {
                signal.collectTime_presence = true;
                SetTimeStamp(p_signal->collectTime, signal.collectTime);
            }
            if (p_signal->value_presence)
            {
                signal.value_presence = true;
                signal.value = p_signal->value;
            }
            if (p_signal->stringVal_presence)
            {
                signal.stringVal_presence = true;
                signal.stringVal = p_signal->stringVal;
            }
            if (p_signal->boolVal_presence)
            {
                signal.boolVal_presence = true;
                signal.boolVal = p_signal->boolVal;
            }
            if (p_signal->timestampVal_presence)
            {
                signal.timestampVal_presence = true;
                SetTimeStamp(p_signal->timestampVal, signal.timestampVal);
            }
            if (p_signal->dataVal_presence)
            {
                signal.dataVal_presence = true;
                signal.dataVal = p_signal->dataVal;
            }
            vdsData.signals.push_back(signal);

        
            ++p_signal;
        }
        
    }
    if (appData.data_presence)
    {
        vdsData.data_presence = true;
        vdsData.data = appData.data;
    }

    
    return result;
}

bool RvsSignalAdapt::SetLockStatusPayload(const LockStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.lockStatus_presence = true;
    bool result = SetLockStatus(payload, m_vdsData.body.serviceData.vehicleStatus.lockStatus);

    return result;
}

bool RvsSignalAdapt::SetMaintenanceStatusPayload(const MaintenanceStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus_presence = true;
    bool result = SetMaintenanceStatus(payload, m_vdsData.body.serviceData.vehicleStatus.maintenanceStatus);

    return result;
}
bool RvsSignalAdapt::SetRunningStatusPayload(const RunningStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.runningStatus_presence = true;
    bool result = SetRunningStatus(payload, m_vdsData.body.serviceData.vehicleStatus.runningStatus);

    return result;
}
bool RvsSignalAdapt::SetClimateStatusPayload(const ClimateStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.climateStatus_presence = true;
    bool result = SetClimateStatus(payload, m_vdsData.body.serviceData.vehicleStatus.climateStatus);

    return result;
}
bool RvsSignalAdapt::SetPollutionStatusPayload(const PollutionStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.pollutionStatus_presence = true;
    bool result = SetPollutionStatus(payload, m_vdsData.body.serviceData.vehicleStatus.pollutionStatus);

    return result;
}
bool RvsSignalAdapt::SetElectricStatusPayload(const ElectricStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.electricStatus_presence = true;
    bool result = SetElectricStatus(payload, m_vdsData.body.serviceData.vehicleStatus.electricStatus);

    return result;
}
bool RvsSignalAdapt::SetBasicVehicleStatusPayload(const BasicVehicleStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence = true;
    bool result = SetBasicVehicleStatus(payload, m_vdsData.body.serviceData.vehicleStatus.basicVehicleStatus);

    return result;
}
bool RvsSignalAdapt::SetAdditionalStatusPayload(const AdditionalStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RvsSignalAdapt::%s\n", __FUNCTION__);
    m_vdsData.body.serviceData.vehicleStatus.additionalStatus_presence = true;
    bool result = SetAdditionalStatus(payload, m_vdsData.body.serviceData.vehicleStatus.additionalStatus);

    return result;
}
bool RvsSignalAdapt::PackService(RvsRequest_t response)
{
    bool result = SignalAdaptBase::PackService(m_vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, 
                "%s(), failed to call SignalAdaptBase::PackService(m_vdsData, response) .\n", 
                __FUNCTION__);
        return result;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, 
            "%s(), success to call SignalAdaptBase::PackService(m_vdsData, response).\n", 
            __FUNCTION__);
    return result;
}
}

