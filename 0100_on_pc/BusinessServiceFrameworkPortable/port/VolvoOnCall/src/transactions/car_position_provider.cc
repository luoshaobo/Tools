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
 *  \file     car_position_provider.cc
 *  \brief    VOC Car position provider transaction base class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/ip_command_broker_signal.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"

#include "features/feature_configs.h"
#include "signals/basic_car_control_signal.h"
#include "signals/cloud_resources.h"
#include "transactions/car_position_provider.h"
#include "transactions/privacy_notification_transaction.h"

#include <cassert>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

CarPositionProvider::CarPositionProvider () : Transaction()
{
}

bool CarPositionProvider::IsCarLocatorEnabled()
{

    bool result = false;

    if (bcc_feature_.IsEnabled())
    {
        std::shared_ptr<BasicCarControlSignal> signal;
        signal = bcc_config_.GetPayload();
        if (signal)
        {
            result = signal->CarLocatorSupported();
        }

    }
    return result;
}

bool CarPositionProvider::IsLocationSharingAllowed()
{
    bool car_stats_upload;
    bool location_services;
    PrivacyNotificationTransaction::GetPrivacySettings(car_stats_upload, location_services);
    return location_services;
}

bool CarPositionProvider::RequestPosition()
{

    IpcbIGNSSService& ipcb_gnss_service_object = fsm::IpCommandBrokerSignalSource::GetInstance().GetIpcbGNSSServiceObject();
    bool ret_val = true;

    //Create request id for IP communication
    //and add it to the list of accepted transactions
    std::shared_ptr<fsm::IpCommandBrokerTransactionId> ip_broker_request_id_ =
            std::make_shared<fsm::IpCommandBrokerTransactionId>();
    MapTransactionId(ip_broker_request_id_);

    //request positions
    //TODO: should we consider it success only if all requests succeed,
    //      or is it enough if any of them succeeds?
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, requesting position information");
    ret_val = ret_val && ipcb_gnss_service_object.DeadReckonedPositionReq(ip_broker_request_id_->GetId());
    ret_val = ret_val && ipcb_gnss_service_object.GNSSPositionDataReq(ip_broker_request_id_->GetId());

    return ret_val;
}

bool CarPositionProvider::ProcessPositionSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Position fsm::Signal processing");
    bool finished = false;
    //save dead reackoned position
    if  (signal->GetSignalType() == fsm::Signal::kDeadReckonedPositionSignal)
    {
        std::shared_ptr<fsm::DeadReckonedPositionSignal> position_signal = std::static_pointer_cast<fsm::DeadReckonedPositionSignal>(signal);
        dead_reckoned_position_ = position_signal->GetPayload();
        received_dead_reckoned_position_ = true;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received dead reckoned position.");

    }
    else if  (signal->GetSignalType() == fsm::Signal::kGNSSPositionDataSignal)
    {
        std::shared_ptr<fsm::GNSSPositionDataSignal> position_signal = std::static_pointer_cast<fsm::GNSSPositionDataSignal>(signal);
        gnss_position_ = position_signal->GetPayload();
        received_gnss_position_ = true;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Received GNSS position.");
    }

    //we should send dead_reckoned_position if corect, otherwise we send gnss position
    //TODO: this logic may need to be adapted in case we need to handle timeouts
    if (received_dead_reckoned_position_)
    {
        bool dead_reckoned_position_sent = false;
        //if valid dead reckon position position then use it
        //TODO: check if we should additionally consider DeadReckonedType == noDr as not relevant input
        if (dead_reckoned_position_.noValidData != 0)
        {
            dead_reckoned_position_sent = SendDeadReckonedPosition(dead_reckoned_position_.drPosition);
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, dead reckoned position invalid");
        }

        //if position sent: finalize
        if (dead_reckoned_position_sent)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, dead reckoned position sent");
            finished = true;
        }
        else
        {
            //if for some reason we have not sent the dead reckoned position then we should
            //use gnss (if already present, if not, just keep waiting for it in the same state)
            if (received_gnss_position_)
            {
                //try to use GNSS position, otherwise report error
                if ( !SendGNSSPosition(gnss_position_))
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, gnss position sending failed, reporting error");
                    SendError(fsm::ResponseInfoData::ResponseCodeError::kInternalError);
                }
                else
                {
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, gnss position sent");
                }
                //at this point both positions available, finalize
                //as we have either successfully transmitted the postion
                //or reported an error
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, position reporting completed");
                finished = true;
            }
            else
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, awaiting gnss position");
            }
        }

    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CarPositionProvider, awaiting dead reckoned position");
    }

    return finished;
}


void CarPositionProvider::SendError(fsm::ResponseInfoData::ResponseCodeError error_code, std::string message)
{

}

void CarPositionProvider::ResetTransaction()
{
    received_dead_reckoned_position_ = false;
    received_gnss_position_ = false;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
