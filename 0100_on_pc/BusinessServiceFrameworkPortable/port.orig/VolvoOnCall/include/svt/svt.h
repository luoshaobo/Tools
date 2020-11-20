///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file svt.h
//	VOC Service car svt request transaction.

// @project		GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date	1-Aug-2018
///////////////////////////////////////////////////////////////////

#ifndef VOC_TRANSACTIONS_SVT_H_
#define VOC_TRANSACTIONS_SVT_H_

#include "timestamp.h"
#include "vpom_IPositioningService.hpp"
#include "signals/geely/gly_vds_svt_signal.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/transactions/smart_transaction.h"
#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"

namespace volvo_on_call
{

const unsigned int kWaitForGPSTime(120);
const static long int GLY_APP_SVT_PREDEFINED_TIMER = 300; // Temporary define a timer to keep SVT's lifecycle, unit: second.

class Svt: public fsm::SmartTransaction
{
 public:

    Svt();
    
    ~Svt ();
 
 public:
    
    class SvtReStartSignal : public fsm::Signal
    {
        public:
            SvtReStartSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kSvtReStart)
            {
            }
            std::string ToString()
            {
                return "SvtReStartSignal";
            }
    };

    class SvtStartSignal : public fsm::Signal
    {
        public:
            SvtStartSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kSvtStart)
            {
            }
            std::string ToString()
            {
                return "SvtStartSignal";
            }
    };

    class SvtPeriodSignal : public fsm::Signal
    {
        public:
            SvtPeriodSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kSvtPeriod)
            {
            }
            std::string ToString()
            {
                return "SvtPeriodSignal";
            }
    };
    
    class SvtStopSignal : public fsm::Signal
    {
        public:
            SvtStopSignal(): fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kSvtStop)
            {
            }
            std::string ToString()
            {
                return "SvtStopSignal";
            }
            
            SvtStopSignal(bool sendData,bool canStop): 
                fsm::Signal(fsm::InternalSignalTransactionId(), VocInternalSignalTypes::kSvtStop)
            {
                m_sendData = sendData;
                m_canStop = canStop;
            }
            
            bool getCanStop()
            {
                return m_canStop;
            }
            
            void setCanStop(bool b)
            {
                m_canStop = b;
            }
            
            bool getSendData()
            {
                return m_sendData;
            }
            
            void setSendData(bool b)
            {
                m_sendData = b;
            }
            
        private:
            bool m_canStop = false;
            bool m_sendData= false;
    };

 public:

    enum States
    {
        kNew = 0,
        kStart, 
        kPeriod,
        kStop,
        kDone
    };

    typedef struct RelationTranId
    {
        fsm::VpomTransactionId requestId;
        fsm::TimeoutTransactionId timeOutId;
        bool isLast;
    } RelationTranId_t;

    enum SignalStatus
    {
        UNCHECK = 0,
        CHECKING,
        CHECKED,
        RUNNING,
        COMPLETE
    };
    
    typedef struct SignalWithStatus
    {
        std::shared_ptr<GlyVdsSvtSignal> signal;
        SignalStatus status;
    } SignalWithStatus_t;

    ////////////////////////////////////////////////////////////
    // @brief :init persist data.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool LoadPersistData();

    ////////////////////////////////////////////////////////////
    // @brief :Enqueue kVDServiceSvt signal to list and trigger next svt signal.
    // @param[in]  signal.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool EnqueueSvtRequest(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kVDServiceSvt signal.
    // @param[in]  signalStatus SignalWithStatus_t to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleSvtRequest(SignalWithStatus_t signalStatus);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kSvtReStart signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleSvtReStart(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kSvtStart signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleStartState(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kSvtPeriod signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandlePeriodState(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kSvtStop signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleStopState(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle entering the state kDone.Will stop the transaction.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleDoneState(State old_state, State new_state);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kTimeout signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleTimeout(std::shared_ptr<fsm::Signal> signal);
    
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle position signal from volvo positioning manager.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandlePosition(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle vehicle comm signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleVehicleComm(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kCarMode signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleCarMode(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle kCarConfig signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HandleCarConfigSvt(std::shared_ptr<fsm::Signal> signal);
    
    ////////////////////////////////////////////////////////////
    // @brief :Handle GlyVdsAppsLcmResumeSignal signal.
    // @param[in]  signal fsm::Signal to handle.
    // @return     only return true
    // @author     uia93888, 30-Nov-2018
    ////////////////////////////////////////////////////////////
    bool HandleAppsLcmResumeSignal(std::shared_ptr<fsm::Signal> signal);

    ////////////////////////////////////////////////////////////
    // @brief :Sends position requests to TCAM
    // @param[in]  isRetry retry will not set wait gps timer
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool RequestPosition(std::shared_ptr<fsm::TransactionId> tranId = nullptr, bool isLast = false);
    
    ////////////////////////////////////////////////////////////
    // @brief :Sends position requests to TCAM
    // @param[in]  isRetry retry will not set wait gps timer
    // @return     only return true
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    std::shared_ptr<fsm::TimeoutTransactionId> WaitForGPSTime();
    
    ////////////////////////////////////////////////////////////
    // @brief :Send track points to TSP.
    // @param[in]  *trackPoints send position data retrieved from TCAM
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SendSvtData(SvtTrackPoint_t trackPoints,bool isLast = false);
    
    ////////////////////////////////////////////////////////////
    // @brief :Send ack to TSP.
    // @param[in]  signalStatus
    // @param[in]  errorCode 
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SendEndAck(SignalWithStatus_t signalStatus,long errorCode);
    
    ////////////////////////////////////////////////////////////
    // @brief :pack GNSSData to SvtTrackPoint_t.
    // @param[in]  vpomPosition  position data retrieved from TCAM
    // @param[out]  *vdsTrackPoint  position data send to TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool VpomToVds(vpom::GNSSData vpomPosition, SvtTrackPoint_t *vdsTrackPoint);
    
    ////////////////////////////////////////////////////////////
    // @brief :Generate fake position.
    // @param[out]  *vdsTrackPoint fake position
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool GenerateFakePos(SvtTrackPoint_t *vdsTrackPoint);
    
    ////////////////////////////////////////////////////////////
    // @brief :Conditions for starting the tracking:
    //         Check param from TSP
    //         SVT has been provisioned.
    //         Car Mode is Normal or Crash.
    //         Car Configuration Parameter 'SVT' is 'With SVT'.
    // @param[in]  request message from TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool CheckStartCondition(SignalWithStatus_t signalStatus);
    
    ////////////////////////////////////////////////////////////
    // @brief :Conditions for terminal the tracking:
    //         check svt is running now
    // @param[in]  signalStatus message from TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool CheckStopCondition(SignalWithStatus_t signalStatus);
    
    ////////////////////////////////////////////////////////////
    // @brief :Conditions for starting the tracking:
    //         SVT has been provisioned.
    // @param[in]  request message from TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool CheckProvision();
    
    ////////////////////////////////////////////////////////////
    // @brief :Conditions for starting the tracking:
    //         Car Mode is Normal or Crash.
    // @param[in]  request message from TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool RequestCarMode();
    
    
    ////////////////////////////////////////////////////////////
    // @brief :Conditions for starting the tracking:
    //         Car Configuration Parameter 'SVT' is 'With SVT'.
    // @param[in]  request message from TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool RequestCarConfigSvt();
    
    ////////////////////////////////////////////////////////////
    // @brief :Conditions for starting the tracking:
    //         Check param from TSP
    // @param[in]  request message from TSP
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool CheckSvtParam(std::shared_ptr<SvtRequest_t> request);
    
    ////////////////////////////////////////////////////////////
    // @brief :set timer to trigger next signal
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void TriggerNextSvtSignal();
    
    ////////////////////////////////////////////////////////////
    // @brief :format Timestamp to string
    // @param[in]  ts Timestamp
    // @return     format string
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    std::string DateTimeToString(Timestamp ts);
    
    ////////////////////////////////////////////////////////////
    // @brief :format string from a utc time
    // @param[in]  utcVal int64_t
    // @return     format string
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    std::string UtcTimeValToString(int64_t utcVal);
    
    ////////////////////////////////////////////////////////////
    // @brief :calculate milliseconds
    // @param[in]  seconds
    // @param[in]  milliseconds
    // @return     milliseconds
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    long long int VdsTimestampToMilliSec(long long int seconds,long long int milliseconds);

    ////////////////////////////////////////////////////////////
    // @brief :add position request id and wait for gps timeout id relationship
    // @param[in]  relationId
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void AddWaitForGPSTick(RelationTranId_t relationId);
    
    ////////////////////////////////////////////////////////////
    // @brief :remove relationship by wait for gps timeout id 
    // @param[in]  tickId
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void RemoveWaitForGPSTick(const fsm::TransactionId& tickId);
    
    ////////////////////////////////////////////////////////////
    // @brief :remove relationship by position request id
    // @param[in]  tickId
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void RemoveVpomRequestTick(const fsm::TransactionId& tickId);
    
    ////////////////////////////////////////////////////////////
    // @brief :judge is last request by wait for gps timeout id 
    // @param[in]  wait for gps timeout id 
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool IsLastByWaitForGPSTick(const fsm::TransactionId& tickId);
    
    ////////////////////////////////////////////////////////////
    // @brief :judge is last request by request position id 
    // @param[in]  request position id 
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool IsLastByVpomRequestTick(const fsm::TransactionId& tickId);
    
    ////////////////////////////////////////////////////////////
    // @brief :judge if exist relationship by wait for gps timeout id 
    // @param[in]  relationId
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool IsWaitForGPSTicks(const fsm::TransactionId& tickId);
    
    ////////////////////////////////////////////////////////////
    // @brief :clear all relationship
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void RemoveAllWaitForGPSTicks();

    ////////////////////////////////////////////////////////////
    // @brief :enqueue tsp svt signal to list
    // @param[in]  signalStatus 
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void EnqueueSvtSignal(SignalWithStatus_t signalStatus); 
    
    ////////////////////////////////////////////////////////////
    // @brief :update signal status
    // @param[in]  signalStatus 
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void UpdateSvtSignal(SignalWithStatus_t signalStatus); 
    
    ////////////////////////////////////////////////////////////
    // @brief :set current signal status as running, 
    //         old running signal set as complete
    // @param[in]  signalStatus 
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void RunSvtSignal(SignalWithStatus_t signalStatus); 
    
    ////////////////////////////////////////////////////////////
    // @brief :get svt signal by status
    // @param[in]  status 
    // @param[in]  signalStatus 
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool GetSvtSignal(SignalStatus status, SignalWithStatus_t* signalStatus); 
    
    ////////////////////////////////////////////////////////////
    // @brief :can not handle if exist CHECKING status
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool CanHandleNextSvtSignal(); 
    
    ////////////////////////////////////////////////////////////
    // @brief :exist not complete signal
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HasInProcessSvtSignal(); 
    
    ////////////////////////////////////////////////////////////
    // @brief :exist not complete signal
    // @param[in]  signalStatus exclude signal
    // @return     True if exist set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool HasInProcessSvtSignal(SignalWithStatus_t signalStatus); 
    
    ////////////////////////////////////////////////////////////
    // @brief :remove all complete svt signal
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    void RemoveCompleteSvtSignal(); 
    
    void MapSvtTransactionId(std::shared_ptr<fsm::Signal> signal)
    {
        MapTransactionId(signal->GetTransactionId().GetSharedCopy());
    }

    Timestamp::TimeVal GetUtcTime()
    {
        Timestamp now;
        return now.epochMicroseconds();
    }
    
private:

    fsm::TimeoutTransactionId m_kSvtStart_tick_id;
    fsm::TimeoutTransactionId m_kSvtPeriod_tick_id;
    fsm::TimeoutTransactionId m_kSvtStop_tick_id;
    vocpersistdatamgr::VocPersistDataMgr& m_vocpersist;
    std::list<RelationTranId_t> m_relationIds;
    std::list<SignalWithStatus_t> m_svtSignalStatusList;
    unsigned int m_waitForGPSTime = kWaitForGPSTime;
    
    std::mutex m_relationIds_mutex;
    std::mutex m_signal_status_mutex;
    std::mutex m_request_timeout_mutex;
    
    std::shared_ptr<fsm::VdServiceTransactionId> m_SvtLcmTransactionId_;
    long m_execteTime;
    
};


} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_SVT_H_

/** \}    end of addtogroup */
