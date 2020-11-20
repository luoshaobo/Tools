///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_pm25_enquire.cc
//	This file handles the Remote Control PM2.5 enquire for application.
// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Aug-2018
///////////////////////////////////////////////////////////////////
#include "remote_ctrl/remote_ctrl_pm25_enquire.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"
#include "dlt/dlt.h"

#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

    /************************************************************/
    // @brief :Constructs
    // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    RemoteCtrlPM25Enquire::RemoteCtrlPM25Enquire(std::shared_ptr<fsm::Signal> initial_signal)
        : RemoteCtrlCommon(initial_signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlPM25Enquire::%s", __FUNCTION__);
        if (fsm::kVDServiceRpp == initial_signal->GetSignalType())
        {
            m_bIsActiveService = true;
        }
        else
        {
            m_bIsActiveService = false;
        }
    }

    /************************************************************/
    // @brief :send request to vuc
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlPM25Enquire::SendRequest()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlPM25Enquire::%s", __FUNCTION__);
        bool ret = false;

        if (m_bIsActiveService)
        {
            fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
            vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

            // Create and map a VehicleComm transaction id.
            std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

            MapTransactionId(vehicle_comm_request_id);

            if (igen.Request_PM25Enquire(vehicle_comm_request_id->GetId()) == vc::RET_OK)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Request sent to VehicleComm.");
                ret = true;
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request, aborting transaction.");
            }
        }
        else 
        {

        }

        return ret;
    }

    /************************************************************/
    // @brief :send response to TSP
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlPM25Enquire::SendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status)
    {

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail::%s,operationSucceeded:%d,errorCode:%d", 
            __FUNCTION__,result.operationSucceeded,result.errorCode);

        GetSignal()->SetResultPayload(result);
        GetSignal()->SetRmcStatusPayload(RmcResponseTypeRpp, status);
        
        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(GetSignal()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlPM25Enquire ack sent ok.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlPM25Enquire ack send fail.");
        }


        return true;
    }
    /************************************************************/
    // @brief :handle requset result from vuc
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlPM25Enquire::HandleResult(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlPM25Enquire::%s", __FUNCTION__);
        bool ret = true;
        fsm::RemoteCtrlBasicResult result;

        if (fsm::Signal::kPMLvlStatusSignal == signal->GetSignalType())
        {
            result.operationSucceeded = true;

            std::shared_ptr<fsm::PMLvlStatusSignal> pmStatus = std::static_pointer_cast<fsm::PMLvlStatusSignal>(signal);

#if 1
            m_RMTStatus.interiorPM25 = pmStatus->GetData()->intPm25Vlu;
            m_RMTStatus.exteriorPM25 = pmStatus->GetData()->outdPm25Vlu;
            m_RMTStatus.interiorPM25Level = pmStatus->GetData()->intPm25Lvl;
            m_RMTStatus.exteriorPM25Level = pmStatus->GetData()->outdPm25Lvl;
#else
            if ((vc::CMPMTAIRPMLVL_LEVEL1 <= pmStatus->GetData()->cmpmtInAirPMLvl) 
                    && (pmStatus->GetData()->cmpmtInAirPMLvl <= vc::CMPMTAIRPMLVL_LEVEL6))
            {
                m_RMTStatus.interiorPM25 = pmStatus->GetData()->cmpmtInAirPMLvl;
            }
            else
            {
                m_RMTStatus.interiorPM25 = CMPMTAIRPMLVL_INVALID;
            }

            if ((vc::CMPMTAIRPMLVL_LEVEL1 <= pmStatus->GetData()->cmpmtOutAirPMLvl) 
                    && (pmStatus->GetData()->cmpmtOutAirPMLvl <= vc::CMPMTAIRPMLVL_LEVEL6))
            {
                m_RMTStatus.exteriorPM25 = pmStatus->GetData()->cmpmtOutAirPMLvl;
            }
            else
            {
                m_RMTStatus.exteriorPM25 = CMPMTAIRPMLVL_INVALID;
            }
#endif
            GetSignal()->SetResultPayload(result);
            GetSignal()->SetRmcStatusPayload(RmcResponseTypeRpp, m_RMTStatus);

            fsm::VocmoSignalSource vocmo_signal_source;
            if (vocmo_signal_source.GeelySendMessage(GetSignal()))
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteControl ack sent ok.");
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteControl ack send fail.");
                ret = false;
            }
        }
        return ret;
    }
} // namespace volvo_on_call
