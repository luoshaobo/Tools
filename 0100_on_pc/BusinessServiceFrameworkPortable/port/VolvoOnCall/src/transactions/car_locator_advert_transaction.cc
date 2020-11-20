/**
 * Copyright (C) 2016 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     car_locator_advert_transaction.cc
 *  \brief    VOC Service car locator advert transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "features/feature_configs.h"
#include "signals/bcc_cl_002_signal.h"
#include "signals/basic_car_control_signal.h"
#include "signals/cloud_resources.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#ifndef VOC_TESTS
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#endif
#include "transactions/car_locator_advert_transaction.h"

#include <cassert>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

CarLocatorAdvertTransaction::CarLocatorAdvertTransaction () :
    Transaction()
{
}

bool CarLocatorAdvertTransaction::WantsSignal(std::shared_ptr<fsm::Signal> signal)
{
    // First check if it matches our mapped transaction ids.
    bool relevant_signal = IsMappedTransactionId(signal->GetTransactionId());

    // Check that specific signal type is expected by this transaction.
    // Note that except for the special new case we do not try to match
    // type to state here, this is beacuse we do not now what might
    // already be in the queue and what the state will be once its
    // actually the singals turn to be handled. State based rejection
    // must happen in HandleSignal.
    switch (signal->GetSignalType())
    {
    case VocSignalTypes::kBccCl001Signal :
    {
        if (state_ == kNew)
        {
            //special case, accept regardless of transaction id mapping
            relevant_signal = true;
        }
        break;
    }
#ifndef VOC_TESTS
    case fsm::Signal::kHornNLight :
    {
        // here we rely on result of IsMappedTransactionId
        break;
    }
#endif
    default:
        relevant_signal = false;
    }
    return relevant_signal;
}

bool CarLocatorAdvertTransaction::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{
    bool is_transaction_finished = false;

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorAdvertTransaction, incomming signal.");
#ifndef VOC_TESTS
    switch (signal->GetSignalType())
    {
        case VocSignalTypes::kBccCl001Signal :
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "BCC-CL-001");

            // This is the originating request, we only expect it in the new state.
            if (state_ == kNew)
            {
                std::shared_ptr<BccCl001Signal> bcc_cl001_signal =
                    std::static_pointer_cast<BccCl001Signal>(signal);

                is_transaction_finished = HandleBccCl001(bcc_cl001_signal);
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CarLocatorAdvertTransaction, received BCC-CL-001 "
                                                      "in bad state, ignoring.");
            }
            break;
        } // case Signal::kBccCl001Signal
        case fsm::Signal::kHornNLight:
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "kHornNLight");
            if (state_ == kRequestSentToCar)
            {
                std::shared_ptr<fsm::HornNLightSignal> horn_and_light_response = std::static_pointer_cast<fsm::HornNLightSignal>(signal);
                //ToDo: error handling missing in case the pointer 'horn_and_light_response' is NULL
                vc::ReturnValue vc_return_value = horn_and_light_response->GetVcStatus();
                vc::HNLReturnCode horn_and_light_return_value = horn_and_light_response->GetData()->return_code;
                is_transaction_finished = HandleHornAndLightResponse(vc_return_value, horn_and_light_return_value);
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,\
                               "CarLocatorAdvertTransaction, received VCRES_HORNNLIGHT in bad "\
                               "state, ignoring.");
            }
            break;
        }  // case fsm::Signal::kHornNLight
        default :
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CarLocatorAdvertTransaction, received "
                                                  "unexpected signal, ignoring.");
    }
#else
    is_transaction_finished = true;
#endif

    // If we are finished we return false to close ourselves down,
    // otherwise we return true to indicate we are not done.
    return !is_transaction_finished;
}
#ifndef VOC_TESTS
bool CarLocatorAdvertTransaction::HandleBccCl001(std::shared_ptr<BccCl001Signal> bcc_cl001_signal)
{

    bool is_transaction_finished = false;
    bool is_car_locator_allowed = false;

    // Check if car locator functionality is allowed
    BccFunc002CloudResource bcc_config;
    BasicCarControlFeatureConfig bcc_feature;

    if (bcc_feature.IsEnabled())
    {
        std::shared_ptr<BasicCarControlSignal> signal;
        signal = bcc_config.GetPayload();
        if (signal)
        {
             is_car_locator_allowed = signal->CarLocatorSupported();
        }
    }

    // Store the initial request
    request_ = bcc_cl001_signal;

    if (!is_car_locator_allowed)
    {
        //if car locator not allowed report error
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Car locator not allowed by cloud configuration");
        is_transaction_finished = true;
        SendError(fsm::ResponseInfoData::ResponseCodeError::kServiceUnavailable,
                  "Service Not Available Due To Provisioning");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Car locator allowed by cloud configuration");

        // Send request to VehicleComm
        vc::ReqHornNLight request = {};

        // First setup request parameters.
        switch (bcc_cl001_signal->GetSupportedAdvert())
        {
        case BccCl001Signal::Honk:
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to perform Honk");
            request.mode = vc::HL_REQUEST_HORN;
            break;
        case BccCl001Signal::Flash:
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to perform Flash");
            request.mode = vc::HL_REQUEST_LIGHT;
            break;
        case BccCl001Signal::HonkAndFlash:
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received request to perform HonkAndFlash");
            request.mode = vc::HL_REQUEST_HORN_AND_LIGHT;
            break;
        default:
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                           "Received unknown car advert request, aborting transaction.");
            SendError();
            state_ = kDone;
            is_transaction_finished = true;
        }


        if (!is_transaction_finished)
        {
            // Create and map a VehicleComm transaction id.
            std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
                    std::make_shared<fsm::VehicleCommTransactionId>();
            MapTransactionId(vehicle_comm_request_id);

            // Send the request
            fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
            vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

            if (igen.Request_HornAndLight(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Request sent to VehicleComm.");
                state_ = kRequestSentToCar;
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request, aborting transaction.");
                SendError();
                state_ = kDone;
                is_transaction_finished = true;
            }
        }
    }
    return is_transaction_finished;
}

bool CarLocatorAdvertTransaction::HandleHornAndLightResponse(vc::ReturnValue vc_return_value,
                                                             vc::HNLReturnCode horn_and_light_return_value)
{
    if (vc_return_value == vc::RET_OK && horn_and_light_return_value == vc::HL_OK)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "success");
        // Horn and light sequence started, great success!
        SendSuccess();
    }
    else if (vc_return_value != vc::RET_OK)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CarLocatorAdvertTransaction, internal error");
        SendError();
    }
    else if (horn_and_light_return_value == vc::HL_SEQUENCE_ONGOING)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CarLocatorAdvertTransaction, already ongoing");
        SendError(fsm::ResponseInfoData::ResponseCodeError::kServiceUnavailable,
                  "Operation already ongoing.");
    }
    else if (horn_and_light_return_value == vc::HL_WRONG_USAGE_MODE)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "CarLocatorAdvertTransaction, bad state");
        SendError(fsm::ResponseInfoData::ResponseCodeError::kServiceUnavailable,
                  "Bad state.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorAdvertTransaction, "
                                               "unexpected response code");
        // Can not happen at time of writing but safety in case
        // new return codes are added and missed.
        SendError();
    }

    state_ = kDone;

    return true;
}
#endif

void CarLocatorAdvertTransaction::SendError(fsm::ResponseInfoData::ResponseCodeError error_code,
                                            const std::string& message)
{
    std::string session_id;

    fsm::CCMTransactionId transaction_id =
        dynamic_cast<const fsm::CCMTransactionId&>(request_->GetTransactionId());

    std::shared_ptr<BccCl002Signal> response = BccCl002Signal::CreateBccCl002Signal(transaction_id,
                                                                                    1);

    request_->GetSessionId(session_id);
    response->SetSessionId(session_id);  //will be silently ignored if invalid

    response->SetError(error_code, message);

    response->SetRecipients({request_->GetSender()});

    fsm::VocmoSignalSource vocmo_signal_source;

    if (vocmo_signal_source.SendMessage(response,
                                        request_->GetTopicPriority()))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Response sent.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorAdvertTransaction, "
                                               "Failed to send reply.");
    }
}

void CarLocatorAdvertTransaction::SendSuccess()
{
    std::string session_id;

    fsm::CCMTransactionId transaction_id =
        dynamic_cast<const fsm::CCMTransactionId&>(request_->GetTransactionId());

    std::shared_ptr<BccCl002Signal> response = BccCl002Signal::CreateBccCl002Signal(transaction_id,
                                                                                    1);

    request_->GetSessionId(session_id);
    response->SetSessionId(session_id);  //will be silently ignored if invalid

    response->SetSuccess(fsm::ResponseInfoData::ResponseCodeSuccess::kUnspecified);

    response->SetRecipients({request_->GetSender()});

    fsm::VocmoSignalSource vocmo_signal_source;

    if (vocmo_signal_source.SendMessage(response,
                                        request_->GetTopicPriority()))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Response sent.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorAdvertTransaction, "
                                               "Failed to send reply.");
    }
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
