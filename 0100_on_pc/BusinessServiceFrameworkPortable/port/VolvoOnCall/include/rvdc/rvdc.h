///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rvdc.h
//	This file handles the RVDC statemachine for application.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Jan-2019
///////////////////////////////////////////////////////////////////
#ifndef RVDC_H_
#define RVDC_H_
//#include <list>

#include "voc_framework/transactions/smart_transaction.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rvdc_signal.h"
#include "rvdc_data.h"
#include "rvdc_signal_queue.h"
#include "rvdc_mdp_queue.h"


namespace volvo_on_call
{
    #define RVDC_DEFAULT_VIN_STR        ("FFFFFFFFFFFFFFFFF")
    #define RVDC_DEFAULT_VIN_LEN        (17)
    typedef struct {
        uint32_t maid;
        uint32_t maversion;
    } RvdcMA_t;
            
    class Rvdc: public fsm::SmartTransaction
    {
    public:

        /************************************************************/
        // @brief :Constructs
        // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
        // @return
        // @author     Nie Yujin
        /************************************************************/
        Rvdc();
        ~Rvdc();
        
        class RvdcNewReqSignal: public fsm::Signal
        {
            public:
                RvdcNewReqSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kRvdcNewReq){}
                std::string ToString(){return "RvdcNewReqSignal";}
        };

        class RvdcExeReqSignal: public fsm::Signal
        {
            public:
                RvdcExeReqSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kRvdcExeReq){}
                std::string ToString(){return "RvdcExeReqSignal";}
        };

        void MapRvdcTransactionId(std::shared_ptr<fsm::Signal> signal)
        {
            MapTransactionId(signal->GetTransactionId().GetSharedCopy());
        }

    protected:
        
        /************************************************************/
        // @brief :Enumerates the states
        // @author     Nie Yujin
        /************************************************************/
        enum ReqStates
        {
            kStarting = 0,
            kIdle,
            kRunning,
            kDone
        };

        /************************************************************/
        // @brief :enter function, will be called when enter kStarting state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterStarting(State old_state, State new_state);
        
        /************************************************************/
        // @brief :enter function, will be called when enter kIdle state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterIdle(State old_state, State new_state);

        /************************************************************/
        // @brief :enter function, will be called when enter kRunning state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterRunning(State old_state, State new_state);

        /************************************************************/
        // @brief :enter function, will be called when enter kDone state
        // @param[in]  old state, new state
        // @return     void
        // @author     Nie Yujin
        /************************************************************/
        void EnterDone(State old_state, State new_state);

        /************************************************************/
        // @brief :handle rvdc related signals, catch them and send RvdcNewReqSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleReqSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle RvdcNewReqSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleNewReqSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle RvdcExeReqSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleExecReqSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle kVINNumberSignal
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleVinNumberSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :request car mode
        // @param[in]  
        // @return     True if request ok
        // @author     Nie Yujin
        /************************************************************/
        bool RequestVinNumber();

        /************************************************************/
        // @brief :send MDP upload request to TSP
        // @param[in]
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool SendMDPUploadReq();

        /************************************************************/
        // @brief :send MA sync request to TSP
        // @param[in]
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool SendMASyncReq(const OpRVDCMeasurementSynchronization_Request_Data& data);

        /************************************************************/
        // @brief :send AssignmentNotify to TSP
        // @param[in]
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool SendAssignmentNotify(const OpRVDCAssignmentNotification_Notification_Data& data);

        /************************************************************/
        // @brief :send MASync result to VGM
        // @param[in]
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool SendMASyncRes(OpRVDCMeasurementSynchronization_Response_Data& data);

        /************************************************************/
        // @brief :send RVDCAuthorizationStatusRequest result to VGM
        // @param[in]
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool SendAuthorizationStatusRes();

        /************************************************************/
        // @brief :handle kRvdcReqMA
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleReqMASignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle kRvdcNotifyAssign
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleNotifyAssignSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle kRvdcUploadMDP
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleUploadMDPSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle kRvdcReqGPSTime
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleReqGPSTimeSignal();

        /************************************************************/
        // @brief :handle kRvdcReqConnectivitySts
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleReqConnectivityStsSignal();

        /************************************************************/
        // @brief :handle kVDServiceRvc
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleVDServiceRvcSignal(std::shared_ptr<fsm::Signal> signal);

        /************************************************************/
        // @brief :handle kRvdcReqAuthorizationSts
        // @param[in]  received signals
        // @return     True if transaction is not yet finished, false if it is finished.
        // @author     Nie Yujin
        /************************************************************/
        bool HandleReqAuthorizationStsSignal();
        
        /************************************************************/
        // @brief :set payload common header
        // @param[in]  payload
        // @return      
        // @author     Nie Yujin
        /************************************************************/
        void SetPlayloadHeader(fsm::RVDC_Data& data);
    protected:
        
    private:
        IpcbIRVDCService &m_rvdcService;
        bool m_bConnected;
        bool m_bMASyncRequested;
        RvdcSignalQueue m_rvdcSigQueue;
        RvdcMDPQueue m_rvdcMDPQueue;
        std::string m_sVin;
        uint32_t m_currSigIndex;        // used to save sended signal request id for upload MDP, 0: not sending
        uint32_t m_MDPIndex;            // sending MDP index, 0: not sending
        std::shared_ptr<GlyVdsRvdcSignal> m_vdcSignal;

    };

} // namespace volvo_on_call

#endif //RVDC_H_

