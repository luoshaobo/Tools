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
 *  \file     role_selected_notification_transaction.h
 *  \brief    Device pairing role selected transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_ROLE_SELECTED_NOTIFICATION_TRANSACTION_H_
#define VOC_TRANSACTIONS_ROLE_SELECTED_NOTIFICATION_TRANSACTION_H_


#include "voc_framework/transactions/smart_transaction.h"

#include "keystore.h"
#include "usermanager_types.h"

#include <openssl/x509.h>


namespace volvo_on_call
{

class RoleSelectedNotificationTransaction: public fsm::SmartTransaction
{
 public:

    /**
     * \brief Creates a "device pairing role selected" transaction.
     * \param[in] initial_signal fsm::Signal which caused transaction to be created.
     */
    RoleSelectedNotificationTransaction (std::shared_ptr<fsm::Signal> initial_signal);

    /**
     * \brief Get the selected role.
     *
     * \return The selected role or kUndefined if not yet set.
     */
    static fsm::UserRole GetSelectedRole();

 private:

    /**
     * \brief Enumerates the states this transaction can be in.
     */
    enum States
    {
        kNew = 0, ///< Newly created.
        kAwaitingCsr, ///< Awaiting a CSR from cloud.
        kAwaitingCatalogueResponse, ///< Awaiting confirmation of certificate upload to cataloue.
        kDone ///< Finished
    };

    /**
     * \brief Tracks the selected role.
     */
    static fsm::UserRole role_;

    /**
     * \brief Instance of Keystore class to avoid creating new instances
     *        in each function needing access to Keystore.
     */
    fsm::Keystore keystore_;

    /**
     * \brief Handle RoleSelectedNotify signal. Will create vehicle
     *        CA, Actor and triger cloud delegate creation if not yet done.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleRoleSelectedNotify(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle CA-DEL-002. Will retry the CSR request.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCaDel002(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle CA-DEL-010, will sign the CSR request, send back
     *        and upload to catalogue.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCaDel010(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle CA-CAT-002, will retry on failure.
     *
     * \param[in] signal The signal.
     *
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleCaCat002(std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Generate a new Ecliptic Curve key.
     *
     * \return Pointer to generated key (caller must free)
     *         or NULL in case of an error.
     */
    EVP_PKEY * GenerateECKey();

    /**
     * \brief Generate verison 4 UUID
     *
     * \return String containing generated UUID in format XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
     *          or empty string in case of error.
     */
    std::string GenerateUUID();

    /**
     * \brief Generate the vehicle CA root key and certificate and store them in Keystore.
     *
     * \return True in case of success false in case of error.
     */
    bool GenerateVehicleCA();

    /**
     * \brief Generate vehicle end point (not CA, e.g. actor, tls) key and certificate and store them in Keystore.
     *
     * \param[in] role Some certificate fields depend on role.
     *
     * \return True in case of success false in case of error.
     */
    bool GenerateVehicleEndPointCertificate(fsm::Keystore::CertificateRole role);

};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_ROLE_SELECTED_NOTIFICATION_TRANSACTION_H_

/** \}    end of addtogroup */
