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
 *  \file     transaction.cc
 *  \brief    VOC Service transaction base class.
 *  \author   Axel Fagerstedt, Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signal_sources/timeout_signal_source.h"
#include "voc_framework/transactions/transaction.h"

#include <algorithm>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{


void Transaction::HandleTimeout (std::shared_ptr<TimeoutSignal> timeout_signal)
{
    //If timeout expected, enqueue it in front of the queue
    if ( WantsSignal(timeout_signal) )
    {
       EnqueueEventAsFirst (timeout_signal);
    }
}

bool Transaction::HandleEvent (std::shared_ptr<Signal> signal)
{
    //Call handle signal to do transaction specific signal handling
    bool return_value = HandleSignal (signal);

    //if transaction is shutting down, remove timeouts
    if (!return_value)
    {
        RemoveMyTimeouts();
    }

    return return_value;
}

bool Transaction::EnqueueSignal (std::shared_ptr<Signal> signal)
{
    //signal should be enqueued only if the transaction is
    //active and the signal is relevant for it
    if ( WantsSignal(signal) )
    {
        return EnqueueEvent(signal);
    }
    return false;
}

void Transaction::MapTransactionId (std::shared_ptr<TransactionId> transaction_id)
{
    std::lock_guard<std::mutex> lock(transaction_id_list_mutex);
    if (!Transaction::IsTransactionMappedUnguarded(*transaction_id))
    {
        mapped_transaction_ids_.push_back(transaction_id);
    }
}

void Transaction::MapSessionId (std::string session_id)
{
    std::lock_guard<std::mutex> lock(session_id_list_mutex);
    if (!Transaction::IsSessionMappedUnguarded(session_id))
    {
        mapped_session_ids_.push_back(session_id);
    }
}

void Transaction::MapSignalType (Signal::SignalType signal_type)
{
    std::lock_guard<std::mutex> lock(signal_type_list_mutex);
    if (!Transaction::IsSignalTypeMappedUnguarded(signal_type))
    {
        mapped_signal_types_.push_back(signal_type);
    }
}

bool Transaction::IsTransactionMappedUnguarded(const TransactionId& transaction_id)
{
    // Try to find a matching TransactionId in the list of mapped ids.
    // As we want to compare by value we use std::find_if instead of
    // std::find and compare the dereferenced pointer rather than the
    // pointer itself.
    // If find_id returns the end iterator it did not find a match.
    return std::find_if(mapped_transaction_ids_.begin(),
                        mapped_transaction_ids_.end(),
                        [&](std::shared_ptr<TransactionId> const& mapped_id)
                        {
                            return *mapped_id == transaction_id;
                        }) != mapped_transaction_ids_.end();
}

bool Transaction::IsSessionMappedUnguarded(const std::string session_id)
{
    // Try to find a matching SessionId in the list of mapped ids.
    return std::find(mapped_session_ids_.begin(),
                     mapped_session_ids_.end(),
                     session_id) != mapped_session_ids_.end();
}

bool Transaction::IsSignalTypeMappedUnguarded(Signal::SignalType signal_type)
{
    // Try to find a matching SessionId in the list of mapped ids.
    return std::find(mapped_signal_types_.begin(),
                     mapped_signal_types_.end(),
                     signal_type) != mapped_signal_types_.end();
}

bool Transaction::IsMappedTransactionId (const TransactionId& transaction_id)
{
    std::lock_guard<std::mutex> lock(transaction_id_list_mutex);
    return IsTransactionMappedUnguarded(transaction_id);
}


bool Transaction::IsMappedSessionId (const std::string session_id)
{
    std::lock_guard<std::mutex> lock(session_id_list_mutex);
    return IsSessionMappedUnguarded(session_id);
}

bool Transaction::IsMappedSignalType (const Signal::SignalType signal_type)
{
    std::lock_guard<std::mutex> lock(signal_type_list_mutex);
    return IsSignalTypeMappedUnguarded(signal_type);
}

void Transaction::RemoveMappedTransactions ()
{
    std::lock_guard<std::mutex> lock(transaction_id_list_mutex);
    mapped_transaction_ids_.clear();
}

void Transaction::RemoveMappedSessions ()
{
    std::lock_guard<std::mutex> lock(session_id_list_mutex);
    mapped_session_ids_.clear();
}

void Transaction::RemoveMappedSignalTypes ()
{
    std::lock_guard<std::mutex> lock(signal_type_list_mutex);
    mapped_signal_types_.clear();
}


} // namespace fsm
/** \}    end of addtogroup */
