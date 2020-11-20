///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file remote_ctrl_window_roof.cc
//	This file handles the Remote Control Windows and Sunroof open/close for application.
// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Aug-2018
///////////////////////////////////////////////////////////////////
#include "remote_ctrl/remote_ctrl_window_roof.h"
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
    RemoteCtrlWinRoof::RemoteCtrlWinRoof(std::shared_ptr<fsm::Signal> initial_signal)
        : RemoteCtrlCommon(initial_signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s", __FUNCTION__);
        if (fsm::kVDServiceRws == initial_signal->GetSignalType())
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
    bool RemoteCtrlWinRoof::SendRequest()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::RemoteCtrlBasicRequest> rcbRequest = GetSignal()->GetRequestPayload();
        
        if (m_bIsActiveService && rcbRequest)
        {
            fsm::RWSType reqType = static_cast<fsm::RWSType>(rcbRequest->value);

            switch (reqType)
            {
            case fsm::CloseWindowOnly:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to CloseWindowOnly");
                    if (rcbRequest->pos < 0)
                    {
                        rcbRequest->pos = 100;
                    }
                    ret = WindowControl(false, rcbRequest->pos);
                    break;
                }

            case fsm::CloseSunroofOnly:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to CloseSunroofOnly");
                    ret = SunroofControl(false);
                    break;
                }

            case fsm::CloseBoth:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to CloseBoth");
                    if (rcbRequest->pos < 0)
                    {
                        rcbRequest->pos = 100;
                    }
                    ret = WindowControl(false, rcbRequest->pos);
                    ret |= SunroofControl(false);
                    break;
                }
                break;
            case fsm::OpenWindowOnly:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to OpenWindowOnly");
                    if (rcbRequest->pos < 0)
                    {
                        rcbRequest->pos = 100;
                    }
                    ret = WindowControl(true, rcbRequest->pos);
                    break;
                }
            case fsm::OpenSunroofOnly:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to OpenSunroofOnly");
                    ret = SunroofControl(true);
                    break;
                }

            case fsm::OpenBoth:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to OpenBoth");
                    if (rcbRequest->pos < 0)
                    {
                        rcbRequest->pos = 100;
                    }
                    ret = WindowControl(true, rcbRequest->pos);
                    ret |= SunroofControl(true);
                    break;
                }

            case fsm::OpenSunshade:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to OpenSunshade");
                    ret = CurtainControl(true);
                    break;
                }

            case fsm::CloseSunshade:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to CloseSunshade");
                    ret = CurtainControl(false);
                    break;
                }

            case fsm::Ventilate:
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to Ventilate");
                    if (rcbRequest->pos < 0)
                    {
                        rcbRequest->pos = 20;
                    }
                    ret = WindowControl(true, rcbRequest->pos);
                    break;
                }

            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Received unknown request : %d, aborting.", reqType);
            }

        }
        else 
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "paameter error, m_bIsActiveService = %d", m_bIsActiveService);
        }

        return ret;
    }

    /************************************************************/
    // @brief :send response to TSP
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlWinRoof::SendResponse(fsm::RemoteCtrlBasicResult result, fsm::RemoteCtrlBasicStatus status)
    {

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlDoorTail::%s,operationSucceeded:%d,errorCode:%d", 
            __FUNCTION__,result.operationSucceeded,result.errorCode);

        GetSignal()->SetResultPayload(result);
        GetSignal()->SetRmcStatusPayload(RmcResponseTypeWin, status);
        
        fsm::VocmoSignalSource vocmo_signal_source;
        if (vocmo_signal_source.GeelySendMessage(GetSignal()))
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof ack sent ok.");
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RemoteCtrlWinRoof ack send fail.");
        }


        return true;
    }
    /************************************************************/
    // @brief :handle requset result from vuc
    // @param[in]  
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlWinRoof::HandleResult(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s", __FUNCTION__);
        bool ret = true;
        fsm::RemoteCtrlBasicResult result;

        if (fsm::Signal::kWinOpenStatusSignal == signal->GetSignalType())
        {
            result.operationSucceeded = true;
            
            std::shared_ptr<fsm::WinOpenStatusSignal> winStatus = std::static_pointer_cast<fsm::WinOpenStatusSignal>(signal);

            m_RMTStatus.winStatusDriver = TransWinStatusToSignalType(winStatus->GetData()->winPosnStsAtDrv);
            m_RMTStatus.winStatusPassenger = TransWinStatusToSignalType(winStatus->GetData()->winPosnStsAtPass);
            m_RMTStatus.winStatusDriverRear = TransWinStatusToSignalType(winStatus->GetData()->winPosnStsReLe);
            m_RMTStatus.winStatusPassengerRear = TransWinStatusToSignalType(winStatus->GetData()->winPosnStsReRi);
            m_RMTStatus.sunroofOpenStatus = TransWinStatusToSignalType(winStatus->GetData()->sunroofPosnSts);

            m_RMTStatus.sunroofPos = winStatus->GetData()->sunroofPosnSts;
            m_RMTStatus.winPosDriver = winStatus->GetData()->winPosnStsAtDrv;
            m_RMTStatus.winPosPassenger = winStatus->GetData()->winPosnStsAtPass;
            m_RMTStatus.winPosDriverRear = winStatus->GetData()->winPosnStsReLe;
            m_RMTStatus.winPosPassengerRear = winStatus->GetData()->winPosnStsReRi;

            GetSignal()->SetResultPayload(result);
            GetSignal()->SetRmcStatusPayload(RmcResponseTypeWin, m_RMTStatus);

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

    /************************************************************/
    // @brief :send windos control message to vuc
    // @param[in]  true: open; false: close
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlWinRoof::WindowControl(bool bOpen, uint8_t value)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s, bOpen = %d, value = %d", __FUNCTION__, bOpen, value);
        bool ret = false;
        vc::ReturnValue rv = vc::RET_OK;
        vc::ReqWinCtrl request;
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        // Create and map a VehicleComm transaction id.
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

        MapTransactionId(vehicle_comm_request_id);


        if (bOpen)
        {
            request.mode = vc::WIN_OPEN_REQ_OPEN;
        }
        else
        {
            request.mode = vc::WIN_OPEN_REQ_CLOSE;
        }
        request.openValue = value;
        rv = igen.Request_WindowsCtrl(&request, vehicle_comm_request_id->GetId());

        if (vc::RET_OK == rv)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Request sent to VehicleComm.");
            ret = true;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request, aborting transaction.");
        }

        return ret;

    }

    /************************************************************/
    // @brief :send sunroof control message to vuc
    // @param[in]  true: open; false: close
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlWinRoof::SunroofControl(bool bOpen)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s, bOpen = %d", __FUNCTION__, bOpen);
        bool ret = false;
        vc::ReturnValue rv = vc::RET_OK;
        vc::ReqRoofCurtCtrl request;
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        // Create and map a VehicleComm transaction id.
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

        MapTransactionId(vehicle_comm_request_id);


        if (bOpen)
        {
            request.mode = vc::ROOF_CURT_CTRL_REQ_OPEN_PANEL;
        }
        else
        {
            request.mode = vc::ROOF_CURT_CTRL_REQ_CLOSE_PANEL;
        }

        rv = igen.Request_SunRoofAndCurtCtrl(&request, vehicle_comm_request_id->GetId());

        if (vc::RET_OK == rv)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Request sent to VehicleComm.");
            ret = true;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request, aborting transaction.");
        }

        return ret;
    }

    /************************************************************/
    // @brief :send cuntain control message to vuc
    // @param[in]  true: open; false: close
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlWinRoof::CurtainControl(bool bOpen)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s, bOpen = %d", __FUNCTION__, bOpen);
        bool ret = false;
        vc::ReturnValue rv = vc::RET_OK;
        vc::ReqRoofCurtCtrl request;
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        // Create and map a VehicleComm transaction id.
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

        MapTransactionId(vehicle_comm_request_id);


        if (bOpen)
        {
            request.mode = vc::ROOF_CURT_CTRL_REQ_OPEN_CURTAIN;
        }
        else
        {
            request.mode = vc::ROOF_CURT_CTRL_REQ_CLOSE_CURTAIN;
        }

        rv = igen.Request_SunRoofAndCurtCtrl(&request, vehicle_comm_request_id->GetId());

        if (vc::RET_OK == rv)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Request sent to VehicleComm.");
            ret = true;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request, aborting transaction.");
        }

        return ret;
    }

    /************************************************************/
    // @brief :send ventilation control message to vuc
    // @param[in]  mode
    // @param[in]  value , todo: unknown value
    // @return     True: success, False:Failed
    // @author     Nie Yujin
    /************************************************************/
    bool RemoteCtrlWinRoof::VentilationControl(int mode, int value)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RemoteCtrlWinRoof::%s, mode = %d, value = %d", __FUNCTION__, mode, value);
        bool ret = false;
        vc::ReturnValue rv = vc::RET_OK;
        vc::ReqWinVentilation request;
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

        // Create and map a VehicleComm transaction id.
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();

        MapTransactionId(vehicle_comm_request_id);

        request.mode = vc::WIN_VENTI_REQ_OPEN;
        request.value = value;

        rv = igen.Request_WinVentilation(&request, vehicle_comm_request_id->GetId());

        if (vc::RET_OK == rv)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Request sent to VehicleComm.");
            ret = true;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request, aborting transaction.");
        }

        return ret;
    }
} // namespace volvo_on_call
