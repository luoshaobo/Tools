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
 *  \file     key_confirmation_transaction.h
 *  \brief    Device pairing key confirmation transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_KEY_CONFIRMATION_TRANSACTION_H_
#define VOC_TRANSACTIONS_KEY_CONFIRMATION_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"

namespace volvo_on_call
{

class KeyConfirmationTransaction: public fsm::SmartTransaction
{
 public:

    /**
     * \brief Creates a "confirm device pairing with physical keys" transaction.
     * \param[in] initial_signal fsm::Signal which caused transaction to be created.
     */
    KeyConfirmationTransaction(std::shared_ptr<fsm::Signal> initial_signal);

 private:

    /**
     * \brief Enumerates the states this transaction can be in.
     */
    enum States
    {
        kNew = 0, ///< Newly created.
        kSearchingForKeys, ///< Initiated key search, awaiting notifications.
        kDone ///< Finished
    };

    /**
     * \brief Handle NumberOfKeysInsideVehicleRequest signal.
     *        Expected in states kNew, kSearchingForKeys.
     *        If in state kNew will start key search, timer
     *        and then go into state kSearchingForKeys.
     *        If in state kSearchingForKeys will verify signal
     *        is a request to stop search, stop the search and
     *        go to state kDone.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleNumberOfKeysInsideVehicleRequest(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle KeysFoundNotification. Expected in state kSearchingForKeys.
     *        Will update IHU through NumberOfKeysInsideVehicleNotify
     *        and if enough keys found will stop search.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleKeysFoundNotification(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle timeout. Expected in state kSearchingForKeys.
     *        Will update IHU through NumberOfKeysInsideVehicleNotify
     *        and stop search,
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction si not yet finished, false if it is.
     */
    bool HandleTimeout(std::shared_ptr<fsm::Signal> signal);
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_KEY_CONFIRMATION_TRANSACTION_H_

/** \}    end of addtogroup */
