
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
 *  \file     feature.h
 *  \brief    VOC Service feature base class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_FEATURES_FEATURE_H_
#define VOC_FRAMEWORK_FEATURES_FEATURE_H_

#include "voc_framework/transactions/transaction.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signal_sources/signal_receiver_interface.h"


#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>

namespace fsm
{

class Feature: public SignalReceiverInterface
{

 public:

    /**
     * \brief Provides the feature with a new signal to process
     *  \param[in]  signal - signal for processing.
     */
    virtual void ProcessSignal (std::shared_ptr<Signal> signal) final;

    /**
     * \brief Moves transaction from the list of active transactions to the list
     *          of inactive transactions.
     *  \param[in]  signal - signal for processing.
     *  \return false if the transaction is active and cannot be transfered, true
     *          if the transaction has been transfered
     */
    bool TransactionComplete (std::shared_ptr<Transaction> transaction);

    /**
     * \brief Destructor
     */
    virtual ~Feature();

 protected:

    const std::string& feature_name;

    /**
     * \brief Constructs a new feature.
     * \param[in] name                The name of the feature.
     * \param[in] accept_header       Accept header value to use when fetching resource for the
                                      feature.
     * \param[in] request_params      HTTP REQ params for downloading cloud resource
     */
    Feature (const std::string& name) : feature_name(name) {}

    /**
     * \brief Broadcasts a signal to all active transactions.
     *  \param[in]  signal - signal for broadcasting.
     *  \return true if at least one transaction accepted the signal (see Transaction
     *          for details about limitations), otherwise false
     */
    bool BroadcastToTransactions (std::shared_ptr<Signal> signal);

    /**
     * \brief Contains feature specific signal processing
     *  \param[in]  signal - signal for processing.
     */
    virtual void HandleSignal (std::shared_ptr<Signal> signal) = 0;

    /**
     * \brief Adds a transaction to the list of active transactions. Base class
     *          takes over the ownership of the transaction and will take care of
     *          of deleting it.
     *  \param[in]  transaction - transaction to be added.
     */
    void AddActiveTransaction (std::shared_ptr<Transaction> transaction);

    /**
     * \brief Adds a transaction to the list of active transactions only if
     *        the transaction is not already a part of the list.
     *  \param[in]  transaction - transaction to be added.
     *  \return true if the transaction is added, false it it already is a part of the list
     */
    bool AddActiveTransactionIfUnique (std::shared_ptr<Transaction> transaction);

    /**
     * \brief Removes a transaction from the active transaction list.
     *  \param[in]  transaction - transaction to be removed.
     */
    void RemoveActiveTransaction (std::shared_ptr<Transaction> transaction);

    /**
     * \brief List of active transactions. All handling of active_transactions
     *          must be thread safe, as there can be parallel calls from different
     *          signal source threads. Once a transaction ends up on the list
     *          of active transactions, the feature takes over the ownership
     *          of such feature. It will terminate the transaction when it reports
     *          itself as not active
     */
    std::list<std::shared_ptr<Transaction>> active_transactions;

    /**
     * \brief Mutex guarding access to the active transactions list
     */
    std::mutex active_transactions_mutex;

 private:

    /**
     * \brief List of inactive transactions
     */
    std::list<std::shared_ptr<Transaction>> inactive_transactions;

    /**
     * \brief Mutex guarding access to the inactive transactions list
     */
    std::mutex inactive_transactions_mutex;

    /**
     * \brief Private default constructor to prevent usage.
     */
    Feature () = delete;

    /**
     * \brief Performs a cleanup of transactions. First identifies inactive
     *        transactions that are still on the active_transactions list
     *        and moves them to the inactive transactions list. Then deletes
     *        those transactions from the inactive transaction list that have
     *        already completed.
     */
    void PurgeInactiveTransactions ();
};

} // namespace fsm
/** \}    end of addtogroup */

#endif //VOC_FRAMEWORK_FEATURES_FEATURE_H_
