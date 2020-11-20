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
 *  \file     car_locator_advert_transaction.h
 *  \brief    VOC Service car position provider base class
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_CAR_POSITION_PROVIDER_H_
#define VOC_TRANSACTIONS_CAR_POSITION_PROVIDER_H_

#include "voc_framework/transactions/transaction.h"
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/response_info_data.h"

#include "ipcb_IGNSSService.hpp"

#include "features/feature_configs.h"
#include "signals/cloud_resources.h"

namespace volvo_on_call
{

class CarPositionProvider: public fsm::Transaction
{

 private:

    /**
     * \brief Stores the information if IHU position has been received
     */
    bool received_dead_reckoned_position_ = false;

    /**
     * \brief Stores the information if TCAM position has been received
     */
    bool received_gnss_position_ = false;

    /**
     * \brief Stores the position reported by IHU
     */
    DRVehiclePosition dead_reckoned_position_ = {};

    /**
     * \brief Stores the position reported by TCAM
     */
    GNSSData gnss_position_ = {};

 protected:
    /**
     * \brief Constructs a new CarPositionProvider.
     */
    CarPositionProvider ();


    /**
     * \brief Checks if configuration allows car locator service. For that BasicCarControl feature must
     *        be enabled and the car locator must be allowed in the BasicCarControl configuration
     * \return True if car locator is allowed, false otherwise
     */
    bool IsCarLocatorEnabled();

    /**
     * \brief Checks if location sharing is allowed by the privacy settings
     * \return True if sharing is allowed, false otherwise
     */
    bool IsLocationSharingAllowed();

    /**
     * \brief Sends position requests to IHU and TCAM
     * \return True if requests sent successfully, false otherwise
     */
    bool RequestPosition();

    /**
     * \brief Processes a received position update. When the position is received from the prefered source (IHU)
     *        an immediate attempt is made to send it. If that fails we use the position from TCAM (or wait for its arrival).
     *        If TCAM position arrives first, we store it and wait for the IHU position.
     * \param[in] signal signal for processing.
     * \return True when the complete position information has been received and notification has been sent. False if
     *         the processing is not yet completed and we are waiting for additional position information.
     */
    bool ProcessPositionSignal(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Transaction specific error messsage sending. Can be overloaded by the final transaction. Defaults
     *        to no error messaging.
     * \param[in] error_code error code.
     * \param[in] message error message.
     */
    virtual void SendError(fsm::ResponseInfoData::ResponseCodeError error_code, std::string message = std::string());

    /**
     * \brief Transaction specific handling of sending the IHU position. Must be implemented by the final transaction
     * \param[in] dead_reckoned_position IHU position.
     * \return True when position has been packed into a message and transmitted successfully, false otherwise.
     */
    virtual bool SendDeadReckonedPosition(DRPositionData& dead_reckoned_position) = 0;

    /**
     * \brief Transaction specific handling of sending the TCAM position. Must be implemented by the final transaction
     * \param[in] gnss_position TCAM position.
     * \return True when position has been packed into a message and transmitted successfully, false otherwise.
     */
    virtual bool SendGNSSPosition(GNSSData& gnss_position) = 0;

    /**
     * \brief Resets the transaction
    */
    void ResetTransaction();


private:
    /**
     * \brief Basic Car Control feature config
    */
    BasicCarControlFeatureConfig bcc_feature_;

    /**
     * \brief Basic Car Control cloud resource
    */
    BccFunc002CloudResource      bcc_config_;
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_CAR_POSITION_PROVIDER_H_

/** \}    end of addtogroup */
