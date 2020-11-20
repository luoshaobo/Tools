/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     car_locator_position_update_transaction.cc
 *  \brief    VOC Service car locator position update transaction.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/bcc_cl_020_signal.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#ifndef VOC_TESTS
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#endif
#include "transactions/privacy_notification_transaction.h"
#include "transactions/car_locator_position_update_transaction.h"

#include <cassert>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

CarLocatorPositionUpdateTransaction::CarLocatorPositionUpdateTransaction () : CarPositionProvider()
{
}

bool CarLocatorPositionUpdateTransaction::WantsSignal(std::shared_ptr<fsm::Signal> signal)
{
    // First check if it matches our mapped transaction ids.
    bool id_match = IsMappedTransactionId(signal->GetTransactionId());

    bool expected_signal = false;  //track if the signal is expected in this transaction

    //check if signal is expected in this transaction
    switch (signal->GetSignalType())
    {
#ifndef VOC_TESTS
        case (fsm::Signal::kEndOfTrip) :
        case (volvo_on_call::VocSignalTypes::kPositionUpdateInternalSignal) :
        {
            expected_signal = true;
            id_match = true;  // special case, we always accept any of position update request signals
                              // as this is an event triggering this transaction
            break;
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
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, accepting signal: %s", signal->ToString().c_str());
        return true;
    }
    else
    {
        return false;
    }
}

bool CarLocatorPositionUpdateTransaction::HandleSignal(std::shared_ptr<fsm::Signal> signal)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, incoming signal: %s", signal->ToString().c_str());
#ifndef VOC_TESTS
    //check again if the signal is appropriate, transaction might have been reset in between Wants and Handle signal
    //also check if location sharing is allowed
    if ( WantsSignal(signal) )
    {
        if (IsCarLocatorEnabled() && IsLocationSharingAllowed() )
        {
            switch (signal->GetSignalType())
            {
                case (fsm::Signal::kEndOfTrip) :
                case (volvo_on_call::VocSignalTypes::kPositionUpdateInternalSignal) :
                {
                    HandlePositionUpdateRequest(signal);
                    break;
                }
                case (fsm::Signal::kDeadReckonedPositionSignal) :
                case (fsm::Signal::kGNSSPositionDataSignal) :
                {
                    HandlePosition(signal);
                    break;
                }
                default :
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "CarLocatorPositionUpdateTransaction, received "
                                                           "unexpected signal %s, ignoring.", signal->ToString().c_str());
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Location reporting not allowed by the configuration");

        }

    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, Got signal that is not relevant anymore: ", signal->ToString().c_str());
    }
#else
    ResetTransaction();
#endif

    //long-lived transaction
    return true;

}

void CarLocatorPositionUpdateTransaction::HandlePositionUpdateRequest(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, VehicleComm signal");

    fsm::Signal::SignalType signal_type = signal->GetSignalType();

    if  (signal_type == fsm::Signal::kEndOfTrip)
    {
        // In case of End Of Trip we'll send position update to all paired users
        fsm::VocmoSignalSource vocmo_signal_source;

        vocmo_signal_source.GetAllPairedUsers(recipients_);
    }
    else if (signal_type == volvo_on_call::VocSignalTypes::kPositionUpdateInternalSignal)
    {
        // In case of InternalSignalPositionUpdate add it's recipients to recipients_
        std::shared_ptr<fsm::InternalSignal<std::vector<fsm::UserId>>> position_update_request =
            std::static_pointer_cast<fsm::InternalSignal<std::vector<fsm::UserId>>>(signal);

        std::vector<fsm::UserId> recipients = *position_update_request->GetPayload();

        for (auto& recipient : recipients)
        {
            if (recipients_.end() == std::find(recipients_.begin(), recipients_.end(), recipient))
            {
                recipients_.push_back(recipient);
            }
        }
    }

    // Request position only if we are in kWaitForTrigger state
    if (kWaitForTrigger == state_)
    {
        if (!RequestPosition())
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "Failed to issue request to IpCommandBroker, reseting transaction.");
            ResetTransaction();
        }
        else
        {
            state_ = kRequestForPositon;
        }
    }
}

void CarLocatorPositionUpdateTransaction::ResetTransaction()
{
    CarPositionProvider::ResetTransaction();
    state_ = kWaitForTrigger;
    RemoveMappedTransactions();
    recipients_.clear();
}

void CarLocatorPositionUpdateTransaction::HandlePosition(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, position signal");

    //check if the state is right for this command
    if (state_ == kRequestForPositon)
    {
        //if position has been sent, finalize
        if (ProcessPositionSignal(signal) == true)
        {
            ResetTransaction();
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                      "CarLocatorPositionUpdateTransaction, received position signal in a bad state, ignoring.");
    }

}

bool CarLocatorPositionUpdateTransaction::SendDeadReckonedPosition(DRPositionData& dead_reckoned_position)
{
    std::shared_ptr<BccCl020Signal> response =
        BccCl020Signal::CreateBccCl020Signal(request_transaction_id_,
                                             GetSequenceNumber());

    response->SetFromDRPositionData(dead_reckoned_position);

    fsm::VocmoSignalSource vocmo_signal_source;

    response->SetRecipients(recipients_);

    if (vocmo_signal_source.SendMessage(response))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, response sent.");
        return true;
    }


    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorPositionUpdateTransaction, "
                   "Failed to send reply.");
    return false;
}

bool CarLocatorPositionUpdateTransaction::SendGNSSPosition(GNSSData& gnss_position)
{
    std::shared_ptr<BccCl020Signal> response
        = BccCl020Signal::CreateBccCl020Signal(request_transaction_id_,
                                               GetSequenceNumber());

    response->SetFromGNSSData(gnss_position);

    fsm::VocmoSignalSource vocmo_signal_source;

    response->SetRecipients(recipients_);

    if (vocmo_signal_source.SendMessage(response))
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarLocatorPositionUpdateTransaction, response sent.");
        return true;
    }


    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "CarLocatorPositionUpdateTransaction, "
                   "Failed to send reply.");
    return false;
}

uint16_t CarLocatorPositionUpdateTransaction::GetSequenceNumber()
{
    return sequence_number_++;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
