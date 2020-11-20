///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_doors_lock_transaction.h
//	This file handles the Remote Control statemachine for application.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	06-Aug-2018
///////////////////////////////////////////////////////////////////
#ifndef REMOTE_CTRL_COMMON_H_
#define REMOTE_CTRL_COMMON_H_

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/vehicle_comm_signal.h"

#include "signals/geely/gly_vds_rmc_signal.h"

#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"

namespace volvo_on_call
{
//    #define RemoteCtrlVFCIndex_Clima         3
//    #define RemoteCtrlVFCIndex_HornLi        10
//    #define RemoteCtrlVFCIndex_Locking       16
//    #define RemoteCtrlVFCIndex_Window        37

    static const uint8_t RMC_VFCGROUP_CENTRALC_CTRL[] = {45, 30, 16};
    static const uint8_t RMC_VFCGROUP_WIN_CTRL[] = {45, 30, 37};
    static const uint8_t RMC_VFCGROUP_HAL_CTRL[] = {45, 30};
    static const uint8_t RMC_VFCGROUP_CLIMA[] = {45, 30};
//    const uint8_t vfcGroupSizeCentral = sizeof(RMC_VFCGROUP_CENTRALC_CTRL) / sizeof(RMC_VFCGROUP_CENTRALC_CTRL[0]);

    enum ErrorCode {
	ErrorCode_incorrectState	= 1,
	ErrorCode_invalidData	= 2,
	ErrorCode_invalidFormat	= 3,
	ErrorCode_powderModeNotCorrect	= 4,
	ErrorCode_conditionsNotCorrect	= 5,
	ErrorCode_executionFailure	= 6,
	ErrorCode_permissionDenied	= 7,
	ErrorCode_timeout	= 8,
	ErrorCode_backupBatteryUsage	= 9
    };
    
    class RemoteCtrlCommon: public fsm::SmartTransaction
    {
    public:

        /************************************************************/
        // @brief :Constructs
        // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
        // @return
        // @author     Nie Yujin
        /************************************************************/
        RemoteCtrlCommon(std::shared_ptr<fsm::Signal> initial_signal);

        class RemoteCtrlStartSignal : public fsm::Signal
        {
            public:
                RemoteCtrlStartSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kRmcStart)
                {
                }
                std::string ToString()
                {
                    return "RemoteCtrlStartSignal";
                }
        };

        class RemoteCtrlCancelSignal : public fsm::Signal
        {
            public:
                RemoteCtrlCancelSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kRmtCancel)
                {
                }
                std::string ToString()
                {
                    return "RemoteCtrlCancelSignal";
                }
        };

    protected:
        /************************************************************/
        // @brief :change windows status type
        // @param[in]  WinStatus with storage type
        // @return     WinStatus with signal type
        // @author     Nie Yujin
        /************************************************************/
        long TransWinStatusToSignalType(long value);
        std::shared_ptr<GlyVdsRmcSignal> GetSignal() {return m_signal;}
        /************************************************************/
        // @brief :send request to vuc, this function should be implement in inherited class
        // @param[in]  
        // @return     True: success, False:Failed
        // @author     Nie Yujin
        /************************************************************/
        virtual bool SendRequest() = 0;

        /************************************************************/
        // @brief :send response to TSP, this function should be implement in inherited class
        // @param[in]  
        // @return     True: success, False:Failed
        // @author     Nie Yujin
        /************************************************************/
        virtual bool SendResponse(fsm::RemoteCtrlBasicResult response);
        virtual bool SendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status) = 0;

        /************************************************************/
        // @brief :handle requset result from vuc, this function should be implement in inherited class
        // @param[in]  
        // @return     True: success, False:Failed
        // @author     Nie Yujin
        /************************************************************/
        virtual bool HandleResult(std::shared_ptr<fsm::Signal> signal) = 0;
        
    private:

        /************************************************************/
        // @brief :Enumerates the states
        // @author     Nie Yujin
        /************************************************************/
        enum ReqStates
        {
            kNoReq = 0, ///< Newly created
            kVFCStarting,
            kRequesting,
            kDone
        };

        /************************************************************/
        // @brief :enter function, will be called when enter kNoReq state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterNoReq(State old_state, State new_state);

        /************************************************************/
        // @brief :enter function, will be called when enter kVFCStarting state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterVFCStarting(State old_state, State new_state);

        /************************************************************/
        // @brief :enter function, will be called when enter kRequesting state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterRequesting(State old_state, State new_state);

        /************************************************************/
        // @brief :enter function, will be called when enter kDone state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterDone(State old_state, State new_state);

        /************************************************************/
        // @brief :handle remote control signals
        //         such as:fsm::kVDServiceRdl, kVDServiceRdu, kVDServiceRtl, kVDServiceRtu, kVDServiceRws, kVDServiceRpp, kVDServiceRhl
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleReqSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle VocInternalSignalTypes::kRmcStart
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleStartSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle fsm::Signal::kRMTCarStatesSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleRMTCarStatesSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle fsm::Signal::kTelmDshb
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleTelmDshbSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle fsm::Signal::kGNSSPositionDataSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandlePosition(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle fsm::Signal::kCarMode, fsm::Signal::kCarUsageMode
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleCarModeSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle fsm::Signal::kVFCActivateSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleVFCSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle result signals
        // such as fsm::Signal::kHornNLight, kDoorLockStatusSignal, kWinOpenStatusSignal, kPMLvlStatusSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleResultSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle timeout signals
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleTimeoutSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :check car mode
        // @param[in]  received signals
        // @return     True if checked ok
        // @author     Nie Yujin
        /************************************************************/
        bool CheckCarModePrecondition(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :check car usage mode
        // @param[in]  received signals
        // @return     True if checked ok
        // @author     Nie Yujin
        /************************************************************/
        bool CheckCarUsageModePrecondition(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :request to VFC activate
        // @param[in]  true:active; false:deactive
        // @return     True if request ok
        // @author     Nie Yujin
        /************************************************************/
        bool RequestVFCActivate(uint8_t id, bool bActivate);

        /************************************************************/
        // @brief :request car mode
        // @param[in]  
        // @return     True if request ok
        // @author     Nie Yujin
        /************************************************************/
        bool RequestCarMode();

        /************************************************************/
        // @brief :request car usage mode
        // @param[in]  
        // @return     True if request ok
        // @author     Nie Yujin
        /************************************************************/
        bool RequestUsageMode();

        /************************************************************/
        // @brief :request rmt car status
        // @param[in]  
        // @return     True if request ok
        // @author     Nie Yujin
        /************************************************************/
        bool RequestRMTStatus();

        /************************************************************/
        // @brief :request position infomation
        // @param[in]  
        // @return     True if request ok
        // @author     Nie Yujin
        /************************************************************/
        bool RequestPosition();
    
        /************************************************************/
        // @brief :read local config
        // @param[in]  trigerred signal type
        // @return     True if checked ok
        // @author     Nie Yujin
        /************************************************************/
        void UpdateLocalConfig(int type);
        
        /************************************************************/
        // @brief :request vender id
        // @param[in]  
        // @return     vender id
        // @author     Nie Yujin
        /************************************************************/
//        unsigned char GetVenderID();

    protected:
        fsm::RemoteCtrlBasicStatus m_RMTStatus;       // RTM CAR Status
        
    private:
        vocpersistdatamgr::VocPersistDataMgr& m_vocpersist; // Persistent manager
        std::shared_ptr<GlyVdsRmcSignal> m_signal;
        
        int m_iType;    // signal type
        
        unsigned int m_VFCTimeout;              // timer for counting VFC start
        unsigned int m_execTimeout;             // timer for counting command execute by vuc
        unsigned int m_usageModeTimeout;        // timer for waiting usag mode ready

        fsm::TimeoutTransactionId m_ttidVFC;    // transaction is for VFC timer
        fsm::TimeoutTransactionId m_ttidExec;   // transaction is for execute timer
        fsm::TimeoutTransactionId m_ttidUsage;  // transaction is for waiting usage mode timer

        bool m_bCarMode;        // true: car mode is ok
        bool m_bUsageMode;      // true: usage mode is ok to execute request
        bool m_bUsageModePre;   // true: usage mode is ok to request vfc active
        uint8_t m_vfcIndex;
        std::vector<uint8_t> m_vfcGroup;
    };

} // namespace volvo_on_call

#endif //REMOTE_CTRL_COMMON_H_

