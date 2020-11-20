///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_doors_lock_transaction.cc
//	This file handles the Remote Control statemachine for application.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	06-Aug-2018
///////////////////////////////////////////////////////////////////
#include "remote_ctrl/remote_ctrl_common.h"
#include "signals/bcc_cl_001_signal.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc_framework/signal_adapt/rmc_signal_adapt.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{
#define USAGEMODE_ENABLE true
#define VFC_ENABLE true
    /************************************************************/
    // @brief :Constructs signal.
    // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    RemoteCtrlCommon::RemoteCtrlCommon(std::shared_ptr<fsm::Signal> initial_signal)
        : fsm::SmartTransaction(kNoReq)
        , m_vocpersist(vocpersistdatamgr::VocPersistDataMgr::GetInstance())
        , m_bCarMode(false)
        , m_bUsageMode(false)
        , m_bUsageModePre(false)
        , m_vfcIndex(0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        m_signal = std::dynamic_pointer_cast<GlyVdsRmcSignal>(initial_signal);

        if (initial_signal)
        {
            m_iType = initial_signal->GetSignalType();
            UpdateLocalConfig(m_iType);
        }
       
        StateFunction noReq =
            std::bind(&RemoteCtrlCommon::EnterNoReq,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        StateFunction vfcStarting =
            std::bind(&RemoteCtrlCommon::EnterVFCStarting,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        StateFunction requesting =
            std::bind(&RemoteCtrlCommon::EnterRequesting,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        StateFunction done =
            std::bind(&RemoteCtrlCommon::EnterDone,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        SignalFunction handle_req_signal =
            std::bind(&RemoteCtrlCommon::HandleReqSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_start_signal =
            std::bind(&RemoteCtrlCommon::HandleStartSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_timeout_signal =
            std::bind(&RemoteCtrlCommon::HandleTimeoutSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_vfc_signal =
            std::bind(&RemoteCtrlCommon::HandleVFCSignal,
            this,
            std::placeholders::_1);
                
        SignalFunction handle_carmode_signal =
            std::bind(&RemoteCtrlCommon::HandleCarModeSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_rtmsts_signal =
            std::bind(&RemoteCtrlCommon::HandleRMTCarStatesSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_telmdasb_signal =
            std::bind(&RemoteCtrlCommon::HandleTelmDshbSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_position =
            std::bind(&RemoteCtrlCommon::HandlePosition,
            this,
            std::placeholders::_1);
        
        SignalFunction handle_result_signal =
            std::bind(&RemoteCtrlCommon::HandleResultSignal,
            this,
            std::placeholders::_1);
        
        StateMap state_map =
            {{kNoReq,
                {noReq,                         // state function
                {{fsm::kVDServiceRdl, handle_req_signal},
                 {fsm::kVDServiceRdu, handle_req_signal},
                 {fsm::kVDServiceRws, handle_req_signal},
                 {fsm::kVDServiceRpp, handle_req_signal},
                 {fsm::kVDServiceRhl, handle_req_signal},
                 {VocInternalSignalTypes::kRmcStart, handle_start_signal},
                 {fsm::Signal::kTimeout, handle_timeout_signal},
                 {fsm::Signal::kRMTCarStatesSignal, handle_rtmsts_signal},
                 {fsm::Signal::kCarMode, handle_carmode_signal},
                 {fsm::Signal::kCarUsageMode, handle_carmode_signal}},
                {kVFCStarting, kRequesting, kDone}}},  // valid transitions
            {kVFCStarting,
                {vfcStarting,                   // state function
                {{fsm::kVDServiceRdl, handle_req_signal},
                 {fsm::kVDServiceRdu, handle_req_signal},
                 {fsm::kVDServiceRws, handle_req_signal},
                 {fsm::kVDServiceRpp, handle_req_signal},
                 {fsm::kVDServiceRhl, handle_req_signal},
                 {fsm::Signal::kTimeout, handle_timeout_signal},
                 {fsm::Signal::kRMTCarStatesSignal, handle_rtmsts_signal},
//                 {fsm::Signal::kTelmDshb, handle_telmdasb_signal},
                 {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                 {fsm::Signal::kCarMode, handle_carmode_signal},
                 {fsm::Signal::kVFCActivateSignal, handle_vfc_signal},
                 {fsm::Signal::kCarUsageMode, handle_carmode_signal}},
                {kRequesting, kDone}}},        // valid transitions
            {kRequesting,
                {requesting,                    // state function
                {{fsm::kVDServiceRdl, handle_req_signal},
                 {fsm::kVDServiceRdu, handle_req_signal},
                 {fsm::kVDServiceRws, handle_req_signal},
                 {fsm::kVDServiceRpp, handle_req_signal},
                 {fsm::kVDServiceRhl, handle_req_signal},
                 {fsm::Signal::kRMTCarStatesSignal, handle_rtmsts_signal},
//                 {fsm::Signal::kTelmDshb, handle_telmdasb_signal},
                 {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                 {fsm::Signal::kHornNLight,             handle_result_signal},
                 {fsm::Signal::kDoorLockStatusSignal,   handle_result_signal},
                 {fsm::Signal::kWinOpenStatusSignal,    handle_result_signal},
                 {fsm::Signal::kPMLvlStatusSignal,      handle_result_signal},
                 {fsm::Signal::kTimeout,                handle_timeout_signal}},
                {kDone}}},                      // valid transitions
            {kDone,
                {done,                          // state function
                SignalFunctionMap(),            // signal function map
                {}}}};                          // valid transitions
        

        SetStateMap(state_map);
        
        MapSignalType(VocInternalSignalTypes::kRmcStart);
        MapSignalType(VocInternalSignalTypes::kRmtCancel);
        MapSignalType(fsm::Signal::kDoorLockStatusSignal);
        MapSignalType(fsm::Signal::kWinOpenStatusSignal);
        MapSignalType(fsm::Signal::kPMLvlStatusSignal);
        MapSignalType(fsm::Signal::kRMTCarStatesSignal);
        MapSignalType(fsm::Signal::kCarMode);
        MapSignalType(fsm::Signal::kCarUsageMode);
//        MapSignalType(fsm::Signal::kTelmDshb);
        MapSignalType(fsm::Signal::kGNSSPositionDataSignal);

        if (initial_signal)
        {
            MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        }

    }
    
    /************************************************************/
    // @brief :enter function, will be called when enter kNoReq state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void RemoteCtrlCommon::EnterNoReq(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
    }

    /************************************************************/
    // @brief :enter function, will be called when enter kVFCStarting state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void RemoteCtrlCommon::EnterVFCStarting(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);

        RemoveTimeout(m_ttidUsage);

        bool ret = true;
        if (m_vfcIndex < m_vfcGroup.size())
        {
            ret = RequestVFCActivate(m_vfcGroup[m_vfcIndex], true);
        }

        if (ret)
        {
            if (m_VFCTimeout > 0)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon: start VFC timer: %i s", m_VFCTimeout);

                m_ttidVFC = RequestTimeout(static_cast<std::chrono::seconds>(m_VFCTimeout), false);
                MapTransactionId(m_ttidVFC.GetSharedCopy());
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon, failed to request VFC start");
            fsm::RemoteCtrlBasicResult result;
            result.operationSucceeded = false;
            result.errorCode = ErrorCode_executionFailure;
            SendResponse(result);
            SetState(kDone);
        }
    }

    /************************************************************/
    // @brief :enter function, will be called when enter kRequesting state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void RemoteCtrlCommon::EnterRequesting(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        fsm::RemoteCtrlBasicResult result;

        RemoveTimeout(m_ttidVFC);

        // Send request to VehicleComm
        bool ret = SendRequest();

        // Send success, start timer and waiting the response
        if (ret)
        {
            if (m_execTimeout > 0)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon: start exec timer: %i s", m_execTimeout);

                m_ttidExec = RequestTimeout(static_cast<std::chrono::seconds>(m_execTimeout), false);
                MapTransactionId(m_ttidExec.GetSharedCopy());
            }
        }
        // Send failed, end the state machine and send response
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, failed to request to execute command.");
            result.operationSucceeded = false;
            result.errorCode = ErrorCode_executionFailure;
            SendResponse(result);
            SetState(kDone);
        }
    }

    /************************************************************/
    // @brief :enter function, will be called when enter kDone state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void RemoteCtrlCommon::EnterDone(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        for (uint8_t i = 0; i < m_vfcGroup.size(); i++)
        {
            RequestVFCActivate(m_vfcGroup[i], false);
        }
    }

    /************************************************************/
    // @brief :handle remote control signals
    //         such as:fsm::kVDServiceRdl, kVDServiceRdu, kVDServiceRtl, kVDServiceRtu, kVDServiceRws, kVDServiceRpp, kVDServiceRhl
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleReqSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        switch (GetState())
        {
        case kNoReq:
        {
            std::shared_ptr<fsm::Signal> startSignal = std::make_shared<RemoteCtrlStartSignal>();
            EnqueueSignal(startSignal);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "start usage mode timer: %i s", m_usageModeTimeout);
            m_ttidUsage = RequestTimeout(static_cast<std::chrono::seconds>(m_usageModeTimeout), false);
            MapTransactionId(m_ttidUsage.GetSharedCopy());
            break;
        }
        case kVFCStarting:
            // TODO: 
            break;
        case kRequesting:
            // TODO:
            break;
        case kDone:
            //Nothing to do
            break;
        default:
            break;
        }

        return true;
    }

    /************************************************************/
    // @brief :handle VocInternalSignalTypes::kRmcStart
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleStartSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        switch (GetState())
        {
        case kNoReq:
            RequestCarMode();
            RequestUsageMode();
            RequestRMTStatus();
            RequestPosition();

            break;
        case kVFCStarting:
            //Nothing to do
            break;
        case kRequesting:
            //Nothing to do
            break;
        case kDone:
            //Nothing to do
            break;
        default:
            break;
        }

        return true;
    }

    /************************************************************/
    // @brief :handle fsm::Signal::kVFCActivateSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleVFCSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        bool ret = true;

        if (kVFCStarting == GetState())
        {
            std::shared_ptr<fsm::VFCActivateSignal> pVFCActiveSignal = std::static_pointer_cast<fsm::VFCActivateSignal>(signal);
            vc::ReturnValue vc_return_value = pVFCActiveSignal->GetVcStatus();

            if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon::Failed to request VFC Active(%d). Try again.", vc_return_value);
                usleep(500000);
                RequestVFCActivate(m_vfcGroup[m_vfcIndex], true);
            }
            else
            {
                std::shared_ptr<vc::ResVFCActivate> pVFCResult = pVFCActiveSignal->GetData();
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon: VenderID: %d, %d", pVFCResult->id, pVFCResult->resule);

                if (pVFCResult->id == m_vfcGroup[m_vfcIndex])
                {
                    if (0 == pVFCResult->resule) // success
                    {
                        if (++m_vfcIndex < m_vfcGroup.size())
                        {
                            RequestVFCActivate(m_vfcGroup[m_vfcIndex], true);
                        }
                        else
                        {
                            if (m_bCarMode && m_bUsageMode)
                            {
                                SetState(kRequesting);
                            }
                        }
                    }
                    else
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon, VFC Active execute failed.");
                        fsm::RemoteCtrlBasicResult result;
                        result.operationSucceeded = false;
                        result.errorCode = ErrorCode_executionFailure;
                        SendResponse(result);
                        SetState(kDone);
                    }
                }
                else
                {
                    //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon: VenderID: %d", pVFCResult->id == GetVenderID());
                }
            }
        }

        return ret;
    }   

    /************************************************************/
    // @brief :handle fsm::Signal::kRMTCarStatesSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleRMTCarStatesSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        bool ret = true;

        std::shared_ptr<fsm::RMTCarStatesSignal> pRMTCarStatesSignal = std::static_pointer_cast<fsm::RMTCarStatesSignal>(signal);
        vc::ReturnValue vc_return_value = pRMTCarStatesSignal->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request to VehicleComm, aborting transaction.");
        }

        std::shared_ptr<vc::ResRMTStatus> pResRMTStatus = pRMTCarStatesSignal->GetData();
        if(pResRMTStatus)
        {         
            m_RMTStatus.doorOpenStatusDriver = pResRMTStatus->doorDrvrSts;
            m_RMTStatus.doorOpenStatusPassenger = pResRMTStatus->doorPassSts;
            m_RMTStatus.doorOpenStatusDriverRear = pResRMTStatus->doorLeReSts;
            m_RMTStatus.doorOpenStatusPassengerRear = pResRMTStatus->doorRiReSts;
            
            m_RMTStatus.doorLockStatusDriver = pResRMTStatus->doorDrvrLockSts;
            m_RMTStatus.doorLockStatusPassenger = pResRMTStatus->doorPassLockSts;
            m_RMTStatus.doorLockStatusDriverRear = pResRMTStatus->doorLeReLockSrs;
            m_RMTStatus.doorLockStatusPassengerRear = pResRMTStatus->doorRiReLockSts;
            
            m_RMTStatus.trunkOpenStatus = pResRMTStatus->trSts;
            m_RMTStatus.trunkLockStatus = pResRMTStatus->trLockSts;
            
            m_RMTStatus.centralLockingStatus = pResRMTStatus->lockgCenSts;
            m_RMTStatus.hoodSts = pResRMTStatus->hoodSts;
            m_RMTStatus.lockgCenStsForUsrFb = pResRMTStatus->lockgCenStsForUsrFb;

            m_RMTStatus.winStatusDriver = TransWinStatusToSignalType(pResRMTStatus->winPosnStsAtDrv);
            m_RMTStatus.winStatusPassenger = TransWinStatusToSignalType(pResRMTStatus->winPosnStsAtPass);
            m_RMTStatus.winStatusDriverRear = TransWinStatusToSignalType(pResRMTStatus->winPosnStsReLe);
            m_RMTStatus.winStatusPassengerRear = TransWinStatusToSignalType(pResRMTStatus->winPosnStsReRi);

            m_RMTStatus.sunroofOpenStatus = pResRMTStatus->sunroofPosnSts;
            
            m_RMTStatus.winPosDriver = pResRMTStatus->winPosnStsAtDrv;
            m_RMTStatus.winPosPassenger = pResRMTStatus->winPosnStsAtPass;
            m_RMTStatus.winPosDriverRear = pResRMTStatus->winPosnStsReLe;
            m_RMTStatus.winPosPassengerRear = pResRMTStatus->winPosnStsReRi;

            m_RMTStatus.ventilateStatus = pResRMTStatus->ventilateStatus;

            m_RMTStatus.interiorPM25 = pResRMTStatus->interiorPM25Vlu;
            m_RMTStatus.exteriorPM25 = pResRMTStatus->exteriorPM25Vlu;
            m_RMTStatus.interiorPM25Level = pResRMTStatus->interiorPM25Lvl;
            m_RMTStatus.exteriorPM25Level = pResRMTStatus->exteriorPM25Lvl;
        }
        else
        {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::HandleRMTCarStatesSignal,get car states failed");
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::vehSpdIndcd = %d", m_RMTStatus.vehSpdIndcd);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::vehSpdIndcdQly = %d", m_RMTStatus.vehSpdIndcdQly);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorOpenStatusDriver = %d", m_RMTStatus.doorOpenStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorOpenStatusPassenger = %d", m_RMTStatus.doorOpenStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorOpenStatusDriverRear = %d", m_RMTStatus.doorOpenStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorOpenStatusPassengerRear = %d", m_RMTStatus.doorOpenStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorLockStatusDriver = %d", m_RMTStatus.doorLockStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorLockStatusPassenger = %d", m_RMTStatus.doorLockStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorLockStatusDriverRear = %d", m_RMTStatus.doorLockStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::doorLockStatusPassengerRear = %d", m_RMTStatus.doorLockStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::trunkOpenStatus = %d", m_RMTStatus.trunkOpenStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::trunkLockStatus = %d", m_RMTStatus.trunkLockStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::centralLockingStatus = %d", m_RMTStatus.centralLockingStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::hoodSts = %d", m_RMTStatus.hoodSts);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::lockgCenStsForUsrFb = %d", m_RMTStatus.lockgCenStsForUsrFb);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winStatusDriver = %d", m_RMTStatus.winStatusDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winStatusPassenger = %d", m_RMTStatus.winStatusPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winStatusDriverRear = %d", m_RMTStatus.winStatusDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winStatusPassengerRear = %d", m_RMTStatus.winStatusPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::sunroofOpenStatus = %d", m_RMTStatus.sunroofOpenStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winPosDriver = %d", m_RMTStatus.winPosDriver);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winPosPassenger = %d", m_RMTStatus.winPosPassenger);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winPosDriverRear = %d", m_RMTStatus.winPosDriverRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::winPosPassengerRear = %d", m_RMTStatus.winPosPassengerRear);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::ventilateStatus = %d", m_RMTStatus.ventilateStatus);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::interiorPM25 = %d", m_RMTStatus.interiorPM25);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::exteriorPM25 = %d", m_RMTStatus.exteriorPM25);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::interiorPM25Level = %d", m_RMTStatus.interiorPM25Level);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "HandleRMTCarStatesSignal::exteriorPM25Level = %d", m_RMTStatus.exteriorPM25Level);
        
        return ret;
    } 

    /************************************************************/
    // @brief :handle fsm::Signal::kTelmDshb
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleTelmDshbSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        /* please use new data struct
        std::shared_ptr<fsm::TelmDshbSiganl> telmDshbSignal = std::static_pointer_cast<fsm::TelmDshbSiganl>(signal);
        const TelmDshb_t data = telmDshbSignal->GetPayload();

        if (1 == data.arg_vehSpdIndcdveSpdIndcdUnit) // 1:mph
        {
            // change to kmph
            m_RMTStatus.vehSpdIndcd = data.arg_vehSpdIndcdvehSpdIndcd1 * 1.609;
        }
        else //0:kmph, others
        {
            m_RMTStatus.vehSpdIndcd = data.arg_vehSpdIndcdvehSpdIndcd1;
        }

        // 0 UndefindDataAccur; 1 TmpUndefdData; 2 DataAccurNotWithinSpcn; 3 AccurData
        if (3 == data.arg_vehSpdIndcdQly)
        {
            m_RMTStatus.vehSpdIndcdQly = 0x0;   // valid
        }
        else
        {
            m_RMTStatus.vehSpdIndcdQly = 0x1;   // invalid
        }

        */

        return true;

    }

    /************************************************************/
    // @brief :handle fsm::Signal::kGNSSPositionDataSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandlePosition(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        vpom::GNSSData gnss_position;
        std::shared_ptr<fsm::VpomGNSSPositionDataSignal> position_signal = std::static_pointer_cast<fsm::VpomGNSSPositionDataSignal>(signal);
        gnss_position = position_signal->GetPayload();

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "position.longlat.latitude:%d.\n", gnss_position.position.longlat.latitude);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "position.longlat.longitude:%d.\n", gnss_position.position.longlat.longitude);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "position.altitude:%d.\n", gnss_position.position.altitude);
        m_RMTStatus.position.latitude = gnss_position.position.longlat.latitude; // Latitude
        m_RMTStatus.position.longitude = gnss_position.position.longlat.longitude; // Longitude
        m_RMTStatus.position.altitude = gnss_position.position.altitude; // Altitide from mean sea level
        m_RMTStatus.position.posCanBeTrusted = 1;
        m_RMTStatus.position.carLocatorStatUploadEn = 1;
        m_RMTStatus.position.marsCoordinates = 0;        
        return true;
    }
        
    /************************************************************/
    // @brief :handle fsm::Signal::kCarMode, fsm::Signal::kCarUsageMode
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleCarModeSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        bool ret = true;
        fsm::RemoteCtrlBasicResult result;

        switch (signal->GetSignalType())
        {
        case fsm::Signal::kCarMode:
            {
                ret = CheckCarModePrecondition(signal);
                break;
            }
        case fsm::Signal::kCarUsageMode:
            {
                ret = CheckCarUsageModePrecondition(signal);
                break;
            }
        default:
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, received unexpected VehicleCommSignal, ignoring.");
        }
        if (ret)
        {
            switch (GetState())
            {
            case kNoReq:
            {
                if (m_bCarMode && m_bUsageModePre)
                {
                    SetState(kVFCStarting);
                }
                break;
            }
            case kVFCStarting:
            {
                //when we have all the car state data evaluated positively, execute
                if (m_bCarMode && m_bUsageMode)
                {
                    SetState(kRequesting);
                }

                break;
            }
            default:
                break;
            }
        }
 
        return true;
    }

    /************************************************************/
    // @brief :handle result signals
    // such as fsm::Signal::kHornNLight, kDoorLockStatusSignal, kWinOpenStatusSignal, kPMLvlStatusSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleResultSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s, status = %d", __FUNCTION__, GetState());
        bool ret = false;
        switch (GetState())
        {
        case kNoReq:
            break;
        case kVFCStarting:
            RequestRMTStatus();
            break;
        case kRequesting:
            ret = HandleResult(signal);
            break;
        case kDone:
            //Nothing to do
            break;
        default:
            break;
        }

        if (ret)
        {
            RemoveTimeout(m_ttidExec);
            SetState(kDone);
        }

        return ret;

    }

    /************************************************************/
    // @brief :handle timeout signals
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::HandleTimeoutSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s, status = %d", __FUNCTION__, GetState());
        bool ret = true;
        fsm::RemoteCtrlBasicResult result;

        if (signal->GetTransactionId() == m_ttidExec)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon, executing timeout occured");
            ret = false;
            result.operationSucceeded = false;
            result.errorCode = ErrorCode_timeout;
            SendResponse(result, m_RMTStatus);
            SetState(kDone);
        }
        else if (signal->GetTransactionId() == m_ttidVFC)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, VFC Start timeout occured");
            //when we have all the car state data evaluated positively, execute
            if (m_bCarMode && m_bUsageMode)
            {
                SetState(kRequesting);
            }
            else
            {
                ret = false;
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_timeout;
                SendResponse(result);
                SetState(kDone);
            }
        }
        else if (signal->GetTransactionId() == m_ttidUsage)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, req usage mode timeout occured");
            //when we have all the car state data evaluated positively, execute
            if (m_bCarMode && m_bUsageModePre)
            {
                SetState(kVFCStarting);
            }
            else if (m_bCarMode && m_bUsageMode)
            {
                SetState(kRequesting);
            }
            else
            {
                ret = false;
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_timeout;
                SendResponse(result);
                SetState(kDone);
            }
        }
        else
        {
            //Nothing to do
        }

        return ret;
    }

    /************************************************************/
    // @brief :check car mode
    // @param[in]  received signals
    // @return     True if checked ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::CheckCarModePrecondition(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        
        bool ret = false;
        
        if (signal)
        {
            // cast to the correct message class:
            std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);

            // ToDo: check for NULL pointer missing.
            vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();

            if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon::%s, Failed to issue request to VehicleComm", __FUNCTION__);
            }
            else
            {
                vc::CarModeState car_mode_return = car_mode_response->GetData()->carmode;

                //car mode must be normal, otherwise MapRequest must be denied
                if (car_mode_return == vc::CAR_NORMAL)
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon, car mode OK:%d", car_mode_return);
                    m_bCarMode = true;
                    ret = true;
                }
                else
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon, car mode NOK:%d", car_mode_return);
                }
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon, received Invaild Signal");
        }

        return ret;
    }

    /************************************************************/
    // @brief :check car usage mode
    // @param[in]  received signals
    // @return     True if checked ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::CheckCarUsageModePrecondition(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        
        bool ret = false;
        
        if (signal)
        {
            // cast to the correct message class:
            std::shared_ptr<fsm::CarUsageModeSignal> car_usage_mode_response = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);

            // ToDo: check for NULL pointer missing.
            vc::ReturnValue vc_return_value = car_usage_mode_response->GetVcStatus();
            if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon::%s, Failed to issue request to VehicleComm", __FUNCTION__);
            }
            else
            {
                vc::CarUsageModeState car_usage_mode_return = car_usage_mode_response->GetData()->usagemode;

                //car usage mode must be either of the following: abandoned, inactive, conveninence or active
                //otherwise MapRequest must be denied
                switch (car_usage_mode_return)
                {
                case vc::CAR_ABANDONED:
                    m_bUsageModePre = true;
                    ret = true;
                    break;
                case vc::CAR_INACTIVE:
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon, car usage mode OK:%d", car_usage_mode_return);
                    m_bUsageModePre = true;
                    m_bUsageMode = true;
                    ret = true;
                    break;
                }
                default:
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon, car usage mode NOK:%d", car_usage_mode_return);
                    break;
                }
                }
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlCommon, received Invaild Signal");
        }

        return ret;
    }

    /************************************************************/
    // @brief :request to VFC activate
    // @param[in]  true:active; false:deactive
    // @return     True if request ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::RequestVFCActivate(uint8_t id, bool bActivate)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s, id = %d:%d", __FUNCTION__, id, bActivate);
        
        bool ret = true;
#ifdef VFC_ENABLE
        vc::ReqVFCActivate request;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);

        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        request.id = id;
        request.type = bActivate;
        //request vfc activate/deactivate
        if (igen.Request_VFCActivate(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                "RemoteCtrlCommon, requests for VFC Activate.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, failed to request VFC Activate");
            ret = false;
        }
#endif
        return ret;
    }

    /************************************************************/
    // @brief :request car mode
    // @param[in]  
    // @return     True if request ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::RequestCarMode()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        
        bool ret = true;
#ifdef USAGEMODE_ENABLE
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);

        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        //request car mode
        if (igen.Request_CarMode(vehicle_comm_request_id->GetId()) == vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                "RemoteCtrlCommon, requests for car mode sent to VehicleComm.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, failed to request car mode from VehicleComm");
            ret = false;
        }
#endif
        return ret;
    }

    /************************************************************/
    // @brief :request car usage mode
    // @param[in]  
    // @return     True if request ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::RequestUsageMode()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
       
        bool ret = true;
#ifdef USAGEMODE_ENABLE
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);

        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        //request usage mode
        if (igen.Request_CarUsageMode(vehicle_comm_request_id->GetId()) == vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                "RemoteCtrlCommon, requests for usage mode sent to VehicleComm.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon, failed to request usage mode from VehicleComm");
            ret = false;
        }
#endif
        return ret;
    }

    /************************************************************/
    // @brief :request rmt car status
    // @param[in]  
    // @return     True if request ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::RequestRMTStatus()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);
        
        bool ret = true;
        
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = 
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);

        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        if (vc::RET_OK == igen.Request_GetRMTStatus(vehicle_comm_request_id->GetId()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                "RemoteCtrlCommon::RequestRMTStatus, requests for RMT Status sent to VehicleComm.");
        } else {
            ret = false;
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "RemoteCtrlCommon::RequestRMTStatus, failed to request RMT status from VehicleComm");
        }
        return ret;
    }

    /************************************************************/
    // @brief :request position infomation
    // @param[in]  
    // @return     True if request ok
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::RequestPosition()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s", __FUNCTION__);

        bool ret = true;
        
        std::shared_ptr<fsm::VpomTransactionId> vpom_request_id = std::make_shared<fsm::VpomTransactionId>();
        MapTransactionId(vpom_request_id);
        
        VpomIPositioningService& vpom_gnss_service_object = fsm::VpomSignalSource::GetInstance().GetPositioningServiceObject();

        if(!vpom_gnss_service_object.GNSSPositionDataRawRequest(vpom_request_id->GetId()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::RequestPosition, failed to request position!\n");
        }

        return ret;
    }
    /************************************************************/
    // @brief :read local config
    // @param[in]  trigerred signal type
    // @return     True if checked ok
    // @author     Nie Yujin
    /************************************************************/
    void RemoteCtrlCommon::UpdateLocalConfig(int type)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s, type = %d", __FUNCTION__, type);

        bool result = false;

        result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::RDU_VFC_TIMEOUT, m_VFCTimeout);
        if(!result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get RDU_VFC_TIMEOUT fail.\n");
            m_VFCTimeout = 5;
        }

        result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::WAIT_CAN_WAKEUP_TIME, m_usageModeTimeout);
        if(!result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get WAIT_CAN_WAKEUP_TIME fail.\n");
            m_usageModeTimeout = 2;
        }
            
        switch (type)
        {
        case fsm::kVDServiceRdl:
        case fsm::kVDServiceRdu:
//        case fsm::kVDServiceRtl:
//        case fsm::kVDServiceRtu:
        {
            if ((m_signal->GetRequestPayload()->requestType & RMC_RDU_TARGET) == RMC_RDU_TARGET)
            {
                if (m_signal->GetRequestPayload()->targetValue == 1)  // trunk
                {
                    result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::TR_UNLOCK_TIMEOUT, m_execTimeout);
                    if(!result)
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get TR_UNLOCK_TIMEOUT fail.\n");
                        m_execTimeout = 15;
                    }
                }
            }
            else if (((m_signal->GetRequestPayload()->requestType & RMC_RDU_DOOR) == RMC_RDU_DOOR) ||
                    ((m_signal->GetRequestPayload()->requestType & RMC_RDL_DOOR) == RMC_RDL_DOOR))
            {
                result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::LOCK_STATUS_UPDATE_TIME, m_execTimeout);
                if(!result)
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get LOCK_STATUS_UPDATE_TIME fail.\n");
                    m_execTimeout = 5;
                }
            }
            else
            {
                fsm::RemoteCtrlBasicResult res;
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                    "RemoteCtrlCommon, request message is not correct.");
                res.operationSucceeded = false;
                res.errorCode = ErrorCode_invalidData;
                SendResponse(res);
                SetState(kDone);
            }

            m_vfcGroup.assign(RMC_VFCGROUP_CENTRALC_CTRL, RMC_VFCGROUP_CENTRALC_CTRL + (sizeof(RMC_VFCGROUP_CENTRALC_CTRL)/sizeof(RMC_VFCGROUP_CENTRALC_CTRL[0])));
            break;
        }

        case fsm::kVDServiceRhl:
        {
            result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::FLASH_STATUS_UPDATE_TIME, m_execTimeout);
            if(!result)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get FLASH_STATUS_UPDATE_TIME fail.\n");
                m_execTimeout = 12;
            }

            result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::CLHAL_VFC_TIMEOUT, m_VFCTimeout);
            if(!result)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get RDU_VFC_TIMEOUT fail.\n");
                m_VFCTimeout = 15;
            }

            m_vfcGroup.assign(RMC_VFCGROUP_HAL_CTRL, RMC_VFCGROUP_HAL_CTRL + (sizeof(RMC_VFCGROUP_HAL_CTRL)/sizeof(RMC_VFCGROUP_HAL_CTRL[0])));
            break;
        }

        case fsm::kVDServiceRpp:
        {
            result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::PM25_STATUS_UPDATE_TIME, m_execTimeout);
            if(!result)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get PM25_STATUS_UPDATE_TIME fail.\n");
                m_execTimeout = 10;
            }
            m_vfcGroup.assign(RMC_VFCGROUP_CLIMA, RMC_VFCGROUP_CLIMA + (sizeof(RMC_VFCGROUP_CLIMA) / sizeof(RMC_VFCGROUP_CLIMA[0])));
            break;
        }

        case fsm::kVDServiceRws:
        {
            result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::WINDOW_STATUS_UPDATE_TIME, m_execTimeout);
            if(!result)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get WINDOW_STATUS_UPDATE_TIME fail.\n");
                m_execTimeout = 20;
            }

            result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::WINDOW_VFC_TIMEOUT, m_VFCTimeout);
            if(!result)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get WINDOW_VFC_TIMEOUT fail.\n");
                m_VFCTimeout = 20;
            }

            m_vfcGroup.assign(RMC_VFCGROUP_WIN_CTRL, RMC_VFCGROUP_WIN_CTRL + (sizeof(RMC_VFCGROUP_WIN_CTRL) / sizeof(RMC_VFCGROUP_WIN_CTRL[0])));
            break;
        }

        default:
            break;
        }
    }
#if 0
    /************************************************************/
    // @brief :request vender id
    // @param[in]  
    // @return     vender id
    // @author     Nie Yujin
    /************************************************************/
    unsigned char RemoteCtrlCommon::GetVenderID()
    {
        unsigned char id = 0;
        
        switch (m_iType)
        {
        case fsm::kVDServiceRdl:
        case fsm::kVDServiceRdu:
        case fsm::kVDServiceRtl:
        case fsm::kVDServiceRtu:
        {
            id = RemoteCtrlVFCIndex_Locking;    //lock
            break;
        }

        case fsm::kVDServiceRhl:
        {
            id = RemoteCtrlVFCIndex_HornLi;
            break;
        }

        case fsm::kVDServiceRpp:
        {
            id = RemoteCtrlVFCIndex_Clima;
            break;
        }

        case fsm::kVDServiceRws:
        {
            id = RemoteCtrlVFCIndex_Window;    //window
            break;
        }

        default:
            break;
        }

        return id;
    }
#endif
    /************************************************************/
    // @brief :change windows status type
    // @param[in]  WinStatus with storage type
    // @return     WinStatus with signal type
    // @author     Nie Yujin
    /************************************************************/
    long RemoteCtrlCommon::TransWinStatusToSignalType(long value)
    {
        long ret = 0;
        if (vc::WINANDROOFANDCURTPOSN_CLSFULL == value)
        {
            ret = fsm::WinStsClosed;
        }
        else if (vc::WINANDROOFANDCURTPOSN_OPENFULL <= value)
        {
            ret = fsm::WinStsFullyOpend;
        }
        else if ((vc::WINANDROOFANDCURTPOSN_PERCOPEN4 <= value) 
                && (value < vc::WINANDROOFANDCURTPOSN_OPENFULL))
        {
            ret = fsm::WinStsOpened;
        }
        else
        {
            ret = fsm::WinStsUnknown;
        }

        return ret;
    }
    /************************************************************/
    // @brief :send response to TSP
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlCommon::SendResponse(fsm::RemoteCtrlBasicResult response)
    {

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon::%s\n,operationSucceeded:%d,errorCode:%d", 
            __FUNCTION__,response.operationSucceeded,response.errorCode);

        m_signal->SetResultPayload(response);

        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(m_signal))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlCommon ack sent ok.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlCommon ack send fail.");
        }


        return true;
    }

} // namespace volvo_on_call
