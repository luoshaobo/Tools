///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_flash_horn.cc
//	This file handles the Remote Control Flash and Horn for application.
// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Aug-2018
///////////////////////////////////////////////////////////////////
#include "remote_ctrl/remote_ctrl_flash_horn.h"
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
    RemoteCtrlFlashHorn::RemoteCtrlFlashHorn(std::shared_ptr<fsm::Signal> initial_signal)
        : RemoteCtrlCommon(initial_signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
        if ( fsm::kVDServiceRhl == initial_signal->GetSignalType())
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
    bool RemoteCtrlFlashHorn::SendRequest()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::%s", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::RemoteCtrlBasicRequest> rcbRequest = GetSignal()->GetRequestPayload();
        
        if (m_bIsActiveService && rcbRequest)
        {
            fsm::RHLType reqType = (fsm::RHLType)rcbRequest->value;
            vc::ReqHornNLight request;
            fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
            vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

            // Create and map a VehicleComm transaction id.
            std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

            MapTransactionId(vehicle_comm_request_id);

            switch (reqType)
            {
            case fsm::Honk:
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to perform Honk");
                request.mode = vc::HL_REQUEST_HORN;
                break;
            case fsm::Flash:
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to perform Flash");
                request.mode = vc::HL_REQUEST_LIGHT;
                break;
            case fsm::HonkAndFlash:
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to perform HonkAndFlash");
                request.mode = vc::HL_REQUEST_HORN_AND_LIGHT;
                break;
            default:
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                    "Received unknown car advert request, aborting transaction.");
            }

            if (igen.Request_HornAndLight(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK)
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
    bool RemoteCtrlFlashHorn::SendResponse(fsm::RemoteCtrlBasicResult response)
    {

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::%s\n,operationSucceeded:%d,errorCode:%d", 
            __FUNCTION__,response.operationSucceeded,response.errorCode);

        GetSignal()->SetResultPayload(response);

        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(GetSignal()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn ack sent ok.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlFlashHorn ack send fail.");
        }


        return true;
    }
    /************************************************************/
    // @brief :send response to TSP
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlFlashHorn::SendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status)
    {

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::%s,operationSucceeded:%d,errorCode:%d", 
            __FUNCTION__,result.operationSucceeded,result.errorCode);

        GetSignal()->SetResultPayload(result);
        GetSignal()->SetRmcStatusPayload(RmcResponseTypeRhl, status);
        
        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(GetSignal()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn ack sent ok.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlFlashHorn ack send fail.");
        }


        return true;
    }
    
    /************************************************************/
    // @brief :handle requset result from vuc
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlFlashHorn::HandleResult(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::%s", __FUNCTION__);
        bool ret = true;
        fsm::RemoteCtrlBasicResult result;

        if (fsm::Signal::kHornNLight == signal->GetSignalType())
        {
            std::shared_ptr<fsm::HornNLightSignal> horn_and_light_response = std::static_pointer_cast<fsm::HornNLightSignal>(signal);

            vc::ReturnValue vc_return_value = horn_and_light_response->GetVcStatus();
            vc::HNLReturnCode horn_and_light_return_value = horn_and_light_response->GetData()->return_code;
            uint8_t carLocatorSts = horn_and_light_response->GetData()->carLocatorSts;

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn, return_code:%d, carLocatorSts:%d", horn_and_light_return_value, carLocatorSts);
#if 1 // for Syrn_RemlCtrl_448,523
            if (vc_return_value == vc::RET_OK)
            {
                switch (carLocatorSts)
                {
                case 0:     //Activation_successful
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::success");
                    // Horn and light sequence started, great success!
                    result.operationSucceeded = true;
                    SendResponse(result, m_RMTStatus);
                    break;
                case 1:     //System_1_fail, System_1 = Exterior light
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::System_1_fail");
                    result.operationSucceeded = false;
                    result.errorCode = ErrorCode_incorrectState;
                    result.vehicleErrorCode = carLocatorSts;
                    result.message = "System1(Exterior light) fail";
                    SendResponse(result);

                    break;
                case 2:     //System_2_fail, System_2 = Sound and signaling
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::System_2_fail");
                    result.operationSucceeded = false;
                    result.errorCode = ErrorCode_incorrectState;
                    result.vehicleErrorCode = carLocatorSts;
                    result.message = "System2(Sound and signaling) fail";
                    SendResponse(result);
                    break;
                case 3:     //Idle
                default:    //never run to here
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::other");
                    result.operationSucceeded = false;
                    result.errorCode = ErrorCode_incorrectState;
                    result.vehicleErrorCode = carLocatorSts;
                    result.message = "Idle";
                    SendResponse(result);
                    break;
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlFlashHorn, internal error");
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_executionFailure;
                SendResponse(result);
            }
#else
            if (vc_return_value == vc::RET_OK && horn_and_light_return_value == vc::HL_OK)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlFlashHorn::success");
                // Horn and light sequence started, great success!
                result.operationSucceeded = true;
                SendResponse(result, m_RMTStatus);
            }
            else if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlFlashHorn, internal error");
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_executionFailure;
                SendResponse(result);
            }
            else if (horn_and_light_return_value == vc::HL_SEQUENCE_ONGOING)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlFlashHorn, already ongoing");
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_executionFailure;

                SendResponse(result);
            }
            else if (horn_and_light_return_value == vc::HL_WRONG_USAGE_MODE)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RemoteCtrlFlashHorn, bad state");
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_conditionsNotCorrect;

                SendResponse(result);
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlFlashHorn::unexpected response code");
                // Can not happen at time of writing but safety in case
                // new return codes are added and missed.
                result.operationSucceeded = false;
                result.errorCode = ErrorCode_executionFailure;

                SendResponse(result);
            }
#endif
        }


        return ret;
    }
} // namespace volvo_on_call
