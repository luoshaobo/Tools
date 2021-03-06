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
 *  \file     car_locator_position_update_transaction.h
 *  \brief    VOC Service car locator position update transaction.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_CAR_LOCATOR_POSITION_UPDATE_TRANSACTION_H_
#define VOC_TRANSACTIONS_CAR_LOCATOR_POSITION_UPDATE_TRANSACTION_H_

#include "voc_framework/signals/ccm.h"
#ifndef VOC_TESTS
  #include "voc_framework/signals/vehicle_comm_signal.h"
#endif

#include "transactions/car_position_provider.h"

#include "ipcb_IGNSSService.hpp"


namespace volvo_on_call
{

class CarLocatorPositionUpdateTransaction: public CarPositionProvider
{
 public:

    /**
     * \brief Constructs a new CarLocatorPositionUpdateTransaction.
     */
     CarLocatorPositionUpdateTransaction ();

    /**
     * \brief Checks if transaction wants a given signal.
     * \param[in] signal fsm::Signal to check.
     * \return True if:
     *         fsm::Signal is a position update request (end of trip or internal signal position update signal)
     *         fsm::Signal is a DeadReckonedPosition or GNSSPositionData and its transaction id is mapped.
     */
    bool WantsSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle a signal.
     *        Any of position request signals will trigger a request to retrieve the current postion.
     *        The order of preference for position information is:
     *        IHU position (map matched or dead reckoned) is primary choice, if this information
     *        is not available, a position from the location manager is used. When position
     *        is established the notification (BCC-CL-020) is sent.
     * \param[in] signal fsm::Signal to handle.
     * \return Always true as this is a long lived transaction.
     */
    bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

 private:

    /**
     * \brief Sequence number holder
     */
     uint16_t sequence_number_ = 1;

     /**
      * Transaction id used when responding, autogenerated.
      */
     fsm::CCMTransactionId request_transaction_id_;

    /**
     * \brief Possible states of the transaction
     */
    enum State
    {
        kWaitForTrigger, ///< idle, awaiting vehicle comm signal
        kRequestForPositon,///< awaiting position information from the Ip Command broker.
    };

    /**
     * \brief Tracks the current state.
     */
    State state_ = kWaitForTrigger;

    /**
     * \brief List of users the position will be sent to.
     */
    std::vector<fsm::UserId> recipients_;

    /**
     * \brief Handles position update request.
     * \param[in] signal fsm::Signal to handle.
     */
    void HandlePositionUpdateRequest(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handles position information from the Ip Command broker and sends the response signal (BCC-CL-020).
     *        The order of preference for position information source is taken into account when determining
     *        the postion. Preferably the IHU originating position is used, if that is not available then the TCAM
     *        originating position is sent. If the position has not beed retrieved successfully from any of the sources
     *        then the error code is sent.
     * \param[in] signal fsm::Signal to handle.
     */
    void HandlePosition(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Sends BCC-CL-020 with position retrieved from IHU
     * \param[in] dead_reckoned_position position information.
     * \return True message could be sent, false otherwise
     */
    bool SendDeadReckonedPosition(DRPositionData& dead_reckoned_position);

    /**
     * \brief Sends BCC-CL-020 with position retrieved from TCAM
     * \param[in] gnss_position position information.
     * \return True message could be sent, false otherwise
     */
    bool SendGNSSPosition(GNSSData& gnss_position);

    /**
     * \brief Resets the transaction to a state in which it awaits the next position update request.
    */
    void ResetTransaction();

    /**
     * \brief Returns the next, consequtive, sequence number
     * \return Next sequence number
    */
    uint16_t GetSequenceNumber();
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_CAR_LOCATOR_POSITION_UPDATE_TRANSACTION_H_

/** \}    end of addtogroup */
