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
 *  \file     privacy_notification_transaction.h
 *  \brief    VOC Service privacy notification transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_PRIVACY_NOTIFICATION_TRANSACTION_H_
#define VOC_TRANSACTIONS_PRIVACY_NOTIFICATION_TRANSACTION_H_

#include "voc_framework/transactions/transaction.h"

namespace volvo_on_call
{

class PrivacyNotificationTransaction: public fsm::Transaction
{
 public:

    /**
     * \brief Creates a privacy notification transaction.
     */
    PrivacyNotificationTransaction () : fsm::Transaction() {}

    /**
     * \brief Check if the transaction wants to handle a given signal.
     *        Only cares about signal type as this is a "one shot"
     *        transaction, after serving the initial request it will
     *        terminate.
     * \param[in] signal The signal to check for.
     * \return True if signal was of type kPrivacyNotificationSignal
     *         False otherwise.
     */
    bool WantsSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handles a signal which is expected to be such that
     *        #WantsSignal would have returned true for it.
     *        Will store the privacy data persistantly,
     *        #GetPrivacySettings can be called by clients to retrive
     *        the last received settings.
     *        This is a "one shot" transaction, after serving the
     *        relevant signal it will terminate.
     * \param[in] signal The signal to handle.
     * \return False unless signal was not wanted, in which case the
     *         true is returned to keep the transaction alive.
     */
    bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Retrieve the currently applicable privacy settings.
     * \param[out] car_stats_upload Will be set to true if car
     *                              stats upload is allowed.
     *                              False if it is not.
     * \param[out] location_services Will be set to true if location
     *                               services is allowed. False if
     *                               it is not.
     * \return True if settings were succesfully retrieved
     *         false otherwise, in which case both out parameters
     *         will be set to false.
     */
    static bool GetPrivacySettings(bool& car_stats_upload, bool& location_services);

 private:

    /**
     * Name of the privacy settings key in persistance.
     */
    static const std::string kPrivacySettingsKey;
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_PRIVACY_NOTIFICATION_TRANSACTION_H_

/** \}    end of addtogroup */
