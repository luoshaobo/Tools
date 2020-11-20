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
*  \file     rvs_transaction.h
*  \brief    rvs transaction.
*  \author   Jun Yang
*
*  \addtogroup VolvoOnCall
*  \{
*/

#ifndef RVS_TRANSACTION_H_
#define RVS_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rvs_signal.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include <time.h>

namespace volvo_on_call
{

class Rvs: public fsm::SmartTransaction
{
public:

    enum RVSTriggerTypes
    {
        kTriggerType_Unknown,           // initialize
        kTriggerType_StartDriving,
        kTriggerType_StopDriving,
        kTriggerType_InDriving,
        kTriggerType_Max
    };

    enum RVSTriggerTypesChangeReason
    {
        kTriggerChgRsn_CarModChg,       // car mode or usage mode changed
        kTriggerChgRsn_Uploaded,        // data uploaded
        kTriggerChgRsn_TimerChg,        // timer config changed
        kTriggerChgRsn_Timeout          // timeout occured (reserved)
    };

    enum RvsCspConfigType
    {
        kRvs_config_empty = 0x0,  //empty config type
        kRvs_bs = 0x01,             //basicVehicleStatus
        kRvs_ls = 0x02,             //lock Status
        kRvs_ms = 0x04,             //Maintenance Status
        kRvs_rs = 0x08,             //Running Status
        kRvs_cs = 0x10,             //Climate Status
        kRvs_es = 0x20,             //Electric Status
        kRvs_ps = 0x40,             //Pollution Status
        kRvs_ts = 0x80,             //Tem Status
        kRvs_cc = 0x100,            //Car Config
        kRvs_as = 0x200,            //additional VehicleStatus
        kRvs_msd = 0x400,           //msd
        kRvs_dtc = 0x800,           //Diagnostics
        kRvs_nf = 0x1000,           //Notification
        kRvs_as_ci = 0x2000,        //additionalVehicleStatus.confAndId
        kRvs_as_ew = 0x4000,        //additionalVehicleStatus.ecuWarningMessages
        kRvs_as_signals_key = 0x8000,   //additionalVehicleStatus.signals.<key>
        kRvs_all_config = 0xFFFF,
        //kRvs_csp_config_type_max
    };


    typedef struct RvsNewSignal
    {
        std::shared_ptr<fsm::Signal> signal;
        int status;
    } RvsNewSignal_t;

    class RvsSignalNewRequest : public fsm::Signal
    {
    public:
        RvsSignalNewRequest(): fsm::Signal(fsm::CCMTransactionId(), kSignal_NewRequest)
        {
        }
        std::string ToString()
        {
            return "RvsSignal_NewRequest";
        }
    };

    class RvsSignalProcessRequest : public fsm::Signal
    {
    public:
        RvsSignalProcessRequest(): fsm::Signal(fsm::CCMTransactionId(), kSignal_ProcessRequest)
        {
        }
        std::string ToString()
        {
            return "RvsSignal_ProcessRequest";
        }
    };

    class RvsSignalCollectVehicleState: public fsm::Signal
    {
    public:
        RvsSignalCollectVehicleState(): fsm::Signal(fsm::CCMTransactionId(), kSignal_CollectVehicleState)
        {
        }
        std::string ToString()
        {
            return "RvsSignal_CollectVehicleState";
        }
    };

    class RvsSignalReceiveVehicleState: public fsm::Signal
    {
    public:
        RvsSignalReceiveVehicleState(): fsm::Signal(fsm::CCMTransactionId(), kSignal_ReceiveVehicleState)
        {
        }
        std::string ToString()
        {
            return "RvsSignal_ReceiveVehicleState";
        }
    };

    class RvsSignalUploadVehicleState: public fsm::Signal
    {
    public:
        RvsSignalUploadVehicleState(): fsm::Signal(fsm::CCMTransactionId(), kSignal_UploadVehicleState)
        {
        }
        std::string ToString()
        {
            return "RvsSignal_UploadVehicleState";
        }
    };

    /**
    * \brief Creates a RVS transaction.
    */
    Rvs();

    /**
    * \brief Destroy a RVS transaction entity.
    */
    ~Rvs();

    //std::shared_ptr<GlyVdsRvsSignal> CreateRvsSignal(std::shared_ptr<fsm::Signal> signal);

    void InitializationVechicleStatus();
    /**
    * \brief Handle HandleRvsStartRequest signal.
    *        Expected in state kStart.
    *
    * \param[in] signal The signal.
    *
    * \return True if transaction is not yet finished, false if it is.
    */
    bool HandleRvsRequestEnqueue(std::shared_ptr<fsm::Signal> signal);

    bool HandleRvsRequestDequeue(RvsNewSignal_t *signal);

    bool HandleRvsNewRequest(std::shared_ptr<fsm::Signal> signal);

    bool HandleRvsProcessRequest(std::shared_ptr<fsm::Signal> signal);

    bool HandleRvsCollectVehicleState(std::shared_ptr<fsm::Signal> signal);

    bool HandleRvsReceiveVehicleState(std::shared_ptr<fsm::Signal> signal);

    bool HandleRvsUploadVehicleState(std::shared_ptr<fsm::Signal> signal);

    bool HandleRvsTimeout(std::shared_ptr<fsm::Signal> signal);

    bool HandlePosition(std::shared_ptr<fsm::Signal> signal);

    /**
    * \brief handle fsm::Signal::kCarMode, fsm::Signal::kCarUsageMode
    *
    * \param[in] received signals
    *
    * \return True if transaction is not yet finished, false if it is finished.
    */
    bool HandleCarModeSignal(std::shared_ptr<fsm::Signal> signal);
    /**
    * \brief Handle entering the state kDone.
    *        Will stop the transaction.
    *
    * \param[in] old_state The old state.
    * \param[in] new_state The new state.
    *
    * \return None.
    */
    void EnterRvsStartState(State old_state, State new_state);

    void EnterRvsExecuteState(State old_state, State new_state);

    void EnterRvsConfigState(State old_state, State new_state);

    void EnterRvsIdleState(State old_state, State new_state);

    void EnterRvsUploadState(State old_state, State new_state);

    void EnterRvsStopState(State old_state, State new_state);

    void MapRvsTransactionId(std::shared_ptr<fsm::Signal> signal)
    {
        MapTransactionId(signal->GetTransactionId().GetSharedCopy());
    }

    bool IsNewRvsSignal()
    {
        return m_isNewRvsSignal;
    }

    void SetNewRvsSignal()
    {
        m_isNewRvsSignal = true;
    }

    std::string StateToString(State state);

    void StartTimer(fsm::TimeoutTransactionId timerTickId, long timeInterval);

    void StopTimer(fsm::TimeoutTransactionId timerTickId);

    bool ReceiveTelmDshbData(std::shared_ptr<fsm::Signal> signal);

    bool ReceiveVehSoHCheckData(std::shared_ptr<fsm::Signal> signal);

    bool SendVehSoHCheckRequest();

    void UpdateConfigType(char* configString);

    bool CheckWhetherExistConfigType(int CurrentConfigTypeSum, int targetConfigType);

    bool RequestPosition(std::shared_ptr<fsm::TransactionId> tranId);

    /**
    * \brief Request Car mode and Usage mode from vehicle_comm
    *
    * \param none
    *
    * \return True if request success, false if error occured
    */
    bool RequestCarUsageMode();

    /**
    * \brief Request Config Value:RVS_INTERVAL
    *
    * \param none
    *
    * \return True if request success, false if error occured
    */
    bool UpdateConfiguration();

    /**
    * \brief Update trigger type if has new reason
    *
    * \param[in] reason
    *
    * \return none
    */
    void UpdateTriggerType(RVSTriggerTypesChangeReason reason);

private:

    enum ReqStates
    {
        kRvsStart = 0,
        kRvsIdle,
        kRvsConfig,
        kRvsExecute,
        kRvsUpload,
        kRvsStop,
        kRvsMax
    };

    enum SignalTypes
    {
        kSignal_NewRequest = 21000,
        kSignal_ProcessRequest,
        kSignal_CollectVehicleState,
        kSignal_ReceiveVehicleState,
        kSignal_UploadVehicleState,
        kSignal_Max
    };

    bool m_isNewRvsSignal = false;

    /**
    * \brief holds the session id for this pairing.
    */
    std::string pairing_session_id_;

    int timeoutTimes = 0;

    fsm::TimeoutTransactionId m_kRvs_period_tick_id;
    fsm::TimeoutTransactionId m_kRvs_Ipcommand_tick_id;
    fsm::TimeoutTransactionId m_kRvs_request_position_tick_id;

    std::shared_ptr<GlyVdsRvsSignal> m_newRvsSignal;

    std::list<RvsNewSignal_t> m_SignalRequestList;

    Rvs_BS_t m_bs;

    Rvs_AS_t m_as;

    Rvs_TS_t m_ts;

    long m_collectInterval;

    long m_uploadInterval;

    int m_needUploadTimes = 0;

    long m_configType;

    std::string m_sVin;
    vc::CarModeState m_carMod;
    vc::CarUsageModeState m_carUsgMod;
    RVSTriggerTypes m_triggerType;
    uint8_t old_srvTrig;

    fsm::LockStatus_Data m_ls;			//Lock Status
    fsm::MaintenanceStatus_Data m_ms;	//Maintenance Status
    fsm::RunningStatus_Data m_rs;		//Running Status
    fsm::ClimateStatus_Data m_cs;		//Climate Status
    fsm::ElectricStatus_Data m_es;		//Electric Status
    fsm::PollutionStatus_Data m_ps;     //Pollution Status
    fsm::AdditionalStatus_Data m_AdditionalStatus;   //Additional Status
    fsm::BasicVehicleStatus_Data m_basicVS;  //Basic Vehicle Status

//To move to common
    bool HandleVinNumberSignal(std::shared_ptr<fsm::Signal> signal);
    bool RequestVinNumber();

    void ComposeRVSSimuData(Rvs_TS_t  &ts);
    void ComposeRVSSimuData(Rvs_AS_DS_t  &as_ds);
    void ComposeRVSSimuData(Rvs_AS_MS_t  &as_ms);
    void ComposeRVSSimuData(Rvs_AS_RS_t  &as_rs);
    void ComposeRVSSimuData(Rvs_AS_DB_t  &as_db);
    void ComposeRVSSimuData(Rvs_AS_ES_t  &as_es);
    void ComposeRVSSimuData(Rvs_AS_CS_t  &as_cs);
    
    void setBasicVehicleStatus_Data(fsm::BasicVehicleStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setLockStatus_Data(fsm::LockStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setMaintenanceStatus_Data(fsm::MaintenanceStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setMaintenanceStatus_Data(fsm::MaintenanceStatus_Data &data_to, const ServiceReminder_Data &data_from);
    void setMaintenanceStatus_Data(fsm::MaintenanceStatus_Data &data_to, const TireDeflationDetection_Data &data_from);
    void setRunningStatus_Data(fsm::RunningStatus_Data &data_to, const TireDeflationDetection_Data &data_from);
    void setRunningStatus_Data(fsm::RunningStatus_Data &data_to, const JournalLog_Data &data_from);
    void setRunningStatus_Data(fsm::RunningStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setClimateStatus_Data(fsm::ClimateStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setElectricStatus_Data(fsm::ElectricStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setElectricStatus_Data(fsm::ElectricStatus_Data &data_to, const JournalLog_Data &data_from);
    
    void setPollutionStatus_Data(fsm::PollutionStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);
    void setAdditionalStatus_Data(fsm::AdditionalStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from);

    int32_t getMetricTemp(int32_t value, uint32_t unit);
    int32_t getMetricDistance(uint16_t input_data, uint32_t unit);
    int32_t getMetricSpeed(uint16_t input_data, uint32_t unit);
};

}// namespace volvo_on_call

#endif //RVS_TRANSACTION_H_
