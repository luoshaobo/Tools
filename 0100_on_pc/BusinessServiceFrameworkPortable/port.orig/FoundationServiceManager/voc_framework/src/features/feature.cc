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
 *  \file     feature.cc
 *  \brief    VOC Service feature base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/features/feature.h"
#include "voc_framework/signals/signal_factory.h"

#include <algorithm>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

bool Feature::TransactionComplete(std::shared_ptr<Transaction> transaction)
{
    if ( !transaction->IsActive() )
    {
       std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
       std::lock_guard<std::mutex> lock_inactive(inactive_transactions_mutex);
       active_transactions.remove(transaction);
       inactive_transactions.push_front(transaction);
       return true;
    }
    else
    {
        return false;
    }
}

void Feature::AddActiveTransaction(std::shared_ptr<Transaction> transaction)
{
    std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
    active_transactions.push_front(transaction);
}

bool Feature::AddActiveTransactionIfUnique(std::shared_ptr<Transaction> transaction)
{
    std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
    bool found = (std::find(active_transactions.begin(), active_transactions.end(), transaction) != active_transactions.end());
    if (!found)
    {
        active_transactions.push_front(transaction);
        return true;
    }
    else
    {
        return false;
    }
}

void Feature::RemoveActiveTransaction(std::shared_ptr<Transaction> transaction)
{
    std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
    active_transactions.remove(transaction);
}

/*****************************************************************************************
*  TODO: currently the handling of the inactive transactions happens in the context
*        of HandleSignal. If this proves to be a performance issue, consider the following
*        1) Instead of parsing the list of active transactions in search for inactive
*           ones the transactions can call TransactionComplete
*        2) Move out the cleanup to a separate tread so that it does not block HandleSignal
******************************************************************************************/
void Feature::PurgeInactiveTransactions()
{
    //first identify the inactive transaction and transfer them
    //from the active to inactive transaction list
    {
        std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
        active_transactions.erase(
            std::remove_if(
                active_transactions.begin(),
                active_transactions.end(),
                [&] (std::shared_ptr<Transaction> transaction) {
                    if (!transaction->IsActive())
                    {
                        std::lock_guard<std::mutex> lock_inactive(inactive_transactions_mutex);
                        inactive_transactions.push_front(transaction);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }),
            active_transactions.end());
    }

    //handle the inactive transactions. Delete transactions that already have
    //completed execution
    {
        std::lock_guard<std::mutex> lock_inactive(inactive_transactions_mutex);
        inactive_transactions.erase(
            std::remove_if(
                inactive_transactions.begin(),
                inactive_transactions.end(),
                [&] (std::shared_ptr<Transaction> transaction) {
                    if (transaction->HasCompleted())
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }),
            inactive_transactions.end() );
    }
}

bool Feature::BroadcastToTransactions(std::shared_ptr<Signal> signal)
{
    bool accepted = false;
    std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
    for (auto transaction:active_transactions)
    {
        if (transaction->EnqueueSignal(signal))
        {
            accepted = true;
        }
    }
    return accepted;
}

Feature::~Feature ()
{
    //first handle transactions from the inactive transactions list
    PurgeInactiveTransactions();
    {
       //clean up remaining transactions in the active transaction list
       std::lock_guard<std::mutex> lock_active(active_transactions_mutex);
       active_transactions.clear();
    }
}

void Feature::ProcessSignal(std::shared_ptr<Signal> signal)
{
    //Handle incomming signal (feature specific implementation)
    HandleSignal(signal);

    //do the cleaning of the inactive transactions
    PurgeInactiveTransactions();
}

} // namespace fsm
/** \}    end of addtogroup */
