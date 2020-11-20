/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     car_locator_map_request_transaction.cc
 *  \brief    VOC Service car locator map request transaction.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */


#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"

#include "signals/bcc_cl_011_signal.h"
#include "signals/bcc_cl_020_signal.h"
#include "signals/signal_types.h"
#include "transactions/car_locator_map_request_transaction.h"
#include "transactions/privacy_notification_transaction.h"


#ifndef VOC_TESTS
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#endif


#include <algorithm>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);


namespace volvo_on_call
{

CarLocatorMapRequestTransaction::CarLocatorMapRequestTransaction () : CarPositionProvider()
{
}

bool CarLocatorMapRequestTransaction::WantsSignal(std::shared_ptr<fsm::Signal> signal)
{
    // First check if it matches our mapped transaction ids.
    bool id_match = IsMappedTransactionId(signal->GetTransactionId());

    bool expected_signal = false;  //track if the signal is expected in this transaction

    //check if signal is expected in this transaction
    switch (signal->GetSignalType())
    {
        case VocSignalTypes::kBccCl010Signal :
        {
            if (state_ == kNew)
            {
                //special case, accept regardless of transaction id mapping
                id_match = true;
                expected_signal = true;
            }
            break;
        }
#ifndef VOC_TESTS
        case fsm::Signal::kCarMode :
        case fsm::Signal::kCarUsageMode :
        {
            expected_signal = true;
        }
#endif
        case (fsm::Signal::kDeadReckonedPositionSignal) :  //IHU position signal (map matched or dead reckoned)
        case (fsm::Signal::kGNSSPositionDataSignal) :      //TCAM position
        {
            expected_signal = true;
            break;
        }
        default:
            expected_signal = false;
    }

    //the signal must be of expected type and must have a matching transaction id
    if (id_match && expected_signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, accepting signal: %s", signal->ToString().c_str());
        return true;
    }
    else
    {
        return false;
    }
}

bool CarLocatorMapRequestTransaction::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, incoming signal: %s", signal->ToString().c_str());
#ifndef VOC_TESTS
    switch (signal->GetSignalType())
    {
        case VocSignalTypes::kBccCl010Signal :
        {
            HandleBccCl010Signal(std::static_pointer_cast<BccCl010Signal>(signal));
            break;
        }
        case (fsm::Signal::kCarMode) :
        case (fsm::Signal::kCarUsageMode) :
        {
            HandleVehicleCommSignal(signal);
            break;
        }
        case (fsm::Signal::kDeadReckonedPositionSignal) :
        case (fsm::Signal::kGNSSPositionDataSignal) :
        {
            HandlePosition(signal);
            break;
        }
        default :
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "CarLocatorMapRequestTransaction, received "
                                                   "unexpected signal %s, ignoring.", signal->ToString().c_str());
    }
#else
    state_ = kDone;
#endif

    //if done then terminate the transaction
    return (state_ != kDone);
    
}


void CarLocatorMapRequestTransaction::HandleBccCl010Signal(std::shared_ptr<BccCl010Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "BCC-CL-010");

    // This is the originating request, we only expect it in the new state.
    if (state_ == kNew)
    {
        // we need to save the request
        request_ = signal;

        //set state in order to prohibit accepting other BCCCL010 signals
        state_ = kCheckPreconditions;

        //Check if preconditions are met
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "CarLocatorMapRequestTransaction, checking car locator setting.");
        if (!IsCarLocatorEnabled())
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                           "CarLocatorMapRequestTransaction, car locator not enabled in the configuration");
            state_ = kDone;
            SendError(fsm::ResponseInfoData::ResponseCodeError::kServiceUnavailable,
                      "Service Not Available Due To Provisioning.");
        }
        else
        {
            //check privacy settings
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                           "CarLocatorMapRequestTransaction, checking privacy settings.");
            if (!IsLocationSharingAllowed())
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "CarLocatorMapRequestTransaction, location sharing not allowed, ignoring the request");
                state_ = kDone;
                SendError(fsm::ResponseInfoData::ResponseCodeError::kServiceUnavailable,
                          "Service Not Available Due To User Settings(Privacy).");
            }
        }

        //if we are allowed to continue
        if (state_ != kDone)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                           "CarLocatorMapRequestTransaction, location sharing allowed.");

            //send requests for car status information to confirm that the car is stationary

            //Create transaction id for Vehicle services and add it
            //to the list of accepted transactions
            std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
                std::make_shared<fsm::VehicleCommTransactionId>();
            MapTransactionId(vehicle_comm_request_id);

            fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
            vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                           "CarLocatorMapRequestTransaction, requesting modes");
            //request car and usage mode
            if ( (igen.Request_CarMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
                 (igen.Request_CarUsageMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) )
            {
                state_ = kRequestSentToCar;
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                               "CarLocatorMapRequestTransaction, requests for car and usage mode sent to VehicleComm.");
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                               "CarLocatorMapRequestTransaction, failed to issue request to VehicleComm, aborting transaction.");
                SendError(fsm::ResponseInfoData::ResponseCodeError::kForbidden);
                state_ = kDone;
            }

        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "CarLocatorMapRequestTransaction, received BCC-CL-010 in bad state, ignoring.");
    }
}


void CarLocatorMapRequestTransaction::HandleVehicleCommSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, VehicleComm signal");

    //the only VC messages we expect are responses to our car and usage mode queries
    switch (signal->GetSignalType())
    {
    case fsm::Signal::kCarMode:
    {
        CheckCarModePrecondition(signal);
        break;
    }
    case fsm::Signal::kCarUsageMode:
    {
        CheckCarUsageModePrecondition(signal);
        break;
    }
    default:
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "CarLocatorMapRequestTransaction, received unexpected VehicleCommSignal, ignoring.");
    }

    //when we have all the car state data evaluated positively we should request
    //position
    if (car_mode_ && car_usage_mode_)
    {
        if (!RequestPosition())
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request to IpCommandBroker, aborting transaction.");
            SendError(fsm::ResponseInfoData::ResponseCodeError::kInternalError);
            state_ = kDone;
        }
        else
        {
            state_ = kRequestForPositon;
        }
    }

}


void CarLocatorMapRequestTransaction::HandlePosition(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, position signal");

    //check if the state is right for this command
    if (state_ == kRequestForPositon)
    {
        //if position has been sent, finalize
        if (ProcessPositionSignal(signal) == true)
        {
            state_ = kDone;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                      "CarLocatorMapRequestTransaction, received position signal in a bad state, ignoring.");
    }

}


void CarLocatorMapRequestTransaction::CheckCarModePrecondition(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, VCRES_CARMODE");
    if (state_ == kRequestSentToCar)
    {
        // cast to the correct message class:
        std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);

        // ToDo: check for NULL pointer missing.
        vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();

        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request to VehicleComm, aborting transaction.");
            SendError(fsm::ResponseInfoData::ResponseCodeError::kInternalError);
            state_ = kDone;
        }

        vc::CarModeState car_mode_return =
            car_mode_response->GetData()->carmode;

        //car mode must be normal, otherwise MapRequest must be denied
        if (car_mode_return == vc::CAR_NORMAL)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, car mode OK");
            car_mode_ = true;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, car mode NOK");
            SendError(fsm::ResponseInfoData::ResponseCodeError::kForbidden);
            state_ = kDone;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "CarLocatorMapRequestTransaction, received VCRES_CARMODE in bad "
                       "state, ignoring.");
    }

}

void CarLocatorMapRequestTransaction::CheckCarUsageModePrecondition(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "VCRES_CARUSAGEMODE:");
    if (state_ == kRequestSentToCar)
    {
        // cast to the correct message class:
        std::shared_ptr<fsm::CarUsageModeSignal> car_usage_mode_response = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);

        // ToDo: check for NULL pointer missing.
        vc::ReturnValue vc_return_value = car_usage_mode_response->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request to VehicleComm, aborting transaction.");
            SendError(fsm::ResponseInfoData::ResponseCodeError::kInternalError);
            state_ = kDone;
        }

        vc::CarUsageModeState car_usage_mode_return = car_usage_mode_response->GetData()->usagemode;

        //car usage mode must be either of the following: abandoned, inactive, conveninence or active
        //otherwise MapRequest must be denied
        switch (car_usage_mode_return)
        {
        case vc::CAR_ABANDONED:
        case vc::CAR_INACTIVE:
        case vc::CAR_USGMODCNVINC:
        case vc::CAR_ACTIVE:
        {
             DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, car usage mode OK");
             car_usage_mode_ = true;
             break;
        }
        default:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, car usage mode NOK");
            SendError(fsm::ResponseInfoData::ResponseCodeError::kForbidden);
            state_ = kDone;
        }
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "CarLocatorMapRequestTransaction, received VCRES_CARUSAGEMODE in bad "
                       "state, ignoring.");
    }

}


void CarLocatorMapRequestTransaction::SendError(fsm::ResponseInfoData::ResponseCodeError error_code, std::string message)
{
    std::string session_id;

    fsm::CCMTransactionId transaction_id =
        dynamic_cast<const fsm::CCMTransactionId&>(request_->GetTransactionId());

    std::shared_ptr<BccCl011Signal> response = BccCl011Signal::CreateBccCl011Signal(transaction_id,
                                                                                    1);

    request_->GetSessionId(session_id);
    response->SetSessionId(session_id);  //will be silently ignored if invalid

    response->SetError(error_code, message);

    response->SetRecipients({request_->GetSender()});

    fsm::VocmoSignalSource vocmo_signal_source;

    if (vocmo_signal_source.SendMessage(response, request_->GetTopicPriority()))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Error message sent.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorMapRequestTransaction, "
                                               "Failed to send error message.");
    }

    state_ = kDone;
}


bool CarLocatorMapRequestTransaction::SendPositionUpdate(DRPositionData *dead_reckoned_position, GNSSData *gnss_position)
{
    bool return_value = true;

    // first send response

    fsm::CCMTransactionId transaction_id =
        dynamic_cast<const fsm::CCMTransactionId&>(request_->GetTransactionId());

    std::shared_ptr<BccCl011Signal> response = BccCl011Signal::CreateBccCl011Signal(transaction_id,
                                                                                    1);

    std::string session_id;
    request_->GetSessionId(session_id);
    response->SetSessionId(session_id);  //will be silently ignored if invalid
    response->SetSuccess();


    if (nullptr != dead_reckoned_position)
    {
        response->SetFromDRPositionData(*dead_reckoned_position);
    }
    else if (nullptr != gnss_position)
    {
        response->SetFromGNSSData(*gnss_position);
    }
    else
    {
        return_value = false;

        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorMapRequestTransaction, fail to set response.");
    }

    fsm::VocmoSignalSource vocmo_signal_source;

    if (return_value)
    {
        fsm::UserId sender = request_->GetSender();

        if (fsm::kUndefinedUserId != sender)
        {
            response->SetRecipients({request_->GetSender()});

            if (vocmo_signal_source.SendMessage(response, request_->GetTopicPriority()))
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, response sent.");
            }
            else
            {
                return_value = false;

                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorMapRequestTransaction, "
                               "Failed to send reply.");
            }
        }
        else
        {
            return_value = false;

            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorMapRequestTransaction, "
                           "No user to send respose to.");
        }
    }

    // then send notification to all other users

    if (return_value)
    {
        fsm::CCMTransactionId notification_transaction_id;

        std::shared_ptr<BccCl020Signal> notification =
            BccCl020Signal::CreateBccCl020Signal(notification_transaction_id,
                                                 1);

        if (nullptr != dead_reckoned_position)
        {
            notification->SetFromDRPositionData(*dead_reckoned_position);
        }
        else if (nullptr != gnss_position)
        {
            notification->SetFromGNSSData(*gnss_position);
        }
        else
        {
            return_value = false;

            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorMapRequestTransaction, fail to set notification");
        }

        if (return_value)
        {
            std::vector<fsm::UserId> users;
            vocmo_signal_source.GetAllPairedUsers(users);
            users.erase(std::remove(users.begin(), users.end(), request_->GetSender()), users.end());
            if (users.size())
            {
                notification->SetRecipients(users);

                if (vocmo_signal_source.SendMessage(notification))
                {
                    state_ = kDone;

                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                                   "CarLocatorMapRequestTransaction, notification sent.");
                }
                else
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CarLocatorMapRequestTransaction, "
                                   "Failed to send notification.");
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorMapRequestTransaction, "
                               "No other users to send notification to.");
            }
        }

    }

    return return_value;
}


bool CarLocatorMapRequestTransaction::SendDeadReckonedPosition(DRPositionData& dead_reckoned_position)
{
    return CarLocatorMapRequestTransaction::SendPositionUpdate(&dead_reckoned_position, nullptr);
}

bool CarLocatorMapRequestTransaction::SendGNSSPosition(GNSSData& gnss_position)
{
    return CarLocatorMapRequestTransaction::SendPositionUpdate(nullptr, &gnss_position);
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
