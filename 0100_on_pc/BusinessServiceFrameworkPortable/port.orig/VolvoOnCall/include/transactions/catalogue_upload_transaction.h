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
 *  \file     catalogue_upload_transaction.h
 *  \brief    VOC Service catalogue upload transaction.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_CATALOGUE_UPLOAD_TRANSACTION_H_
#define VOC_TRANSACTIONS_CATALOGUE_UPLOAD_TRANSACTION_H_

#include "voc_framework/signals/signal.h"
#include "voc_framework/transactions/transaction.h"

namespace volvo_on_call
{

class CatalogueUploadTransaction: public fsm::Transaction
{
 public:

    /**
     * \brief Constructs a new CatalogueUploadTransaction.
     */
    CatalogueUploadTransaction ();

    /**
     * \brief Checks if transaction wants a given signal.
     * \param[in] signal fsm::Signal to check.
     * \return True if:
     *         fsm::Signal is a Catalogue signal and transaction is still in state #kNew.
     *         Singal is a CA-CAT-002 and its transaction id is mapped
     */
    bool WantsSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handle a signal.
     *        If signal is a Catalogue signal and transaction is still in state #kNew
     *        then the upload of certificates to the catalogue will be performed.
     *        If signal is CA-CAT-002 and the state is kCertificatesUpload then the status
     *        will be logged.
     * \param[in] signal fsm::Signal to handle.
     * \return True if transaction is not yet finished, false if it is.
     */
    bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

 private:

    /**
     * \brief Possible states of the transaction
     */
    enum State
    {
        kNew, ///< Created but no signals have been processed-
        kCertificatesUpload, ///< Certificates are uploaded
        kDone, ///< Done, no new signals should be handled.
    };

    /**
     * \brief Tracks the current state.
     */
    State state_ = kNew;


    /**
     * Transaction id of the upload request. Used to match the response
     */
    fsm::CCMTransactionId upload_transaction_id_;

    /**
     * \brief Uploads certificates to the catalogue.
     *
     * \return True if upload suucceeded, false if it failed.
     */
    bool UploadCertificates();

};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_CATALOGUE_UPLOAD_TRANSACTION_H_

/** \}    end of addtogroup */
