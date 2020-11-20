///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_door_tailgate.cc
//	This file handles the Remote Control Door and Tailgate locking for application.
// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Aug-2018
///////////////////////////////////////////////////////////////////
#include "remote_ctrl/remote_ctrl_door_tailgate.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"
#include "dlt/dlt.h"

#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_adapt/rmc_signal_adapt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

    /************************************************************/
    // @brief :Constructs
    // @param[in]  initial_signal,fsm::Signal, which caused transaction to be created.
    // @return
    // @author     Nie Yujin
    /************************************************************/
    RemoteCtrlDoorTail::RemoteCtrlDoorTail(std::shared_ptr<fsm::Signal> initial_signal)
        : RemoteCtrlCommon(initial_signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s, signal type = %d", __FUNCTION__, initial_signal->GetSignalType());
        switch (initial_signal->GetSignalType())
        {
        case fsm::kVDServiceRdl:
        case fsm::kVDServiceRdu:
//        case fsm::kVDServiceRtl:
//        case fsm::kVDServiceRtu:
            m_bIsActiveService = true;
            break;
        default:
            m_bIsActiveService = false;
            break;
        }
    }

    /************************************************************/
    // @brief :send request to vuc
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlDoorTail::SendRequest()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail::%s", __FUNCTION__);
        bool ret = false;

        if (m_bIsActiveService)
        {
            std::shared_ptr<fsm::RemoteCtrlBasicRequest> rmcRequest = GetSignal()->GetRequestPayload();
            vc::ReqDoorLockUnlock request;
            fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
            vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

            // Create and map a VehicleComm transaction id.
            std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

            MapTransactionId(vehicle_comm_request_id);

            request.centralLockReq = 0;

            if ((rmcRequest->requestType & RMC_RDU_TARGET) == RMC_RDU_TARGET)
            {
                if (rmcRequest->targetValue == 1)  // trunk
                {
                    request.centralLockReq = 3;             //unlock by Tr
                }
            }
            else if ((rmcRequest->requestType & RMC_RDU_DOOR) == RMC_RDU_DOOR)
            {
                request.centralLockReq = 2;             //Unlock
            }
            else if ((rmcRequest->requestType & RMC_RDL_DOOR) == RMC_RDL_DOOR)
            {
                request.centralLockReq = 1;             //Lock
            }
            else
            {
                // nothing to do
            }

            if (request.centralLockReq != 0)
            {
                if (igen.Request_LockDoor(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK)
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail::Request_LockDoor:%d sent to VehicleComm.", request.centralLockReq);
                    ret = true;
                }
                else
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlDoorTail::Failed to Request_LockDoor.");
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail:: unknown request.");
            }

        }
        else 
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail is not active service.");
        }

        return ret;
    }

    /************************************************************/
    // @brief :send response to TSP
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlDoorTail::SendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status)
    {

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail::%s,operationSucceeded:%d,errorCode:%d", 
            __FUNCTION__,result.operationSucceeded,result.errorCode);

        GetSignal()->SetResultPayload(result);
        GetSignal()->SetRmcStatusPayload(RmcResponseTypeDoor, status);
        
        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(GetSignal()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTailack sent ok.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlDoorTail ack send fail.");
        }


        return true;
    }

    /************************************************************/
    // @brief :handle requset result from vuc
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlDoorTail::HandleResult(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail::%s", __FUNCTION__);
        
        bool ret = true;
        fsm::RemoteCtrlBasicResult result;

        if (fsm::Signal::kDoorLockStatusSignal == signal->GetSignalType())
        {
#if 1 // nieyj temp
            // TODO: it should implement in internal function

            result.operationSucceeded = true;

            std::shared_ptr<fsm::DoorLockStatusSignal> doorLockStatus = std::static_pointer_cast<fsm::DoorLockStatusSignal>(signal);

            m_RMTStatus.doorOpenStatusDriver = doorLockStatus->GetData()->driverDoorState;
            m_RMTStatus.doorOpenStatusPassenger = doorLockStatus->GetData()->passengerDoorState;
            m_RMTStatus.doorOpenStatusDriverRear = doorLockStatus->GetData()->driverSideRearDoorState;
            m_RMTStatus.doorOpenStatusPassengerRear = doorLockStatus->GetData()->passengerSideRearDoorState;
            m_RMTStatus.doorLockStatusDriver = doorLockStatus->GetData()->driverDoorLockState;
            m_RMTStatus.doorLockStatusPassenger = doorLockStatus->GetData()->passengerDoorLockState;
            m_RMTStatus.doorLockStatusDriverRear = doorLockStatus->GetData()->driverSideRearDoorLockState;
            m_RMTStatus.doorLockStatusPassengerRear = doorLockStatus->GetData()->passengerSideRearDoorLockState;
            m_RMTStatus.trunkOpenStatus = doorLockStatus->GetData()->tailgateOrBootlidState;
            m_RMTStatus.trunkLockStatus = doorLockStatus->GetData()->tailgateOrBootlidLockState;
            m_RMTStatus.centralLockingStatus = doorLockStatus->GetData()->centralLockState;
            m_RMTStatus.lockgCenStsForUsrFb = doorLockStatus->GetData()->centralLockingStateForUserFeedback;
            m_RMTStatus.hoodSts = doorLockStatus->GetData()->hoodState;
            m_RMTStatus.hashoodLockSts = false;
            m_RMTStatus.hoodLockSts = 0;

            ret = SendResponse(result, m_RMTStatus);
#endif
        }
        else
        {
            ret = false;
        }

        return ret;
    }

} // namespace volvo_on_call
