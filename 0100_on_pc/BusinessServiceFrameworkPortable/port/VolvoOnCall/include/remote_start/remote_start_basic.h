///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_basic.h
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   12-Nov-2018
///////////////////////////////////////////////////////////////////
#ifndef GLY_APPLICATION_REMOTE_START_BASICCLASS_H_
#define GLY_APPLICATION_REMOTE_START_BASICCLASS_H_

#include <iostream>
#include "remote_start_common.h"

#include "voc_framework/transactions/smart_transaction.h" 
#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rce_signal.h"
#include "signals/geely/gly_vds_res_signal.h"

#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

//add ASN1c message.
#include "usermanager_types.h"


namespace volvo_on_call
{
    class RMTStartBasic: public fsm::SmartTransaction
    {
    public:
        typedef struct {
            bool m_UsageModeSignalFlage;
            bool m_CarModeSignalFlags;
            bool m_EngineLevelFlags;
            bool m_EngineStatesFlags;
        } Flags;

        typedef enum{
            kNew = 0,
            kCheckPreCondition,  //*1
            kOperateEgnine,  //*2
            kHandleRequest,  //*3
            kGetStates,  //*4
            kWaitingStop,  //*5
            kDone,  //*6
            kTimeoutStop // <* it's used for stopping remote climatization, when running timer is timeout
        } States;
    public:
        ////////////////////////////////////////////////////////////
        // @brief : RMTStartCommon
        //
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        RMTStartBasic();

        ////////////////////////////////////////////////////////////
        // @brief : ~RMTStartCommon
        //
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        virtual ~RMTStartBasic();

        ////////////////////////////////////////////////////////////
        // @brief : init data
        //
        // @param[in]  SendSingalCallBack
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        void BasicInit(SendSingalCallBack SendSignal_cb);

        ////////////////////////////////////////////////////////////
        // @brief : Create GlyVdsRceSignal signal
        //
        // @author     uia93888, Jan 23, 2019
        ///////////////////////////////////////////////////////////
        std::shared_ptr<volvo_on_call::GlyVdsRceSignal> CreateGlyVdsRceSignal(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : AddTransactionIdToMap Insert signal's transaction-id to Map
        //
        // @param[in]  signal
        // @return     void
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        void AddTransactionIdToMap(std::shared_ptr<fsm::Signal> signal)
        {
            MapTransactionId(signal->GetTransactionId().GetSharedCopy());
        }

        ////////////////////////////////////////////////////////////
        // @brief : Request_PreConditiones
        //
        // @param[in]  service_type
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Request_PreCondition(ServiceType service_type);

        ////////////////////////////////////////////////////////////
        // @brief : Request_VFCActive
        //
        // @param[in]  keep_wake
        // @author     uia93888, Jan 30, 2019
        ///////////////////////////////////////////////////////////
        bool Request_VFCActive(bool keep_wake);

        ////////////////////////////////////////////////////////////
        // @brief : Request_StartEngine, to start engine
        //
        // @return     bool
        // @author     uia93888, Jan 23, 2019
        ///////////////////////////////////////////////////////////
        bool Request_StartEngine();

        ////////////////////////////////////////////////////////////
        // @brief : Request_DelayEnginneRunningTimer, delay engine running time
        //
        // @return     bool
        // @author     uia93888, Jan 23, 2019
        ///////////////////////////////////////////////////////////
        bool Request_DelayEnginneRunningTimer();

        ////////////////////////////////////////////////////////////
        // @brief : handle the request signal from TSP server
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Jan 23, 2019
        bool Handle_RequestSignal(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : get usage mode status
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_UsageModeSignal(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : get car mode status
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_CarModeSignal(std::shared_ptr<fsm::Signal> signal);

        ///////////////////////////////////////////////////////////
        // @brief : handle signal get engine level
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_EngineLevelSignal(std::shared_ptr<fsm::Signal> signal);

        ///////////////////////////////////////////////////////////
        // @brief : get engine status(running/off/strtg) and engine is in remote or not
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_EngineStatesSignal(std::shared_ptr<fsm::Signal> signal);

        ///////////////////////////////////////////////////////////
        // @brief : Handle_VFCSignal
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Jan 30, 2019
        ///////////////////////////////////////////////////////////
        bool Handle_VFCSignal(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : get start engine successful or failed
        //
        // @param[in]  vfc_id
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_StartEngineSignal(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : handle Delay Engine runing time signal
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_DelayEngineSignal(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : GetCarModeSignalFlags
        //
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool GetUsageModeSignalFlage() { return m_flags.m_UsageModeSignalFlage; }

        ////////////////////////////////////////////////////////////
        // @brief : GetCarModeSignalFlags
        //
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool GetCarModeSignalFlags() { return m_flags.m_CarModeSignalFlags; }

        ////////////////////////////////////////////////////////////
        // @brief : InitFlagsStruct
        //
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        void InitFlagsStruct();

        ////////////////////////////////////////////////////////////
        // @brief : handle timeot
        //
        // @param[in] signal
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_GlyTimeout(std::shared_ptr<fsm::Signal> signal);

        ////////////////////////////////////////////////////////////
        // @brief : Handle_DoneState
        //
        // @param[in] signal
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        bool Handle_DoneState();

        ////////////////////////////////////////////////////////////
        // @brief : handle Delay Engine runing time signal
        //
        // @param[in[  Signal
        // @return     bool
        // @author     uia93888, Nov 12, 2018
        ///////////////////////////////////////////////////////////
        virtual bool Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal) = 0;

        ////////////////////////////////////////////////////////////
        // @brief : send response to TSP
        //
        // @param[in]  RMT_ClimatizationResponse_t
        // @author     uia93888, Jan 24, 2019
        ///////////////////////////////////////////////////////////
        virtual bool SendResponseToTSP(RMT_ClimatizationResponse_t response) = 0;

        ////////////////////////////////////////////////////////////
        // @brief : send request to Vuc to start/stop remote start applications
        //          such as: climatization, steer wheel heat, seat heat, seat 
        //          ventilation
        //
        // @param[in]  RMT_ClimatizationRequest_t
        // @author     uia93888, Jan 24, 2019
        ///////////////////////////////////////////////////////////
        virtual bool SendRequest(std::shared_ptr<RMT_ClimatizationRequest_t> request) = 0;

    protected:
        ////////////////////////////////////////////////////////////
        // @brief : HandleFailed
        //
        // @param[in]  RMT_ClimatizationRequest_t
        // @author     uia93888, Jan 24, 2019
        ///////////////////////////////////////////////////////////
        void HandleFailed(ASN_ErrorCode error_code);

    private:
        ////////////////////////////////////////////////////////////
        // @brief : decides to start engine or delay engine running time
        //
        // @param[in]  RMT_ClimatizationRequest_t
        // @author     uia93888, Jan 24, 2019
        ///////////////////////////////////////////////////////////
        bool OperateEingne();

    protected:
        vc::IGeneral& m_igen;
        SendSingalCallBack m_callback;
        fsm::TimeoutTransactionId m_TSPTimeoutId;
        fsm::TimeoutTransactionId m_timeoutTransactionId;
        fsm::TimeoutTransactionId m_RunngTransactionId;
        std::shared_ptr<volvo_on_call::GlyVdsRceSignal> m_pGlyVdsRceSignal;
        OperateCommands m_cmd;
        vocpersistdatamgr::VocPersistDataMgr& m_vocpersist;
        unsigned int m_VFCTimeout;
        unsigned int m_TSPTimeout;
        unsigned int m_RequestTimeout;

    private:
        Flags m_flags;
        vc::ErsStrtApplSts m_ErsStartApplSts;
        vc::EngStlWdSts m_EngStlWdSts;
    };

} //volvo_on_call

#endif //GLY_APPLICATION_REMOTE_START_BASICCLASS_H_