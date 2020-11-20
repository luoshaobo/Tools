
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
 *  \file     admin_confirmation_transaction.h
 *  \brief    Device pairing admin confirmation transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_ADMIN_CONFIRMATION_TRANSACTION_H_
#define VOC_TRANSACTIONS_ADMIN_CONFIRMATION_TRANSACTION_H_

#include "voc_framework/transactions/smart_transaction.h"

namespace volvo_on_call
{

class AdminConfirmationTransaction: public fsm::SmartTransaction
{
 public:

    /**
     * \brief Creates a "confirm device pairing with admin" transaction.
     * \param[in] initial_signal fsm::Signal which caused transaction to be created.
     */
    AdminConfirmationTransaction(std::shared_ptr<fsm::Signal> initial_signal);

 private:

    /**
     * \brief Enumerates the states this transaction can be in.
     */
    enum States
    {
        kNew = 0, ///< Newly created.
        kAwaitingResponse, ///< Request sent, awaiting response.
        kDone ///< Finished
    };

    /**
     * \brief Handle DevicePairingConfirmationByAdminRequest signal.
     *        Expected in state kNew.
     *        Will send out a request for confirmation to the specified admin user
     *        and set state to kAwaitingResponse.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleDevicePairingConfirmationByAdminRequest(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle DevicePairingConfirmationByAdminDeviceResponse signal.
     *        Expected in state kAwaitingResponse.
     *        Will send DevicePairingByAdminNotify to IHU and set state to kDone.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleDevicePairingConfirmationByAdminDeviceResponse(std::shared_ptr<fsm::Signal> signal);
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_ADMIN_CONFIRMATION_TRANSACTION_H_

/** \}    end of addtogroup */
