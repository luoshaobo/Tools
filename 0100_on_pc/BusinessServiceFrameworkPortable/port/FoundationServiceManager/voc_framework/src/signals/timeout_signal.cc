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
 *  \file     timeout_signal.cc
 *  \brief    VOC Service timeout signal class.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/timeout_signal.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

TimeoutSignal::TimeoutSignal (TimeoutTransactionId& timeout_id)  :
     Signal::Signal(timeout_id, Signal::kTimeout)
{
}

/**
 * \brief String identifier for printouts
 */
std::string TimeoutSignal::ToString()
{
    return "TimeoutSignal";
};

} // namespace fsm
/** \}    end of addtogroup */
