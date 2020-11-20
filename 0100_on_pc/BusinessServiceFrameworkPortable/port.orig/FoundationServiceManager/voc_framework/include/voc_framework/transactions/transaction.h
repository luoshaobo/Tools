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
 *  \file     transaction.h
 *  \brief    VOC Service transaction base class.
 *  \author   Axel Fagerstedt, Piotr Tomaszewski
 *
 *  \addtogroup SW_package
 *  \{
 */

#ifndef VOC_FRAMEWORK_TRANSACTIONS_TRANSACTION_H_
#define VOC_FRAMEWORK_TRANSACTIONS_TRANSACTION_H_

#include "voc_framework/transactions/event_queue.h"
#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signal_sources/timeout_receiver.h"


#include <condition_variable>
#include <mutex>
#include <deque>
#include <vector>
#include <thread>


namespace fsm
{

class Transaction:  public TimeoutReceiver, public EventQueue<std::shared_ptr<Signal>>
{
 public:


    /**
     * \brief Adds new signal to the transaction queue. The signal is either
     *        enqueued for later processing or immediately rejected. Enqueuing
     *        does not guarantee processing, as the signal my be later rejected by
     *        the consumer thread.
     *  \param[in]  signal - signal for processing.
     *  \return false if immediately rejected, true if enqueued for later processing.
     */
    bool EnqueueSignal (std::shared_ptr<Signal> signal);


    /**
     * \brief Process the timeout signal.
     *  \param[in]  timeout_signal - signal for processing.
     */
    void HandleTimeout (std::shared_ptr<TimeoutSignal> timeout_signal);

 private:
    /** \brief Mutex synchronizing access to the transaction id list */
    std::mutex transaction_id_list_mutex;

    /** \brief Mutex synchronizing access to the session id list */
    std::mutex session_id_list_mutex;

    /** \brief Mutex synchronizing access to the signal type list */
    std::mutex signal_type_list_mutex;

    /**
     * \brief List of all mapped transaction ids.
     */
    std::vector<std::shared_ptr<TransactionId>> mapped_transaction_ids_;

    /**
     * \brief List of all mapped session ids.
     */
    std::vector<std::string> mapped_session_ids_;

    /**
     * \brief List of all mapped signal types.
     */
    std::vector<Signal::SignalType> mapped_signal_types_;

    /**
     * \brief Check if a transaction id is part of the
     *        list of transaction ids mapped to this transaction. This operation is meant for internal use,
     *        it contains only the search logic and does not perform any access control. Mutex should
     *        normally be aquired prior to using this method
     * \param[in] transaction_id The id the check.
     * \return True if part of the list, false otherwise.
     */
    bool IsTransactionMappedUnguarded(const TransactionId& transaction_id);

    /**
     * \brief Check if a session id is part of the
     *        list of session ids mapped to this transaction. This operation is meant for internal use,
     *        it contains only the search logic and does not perform any access control. Mutex should
     *        normally be aquired prior to using this method
     * \param[in] session_id The id the check.
     * \return True if part of the list, false otherwise.
     */
    bool IsSessionMappedUnguarded(std::string session_id);

    /**
     * \brief Check if a signal type is part of the
     *        list of singal types mapped to this transaction. This operation is meant for internal use,
     *        it contains only the search logic and does not perform any access control. Mutex should
     *        normally be aquired prior to using this method
     * \param[in] signal_type The type the check.
     * \return True if part of the list, false otherwise.
     */
    bool IsSignalTypeMappedUnguarded(Signal::SignalType signal_type);


 protected:

    /**
     * \brief Constructor is hidden, as only subclasses should be instatiated.
     *        Subclasses must make sure to execute this constructor as well.
     */
    Transaction () : EventQueue() {};

    /**
     * \brief Decides is a signal is relevant for the transaction, this method
     *          must be overriden in the subclasses.
     *  \param[in]  signal Signal for processing.
     *  \return True if signal is relevant and should be enqueued, false otherwise.
     */
    virtual bool WantsSignal (std::shared_ptr<Signal> signal) = 0;

    /**
    * \brief Instantiation of event handling logic, defines abstract method from event queue.
    *        Internally it will call HandleSignal.
    * \param[in]  signal Signal for processing.
    * \return true if further signals are expected, false when no more signals
    *         are expected and the transaction can be terminated
    */
    bool HandleEvent (std::shared_ptr<Signal> signal) final;

    /**
    * \brief Signal handling logic, this method must be overriden in the subclasses.
    * \param[in]  signal Signal for processing.
    * \return true if further signals are expected, false when no more signals
    *         are expected and the transaction can be terminated
    */
    virtual bool HandleSignal (std::shared_ptr<Signal> signal) = 0;

    /**
     * \brief Add a transaction id into the internal list
     *        of transactions ids mapped to this transaction.
     * \param[in] transaction_id The id to add.
     * \return None.
     */
    void MapTransactionId (std::shared_ptr<TransactionId> transaction_id);

    /**
     * \brief Check if a transaction id is part of the
     *        list of transaction ids mapped to this feature.
     * \param[in] transaction_id The id the check.
     * \return True if part of the list, false otherwise.
     */
    bool IsMappedTransactionId (const TransactionId& transaction_id);

    /**
     * \brief Cleans the list of mapped transaction ids.
     */
    void RemoveMappedTransactions ();

    /**
     * \brief Add a session id into the internal list
     *        of session ids mapped to this transaction.
     * \param[in] session_id The id to add.
     * \return None.
     */
    void MapSessionId (std::string session_id);

    /**
     * \brief Check if a session id is part of the
     *        list of session ids mapped to this feature.
     * \param[in] session_id The id the check.
     * \return True if part of the list, false otherwise.
     */
    bool IsMappedSessionId (const std::string session_id);

    /**
     * \brief Cleans the list of mapped session ids.
     */
    void RemoveMappedSessions ();

    /**
     * \brief Add a signal type into the internal list
     *        of signal types mapped to this transaction.
     * \param[in] signal_type The type to add.
     * \return None.
     */
    void MapSignalType (Signal::SignalType signal_type);

    /**
     * \brief Check if a signal type is part of the
     *        list of signal types mapped to this feature.
     * \param[in] signal_type The type the check.
     * \return True if part of the list, false otherwise.
     */
    bool IsMappedSignalType (Signal::SignalType signal_type);

    /**
     * \brief Cleans the list of mapped signal types.
     */
    void RemoveMappedSignalTypes ();
};

} // namespace fsm

#endif //VOC_FRAMEWORK_TRANSACTIONS_TRQANSACTION_H_

/** \}    end of addtogroup */
