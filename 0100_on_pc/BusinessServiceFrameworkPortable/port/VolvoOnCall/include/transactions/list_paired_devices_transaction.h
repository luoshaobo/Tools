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
 *  \file     list_paired_devices_transaction.h
 *  \brief    Device pairing list paired devices transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_TRANSACTIONS_LIST_PAIRED_DEVICES_TRQANSACTION_H_
#define VOC_TRANSACTIONS_LIST_PAIRED_DEVICES_TRQANSACTION_H_

#include "voc_framework/transactions/transaction.h"

namespace volvo_on_call
{

class ListPairedDevicesTransaction: public fsm::Transaction
{
 public:

    /**
     * \brief Creates a "list paired devices" transaction.
     */
    ListPairedDevicesTransaction () : fsm::Transaction() {}

    /**
     * \brief Check if the transaction wants to handle a given signal.
     *        Only cares about signal type as this is a "one shot"
     *        transaction, after serving the initial request it will
     *        terminate.
     * \param[in] signal The signal to check for.
     * \return True if signal was of type kListPairedDevicesRequest
     *         False otherwise.
     */
    bool WantsSignal (std::shared_ptr<fsm::Signal> signal);

    /**
     * \brief Handles a signal which is expected to be such that
     *        #WantsSignal would have returned true for it.
     *        Will get the list of paired users and return it.
     *        This is a "one shot" transaction, after serving the
     *        relevant signal it will terminate.
     * \param[in] signal The signal to handle.
     * \return False unless signal was not wanted, in which case the
     *         true is returned to keep the transaction alive.
     */
    bool HandleSignal (std::shared_ptr<fsm::Signal> signal);
};

} // namespace volvo_on_call

#endif //VOC_TRANSACTIONS_LIST_PAIRED_DEVICES_TRQANSACTION_H_

/** \}    end of addtogroup */
