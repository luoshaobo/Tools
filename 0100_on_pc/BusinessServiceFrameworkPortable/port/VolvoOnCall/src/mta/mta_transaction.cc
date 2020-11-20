/**
 * Copyright (C) 2018 Continental AG and subsidiaries
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     admin_confirmation_transaction.h
 *  \brief    Device pairing admin confirmation transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include <sstream>
#include <iomanip>

#include "mta/mta_transaction.h"
#include "timestamp.h"
#include "dlt/dlt.h"

#include "vehicle_comm_interface.hpp"
#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc_framework/signals/vehicle_comm_signal.h"

extern "C"
{
#include "powermgr.h"
}

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

bool GetPowerMode(fsm::MTAData_t& mta)
{
    tpPM_ReturnType_t return_status = E_PM_RET_FAILURE;
    tpVCS_PowerState_t power_state = E_VCS_POWER_STATE_INVALID;

    //get current power mode
    if((return_status = tpPM_getPowerMode(&power_state)) != E_PM_RET_SUCCESS)
    {
        DLT_LOG_STRINGF(dlt_voc,
                        DLT_LOG_ERROR,
                        "%s: Cannot get power mode(return:%d) ",
                        __FUNCTION__,
                        return_status);

        return false;
    }
    else
    {
       // mta.power_mode : normal/standby/sleep_poll/off
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: Power state: [%d]", __FUNCTION__, power_state);
        switch(power_state)
        {
             case E_VCS_POWER_STATE_INVALID: // Invalid power state
                mta.power_mode = fsm::MTA_Power_Mode_Power_Off;
                break;
             case E_VCS_POWER_STATE_NORMAL:   // Normal power state
                mta.power_mode = fsm::MTA_Power_Mode_Normal;
                break;
             case E_VCS_POWER_STATE_SHUTDOWN_DELAY:   //Shutdown delay power state
                mta.power_mode = fsm::MTA_Power_Mode_Normal;
                break;
             case E_VCS_POWER_STATE_NAD_ON:// Digital NAD on power state
                mta.power_mode = fsm::MTA_Power_Mode_Standby;
                break;
             case E_VCS_POWER_STATE_DRX:// Polling power state
                mta.power_mode = fsm::MTA_Power_Mode_Sleep_Poll;
                break;
             case E_VCS_POWER_STATE_FULL_SLEEP:// Full sleep power state, NAD is off
                mta.power_mode = fsm::MTA_Power_Mode_Power_Off;
                break;
             case E_VCS_POWER_STATE_HALT: // Halt power mode
                mta.power_mode = fsm::MTA_Power_Mode_Power_Off;
                break;
             case E_VCS_POWER_STATE_ERA_CALLBACK:// ERA call back power mode
                mta.power_mode = fsm::MTA_Power_Mode_Standby;
                break;
             case E_VCS_POWER_STATE_LAST: //E_VCS_POWER_STATE_LAST
                mta.power_mode = fsm::MTA_Power_Mode_Power_Off;
                break;
             default:
                mta.power_mode = fsm::MTA_Power_Mode_Normal;
                break;
        }
    }
    return true;
}

void PrintMTA(const fsm::MTAData_t& mta )
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "vin           :%s", mta.vin.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "ecu_serial_number :%s", mta.ecu_serial_number.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "msisdn        :%s", mta.msisdn.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "imsi          :%s", mta.imsi.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "icc_id        :%s", mta.icc_id.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "mobile_network:%s", mta.mobile_network.c_str());

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "power_source  :%d", mta.power_source);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "power_mode    :%d", mta.power_mode);
}

MTATransaction::MTATransaction() : fsm::SmartTransaction(kStart)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    m_currentRetryCount = 0;
    m_requestVinResult = false;
    m_requestEcuSerialNumerResult = false;
    m_handleVin = false;
    m_handleEcuSerialNumer = false;

    fsm::VdServiceTransactionId vdsTransId;
    asn_wrapper::VDServiceRequest *vdmsg = nullptr;
    m_mtaSignal =  std::dynamic_pointer_cast<GlyVdsMTASignal> (GlyVdsMTASignal::CreateGlyVdsMTASignal(vdsTransId, vdmsg));
    if (m_mtaSignal.get() == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "MTA:%s() m_currentMTAData.get() == nullptr", __FUNCTION__);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "MTA:%s()m_currentMTAData = %d", __FUNCTION__, (void *) m_mtaSignal.get());

    SignalFunction handle_start_upload_mta_signal = std::bind(&MTATransaction::HandleStartUploadMTA,this,std::placeholders::_1);
    //SignalFunction handle_start_collect_mta_signal = std::bind(&MTATransaction::HandleStartCollectMTAData,this,std::placeholders::_1);

    SignalFunction handle_vin_number_signal = std::bind(&MTATransaction::HandleVINNumber,this,std::placeholders::_1);
    SignalFunction handle_tcam_hw_ver_signal = std::bind(&MTATransaction::HandleGetTcamHwVer,this,std::placeholders::_1);
    SignalFunction handle_time_out_signal = std::bind(&MTATransaction::HandleTimeOut,this,std::placeholders::_1);

    StateMap state_map = {{kStart,              {nullptr,SignalFunctionMap(), {kStartCollectData}}},
                          {kStartCollectData,   {nullptr,SignalFunctionMap(), {kCollectData,kUpload}}},
                          {kCollectData,        {nullptr,SignalFunctionMap(), {kUpload}}},
                          {kUpload,             {nullptr,SignalFunctionMap(), {kStartCollectData,kStop}}}
                         };

    state_map[kStart].signal_function_map[fsm::Signal::kSubscribeTopicSignal] = handle_start_upload_mta_signal;
    state_map[kStartCollectData].state_function = std::bind(&MTATransaction::HandleStartCollectMTAData,this);

    state_map[kCollectData].signal_function_map[fsm::Signal::kVINNumberSignal] = handle_vin_number_signal;
    state_map[kCollectData].signal_function_map[fsm::Signal::kTcamHwVerSignal] = handle_tcam_hw_ver_signal;

    state_map[kUpload].state_function = std::bind(&MTATransaction::HandleUploadMTA,this);
    state_map[kUpload].signal_function_map[fsm::Signal::kTimeout] = handle_time_out_signal;

    state_map[kStop].state_function = std::bind(&MTATransaction::HandleStopMTAService,this);

    SetStateMap(state_map);

    MapSignalType(fsm::Signal::kSubscribeTopicSignal);
    MapSignalType(fsm::Signal::kVINNumberSignal);
    MapSignalType(fsm::Signal::kTcamHwVerSignal);
}

bool MTATransaction::HandleTimeOut(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MTATransaction::HandleTimeOut");
    fsm::TimeoutTransactionId* tsId = (fsm::TimeoutTransactionId*)&(signal->GetTransactionId());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "signal type %d %ld",
        signal->GetSignalType(),
        tsId->GetId());

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:end", __FUNCTION__);

   ++m_currentRetryCount;
   if(m_currentRetryCount <= m_maxRetryTimes )
   {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MTATransaction::StartTimerAndRetry");
        m_timeoutTransactionId = fsm::TimeoutTransactionId();
        MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
        RequestTimeout(m_timeoutTransactionId,static_cast<std::chrono::seconds>(m_kTimeout60), false);
        SetState(kStartCollectData);
   }
   else
   {
      SetState(kUpload);
   }

    return true;
}

bool MTATransaction::HandleVINNumber(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "mta %s", __FUNCTION__);

    std::shared_ptr<fsm::VINNumberSignal> vinNumberSignal = std::static_pointer_cast<fsm::VINNumberSignal>(signal);

    m_handleVin = true;

    if(vinNumberSignal)
    {
        if(vinNumberSignal->GetData()->vin_number[0] == 0xFF) // no vin_number exists( no canoe condition)
        {
           DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "mta, vin number invalid.");
        }
        else
        {
            m_mtaData.vin = vinNumberSignal->GetData()->vin_number;
        }
    }
    else
    {
       DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "mta, get vin number fail.");
    }

    if(!m_requestEcuSerialNumerResult || m_handleEcuSerialNumer)
    {
       SetState(kUpload);
    }

    return true;
}

bool MTATransaction::HandleGetTcamHwVer(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "mta %s", __FUNCTION__);

    m_handleEcuSerialNumer = true;

    std::shared_ptr<fsm::TcamHwVerSignal> tcamHwVerSignal = std::static_pointer_cast<fsm::TcamHwVerSignal>(signal);
    if(tcamHwVerSignal)
    {
        std::stringstream ss;
        std::for_each(tcamHwVerSignal->GetData()->eol_volvo_ecu_serial_number,
                      tcamHwVerSignal->GetData()->eol_volvo_ecu_serial_number+4,
                      [&ss](uint8_t c){ ss << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(c); } );
        m_mtaData.ecu_serial_number = ss.str();
    }
    else
    {
       DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "mta, get ecu serial number fail.");
    }

    if( !m_requestVinResult || m_handleVin )
    {
       SetState(kUpload);
    }

    return true;
}

bool MTATransaction::StartTimerAndRetry()
{
    m_handleVin = false;
    m_handleEcuSerialNumer = false;
    m_requestVinResult = false;
    m_requestEcuSerialNumerResult = false;

    ++m_currentRetryCount;
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MTATransaction::StartTimerAndRetry");
    m_timeoutTransactionId = fsm::TimeoutTransactionId();
    MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
    RequestTimeout(m_timeoutTransactionId,static_cast<std::chrono::seconds>(m_kTimeout60), false);
    SetState(kStartCollectData);
    return true;
}

bool MTATransaction::HandleStartCollectMTAData()
{
    //init m_mtaData struct
    m_mtaData.vin.clear();
    m_mtaData.ecu_serial_number.clear();
    m_mtaData.msisdn.clear();
    m_mtaData.imsi.clear();
    m_mtaData.icc_id.clear();
    m_mtaData.mobile_network.clear();

    m_requestVinResult = RequestVinNumber();
    m_requestEcuSerialNumerResult = RequesTcamHwVersion();

    if( !m_requestVinResult && !m_requestEcuSerialNumerResult)
    {
       SetState(kUpload);
       return true;
    }

    SetState(kCollectData);
    return true;

}

bool MTATransaction:: HandleStartUploadMTA(std::shared_ptr<fsm::Signal> signal)
{
   DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MTATransaction::HandleStartUploadMTA");

  vocpersistdatamgr::VocPersistDataMgr& vocpersist = vocpersistdatamgr::VocPersistDataMgr::GetInstance();

  std::shared_ptr<fsm::InternalSignal<bool>> internal_signal = std::static_pointer_cast<fsm::InternalSignal<bool>>(signal);

  uint32_t mta_uploaded_flag = 0; // 0: have not upload, ready to upload
  if(! *internal_signal->GetPayload()) // if not force upload
  {
      bool result = vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::MTA_UPLOADED_FLAG, mta_uploaded_flag);
      if(!result)
      {
         mta_uploaded_flag = 0;
      }

      if(1==mta_uploaded_flag)
      {
         DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "need not upload mta to tsp this time.");
         SetState(kStop);
         return true;
      }
  }

  SetState(kStartCollectData);

  return true;
}

bool MTATransaction::HandleUploadMTA()
{
   DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "HandleUploadMTA");
  //m_mtaData.vin = "12345678901234567";
  //m_mtaData.serial_number = "004401810357539";
  //m_mtaData.msisdn = "18616351241";
  //m_mtaData.imsi = "460018319704073";
  //m_mtaData.icc_id = "89860118795910140735";
  //m_mtaData.mobile_network = "4G";
  //m_mtaData.power_source = 0;
  //m_mtaData.power_mode = 0;

  bool collect_data_succes = true;

  if(!GetPowerMode(m_mtaData))
  {
     collect_data_succes = false;
  }

  fsm::VocmoSignalSource vocmo_signal_source;
   // get sim info: msisdn,imsi,iccId
  if( !vocmo_signal_source.GetSimCurrentProfile(m_mtaData))
  {
     collect_data_succes = false;
  }

   // get mobileNetwork : 2G/3G/4G/wifi ?
  if( !vocmo_signal_source.GetNadIfNetworkSignalQuality(m_mtaData))
  {
     collect_data_succes = false;
  }

  if( m_mtaData.vin.empty() || m_mtaData.ecu_serial_number.empty())
  {
     collect_data_succes = false;
  }

  if(!collect_data_succes)
  {
    if( 0 == m_currentRetryCount)
    {
       StartTimerAndRetry();
       return true;
    }

    // while m_currentRetryCount == m_maxRetryTimes, it means the third trying.
    // running here aslo means the third trying failed, so need not to wait timeout anymore
    if(m_currentRetryCount < m_maxRetryTimes)
    {
        return true; // wait for timeout signal and try to start collecting data again
    }
  }

   // set power soure default value
   m_mtaData.power_source = fsm::MTA_Power_Sourece_Main_Battery;

   PrintMTA(m_mtaData);

   bool set_payload_result = false;
   bool send_result = false;

   set_payload_result = m_mtaSignal->SetMTAPayload(m_mtaData);
   if ( !set_payload_result )
   {
       DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s: SetMTAPayload failed.", __FUNCTION__);
   }

   if(set_payload_result)
   {
      send_result = vocmo_signal_source.GeelySendMessage(m_mtaSignal);
   }

   if (send_result)
   {
       DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MTA: upload actvd ok.");
   }
   else
   {
       DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "MTA: upload actvd fail.");
   }

   DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s:end", __FUNCTION__);

   if(send_result)
   {
      vocpersistdatamgr::VocPersistDataMgr& vocpersist = vocpersistdatamgr::VocPersistDataMgr::GetInstance();
      vocpersist.SetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::MTA_UPLOADED_FLAG, 1);
   }

   SetState(kStop);
   return true;
}

bool MTATransaction::HandleStopMTAService()
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "MTATransaction::HandleStopMTAService");
    return true;
}

bool MTATransaction::RequestVinNumber()
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "mta RequestVinNumber");

    bool ret = true;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

    if (vc::RET_OK == igen.Request_VINNumber(vehicle_comm_request_id->GetId()))
    {
        ret = true;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,"mta send VehicleComm vinNumber request ok.");
    }
    else
    {
        ret = false;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,"mta send VehicleComm vinNumber request fail.");
    }
    return ret;
}

bool MTATransaction::RequesTcamHwVersion()
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "mta RequesTcamHwVersion");

    bool ret = true;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc::IDiagnostics& igen = vc_signal_source.GetVehicleCommClientObject().GetDiagnosticsInterface();

    if (vc::RET_OK == igen.Request_GetTcamHwVersion(vehicle_comm_request_id->GetId()))
    {
        ret = true;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,"mta send VehicleComm tcamHwVersion request ok.");
    }
    else
    {
        ret = false;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,"mta send VehicleComm tcamHwVersion request fail.");
    }
    return ret;
}

}// namespace volvo_on_call
/** \}    end of addtogroup */
