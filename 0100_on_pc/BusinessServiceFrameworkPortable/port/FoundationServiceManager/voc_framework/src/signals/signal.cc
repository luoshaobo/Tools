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
 *  \file     signal.cc
 *  \brief    VOC Service signal base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/signal.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

Signal::Signal (const TransactionId& transaction_id, const SignalType signal_type)  :
    my_signal_type(signal_type)
{
    transaction_id_ = transaction_id.GetSharedCopy();
}

const TransactionId& Signal::GetTransactionId ()
{
    return *transaction_id_;
}


} // namespace fsm
/** \}    end of addtogroup */
