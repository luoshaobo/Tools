////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file rms_transcation.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/transactions/transaction_id.h"

#include "rms/util/utils.h"
#include "rms/config/settings.h"
#include "rms/sys/postoffice.h"
#include "rms/service/alerts/storage.h"
#include "rms/sys/timerManager.h"
#include "rms/service/network/networkManager.h"
#include "rms/service/alerts/alertManager.h"
#include "rms/service/dbus/dBusManager.h"
#include "rms/service/data/dataProcessor.h"
#include "rms/service/dbus/routeDBusMethods.h"
#include "rms/rms_transaction.h"

DLT_IMPORT_CONTEXT(dlt_voc);

#define     UDL_DCDCSTATE_INDEX_LIMIT       2
#define     UDL_WARNING_LEVEL_LIMIT         4
#define     UDL_CHARGINGSTATE_INDEX_LIMIT   6
#define     UDL_ENGINESTATE_INDEX_LIMIT     10
#define     UDL_ELEC_MACHINE_LIMIT          15
#define     UDL_OPEATIONMODE_INDEX_LIMIT    16
#define     UDL_SOC_LIMIT                   100
#define     UDL_CELLS_LIMIT                 200
#define     UDL_NUMBER_LIMIT                250
#define     UDL_VEHILESPEED_LIMIT           2200
#define     UDL_VOLTAGE_LIMIT               10000
#define     UDL_MAX_VOLTAGE_LIMIT           15000
#define     UDL_CURRENT_LIMIT               20000
#define     UDL_CONSUMPTION_LIMIT           60000
#define     UDL_MAX_CELLS_LIMIT             65531
#define     UDL_ACCUMMILEAGE_LIMIT          9999999

ChargingState m_chargingState[] = {CS_ANOMALY,CS_CHARGING_PARKING,CS_CHARGING_DRIVING,CS_UNCHARGED,CS_CHARGING_COMPETED,CS_INVALID};
OperationMode m_operationMode[] = {OM_ELECTRIC,OM_ELECTRIC,OM_HYBRID,OM_HYBRID,OM_HYBRID,
                                                OM_HYBRID,OM_ELECTRIC,OM_ELECTRIC,OM_HYBRID,OM_HYBRID,
                                                OM_ELECTRIC,OM_HYBRID,OM_HYBRID,OM_ANOMALY,OM_INVALID,OM_INVALID};
DCDCState m_dcdcState[] = {DCS_CUT_OFF,DCS_IN_OPERATION,DCS_ANOMALY,DCS_INVALID};
EngineState m_engineState[] = {ES_OFF,ES_OFF,ES_OFF,ES_OFF,ES_OFF,ES_ON,ES_OFF,ES_OFF,ES_ON,ES_OFF};
ElectricalMachineState machineState[]={EMS_POWER_CONSUMTION,EMS_POWER_GENERATION,EMS_DEACTIVATION,EMS_READINESS,EMS_ANOMALY,EMS_INVALID};

namespace volvo_on_call
{

/************************************************************/
// @brief      Constructs RmsTransaction
// @param[in]  initial state
// @return
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
RmsTransaction::RmsTransaction() : fsm::SmartTransaction(kStart)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: %s", __FUNCTION__);

    isLoginStart = true;
    isVINInfoSet = false;
    isICCIDInfoSet = false;
    isRMSDataSignal = false;
    isBattVoltageSignal = false;
    isBattTempSignal = false;
    isBattCodSignal = false;
    iswarningOccur = true;

    mLastUploadData = std::make_shared<PeriodicDBUSData>(PeriodicDBUSData());
    uint8_t *pDstPeriodUploadData = reinterpret_cast<uint8_t *>( mLastUploadData.get());
    if(pDstPeriodUploadData != nullptr)
    {
        for(uint32_t i=0; i<sizeof(PeriodicDBUSData); i++)
        {
            pDstPeriodUploadData[i] = 0;
        }
    }

    if (Settings::getInstance()->config.load() != TRUE)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Failed to read the configuration file, the default configuration will be used.");
    }

    Time t;
    std::time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    std::tm* tm = std::localtime(&time);
    if(tm != nullptr)
    {
        t.year = tm->tm_year - 100;
        t.month = tm->tm_mon + 1;
        t.day = tm->tm_mday;
        t.hour = tm->tm_hour;
        t.minute = tm->tm_min;
        t.second = tm->tm_sec;
        Settings::getInstance()->setUTCTime(t);
    }

    Time ut = Settings::getInstance()->getUTCTime();
    Time ct = Settings::getInstance()->getChinaTime();

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: UTC: %02d.%02d.%02d-%02d:%02d:%02d",
        ut.year, ut.month, ut.day,
        ut.hour, ut.minute, ut.second);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: CHINA: %02d.%02d.%02d-%02d:%02d:%02d",
        ct.year, ct.month, ct.day,
        ct.hour, ct.minute, ct.second);

    NetworkManager::getInstance()->start();
    AlertManager::getInstance()->start();
    DataProcessor::getInstance()->start();
    DBusManager::getInstance()->start();

    // init State Machine
    StateMap state_map = {{kStart,
                           {nullptr,
                            fsm::SmartTransaction::SignalFunctionMap(),
                            {kCollectData, kStop}}},
                          {kCollectData,
                           {nullptr,
                            fsm::SmartTransaction::SignalFunctionMap(),
                            {kStop}}},
                          {kStop,
                           {nullptr,
                            fsm::SmartTransaction::SignalFunctionMap(),
                            {}}}};

    // To Do delete
    state_map[kStart].signal_function_map[sStart] =
        std::bind(&RmsTransaction::handleStart,
                  this,
                  std::placeholders::_1);
#if 0
    state_map[kStart].signal_function_map[fsm::Signal::kTelmDshb] =
        std::bind(&RmsTransaction::handleCollectTelmDshbData,
                  this,
                  std::placeholders::_1);

    state_map[kStart].signal_function_map[fsm::Signal::kRemoteMonitoringInfo] =
        std::bind(&RmsTransaction::handleCollectRemoteMonitoringInfo,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kTimeout] =
    std::bind(&RmsTransaction::HandleTimeOut,
              this,
              std::placeholders::_1);
#endif
    state_map[kCollectData].signal_function_map[fsm::Signal::kTimeout] =
        std::bind(&RmsTransaction::HandleTimeOut,
              this,
              std::placeholders::_1);

    state_map[kStart].signal_function_map[fsm::Signal::kRemoteMonitoringInfo] =
    std::bind(&RmsTransaction::HandleUploadData,
              this,
              std::placeholders::_1);

    state_map[kStart].signal_function_map[fsm::Signal::kHvBattVoltageData] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);


    state_map[kStart].signal_function_map[fsm::Signal::kHvBattTempData] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);

    state_map[kStart].signal_function_map[fsm::Signal::kHvBattCod] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kRemoteMonitoringInfo] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kHvBattVoltageData] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);


    state_map[kCollectData].signal_function_map[fsm::Signal::kHvBattTempData] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[fsm::Signal::kHvBattCod] =
        std::bind(&RmsTransaction::HandleUploadData,
                  this,
                  std::placeholders::_1);

    state_map[kCollectData].signal_function_map[sStop] =
        std::bind(&RmsTransaction::handleStop,
                  this,
                  std::placeholders::_1);

    SetStateMap(state_map);

    fsm::SmartTransaction::MapSignalType(sStart);
    fsm::SmartTransaction::MapSignalType(sStop);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kRemoteMonitoringInfo);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kHvBattVoltageData);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kHvBattTempData);
    fsm::SmartTransaction::MapSignalType(fsm::Signal::kHvBattCod);
}

RmsTransaction::~RmsTransaction()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);
}

/************************************************************/
// @brief      Callback function to handleCollectTelmDshbData
// @param[in]  fsm::Signal
// @return     true or false
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
bool RmsTransaction::handleCollectTelmDshbData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);

    std::shared_ptr<fsm::TelmDshbSiganl> telmDshbSignal =
                    std::static_pointer_cast<fsm::TelmDshbSiganl>(signal);
    const OpTelmDshb_NotificationCyclic_Data data = telmDshbSignal->GetPayload();
    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: VIN=%u", data.arg_blockNr);

    // Settings::getInstance()->setVin( std::string(data.arg_blockNr, GSL_VIN_LENGTH) );
    isVINInfoSet = true;

    if (kCollectData != GetState())
    {
        SetState(kCollectData);
    }

    return true;
}

/************************************************************/
// @brief      Callback function to handleSetConnectionParams
// @param[in]  fsm::Signal
// @return     true or false
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
bool RmsTransaction::handleSetConnectionParams(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);

    if(isVINInfoSet && isICCIDInfoSet)
    {
        //ServerConnectionParams data;

        //routeConnectionParams(&data);
    }
    return true;
}

/************************************************************/
// @brief      Callback function to handleSetConfigurationParam
// @param[in]  fsm::Signal
// @return     true or false
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
bool RmsTransaction::handleSetConfigurationParam(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);

    //xEVConfigurations data;

    //routeConfigurationParams(&data);

    return true;
}

/************************************************************/
// @brief      Callback function to handleSetPeriodicDBUSData
// @param[in]  fsm::Signal
// @return     true or false
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
bool RmsTransaction::handleSetPeriodicDBUSData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);

    PeriodicDBUSData data;

    routeDBusPeriodicData(&data);

    return true;
}

/************************************************************/
// @brief      Callback function to handleStart
// @param[in]  fsm::Signal
// @return     true or false
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
bool RmsTransaction::handleStart(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);

    routeStartXEVCDM();

    SetState(kCollectData);

    m_RmsCycleUploadTimerId= RequestTimeout(static_cast<std::chrono::seconds>(1), true);
    MapTransactionId(m_RmsCycleUploadTimerId.GetSharedCopy());

    return true;
}

/************************************************************/
// @brief      Callback function to handleStop
// @param[in]  fsm::Signal
// @return     true or false
// @author     Shen Jiaojiao, 12-Sep-2018
/************************************************************/
bool RmsTransaction::handleStop(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"RMS: %s\n", __FUNCTION__);

    routeStopXEVCDM();

    SetState(kStop);

    return true;
}

/************************************************************/
// @brief      Callback function to HandleUploadData
// @param[in]  fsm::Signal
// @return     true or false
// @author     Lei Li, 12-Oct-2018
/************************************************************/
bool RmsTransaction::HandleUploadData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: %s received RMS signal: %s ", __FUNCTION__, signal->ToString().c_str());

    bool return_value = true;
    PeriodicDBUSData* data = mLastUploadData.get();

    switch(signal->GetSignalType())
    {
        case fsm::Signal::kRemoteMonitoringInfo:
        {
            isRMSDataSignal = true;

            std::shared_ptr<fsm::RemoteMonitoringInfoSignal> rms_notification_signal = std::static_pointer_cast<fsm::RemoteMonitoringInfoSignal>(signal);
            const OpRemoteMonitoringInfo_NotificationCyclic_Data rms_notification_data = rms_notification_signal->GetPayload();

            //vehicle state

            //chargingState
            if(rms_notification_data.vehicleStsData.chrgnSts >= Ipcb_ChrgnSts_fault && rms_notification_data.vehicleStsData.chrgnSts <= Ipcb_ChrgnSts_invalid)
                data->vehicleData.chargingState = m_chargingState[rms_notification_data.vehicleStsData.chrgnSts];
            else
                data->vehicleData.chargingState = CS_INVALID;

            //operationMode
            if(rms_notification_data.vehicleStsData.vehOperMod.dispOfPrpsnMod >= Ipcb_dispOfPrpsnMod_NotRdy && rms_notification_data.vehicleStsData.vehOperMod.dispOfPrpsnMod <= Ipcb_dispOfPrpsnMod_NotUsed)
                data->vehicleData.operationMode = m_operationMode[rms_notification_data.vehicleStsData.vehOperMod.dispOfPrpsnMod];
            else
                data->vehicleData.operationMode = OM_INVALID;

            //vehicle speed
            uint32_t veSpdIndcdUnit = rms_notification_data.vehicleStsData.vehSpdIndcdGroup.vehSpdIndcd.veSpdIndcdUnit;
            uint32_t vehSpdIndcdQly = rms_notification_data.vehicleStsData.vehSpdIndcdGroup.vehSpdIndcdQly;
            switch(vehSpdIndcdQly)
            {
                case 0:
                case 1:
                    data->vehicleData.vehicleSpeed = IL_WORD_INVALID;
                    break;
                case 2:
                    data->vehicleData.vehicleSpeed = IL_WORD_ANOMALY;
                    break;
                case 3:
                    if(veSpdIndcdUnit == 0)//VehSpdIndcdUnit_Kmph
                        data->vehicleData.vehicleSpeed = (rms_notification_data.vehicleStsData.vehSpdIndcdGroup.vehSpdIndcd.vehSpdIndcd1 & 0xFF) * 10;//calculate formula,refer to requirement
                    else if(veSpdIndcdUnit == 1)//VehSpdIndcdUnit_Mph
                        data->vehicleData.vehicleSpeed = (rms_notification_data.vehicleStsData.vehSpdIndcdGroup.vehSpdIndcd.vehSpdIndcd1 & 0xFF) / 1000 * 10;//calculate formula,refer to requirement
                    else
                    {
                        data->vehicleData.vehicleSpeed = IL_WORD_INVALID;
                        break;
                    }

                    if(data->vehicleData.vehicleSpeed < 0 || data->vehicleData.vehicleSpeed > UDL_VEHILESPEED_LIMIT)
                        data->vehicleData.vehicleSpeed = IL_WORD_ANOMALY;
                    break;
                default:
                    data->vehicleData.vehicleSpeed = IL_WORD_INVALID;
                    break;
            }

            //accumMileage
            if(rms_notification_data.vehicleStsData.bkpOfDstTrvld < 0 || rms_notification_data.vehicleStsData.bkpOfDstTrvld > UDL_ACCUMMILEAGE_LIMIT)
                data->vehicleData.accumMileage = IL_DWORD_INVALID;
            else
                data->vehicleData.accumMileage = rms_notification_data.vehicleStsData.bkpOfDstTrvld * 10;//calculate formula,refer to requirement

            //totalVoltage
            data->vehicleData.totalVoltage = rms_notification_data.vehicleStsData.hvBattSts.hvBattUDc * 0.25 * 10;//calculate formula,refer to requirement
            if(data->vehicleData.totalVoltage < 0 || data->vehicleData.totalVoltage > UDL_VOLTAGE_LIMIT)
                data->vehicleData.totalVoltage = IL_WORD_INVALID;

            //totalCurrent
            data->vehicleData.totalCurrent = rms_notification_data.vehicleStsData.hvBattSts.hvBattIDc1 - 16380 + 10000;//calculate formula,refer to requirement
            if(data->vehicleData.totalCurrent < 0 || data->vehicleData.totalCurrent > UDL_CURRENT_LIMIT)
                data->vehicleData.totalCurrent = IL_WORD_INVALID;

            //soc
            data->vehicleData.soc = rms_notification_data.vehicleStsData.hvBattSts.dispHvBattLvlOfChrg * 0.1;//calculate formula,refer to requirement
            if(data->vehicleData.soc < 0 || data->vehicleData.soc > UDL_SOC_LIMIT)
                data->vehicleData.soc = IL_BYTE_ANOMALY;

            //dcdcState
            if(rms_notification_data.vehicleStsData.dcDcActvd >= Ipcb_dcDcActvd_NoConversionToLVSide && rms_notification_data.vehicleStsData.dcDcActvd <= Ipcb_dcDcActvd_ConversionToLVSide)
                data->vehicleData.dcdcState = m_dcdcState[rms_notification_data.vehicleStsData.dcDcActvd];
            else
                data->vehicleData.dcdcState = DCS_INVALID;

            //gearPosition
            data->vehicleData.gearPosition.byte = (rms_notification_data.vehicleStsData.telmGearInfo.accrPedlPsd.accrPedlPsd << 2) & 0x04;
            data->vehicleData.gearPosition.byte |= (rms_notification_data.vehicleStsData.telmGearInfo.brkPedlPsd.brkPedlPsd << 3) & 0x08;

            if(rms_notification_data.vehicleStsData.telmGearInfo.gearLvrIndcn == 4)
            {
                if(rms_notification_data.vehicleStsData.telmGearInfo.gearIndcnRec.gearIndcn >= 0 && rms_notification_data.vehicleStsData.telmGearInfo.gearIndcnRec.gearIndcn < 7)
                    data->vehicleData.gearPosition.byte |= (rms_notification_data.vehicleStsData.telmGearInfo.gearIndcnRec.gearIndcn << 4) & 0xF0;
            }
            else
            {
                switch(rms_notification_data.vehicleStsData.telmGearInfo.gearLvrIndcn)
                {
                    case 0:
                        data->vehicleData.gearPosition.byte |= 0xF0;
                        break;
                    case 1:
                        data->vehicleData.gearPosition.byte |= 0xD0;
                        break;
                    case 2:
                        data->vehicleData.gearPosition.byte |= 0x00;
                        break;
                    case 3:
                        data->vehicleData.gearPosition.byte |= 0xE0;
                        break;
                    default:
                        break;
                }
            }

            //Insulation resistance
            data->vehicleData.insulationResistance = rms_notification_data.vehicleStsData.hvIsoR;

            //stroke Pedal
            data->vehicleData.reserved.pedals.strokeAccelPedal = rms_notification_data.vehicleStsData.obdAccrPedRat * 0.390625;

            //brake Pedal
            if(rms_notification_data.vehicleStsData.telmGearInfo.brkPedlPsd.brkPedlPsd == 0)
                data->vehicleData.reserved.pedals.brakePedalState = 0;
            else if(rms_notification_data.vehicleStsData.telmGearInfo.brkPedlPsd.brkPedlPsd == 1)
            {
                data->vehicleData.reserved.pedals.brakePedalState = 0x65;
            }

            //electrical machine SN
            (data->electrmMachineData.elecMachineDataArray)[0].sn = rms_notification_data.tractionMotorData.igmGeneric.eMSseqNr;
            if((data->electrmMachineData.elecMachineDataArray)[0].sn < 1 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].sn > UDL_ELEC_MACHINE_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].sn = IL_BYTE_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].sn = rms_notification_data.tractionMotorData.iemGeneric.eMSseqNr;
            if((data->electrmMachineData.elecMachineDataArray)[1].sn < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].sn > UDL_ELEC_MACHINE_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].sn = IL_BYTE_INVALID;
            }

            //electrical machine State
            if(rms_notification_data.tractionMotorData.igmGeneric.modStatusRms <= Ipcb_ModStatusRms_invalid ||
                    rms_notification_data.tractionMotorData.igmGeneric.modStatusRms > Ipcb_ModStatusRms_invalid1)
                data->electrmMachineData.elecMachineDataArray[0].ems = EMS_INVALID;
            else
            {
                data->electrmMachineData.elecMachineDataArray[0].ems = machineState[rms_notification_data.tractionMotorData.igmGeneric.modStatusRms];
            }

            if(rms_notification_data.tractionMotorData.iemGeneric.modStatusRms <= Ipcb_ModStatusRms_invalid ||
                    rms_notification_data.tractionMotorData.iemGeneric.modStatusRms > Ipcb_ModStatusRms_invalid1)
                data->electrmMachineData.elecMachineDataArray[1].ems = EMS_INVALID;
            else
            {
                data->electrmMachineData.elecMachineDataArray[1].ems = machineState[rms_notification_data.tractionMotorData.iemGeneric.modStatusRms];
            }

            //electrical machine temperatureController
            (data->electrmMachineData.elecMachineDataArray)[0].temperatureController = rms_notification_data.tractionMotorData.isgInvrT - 50 + 40;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[0].temperatureController < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].temperatureController > UDL_NUMBER_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].temperatureController = IL_BYTE_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].temperatureController = rms_notification_data.tractionMotorData.whlMotSysInvrT - 50 + 40;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[1].temperatureController < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].temperatureController > UDL_NUMBER_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].temperatureController = IL_BYTE_INVALID;
            }

            //electrical machine speed
            (data->electrmMachineData.elecMachineDataArray)[0].speed = rms_notification_data.tractionMotorData.isgSpdActSgn + 20000;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[0].speed < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].speed > IL_WORD_MAX)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].speed = IL_WORD_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].speed = (rms_notification_data.tractionMotorData.whlMotSysSpdAct - 16384) / 10 + 20000;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[1].speed < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].speed > IL_WORD_MAX)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].speed = IL_WORD_INVALID;
            }

            //electrical machine Torque
            (data->electrmMachineData.elecMachineDataArray)[0].torque = (rms_notification_data.tractionMotorData.isgTqAct.isgTqAct1 - 8188) * 10 + 20000;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[0].torque < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].torque > IL_WORD_MAX)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].torque = IL_WORD_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].torque = (rms_notification_data.tractionMotorData.whlMotSysTqEst.tqAct * 4 - 8188) * 10 + 20000;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[1].torque < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].torque > IL_WORD_MAX)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].torque = IL_WORD_INVALID;
            }

            //electrical machine temperature
            (data->electrmMachineData.elecMachineDataArray)[0].temperature = rms_notification_data.tractionMotorData.isgMotT - 50 + 40;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[0].temperature < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].temperature > UDL_NUMBER_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].temperature = IL_BYTE_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].temperature = rms_notification_data.tractionMotorData.whlMotSysMotT - 50 + 40;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[1].temperature < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].temperature > UDL_NUMBER_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].temperature = IL_BYTE_INVALID;
            }

            //electrical machine inVoltage
            (data->electrmMachineData.elecMachineDataArray)[0].inVoltage = rms_notification_data.tractionMotorData.isgUDc * 0.25 * 10;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[0].inVoltage < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].inVoltage > UDL_CONSUMPTION_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].inVoltage = IL_WORD_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].inVoltage = rms_notification_data.tractionMotorData.whlMotSysUdc * 0.25 * 10;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[1].inVoltage < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].inVoltage > UDL_CONSUMPTION_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].inVoltage = IL_WORD_INVALID;
            }

            //electrical machine dcCurrent
            (data->electrmMachineData.elecMachineDataArray)[0].dcCurrent = rms_notification_data.tractionMotorData.isgIDc - 8180 + 10000;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[0].dcCurrent < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[0].dcCurrent > UDL_CURRENT_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[0].dcCurrent = IL_WORD_INVALID;
            }

            (data->electrmMachineData.elecMachineDataArray)[1].dcCurrent = rms_notification_data.tractionMotorData.whlMotSysIdc - 8180 + 10000;//calculate formula,refer to requirement
            if((data->electrmMachineData.elecMachineDataArray)[1].dcCurrent < 0 ||
                    (data->electrmMachineData.elecMachineDataArray)[1].dcCurrent > UDL_CURRENT_LIMIT)
            {
                (data->electrmMachineData.elecMachineDataArray)[1].dcCurrent = IL_WORD_INVALID;
            }

            //Engine state
            auto iter = find(rms_notification_data.engineData.engFltIndcn.begin(),rms_notification_data.engineData.engFltIndcn.end(),Ipcb_EngFltIndcn_devErrSts2_Flt);
            if(iter == rms_notification_data.engineData.engFltIndcn.end())
            {
                if(rms_notification_data.engineData.engst1WdSts >= Ipcb_engSt_Ini && rms_notification_data.engineData.engst1WdSts <= Ipcb_engSt_AftRun)
                    data->engineData.engineState = m_engineState[rms_notification_data.engineData.engst1WdSts];
                else
                    data->engineData.engineState = ES_INVALID;
            }
            else
            {
                data->engineData.engineState = ES_ANOMALY;
            }

            //Engine Crankshaft speed
            data->engineData.crankshaftSpeed = rms_notification_data.engineData.engN2 * 0.5;//calculate formula,refer to requirement
            if(data->engineData.crankshaftSpeed < 0 || data->engineData.crankshaftSpeed > UDL_CONSUMPTION_LIMIT)
                data->engineData.crankshaftSpeed = IL_WORD_INVALID;

            //Engine Fuel consumption
            data->engineData.fuelConsumption = rms_notification_data.engineData.engFuCnsFild * 200 / 10;//calculate formula,refer to requirement
            if(data->engineData.fuelConsumption < 0 || data->engineData.fuelConsumption > UDL_CONSUMPTION_LIMIT)
                data->engineData.fuelConsumption = IL_WORD_INVALID;

            //warning
            std::vector<uint32_t> m_vWarning;

            if(rms_notification_data.alarmData.generalAlarmFlag.hvCellTDifFlt != 0)
            {
                data->warningData.mark.bit.temprDiff &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvCellTDifFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvCellTOverFlt != 0)
            {
                data->warningData.mark.bit.temprBatt &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvCellTOverFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvPackUOverFlt != 0)
            {
                data->warningData.mark.bit.overVoltageVehicle &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvPackUOverFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvPackUUnderFlt != 0)
            {
                data->warningData.mark.bit.underVoltageVehicle &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvPackUUnderFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvSocLoFlt != 0)
            {
                data->warningData.mark.bit.lowSOC &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvSocLoFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvCellUOverFlt != 0)
            {
                data->warningData.mark.bit.overVoltageCell &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvCellUOverFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvCellUUnderFlt != 0)
            {
                data->warningData.mark.bit.underVoltageCell &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvCellUUnderFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvSocHiFlt != 0)
            {
                data->warningData.mark.bit.highSOC &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvSocHiFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvSocHopFlt != 0)
            {
                data->warningData.mark.bit.jumpSOC &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvSocHopFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvBattMismatchFlt != 0)
            {
                data->warningData.mark.bit.unmatched &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvBattMismatchFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvCellUDifFlt != 0)
            {
                data->warningData.mark.bit.poorConsCell &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvCellUDifFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvCellUDifFlt != 0)
            {
                data->warningData.mark.bit.poorConsCell &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvCellUDifFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvIsoFlt != 0)
            {
                data->warningData.mark.bit.insulation &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvIsoFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.fltTDcDc != 0)
            {
                data->warningData.mark.bit.dcdcTempr &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.fltTDcDc);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.escWarnIndcnReq.escWarnIndcnReq1 == 0 ||
                    rms_notification_data.alarmData.generalAlarmFlag.escWarnIndcnReq.escWarnIndcnReq1 == 1 ||
                    rms_notification_data.alarmData.generalAlarmFlag.brkAndAbsWarnIndcnReq.absWarnIndcnReq == 0 ||
                    rms_notification_data.alarmData.generalAlarmFlag.brkAndAbsWarnIndcnReq.absWarnIndcnReq == 1)
            {
                data->warningData.mark.bit.breaks &= 0x1;
                m_vWarning.push_back(data->warningData.mark.bit.breaks);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.fltElecDcDc.fltElecDcDc != 0)
            {
                data->warningData.mark.bit.dcdcState &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.fltElecDcDc.fltElecDcDc);
            }

            if(rms_notification_data.tractionMotorData.igmGeneric.motTAlrmSt != 0 ||
                    rms_notification_data.tractionMotorData.iemGeneric.motTAlrmSt != 0)
            {
                data->warningData.mark.bit.emcTempr &= 0x1;
                m_vWarning.push_back(data->warningData.mark.bit.emcTempr);
            }

            if(rms_notification_data.tractionMotorData.igmGeneric.invrtTAlrmSt != 0 ||
                    rms_notification_data.tractionMotorData.iemGeneric.invrtTAlrmSt != 0)
            {
                data->warningData.mark.bit.emTempr &= 0x1;
                m_vWarning.push_back(data->warningData.mark.bit.emTempr);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvilFlt != 0)
            {
                data->warningData.mark.bit.highVIlock &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvilFlt);
            }

            if(rms_notification_data.alarmData.generalAlarmFlag.hvPackOverChrgFlt != 0)
            {
                data->warningData.mark.bit.overCharge &= 0x1;
                m_vWarning.push_back(rms_notification_data.alarmData.generalAlarmFlag.hvPackOverChrgFlt);
            }

            if(!m_vWarning.empty())
            {
                WarningLevel m_uploadWarningLevel[] = {WL_LEVEL0,WL_LEVEL1,WL_LEVEL2,WL_LEVEL3};

                sort(m_vWarning.begin(),m_vWarning.end());
                if(m_vWarning[m_vWarning.size() -1] >= 0 && m_vWarning[m_vWarning.size() -1] < UDL_WARNING_LEVEL_LIMIT)
                    data->warningData.level = m_uploadWarningLevel[m_vWarning[m_vWarning.size() -1]];
                else
                    data->warningData.level = WL_LEVEL_INVALID;
            }

            break;
        }
        case fsm::Signal::kHvBattVoltageData:
        {
            isBattVoltageSignal = true;

            std::shared_ptr<fsm::HvBattVoltageDataSignal> batt_voltage_signal = std::static_pointer_cast<fsm::HvBattVoltageDataSignal>(signal);
            const OpHvBattVoltageData_Notification_Data batt_voltage_data = batt_voltage_signal->GetPayload();

            //limitValue BattMaxVoltage
            data->limitValueData.idBattMaxVoltage = batt_voltage_data.hvBattVoltMaxSerlNr;
            if(data->limitValueData.idBattMaxVoltage < 1 || data->limitValueData.idBattMaxVoltage > UDL_NUMBER_LIMIT)
                data->limitValueData.idBattMaxVoltage = IL_BYTE_INVALID;

            //limitValue CellMaxVoltage
            data->limitValueData.idCellMaxVoltage = batt_voltage_data.cellUMaxNr;
            if(data->limitValueData.idCellMaxVoltage < 1 || data->limitValueData.idCellMaxVoltage > UDL_NUMBER_LIMIT)
                data->limitValueData.idCellMaxVoltage = IL_BYTE_INVALID;

            //limitValue maxCellVoltage
            data->limitValueData.maxCellVoltage = batt_voltage_data.cellUMax;
            if(data->limitValueData.maxCellVoltage < 1 || data->limitValueData.maxCellVoltage > UDL_MAX_VOLTAGE_LIMIT)
                data->limitValueData.maxCellVoltage = IL_WORD_INVALID;

            //limitValue BattMinVoltage
            data->limitValueData.idBattMinVoltage = batt_voltage_data.hvBattVoltMinSerlNr;
            if(data->limitValueData.idBattMinVoltage < 1 || data->limitValueData.idBattMinVoltage > UDL_NUMBER_LIMIT)
                data->limitValueData.idBattMinVoltage = IL_BYTE_INVALID;

            //limitValue CellMinVoltage
            data->limitValueData.idCellMinVoltage = batt_voltage_data.cellUMinNr;
            if(data->limitValueData.idCellMinVoltage < 1 || data->limitValueData.idCellMinVoltage > UDL_NUMBER_LIMIT)
                data->limitValueData.idCellMinVoltage = IL_BYTE_INVALID;

            //limitValue minCellVoltage
            data->limitValueData.minCellVoltage = batt_voltage_data.cellUMin;
            if(data->limitValueData.minCellVoltage < 1 || data->limitValueData.minCellVoltage > UDL_MAX_VOLTAGE_LIMIT)
                data->limitValueData.minCellVoltage = IL_WORD_INVALID;

            //chargeable energy-storage subsystems Number
            data->voltCESSData.size = batt_voltage_data.hvBattNr;
            if(data->voltCESSData.size < 1 || data->voltCESSData.size > UDL_NUMBER_LIMIT)
                data->voltCESSData.size = IL_BYTE_INVALID;

            //chargeable energy-storage subsystem code
            data->voltCESSData.data.code = batt_voltage_data.packNr;
            if(data->voltCESSData.data.code < 1 ||data->voltCESSData.data.code > UDL_NUMBER_LIMIT)
                data->voltCESSData.data.code = IL_BYTE_INVALID;

            //chargeable energy-storage device Voltage
            data->voltCESSData.data.voltage = batt_voltage_data.battU;
            if(data->voltCESSData.data.voltage < 0 || data->voltCESSData.data.voltage > UDL_VOLTAGE_LIMIT)
                data->voltCESSData.data.voltage = IL_WORD_INVALID;

            //chargeable energy-storage device Current
            data->voltCESSData.data.current = batt_voltage_data.hvBattIDc1 - 16380 + 10000;
            if(data->voltCESSData.data.current < 0 || data->voltCESSData.data.current > UDL_CURRENT_LIMIT)
                data->voltCESSData.data.current = IL_WORD_INVALID;

            //Total number of cell
            data->voltCESSData.data.totalCells = batt_voltage_data.cellTotNr;
            if(data->voltCESSData.data.totalCells < 0 || data->voltCESSData.data.totalCells > UDL_MAX_CELLS_LIMIT)
                data->voltCESSData.data.totalCells = IL_WORD_INVALID;

            //starting battery SN
            data->voltCESSData.data.snCell = batt_voltage_data.hvBattCellUInfo[0].cellUNr;

            //Total cell number of this frame
            data->voltCESSData.data.frameCells = batt_voltage_data.hvBattCellUInfo[0].msgPackTotNr;
            if(data->voltCESSData.data.frameCells < 1 || data->voltCESSData.data.frameCells > UDL_CELLS_LIMIT)
                data->voltCESSData.data.frameCells = IL_BYTE_INVALID;

            //Total cell number of this frame

            break;
        }
        case fsm::Signal::kHvBattTempData:
        {
            isBattTempSignal = true;

            std::shared_ptr<fsm::HvBattTempDataSignal> batt_temp_signal = std::static_pointer_cast<fsm::HvBattTempDataSignal>(signal);
            const OpHvBattTempData_Notification_Data batt_temp_data = batt_temp_signal->GetPayload();

            //limitValue idMaxTemp
            data->limitValueData.idMaxTemp = batt_temp_data.hvBattTMaxSerlNr;
            if(data->limitValueData.idMaxTemp < 1 || data->limitValueData.idMaxTemp > UDL_NUMBER_LIMIT)
                data->limitValueData.idMaxTemp = IL_BYTE_INVALID;

            //limitValue codeMaxTemprProbe
            if(data->limitValueData.idMaxTemp != IL_BYTE_INVALID)
            {
                data->limitValueData.codeMaxTemprProbe = (batt_temp_data.hvBattCellTInfo[data->limitValueData.idMaxTemp]).hvTempSnsrTMaxsSerlNr;
                if(data->limitValueData.codeMaxTemprProbe < 1 || data->limitValueData.codeMaxTemprProbe > UDL_NUMBER_LIMIT)
                    data->limitValueData.codeMaxTemprProbe = IL_BYTE_INVALID;
            }

            //limitValue maxTemp
            if(data->limitValueData.idMaxTemp != IL_BYTE_INVALID)
            {
                data->limitValueData.maxTemp = (batt_temp_data.hvBattCellTInfo[data->limitValueData.idMaxTemp]).hvBattTMax;
                if(data->limitValueData.maxTemp < 1 || data->limitValueData.maxTemp > UDL_NUMBER_LIMIT)
                    data->limitValueData.maxTemp = IL_BYTE_INVALID;
            }

            //limitValue idMinTemp
            data->limitValueData.idMinTemp = batt_temp_data.hvBattTMinSerlNr;
            if(data->limitValueData.idMinTemp < 1 || data->limitValueData.idMinTemp > UDL_NUMBER_LIMIT)
                data->limitValueData.idMinTemp = IL_BYTE_INVALID;

            //limitValue codeMinTemprProbe
            if(data->limitValueData.idMinTemp != IL_BYTE_INVALID)
            {
                data->limitValueData.codeMinTemprProbe = (batt_temp_data.hvBattCellTInfo[data->limitValueData.idMinTemp]).hvTempSnsrTMinsSerlNr;
                if(data->limitValueData.codeMinTemprProbe < 1 || data->limitValueData.codeMinTemprProbe > UDL_NUMBER_LIMIT)
                    data->limitValueData.codeMinTemprProbe = IL_BYTE_INVALID;
            }

            //limitValue MinTemp
            if(data->limitValueData.idMinTemp != IL_BYTE_INVALID)
            {
                data->limitValueData.minTemp = (batt_temp_data.hvBattCellTInfo[data->limitValueData.idMinTemp]).hvBattTMin;
                if(data->limitValueData.minTemp < 1 || data->limitValueData.minTemp > UDL_NUMBER_LIMIT)
                    data->limitValueData.minTemp = IL_BYTE_INVALID;
            }

            //chargeable energy-storage subsystems Number
            data->tempCESSData.size = batt_temp_data.hvBattNr;
            if(data->tempCESSData.size < 1 || data->tempCESSData.size > UDL_NUMBER_LIMIT)
                data->tempCESSData.size = IL_BYTE_INVALID;

            //chargeable energy-storage subsystem Code
            data->tempCESSData.data.code = batt_temp_data.packNr;
            if(data->tempCESSData.data.code < 1 || data->tempCESSData.data.code > UDL_NUMBER_LIMIT)
                data->tempCESSData.data.code = IL_BYTE_INVALID;

            //chargeable energy-storage Quantity
            data->tempCESSData.data.size = batt_temp_data.hvBattCellTNr;
            if(data->tempCESSData.data.size < 1 || data->tempCESSData.data.size > IL_WORD_MAX)
                data->tempCESSData.data.size = IL_WORD_INVALID;

            //Temperature value detected by each temperature probe of chargeable energy-storage subsystem

            break;
        }
        case fsm::Signal::kHvBattCod:
        {
            isBattCodSignal = true;

            //std::shared_ptr<fsm::HvBattCodSignal> batt_Cod_signal = std::static_pointer_cast<fsm::HvBattCodSignal>(signal);
            //const OpHvBattCod_Notification_Data batt_Cod_data = batt_Cod_signal->GetPayload();

            break;
        }
        default:
            break;
    }

    if(isLoginStart && isRMSDataSignal && isBattVoltageSignal && isBattTempSignal && isBattCodSignal)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: send start signal");
        isLoginStart = false;
        std::shared_ptr<fsm::Signal> start = std::make_shared<StartSignal>();
        fsm::SmartTransaction::EnqueueSignal(start);
    }

    return return_value;
}

bool RmsTransaction::HandleTimeOut(std::shared_ptr<fsm::Signal> signal)
{
    if (signal->GetTransactionId() == m_RmsCycleUploadTimerId)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS:%s:m_RmsCycleUploadTimerId", __FUNCTION__);

        if((mLastUploadData.get())->warningData.level == WL_LEVEL3 && !iswarningOccur &&
            !AlertManager::getInstance()->getWarningMode())
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS:Enter Warning Mode");

            iswarningOccur = true;
            routeEnterWarningMode();
        }else if((mLastUploadData.get())->warningData.level == WL_LEVEL0 && !AlertManager::getInstance()->getWarningMode())
        {
            iswarningOccur = false;
        }

        //get current UTC time
        std::time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        std::tm* tm = std::localtime(&time);
        if(tm != nullptr)
        {
            mLastUploadData.get()->time.year = tm->tm_year - 100;
            mLastUploadData.get()->time.month = tm->tm_mon + 1;
            mLastUploadData.get()->time.day = tm->tm_mday;
            mLastUploadData.get()->time.hour = tm->tm_hour;
            mLastUploadData.get()->time.minute = tm->tm_min;
            mLastUploadData.get()->time.second = tm->tm_sec;
            Settings::getInstance()->setUTCTime(mLastUploadData.get()->time);
        }

        Time ut = Settings::getInstance()->getUTCTime();
        Time ct = Settings::getInstance()->getChinaTime();

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: HandleUploadData UTC: %02d.%02d.%02d-%02d:%02d:%02d",
            ut.year, ut.month, ut.day,
            ut.hour, ut.minute, ut.second);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: HandleUploadData CHINA: %02d.%02d.%02d-%02d:%02d:%02d",
            ct.year, ct.month, ct.day,
            ct.hour, ct.minute, ct.second);

        RemoveTimeout(m_RmsCycleUploadTimerId);
        m_RmsCycleUploadTimerId= RequestTimeout(static_cast<std::chrono::seconds>(1), true);
        MapTransactionId(m_RmsCycleUploadTimerId.GetSharedCopy());

        routeDBusPeriodicData(mLastUploadData.get());
    }

    return true;
}

} // namespace volvo_on_call

