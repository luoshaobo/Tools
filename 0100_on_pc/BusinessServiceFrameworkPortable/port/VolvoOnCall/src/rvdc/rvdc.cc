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
// @Init date	28-Jan-2019
///////////////////////////////////////////////////////////////////
#include "rvdc/rvdc.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "dlt/dlt.h"
#include "timestamp.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

    /************************************************************/
    // @brief :Constructs signal.
    // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    Rvdc::Rvdc()
        : fsm::SmartTransaction(kStarting)
        , m_rvdcService(fsm::IpCommandBrokerSignalSource::GetInstance().GetIpcbRvdcServiceObject())
        , m_bConnected(true)
        , m_bMASyncRequested(false)
        , m_rvdcSigQueue()
        , m_rvdcMDPQueue()
        , m_sVin(RVDC_DEFAULT_VIN_STR)
        , m_currSigIndex(0)
        , m_MDPIndex(0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

        fsm::VdServiceTransactionId vdsTransId;
        asn_wrapper::VDServiceRequest *vdmsg = nullptr;
        m_vdcSignal =  std::dynamic_pointer_cast<GlyVdsRvdcSignal> (GlyVdsRvdcSignal::CreateGlyVdsRvdcSignal(vdsTransId, vdmsg));

        StateFunction starting =
            std::bind(&Rvdc::EnterStarting,
            this,
            std::placeholders::_1,
            std::placeholders::_2);
        
        StateFunction idle =
            std::bind(&Rvdc::EnterIdle,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        StateFunction running =
            std::bind(&Rvdc::EnterRunning,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        StateFunction done =
            std::bind(&Rvdc::EnterDone,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        SignalFunction handle_req_signal =
            std::bind(&Rvdc::HandleReqSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_new_request =
            std::bind(&Rvdc::HandleNewReqSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_exec_request =
            std::bind(&Rvdc::HandleExecReqSignal,
            this,
            std::placeholders::_1);

        SignalFunction handle_vin_signal =
            std::bind(&Rvdc::HandleVinNumberSignal,
            this,
            std::placeholders::_1);

        // Todo: handle connection status changed signal, if the status changes to disconnected, set state as kStarting
        StateMap state_map =
            {{kStarting,
                {starting,                         // state function
                {
                 {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                 {fsm::Signal::kRvdcReqMA, handle_req_signal},
                 {fsm::Signal::kRvdcNotifyAssign, handle_req_signal},
                 {fsm::Signal::kRvdcUploadMDP, handle_req_signal},
                 {fsm::Signal::kRvdcReqGPSTime, handle_req_signal},
                 {fsm::Signal::kRvdcReqConnectivitySts, handle_req_signal},
                 {fsm::Signal::kRvdcReqAuthorizationSts, handle_req_signal},
                 {fsm::kVDServiceRvc, handle_req_signal},
                },
                {kIdle, kDone}}},  // valid transitions
            {kIdle,
                {idle,                         // state function
                {
                 {VocInternalSignalTypes::kRvdcNewReq, handle_new_request},
                 {VocInternalSignalTypes::kRvdcExeReq, handle_exec_request},
                 {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                 {fsm::Signal::kRvdcReqMA, handle_req_signal},
                 {fsm::Signal::kRvdcNotifyAssign, handle_req_signal},
                 {fsm::Signal::kRvdcUploadMDP, handle_req_signal},
                 {fsm::Signal::kRvdcReqGPSTime, handle_req_signal},
                 {fsm::Signal::kRvdcReqConnectivitySts, handle_req_signal},
                 {fsm::Signal::kRvdcReqAuthorizationSts, handle_req_signal},
                 {fsm::kVDServiceRvc, handle_req_signal},
                },
                {kStarting, kRunning, kDone}}},  // valid transitions
            {kRunning,
                {running,                   // state function
                {
                 {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                 {fsm::Signal::kRvdcReqMA, handle_req_signal},
                 {fsm::Signal::kRvdcNotifyAssign, handle_req_signal},
                 {fsm::Signal::kRvdcUploadMDP, handle_req_signal},
                 {fsm::Signal::kRvdcReqGPSTime, handle_req_signal},
                 {fsm::Signal::kRvdcReqConnectivitySts, handle_req_signal},
                 {fsm::Signal::kRvdcReqAuthorizationSts, handle_req_signal},
                 {fsm::kVDServiceRvc, handle_req_signal},
                },
                {kStarting, kIdle, kDone}}},        // valid transitions
            {kDone,
                {done,                          // state function
                SignalFunctionMap(),            // signal function map
                {}}}};                          // valid transitions
        

        SetStateMap(state_map);
        
        MapSignalType(VocInternalSignalTypes::kRvdcNewReq);
        MapSignalType(VocInternalSignalTypes::kRvdcExeReq);
        
        MapSignalType(fsm::Signal::kRvdcReqMA);
        MapSignalType(fsm::Signal::kRvdcNotifyAssign);
        MapSignalType(fsm::Signal::kRvdcUploadMDP);
        MapSignalType(fsm::Signal::kRvdcReqAuthorizationSts);

        SetState(kIdle);
        RequestVinNumber();
    }

    Rvdc::~Rvdc  ()
    {
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"Rvdc::%s\n", __FUNCTION__);
    }

    /************************************************************/
    // @brief :enter function, will be called when enter kStarting state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void Rvdc::EnterStarting(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
    }
    
    /************************************************************/
    // @brief :enter function, will be called when enter kIdle state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void Rvdc::EnterIdle(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s, from %d to %d", __FUNCTION__, old_state, new_state);

        if(m_rvdcSigQueue.isEmpty() == false)
        {
            std::shared_ptr<fsm::Signal> exeSignal = std::make_shared<RvdcExeReqSignal>();
            EnqueueSignal(exeSignal);
        }
        else if((m_rvdcMDPQueue.isEmpty()) == false && (m_MDPIndex == 0))
        {
            SendMDPUploadReq();    // To send MDP to TSP
        }
    }

    /************************************************************/
    // @brief :enter function, will be called when enter kRunning state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void Rvdc::EnterRunning(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
    }

    /************************************************************/
    // @brief :enter function, will be called when enter kDone state
    // @param[in]  old state, new state
    // @return     void
    // @author     Nie Yujin
    /************************************************************/
    void Rvdc::EnterDone(State old_state, State new_state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
    }

    /************************************************************/
    // @brief :handle rvdc related signals, catch them and send RvdcNewReqSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleReqSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        RvdcSignal_t newSignalIn;

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s signal addr:%0x\n", __FUNCTION__,signal.get());

        newSignalIn.signal = signal;
        newSignalIn.status = eSigStsWaitRun;
        RvdcSignal rvdcSignal = RvdcSignal(newSignalIn);

        m_rvdcSigQueue.addRvdcSignal(rvdcSignal);

        std::shared_ptr<fsm::Signal> newSignal = std::make_shared<RvdcNewReqSignal>();
        EnqueueSignal(newSignal);
            
            
        return true;
    }

    /************************************************************/
    // @brief :handle RvdcNewReqSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleNewReqSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        //if state in Idle, need trigger one signal.
        if(GetState() == kIdle)
        {
            std::shared_ptr<fsm::Signal> exeSignal = std::make_shared<RvdcExeReqSignal>();
            EnqueueSignal(exeSignal);
        }
        
        return true;
    }

    /************************************************************/
    // @brief :handle RvdcExeReqSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleExecReqSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        bool ret = false;
        RvdcSignal exeSignal;

        SetState(kRunning);

        if (m_rvdcSigQueue.isEmpty() == false)
        {
            exeSignal = m_rvdcSigQueue.getNextRvdcSignal();
        }

        int iType = exeSignal.getSignal()->GetSignalType();
        switch(iType)
        {
        case fsm::Signal::kRvdcReqMA:
            ret = HandleReqMASignal(exeSignal.getSignal());
            break;
        case fsm::Signal::kRvdcNotifyAssign:
            ret = HandleNotifyAssignSignal(exeSignal.getSignal());
            break;
        case fsm::Signal::kRvdcUploadMDP:
            ret = HandleUploadMDPSignal(exeSignal.getSignal());
            break;
        case fsm::Signal::kRvdcReqGPSTime:
            ret = HandleReqGPSTimeSignal();
            break;
        case fsm::Signal::kRvdcReqConnectivitySts:
            ret = HandleReqConnectivityStsSignal();
            break;
        case fsm::Signal::kRvdcReqAuthorizationSts:
            ret = HandleReqAuthorizationStsSignal();
            break;
        case fsm::kVDServiceRvc:
            m_vdcSignal = std::dynamic_pointer_cast<GlyVdsRvdcSignal>(exeSignal.getSignal());
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", "fsm::kVDServiceRvc");
			ret = HandleVDServiceRvcSignal(exeSignal.getSignal());
            break;
        default:
            break;
        }

        if (ret)
        {
            m_rvdcSigQueue.removeRvdcSignal(exeSignal);
        }

        SetState(kIdle);
        return true;
    }

    /************************************************************/
    // @brief :send MDP upload request to TSP
    // @param[in]
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::SendMDPUploadReq()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        fsm::RVDC_Data payload;

        RvdcMDP rvdcMDP = m_rvdcMDPQueue.getNextRvdcMDP();
        
        //set payload
        SetPlayloadHeader(payload);
        payload.header.ackRequired = true;
        payload.maBusinessType = app_MaBusinessType_measurementDataPackage;
        payload.measurementDataPackage_presence = true;
        payload.measurementDataPackage.Assign(rvdcMDP.getMdp());
        m_MDPIndex = rvdcMDP.getIndex();

        m_vdcSignal->SetResultPayload(payload);

        //send to server
        fsm::VocmoSignalSource vocmo_signal_source;
        bool result = vocmo_signal_source.GeelySendMessage(m_vdcSignal);
        if (result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "rvdc upload mdp sent ok.\n");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "rvdc upload mdp send fail.\n");
        }
        std::shared_ptr<fsm::RVDC_Data> rvdcData = m_vdcSignal->GetResponsePayload();
        m_currSigIndex = rvdcData->header.requestid;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "rvdc requestid = %d----->", m_currSigIndex);
        return ret;
    }

    /************************************************************/
    // @brief :send MA sync request to TSP
    // @param[in]
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::SendMASyncReq(const OpRVDCMeasurementSynchronization_Request_Data& data)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        fsm::RVDC_Data payload;

        //set payload
        SetPlayloadHeader(payload);

        payload.maBusinessType = app_MaBusinessType_maSynchronizationReq;
        payload.maSynchronizationReq_presence = true;
        payload.maSynchronizationReq.storageQuota = data.storagequota;
        payload.maSynchronizationReq.maFormatVersion = data.maformatversion;
        payload.maSynchronizationReq.installedMa_presence = false;

        for (auto pInstalledMa = data.ma.begin(); pInstalledMa != data.ma.end();) 
        {
            payload.maSynchronizationReq.installedMa_presence = true;
            fsm::Ma_Data installedMa;
            installedMa.maId = pInstalledMa->maid;
            installedMa.maVersion = pInstalledMa->maversion;
            payload.maSynchronizationReq.installedMa.push_back(installedMa);
            ++pInstalledMa;
        }
        
        payload.maSynchronizationReq.serviceParameters_presence = false;

        m_vdcSignal->SetResultPayload(payload);

        //send to server
        fsm::VocmoSignalSource vocmo_signal_source;
        bool result = vocmo_signal_source.GeelySendMessage(m_vdcSignal);
        if (result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "rvdc SendMASyncReq sent ok.\n");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "rvdc SendMASyncReq send fail.\n");
        }
        return ret;
    }

    /************************************************************/
    // @brief :send AssignmentNotify to TSP
    // @param[in]
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::SendAssignmentNotify(const OpRVDCAssignmentNotification_Notification_Data& data)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        fsm::RVDC_Data payload;

        //set payload
        SetPlayloadHeader(payload);

        payload.maBusinessType = app_MaBusinessType_assignmentNotification;
        payload.assignmentNotification_presence = true;
        payload.assignmentNotification.Assign(data);

        //send to server
        m_vdcSignal->SetResultPayload(payload);

        fsm::VocmoSignalSource vocmo_signal_source;
        bool result = vocmo_signal_source.GeelySendMessage(m_vdcSignal);
        if (result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "rvdc SendAssignmentNotify sent ok.\n");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "rvdc SendAssignmentNotify send fail.\n");
        }
        return ret;
    }

    /************************************************************/
    // @brief :send MASync result to VGM
    // @param[in]
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::SendMASyncRes(OpRVDCMeasurementSynchronization_Response_Data& data)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        bool ret = true;

        if (m_bMASyncRequested)
        {
            m_rvdcService.sendRVDCServiceData(OperationId::RVDCMeasurementSynchronization, OperationType::RESPONSE, 0, data);
            m_bMASyncRequested = false;
        }
        else
        {
            m_rvdcService.sendRVDCServiceData(OperationId::RVDCMeasurementSynchronization, OperationType::NOTIFICATION, 0, data);
        }
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RVDCMeasurementSynchronization");
        
        return ret;
    }

    /************************************************************/
    // @brief :send RVDCAuthorizationStatusRequest result to VGM
    // @param[in]
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::SendAuthorizationStatusRes()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        bool ret = true;

        // Todo: set Authorization Status
        OpRVDCAuthorizationStatusRequest_Response_Data data;
        data.collectfunction = true;
        data.authorizationstatus.datacollection = true;
        data.authorizationstatus.remotediagnostics = true;
        
        m_rvdcService.sendRVDCServiceData(OperationId::RVDCAuthorizationStatusRequest, OperationType::RESPONSE, 0, data);


        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RVDCAuthorizationStatusRequest-Response complite");
        
        return ret;
    }

    /************************************************************/
    // @brief :request car mode
    // @param[in]  
    // @return     True if request ok
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::RequestVinNumber()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);

        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        //request car mode
        if (igen.Request_VINNumber(vehicle_comm_request_id->GetId()) == vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                "Rvdc, requests for vinNumber sent to VehicleComm.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                "Rvdc, failed to request vinNumber from VehicleComm");
            ret = false;
        }
        return ret;
    }

    /************************************************************/
    // @brief :handle kVINNumberSignal
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleVinNumberSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        std::shared_ptr<fsm::VINNumberSignal> vinNumberSignal = std::static_pointer_cast<fsm::VINNumberSignal>(signal);

        if(vinNumberSignal)
        {
            if(vinNumberSignal->GetData()->vin_number[0] == 0xFF) // no vin_number exists
            {
               DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "rvdc, vin number invalid.");
            }
            else
            {
                m_sVin = vinNumberSignal->GetData()->vin_number;
            }
        }
        else
        {
           DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "rvdc, get vin number fail.");
        }
        return ret;
    }

    /************************************************************/
    // @brief :handle kRvdcReqMA
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleReqMASignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        std::shared_ptr<fsm::RVDCMeasurementSynchronizationRequestSignal> pSignal =
                        std::static_pointer_cast<fsm::RVDCMeasurementSynchronizationRequestSignal>(signal);
        const OpRVDCMeasurementSynchronization_Request_Data &data = pSignal->GetPayload();

        SendMASyncReq(data);

        m_bMASyncRequested = true;
        return ret;
    }

    /************************************************************/
    // @brief :handle kRvdcNotifyAssign
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleNotifyAssignSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        
        bool ret = true;
        std::shared_ptr<fsm::RVDCAssignmentNotificaitonSignal> pSignal =
                        std::static_pointer_cast<fsm::RVDCAssignmentNotificaitonSignal>(signal);
        const OpRVDCAssignmentNotification_Notification_Data &data = pSignal->GetPayload();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, maid:%d, maversion:%d", __FUNCTION__, data.maid, data.maversion);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, timestamp:%s", __FUNCTION__, data.timestamp.timestamp1.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, newstatus:%s", __FUNCTION__, data.newstatus.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, reason:%s", __FUNCTION__, data.reason.c_str());

        SendAssignmentNotify(data);

        return ret;
    }

    /************************************************************/
    // @brief :handle kRvdcUploadMDP
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleUploadMDPSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        bool ret = true;
        std::shared_ptr<fsm::RVDCMeasurementDataPackageSignal> pSignal =
                        std::static_pointer_cast<fsm::RVDCMeasurementDataPackageSignal>(signal);
        const OpRVDCMeasurementDataPackage_Request_Data &data = pSignal->GetPayload();
    
        // Todo: store the MDP
        RvdcMDP rvdcMDP = RvdcMDP(data, eMDPUpStsWaitUpload);
        m_rvdcMDPQueue.addRvdcMDP(rvdcMDP);
        
        // send ack to VGM
        OpRVDCMeasurementDataPackage_Response_Data rvdcRes;
        rvdcRes.mdprecievedcorrectly = true;
        m_rvdcService.sendRVDCServiceData(OperationId::RVDCMeasurementDataPackage, OperationType::RESPONSE, 0, rvdcRes);
        
        return ret;
    }

    /************************************************************/
    // @brief :handle kRvdcReqGPSTime
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleReqGPSTimeSignal()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        bool ret = true;
        // Todo: get GPSTime & send to VGM
        return ret;
    }

    /************************************************************/
    // @brief :handle kRvdcReqConnectivitySts
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleReqConnectivityStsSignal()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        bool ret = true;
        // Todo: get the latest connectivity status
        // Todo: if it's connected, send the status to VGM
        return ret;
    }

    /************************************************************/
    // @brief :handle kRvdcReqAuthorizationSts
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleReqAuthorizationStsSignal()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        bool ret = true;

        ret = SendAuthorizationStatusRes();

        return ret;
    }

    /************************************************************/
    // @brief :handle kVDServiceRvc
    // @param[in]  received signals
    // @return     True if transaction is not yet finished, false if it is finished.
    // @author     Nie Yujin
    /************************************************************/
    bool Rvdc::HandleVDServiceRvcSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);
        bool ret = true;
        std::shared_ptr<fsm::RVDC_Data> rvdcData = (std::dynamic_pointer_cast<GlyVdsRvdcSignal>(signal))->GetRequestPayload();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, maBusinessType = %d" , __FUNCTION__, rvdcData->maBusinessType);
        
        switch (rvdcData->maBusinessType)
        {
        case App_MaBusinessType::app_MaBusinessType_maSynchronization:
        {
            if (rvdcData->maSynchronization_presence)
            {
                OpRVDCMeasurementSynchronization_Response_Data data;
                rvdcData->maSynchronization.AssignTo(data);

                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s, removemas_presence = %d" , __FUNCTION__, data.removemas_presence);
                SendMASyncRes(data);
            }
            break;
        }
        case  App_MaBusinessType::app_MaBusinessType_measurementDataPackage:
        {
            if (rvdcData->header.requestid == m_currSigIndex)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s, received mdp upload ack, delete", __FUNCTION__);
                m_rvdcMDPQueue.removeRvdcMDP(m_MDPIndex);
                m_MDPIndex = 0;
                m_currSigIndex = 0;
            }
            break;
        }
        default:
            break;
        }

        return ret;
    }

    /************************************************************/
    // @brief :set payload common header
    // @param[in]  payload
    // @return      
    // @author     Nie Yujin
    /************************************************************/
    void Rvdc::SetPlayloadHeader(fsm::RVDC_Data& payload)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Rvdc::%s", __FUNCTION__);

        Timestamp now;

        //Set Header        
        payload.header.requestid = random(255); //m_currSigIndex;
        payload.header.timeStampSeconds = (now.epochMicroseconds()/1000000);
        payload.header.timeStampMilliseconds_presence = true;
        payload.header.timeStampMilliseconds = (now.epochMicroseconds()%1000000)/1000;
        payload.header.eventId_presence = true;
        payload.header.eventIdSeconds = (now.epochMicroseconds()/1000000);
        payload.header.eventIdMilliseconds_presence = true;
        payload.header.eventIdMilliseconds = (now.epochMicroseconds()%1000000)/1000;
        payload.header.creatorId_presence = true;
        payload.header.creatorId = 1;
        payload.header.messageTTL_presence = true;
        payload.header.messageTTL = 120;
        payload.header.requestType_presence = false;
        payload.header.requestType = 1;
        payload.header.ackRequired_presence = true;
        payload.header.ackRequired = false;

        //Set common rvdc data        
        payload.rvdcSetting_presence = false;
        payload.assignmentNotification_presence = false;
        payload.maSynchronization_presence = false;
        payload.exceptionHandling_presence = false;
        payload.measurementDataPackage_presence = false;
        payload.serviceId = 250;
        payload.appId = app_geely;
        payload.maOrderId = "abc";
//        payload.timestamp = "2018/11/28 10/18/55 EST";
        payload.timestamp = to_string(now.epochMicroseconds()/1000000);
        payload.vin = m_sVin;
    }

} // namespace volvo_on_call
