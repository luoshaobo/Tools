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
 *  \file     timeout_receiver.cc
 *  \brief    VOC Service timeout receiver class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */


#include "voc_framework/signal_sources/timeout_receiver.h"

#include "voc_framework/signal_sources/timeout_signal_source.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{



TimeoutReceiver::~TimeoutReceiver ()
{
    TimeoutSignalSource& timeout_signal_source = TimeoutSignalSource::GetInstance();

    if (timeout_requested_)
    {
        timeout_signal_source.RemoveExpiredTimeouts();
    }
}


TimeoutTransactionId TimeoutReceiver::RequestTimeout (std::chrono::seconds delay, bool periodic)
{
    TimeoutSignalSource& timeout_signal_source = TimeoutSignalSource::GetInstance();
    TimeoutTransactionId transaction_id;

    try
    {
        transaction_id = timeout_signal_source.RequestTimeout(shared_from_this() , delay, periodic);
    }
    catch (const std::bad_weak_ptr& e)
    {
        DLT_LOG_STRING(dlt_libfsm,
                       DLT_LOG_ERROR,
                       "TimeoutReceiver error, an object implementing timeout"
                       " receiver must be instantiated as a shared pointer");
        std::abort();
    };

    //flag that the interface has been used
    timeout_requested_ = true;

    return transaction_id;
}


void TimeoutReceiver::RequestTimeout (TimeoutTransactionId transaction_id, std::chrono::seconds delay, bool periodic)
{
    TimeoutSignalSource& timeout_signal_source = TimeoutSignalSource::GetInstance();

    try
    {
        timeout_signal_source.RequestTimeout(shared_from_this() ,transaction_id, delay, periodic);
    }
    catch (const std::bad_weak_ptr& e)
    {
        DLT_LOG_STRING(dlt_libfsm,
                       DLT_LOG_ERROR,
                       "TimeoutReceiver error, an object implementing timeout"
                       " receiver must be instantiated as a shared pointer");
        std::abort();
    };

    //flag that the interface has been used
    timeout_requested_ = true;
}

void TimeoutReceiver::RemoveTimeout (TimeoutTransactionId transaction_id)
{
    TimeoutSignalSource& timeout_signal_source = TimeoutSignalSource::GetInstance();
    timeout_signal_source.RemoveTimeout(transaction_id);
}


/**
 * \brief Removes all timeout requests issued by this timer receiver
 */
void TimeoutReceiver::RemoveMyTimeouts ()
{
    if (timeout_requested_)
    {
        TimeoutSignalSource& timeout_signal_source = TimeoutSignalSource::GetInstance();
        timeout_signal_source.RemoveTimeouts(shared_from_this());
    }
}


} // namespace fsm
/** \}    end of addtogroup */
