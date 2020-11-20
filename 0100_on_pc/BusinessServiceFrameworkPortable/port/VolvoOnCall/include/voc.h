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
 *  \file     voc.h
 *  \brief    VOC common types, constants and utility functions.
 *  \author   Axel Fagerstedt, et al.
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_VOC_H_
#define VOC_VOC_H_

#include <openssl/x509.h>

#include "voc_framework/transactions/transaction_id.h"
#include "voc_utils.h"

namespace volvo_on_call
{

// Common types and constants used throughout Voc
// Utility functions could also be placed here.

//TODO: workaround in use when missing headunit
/**
 * \brief for test predefined VC transaction id
 */
extern fsm::VehicleCommTransactionId default_ihu_transaction_id;

} // namespace volvo_on_call

#endif //VOC_VOC_H_

/** \}    end of addtogroup */
