///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_engine.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
//
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   07-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef GLY_REMOTE_START_ENGINE_H_
#define GLY_REMOTE_START_ENGINE_H_

#include <condition_variable>
#include <mutex>
#include <deque>
#include <thread>
#include <atomic>

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_res_signal.h"
#include "remote_start_common.h"

//add ASN1c message.
#include "usermanager_types.h"
//#include "VDServiceRequest.h"

#include "dlt/dlt.h"
DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

    /**
     * \brief:
     * this enum is ued for engine started by TSP or applications
     */
    typedef enum {
        StartEngineMode_No,     //*< engine has not started now, it's init by trigger.
        StartEngineMode_TSP,    //*< engine started by TSP server(moblie app etc.)
        StartEngineMode_APPS    //*< engine started by other applications those are in voc process.
    } StartEngineMode;

    typedef std::deque<std::shared_ptr<fsm::Signal>> RMT_SignalDeque;
    static const int RMT_RESPONSE_TIME_DEFAULTE = 15; //the default value of ers_response_time
    static const int RMT_RESPONSE_TIME_MAX = 255;     //the max value of ers_response_time
    static const int RMT_RESPONSE_TIME_MIN = 0;       //the min value of ers_response_time

    static const int RMT_RES_STATUS_UPDATE_TIME_DEFAULT = 15; // uint:seconds, default value of RMT_RES_STATUS_UPDATE_TIME
    static const int RMT_RES_STATUS_UPDATE_TIME_MAX = 127;   // the max value of RMT_RES_STATUS_UPDATE_TIME
    static const int RMT_RES_STATUS_UPDATE_TIME_MIN = 0;     // the min value of RMT_RES_STATUS_UPDATE_TIME

    static const uint8_t RMT_ERS_ACTIVE_VFC_ID = 18;        // res vfc-wakeup vfc-id
    static const int RMT_RES_VFC_WAKEUP_TIME_DEFAULT = 3;   // uint:seconds, default value of RMT_RES_VFC_WAKEUP_TIME
    static const int RMT_RES_VFC_WAKEUP_TIME_MAX = 127;     // the max value of RMT_RES_VFC_WAKEUP_TIME
    static const int RMT_RES_VFC_WAKEUP_TIME_MIN = 0;       // the min value of RMT_RES_VFC_WAKEUP_TIME

    static const int RMT_ERS_RESPONSE_VALID_TIME_DEFAULT = 1200; // response to TSP timeout default timer;
    static const int RMT_ERS_RESPONSE_VALID_TIME_MAX = 4096; // response to TSP timeout default timer;
    static const int RMT_ERS_RESPONSE_VALID_TIME_MIN = 0; // response to TSP timeout default timer;

    static const uint8_t RMT_VFC_SUCCESS = 0;   // operate VFC success

    static const std::string RMT_ERS_STARTING_OR_RUNNING = "Engine status is ErsStsStrtg or ErsStsRunng";
    static const std::string RMT_ERS_STOPED = "Engine status is ErsStsOff";

    typedef enum{
        AvlSts1_TmpNotAvl = 0x0,
        AvlSts1_PrmntNotAvl,
        AvlSts1_Avl
    } ImobRemMgrChk;

    typedef enum{
        ImobVehRemMgrSts_IdleRemMgrSts = 0x0,
        ImobVehRemMgrSts_ImobnRemMgrSts,
        ImobVehRemMgrSts_NoImobnRemMgrSts,
        ImobVehRemMgrSts_ImobnStrtDiRemMgrSts
    } ImobVehRemMgrSts;

    /**
     * \brief Res
     * a class that handles message(ers-request from TSP)
     */
    class RMT_Engine: public fsm::SmartTransaction
    {
        public:
            ////////////////////////////////////////////////////////////
            // @brief : GetInstance create singleton
            //
            // @return  static std::shared_ptr<RMT_Engine>
            // @author     uia93888, Dec 6, 2018
            ///////////////////////////////////////////////////////////
            /*static std::shared_ptr<RMT_Engine> GetInstance(){
                    static std::shared_ptr<RMT_Engine> Instance = std::make_shared<RMT_Engine>();
                return Instance;
            }*/

            ////////////////////////////////////////////////////////////
            // @brief : Res
            //
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            RMT_Engine();

            ////////////////////////////////////////////////////////////
            // @brief : ~Res
            //
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            ~RMT_Engine();

            ////////////////////////////////////////////////////////////
            // @brief : CreateGlyVdsResSignal create a GlyVdsResSignal-signal
            //
            // @param[in]  signal
            // @return     GlyVdsResSignal's smart-pointer
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            std::shared_ptr<GlyVdsResSignal> CreateGlyVdsResSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : AddTransactionIdToMap Insert signal's transaction-id to Map
            //
            // @param[in]  signal
            // @return     void
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            void AddTransactionIdToMap(std::shared_ptr<fsm::Signal> signal)
            {
                MapTransactionId(signal->GetTransactionId().GetSharedCopy());
            }

            /**
             * \brief Possible states of the remote_start_engine
             */
            typedef enum
            {
                kNew,                       // new request
                kWaitVFCResponse,           // wait vfc response
                kWaitCheckStrtApplSts,      // wait check ErsStrtAppStsInt
                kWaitPreConCheckResponse,   // precondition check
                kWaitOperateEngineResponse, // waiting operate-engine-request response
                kWaitGetVehicleState,       // Get Vehicle states
                kDone                       // done
            }States;

            /**
             * \brief create a structure, include ServiceCmd and duration parameters.
             */
            struct StartEngineStruct{
                ServiceCmd m_serviceCmd;  // the command to operate engine
                int duration;          // ERS-run-time, the Time in 10s increment, maxium 1 year;
            };

            ////////////////////////////////////////////////////////////
            // @brief : Start run thread(m_signal_thread)
            //
            // @return     void
            // @author     uia93888, Dec 10, 2018
            ///////////////////////////////////////////////////////////
            void Start(SendSingalCallBack cb);

            ////////////////////////////////////////////////////////////
            // @brief : create thread success
            //
            // @return     void
            // @author     uia93888, Dec 10, 2018
            ///////////////////////////////////////////////////////////
            bool CreateThreadSuccessFul();

        private:
            ////////////////////////////////////////////////////////////
            // @brief : Request_Preconditions
            //
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool Request_Preconditions();

            ////////////////////////////////////////////////////////////
            // @brief : Request_VFCActive
            //
            // @param[in]  bool: true(Active VFC), false(DeActive VFC)
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool Request_VFCActive(bool active_vfc);

            ////////////////////////////////////////////////////////////
            // @brief : Request_EngineStates
            //
            // @return     bool, true: wakeup vfc   false: unwakeup vfc
            // @author     uia93888, Oct 23, 2018
            bool Request_EngineStates();

            ////////////////////////////////////////////////////////////
            // @brief : StartEngine
            //
            // @param[in]  ersRequest
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool Request_StartEngine(std::shared_ptr<ErsRequest_t> ersRequest);

            ////////////////////////////////////////////////////////////
            // @brief : Request_OperateEngine
            //
            // @param[in]  ersRequest
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool Request_OperateEngine();

            ////////////////////////////////////////////////////////////
            // @brief : use random number to reponse Vuc
            //
            // @return     bool
            // @author     uia93888, Mar 14, 2019
            ///////////////////////////////////////////////////////////
            bool Request_RMTEngineSecurityReponse(std::shared_ptr<vc::Res_RMTEngineSecurityRandom> pRandom);

            ////////////////////////////////////////////////////////////
            // @brief : Request_VehicleStates
            //
            // @return     bool
            // @author     uia93888, Septempber 19, 2018
            ///////////////////////////////////////////////////////////
            bool Request_VehicleStates();

            ////////////////////////////////////////////////////////////
            // @brief : HandleGlyRequest
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool HandleGlyRequest(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleStartSignalRequest
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool HandleStartSignalRequest(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleVFCSignal
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool HandleVFCSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleUsageModeSIgnal
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool HandleUsageModeSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleCarModeSignal
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool HandleCarModeSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleEngineLevelSignal
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Mar 1, 2019
            ///////////////////////////////////////////////////////////
            bool HandleEngineLevelSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : Get the random numbers, and use them to response to Vuc
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Mar 14, 2019
            ///////////////////////////////////////////////////////////
            bool HandleRMTEngineSecurityRandomSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : Handle this signal, and get security authentication success or not
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Mar 14, 2019
            ///////////////////////////////////////////////////////////
            bool HandleRMTEngineSecurityResultSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleRMTEngineSignal
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
             bool HandleRMTEngineSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : HandleRMTCarStatesSignal
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 19, 2018
            ///////////////////////////////////////////////////////////
             bool HandleRMTVehicleStatesSignal(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : GlyTimeout
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            bool GlyTimeout(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : AddSignalDeque
            //
            // @param[in]  signal
            // @return     bool
            // @author     uia93888, Nov 23, 2018
            ///////////////////////////////////////////////////////////
            bool AddSignalDeque(std::shared_ptr<fsm::Signal> signal);

            ////////////////////////////////////////////////////////////
            // @brief : IsSignalDequeActive
            //
            // @param[in]  signal
            // @return     void
            // @author     uia93888, Nov 23, 2018
            ///////////////////////////////////////////////////////////
            bool IsSignalDequeActive(){ return !m_finalizing; }

        private:
            ////////////////////////////////////////////////////////////
            // @brief : SendError
            //
            // @param[in]  serviceResult
            // @return     void
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            void SendError(ErsResponse_t& serviceResult);

            ////////////////////////////////////////////////////////////
            // @brief : SendSuccessful
            //
            // @param[in]  std::shared_ptr<vc::ResRMTStatus>
            // @return     void
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            void SendSuccessful(std::shared_ptr<vc::ResRMTStatus> _pResRMTStatus);

            ////////////////////////////////////////////////////////////
            // @brief : SendStopSuccessful
            //
            // @param[in]  vc::CarUsageModeState
            // @return     void
            // @author     uia93888, Septempber 6, 2018
            ///////////////////////////////////////////////////////////
            void SendStopSuccessful(vc::CarUsageModeState state);

            ////////////////////////////////////////////////////////////
            // @brief : SignalLoop
            //
            // @param[in]  vc::CarUsageModeState
            // @return     void
            // @author     uia93888, Nov 23, 2018
            ///////////////////////////////////////////////////////////
            void HandleSignalDequeLoop();

            ////////////////////////////////////////////////////////////
            // @brief : StopLoop, stop the signal loop
            //
            // @param[in]  vc::CarUsageModeState
            // @return     void
            // @author     uia93888, Nov 23, 2018
            ///////////////////////////////////////////////////////////
            void StopLoop();

            ////////////////////////////////////////////////////////////
            // @brief : TerminateSignalThread
            //
            // @param[in]  vc::CarUsageModeState
            // @return     void
            // @author     uia93888, Nov 23, 2018
            ///////////////////////////////////////////////////////////
            void TerminateSignalThread();

            ////////////////////////////////////////////////////////////
            // @brief : NotifyHandleGeelyRequest
            //
            // @param[in]  vc::CarUsageModeState
            // @return     void
            // @author     uia93888, Nov 23, 2018
            ///////////////////////////////////////////////////////////
            void NotifyHandleGeelyRequest();

        private:
            ////////////////////////////////////////////////////////////
            // @brief : HandleWaitCheckStrtApplStsTimeout
            //
            // @return     void
            // @author     uia93888, Oct 27018
            ///////////////////////////////////////////////////////////
            void HandleWaitCheckStrtApplStsTimeout();

            ////////////////////////////////////////////////////////////
            // @brief : IsCurrentRequestVaild
            //
            // @return     bool true: vaild, false: unvalid
            // @author     uia93888, Oct 27018
            ///////////////////////////////////////////////////////////
            bool IsCurrentRequestVaild();

            ////////////////////////////////////////////////////////////
            // @brief : Handle_WaitOperateEngineResponseState 
            //          handle RMTEngineSignal, when RMT_Engine's state is kWaitOperateEngineResponse
            //
            // @param[in]  signal
            // @return     bool true: vaild, false: unvalid
            // @author     uia93888, Oct 27018
            ///////////////////////////////////////////////////////////
            bool Handle_WaitOperateEngineResponseState(std::shared_ptr<fsm::RMTEngineSignal> signal);

            std::map<States, std::string> states_map = {
                {kNew, "kNew"},
                {kWaitVFCResponse, "kWaitVFCResponse"},
                {kWaitCheckStrtApplSts, "kWaitCheckStrtApplSts"},
                {kWaitPreConCheckResponse, "kWaitPreConCheckResponse"},
                {kWaitOperateEngineResponse, "kWaitOperateEngineResponse"},
                {kWaitGetVehicleState, "kWaitGetVehicleState"},
                {kDone, "kDone"}
            };
            typedef std::map<States, std::string>::iterator ITER;
            ////////////////////////////////////////////////////////////
            // @brief : GetRMT_EngineState
            //
            // @return     string
            // @author     uia93888, Oct 27018
            ///////////////////////////////////////////////////////////
            std::string GetRMT_EngineState();

            vocpersistdatamgr::VocPersistDataMgr& m_vocpersist;     // get the value from persistence_dconfig_voc.xml
            std::shared_ptr<volvo_on_call::GlyVdsResSignal> m_pGlyVdsResSignal;
            vc::ErsCmd m_operateCmd;
            vc::ErsStrtApplSts m_ersStrtAppStsInt;
            fsm::TimeoutTransactionId m_timeoutTransactionId;
            fsm::TimeoutTransactionId m_TSPTimeoutId;
            bool m_UsageModeFlag;
            bool m_CarModeFlag;
            bool m_EngineStateFlag;
            bool m_EngineLevelFlag;
            bool m_VFCFlag; // when RMT_Engine to active VFC successful, it's value is true

            RMT_SignalDeque m_signalDeque;
            std::thread m_signal_thread;
            std::mutex m_signal_mutex;
            std::condition_variable m_signal_condition_variable;
            bool m_finalizing;
            /**
             * @brief false: current geely-request is unvalid, handle next request; true: current geely-request is valid,
             * @notes geely-reqeust means: engine start or engine stop request.
             */
            bool m_close_flag;
            std::atomic<bool> m_create_thread_success;

            StartEngineMode m_engine_mode;
            std::shared_ptr<volvo_on_call::AppEngineStatRequestSignal> m_pAppRequestSignal;
            SendSingalCallBack m_cb;
        };
}
#endif //GLY_REMOTE_START_ENGINE_H_
// \} end of addtogroup
